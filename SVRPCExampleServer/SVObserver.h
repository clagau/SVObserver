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
#include "SVRPCExampleServer/ImageCache.h"
#include "SVRPCExampleServer/Simulation.h"
#include "SVRPCExampleServer/SVOSharedMemory.h"
#include "SVRPCLibrary/RPCServer.h"
#include "SVRPCLibrary/RequestHandler.h"

using namespace SvAuth;
using namespace SvHttp;
using namespace SvPb;
using namespace SvRpc;

namespace SVRPCExample
{
class SVObserver
{
public:
	SVObserver(SimulationSettings& settings, SVOSharedMemory& sm, HttpServerSettings& httpSettings, boost::asio::io_context& io_context)
		: m_simulation(io_context, settings, sm)
		, m_shared_memory(sm)
		, m_io_context(io_context)
		, m_request_handler()
	{
		register_dummy_handler(m_request_handler);
		
		m_rpc_server = std::make_unique<RPCServer>(&m_request_handler);

		httpSettings.pEventHandler = m_rpc_server.get();
		httpSettings.DataDir = std::experimental::filesystem::path(".") / ".." / ".." / "seidenader-frontend" / "dist";
		httpSettings.HttpRequestHandler = std::bind(&SVObserver::on_http_request, this, std::placeholders::_1, std::placeholders::_2);
		m_http_server = std::make_unique<HttpServer>(httpSettings, io_context);
		m_http_server->start();

		m_simulation.start();
	}

private:
	bool on_http_request(const SvHttp::HttpRequest& req, SvHttp::HttpResponse& res)
	{
		return false;
	}

