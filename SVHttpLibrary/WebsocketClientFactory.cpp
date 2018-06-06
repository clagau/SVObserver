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
