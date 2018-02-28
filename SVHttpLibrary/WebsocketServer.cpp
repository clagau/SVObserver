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

#include "SVHttpLibrary/WebsocketServer.h"

namespace SVHTTP
{
static boost::asio::ip::tcp::endpoint build_endpoint(const WebsocketServerSettings& settings)
{
	using namespace boost::asio::ip;
	return tcp::endpoint(address_v4::from_string(settings.host), settings.port);
}
WebsocketServer::WebsocketServer(const WebsocketServerSettings& settings,
	boost::asio::io_service& io_service,
	WebsocketServerConnection::EventHandler* event_handler)
	: m_settings(settings), m_io_service(io_service), m_cleanup_timer(m_io_service),
	m_acceptor(io_service, build_endpoint(settings)), m_event_handler(event_handler), m_next_connection_id(0)
{
}
void WebsocketServer::start()
{
	start_accept();
	schedule_cleanup();
}
void WebsocketServer::start_accept()
{
	auto connection =
		std::make_shared<WebsocketServerConnection>(m_settings, m_io_service, ++m_next_connection_id, m_event_handler);
	m_acceptor.async_accept(connection->socket(),
		std::bind(&WebsocketServer::handle_accept, this, connection, std::placeholders::_1));
}
void WebsocketServer::handle_accept(std::shared_ptr<WebsocketServerConnection> connection,
	const boost::system::error_code& ec)
{
	if (ec)
	{
		BOOST_LOG_TRIVIAL(error) << "Error while accepting connection: " << ec;
		return;
	}

	connection->start();

	m_connections.push_back(connection);

	start_accept();
}
void WebsocketServer::schedule_cleanup()
{
	m_cleanup_timer.expires_from_now(boost::posix_time::seconds(m_settings.connection_cleanup_interval_sec));
	m_cleanup_timer.async_wait(std::bind(&WebsocketServer::do_cleanup, this, std::placeholders::_1));
}
void WebsocketServer::do_cleanup(const boost::system::error_code& error)
{
	if (error)
	{
		BOOST_LOG_TRIVIAL(error) << "Cleanup schedule error: " << error;
		return;
	}

	// delete connection added during last cleanup
	m_connections_marked_for_deletion.clear();

	for (auto it = m_connections.begin(); it != m_connections.end();)
	{
		auto conn = *it;
		if (!conn->isOpen())
		{
			BOOST_LOG_TRIVIAL(debug) << "Marking connection for deletion";
			it = m_connections.erase(it);
			m_connections_marked_for_deletion.push_back(conn);
		}
		else
		{
			++it;
		}
	}

	schedule_cleanup();
}
}
