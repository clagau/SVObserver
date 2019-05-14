//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file SVRPCStreamClient.cpp
/// All Rights Reserved
//******************************************************************************
/// Client-side for the SVRPCLibrary example
//******************************************************************************

#include "stdafx.h"

#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "SVLogLibrary/Logging.h"
#include "SVRPCStreamLibrary/format.h"
#include "SVRPCLibrary/RPCClient.h"
#include "SVRPCLibrary/SimpleClient.h"
#include "SVSystemLibrary/SVFuture.h"

using namespace SvPenv;
using namespace SvRpc;
using namespace SvRpc::Stream;
using namespace SvSyl;

class StreamClient
{
public:
	StreamClient(RPCClient& rpcClient)
		: streamClient(rpcClient)
		, resPromise(std::make_shared<SvSyl::SVPromise<int>>())
	{
	}
	SvSyl::SVFuture<int> run()
	{
		Observer<GetThrottledStreamResponse > observer(
			std::bind(&StreamClient::onNext, this, std::placeholders::_1),
			std::bind(&StreamClient::onFinish, this),
			std::bind(&StreamClient::onError, this, std::placeholders::_1)
		);

		GetThrottledStreamRequest req;
		req.set_count(100);
		context = streamClient.stream(std::move(req), observer);
		return resPromise->get_future();
	}

private:
	SvSyl::SVFuture<void> onNext(GetThrottledStreamResponse&& res)
	{
		BOOST_LOG_TRIVIAL(info) << "Received stream response: " << res.sequence();
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
		return SvSyl::SVFuture<void>::make_ready();
	}
	void onFinish()
	{
		resPromise->set_value(42);
	}
	void onError(const Error& err)
	{
		resPromise->set_exception(errorToExceptionPtr(err));
	}

private:
	SimpleClient<ApplicationMessages, GetThrottledStreamRequest, GetThrottledStreamResponse> streamClient;
	std::shared_ptr<SvSyl::SVPromise<int>> resPromise;
	ClientStreamContext context {nullptr};
};

int main()
{
	try
	{
		SvHttp::WebsocketClientSettings websocketClientSettings;
		websocketClientSettings.Host = "127.0.0.1";
		websocketClientSettings.Port = 8080;

		auto pRpcClient = std::make_unique<RPCClient>(websocketClientSettings);
		auto ConnectTimeout = boost::posix_time::seconds(6);
		pRpcClient->waitForConnect(ConnectTimeout);
		
		StreamClient streamClient(*pRpcClient);
		auto future = streamClient.run();
		auto res = future.get();

		BOOST_LOG_TRIVIAL(info) << "Finished with \"" << res << "\". Exiting.";
	}
	catch (std::exception& e)
	{
		SV_LOG_GLOBAL(error) << e.what();
	}
	catch (...)
	{
		SV_LOG_GLOBAL(error) << "Unhandeled exception";
	}
	system("pause");
	return 0;
}
