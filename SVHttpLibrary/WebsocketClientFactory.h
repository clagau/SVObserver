//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file WebsocketClientFactory.h
/// All Rights Reserved
//******************************************************************************
/// TODO
//******************************************************************************

#pragma once

#include <memory>

#include "SvHttpLibrary/WebSocketClient.h"

namespace SvHttp
{
class WebsocketClientFactory
{
public:
	WebsocketClientFactory(std::string host, uint16_t port);

	std::shared_ptr<WebsocketClient> create(WebsocketClient::EventHandler*);

private:
	std::string m_Host;
	uint16_t m_Port;
};

} // namespace SvHttp
