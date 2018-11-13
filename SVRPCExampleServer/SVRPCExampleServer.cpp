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
#include "SVRPCExampleLibrary/format.h"
#include "SVRPCExampleServer/bitmap_image.hpp"
#include "SVRPCLibrary/RPCServer.h"
#include "SVRPCLibrary/RequestHandler.h"

using namespace SvAuth;
using namespace SvHttp;
using namespace SvPb;
using namespace SvRpc;
using namespace SvRpc::Example;

static std::atomic_bool s_restartServer = false;
static DeviceModeType s_deviceMode = DeviceModeType::runMode;

void register_auth_handler(RequestHandler& requestHandler, AuthManager* am)
{
	requestHandler.registerAuthHandler([am](const std::string& token) -> bool
	{
		return am->rpcAuth(token);
	});
}

bool on_http_request(RestHandler& rRestHandler, const SvHttp::HttpRequest& req, SvHttp::HttpResponse& res)
{
	if (rRestHandler.onRestRequest(req, res))
	{
		return true;
	}

	return false;
}

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
		error.set_errorcode(SvPenv::ErrorCode::internalError);
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

void register_example_handler(RequestHandler& requestHandler, boost::asio::io_service& io_service)
{
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
}

static void createImageOfSize(std::string& data, unsigned int width, unsigned int height)
{
	bitmap_image fractal(width, height);
	fractal.clear();

	static int iter = 0;
	int curr_iter = ++iter;
	int red_mult = curr_iter % 3 > 0 ? 1 : 0;
	int green_mult = (curr_iter + 1) % 3 > 0 ? 1 : 0;
	int blue_mult = (curr_iter + 2) % 3 > 0 ? 1 : 0;

	for (unsigned int y = 0; y < height; ++y)
	{
		for (unsigned int x = 0; x < width; ++x)
		{
			const unsigned char red = red_mult * std::round(255 * x / (1.0 * width));
			const unsigned char green = green_mult * std::round(255 * x / (1.0 * width));
			const unsigned char blue = blue_mult * std::round(255 * x / (1.0 * width));
			fractal.set_pixel(x, y, red, green, blue);
		}
	}

	std::ostringstream ss;
	fractal.save_image(ss);
	data = ss.str();
}

static void getImageForId(Image& img, const ImageId& id)
{
	if (id.imageindex() == 0)
	{
		img.set_height(0);
		img.set_width(0);
	}
	if (id.imageindex() == 100)
	{
		img.set_height(1);
		img.set_width(1);
		createImageOfSize(*img.mutable_rgbdata(), 100, 100);
	}
	if (id.imageindex() == 200)
	{
		img.set_height(200);
		img.set_width(200);
		createImageOfSize(*img.mutable_rgbdata(), 128, 128);
	}
	if (id.imageindex() == 300)
	{
		img.set_height(300);
		img.set_width(300);
		createImageOfSize(*img.mutable_rgbdata(), 300, 300);
	}
}

static void addValueToProduct(Product& prod, const std::string& listname, bool withValueNames = true)
{
	std::string inspection = (listname == "monitorlist1") ? "inspection1" : "inspection2";
	if (withValueNames)
	{
		*prod.add_valuenames() = inspection + ".weight";
	}
	auto& weight = *prod.add_values();
	weight.set_type(Variant::DataCase::kFltVal);
	static int v = 0;
	weight.set_fltval(++v + 0.23f);
	if (withValueNames)
	{
		*prod.add_valuenames() = inspection + ".place";
	}
	auto& place = *prod.add_values();
	place.set_type(Variant::DataCase::kStrVal);
	place.set_strval("Munich");
}

