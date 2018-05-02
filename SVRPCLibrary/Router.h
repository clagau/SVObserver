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
#pragma endregion Includes

namespace SvRpc
{
class Router
{
public:
	Router::Router(RPCClient& rClient, RequestHandler* pRequestHandler);
	Router::Router(const std::string& rServerAddress, unsigned short ServerPort, RequestHandler* pRequestHandler);

private:
	void RouteRequest(SvPenv::Envelope&&, SvRpc::Task<SvPenv::Envelope>);

	std::string m_ServerAddress;
	unsigned short m_ServerPort{0};
	std::unique_ptr<SvRpc::RPCClient> m_pClientRouter {nullptr};
};

} // namespace SvRpc
