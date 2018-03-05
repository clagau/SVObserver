//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file SVRPCExampleServer.cpp
/// All Rights Reserved
//******************************************************************************
/// Server-side for the SVRPCLibrary example
//******************************************************************************

#include "stdafx.h"

#include <sstream>

#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>

#include "SVHttpLibrary/WebsocketServer.h"
#include "SVRPCExampleServer/format.pb.h"
#include "SVRPCLibrary/RPCServer.h"
#include "SVRPCLibrary/RequestHandler.h"

using namespace SVHTTP;
using namespace SVRPC;
using namespace SVRPC::Example;



void GetCounterStreamRequestFkt(GetCounterStreamRequest&& Request, SVRPC::Observer<GetCounterStreamResponse> observer)
{
	for (int i = Request.start(); i < Request.count(); i++)
	{
		
		GetCounterStreamResponse resp;
		resp.set_counter(i);
		///ToDO 
		observer.onNext(std::move(resp));
	}
	
	observer.finish();
}

int main()
{
	try
	{
		boost::asio::io_service io_service {1};
		RequestHandler requestHandler;
		
		requestHandler.registerRequestHandler<
			ApplicationMessages,
			ApplicationMessages::kHelloWorldReq,
			HelloWorldReq,
			HelloWorldRes>(
			[](HelloWorldReq&& req, Task<HelloWorldRes> task)
		{
			std::stringstream ss;
			ss << "Hello " << req.name() << "!";
			HelloWorldRes res;
			res.set_message(ss.str());
			task.finish(std::move(res));
		});

		auto fkt = &GetCounterStreamRequestFkt;
		requestHandler.registerStreamHandler<
			ApplicationMessages,
			ApplicationMessages::kGetCounterStreamRequest,
			GetCounterStreamRequest,
			GetCounterStreamResponse>
			([&fkt, &io_service ](GetCounterStreamRequest&& req, SVRPC::Observer<GetCounterStreamResponse> observer)
		{
			
			fkt(std::move(req), observer);
			//std::function<void(void)> f = std::bind(*fkt, req, observer);
			//auto f = boost::bind(*fkt, boost::ref(req), boost::ref(observer));
			//io_service.post(f);
		}
		);
			


		RPCServer rpcServer(&requestHandler);

		WebsocketServerSettings settings;
		
		WebsocketServer server(settings, io_service, &rpcServer);
		server.start();

		BOOST_LOG_TRIVIAL(info) << "Server running on ws://" << settings.host << ":" << settings.port << "/";

		io_service.run();
	}
	catch (std::exception& e)
	{
		BOOST_LOG_TRIVIAL(error) << e.what();
	}
	system("pause");
	return 0;
}