static void register_dummy_handler(RequestHandler& requestHandler)
{
	requestHandler.registerRequestHandler<
		SVRCMessages,
		SVRCMessages::kGetGatewayVersionRequest,
		GetGatewayVersionRequest,
		GetVersionResponse>(
		[](GetGatewayVersionRequest&& req, Task<GetVersionResponse> task)
	{
		SV_LOG_GLOBAL(info) << "GetGatewayVersionRequest";
		GetVersionResponse res;
		res.set_version("1.0.0");
		task.finish(std::move(res));
	});
	requestHandler.registerRequestHandler<
		SVRCMessages,
		SVRCMessages::kGetSVObserverVersionRequest,
		GetSVObserverVersionRequest,
		GetVersionResponse>(
		[](GetSVObserverVersionRequest&& req, Task<GetVersionResponse> task)
	{
		SV_LOG_GLOBAL(info) << "GetSVObserverVersionRequest";
		GetVersionResponse res;
		res.set_version("1.0.0");
		task.finish(std::move(res));
	});
	requestHandler.registerRequestHandler<
		SVRCMessages,
		SVRCMessages::kSetDeviceModeRequest,
		SetDeviceModeRequest,
		StandardResponse>(
		[](SetDeviceModeRequest&& req, Task<StandardResponse> task)
	{
		SV_LOG_GLOBAL(info) << "SetDeviceModeRequest";
		s_deviceMode = req.mode();
		StandardResponse res;
		task.finish(std::move(res));
		s_restartServer = true;
	});
	requestHandler.registerRequestHandler<
		SVRCMessages,
		SVRCMessages::kGetDeviceModeRequest,
		GetDeviceModeRequest,
		GetDeviceModeResponse>(
		[](GetDeviceModeRequest&& req, Task<GetDeviceModeResponse> task)
	{
		SV_LOG_GLOBAL(info) << "GetDeviceModeRequest";
		GetDeviceModeResponse res;
		res.set_mode(s_deviceMode);
		task.finish(std::move(res));
	});
	requestHandler.registerRequestHandler<
		SVRCMessages,
		SVRCMessages::kQueryListNameRequest,
		QueryListNameRequest,
		QueryListNameResponse>(
		[](QueryListNameRequest&& req, Task<QueryListNameResponse> task)
	{
		SV_LOG_GLOBAL(info) << "QueryListNameRequest";
		QueryListNameResponse res;
		res.add_listname("monitorlist1");
		res.add_listname("monitorlist2");
		task.finish(std::move(res));
	});
	requestHandler.registerRequestHandler<
		SVRCMessages,
		SVRCMessages::kQueryMonitorListNamesRequest,
		QueryMonitorListNamesRequest,
		NamesResponse>(
		[](QueryMonitorListNamesRequest&& req, Task<NamesResponse> task)
	{
		SV_LOG_GLOBAL(info) << "QueryMonitorListNamesRequest";
		NamesResponse res;
		res.mutable_names()->mutable_item()->set_strval("monitorlist1;monitorlist2");
		res.mutable_names()->set_count(2);
		task.finish(std::move(res));
	});
	requestHandler.registerRequestHandler<
		SVRCMessages,
		SVRCMessages::kGetInspectionNamesRequest,
		GetInspectionNamesRequest,
		NamesResponse>(
		[](GetInspectionNamesRequest&& req, Task<NamesResponse> task)
	{
		SV_LOG_GLOBAL(info) << "GetInspectionNamesRequest";
		NamesResponse res;
		res.mutable_names()->mutable_item()->set_strval("inspection1;inspection2");
		res.mutable_names()->set_count(2);
		task.finish(std::move(res));
	});
	requestHandler.registerRequestHandler<
		SVRCMessages,
		SVRCMessages::kQueryListItemRequest,
		QueryListItemRequest,
		QueryListItemResponse>(
		[](QueryListItemRequest&& req, Task<QueryListItemResponse> task)
	{
		SV_LOG_GLOBAL(info) << "QueryListItemRequest";
		auto inspection = (req.listname() == "monitorlist1")
			? std::string("inspection1")
			: std::string("inspection2");
		QueryListItemResponse res;
		if (req.queryimages())
		{
			res.add_imagedeflist()->set_name(inspection + ".image");
		}
		if (req.queryvalues())
		{
			res.add_valuedeflist()->set_name(inspection + ".weight");
			res.add_valuedeflist()->set_name(inspection + ".place");
		}
		task.finish(std::move(res));
	});
	requestHandler.registerRequestHandler<
		SVRCMessages,
		SVRCMessages::kGetProductRequest,
		GetProductRequest,
		GetProductResponse>(
		[](GetProductRequest&& req, Task<GetProductResponse> task)
	{
		SV_LOG_GLOBAL(info) << "GetProductRequest";
		GetProductResponse res;
		Product& prod = *res.mutable_productitem();
		prod.set_trigger(0);
		prod.set_status(State::isValid);
		addValueToProduct(prod, req.listname(), req.nameinresponse());
		ImageId& imgId200px = *prod.add_images();
		imgId200px.set_imageindex(200);
		imgId200px.set_imagestore(200);
		imgId200px.set_slotindex(200);
		prod.add_imagenames("200px");
		ImageId& imgId300px = *prod.add_images();
		imgId300px.set_imageindex(300);
		imgId300px.set_imagestore(300);
		imgId300px.set_slotindex(300);
		prod.add_imagenames("300px");
		task.finish(std::move(res));
	});
	requestHandler.registerRequestHandler<
		SVRCMessages,
		SVRCMessages::kGetRejectRequest,
		GetRejectRequest,
		GetRejectResponse>(
		[](GetRejectRequest&& req, Task<GetRejectResponse> task)
	{
		SV_LOG_GLOBAL(info) << "GetRejectRequest";
		GetRejectResponse res;
		Product& prod = *res.mutable_productitem();
		static uint32_t trigger_count = 0;
		prod.set_trigger(++trigger_count / 4);
		prod.set_status(State::isValid);
		addValueToProduct(prod, req.listname(), req.nameinresponse());
		prod.add_imagenames("rejected image");
		ImageId& imgId = *prod.add_images();
		imgId.set_imageindex(300);
		imgId.set_imagestore(300);
		imgId.set_slotindex(300);
		task.finish(std::move(res));
	});
	requestHandler.registerRequestHandler<
		SVRCMessages,
		SVRCMessages::kGetImageFromIdRequest,
		GetImageFromIdRequest,
		GetImageFromIdResponse>(
		[](GetImageFromIdRequest&& req, Task<GetImageFromIdResponse> task)
	{
		SV_LOG_GLOBAL(info) << "GetImageFromIdRequest";
		GetImageFromIdResponse res;
		auto& image = *res.mutable_imagedata();
		getImageForId(image, req.id());
		task.finish(std::move(res));
	});
	requestHandler.registerRequestHandler<
		SVRCMessages,
		SVRCMessages::kGetFileRequest,
		GetFileRequest,
		GetFileResponse>(
		[](GetFileRequest&& req, Task<GetFileResponse> task)
	{
		SV_LOG_GLOBAL(info) << "GetFileRequest";
		GetFileResponse res;
		res.set_hresult(1);
		task.finish(std::move(res));
	});
	requestHandler.registerStreamHandler<
		SVRCMessages,
		SVRCMessages::kGetImageStreamFromIdRequest,
		GetImageStreamFromIdRequest,
		GetImageStreamFromIdResponse>(
		[](GetImageStreamFromIdRequest&& req, Observer<GetImageStreamFromIdResponse> observer, std::shared_ptr<ServerStreamContext> ctx)
	{
		SV_LOG_GLOBAL(info) << "GetImageStreamFromIdRequest";
		for (auto i = 0u; i < req.count() && !ctx->isCancelled(); ++i)
		{
			GetImageStreamFromIdResponse res;
			auto& image = *res.mutable_imagedata();
			getImageForId(image, req.id());
			auto future = observer.onNext(std::move(res));
			future.get();
		}
		observer.finish();
	});
	requestHandler.registerStreamHandler<
		SVRCMessages,
		SVRCMessages::kGetNotificationStreamRequest,
		GetNotificationStreamRequest,
		GetNotificationStreamResponse>(
		[](GetNotificationStreamRequest&& req, Observer<GetNotificationStreamResponse> observer, std::shared_ptr<ServerStreamContext> ctx)
	{
		SV_LOG_GLOBAL(info) << "GetNotificationStreamRequest";
		observer.finish();
	});
}

