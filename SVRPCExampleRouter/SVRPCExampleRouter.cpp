//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file SVRPCExampleRouter.cpp
/// All Rights Reserved
//******************************************************************************
/// Example for the SVRPCLibrary's Router
//******************************************************************************

#include "stdafx.h"

#include <sstream>

#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>

#include "SVHttpLibrary/WebsocketServer.h"
#include "SVRPCExampleLibrary/format.h"
#include "SVRPCLibrary/Router.h"
#include "SVRPCLibrary/RPCClient.h"
#include "SVRPCLibrary/RPCServer.h"
#include "SVRPCLibrary/RequestHandler.h"

using namespace SvHttp;
using namespace SvRpc;
using namespace SvRpc::Example;

int main()
{
	try
	{
		// handler for all messages that will be handled by the router already
		RequestHandler requestHandler;
		requestHandler.registerRequestHandler<
			ApplicationMessages,
			ApplicationMessages::kHelloRouterReq,
			HelloRouterReq,
			HelloRouterRes>(
			[](HelloRouterReq&& req, Task<HelloRouterRes> task)
		{
			std::stringstream ss;
			ss << "Hello " << req.name() << " from RPC Router!";
			HelloRouterRes res;
			res.set_message(ss.str());
			task.finish(std::move(res));
		});

		WebsocketClientSettings clientSettings;
		clientSettings.Host = "127.0.0.1";
		clientSettings.Port = 8080;

		// first connect to upstream server
		RPCClient rpcClient(clientSettings);
		rpcClient.waitForConnect(boost::posix_time::seconds(6));

		// router will forward unhandled messages to upstream server via given client
		Router rpcRouter(rpcClient, &requestHandler);

		// the server (incl underlying websocket server) other clients can connect to
		RPCServer rpcServer(&requestHandler);
		WebsocketServerSettings serverSettings;
		serverSettings.Host = "127.0.0.1";
		serverSettings.Port = 8081;
		boost::asio::io_service io_service;
		WebsocketServer server(serverSettings, io_service, &rpcServer);
		server.start();

		BOOST_LOG_TRIVIAL(info) << "Router running on ws://" << serverSettings.Host << ":" << serverSettings.Port << "/";

		io_service.run();
	}
	catch (std::exception& e)
	{
		BOOST_LOG_TRIVIAL(error) << e.what();
	}
	system("pause");
	return 0;
}
