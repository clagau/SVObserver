//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file WebsocketClient.cpp
/// All Rights Reserved
//******************************************************************************
/// Class for connecting to a Websocket server.
///
/// The state machine allows receiving and sending messages in parallel.
//******************************************************************************

#include "stdafx.h"

#include <iostream>

#include <boost/log/trivial.hpp>

#include "SVHttpLibrary/Handshake.h"
#include "SVHttpLibrary/WebsocketClient.h"

#define BUF_SIZE (64 * 1024)

namespace SVHTTP
{
WebsocketClient::WebsocketClient(boost::asio::io_service& io_service, EventHandler* event_handler)
	: m_io_service(io_service), m_socket(io_service), m_handshake(), m_event_handler(event_handler)
{
	if (!m_event_handler)
	{
		throw new std::runtime_error("WebsocketClient: no message handler provided!");
	}
}
boost::future<void> WebsocketClient::connect(std::string host, uint16_t port)
{
	BOOST_LOG_TRIVIAL(debug) << "Websocket client connecting to ws://" << host << ":" << port << "/";
	auto address = boost::asio::ip::address_v4::from_string(host);
	auto endpoint = boost::asio::ip::tcp::endpoint(address, port);
	m_socket.async_connect(endpoint, std::bind(&WebsocketClient::handle_connect, this, std::placeholders::_1));
	return m_connect_promise.get_future();
}
void WebsocketClient::disconnect()
{
	m_shutting_down = true;
	m_socket.close();
}
void WebsocketClient::handle_connect(const boost::system::error_code& ec)
{
	if (ec)
	{
		handle_connection_error(ec);
		return;
	}

	send_handshake();
}
void WebsocketClient::handle_connection_error(const boost::system::error_code& ec)
{
	if (!ec)
	{
		return;
	}

	if (m_shutting_down)
	{
		return;
	}

	if (ec == boost::asio::error::eof)
	{
		BOOST_LOG_TRIVIAL(info) << "server disconnected :(";
	}
	else
	{
		BOOST_LOG_TRIVIAL(error) << "server connection error: " << ec;
	}

	close_connection();
}
void WebsocketClient::handle_connection_success()
{
	m_connect_promise.set_value();
	m_event_handler->onConnect();
}
void WebsocketClient::close_connection()
{
	if (m_socket.is_open())
	{
		m_socket.close();
	}
	if (!m_disconnect_event_sent && !m_shutting_down)
	{
		m_disconnect_event_sent = true;
		m_event_handler->onDisconnect();
	}
}
void WebsocketClient::send_handshake()
{
	auto request = m_handshake.generateClienRequest();
	m_buf = std::vector<char> {request.begin(), request.end()};
	boost::asio::async_write(
		m_socket,
		boost::asio::buffer(m_buf),
		std::bind(&WebsocketClient::handle_handshake_sent, this, std::placeholders::_1, std::placeholders::_2));
}
void WebsocketClient::handle_handshake_sent(const boost::system::error_code& error, size_t bytes_sent)
{
	if (error)
	{
		handle_connection_error(error);
		return;
	}

	read_handshake_response();
}
void WebsocketClient::read_handshake_response()
{
	m_buf.clear();
	m_buf.resize(BUF_SIZE);
	boost::asio::async_read(
		m_socket,
		boost::asio::buffer(m_buf),
		boost::asio::transfer_at_least(1),
		std::bind(&WebsocketClient::handle_handshake_response, this, std::placeholders::_1, std::placeholders::_2));
}
void WebsocketClient::handle_handshake_response(const boost::system::error_code& error, size_t bytes_read)
{
	if (error)
	{
		handle_connection_error(error);
		return;
	}

	auto state = m_handshake.parseServerResponse(m_buf.data(), bytes_read);
	switch (state)
	{
		case Handshake::PS_INCOMPLETE:
			read_handshake_response();
			break;

		case Handshake::PS_SUCCESS:
			read_buffer();
			// connect is done now
			BOOST_LOG_TRIVIAL(debug) << "Websocket client successfully connected";
			handle_connection_success();
			break;

		case Handshake::PS_ERROR:
		case Handshake::PS_INVALID:
			BOOST_LOG_TRIVIAL(error) << "Received unknown handshake response from server.";
			close_connection();
			break;
	}
}
void WebsocketClient::sendTextMessage(const std::vector<char>& buf)
{
	auto res_frame = std::make_shared<std::vector<char>>();
	m_websocket_parser.makeFrame(*res_frame, WebSocketParser::TEXT_FRAME, buf.data(), buf.size());

	boost::asio::async_write(
		m_socket,
		boost::asio::buffer(*res_frame),
		std::bind(
		&WebsocketClient::handle_request_sent, this, std::placeholders::_1, std::placeholders::_2, res_frame));
}
void WebsocketClient::sendBinaryMessage(const std::vector<char>& buf)
{
	auto res_frame = std::make_shared<std::vector<char>>();
	m_websocket_parser.makeFrame(*res_frame, WebSocketParser::BINARY_FRAME, buf.data(), buf.size());

	boost::asio::async_write(
		m_socket,
		boost::asio::buffer(*res_frame),
		std::bind(
		&WebsocketClient::handle_request_sent, this, std::placeholders::_1, std::placeholders::_2, res_frame));
}
void WebsocketClient::handle_request_sent(const boost::system::error_code& error,
	size_t bytes_sent,
	std::shared_ptr<std::vector<char>> buf)
{
	if (error)
	{
		handle_connection_error(error);
		return;
	}
}
void WebsocketClient::read_buffer()
{
	m_buf.clear();
	m_buf.resize(BUF_SIZE);
	boost::asio::async_read(
		m_socket,
		boost::asio::buffer(m_buf),
		boost::asio::transfer_at_least(1),
		std::bind(&WebsocketClient::handle_read_buffer, this, std::placeholders::_1, std::placeholders::_2));
}
void WebsocketClient::handle_read_buffer(const boost::system::error_code& error, size_t bytes_read)
{
	if (error)
	{
		handle_connection_error(error);
		return;
	}

	m_frames.insert(m_frames.end(), m_buf.begin(), m_buf.begin() + bytes_read);

	while (process_frame(m_frames, m_payload))
	{
		;
	}

	if (m_socket.is_open())
	{
		read_buffer();
	}
}
bool WebsocketClient::process_frame(std::vector<char>& frames, std::vector<char>& payload)
{
	size_t frame_len = 0;
	auto frame_type = m_websocket_parser.parseFrame(
		reinterpret_cast<unsigned char*>(frames.data()), frames.size(), frame_len, payload);
	switch (frame_type)
	{
		case WebSocketParser::ERROR_FRAME:
			BOOST_LOG_TRIVIAL(error) << "Error while parsing incoming websocket message! Closing connection.";
			close_connection();
			return false;

		case WebSocketParser::CLOSING_FRAME:
			close_connection();
			return false;

		case WebSocketParser::TEXT_FRAME:
			m_event_handler->onTextMessage(payload);
			payload.clear();
			frames.erase(frames.begin(), frames.begin() + frame_len);
			return true;

		case WebSocketParser::BINARY_FRAME:
			m_event_handler->onBinaryMessage(payload);
			payload.clear();
			frames.erase(frames.begin(), frames.begin() + frame_len);
			return true;

		case WebSocketParser::PING_FRAME:
			send_pong();
			frames.erase(frames.begin(), frames.begin() + frame_len);
			return true;

		case WebSocketParser::PONG_FRAME:
			// ignore pong messages sent from server
			frames.erase(frames.begin(), frames.begin() + frame_len);
			return true;

		case WebSocketParser::CONTINUATION_FRAME:
			frames.erase(frames.begin(), frames.begin() + frame_len);
			return true;

		case WebSocketParser::INCOMPLETE_FRAME:
			return false;

		default:
			BOOST_LOG_TRIVIAL(error) << "Received unknown WebSocket frame: " << frame_type;
			close_connection();
			return false;
	}
}
void WebsocketClient::send_pong()
{
	auto res_frame = std::make_shared<std::vector<char>>();
	m_websocket_parser.makeFrame(*res_frame, WebSocketParser::PONG_FRAME, NULL, 0);

	boost::asio::async_write(
		m_socket,
		boost::asio::buffer(*res_frame),
		std::bind(
		&WebsocketClient::handle_request_sent, this, std::placeholders::_1, std::placeholders::_2, res_frame));
}
}