static boost::log::trivial::severity_level
map_to_boost_log_severity(const LogSeverity& severity)
{
	switch (severity)
	{
		case LogSeverity::fatal:
			return boost::log::trivial::fatal;
		case LogSeverity::error:
			return boost::log::trivial::error;
		case LogSeverity::warning:
			return boost::log::trivial::warning;
		case LogSeverity::info:
			return boost::log::trivial::info;
		case LogSeverity::debug:
			return boost::log::trivial::debug;
		case LogSeverity::trace:
			return boost::log::trivial::trace;
		default:
			return boost::log::trivial::error;
	}
}

static void store_log_entry(const std::string& client, const LogEntry& entry)
{
	const auto severity = map_to_boost_log_severity(entry.severity());
	BOOST_LOG_STREAM_WITH_PARAMS(
		::boost::log::trivial::logger::get(),
		(::boost::log::keywords::severity = severity)
	) << "[" << client << "] " << entry.message();
}

static void register_log_handler(RequestHandler& requestHandler)
{
	requestHandler.registerRequestHandler<
		SVRCMessages,
		SVRCMessages::kStoreClientLogsRequest,
		StoreClientLogsRequest,
		EmptyResponse>(
		[](StoreClientLogsRequest&& req, Task<EmptyResponse> task)
	{
		SV_LOG_GLOBAL(info) << "StoreClientLogsRequest";
		const auto& client = req.client();
		for (const auto& logEntry : req.logs())
		{
			store_log_entry(client, logEntry);
		}
		task.finish(EmptyResponse());
	});
}

