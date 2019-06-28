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
#include "SVRPCExampleServer/bitmap_image.hpp"
#include "SVRPCExampleServer/SVObserver.h"
#include "SVRPCExampleServer/SVOGateway.h"
#include "SVRPCExampleServer/SVOSharedMemory.h"
#include "SVRPCLibrary/RPCServer.h"
#include "SVRPCLibrary/RequestHandler.h"

using namespace SvAuth;
using namespace SvHttp;
using namespace SvPb;
using namespace SvRpc;

int main()
{
	try
	{
		boost::asio::io_context observer_io_context {1};
		boost::asio::io_context gateway_io_context {1};
		SVRPCExample::SVOSharedMemory shared_memory;

		auto observer_thread = std::thread([&observer_io_context, &shared_memory]() {
			SvHttp::HttpServerSettings httpSettings;
			httpSettings.Host = "127.0.0.1";
			httpSettings.Port = 8081;

			SVRPCExample::SimulationSettings simSettings;
			simSettings.Frequency = 10;
			simSettings.PercentRejects = 0.10;
			simSettings.ImageSizePx = 30;

			SVRPCExample::SVObserver observer(simSettings, shared_memory, httpSettings, observer_io_context);
			//SV_LOG_GLOBAL(info) << "Server running on ws://" << httpSettings.Host << ":" << httpSettings.Port << "/";

			observer_io_context.run();
		});

		auto gateway_thread = std::thread([&gateway_io_context, &shared_memory]() {
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
	auto size = id.imageindex();
	if (size == 0)
	{
		img.set_height(0);
		img.set_width(0);
	}
	else
	{
		img.set_height(size);
		img.set_width(size);
		createImageOfSize(*img.mutable_rgbdata(), size, size);
	}
}

static void addValueToProduct(Product& prod, const std::string& listname, bool withValueNames = true)
{
	if (withValueNames)
	{
		*prod.add_valuenames() = "inspection1.weight";
	}
	auto& weight = *prod.add_values();
	weight.set_type(Variant::DataCase::kFltVal);
	static int v = 0;
	weight.set_fltval(++v + 0.23f);
	if (withValueNames)
	{
		*prod.add_valuenames() = "inspection2.place";
	}
	auto& place = *prod.add_values();
	place.set_type(Variant::DataCase::kStrVal);
	place.set_strval("Munich");
}

struct NotificationSender : public std::enable_shared_from_this<NotificationSender>
{
	boost::asio::io_context& m_io_context;
	Observer<GetNotificationStreamResponse> m_observer;
	std::shared_ptr<ServerStreamContext> m_ctx;
	std::shared_ptr<boost::asio::deadline_timer> m_timer;

	NotificationSender(boost::asio::io_context& io_context, Observer<GetNotificationStreamResponse> observer, std::shared_ptr<ServerStreamContext> ctx)
		: m_io_context(io_context), m_observer(observer), m_ctx(ctx)
	{
	}
	void schedule_timer()
	{
		m_timer = std::make_shared<boost::asio::deadline_timer>(m_io_context);
		m_timer->expires_from_now(boost::posix_time::milliseconds(1000));
		m_timer->async_wait(std::bind(&NotificationSender::on_timer, shared_from_this(),  std::placeholders::_1));
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

		GetNotificationStreamResponse res;
		res.set_currentmode(SvPb::DeviceModeType::runMode);
		m_observer.onNext(std::move(res));

		schedule_timer();
	}
};

static void buildObjectSelectorItemsResponse(TreeItem& item, std::vector<std::string> levels)
{
	if (levels.empty())
	{
		return;
	}

	auto name = levels.front();
	levels.erase(levels.begin());

	item.set_name(name);

	for (int i = 0; i < levels.size() + 1; ++i)
	{
		auto& child = *item.add_children();
		buildObjectSelectorItemsResponse(child, levels);
	}

}

static void register_dummy_handler(RequestHandler& requestHandler, boost::asio::io_context& io_context)
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
		res.mutable_names()->mutable_item()->set_strval("monitorlist1");
		res.mutable_names()->set_count(1);
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
		QueryListItemResponse res;
		if (req.queryimages())
		{
			res.add_imagedeflist()->set_name("inspection1.image1");
			res.add_imagedeflist()->set_name("inspection2.image2");
		}
		if (req.queryvalues())
		{
			res.add_valuedeflist()->set_name("inspection1.weight");
			res.add_valuedeflist()->set_name("inspection2.place");
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
		prod.set_trigger(get_next_trigger_count());
		prod.set_status(State::isValid);
		addValueToProduct(prod, req.listname(), req.nameinresponse());
		ImageId& imgId300px = *prod.add_images();
		imgId300px.set_imageindex(300);
		//imgId300px.set_imagestore(300);
		//imgId300px.set_slotindex(300);
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
		prod.set_trigger(get_next_trigger_count());
		prod.set_status(State::isValid);
		addValueToProduct(prod, req.listname(), req.nameinresponse());
		prod.add_imagenames("rejected image");
		ImageId& imgId = *prod.add_images();
		imgId.set_imageindex(300);
		//imgId.set_imagestore(300);
		//imgId.set_slotindex(300);
		task.finish(std::move(res));
	});
	requestHandler.registerRequestHandler<
		SVRCMessages,
		SVRCMessages::kGetFailStatusRequest,
		GetFailStatusRequest,
		GetFailStatusResponse>(
		[](GetFailStatusRequest&& req, Task<GetFailStatusResponse> task)
	{
		SV_LOG_GLOBAL(info) << "GetFailStatusResponse";
		GetFailStatusResponse res;
#if 0
		{
			Product& prod = *res.add_products();
			prod.set_trigger(s_trigger_count);
			prod.set_status(State::isValid);
			addValueToProduct(prod, req.listname(), req.nameinresponse());
		}
		{
			Product& prod = *res.add_products();
			prod.set_trigger(s_trigger_count - 1);
			prod.set_status(State::isValid);
			addValueToProduct(prod, req.listname(), req.nameinresponse());
		}
		{
			Product& prod = *res.add_products();
			prod.set_trigger(s_trigger_count - 2);
			prod.set_status(State::isValid);
			addValueToProduct(prod, req.listname(), req.nameinresponse());
		}
#endif
		res.set_status(State::isValid);
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
		SVRCMessages::kPutFileRequest,
		PutFileRequest,
		StandardResponse>(
		[](PutFileRequest&& req, Task<StandardResponse> task)
	{
		SV_LOG_GLOBAL(info) << "PutFileRequest " << req.destinationpath();
		std::ofstream out(req.destinationpath());
		out << req.filedata();
		out.close();
		StandardResponse res;
		res.set_hresult(0);
		task.finish(std::move(res));
	});
	requestHandler.registerRequestHandler<
		SVRCMessages,
		SVRCMessages::kGetFileRequest,
		GetFileRequest,
		GetFileResponse>(
		[](GetFileRequest&& req, Task<GetFileResponse> task)
	{
		SV_LOG_GLOBAL(info) << "GetFileRequest " << req.sourcepath();
		std::ifstream in(req.sourcepath());
		std::stringstream buffer;
		buffer << in.rdbuf();
		const auto str = buffer.str();
		GetFileResponse res;
		if (!str.empty())
		{
			res.set_hresult(0);
			res.set_filedata(buffer.str());
		}
		else
		{
			res.set_hresult(1);
		}
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
		[&io_context](GetNotificationStreamRequest&& req, Observer<GetNotificationStreamResponse> observer, std::shared_ptr<ServerStreamContext> ctx)
	{
		SV_LOG_GLOBAL(info) << "GetNotificationStreamRequest";
#if 0
		auto sender = std::make_shared<NotificationSender>(io_context, observer, ctx);
		sender->schedule_timer();
#else
		observer.finish();
#endif
	});
	requestHandler.registerRequestHandler<
		SVRCMessages,
		SVRCMessages::kGetObjectSelectorItemsRequest,
		GetObjectSelectorItemsRequest,
		GetObjectSelectorItemsResponse>(
		[](GetObjectSelectorItemsRequest&& req, Task<GetObjectSelectorItemsResponse> task)
	{
		SV_LOG_GLOBAL(info) << "GetObjectSelectorItemsRequest";
		GetObjectSelectorItemsResponse res;
		std::vector<std::string> levels = {"Root", "Level1", "Level2"};
		buildObjectSelectorItemsResponse(*res.mutable_tree(), levels);
		task.finish(std::move(res));
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
		boost::asio::io_context io_context {1};

			SvOgw::Settings settings;
			SvOgw::SettingsLoader settingsLoader;
			settingsLoader.loadFromIni(settings);
			settings.httpSettings.Host = "0.0.0.0";
			settings.httpSettings.Port = 8080;
			settings.httpSettings.DataDir = std::experimental::filesystem::path(".") / ".." / ".." / "seidenader-frontend" / "dist";

			SvHttp::WebsocketClientSettings client_settings;
			client_settings.Host = "127.0.0.1";
			client_settings.Port = 8081;

			SVRPCExample::SVOGateway gateway(shared_memory, settings, client_settings, gateway_io_context);

			gateway_io_context.run();
		});

		while (true)
		{
			std::this_thread::sleep_for(std::chrono::seconds(5));
		}
	}
	catch (std::exception& e)
	{
		SV_LOG_GLOBAL(error) << e.what();
	}
	system("pause");
	return 0;
}
