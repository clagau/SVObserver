//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file WebsocketServerConnection.cpp
/// All Rights Reserved
//******************************************************************************
/// Class for handling a single client connection.
///
/// You can send multiple messages without waiting for the previous to finish
/// writing to the wire, as this class takes care of queueing.
///
/// The class also takes care of sending ping frames to the clint and
/// disconnecting unresponsive clients.
//******************************************************************************

#include "stdafx.h"

#include <iostream>

#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>

#include "SVHttpLibrary/Handshake.h"
#include "SVHttpLibrary/WebsocketServerConnection.h"

namespace SVHTTP
{
WebsocketServerConnection::WebsocketServerConnection(const WebsocketServerSettings& settings,
	boost::asio::io_service& io_service,
	int connection_id,
	EventHandler* event_handler)
	: m_connection_id(connection_id), m_settings(settings), m_io_service(io_service), m_socket(io_service),
	m_ping_timer(io_service), m_ping_timeout_count(0), m_handshake(), m_event_handler(event_handler)
{
}
bool WebsocketServerConnection::isOpen() const
{
	return m_socket.is_open();
}
boost::asio::ip::tcp::socket& WebsocketServerConnection::socket()
{
	return m_socket;
}
void WebsocketServerConnection::start()
{
	read_handshake();
}
void WebsocketServerConnection::read_handshake()
{
	m_buf.assign(m_settings.read_buffer_size, '\0');
	boost::asio::async_read(
		m_socket,
		boost::asio::buffer(m_buf),
		boost::asio::transfer_at_least(1),
		std::bind(
		&WebsocketServerConnection::handle_handshake_read, this, std::placeholders::_1, std::placeholders::_2));
}
void WebsocketServerConnection::handle_handshake_read(const boost::system::error_code& error, size_t bytes_read)
{
	if (error)
	{
		handle_error(error);
		return;
	}

	auto state = m_handshake.parseClientRequest(m_buf.data(), bytes_read);
	switch (state)
	{
		case Handshake::PS_INCOMPLETE:
			read_handshake();
			break;

		case Handshake::PS_SUCCESS:
			send_handshake_response();
			break;

		case Handshake::PS_ERROR:
		case Handshake::PS_INVALID:
			BOOST_LOG_TRIVIAL(error) << "Received unknown handshake request from client.";
			m_socket.close();
			break;
	}
}
void WebsocketServerConnection::send_handshake_response()
{
	auto response = m_handshake.generateServerResponse();
	m_buf = std::vector<char> {response.begin(), response.end()};
	boost::asio::async_write(
		m_socket,
		boost::asio::buffer(m_buf),
		std::bind(
		&WebsocketServerConnection::handle_handshake_sent, this, std::placeholders::_1, std::placeholders::_2));
}
void WebsocketServerConnection::handle_handshake_sent(const boost::system::error_code& error, size_t bytes_read)
{
	if (error)
	{
		handle_error(error);
		return;
	}

	m_event_handler->onConnect(m_connection_id, *this);

	read_buffer();
	schedule_ping();
}
void WebsocketServerConnection::read_buffer()
{
	m_buf.resize(m_settings.read_buffer_size);
	boost::asio::async_read(m_socket,
		boost::asio::buffer(m_buf),
		boost::asio::transfer_at_least(1),
		std::bind(&WebsocketServerConnection::handle_read_buffer,
		this,
		std::placeholders::_1,
		std::placeholders::_2,
		std::ref(m_buf)));
}
void WebsocketServerConnection::handle_read_buffer(const boost::system::error_code& error,
	size_t bytes_read,
	std::vector<char>& buf)
{
	if (error)
	{
		handle_error(error);
		if (m_socket.is_open())
		{
			m_socket.close();
		}
		return;
	}

	auto prev_size = m_frames.size();
	m_frames.reserve(prev_size + bytes_read + 1);
	m_frames.insert(m_frames.end(), buf.data(), buf.data() + bytes_read);

	while (process_frame(m_frames, m_payload))
	{
		;
	}

	if (m_socket.is_open())
	{
		read_buffer();
	}
}
bool WebsocketServerConnection::process_frame(std::vector<char>& frames, std::vector<char>& payload)
{
	size_t frame_len = 0;
	auto frame_type = m_websocket_parser.parseFrame(
		reinterpret_cast<unsigned char*>(frames.data()), frames.size(), frame_len, payload);

	switch (frame_type)
	{
		case WebSocketParser::ERROR_FRAME:
			BOOST_LOG_TRIVIAL(error) << "Error while parsing incoming websocket message! Closing connection.";
			m_socket.close();
			return false;

		case WebSocketParser::CLOSING_FRAME:
			m_socket.close();
			return false;

		case WebSocketParser::TEXT_FRAME:
			m_event_handler->onTextMessage(m_connection_id, payload);
			payload.clear();
			frames.erase(frames.begin(), frames.begin() + frame_len);
			return true;

		case WebSocketParser::BINARY_FRAME:
			m_event_handler->onBinaryMessage(m_connection_id, payload);
			payload.clear();
			frames.erase(frames.begin(), frames.begin() + frame_len);
			return true;

		case WebSocketParser::PING_FRAME:
			BOOST_LOG_TRIVIAL(warning) << "server should not receive ping message!";
			frames.erase(frames.begin(), frames.begin() + frame_len);
			return true;

		case WebSocketParser::PONG_FRAME:
			m_ping_timeout_count = 0;
			// TODO: really clear frames? according to RFC, we must be able to handle
			//       control frames in the middle of a fragmented message.
			frames.erase(frames.begin(), frames.begin() + frame_len);
			return true;

		case WebSocketParser::CONTINUATION_FRAME:
			frames.erase(frames.begin(), frames.begin() + frame_len);
			return true;

		case WebSocketParser::INCOMPLETE_FRAME:
			return false;

		default:
			BOOST_LOG_TRIVIAL(error) << "Received unknown WebSocket frame " << frame_type;
			m_socket.close();
			return false;
	}
}
std::future<void> WebsocketServerConnection::sendTextMessage(const std::vector<char>& buf)
{
	return send_message_impl(buf, WebSocketParser::TEXT_FRAME);
}
std::future<void> WebsocketServerConnection::sendBinaryMessage(const std::vector<char>& buf)
{
	return send_message_impl(buf, WebSocketParser::BINARY_FRAME);
}
std::future<void> WebsocketServerConnection::send_message_impl(const std::vector<char>& buf,
	WebSocketParser::FrameType type)
{
	auto promise = std::make_shared<std::promise<void>>();
	if (m_settings.split_large_payload)
	{
		std::vector<std::vector<char>> frames;
		m_websocket_parser.makeFrames(frames, type, buf.data(), buf.size());
		m_io_service.dispatch([this, frames = std::move(frames), promise]() {
			for (size_t i = 0; i < frames.size(); ++i)
			{
				PendingFrame pendingFrame;
				pendingFrame.frame = std::move(frames[i]);
				// fullfill promise after last write only
				if (i + 1 == frames.size())
				{
					pendingFrame.promise = promise;
				}
				m_frame_queue.emplace(std::move(pendingFrame));
			}
			send_next_frame();
		});
	}
	else
	{
		std::vector<char> frame;
		m_websocket_parser.makeFrame(frame, WebSocketParser::BINARY_FRAME, buf.data(), buf.size());
		m_io_service.dispatch([this, frame = std::move(frame), promise]() {
			PendingFrame pendingFrame;
			pendingFrame.frame = std::move(frame);
			pendingFrame.promise = promise;
			m_frame_queue.emplace(std::move(pendingFrame));
			send_next_frame();
		});
	}
	return promise->get_future();
}
void WebsocketServerConnection::send_next_frame()
{
	if (m_frame_queue.empty() || m_sending_frame)
	{
		return;
	}

	m_sending_frame = true;
	auto& pendingFrame = m_frame_queue.front();
	boost::asio::async_write(
		m_socket,
		boost::asio::buffer(pendingFrame.frame),
		std::bind(&WebsocketServerConnection::handle_frame_sent, this, std::placeholders::_1, std::placeholders::_2));
}
void WebsocketServerConnection::handle_frame_sent(const boost::system::error_code& error, size_t bytes_sent)
{
	auto promise = m_frame_queue.front().promise;
	if (promise)
	{
		promise->set_value();
	}
	m_frame_queue.pop();
	m_sending_frame = false;

	if (error)
	{
		handle_error(error);
		if (m_socket.is_open())
		{
			m_socket.close();
		}
		return;
	}

	send_next_frame();
}

void WebsocketServerConnection::schedule_ping()
{
	if (!m_socket.is_open())
	{
		return;
	}

	m_ping_timer.expires_from_now(boost::posix_time::seconds(m_settings.ping_interval_sec));
	m_ping_timer.async_wait(std::bind(&WebsocketServerConnection::on_ping_interval, this, std::placeholders::_1));
}

void WebsocketServerConnection::on_ping_interval(const boost::system::error_code& error)
{
	if (error)
	{
		if (error != boost::asio::error::operation_aborted)
		{
			BOOST_LOG_TRIVIAL(error) << "Ping schedule error: " << error;
		}
		return;
	}

	++m_ping_timeout_count;
	if (m_settings.ping_timeout_count > 0 && m_ping_timeout_count > m_settings.ping_timeout_count)
	{
		BOOST_LOG_TRIVIAL(info) << "Client did not respond to ping messages. Closing connection.";
		m_socket.close();
		return;
	}

	PendingFrame pendingFrame;
	m_websocket_parser.makeFrame(pendingFrame.frame, WebSocketParser::PING_FRAME, nullptr, 0);
	m_frame_queue.emplace(std::move(pendingFrame));
	send_next_frame();

	schedule_ping();
}

void WebsocketServerConnection::handle_error(const boost::system::error_code& error)
{
	if (!error)
	{
		return;
	}

	if (error == boost::asio::error::eof || error == boost::asio::error::connection_reset ||
		error == boost::asio::error::connection_aborted ||
		error.value() == WSAEBADF) // bad file descriptor, when connection already closed
	{
		if (!m_disconnect_error_handled)
		{
			BOOST_LOG_TRIVIAL(info) << "Client disconnected";
			m_event_handler->onDisconnect(m_connection_id);
		}
		m_disconnect_error_handled = true;
		return;
	}

	BOOST_LOG_TRIVIAL(warning) << "client connection error: " << error;
}
}
