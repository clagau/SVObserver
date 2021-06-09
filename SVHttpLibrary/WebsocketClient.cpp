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
#include <winerror.h>

#include "SVLogLibrary/Logging.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageTextGenerator.h"
#include "SVStatusLibrary/MessageManager.h"

#include "WebsocketClient.h"
#include "WebsocketClientSettings.h"


namespace SvHttp
{
WebsocketClient::WebsocketClient(WebsocketClientSettings& rSettings, EventHandler* pEventHandler)
	: m_rSettings(rSettings)
	, m_IoContext(1)
	, m_IoWork(std::make_unique<boost::asio::io_context::work>(m_IoContext))
	, m_Resolver(m_IoContext)
	, m_Socket(m_IoContext)
	, m_ControlCommand([this](boost::beast::websocket::frame_type frame_type, boost::beast::string_view text) {return handle_control_command(frame_type, text); })
	, m_PingTimer(m_IoContext)
	, m_PingTimeoutCount(0)
	, m_pEventHandler(pEventHandler)
{
	if (!m_pEventHandler)
	{
		throw new std::runtime_error("WebsocketClient: no message handler provided!");
	}
	m_Socket.control_callback(m_ControlCommand);
	m_Socket.read_message_max(rSettings.MaxMessageSize);
	m_Socket.write_buffer_bytes(rSettings.WriteBufferSize);
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

std::shared_ptr<WebsocketClient> WebsocketClient::create(WebsocketClientSettings& rSettings, EventHandler* pEventHandler)
{
	return std::shared_ptr<WebsocketClient>(new WebsocketClient(rSettings, pEventHandler));
}

void WebsocketClient::connect()
{
	SV_LOG_GLOBAL(debug) << "Websocket client connecting to ws://" << m_rSettings.Host << ":" << m_rSettings.Port << m_rSettings.Path;
	auto query = boost::asio::ip::tcp::resolver::query(m_rSettings.Host, std::to_string(m_rSettings.Port));
	auto handleResolveFunctor = [this](const boost::system::error_code& rError, boost::asio::ip::tcp::resolver::results_type results) { return handle_resolve(rError, results); };
	m_Resolver.async_resolve(query, handleResolveFunctor);
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
			std::stringstream ss;
			ss << ec;
			SvDef::StringVector msgList;
			msgList.push_back(ss.str());
			SV_LOG_GLOBAL(warning) << SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_WebSocket_ConnectionCloseError, msgList);
			SvStl::MessageManager Exception(SvStl::MsgType::Notify);
			Exception.setMessage(SVMSG_SVO_2_GENERAL_INFORMATIONAL, SvStl::Tid_WebSocket_ConnectionCloseError, msgList, SvStl::SourceFileParams(StdMessageParams));
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

void WebsocketClient::handle_resolve(const boost::system::error_code& ,
	boost::asio::ip::tcp::resolver::results_type results)
{
	auto handleConnectFunctor = [this](const boost::system::error_code& rError, boost::asio::ip::tcp::resolver::iterator it) { return handle_connect(rError, it); };
	boost::asio::async_connect(m_Socket.next_layer(), results.begin(), results.end(), handleConnectFunctor);
}

void WebsocketClient::handle_connect(const boost::system::error_code& rError, boost::asio::ip::tcp::resolver::iterator)
{
	if (rError)
	{
		handle_connection_error(rError);
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

	if (ec == boost::asio::error::eof ||
		ec == boost::asio::error::connection_reset ||
		ec == boost::asio::error::connection_aborted ||
		ec.value() == WSAEBADF) // bad file descriptor, when connection already closed
	{
		SV_LOG_GLOBAL(info) << "server disconnected :(";
	}
	else if (ec.value() == WSAECONNREFUSED)
	{
		SV_LOG_GLOBAL(debug) << "No connection could be made because the target machine actively refused it";
	}
	else
	{
		std::stringstream ss;
		ss << ec;
		SvDef::StringVector msgList;
		msgList.push_back(ss.str());
		SV_LOG_GLOBAL(warning) << SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_WebSocket_ConnectionError, msgList);
		SvStl::MessageManager Exception(SvStl::MsgType::Notify);
		Exception.setMessage(SVMSG_SVO_2_GENERAL_INFORMATIONAL, SvStl::Tid_WebSocket_ConnectionError, msgList, SvStl::SourceFileParams(StdMessageParams));
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
			std::stringstream ss;
			ss << ec;
			SvDef::StringVector msgList;
			msgList.push_back(ss.str());
			SV_LOG_GLOBAL(warning) << SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_WebSocket_ConnectionCloseError, msgList);
			SvStl::MessageManager Exception(SvStl::MsgType::Notify);
			Exception.setMessage(SVMSG_SVO_2_GENERAL_INFORMATIONAL, SvStl::Tid_WebSocket_ConnectionCloseError, msgList, SvStl::SourceFileParams(StdMessageParams));
		}
	}
	if (!m_IsDisconnectEventSent && !m_IsShuttingDown)
	{
		m_IsDisconnectEventSent = true;
		SV_LOG_GLOBAL(debug) << "Server disconnected";
		m_pEventHandler->onDisconnect();
	}
}

