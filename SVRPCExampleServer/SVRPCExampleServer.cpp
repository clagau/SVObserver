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
