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

#include "SvHttpLibrary/WebsocketServerConnection.h"
#include "SvHttpLibrary/WebsocketServerSettings.h"

namespace SvHttp
{
class WebsocketServer
{
public:
	WebsocketServer(const WebsocketServerSettings& rSettings,
		boost::asio::io_service& rIoService,
		WebsocketServerConnection::EventHandler* pEventHandler);

	void start();

private:
	void start_accept();
	void handle_accept(std::shared_ptr<WebsocketServerConnection> conn, const boost::system::error_code& error);

	void schedule_cleanup();
	void do_cleanup(const boost::system::error_code& error);

private:
	const WebsocketServerSettings& m_rSettings;
	boost::asio::io_service& m_rIoService;
	boost::asio::deadline_timer m_CleanupTimer;
	boost::asio::ip::tcp::acceptor m_Acceptor;
	int m_NextConnectionId;
	WebsocketServerConnection::EventHandler* m_pEventHandler;
	std::vector<std::shared_ptr<WebsocketServerConnection>> m_Connections;
	std::vector<std::shared_ptr<WebsocketServerConnection>> m_ConnectionsMarkedForDeletion;
};

} // namespace SvHttp
