#pragma once

#include <chrono>
#include <functional>
#include <map>
#include <string>
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
#include "SVRPCExampleServer/SVOSharedMemory.h"
#include "SVRPCLibrary/RequestHandler.h"
#include "SVRPCLibrary/Router.h"
#include "SVRPCLibrary/RPCServer.h"
#include "SVRPCLibrary/RPCClient.h"
#include "SVRPCLibrary/SimpleClient.h"

using namespace SvAuth;
using namespace SvHttp;
using namespace SvPb;
using namespace SvRpc;

namespace SVRPCExample
{
class SVOGateway
{
public:
	SVOGateway(
		SVOSharedMemory& shared_memory,
		SvOgw::Settings& settings,
		SvHttp::WebsocketClientSettings& client_settings,
		boost::asio::io_context& io_context)
		: m_shared_memory(shared_memory)
		, m_io_context(io_context)
		, m_auth_manager(settings.authSettings)
		, m_rest_handler(m_auth_manager)
		, m_request_handler()
		, m_rpc_client(std::make_unique<RPCClient>(client_settings))
		, m_stream_client(*m_rpc_client)
	{
		register_auth_handler(m_request_handler, &m_auth_manager);
		register_dummy_handler(m_request_handler);
		register_log_handler(m_request_handler);

		m_router = std::make_unique<Router>(*m_rpc_client, &m_request_handler);
		m_rpc_server = std::make_unique<RPCServer>(&m_request_handler);

		settings.httpSettings.pEventHandler = m_rpc_server.get();
		settings.httpSettings.HttpRequestHandler = std::bind(&SVOGateway::on_http_request, this, std::placeholders::_1, std::placeholders::_2);
		m_http_server= std::make_unique<HttpServer>(settings.httpSettings, io_context);
		m_http_server->start();

		m_rpc_client->waitForConnectAsync().then(m_io_context, [this](auto future)
		{
			subscribe_to_triggers();
		});
	}

private:
	bool on_http_request(const SvHttp::HttpRequest& req, SvHttp::HttpResponse& res)
	{
		if (m_rest_handler.onRestRequest(req, res))
		{
			return true;
		}

		return false;
	}

	void register_auth_handler(RequestHandler& requestHandler, AuthManager* am)
	{
		requestHandler.registerAuthHandler([am](const std::string& token) -> bool
		{
			return am->rpcAuth(token);
		});
	}

	void register_dummy_handler(RequestHandler& requestHandler)
	{
		requestHandler.registerRequestHandler<
			SVRCMessages,
			SVRCMessages::kGetGatewayVersionRequest,
			GetGatewayVersionRequest,
			GetVersionResponse>(
			[this](GetGatewayVersionRequest&& req, Task<GetVersionResponse> task)
		{
			SV_LOG_GLOBAL(info) << "GetGatewayVersionRequest";
			if (rand() > 0.9)
			{
				GetVersionResponse res;
				res.set_version("1.0.0");
				task.finish(std::move(res));
			}
			else
			{
				WindowsError windowsError;
				windowsError.set_hresult(213);
				task.error(build_packed_error(
					SvPenv::ErrorCode::internalError,
					"Internal Server Error",
					windowsError));
			}
		});
		requestHandler.registerRequestHandler<
			SVRCMessages,
			SVRCMessages::kGetImageFromIdRequest,
			GetImageFromIdRequest,
			GetImageFromIdResponse>(
			[this](GetImageFromIdRequest&& req, Task<GetImageFromIdResponse> task)
		{
			SV_LOG_GLOBAL(info) << "GetImageFromIdRequest";
			task.error(build_error(SvPenv::ErrorCode::notImplemented, "GetImageFromIdRequest not implemented"));
		});
		requestHandler.registerStreamHandler<
			SVRCMessages,
			SVRCMessages::kGetProductStreamRequest,
			GetProductStreamRequest,
			GetProductStreamResponse>(
			[this](GetProductStreamRequest&& req, Observer<GetProductStreamResponse> observer, std::shared_ptr<ServerStreamContext> ctx)
		{
			SV_LOG_GLOBAL(info) << "GetProductStreamRequest";
			// TODO stream all previous trigger if requested!
			if (req.rejectsonly())
			{
				ctx->enableThrottling();
				ctx->setMaxPendingAcks(1);
				m_reject_subs.push_back(std::make_tuple(req, observer, ctx));
			}
			else
			{
				ctx->enableThrottling();
				ctx->setMaxPendingAcks(0);
				m_product_subs.push_back(std::make_tuple(req, observer, ctx));
			}
			
		});
	}

