//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file SVRPCExampleClient.cpp
/// All Rights Reserved
//******************************************************************************
/// Client-side for the SVRPCLibrary example
//******************************************************************************

#include "stdafx.h"

#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <boost/thread.hpp>

#include "SVRPCExampleLibrary/format.h"
#include "SVRPCLibrary/RPCClient.h"
#include "SVRPCLibrary/SimpleClient.h"

using namespace SvPenv;
using namespace SvRpc;
using namespace SvRpc::Example;

int main()
{
	try
	{
		auto Timeout = boost::posix_time::seconds(5);
		auto pRpcClient = std::make_unique<RPCClient>("127.0.0.1", 8081);
		pRpcClient->waitForConnect(6000);
		{
			HelloWorldReq req;
			req.set_name("Homer Simpson");
			SimpleClient<ApplicationMessages, HelloWorldReq, HelloWorldRes> client(*pRpcClient);
			auto res = client.request(std::move(req), Timeout).get();
			BOOST_LOG_TRIVIAL(info) << res.message();
		}
		
		/// Alternative
		{
			HelloWorldReq req;
			req.set_name("Marcus Eichengruen");
			auto promise = std::make_shared<std::promise<HelloWorldRes>>();
			auto FinishFkt = [&promise](HelloWorldRes&& res) { promise->set_value(res); };
			auto ErrorFkt = [&promise](const Error& err) { promise->set_exception(errorToExceptionPtr(err)); };
			auto task = Task<HelloWorldRes>(FinishFkt, ErrorFkt);
			SimpleClient<ApplicationMessages, HelloWorldReq, HelloWorldRes> client2(*pRpcClient);
			client2.request(std::move(req), task,Timeout);
			HelloWorldRes res2 = (promise->get_future()).get();
			BOOST_LOG_TRIVIAL(info) << res2.message();
		}
		/// Request answered by Router
		{
			HelloRouterReq req;
			req.set_name("Bart Simpson");
			SimpleClient<ApplicationMessages, HelloRouterReq, HelloRouterRes> client(*pRpcClient);
			auto res = client.request(std::move(req), Timeout).get();
			BOOST_LOG_TRIVIAL(info) << res.message();
		}
		/// Streaming
		{
			auto lastcounter = 0;
			auto CounterPromise = std::make_shared<std::promise<int>>();
			auto nextFkt = [&lastcounter](GetCounterStreamResponse&& resp)-> std::future<void>
			{
				lastcounter = resp.counter();
				BOOST_LOG_TRIVIAL(info) << resp.counter();
				return std::future<void>();
			};
			auto FinishFkt = [CounterPromise, &lastcounter]()
			{
				CounterPromise->set_value(lastcounter);
			};
			auto errorFkt = [CounterPromise, &lastcounter](const Error& err)
			{ 
				CounterPromise->set_exception(errorToExceptionPtr(err));
			};
			auto observer = Observer<GetCounterStreamResponse>(nextFkt, FinishFkt, errorFkt);

			SimpleClient<ApplicationMessages, GetCounterStreamRequest, GetCounterStreamResponse> streamClient(*pRpcClient);

			GetCounterStreamRequest getCounterStreamRequest;
			getCounterStreamRequest.set_start(0);
			auto ctx = streamClient.stream(std::move(getCounterStreamRequest), observer);
			std::this_thread::sleep_for(std::chrono::seconds(2));
			ctx.cancel();

			auto res = CounterPromise->get_future().get();
			BOOST_LOG_TRIVIAL(info) << "Finish last counter was " << res;
		}
	}
	catch (std::exception& e)
	{
		BOOST_LOG_TRIVIAL(error) << e.what();
	}
	system("pause");
	return 0;
}
