//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file WebsocketClientFactory.h
/// All Rights Reserved
//******************************************************************************
/// TODO
//******************************************************************************

#pragma once

#include <memory>

#include "WebSocketClient.h"

namespace SvHttp
{
struct WebsocketClientSettings;

class WebsocketClientFactory
{
public:
	WebsocketClientFactory(WebsocketClientSettings&);

	std::shared_ptr<WebsocketClient> create(WebsocketClient::EventHandler*);

private:
	WebsocketClientSettings& m_rSettings;
};

} // namespace SvHttp
