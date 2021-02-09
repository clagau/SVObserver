//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file Router.cpp
/// All Rights Reserved
//******************************************************************************
/// Helper class that automatically forwards all messages not handled by the
/// given RequestHandler to the given RPC Client or created client
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "Router.h"
#include "RPCClient.h"
#pragma endregion Includes

namespace SvRpc
{
static const boost::posix_time::seconds cConnectTimeout{2};

static bool conncet_client(SvRpc::RPCClient& client)
{
	if (client.isConnected())
	{
		return true;
	}

	client.waitForConnect(cConnectTimeout);

	return client.isConnected();
}

Router::Router(RPCClient& rClient, RequestHandler& rRequestHandler)
{
	rRequestHandler.registerDefaultRequestHandler([&rClient](SvPenv::Envelope&& Request, Task<SvPenv::Envelope> Task)
	{
		if (!conncet_client(rClient))
		{
			Task.error(SvStl::build_error(SvPenv::ErrorCode::badGateway, _T("No connection to SVObserver")));
			return;
		}

		rClient.request(std::move(Request), std::move(Task));
	});

	rRequestHandler.registerDefaultStreamHandler([&rClient](SvPenv::Envelope&& Request, Observer<SvPenv::Envelope> Observer, ServerStreamContext::Ptr pServerContext)
	{
		if (!conncet_client(rClient))
		{
			Observer.error(SvStl::build_error(SvPenv::ErrorCode::badGateway, _T("No connection to SVObserver")));
			return;
		}

		auto client_ctx = rClient.stream(std::move(Request), std::move(Observer));
		pServerContext->registerOnCancelHandler([client_ctx]() mutable
		{
			client_ctx.cancel();
		});
	});
}

} // namespace SvRpc
