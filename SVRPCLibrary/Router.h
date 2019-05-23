//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file Router.h
/// All Rights Reserved
//******************************************************************************
/// Helper class that automatically forwards all messages not handled by the
/// given RequestHandler to the given RPC Client or created client
//******************************************************************************

#pragma once

#pragma region Includes
#include "RequestHandler.h"
#include "RPCClient.h"
#include "RequestHandler.h"
#include "SVHttpLibrary/WebsocketClientSettings.h"
#pragma endregion Includes

namespace SvRpc
{
class Router
{
public:
	Router::Router(RPCClient& rClient, RequestHandler* pRequestHandler);
	Router::Router(const SvHttp::WebsocketClientSettings& rClientSettings, RequestHandler* pRequestHandler, std::function<void(ClientStatus)> StatusCallback);

private:
	bool Router::ConnectToRouter();
	SvHttp::WebsocketClientSettings m_Settings;
	std::unique_ptr<SvRpc::RPCClient> m_pClientRouter {nullptr};
	std::function<void(ClientStatus)> m_pStatusCallback {nullptr};
};

} // namespace SvRpc
