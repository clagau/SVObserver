//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file SVRCWEBSOCKETSERVER.h
/// All Rights Reserved 
//*****************************************************************************
//Singelton class holds an inproc Websocketserver
//******************************************************************************
#pragma once
#include "SVRCCommandInterface.h"
#include "SVRCRequestHandler.h"
#include "SVRPCLibrary\RPCServer.h"
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include "SVHttpLibrary\WebsocketServer.h"
#include "SVHttpLibrary\WebsocketServerSettings.h"


class SVRCWebsocketServer
{
public:	
	static SVRCWebsocketServer* Instance();
	~SVRCWebsocketServer();
	void Start(std::shared_ptr<SVRCCommandInterface> pCmd, std::shared_ptr<SvHttp::WebsocketServerSettings>  pSettings);
	void Stop();
	static const int ThreadCount = 2;

private:
	SVRCWebsocketServer();
	SVRCWebsocketServer(const SVRCWebsocketServer& )= delete;
	std::shared_ptr<SVRCCommandInterface>   m_pCommandInterface;
	std::unique_ptr<SVRCRequestHandler> m_pRequestHandler;
	std::unique_ptr<SvRpc::RPCServer>    m_pRpcServer;
	std::shared_ptr<SvHttp::WebsocketServerSettings> m_pSettings;
	
	std::shared_ptr<boost::asio::io_service::strand> m_pstrand;
	boost::asio::io_service m_io_service;
	std::unique_ptr<SvHttp::WebsocketServer> m_pWebsocketserver;
	std::unique_ptr<std::thread> m_threads[ThreadCount];

};
