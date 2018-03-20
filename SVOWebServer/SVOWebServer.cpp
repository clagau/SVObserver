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

#include <boost/log/trivial.hpp>

#include <mil.h>

#include "WebsocketLibrary/Logging.h"
#include "SharedMemoryAccess.h"
#include "SharedMemoryAccessMock.h"
#include "ServerRequestHandler.h"
#include "SettingsLoader.h"
#include "SvHttpLibrary/WebsocketServer.h"
#include "SVRPCLibrary/RPCServer.h"
#include "SVOWebServerService.h"
#include "SVMessage\SVMessage.h"
#include "SVStatusLibrary\MessageManager.h"

static std::unique_ptr<SvOws::SharedMemoryAccessInterface> createSharedMemoryAccess(bool dummy)
{
	if (dummy)
	{
		return std::make_unique<SvOws::SharedMemoryAccessMock>();
	}
	else
	{
		return std::make_unique<SvOws::SharedMemoryAccess>();
	}
}

bool CheckCommandLineArgs(int argc, _TCHAR* argv[], LPCTSTR option)
{
	bool bFound = false;
	if (argc > 1)
	{
		for (int i = 1; i < argc && !bFound; i++)
		{
			if (_tcsicmp(argv[i], option) == 0)
			{
				bFound = true;
			}
		}
	}
	return bFound;
}


void StartWebServer(DWORD argc, LPTSTR  *argv)
{
	SvStl::MessageMgrStd Exception(SvStl::LogOnly);
	Exception.setMessage(SVMSG_SVWebSrv_2_GENERAL_INFORMATIONAL, SvStl::Tid_Started, SvStl::SourceFileParams(StdMessageParams));
	MIL_ID myAppId = M_NULL;
	try
	{
		SvOws::Settings settings;
		SvOws::SettingsLoader settingsLoader;
		settingsLoader.loadFromRegistry(settings);
		//@Todo[MEC][8.00] [08.03.2018] could also be in Registry
		settings.websocketSettings.Host = "0.0.0.0";
		init_logging(settings.logSettings);
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
		SvOws::ServerRequestHandler requestHandler(sharedMemoryAccess.get());
		SvRpc::RPCServer rpcServer(&requestHandler);

		SvHttp::WebsocketServer server(settings.websocketSettings, gIoService, &rpcServer);
		server.start();

		gIoService.run();
	}
	catch (std::exception& e)
	{
		BOOST_LOG_TRIVIAL(error) << e.what();
	}
	if (myAppId != M_NULL)
	{
		MappFree(myAppId);
	}
	Exception.setMessage(SVMSG_SVWebSrv_2_GENERAL_INFORMATIONAL, SvStl::Tid_Stopped, SvStl::SourceFileParams(StdMessageParams));
}

int main(int argc, _TCHAR* argv[])
{
	int Result{0};
	SvStl::MessageMgrStd Exception(SvStl::LogOnly);
	Exception.setMessage(SVMSG_SVWebSrv_2_GENERAL_INFORMATIONAL, SvStl::Tid_Started, SvStl::SourceFileParams(StdMessageParams));

	if(CheckCommandLineArgs(argc, argv, _T("/service")))
	{
		//Function pointer for starting the threads
		gpStartThread = &StartWebServer;

		SERVICE_TABLE_ENTRY ServiceTable[] =
		{
			{cServiceName, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
			{nullptr, nullptr}
		};

		if (!StartServiceCtrlDispatcher(ServiceTable))
		{
	#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)	
			OutputDebugString(_T("StartServiceCtrlDispatcher returned error"));
	#endif		
			Result = GetLastError();
			// running as console (for Debug?)
			if (ERROR_FAILED_SERVICE_CONTROLLER_CONNECT == Result)
			{
				StartWebServer(argc, argv);
			}
		}
	}
	else
	{
		StartWebServer(argc, argv);
	}

	Exception.setMessage(SVMSG_SVWebSrv_2_GENERAL_INFORMATIONAL, SvStl::Tid_Stopped, SvStl::SourceFileParams(StdMessageParams));
	return Result;
}
