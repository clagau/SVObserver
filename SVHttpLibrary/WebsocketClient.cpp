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

#include "WebsocketClient.h"
#include "Defines.h"

#define BUF_SIZE (64 * 1024)

namespace SvHttp
{
WebsocketClient::WebsocketClient(EventHandler* pEventHandler)
	: m_IoContext(1)
	, m_IoWork(std::make_unique<boost::asio::io_context::work>(m_IoContext))
	, m_Resolver(m_IoContext)
	, m_Socket(m_IoContext)
	, m_pEventHandler(pEventHandler)
{
	if (!m_pEventHandler)
	{
		throw new std::runtime_error("WebsocketClient: no message handler provided!");
	}

	m_Socket.read_message_max(MAX_MESSAGE_SIZE);
	m_Socket.write_buffer_size(BUF_SIZE);
	m_IoThread = std::thread([this]() { m_IoContext.run(); });
}

WebsocketClient::~WebsocketClient()
{
	disconnect();
	if (m_IoThread.joinable())
	{
		m_IoThread.join();
	}
}

std::shared_ptr<WebsocketClient> WebsocketClient::create(EventHandler* pEventHandler)
{
	return std::shared_ptr<WebsocketClient>(new WebsocketClient(pEventHandler));
}

void WebsocketClient::connect(std::string host, uint16_t port, std::string path)
{
	m_Host = host;
	m_Port = port;
	m_Path = path;
	BOOST_LOG_TRIVIAL(debug) << "Websocket client connecting to ws://" << host << ":" << port << "/";
	auto query = boost::asio::ip::tcp::resolver::query(host, std::to_string(port));
	m_Resolver.async_resolve(query, std::bind(&WebsocketClient::handle_resolve, this, std::placeholders::_1, std::placeholders::_2));
}

void WebsocketClient::disconnect()
{
	if (m_IsShuttingDown)
	{
		return;
	}

	m_IsShuttingDown = true;
	if (m_Socket.is_open())
	{
		boost::system::error_code ec;
		m_Socket.next_layer().close(ec);
		if (ec)
		{
			BOOST_LOG_TRIVIAL(warning) << "Error while closing websocket connection: " << ec;
		}
	}
	m_IoWork.reset();
	if (!m_IoContext.stopped())
	{
		m_IoContext.stop();
	}
	if (m_IoThread.joinable())
	{
		m_IoThread.join();
	}
}

void WebsocketClient::handle_resolve(const boost::system::error_code& ec,
	boost::asio::ip::tcp::resolver::results_type results)
{
	boost::asio::async_connect(m_Socket.next_layer(), results.begin(), results.end(),
		std::bind(&WebsocketClient::handle_connect, this, std::placeholders::_1));
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

	if (m_IsShuttingDown)
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
	m_pEventHandler->onConnect();
}

void WebsocketClient::close_connection()
{
	if (m_Socket.is_open())
	{
		boost::system::error_code ec;
		m_Socket.next_layer().close(ec);
		if (ec)
		{
			BOOST_LOG_TRIVIAL(warning) << "Error while closing websocket connection: " << ec;
		}
	}
	if (!m_IsDisconnectEventSent && !m_IsShuttingDown)
	{
		m_IsDisconnectEventSent = true;
		m_pEventHandler->onDisconnect();
	}
}

void WebsocketClient::send_handshake()
{
	m_Socket.async_handshake(m_Host, m_Path, std::bind(&WebsocketClient::handle_handshake_response, this, std::placeholders::_1));
}

void WebsocketClient::handle_handshake_response(const boost::system::error_code& error)
{
	if (error)
	{
		handle_connection_error(error);
		return;
	}

	read_buffer();
	// connect is done now
	BOOST_LOG_TRIVIAL(debug) << "Websocket client successfully connected";
	handle_connection_success();
}

void WebsocketClient::sendTextMessage(const std::vector<char>& buf)
{
	auto res_frame = std::make_shared<std::vector<char>>(buf);
	m_Socket.text(true);
	m_Socket.async_write(
		boost::asio::buffer(*res_frame),
		std::bind(&WebsocketClient::handle_request_sent, this, std::placeholders::_1, std::placeholders::_2, res_frame));
}

void WebsocketClient::sendBinaryMessage(const std::vector<char>& buf)
{
	auto res_frame = std::make_shared<std::vector<char>>(buf);
	m_Socket.binary(true);
	m_Socket.async_write(
		boost::asio::buffer(*res_frame),
		std::bind(&WebsocketClient::handle_request_sent, this, std::placeholders::_1, std::placeholders::_2, res_frame));
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
	m_Buf.clear();
	m_Buf.resize(BUF_SIZE);
	boost::asio::async_read(
		m_Socket,
		boost::asio::buffer(m_Buf),
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

	auto prev_size = m_Payload.size();
	m_Payload.reserve(prev_size + bytes_read + 1);
	m_Payload.insert(m_Payload.end(), m_Buf.data(), m_Buf.data() + bytes_read);
	m_Buf.clear();

	if (m_Socket.is_message_done())
	{
		if (m_Socket.got_binary())
		{
			m_pEventHandler->onBinaryMessage(m_Payload);
		}
		else
		{
			m_pEventHandler->onTextMessage(m_Payload);
		}
		m_Payload.clear();
	}

	if (m_Socket.is_open())
	{
		read_buffer();
	}
}

} // namespace SvHttp
