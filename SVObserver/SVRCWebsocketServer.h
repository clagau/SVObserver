//*****************************************************************************
/// \copyright (c) 2018,2018 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file SVRCWEBSOCKETSERVER.h
/// All Rights Reserved 
//*****************************************************************************
//Singelton class holds an inproc Websocketserver
//******************************************************************************
#pragma once
//Moved to precompiled header: #include <boost/asio.hpp>
#include "SVRCRequestHandler.h"
#include "SVRPCLibrary\RPCServer.h"
#include "SVHttpLibrary\HttpServer.h"
#include "SVHttpLibrary\HttpServerSettings.h"

class SVRCWebsocketServer
{
public:
	static SVRCWebsocketServer* Instance();
	~SVRCWebsocketServer();
	void Start(std::shared_ptr<SVRCCommand> pCmd, std::unique_ptr<SvHttp::HttpServerSettings> &&rpSettings);
	void Stop();

private:
	SVRCWebsocketServer();
	SVRCWebsocketServer(const SVRCWebsocketServer&) = delete;
	std::shared_ptr<SVRCCommand> m_pCommand;
	std::unique_ptr<SVRCRequestHandler> m_pRequestHandler;
	std::unique_ptr<SvRpc::RPCServer> m_pRpcServer;

	std::unique_ptr<SvHttp::HttpServerSettings> m_pSettings;

	std::atomic_bool m_bIsRunning {false};
	boost::asio::io_context m_io_service;
	std::unique_ptr<SvHttp::HttpServer> m_pHttpserver;
	std::unique_ptr<std::thread> m_pThread;
};
