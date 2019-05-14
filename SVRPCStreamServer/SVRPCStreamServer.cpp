//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file SVRPCStreamServer.cpp
/// All Rights Reserved
//******************************************************************************
/// Server-side for the SVRPCLibrary example
//******************************************************************************

#include "stdafx.h"

#include <chrono>
#include <functional>
#include <map>
#include <sstream>
#include <thread>

#include <boost/asio.hpp>

#include "SVAuthLibrary/AuthManager.h"
#include "SVAuthLibrary/RestHandler.h"
#include "SVHttpLibrary/HttpServer.h"
#include "SvHttpLibrary/HttpRequest.h"
#include "SvHttpLibrary/HttpResponse.h"
#include "SVLogLibrary/Logging.h"
#include "SVOGateway/SettingsLoader.h"
#include "SVProtoBuf/SVRC.h"
#include "SVRPCStreamLibrary/format.h"
#include "SVRPCLibrary/RPCServer.h"
#include "SVRPCLibrary/Router.h"
#include "SVRPCLibrary/RequestHandler.h"

using namespace SvAuth;
using namespace SvHttp;
using namespace SvPb;
using namespace SvRpc;
using namespace SvRpc::Stream;

bool on_http_request(RestHandler& rRestHandler, const SvHttp::HttpRequest& req, SvHttp::HttpResponse& res)
{
	if (rRestHandler.onRestRequest(req, res))
	{
		return true;
	}

	return false;
}

struct StreamSender : public std::enable_shared_from_this<StreamSender>
{
	boost::asio::io_context& m_io_context;
	Observer<GetThrottledStreamResponse> m_observer;
	std::shared_ptr<boost::asio::deadline_timer> m_timer;
	ServerStreamContext::Ptr m_ctx;
	size_t curr_count = 0;
	size_t max_count;

	StreamSender(boost::asio::io_context& io_context, Observer<GetThrottledStreamResponse> observer, ServerStreamContext::Ptr ctx, size_t max_count)
		: m_io_context(io_context), m_observer(observer), m_ctx(ctx), max_count(max_count)
	{
		ctx->enableThrottling();
		ctx->setMaxPendingAcks(2);
	}
	void schedule_timer()
	{
		m_timer = std::make_shared<boost::asio::deadline_timer>(m_io_context);
		m_timer->expires_from_now(boost::posix_time::milliseconds(100));
		m_timer->async_wait(std::bind(&StreamSender::on_timer, shared_from_this(), std::placeholders::_1));
	}
	void on_timer(boost::system::error_code ec)
	{
		if (ec)
		{
			SV_LOG_GLOBAL(error) << "NotificationSender error: " << ec;
			return;
		}

		if (m_ctx->isCancelled())
		{
			SV_LOG_GLOBAL(error) << "NotificationSender client disconnected. stopping notifications.";
			return;
		}

		if (curr_count >= max_count)
		{
			m_observer.finish();
			return;
		}

		auto x = ++curr_count;

		GetThrottledStreamResponse res;
		res.set_sequence(curr_count);
		m_observer.onNext(std::move(res)).then(m_io_context, [x](auto f)
		{
			try
			{
				f.get();
				BOOST_LOG_TRIVIAL(info) << "Client acknowledged the message " << x;
			}
			catch (...)
			{
				BOOST_LOG_TRIVIAL(info) << "Client is overloaded " << x;
			}
		});

		schedule_timer();
	}
	void send_response()
	{
		auto self = shared_from_this();

		if (m_ctx->isCancelled())
		{
			SV_LOG_GLOBAL(error) << "StreamSender client disconnected. stopping notifications.";
			return;
		}

		if (curr_count >= max_count)
		{
			m_observer.finish();
			return;
		}

		++curr_count;

		GetThrottledStreamResponse res;
		auto future = m_observer.onNext(std::move(res));
		future.then(m_io_context, [this, self](auto f)
		{
			send_response();
		});
		
	}
};

void register_stream_handler(RequestHandler& requestHandler, boost::asio::io_context& io_context)
{
	requestHandler.registerStreamHandler<
		ApplicationMessages,
		ApplicationMessages::kGetThrottledStreamRequest,
		GetThrottledStreamRequest,
		GetThrottledStreamResponse>(
		([&io_context](GetThrottledStreamRequest&& req, Observer<GetThrottledStreamResponse> observer, ServerStreamContext::Ptr ctx)
	{
		auto sender = std::make_shared<StreamSender>(io_context, observer, ctx, req.count());
		sender->schedule_timer();
	}));
}

int main()
{
	try
	{
		boost::asio::io_context io_context {1};

		SvOgw::Settings settings;
		SvOgw::SettingsLoader settingsLoader;
		settingsLoader.loadFromIni(settings);

		RequestHandler requestHandler;
		register_stream_handler(requestHandler, io_context);
		auto rpcServer = std::make_unique<RPCServer>(&requestHandler);

		SvHttp::HttpServerSettings httpServerSettings;
		httpServerSettings.pEventHandler = rpcServer.get();
		httpServerSettings.Host = "0.0.0.0";
		httpServerSettings.Port = 8080;

		auto httpServer = std::make_unique<HttpServer>(httpServerSettings, io_context);
		httpServer->start();

		SV_LOG_GLOBAL(info) << "Server running on ws://" << httpServerSettings.Host << ":" << httpServerSettings.Port << "/";

		auto thread = std::thread([&io_context]() { io_context.run(); });

		char c;
		std::cout << "Press any key to quit: ";
		std::cin >> c;

		httpServer->stop();
		io_context.stop();
		thread.join();

		rpcServer.reset();
		httpServer.reset();
	}
	catch (std::exception& e)
	{
		SV_LOG_GLOBAL(error) << e.what();
	}
	system("pause");
	return 0;
}
