//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file WebsocketClientFactory.h
/// All Rights Reserved
//******************************************************************************
/// TODO
//******************************************************************************

#pragma once

#include <memory>

#include <boost/asio/io_service.hpp>

#include "SVHttpLibrary/WebSocketClient.h"

namespace SVHTTP
{
class WebsocketClientFactory
{
public:
	WebsocketClientFactory(std::string host, uint16_t port);

	std::unique_ptr<WebsocketClient> create(
		boost::asio::io_service& io_service,
		WebsocketClient::EventHandler*,
		bool wait_for_connect = false);

//private:
	std::string m_host;
	uint16_t m_port;
};
}
