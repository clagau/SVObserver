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
#include <boost/log/trivial.hpp>

#include "SVHttpLibrary/HttpServer.h"
#include "SvHttpLibrary/HttpRequest.h"
#include "SvHttpLibrary/HttpResponse.h"
#include "SVProtoBuf/Protobuf2Rapidjson.h"
#include "SVProtoBuf/SVAuth.h"
#include "SVProtoBuf/SVRC.h"
#include "SVRPCExampleLibrary/format.h"
#include "SVRPCExampleServer/SeidenaderLogo100px.h"
#include "SVRPCExampleServer/SeidenaderLogo200px.h"
#include "SVRPCExampleServer/SeidenaderLogo300px.h"
#include "SVRPCLibrary/RPCServer.h"
#include "SVRPCLibrary/RequestHandler.h"

using namespace SvHttp;
using namespace SvPb;
using namespace SvRpc;
using namespace SvRpc::Example;

struct ClientChunk
{
	uint32_t revision;
	std::string chunk;
};

static std::map<std::string, ClientChunk> s_clientChunks = {};
static const auto s_emptyChunk = std::string {""};

void register_auth_handler(RequestHandler& requestHandler)
{
	requestHandler.registerAuthHandler([](const std::string& token) -> bool
	{
		if (!token.empty())
		{
			BOOST_LOG_TRIVIAL(info) << "Incoming request with token: " << token;
		}
		return true;
	});
}

bool on_http_request(const SvHttp::HttpRequest& req, SvHttp::HttpResponse& res)
{
	if (req.Method == boost::beast::http::verb::post &&
		req.Url.path() == "/auth")
	{
		if (req.Body.size() < 2)
		{
			res.Status = boost::beast::http::status::bad_request;
			return true;
		}

		std::string error;
		SvAuth::LoginRequest loginRequest;
		if (!Protobuf2Rapidjson::decode_from_cstring(req.Body.data(), req.Body.size(), &loginRequest, &error))
		{
			res.Status = boost::beast::http::status::bad_request;
			res.Body = error;
			return true;
		}

		if (loginRequest.username() != "admin@seidenader.de" || loginRequest.password() != "seidenader")
		{
			res.Status = boost::beast::http::status::unauthorized;
			return true;
		}

		SvAuth::LoginResponse resMsg;
		resMsg.mutable_data()->set_name("Administrator");
		resMsg.mutable_data()->set_token("eyJuYW1lIjoiY2Jhc3R1Y2siLCJ0b2tlbiI6MTIzNDV9");

		res.Status = boost::beast::http::status::ok;
		res.ContentType = "application/json";
		res.Body = Protobuf2Rapidjson::encode_to_string(resMsg);
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
		*img.mutable_rgbdata() = std::string(reinterpret_cast<const char*>(SEIDENADER_LOGO_100PX), SEIDENADER_LOGO_100PX_LEN);
	}
	if (id.imageindex() == 200)
	{
		img.set_height(200);
		img.set_width(200);
		*img.mutable_rgbdata() = std::string(reinterpret_cast<const char*>(SEIDENADER_LOGO_200PX), SEIDENADER_LOGO_200PX_LEN);
	}
	if (id.imageindex() == 300)
	{
		img.set_height(300);
		img.set_width(300);
		*img.mutable_rgbdata() = std::string(reinterpret_cast<const char*>(SEIDENADER_LOGO_300PX), SEIDENADER_LOGO_300PX_LEN);
	}
}