	void register_dummy_handler(RequestHandler& requestHandler)
	{
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
			[this](SetDeviceModeRequest&& req, Task<StandardResponse> task)
		{
			SV_LOG_GLOBAL(info) << "SetDeviceModeRequest";
			m_simulation.setDeviceMode(req.mode());
			StandardResponse res;
			task.finish(std::move(res));
		});
		requestHandler.registerRequestHandler<
			SVRCMessages,
			SVRCMessages::kGetDeviceModeRequest,
			GetDeviceModeRequest,
			GetDeviceModeResponse>(
			[this](GetDeviceModeRequest&& req, Task<GetDeviceModeResponse> task)
		{
			SV_LOG_GLOBAL(info) << "GetDeviceModeRequest";
			GetDeviceModeResponse res;
			res.set_mode(m_simulation.getDeviceMode());
			task.finish(std::move(res));
		});
		requestHandler.registerRequestHandler<
			SVRCMessages,
			SVRCMessages::kGetConfigReportRequest,
			GetConfigReportRequest,
			GetConfigReportResponse>(
			[this](GetConfigReportRequest&& req, Task<GetConfigReportResponse> task)
		{
			SV_LOG_GLOBAL(info) << "GetConfigReportRequest";
			GetConfigReportResponse res;
			res.set_report(m_simulation.getConfiguration());
			task.finish(std::move(res));
		});
		requestHandler.registerRequestHandler<
			SVRCMessages,
			SVRCMessages::kQueryListNameRequest,
			QueryListNameRequest,
			QueryListNameResponse>(
			[this](QueryListNameRequest&& req, Task<QueryListNameResponse> task)
		{
			SV_LOG_GLOBAL(info) << "QueryListNameRequest";
			QueryListNameResponse res;
			auto listnames = m_simulation.getListNames();
			for (const auto& listname : listnames)
			{
				res.add_listname(listname);
			}
			task.finish(std::move(res));
		});
		requestHandler.registerRequestHandler<
			SVRCMessages,
			SVRCMessages::kQueryMonitorListNamesRequest,
			QueryMonitorListNamesRequest,
			NamesResponse>(
			[this](QueryMonitorListNamesRequest&& req, Task<NamesResponse> task)
		{
			SV_LOG_GLOBAL(info) << "QueryMonitorListNamesRequest";
			NamesResponse res;
			applyStringList(*res.mutable_names(), m_simulation.getListNames());
			task.finish(std::move(res));
		});
		requestHandler.registerRequestHandler<
			SVRCMessages,
			SVRCMessages::kGetInspectionNamesRequest,
			GetInspectionNamesRequest,
			NamesResponse>(
			[this](GetInspectionNamesRequest&& req, Task<NamesResponse> task)
		{
			SV_LOG_GLOBAL(info) << "GetInspectionNamesRequest";
			NamesResponse res;
			applyStringList(*res.mutable_names(), m_simulation.getInspectionNames());
			task.finish(std::move(res));
		});
		requestHandler.registerRequestHandler<
			SVRCMessages,
			SVRCMessages::kQueryListItemRequest,
			QueryListItemRequest,
			QueryListItemResponse>(
			[this](QueryListItemRequest&& req, Task<QueryListItemResponse> task)
		{
			QueryListItemResponse res;
			if (req.queryimages())
			{
				const auto& names = m_simulation.getImageNames();
				for (const auto& name : names)
				{
					res.add_imagedeflist()->set_name(name);
				}
			}
			if (req.queryvalues())
			{
				const auto& names = m_simulation.getValueNames();
				for (const auto& name : names)
				{
					res.add_valuedeflist()->set_name(name);
				}
			}
			task.finish(std::move(res));
		});
		requestHandler.registerRequestHandler<
			SVRCMessages,
			SVRCMessages::kGetProductRequest,
			GetProductRequest,
			GetProductResponse>(
			[this](GetProductRequest&& req, Task<GetProductResponse> task)
		{
			SV_LOG_GLOBAL(info) << "GetProductRequest";
			GetProductResponse res;
			m_simulation.getCurrentProduct(*res.mutable_productitem(), req.listname(), req.nameinresponse());
			task.finish(std::move(res));
		});
		requestHandler.registerRequestHandler<
			SVRCMessages,
			SVRCMessages::kGetRejectRequest,
			GetRejectRequest,
			GetRejectResponse>(
			[this](GetRejectRequest&& req, Task<GetRejectResponse> task)
		{
			SV_LOG_GLOBAL(info) << "GetRejectRequest";
			GetRejectResponse res;
			m_simulation.getCurrentProduct(*res.mutable_productitem(), req.listname(), req.nameinresponse());
			task.finish(std::move(res));
		});
		requestHandler.registerRequestHandler<
			SVRCMessages,
			SVRCMessages::kGetFailStatusRequest,
			GetFailStatusRequest,
			GetFailStatusResponse>(
			[this](GetFailStatusRequest&& req, Task<GetFailStatusResponse> task)
		{
			SV_LOG_GLOBAL(info) << "GetFailStatusResponse";
			GetFailStatusResponse res;
#if 0
			{
				Product& prod = *res.add_products();
				prod.set_trigger(s_trigger_count);
				prod.set_status(State::isValid);
				add_value_to_product(prod, req.listname(), req.nameinresponse());
			}
			{
				Product& prod = *res.add_products();
				prod.set_trigger(s_trigger_count - 1);
				prod.set_status(State::isValid);
				add_value_to_product(prod, req.listname(), req.nameinresponse());
			}
			{
				Product& prod = *res.add_products();
				prod.set_trigger(s_trigger_count - 2);
				prod.set_status(State::isValid);
				add_value_to_product(prod, req.listname(), req.nameinresponse());
			}
#endif
			res.set_status(State::isValid);
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
		requestHandler.registerRequestHandler<
			SVRCMessages,
			SVRCMessages::kGetObjectSelectorItemsRequest,
			GetObjectSelectorItemsRequest,
			GetObjectSelectorItemsResponse>(
			[this](GetObjectSelectorItemsRequest&& req, Task<GetObjectSelectorItemsResponse> task)
		{
			SV_LOG_GLOBAL(info) << "GetObjectSelectorItemsRequest";
			GetObjectSelectorItemsResponse res;
			m_simulation.getObjectSelectorItems(*res.mutable_tree());
			task.finish(std::move(res));
		});
		requestHandler.registerStreamHandler<
			SVRCMessages,
			SVRCMessages::kGetNotificationStreamRequest,
			GetNotificationStreamRequest,
			GetNotificationStreamResponse>(
			[this](GetNotificationStreamRequest&& req, Observer<GetNotificationStreamResponse> observer, std::shared_ptr<ServerStreamContext> ctx)
		{
			SV_LOG_GLOBAL(info) << "GetNotificationStreamRequest";
			m_simulation.addNotificationSubscriber(std::move(observer), ctx);
		});
		requestHandler.registerStreamHandler<
			SVRCMessages,
			SVRCMessages::kGetTriggerStreamRequest,
			GetTriggerStreamRequest,
			GetTriggerStreamResponse>(
			[this](GetTriggerStreamRequest&& req, Observer<GetTriggerStreamResponse> observer, std::shared_ptr<ServerStreamContext> ctx)
		{
			SV_LOG_GLOBAL(info) << "GetTriggerStreamRequest";
			m_simulation.addRejectSubscriber(std::move(observer), ctx);
		});
	}

private:
	void applyStringList(SvPb::Value& value, const std::vector<std::string>& list)
	{
		auto strval = std::string{};
		for (const auto& str : list)
		{
			if (!strval.empty())
			{
				strval += ";";
			}
			strval += str;
		}
		value.mutable_item()->set_strval(strval);
		value.set_count(static_cast<int>(list.size()));
	}

private:
	Simulation m_simulation;
	SVOSharedMemory& m_shared_memory;
	boost::asio::io_context& m_io_context;
	RequestHandler m_request_handler;
	std::unique_ptr<RPCServer> m_rpc_server;
	std::unique_ptr<HttpServer> m_http_server;
};
}
