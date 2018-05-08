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
#pragma endregion Includes

namespace SvRpc
{
static const int cTwoSeconds = 2000;
//@Todo[MEC][8.00] [04.05.2018] ADD Router for StreamHandler
Router::Router(RPCClient& rClient, RequestHandler* pRequestHandler)
{
	if(nullptr != pRequestHandler)
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
}

Router::Router(const std::string& rServerAddress, unsigned short ServerPort, RequestHandler* pRequestHandler) :
	m_ServerAddress{rServerAddress},
	m_ServerPort{ServerPort}
{
	if (nullptr != pRequestHandler)
	{
		pRequestHandler->registerDefaultRequestHandler(std::bind(&Router::RouteRequest, this, std::placeholders::_1, std::placeholders::_2));
	}
}

void Router::RouteRequest(SvPenv::Envelope&& Request, SvRpc::Task<SvPenv::Envelope> Task)
{
	if (nullptr == m_pClientRouter)
	{
		m_pClientRouter = std::make_unique<SvRpc::RPCClient>(m_ServerAddress, m_ServerPort);
	}
	if (nullptr != m_pClientRouter)
	{
		if (!m_pClientRouter->isConnected())
		{
			m_pClientRouter->waitForConnect(cTwoSeconds);
		}
		if (m_pClientRouter->isConnected())
		{
			//This routes the request further to SVObserver
			m_pClientRouter->request(std::move(Request), std::move(Task));
		}
		else
		{
			Task.error(SvRpc::build_error(SvPenv::ErrorCode::BadGateway, _T("No connection to SVObsrver")));
		}
	}
	else
	{
		Task.error(SvRpc::build_error(SvPenv::ErrorCode::BadGateway, _T("No connection to SVObsrver")));
	}
}

} // namespace SvRpc
