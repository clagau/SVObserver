//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file HttpServer.cpp
/// All Rights Reserved
//******************************************************************************
/// Not much to see here. The actual handling of the connection to the client
/// happens in WebsocketServerConnection. Here we are just managing the
/// connection itself.
//******************************************************************************

#include "stdafx.h"

#include <memory>

#include "SVLogLibrary/Logging.h"

#include "HttpServer.h"

namespace SvHttp
{
static boost::asio::ip::tcp::endpoint build_endpoint(const HttpServerSettings& settings)
{
	const auto addr = boost::asio::ip::address_v4::from_string(settings.Host);
	return boost::asio::ip::tcp::endpoint(addr, settings.Port);
}

HttpServer::HttpServer(const HttpServerSettings& rSettings, boost::asio::io_context& rIoContext)
	: m_rSettings(rSettings)
	, m_rIoContext(rIoContext)
	, m_CleanupTimer(rIoContext)
	, m_Acceptor(rIoContext, build_endpoint(rSettings), false)
	, m_Socket(rIoContext)
	, m_NextConnectionId(0)
{
}

void HttpServer::start()
{
	if (m_bIsRunning)
	{
		return;
	}

	start_accept();
	schedule_cleanup();
	m_bIsRunning = true;
}

void HttpServer::stop()
{
	if (!m_bIsRunning)
	{
		return;
	}

	m_bIsRunning = false;
	m_Acceptor.cancel();
	m_Acceptor.close();
	m_CleanupTimer.cancel();
	close_connections(m_ConnectionsMarkedForDeletion);
	close_connections(m_Connections);
}

void HttpServer::close_connections(std::vector<std::shared_ptr<HttpServerConnection>>& connections)
{
	for (auto& conn : connections)
	{
		conn->close();
	}
	connections.clear();
}

void HttpServer::start_accept()
{
	m_Acceptor.async_accept(m_Socket,
		std::bind(&HttpServer::handle_accept, this, std::placeholders::_1));
}

void HttpServer::handle_accept(const boost::system::error_code& ec)
{
	if (ec == boost::asio::error::operation_aborted)
	{
		return;
	}
	if (ec)
	{
		SV_LOG_GLOBAL(error) << "Error while accepting connection: " << ec;
		return;
	}

	auto connection = HttpServerConnection::create(m_rSettings, m_rIoContext, std::move(m_Socket), ++m_NextConnectionId);

	connection->start();

	m_Connections.push_back(connection);

	start_accept();
}

void HttpServer::schedule_cleanup()
{
	m_CleanupTimer.expires_from_now(boost::posix_time::seconds(m_rSettings.ConnectionCleanupIntervalSec));
	m_CleanupTimer.async_wait(std::bind(&HttpServer::do_cleanup, this, std::placeholders::_1));
}

void HttpServer::do_cleanup(const boost::system::error_code& error)
{
	if (error == boost::asio::error::operation_aborted)
	{
		return;
	}
	if (error)
	{
		SV_LOG_GLOBAL(error) << "Cleanup schedule error: " << error;
		return;
	}

	// delete connection added during last cleanup
	m_ConnectionsMarkedForDeletion = {};

	for (auto it = m_Connections.begin(); it != m_Connections.end();)
	{
		auto conn = *it;
		if (!conn->isOpen())
		{
			SV_LOG_GLOBAL(debug) << "Marking connection for deletion";
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
