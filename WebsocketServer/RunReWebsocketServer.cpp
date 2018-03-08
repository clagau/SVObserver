//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file RunReWebsocketServer.cpp
/// All Rights Reserved
//******************************************************************************
/// Main entry point that
///  - reads settings from registry
///  - initializes logging
///  - initializes Matrox Image Library
///  - connects to the shared memory
///  - starts the Websocket and RPC Server
//******************************************************************************

#include "stdafx.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>

#include <mil.h>

#include "WebsocketLibrary/Logging.h"
#include "WebsocketServer/RRSSharedMemoryAccess.h"
#include "WebsocketServer/RRSSharedMemoryAccessMock.h"
#include "WebsocketServer/ServerRequestHandler.h"
#include "WebsocketServer/SettingsLoader.h"
#include "SVHttpLibrary/WebsocketServer.h"
#include "SVRPCLibrary/RPCServer.h"

static std::unique_ptr<RRWS::RRSSharedMemoryAccessInterface> createSharedMemoryAccess(bool dummy)
{
	if (dummy)
	{
		return std::make_unique<RRWS::RRSSharedMemoryAccessMock>();
	}
	else
	{
		return std::make_unique<RRWS::RRSSharedMemoryAccess>();
	}
}

int main()
{
	MIL_ID myAppId = M_NULL;
	try
	{
		RRWS::Settings settings;
		RRWS::SettingsLoader settingsLoader;
		settingsLoader.loadFromRegistry(settings);
		//@Todo[MEC][8.00] [08.03.2018] could also be in Regestry
		settings.websocketSettings.Host = "0.0.0.0";
		RRWS::init_logging(settings.logSettings);
		BOOST_LOG_TRIVIAL(info) << "RunReWebsocketServer is starting";

		BOOST_LOG_TRIVIAL(debug) << "Initializing Matrox Image Library";
		// Allocate MilSystem
		myAppId = MappAlloc(M_DEFAULT, M_NULL);
		BOOST_LOG_TRIVIAL(debug) << "MatrixImage library was successfully initialized";
		if (myAppId == M_NULL)
		{
			throw std::exception("MapAlloc failed");
		}

		auto sharedMemoryAccess = createSharedMemoryAccess(settings.dummySharedMemory);
		RRWS::ServerRequestHandler requestHandler(sharedMemoryAccess.get());
		SVRPC::RPCServer rpcServer(&requestHandler);

		boost::asio::io_service io_service {1};
		SVHTTP::WebsocketServer server(settings.websocketSettings, io_service, &rpcServer);
		server.start();

		io_service.run();
	}
	catch (std::exception& e)
	{
		BOOST_LOG_TRIVIAL(error) << e.what();
	}
	if (myAppId != M_NULL)
	{
		MappFree(myAppId);
	}
	system("pause");
	return 0;
}