void WebsocketClient::send_handshake()
{
	if (!m_rSettings.Protocol.empty())
	{
		auto protocol = m_rSettings.Protocol;
		m_Socket.set_option(boost::beast::websocket::stream_base::decorator(
			[protocol](boost::beast::websocket::request_type& m)
			{
				m.insert(boost::beast::http::field::sec_websocket_protocol, protocol);
			}
		));
	}

	auto handshakeResponseFunctor = [this](const boost::system::error_code& rError) { return handle_handshake_response(rError); };
	m_Socket.async_handshake(m_rSettings.Host, m_rSettings.Path, handshakeResponseFunctor);
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
	SV_LOG_GLOBAL(debug) << "Websocket client successfully connected";
	handle_connection_success();
	schedule_ping();
}

void WebsocketClient::sendTextMessage(const std::vector<char>& buf)
{
	auto res_frame = std::make_shared<std::vector<char>>(buf);
	m_IoContext.dispatch([this, res_frame]()
	{
		write_message(true, res_frame);
	});
}

void WebsocketClient::sendBinaryMessage(const std::vector<char>& buf)
{
	auto res_frame = std::make_shared<std::vector<char>>(buf);
	m_IoContext.dispatch([this, res_frame]()
	{
		write_message(true, res_frame);
	});
}

void WebsocketClient::write_message(bool is_binary, std::shared_ptr<std::vector<char>> buf)
{
	if (m_bIsWriting)
	{
		m_WriteQueue.push_back(std::make_pair<>(is_binary, buf));
	}
	else
	{
		write_message_impl(is_binary, buf);
	}
}

void WebsocketClient::write_message_impl(bool is_binary, std::shared_ptr<std::vector<char>> buf)
{
	if (is_binary)
	{
		m_Socket.binary(true);
	}
	else
	{
		m_Socket.text(true);
	}

	m_bIsWriting = true;
	auto handleRequestSentFunctor = [this, buf](const boost::system::error_code& rError, size_t bytes) { return handle_request_sent(rError, bytes, buf); };
	m_Socket.async_write(boost::asio::buffer(*buf), handleRequestSentFunctor);
}

void WebsocketClient::check_write_queue()
{
	if (m_bIsWriting || m_WriteQueue.empty())
	{
		return;
	}

	auto head = m_WriteQueue.front();
	m_WriteQueue.pop_front();
	write_message_impl(head.first, head.second);
}

void WebsocketClient::handle_request_sent(const boost::system::error_code& error,
	size_t,
	std::shared_ptr<std::vector<char>> buf)
{
	m_bIsWriting = false;
	check_write_queue();

	if (error)
	{
		handle_connection_error(error);
		return;
	}
}

void WebsocketClient::read_buffer()
{
	m_Buf.clear();
	m_Buf.resize(m_rSettings.ReadBufferSize);
	auto handleReadBufferFunctor = [this](const boost::system::error_code& rError, size_t bytes_read) { return handle_read_buffer(rError, bytes_read); };
	boost::asio::async_read(m_Socket, boost::asio::buffer(m_Buf), boost::asio::transfer_at_least(1), handleReadBufferFunctor);
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
			m_pEventHandler->onBinaryMessage(std::move(m_Payload));
		}
		else
		{
			m_pEventHandler->onTextMessage(std::move(m_Payload));
		}
		m_Payload.clear();
	}

	if (m_Socket.is_open())
	{
		read_buffer();
	}
}

void WebsocketClient::schedule_ping()
{
	if (!m_Socket.is_open())
	{
		return;
	}

	m_PingTimer.expires_from_now(boost::posix_time::seconds(m_rSettings.PingIntervalSec));
	auto pingIntervalFunctor = [this](const boost::system::error_code& rError) { return on_ping_interval(rError); };
	m_PingTimer.async_wait(pingIntervalFunctor);
}

void WebsocketClient::on_ping_interval(const boost::system::error_code& error)
{
	if (error)
	{
		if (error != boost::asio::error::operation_aborted)
		{
			SV_LOG_GLOBAL(error) << "Ping schedule error: " << error;
		}
		return;
	}

	++m_PingTimeoutCount;
	if (m_rSettings.PingTimeoutCount > 0 && m_PingTimeoutCount > m_rSettings.PingTimeoutCount)
	{
		SV_LOG_GLOBAL(info) << SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_WebSocket_ConnectionCloseError);
		SvStl::MessageManager Exception(SvStl::MsgType::Notify);
		Exception.setMessage(SVMSG_SVO_2_GENERAL_INFORMATIONAL, SvStl::Tid_WebSocket_PingTimeout, SvStl::SourceFileParams(StdMessageParams));

		m_Socket.next_layer().close();
		return;
	}

	auto handlePingSentFunctor = [this](const boost::system::error_code& rError) { return handle_ping_sent(rError); };
	m_Socket.async_ping(boost::beast::websocket::ping_data(), handlePingSentFunctor);

	schedule_ping();
}

void WebsocketClient::handle_ping_sent(const boost::system::error_code& error)
{
	if (error)
	{
		handle_connection_error(error);
		return;
	}
}

void WebsocketClient::handle_control_command(boost::beast::websocket::frame_type frame_type, boost::beast::string_view)
{
	switch (frame_type)
	{
		case boost::beast::websocket::frame_type::pong:
			m_PingTimeoutCount = 0;
			break;
		default:
			break;
	}
}

} // namespace SvHttp
