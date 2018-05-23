//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file WebsocketServer.cpp
/// All Rights Reserved
//******************************************************************************
/// Not much to see here. The actual handling of the connection to the client
/// happens in WebsocketServerConnection. Here we are just managing the
/// connection itself.
//******************************************************************************

#include "stdafx.h"

#include <memory>

#include <boost/log/trivial.hpp>

#include "SvHttpLibrary/WebsocketServer.h"

namespace SvHttp
{
static boost::asio::ip::tcp::endpoint build_endpoint(const WebsocketServerSettings& settings)
{
	using namespace boost::asio::ip;
	return tcp::endpoint(address_v4::from_string(settings.Host), settings.Port);
}

WebsocketServer::WebsocketServer(const WebsocketServerSettings& rSettings,
	boost::asio::io_context& rIoContext,
	WebsocketServerConnection::EventHandler* pEventHandler)
	: m_rSettings(rSettings), m_rIoContext(rIoContext), m_CleanupTimer(rIoContext),
	m_Acceptor(rIoContext, build_endpoint(rSettings)), m_Socket(rIoContext), m_pEventHandler(pEventHandler), m_NextConnectionId(0)
{
}

void WebsocketServer::start()
{
	start_accept();
	schedule_cleanup();
}

void WebsocketServer::stop()
{
	m_Acceptor.cancel();
	m_CleanupTimer.cancel();
}

void WebsocketServer::start_accept()
{
	m_Acceptor.async_accept(m_Socket,
		std::bind(&WebsocketServer::handle_accept, this, std::placeholders::_1));
}

void WebsocketServer::handle_accept(const boost::system::error_code& ec)
{
	if (ec == boost::asio::error::operation_aborted)
	{
		return;
	}
	if (ec)
	{
		BOOST_LOG_TRIVIAL(error) << "Error while accepting connection: " << ec;
		return;
	}

	auto connection = WebsocketServerConnection::create(m_rSettings, m_rIoContext, std::move(m_Socket), ++m_NextConnectionId, m_pEventHandler);

	connection->start();

	m_Connections.push_back(connection);

	start_accept();
}

void WebsocketServer::schedule_cleanup()
{
	m_CleanupTimer.expires_from_now(boost::posix_time::seconds(m_rSettings.ConnectionCleanupIntervalSec));
	m_CleanupTimer.async_wait(std::bind(&WebsocketServer::do_cleanup, this, std::placeholders::_1));
}

void WebsocketServer::do_cleanup(const boost::system::error_code& error)
{
	if (error == boost::asio::error::operation_aborted)
	{
		return;
	}
	if (error)
	{
		BOOST_LOG_TRIVIAL(error) << "Cleanup schedule error: " << error;
		return;
	}

	// delete connection added during last cleanup
	m_ConnectionsMarkedForDeletion.clear();

	for (auto it = m_Connections.begin(); it != m_Connections.end();)
	{
		auto conn = *it;
		if (!conn->isOpen())
		{
			BOOST_LOG_TRIVIAL(debug) << "Marking connection for deletion";
			it = m_Connections.erase(it);
			m_ConnectionsMarkedForDeletion.push_back(conn);
		}
		else
		{
			++it;
		}
	}

	schedule_cleanup();
}

} // namespace SvHttp
