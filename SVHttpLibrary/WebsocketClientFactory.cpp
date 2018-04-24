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

std::shared_ptr<WebsocketClient> WebsocketClientFactory::create(
	WebsocketClient::EventHandler* pHandler)
{
	auto client = WebsocketClient::create(pHandler);
	client->connect(m_Host, m_Port);
	return client;
}

} // namespace SvHttp
