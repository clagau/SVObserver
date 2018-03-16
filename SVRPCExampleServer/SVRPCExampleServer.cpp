//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file SVRPCExampleServer.cpp
/// All Rights Reserved
//******************************************************************************
/// Server-side for the SVRPCLibrary example
//******************************************************************************

#include "stdafx.h"

#include <chrono>
#include <sstream>
#include <thread>

#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>

#include "SVHttpLibrary/WebsocketServer.h"
#include "SVRPCExampleLibrary/format.h"
#include "SVRPCLibrary/RPCServer.h"
#include "SVRPCLibrary/RequestHandler.h"

using namespace SVHTTP;
using namespace SVRPC;
using namespace SVRPC::Example;

static void counter_async(const boost::system::error_code& ec,
	std::shared_ptr<boost::asio::deadline_timer> timer,
	boost::asio::io_service& io_service,
	SVRPC::Observer<GetCounterStreamResponse> observer,
	ServerStreamContext::Ptr ctx,
	int i)
{
	if (ec)
	{
		SVRPC::Error error;
		error.set_message(ec.message());
		error.set_error_code(SVRPC::ErrorCode::InternalError);
		observer.error(error);
		return;
	}
	if (ctx->isCancelled())
	{
		observer.finish();
		return;
	}

	GetCounterStreamResponse resp;
	resp.set_counter(i);
	observer.onNext(std::move(resp));

	timer = std::make_shared<boost::asio::deadline_timer>(io_service);
	timer->expires_from_now(boost::posix_time::milliseconds(100));
	timer->async_wait(std::bind(counter_async, std::placeholders::_1, timer, std::ref(io_service), observer, ctx, ++i));
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

		requestHandler.registerStreamHandler<
			ApplicationMessages,
			ApplicationMessages::kGetCounterStreamRequest,
			GetCounterStreamRequest,
			GetCounterStreamResponse>
			([&io_service](GetCounterStreamRequest&& req, SVRPC::Observer<GetCounterStreamResponse> observer, ServerStreamContext::Ptr ctx)
		{
			counter_async({}, nullptr, io_service, observer, ctx, req.start());
		});

		RPCServer rpcServer(&requestHandler);
		WebsocketServerSettings settings;
		settings.Port = 8080;
		WebsocketServer server(settings, io_service, &rpcServer);
		server.start();

		BOOST_LOG_TRIVIAL(info) << "Server running on ws://" << settings.Host << ":" << settings.Port << "/";

		io_service.run();
	}
	catch (std::exception& e)
	{
		BOOST_LOG_TRIVIAL(error) << e.what();
	}
	system("pause");
	return 0;
}