	void register_log_handler(RequestHandler& requestHandler)
	{
		requestHandler.registerRequestHandler<
			SVRCMessages,
			SVRCMessages::kStoreClientLogsRequest,
			StoreClientLogsRequest,
			EmptyResponse>(
			[this](StoreClientLogsRequest&& req, Task<EmptyResponse> task)
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

	void store_log_entry(const std::string& client, const LogEntry& entry)
	{
		const auto severity = map_to_boost_log_severity(entry.severity());
		BOOST_LOG_STREAM_WITH_PARAMS(
			::boost::log::trivial::logger::get(),
			(::boost::log::keywords::severity = severity)
		) << "[" << client << "] " << entry.message();
	}

	boost::log::trivial::severity_level
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

private:
	void subscribe_to_triggers()
	{
		auto nextFkt = [this](GetTriggerStreamResponse&& res)-> SvSyl::SVFuture<void>
		{
			if (res.is_reject())
			{
				//SV_LOG_GLOBAL(error) << "on trigger. is_reject=" << res.is_reject();
				on_trigger_reject(res);
			}

			on_trigger_product(res);

			// do not block the trigger stream until
			// all clients finished their handling
			return SvSyl::SVFuture<void>::make_ready();
		};
		auto FinishFkt = [this]()
		{
			on_trigger_finish();
		};
		auto errorFkt = [this](const SvPenv::Error& err)
		{
			on_trigger_error(err);
		};

		auto observer = Observer<GetTriggerStreamResponse>(nextFkt, FinishFkt, errorFkt);

		GetTriggerStreamRequest req;
		auto client_ctx = m_stream_client.stream(std::move(req), observer);
	}

	void on_trigger_reject(const GetTriggerStreamResponse& triggerRes)
	{
		auto trigger = triggerRes.trigger();

		// TODO add mutex
		for (auto it = m_reject_subs.begin(); it != m_reject_subs.end();)
		{
			auto& req = std::get<0>(*it);
			auto& observer = std::get<1>(*it);
			auto& ctx = std::get<2>(*it);

			if (ctx->isCancelled())
			{
				SV_LOG_GLOBAL(error) << "reject sender client disconnected. stopping notifications.";
				it = m_reject_subs.erase(it);
				continue;
			}

			InspectionResult inspection;
			if (m_shared_memory.getInspectionResultForTrigger(trigger, req.inspectionid(), inspection))
			{
				GetProductStreamResponse res;
				extract_data(inspection, req, res);
				observer.onNext(std::move(res));
			}
			else
			{
				// TODO send error to client?
				//observer.onError();
			}

			++it;
		}
	}

	void on_trigger_product(const GetTriggerStreamResponse& triggerRes)
	{
		auto trigger = triggerRes.trigger();

		// TODO add mutex
		for (auto it = m_product_subs.begin(); it != m_product_subs.end();)
		{
			auto& req = std::get<0>(*it);
			auto& observer = std::get<1>(*it);
			auto& ctx = std::get<2>(*it);

			if (ctx->isCancelled())
			{
				SV_LOG_GLOBAL(error) << "product sender client disconnected. stopping notifications.";
				it = m_product_subs.erase(it);
				continue;
			}

			// TODO do this only once outside of this loop
			InspectionResult inspection;
			if (m_shared_memory.getInspectionResultForTrigger(trigger, req.inspectionid(), inspection))
			{
				GetProductStreamResponse res;
				extract_data(inspection, req, res);
				observer.onNext(std::move(res));
			}
			else
			{
				// TODO send error to clients?
				//observer.onError();
			}

			++it;
		}
	}

	void on_trigger_finish()
	{
		// TODO forward finish to all clients or reconnect?
		//self->m_observer.finish();
	}

	void on_trigger_error(const SvPenv::Error& err)
	{
		// TODO forward error to all clients?
		//self->m_observer.error(err);
	}

	void extract_data(const InspectionResult& inspection, const GetProductStreamRequest& req, GetProductStreamResponse& res)
	{
		res.set_trigger(inspection.trigger());
		res.set_isreject(inspection.isreject());

		for (int i = 0, len = inspection.valueids_size(); i < len; ++i)
		{
			for (int j = 0, len2 = req.valueids_size(); j < len2; ++j)
			{
				if (inspection.valueids(i) == req.valueids(j))
				{
					res.add_values()->CopyFrom(inspection.values(i));
					// TODO only for first response!
					res.add_valuenames(inspection.valuenames(i));
					break;
				}
			}
		}

		for (int i = 0, len = inspection.imageids_size(); i < len; ++i)
		{
			for (int j = 0, len2 = req.imageids_size(); j < len2; ++j)
			{
				if (inspection.imageids(i) == req.imageids(j))
				{
					res.add_images()->CopyFrom(inspection.images(i));
					// TODO only for first response!
					res.add_imagenames(inspection.imagenames(i));
					break;
				}
			}
		}
	}

private:
	SVOSharedMemory& m_shared_memory;
	boost::asio::io_context& m_io_context;
	AuthManager m_auth_manager;
	RestHandler m_rest_handler;
	RequestHandler m_request_handler;
	std::unique_ptr<HttpServer> m_http_server;
	std::unique_ptr<RPCServer> m_rpc_server;
	std::unique_ptr<RPCClient> m_rpc_client;
	std::unique_ptr<Router> m_router;

	SimpleClient<SVRCMessages, GetTriggerStreamRequest, GetTriggerStreamResponse> m_stream_client;

	std::vector<std::tuple<GetProductStreamRequest, Observer<GetProductStreamResponse>, std::shared_ptr<ServerStreamContext>>> m_product_subs;
	std::vector<std::tuple<GetProductStreamRequest, Observer<GetProductStreamResponse>, std::shared_ptr<ServerStreamContext>>> m_reject_subs;
};
}
