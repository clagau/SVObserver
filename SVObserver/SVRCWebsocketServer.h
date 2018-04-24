//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file SVRCWEBSOCKETSERVER.h
/// All Rights Reserved 
//*****************************************************************************
//Singelton class holds an inproc Websocketserver
//******************************************************************************
#pragma once
//Moved to precompiled header: #include <boost/asio.hpp>
#include "SVRCRequestHandler.h"
#include "SVRPCLibrary\RPCServer.h"
#include "SVHttpLibrary\WebsocketServer.h"
#include "SVHttpLibrary\WebsocketServerSettings.h"

class SVRCWebsocketServer
{
public:
	static SVRCWebsocketServer* Instance();
	~SVRCWebsocketServer();
	void Start(std::shared_ptr<SVRCCommand> pCmd, std::shared_ptr<SvHttp::WebsocketServerSettings>  pSettings);
	void Stop();

private:
	SVRCWebsocketServer();
	SVRCWebsocketServer(const SVRCWebsocketServer&) = delete;
	std::shared_ptr<SVRCCommand> m_pCommand;
	std::unique_ptr<SVRCRequestHandler> m_pRequestHandler;
	std::unique_ptr<SvRpc::RPCServer> m_pRpcServer;
	std::shared_ptr<SvHttp::WebsocketServerSettings> m_pSettings;
	
	boost::asio::io_service m_io_service;
	std::unique_ptr<SvHttp::WebsocketServer> m_pWebsocketserver;
	std::unique_ptr<std::thread> m_pThread;
};
