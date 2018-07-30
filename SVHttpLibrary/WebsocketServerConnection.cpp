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

#include "WebsocketServerConnection.h"

namespace SvHttp
{
WebsocketServerConnection::WebsocketServerConnection(const WebsocketServerSettings& rSettings,
	boost::asio::io_context& rIoContext,
	boost::asio::ip::tcp::socket Socket,
	int ConnectionId,
	EventHandler* pEventHandler)
	: m_ConnectionId(ConnectionId)
	, m_rSettings(rSettings)
	, m_rIoContext(rIoContext)
	, m_Socket(std::move(Socket))
	, m_pEventHandler(pEventHandler)
{
	m_Socket.read_message_max(rSettings.MaxMessageSize);
	m_Socket.write_buffer_size(rSettings.WriteBufferSize);
}

std::shared_ptr<WebsocketServerConnection> WebsocketServerConnection::create(
	const WebsocketServerSettings& rSettings,
	boost::asio::io_context& rIoContext,
	boost::asio::ip::tcp::socket Socket,
	int ConnectionId,
	EventHandler* pEventHandler)
{
	return std::shared_ptr<WebsocketServerConnection>(new WebsocketServerConnection(rSettings, rIoContext, std::move(Socket), ConnectionId, pEventHandler));
}

bool WebsocketServerConnection::isOpen() const
{
	return m_Socket.is_open();
}

void WebsocketServerConnection::start()
{
	m_Socket.async_accept_ex(
		std::bind(&WebsocketServerConnection::handle_response_decoration, shared_from_this(), std::placeholders::_1),
		std::bind(&WebsocketServerConnection::handle_handshake_done, shared_from_this(), std::placeholders::_1)
	);
}

void WebsocketServerConnection::handle_response_decoration(boost::beast::websocket::response_type& m)
{
	m.insert(boost::beast::http::field::server, "SVObserver");
}

void WebsocketServerConnection::handle_handshake_done(const boost::system::error_code& error)
{
	if (error)
	{
		handle_error(error);
		return;
	}

	m_pEventHandler->onConnect(m_ConnectionId, *this);

	read_buffer();
}

void WebsocketServerConnection::read_buffer()
{
	m_Buf.resize(m_rSettings.ReadBufferSize);
	boost::asio::async_read(m_Socket,
		boost::asio::buffer(m_Buf),
		boost::asio::transfer_at_least(1),
		std::bind(&WebsocketServerConnection::handle_read_buffer,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2));
}

void WebsocketServerConnection::handle_read_buffer(const boost::system::error_code& error, size_t bytes_read)
{
	if (error)
	{
		handle_error(error);
		if (m_Socket.is_open())
		{
			boost::system::error_code ec;
			m_Socket.next_layer().close(ec);
			if (ec)
			{
				BOOST_LOG_TRIVIAL(warning) << "Error while closing websocket connection: " << ec;
			}
		}
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
			m_pEventHandler->onBinaryMessage(m_ConnectionId, m_Payload);
		}
		else
		{
			m_pEventHandler->onTextMessage(m_ConnectionId, m_Payload);
		}
		m_Payload.clear();
	}

	if (m_Socket.is_open())
	{
		read_buffer();
	}
}

std::future<void> WebsocketServerConnection::sendTextMessage(const std::vector<char>& buf)
{
	return send_message_impl(buf, false);
}

std::future<void> WebsocketServerConnection::sendBinaryMessage(const std::vector<char>& buf)
{
	return send_message_impl(buf, true);
}

std::future<void> WebsocketServerConnection::send_message_impl(const std::vector<char>& buf, bool is_binary)
{
	auto promise = std::make_shared<std::promise<void>>();
	m_rIoContext.dispatch([this, buf, is_binary, promise]()
	{
		PendingFrame pendingFrame;
		pendingFrame.IsBinary = is_binary;
		pendingFrame.Frame = std::move(buf);
		pendingFrame.Promise = promise;
		m_FrameQueue.emplace(std::move(pendingFrame));
		send_next_frame();
	});
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
	m_Socket.binary(pendingFrame.IsBinary);
	m_Socket.async_write(
		boost::asio::buffer(pendingFrame.Frame),
		std::bind(&WebsocketServerConnection::handle_frame_sent, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
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
			boost::system::error_code ec;
			m_Socket.next_layer().close(ec);
			if (ec)
			{
				BOOST_LOG_TRIVIAL(warning) << "Error while closing websocket connection: " << ec;
			}
		}
		return;
	}

	send_next_frame();
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
