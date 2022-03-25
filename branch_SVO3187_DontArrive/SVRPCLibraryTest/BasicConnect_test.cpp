#include "stdafx.h"

#include <boost/asio.hpp>
#include <boost/test/unit_test.hpp>

#include <SVHttpLibrary/HttpServer.h>
#include <SVHttpLibrary/HttpServerSettings.h>
#include <SVHttpLibrary/WebsocketClientSettings.h>
#include <SVProtobuf/SVRC.h>
#include <SVRPCLibrary/RequestHandler.h>
#include <SVRPCLibrary/RPCClient.h>
#include <SVRPCLibrary/RPCServer.h>
#include <SVRPCLibrary/SimpleClient.h>

using namespace SvHttp;
using namespace SvPb;
using namespace SvPenv;
using namespace SvRpc;

static const char* S_HOST = "127.0.0.1";
static const uint16_t S_PORT = 8082;

struct TestClient
{
	TestClient()
		: m_Settings(build_settings())
		, m_pRpcClient(std::make_unique<RPCClient>(m_Settings))
		, m_getVersionClient(*m_pRpcClient)
	{
		m_pRpcClient->waitForConnect(m_ConnectTimeout);
	}

	SvSyl::SVFuture<GetVersionResponse> getGatewayVersion()
	{
		return m_getVersionClient.request(GetGatewayVersionRequest(), m_RequestTimeout);
	}

private:
	static WebsocketClientSettings build_settings()
	{
		WebsocketClientSettings settings;
		settings.Host = S_HOST;
		settings.Port = S_PORT;
		return settings;
	}

	SvHttp::WebsocketClientSettings m_Settings;
	std::unique_ptr<RPCClient> m_pRpcClient;
	boost::posix_time::seconds m_ConnectTimeout {2};
	boost::posix_time::seconds m_RequestTimeout {2};
	SimpleClient<SVRCMessages, GetGatewayVersionRequest, GetVersionResponse> m_getVersionClient;
};

struct TestServer
{
	explicit TestServer(RequestHandlerBase* pRequestHandler)
		: m_pRpcServer(std::make_unique<RPCServer>(pRequestHandler))
	{
		m_Settings.Port = S_PORT;
		m_Settings.Host = S_HOST;
		m_Settings.pEventHandler = m_pRpcServer.get();
		m_pHttpServer = std::make_unique<HttpServer>(m_Settings, m_IoService);
		m_pHttpServer->start();
		m_IoThread = std::thread([this]() { m_IoService.run(); });
	}

	void stop()
	{
		m_pHttpServer->stop();
		m_IoService.stop();
		m_IoThread.join();
		m_pRpcServer.reset();
		m_pHttpServer.reset();
	}

	std::thread m_IoThread;
	boost::asio::io_service m_IoService;
	HttpServerSettings m_Settings;
	std::unique_ptr<HttpServer> m_pHttpServer;
	std::unique_ptr<RPCServer> m_pRpcServer;
};

static void register_get_gateway_version_handler(RequestHandler& rRequestHandler)
{
	rRequestHandler.registerRequestHandler<
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

};

BOOST_AUTO_TEST_CASE(should_respond_to_simple_request)
{
	RequestHandler requestHandler;
	register_get_gateway_version_handler(requestHandler);

	TestServer testServer(&requestHandler);

	TestClient testClient;
	testClient.getGatewayVersion().get();

	testServer.stop();
}
