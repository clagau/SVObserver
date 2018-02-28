//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file WebsocketServer.h
/// All Rights Reserved
//******************************************************************************
/// Use this class to create a Websocket server.
///
/// Just call start and it will bind a TCP socket on the host and port
/// specified in the WebsocketServerSettings. For each new client connection, a
/// new WebsocketServerConnection is created. The provided MessageHandler
/// instance will be passed to each connection.
///
/// This class will also automatically clean up all closed connections after
/// a configurable grace period.
//******************************************************************************

#pragma once

#include <memory>
#include <vector>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "SVHttpLibrary/WebsocketServerConnection.h"
#include "SVHttpLibrary/WebsocketServerSettings.h"

namespace SVHTTP
{
class WebsocketServer
{
public:
	WebsocketServer(const WebsocketServerSettings& settings,
		boost::asio::io_service& io_service,
		WebsocketServerConnection::EventHandler* event_handler);

	void start();

private:
	void start_accept();
	void handle_accept(std::shared_ptr<WebsocketServerConnection> conn, const boost::system::error_code& error);

	void schedule_cleanup();
	void do_cleanup(const boost::system::error_code& error);

private:
	const WebsocketServerSettings& m_settings;
	boost::asio::io_service& m_io_service;
	boost::asio::deadline_timer m_cleanup_timer;
	boost::asio::ip::tcp::acceptor m_acceptor;
	int m_next_connection_id;
	WebsocketServerConnection::EventHandler* m_event_handler;
	std::vector<std::shared_ptr<WebsocketServerConnection>> m_connections;
	std::vector<std::shared_ptr<WebsocketServerConnection>> m_connections_marked_for_deletion;
};
}
