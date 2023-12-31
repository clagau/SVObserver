//*****************************************************************************
/// \copyright (c) 2018,2018 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file SVRCWebsocketServer.cpp
/// All Rights Reserved 
//*****************************************************************************
//Singelton class holds an inproc Websocketserver
//******************************************************************************
#include "StdAfx.h"
#include "SVRCWebsocketServer.h"

constexpr int cThreadCount = 1;

SVRCWebsocketServer::SVRCWebsocketServer()
	:m_io_service {cThreadCount}
{

}

void SVRCWebsocketServer::Start(std::shared_ptr<SVRCCommand> pCmd, std::unique_ptr<SvHttp::HttpServerSettings>&&  rpSettings)
{
	if (m_bIsRunning)
	{
		return;
	}

	m_pSettings = std::move(rpSettings);
	m_pCommand = pCmd;
	m_pRequestHandler = std::make_unique<SVRCRequestHandler>(m_pCommand.get());
	m_pRpcServer = std::make_unique<SvRpc::RPCServer>(m_pRequestHandler.get());
	m_pSettings->pEventHandler = m_pRpcServer.get();
	m_pHttpserver = std::make_unique<SvHttp::HttpServer>(*m_pSettings.get(), m_io_service);
	m_pHttpserver->start();
	m_bIsRunning = true;
	m_io_service.restart();

	m_pThread.reset(new std::thread([this]()
	{
		m_io_service.run();
	}
	));
}

void SVRCWebsocketServer::Stop()
{
	if (!m_bIsRunning)
	{
		return;
	}

	m_bIsRunning = false;
	if(nullptr != m_pHttpserver.get())
	{
		m_pHttpserver->stop();
		m_io_service.stop();
	}

	if (m_pThread.get() && m_pThread->joinable())
	{
		m_pThread->join();
	}
	m_pHttpserver.reset();
	m_pRpcServer.reset();
	m_pRequestHandler.reset();
	m_pCommand.reset();
	m_pSettings.reset();
}
SVRCWebsocketServer::~SVRCWebsocketServer()
{
	//stop is called in int SVObserverApp::ExitInstance()
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