static void addValueToProduct(Product& prod)
{
	*prod.add_valuenames() = "weight";
	auto& weight = *prod.add_values();
	weight.set_type(Variant::DataCase::kFltVal);
	weight.set_fltval(42.23f);
	*prod.add_valuenames() = "place";
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
		BOOST_LOG_TRIVIAL(info) << "GetGatewayVersionRequest";
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
		BOOST_LOG_TRIVIAL(info) << "GetSVObserverVersionRequest";
		GetVersionResponse res;
		res.set_version("1.0.0");
		task.finish(std::move(res));
	});
	requestHandler.registerRequestHandler<
		SVRCMessages,
		SVRCMessages::kQueryListNameRequest,
		QueryListNameRequest,
		QueryListNameResponse>(
		[](QueryListNameRequest&& req, Task<QueryListNameResponse> task)
	{
		BOOST_LOG_TRIVIAL(info) << "QueryListNameRequest";
		QueryListNameResponse res;
		res.add_listname("name1");
		task.finish(std::move(res));
	});
	requestHandler.registerRequestHandler<
		SVRCMessages,
		SVRCMessages::kGetProductRequest,
		GetProductRequest,
		GetProductResponse>(
		[](GetProductRequest&& req, Task<GetProductResponse> task)
	{
		BOOST_LOG_TRIVIAL(info) << "GetProductRequest";
		GetProductResponse res;
		Product& prod = *res.mutable_productitem();
		prod.set_trigger(0);
		prod.set_status(State::isValid);
		addValueToProduct(prod);
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
		BOOST_LOG_TRIVIAL(info) << "GetRejectRequest";
		GetRejectResponse res;
		Product& prod = *res.mutable_productitem();
		prod.set_trigger(0);
		prod.set_status(State::isValid);
		addValueToProduct(prod);
		prod.add_imagenames("rejected image");
		ImageId& imgId = *prod.add_images();
		imgId.set_imageindex(100);
		imgId.set_imagestore(100);
		imgId.set_slotindex(100);
		task.finish(std::move(res));
	});
	requestHandler.registerRequestHandler<
		SVRCMessages,
		SVRCMessages::kGetImageFromIdRequest,
		GetImageFromIdRequest,
		GetImageFromIdResponse>(
		[](GetImageFromIdRequest&& req, Task<GetImageFromIdResponse> task)
	{
		BOOST_LOG_TRIVIAL(info) << "GetImageFromIdRequest";
		GetImageFromIdResponse res;
		auto& image = *res.mutable_imagedata();
		getImageForId(image, req.id());
		task.finish(std::move(res));
	});
	requestHandler.registerStreamHandler<
		SVRCMessages,
		SVRCMessages::kGetImageStreamFromIdRequest,
		GetImageStreamFromIdRequest,
		GetImageStreamFromIdResponse>(
		[](GetImageStreamFromIdRequest&& req, Observer<GetImageStreamFromIdResponse> observer, std::shared_ptr<ServerStreamContext> ctx)
	{
		BOOST_LOG_TRIVIAL(info) << "GetImageStreamFromIdRequest";
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
}

static void register_client_chunk_handler(RequestHandler& requestHandler)
{
	requestHandler.registerRequestHandler<
		SVRCMessages,
		SVRCMessages::kSaveClientChunkRequest,
		SaveClientChunkRequest,
		SaveClientChunkResponse>(
		[](SaveClientChunkRequest&& req, Task<SaveClientChunkResponse> task)
	{
		BOOST_LOG_TRIVIAL(info) << "SaveClientChunkRequest";
		const auto& user = req.user();
		const auto& oldRevision = req.revision();
		const auto newRevision = oldRevision + 1;
		auto it = s_clientChunks.find(user);
		if (it != s_clientChunks.end())
		{
			auto& hdl = it->second;
			if (hdl.revision != oldRevision)
			{
				BOOST_LOG_TRIVIAL(warning)
					<< "Revision in SaveClientChunkRequest does not match: "
					<< oldRevision << " != " << hdl.revision;
				auto error = build_error(SvPenv::badRequest, "Provided revision does not match");
				SaveClientChunkError errorDetails;
				errorDetails.set_requestrevision(oldRevision);
				errorDetails.set_currentrevision(hdl.revision);
				error.mutable_payload()->PackFrom(errorDetails);
				task.error(error);
				return;
			}

			hdl.revision = newRevision;
			hdl.chunk = std::move(*req.mutable_chunk());
		}
		else
		{
			if (oldRevision != 0)
			{
				BOOST_LOG_TRIVIAL(warning)
					<< "Revision in SaveClientChunkRequest is not zero for initial save.";
				task.error(build_error(SvPenv::badRequest, "Initial save must use revision 0"));
				return;
			}

			ClientChunk hdl;
			hdl.revision = newRevision;
			hdl.chunk = std::move(*req.mutable_chunk());
			s_clientChunks.emplace(user, std::move(hdl));
		}

		SaveClientChunkResponse res;
		res.set_revision(newRevision);
		task.finish(std::move(res));
	});
	requestHandler.registerRequestHandler<
		SVRCMessages,
		SVRCMessages::kLoadClientChunkRequest,
		LoadClientChunkRequest,
		LoadClientChunkResponse>(
		[](LoadClientChunkRequest&& req, Task<LoadClientChunkResponse> task)
	{
		BOOST_LOG_TRIVIAL(info) << "LoadClientChunkRequest";
		const auto& user = req.user();
		const auto& revision = req.revision();
		auto it = s_clientChunks.find(user);
		if (it != s_clientChunks.end())
		{
			auto& hdl = it->second;
			if (revision > 0 && revision != hdl.revision)
			{
				BOOST_LOG_TRIVIAL(warning)
					<< "Revision in LoadClientChunkRequest does not match: "
					<< revision << " != " << hdl.revision;
				task.error(build_error(SvPenv::badRequest, "Provided revision does not match"));
				return;
			}
			LoadClientChunkResponse res;
			res.set_revision(hdl.revision);
			res.set_chunk(hdl.chunk);
			task.finish(std::move(res));
		}
		else
		{
			BOOST_LOG_TRIVIAL(info) << "Request load for unknown user. Returning empty chunk.";
			LoadClientChunkResponse res;
			res.set_revision(0);
			res.set_chunk(s_emptyChunk);
			task.finish(std::move(res));
		}
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
		BOOST_LOG_TRIVIAL(info) << "StoreClientLogsRequest";
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
		RequestHandler requestHandler;
		register_auth_handler(requestHandler);
		register_example_handler(requestHandler, io_service);
		register_dummy_handler(requestHandler);
		register_client_chunk_handler(requestHandler);
		register_log_handler(requestHandler);
		
		auto rpcServer = std::make_unique<RPCServer>(&requestHandler);

		HttpServerSettings settings;
		settings.Port = 8080;
		settings.pEventHandler = rpcServer.get();
		settings.bEnableFileServing = true;
		settings.DataDir = std::experimental::filesystem::path(".") / ".." / ".." / "seidenader-prototype" / "frontend" / "build";
		settings.DefaultIndexHtmlFile = "index.html";
		settings.DefaultErrorHtmlFile = "index.html"; // enables SPA
		settings.HttpRequestHandler = std::bind(&on_http_request, std::placeholders::_1, std::placeholders::_2);
		auto server = std::make_unique<HttpServer>(settings, io_service);
		server->start();

		BOOST_LOG_TRIVIAL(info) << "Server running on ws://" << settings.Host << ":" << settings.Port << "/";

		auto thread = std::thread([&io_service]() { io_service.run(); });

		while (true)
		{
			std::this_thread::sleep_for(std::chrono::seconds(5));
		}

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
