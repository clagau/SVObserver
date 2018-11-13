//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file HttpServer.h
/// All Rights Reserved
//******************************************************************************
/// Use this class to create a Http server.
///
/// Just call start and it will bind a TCP socket on the host and port
/// specified in the HttpServerSettings. For each new client connection, a
/// new WebsocketServerConnection is created. The provided MessageHandler
/// instance will be passed to each connection.
///
/// This class will also automatically clean up all closed connections after
/// a configurable grace period.
//******************************************************************************

#pragma once

#include <memory>
#include <vector>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "HttpServerConnection.h"
#include "HttpServerSettings.h"

namespace SvHttp
{

class HttpServer
{
public:
	HttpServer(const HttpServerSettings& rSettings, boost::asio::io_context& rIoContext);

	void start();
	void stop();

private:
	void start_accept();
	void handle_accept(const boost::system::error_code& error);

	void schedule_cleanup();
	void do_cleanup(const boost::system::error_code& error);

	void close_connections(std::vector<std::shared_ptr<HttpServerConnection>>&);

private:
	const HttpServerSettings& m_rSettings;
	boost::asio::io_context& m_rIoContext;
	boost::asio::deadline_timer m_CleanupTimer;
	boost::asio::ip::tcp::acceptor m_Acceptor;
	boost::asio::ip::tcp::socket m_Socket;
	std::atomic_bool m_bIsRunning {false};
	int m_NextConnectionId;
	std::vector<std::shared_ptr<HttpServerConnection>> m_Connections;
	std::vector<std::shared_ptr<HttpServerConnection>> m_ConnectionsMarkedForDeletion;
};

} // namespace SvHttp
