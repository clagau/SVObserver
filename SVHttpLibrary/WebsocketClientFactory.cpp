//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file WebsocketClientFactory.cpp
/// All Rights Reserved
//******************************************************************************
/// TODO
//******************************************************************************

#include "stdafx.h"

#include "SvHttpLibrary/WebsocketClientFactory.h"

namespace SvHttp
{

WebsocketClientFactory::WebsocketClientFactory(std::string host, uint16_t port)
	: m_Host(host), m_Port(port)
{
}

std::unique_ptr<WebsocketClient> WebsocketClientFactory::create(
	boost::asio::io_service& rIoService,
	WebsocketClient::EventHandler* pHandler,
	bool bWaitForConnect)
{
	auto client = std::make_unique<WebsocketClient>(rIoService, pHandler);
	auto future = client->connect(m_Host, m_Port);
	if (bWaitForConnect)
	{
		future.get();
	}
	return client;
}

} // namespace SvHttp
