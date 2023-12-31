//******************************************************************************
/// \copyright (c) 2017,2018 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file WebsocketClientFactory.cpp
/// All Rights Reserved
//******************************************************************************
/// TODO
//******************************************************************************

#include "stdafx.h"
#include "WebsocketClientFactory.h"
#include "WebsocketClientSettings.h"

namespace SvHttp
{

WebsocketClientFactory::WebsocketClientFactory(WebsocketClientSettings& rSettings)
	: m_rSettings(rSettings)
{
}

std::shared_ptr<WebsocketClient> WebsocketClientFactory::create(
	WebsocketClient::EventHandler* pHandler)
{
	auto client = WebsocketClient::create(m_rSettings, pHandler);
	client->connect();
	return client;
}

} // namespace SvHttp
