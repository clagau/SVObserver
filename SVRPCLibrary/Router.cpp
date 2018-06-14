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
static const boost::posix_time::seconds cConnectTimeout{2};
Router::Router(RPCClient& rClient, RequestHandler* pRequestHandler)
{
	if (nullptr != pRequestHandler)
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


Router::Router(const std::string& rServerAddress, unsigned short ServerPort, RequestHandler* pRequestHandler)
{
	m_Settings.Host = rServerAddress;
	m_Settings.Port = ServerPort;
	if (nullptr != pRequestHandler)
	{
		pRequestHandler->registerDefaultRequestHandler([this](SvPenv::Envelope&& Request, Task<SvPenv::Envelope> Task)
		{
			if (ConnectToRouter())
			{
				m_pClientRouter->request(std::move(Request), std::move(Task));
			}
			else
			{
				Task.error(SvRpc::build_error(SvPenv::ErrorCode::badGateway, _T("No connection to SVObserver")));
			}

		});

		pRequestHandler->registerDefaultStreamHandler([this](SvPenv::Envelope&& Request, Observer<SvPenv::Envelope> Observer, ServerStreamContext::Ptr pServerContext)
		{
			try
			{
				if (ConnectToRouter())
				{
					auto client_ctx = m_pClientRouter->stream(std::move(Request), std::move(Observer));
					pServerContext->registerOnCancelHandler([client_ctx]() mutable
					{
						client_ctx.cancel();
					});
				}
				else
				{
					Observer.error(SvRpc::build_error(SvPenv::ErrorCode::badGateway, _T("No connection to SVObserver")));
				}
			}
			catch (const std::exception&)
			{
			}
		});
	}
}



bool Router::ConnectToRouter()
{
	if (m_pClientRouter && m_pClientRouter->isConnected())
	{
		return true;
	}
	if (nullptr == m_pClientRouter)
	{
		m_pClientRouter = std::make_unique<SvRpc::RPCClient>(m_Settings);
	}
	if (!m_pClientRouter->isConnected())
	{
		m_pClientRouter->waitForConnect(cConnectTimeout);
	}
	if (m_pClientRouter->isConnected())
	{
		return true;
	}
	else
	{
		m_pClientRouter.reset();
		return false;
	}
}




} // namespace SvRpc
