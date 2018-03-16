//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file Router.h
/// All Rights Reserved
//******************************************************************************
/// Helper class that automatically forwards all messages not handled by the
/// given RequestHandler to the given RPC Client.
//******************************************************************************

#pragma once

#include "SVRPCLibrary/RequestHandler.h"
#include "SVRPCLibrary/RPCClient.h"
#include "SVRPCLibrary/RequestHandler.h"

namespace SVRPC
{
class Router
{
public:
	Router::Router(RPCClient& rClient, RequestHandler* pRequestHandler)
	{
		pRequestHandler->registerDefaultRequestHandler([&rClient](Envelope&& req, Task<Envelope> task)
		{
			rClient.request(std::move(req), std::move(task));
		});
		pRequestHandler->registerDefaultStreamHandler([&rClient](Envelope&& req, Observer<Envelope> observer, ServerStreamContext::Ptr server_ctx)
		{
			auto client_ctx = rClient.stream(std::move(req), std::move(observer));
			server_ctx->registerOnCancelHandler([client_ctx]() mutable
			{
				client_ctx.cancel();
			});
		});
	}
};

} // namespace SVRPC
