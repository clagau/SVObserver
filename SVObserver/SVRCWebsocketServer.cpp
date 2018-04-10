//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file SVRCWebsocketServer.cpp
/// All Rights Reserved 
//*****************************************************************************
//Singelton class holds an inproc Websocketserver
//******************************************************************************
#include "StdAfx.h"
#include "SVRCWebsocketServer.h"

SVRCWebsocketServer::SVRCWebsocketServer()
	:m_io_service {1}
{

}
void SVRCWebsocketServer::Start(std::shared_ptr<SVRCCommand> pCmd, std::shared_ptr<SvHttp::WebsocketServerSettings>  pSettings)
{
	m_pSettings = pSettings;
	m_pCommand = pCmd;
	m_pRequestHandler = std::make_unique<SVRCRequestHandler>(m_pCommand.get());
	m_pRpcServer = std::make_unique<SvRpc::RPCServer>(m_pRequestHandler.get());
	m_pWebsocketserver = std::make_unique<SvHttp::WebsocketServer>(*pSettings, m_io_service, m_pRpcServer.get());
	m_pWebsocketserver->start();

	m_pThread.reset(new std::thread([this]()
	{
		m_io_service.run();
	}
	));
}

void SVRCWebsocketServer::Stop()
{
	m_pWebsocketserver->stop();
	m_io_service.stop();

	if (m_pThread.get() && m_pThread->joinable())
	{
		m_pThread->join();
	}

}
SVRCWebsocketServer::~SVRCWebsocketServer()
{
	Stop();
}

SVRCWebsocketServer* SVRCWebsocketServer::Instance()
{
	static  std::unique_ptr<SVRCWebsocketServer> instance;
	if (nullptr == instance.get())
	{

		instance = std::unique_ptr<SVRCWebsocketServer>(new SVRCWebsocketServer());
	}
	return instance.get();
}