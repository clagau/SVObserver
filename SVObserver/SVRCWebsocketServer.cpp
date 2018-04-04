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
	:m_io_service {SVRCWebsocketServer::ThreadCount}
{
	m_pstrand = std::make_shared<boost::asio::io_service::strand>(m_io_service);
}
void SVRCWebsocketServer::Start(std::shared_ptr<SVRCCommandInterface> pCmd, std::shared_ptr<SvHttp::WebsocketServerSettings>  pSettings)
{
	m_pSettings = pSettings;
	m_pCommandInterface = pCmd;
	m_pRequestHandler = std::make_unique<SVRCRequestHandler>(m_pCommandInterface.get(), m_pstrand);
	m_pRpcServer = std::make_unique<SvRpc::RPCServer>(m_pRequestHandler.get());
	m_pWebsocketserver = std::make_unique<SvHttp::WebsocketServer>(*pSettings, m_io_service, m_pRpcServer.get());
	m_pWebsocketserver->start();
	for (int i = 0; i < ThreadCount; i++)
	{
		m_threads[i].reset(new std::thread([this]()
		{
			m_io_service.run();
		}
		));
	}
}
void SVRCWebsocketServer::Stop()
{
	m_io_service.stop();
	for (int i = 0; i < ThreadCount; i++)
	{
		if (m_threads[i].get())
		{
			m_threads[i]->join();
		
		}
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