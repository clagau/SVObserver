//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file WebsocketClientFactory.cpp
/// All Rights Reserved
//******************************************************************************
/// TODO
//******************************************************************************

#include "stdafx.h"

#include "SVHttpLibrary/WebsocketClientFactory.h"

namespace SVHTTP
{

WebsocketClientFactory::WebsocketClientFactory(std::string host, uint16_t port)
	: m_host(host), m_port(port)
{
}

std::unique_ptr<WebsocketClient> WebsocketClientFactory::create(
	boost::asio::io_service& io_service,
	WebsocketClient::EventHandler* handler,
	bool wait_for_connect)
{
	auto client = std::make_unique<WebsocketClient>(io_service, handler);
	auto future = client->connect(m_host, m_port);
	if (wait_for_connect)
	{
		future.get();
	}
	return client;
}
}
