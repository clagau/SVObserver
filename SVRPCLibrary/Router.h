//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file Router.h
/// All Rights Reserved
//******************************************************************************
/// Helper class that automatically forwards all messages not handled by the
/// given RequestHandler to the given RPC Client.
//******************************************************************************

#pragma once

#include "RequestHandler.h"
#include "RPCClient.h"
#include "RequestHandler.h"

namespace SvRpc
{
class Router
{
public:
	Router::Router(RPCClient& rClient, RequestHandler* pRequestHandler)
	{
		pRequestHandler->registerDefaultRequestHandler([&rClient](SvPenv::Envelope&& Request, Task<SvPenv::Envelope> Task)
		{
			rClient.request(std::move(Request), std::move(Task));
		});
		pRequestHandler->registerDefaultStreamHandler([&rClient](SvPenv::Envelope&& Request, Observer<SvPenv::Envelope> Observer, ServerStreamContext::Ptr pServerContext)
		{
			auto client_ctx = rClient.stream(std::move(Request), std::move(Observer));
			pServerContext->registerOnCancelHandler([client_ctx]() mutable
			{
				client_ctx.cancel();
			});
		});
	}
};

} // namespace SvRpc
