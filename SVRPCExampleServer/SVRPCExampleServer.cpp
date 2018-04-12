//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file SVRPCExampleServer.cpp
/// All Rights Reserved
//******************************************************************************
/// Server-side for the SVRPCLibrary example
//******************************************************************************

#include "stdafx.h"

#include <chrono>
#include <functional>
#include <sstream>
#include <thread>

#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>

#include "SVHttpLibrary/WebsocketServer.h"
#include "SVRPCExampleLibrary/format.h"
#include "SVRPCLibrary/RPCServer.h"
#include "SVRPCLibrary/RequestHandler.h"

using namespace SvHttp;
using namespace SvRpc;
using namespace SvRpc::Example;

static void counter_async(const boost::system::error_code& ec,
	std::shared_ptr<boost::asio::deadline_timer> timer,
	boost::asio::io_service& io_service,
	Observer<GetCounterStreamResponse> observer,
	ServerStreamContext::Ptr ctx,
	int i)
{
	if (ec)
	{
		SvPenv::Error error;
		error.set_message(ec.message());
		error.set_error_code(SvPenv::ErrorCode::InternalError);
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
	try
	{
		observer.onNext(std::move(resp));
	}
	catch (const ConnectionLostException&)
	{
		return;
	}

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
			([&io_service](GetCounterStreamRequest&& req, Observer<GetCounterStreamResponse> observer, ServerStreamContext::Ptr ctx)
		{
			counter_async({}, nullptr, io_service, observer, ctx, req.start());
		});

		auto rpcServer = std::make_unique<RPCServer>(&requestHandler);

		WebsocketServerSettings settings;
		settings.Port = 8080;
		auto server = std::make_unique<WebsocketServer>(settings, io_service, rpcServer.get());
		server->start();

		BOOST_LOG_TRIVIAL(info) << "Server running on ws://" << settings.Host << ":" << settings.Port << "/";

		auto thread = std::thread([&io_service]() { io_service.run(); });
		
		std::this_thread::sleep_for(std::chrono::seconds(5));
		
		server->stop();
		io_service.stop();
		thread.join();

		rpcServer.reset();
		server.reset();
	}
	catch (std::exception& e)
	{
		BOOST_LOG_TRIVIAL(error) << e.what();
	}
	system("pause");
	return 0;
}