int main()
{
	try
	{
		boost::asio::io_service io_service {1};

		SvOgw::Settings settings;
		SvOgw::SettingsLoader settingsLoader;
		settingsLoader.loadFromIni(settings);

		AuthManager authManager(settings.authSettings);
		RestHandler restHandler(authManager);

		RequestHandler requestHandler;
		register_auth_handler(requestHandler, &authManager);
		register_example_handler(requestHandler, io_service);
		register_dummy_handler(requestHandler);
		register_log_handler(requestHandler);

		auto rpcServer = std::make_unique<RPCServer>(&requestHandler);

		auto& httpSettings = settings.httpSettings;
		httpSettings.pEventHandler = rpcServer.get();
		httpSettings.DataDir = std::experimental::filesystem::path(".") / ".." / ".." / "seidenader-prototype" / "frontend" / "dist";
		httpSettings.HttpRequestHandler = std::bind(&on_http_request, std::ref(restHandler), std::placeholders::_1, std::placeholders::_2);
		auto server = std::make_unique<HttpServer>(httpSettings, io_service);
		server->start();

		SV_LOG_GLOBAL(info) << "Server running on ws://" << httpSettings.Host << ":" << httpSettings.Port << "/";

		auto thread = std::thread([&io_service]() { io_service.run(); });

		while (true)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			if (s_restartServer)
			{
				SV_LOG_GLOBAL(warning) << "Stopping current http server!";
				server->stop();
				server.reset();

				std::this_thread::sleep_for(std::chrono::seconds(3));
				SV_LOG_GLOBAL(warning) << "Starting new http server!";
				server = std::make_unique<HttpServer>(httpSettings, io_service);
				server->start();
				SV_LOG_GLOBAL(warning) << "Server running on ws://" << httpSettings.Host << ":" << httpSettings.Port << "/";
				
				s_restartServer = false;
			}
		}

		server->stop();
		io_service.stop();
		thread.join();

		rpcServer.reset();
		server.reset();
	}
	catch (std::exception& e)
	{
		SV_LOG_GLOBAL(error) << e.what();
	}
	system("pause");
	return 0;
}
