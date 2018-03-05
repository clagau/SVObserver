//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file SVRPCExampleClient.cpp
/// All Rights Reserved
//******************************************************************************
/// Client-side for the SVRPCLibrary example
//******************************************************************************

#include "stdafx.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <boost/thread.hpp>

#include "SVHttpLibrary/WebsocketClient.h"
#include "SVRPCExampleServer/format.pb.h"
#include "SVRPCLibrary/RPCClient.h"
#include "SVRPCLibrary/SimpleClient.h"

using namespace SVRPC;
using namespace SVRPC::Example;

int main()
{
	try
	{
		//SVHTTP::WebsocketClientFactory wsClientFactory("127.0.0.1", 8080);
		auto Timeout = boost::posix_time::seconds(5);
		SVRPC::RPCClient rpcClient("127.0.0.1", 8080);
		rpcClient.Connect("127.0.0.1", 8080);
		rpcClient.waitForConnect(6000);
		{
			HelloWorldReq req;
			req.set_name("Homer Simpson");
			SVRPC::SimpleClient<ApplicationMessages, HelloWorldReq, HelloWorldRes> client(rpcClient);
			auto res = client.request(std::move(req), Timeout).get();
			BOOST_LOG_TRIVIAL(info) << res.message();
		}
		
		//////Alternative
		{
			HelloWorldReq req;
			req.set_name("Marcus Eichengruen");
			auto promise = std::make_shared<std::promise<HelloWorldRes>>();
			auto FinishFkt = [&promise](HelloWorldRes&& res) { promise->set_value(res); };
			auto ErrorFkt = [&promise](const SVRPC::Error& err) { promise->set_exception(SVRPC::errorToExceptionPtr(err)); };
			auto task = SVRPC::Task<HelloWorldRes>(FinishFkt, ErrorFkt);
			SimpleClient<ApplicationMessages, HelloWorldReq, HelloWorldRes> client2(rpcClient);
			client2.request(std::move(req), task,Timeout);
			HelloWorldRes res2 = (promise->get_future()).get();
			BOOST_LOG_TRIVIAL(info) << res2.message();
		}
		//////Streaming ???
		{
			int lastcounter {0};
			GetCounterStreamRequest getcounterStreamRequest;
			getcounterStreamRequest.set_count(500);
			getcounterStreamRequest.set_start(0);
			SVRPC::SimpleClient<ApplicationMessages, GetCounterStreamRequest, GetCounterStreamResponse> streamClient(rpcClient);
			auto CounterPromise = std::make_shared<std::promise<GetCounterStreamResponse>>();
			auto nextFkt = [&lastcounter](GetCounterStreamResponse&& resp)-> std::future<void>
			{
				lastcounter = resp.counter();
				BOOST_LOG_TRIVIAL(info) << resp.counter();
				//::Sleep(100);
				return std::future<void>();
			};
			auto FinishFkt= [CounterPromise, lastcounter]()
			{
				GetCounterStreamResponse resp;
				BOOST_LOG_TRIVIAL(info) << "Finish last counter was "  << lastcounter; 
				resp.set_counter(lastcounter);
				CounterPromise->set_value(resp);
			};
			auto errorFkt = [CounterPromise](const SVRPC::Error& err)
			{ 
				CounterPromise->set_exception(SVRPC::errorToExceptionPtr(err));
			};


			auto observer = SVRPC::Observer<GetCounterStreamResponse>(nextFkt, FinishFkt, errorFkt);
			streamClient.stream(std::move(getcounterStreamRequest), observer);
			GetCounterStreamResponse resp = CounterPromise->get_future().get();
		}
	
	}
	catch (std::exception& e)
	{
		BOOST_LOG_TRIVIAL(error) << e.what();
	}
	system("pause");
	return 0;
}
