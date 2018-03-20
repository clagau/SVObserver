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

#include "SvHttpLibrary/Handshake.h"
#include "SvHttpLibrary/WebsocketServerConnection.h"

namespace SvHttp
{
WebsocketServerConnection::WebsocketServerConnection(const WebsocketServerSettings& rSettings,
	boost::asio::io_service& rIoService,
	int ConnectionId,
	EventHandler* pEventHandler)
	: m_ConnectionId(ConnectionId), m_rSettings(rSettings), m_rIoService(rIoService), m_Socket(rIoService),
	m_PingTimer(rIoService), m_PingTimeoutCount(0), m_Handshake(), m_pEventHandler(pEventHandler)
{
}

bool WebsocketServerConnection::isOpen() const
{
	return m_Socket.is_open();
}

boost::asio::ip::tcp::socket& WebsocketServerConnection::socket()
{
	return m_Socket;
}

void WebsocketServerConnection::start()
{
	read_handshake();
}

void WebsocketServerConnection::read_handshake()
{
	m_Buf.assign(m_rSettings.ReadBufferSize, '\0');
	boost::asio::async_read(
		m_Socket,
		boost::asio::buffer(m_Buf),
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

	auto state = m_Handshake.parseClientRequest(m_Buf.data(), bytes_read);
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
			m_Socket.close();
			break;
	}
}

void WebsocketServerConnection::send_handshake_response()
{
	auto response = m_Handshake.generateServerResponse();
	m_Buf = std::vector<char> {response.begin(), response.end()};
	boost::asio::async_write(
		m_Socket,
		boost::asio::buffer(m_Buf),
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

	m_pEventHandler->onConnect(m_ConnectionId, *this);

	read_buffer();
	schedule_ping();
}

void WebsocketServerConnection::read_buffer()
{
	m_Buf.resize(m_rSettings.ReadBufferSize);
	boost::asio::async_read(m_Socket,
		boost::asio::buffer(m_Buf),
		boost::asio::transfer_at_least(1),
		std::bind(&WebsocketServerConnection::handle_read_buffer,
		this,
		std::placeholders::_1,
		std::placeholders::_2,
		std::ref(m_Buf)));
}

void WebsocketServerConnection::handle_read_buffer(const boost::system::error_code& error,
	size_t bytes_read,
	std::vector<char>& buf)
{
	if (error)
	{
		handle_error(error);
		if (m_Socket.is_open())
		{
			m_Socket.close();
		}
		return;
	}

	auto prev_size = m_Frames.size();
	m_Frames.reserve(prev_size + bytes_read + 1);
	m_Frames.insert(m_Frames.end(), buf.data(), buf.data() + bytes_read);

	while (process_frame(m_Frames, m_Payload))
	{
		;
	}

	if (m_Socket.is_open())
	{
		read_buffer();
	}
}

bool WebsocketServerConnection::process_frame(std::vector<char>& frames, std::vector<char>& payload)
{
	size_t frame_len = 0;
	auto frame_type = m_WebsocketParser.parseFrame(
		reinterpret_cast<unsigned char*>(frames.data()), frames.size(), frame_len, payload);

	switch (frame_type)
	{
		case WebSocketParser::ERROR_FRAME:
			BOOST_LOG_TRIVIAL(error) << "Error while parsing incoming websocket message! Closing connection.";
			m_Socket.close();
			return false;

		case WebSocketParser::CLOSING_FRAME:
			m_Socket.close();
			return false;

		case WebSocketParser::TEXT_FRAME:
			m_pEventHandler->onTextMessage(m_ConnectionId, payload);
			payload.clear();
			frames.erase(frames.begin(), frames.begin() + frame_len);
			return true;

		case WebSocketParser::BINARY_FRAME:
			m_pEventHandler->onBinaryMessage(m_ConnectionId, payload);
			payload.clear();
			frames.erase(frames.begin(), frames.begin() + frame_len);
			return true;

		case WebSocketParser::PING_FRAME:
			BOOST_LOG_TRIVIAL(warning) << "server should not receive ping message!";
			frames.erase(frames.begin(), frames.begin() + frame_len);
			return true;

		case WebSocketParser::PONG_FRAME:
			m_PingTimeoutCount = 0;
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
			m_Socket.close();
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
	if (m_rSettings.SplitLargePayload)
	{
		std::vector<std::vector<char>> frames;
		m_WebsocketParser.makeFrames(frames, type, buf.data(), buf.size());
		m_rIoService.dispatch([this, frames = std::move(frames), promise]() {
			for (size_t i = 0; i < frames.size(); ++i)
			{
				PendingFrame pendingFrame;
				pendingFrame.Frame = std::move(frames[i]);
				// fullfill promise after last write only
				if (i + 1 == frames.size())
				{
					pendingFrame.Promise = promise;
				}
				m_FrameQueue.emplace(std::move(pendingFrame));
			}
			send_next_frame();
		});
	}
	else
	{
		std::vector<char> frame;
		m_WebsocketParser.makeFrame(frame, WebSocketParser::BINARY_FRAME, buf.data(), buf.size());
		m_rIoService.dispatch([this, frame = std::move(frame), promise]() {
			PendingFrame pendingFrame;
			pendingFrame.Frame = std::move(frame);
			pendingFrame.Promise = promise;
			m_FrameQueue.emplace(std::move(pendingFrame));
			send_next_frame();
		});
	}
	return promise->get_future();
}

void WebsocketServerConnection::send_next_frame()
{
	if (m_FrameQueue.empty() || m_IsSendingFrame)
	{
		return;
	}

	m_IsSendingFrame = true;
	auto& pendingFrame = m_FrameQueue.front();
	boost::asio::async_write(
		m_Socket,
		boost::asio::buffer(pendingFrame.Frame),
		std::bind(&WebsocketServerConnection::handle_frame_sent, this, std::placeholders::_1, std::placeholders::_2));
}

void WebsocketServerConnection::handle_frame_sent(const boost::system::error_code& error, size_t bytes_sent)
{
	auto promise = m_FrameQueue.front().Promise;
	if (promise)
	{
		promise->set_value();
	}
	m_FrameQueue.pop();
	m_IsSendingFrame = false;

	if (error)
	{
		handle_error(error);
		if (m_Socket.is_open())
		{
			m_Socket.close();
		}
		return;
	}

	send_next_frame();
}

void WebsocketServerConnection::schedule_ping()
{
	if (!m_Socket.is_open())
	{
		return;
	}

	m_PingTimer.expires_from_now(boost::posix_time::seconds(m_rSettings.PingIntervalSec));
	m_PingTimer.async_wait(std::bind(&WebsocketServerConnection::on_ping_interval, this, std::placeholders::_1));
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

	++m_PingTimeoutCount;
	if (m_rSettings.PingTimeoutCount > 0 && m_PingTimeoutCount > m_rSettings.PingTimeoutCount)
	{
		BOOST_LOG_TRIVIAL(info) << "Client did not respond to ping messages. Closing connection.";
		m_Socket.close();
		return;
	}

	PendingFrame pendingFrame;
	m_WebsocketParser.makeFrame(pendingFrame.Frame, WebSocketParser::PING_FRAME, nullptr, 0);
	m_FrameQueue.emplace(std::move(pendingFrame));
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
		if (!m_IsDisconnectErrorHandled)
		{
			BOOST_LOG_TRIVIAL(info) << "Client disconnected";
			m_pEventHandler->onDisconnect(m_ConnectionId);
		}
		m_IsDisconnectErrorHandled = true;
		return;
	}

	BOOST_LOG_TRIVIAL(warning) << "client connection error: " << error;
}

} // namespace SvHttp
