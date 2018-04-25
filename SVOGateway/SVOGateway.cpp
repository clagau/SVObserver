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

#include <boost/log/trivial.hpp>

#include <mil.h>

#include "WebsocketLibrary/Logging.h"
#include "SharedMemoryAccess.h"
#include "SharedMemoryAccessMock.h"
#include "ServerRequestHandler.h"
#include "SettingsLoader.h"
#include "SvHttpLibrary/WebsocketServer.h"
#include "SVRPCLibrary/RPCServer.h"
#include "SVOGatewayService.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"

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
	try
	{
		SvOws::Settings settings;
		SvOws::SettingsLoader settingsLoader;
		settingsLoader.loadFromRegistry(settings);
		//@Todo[MEC][8.00] [08.03.2018] could also be in Registry
		settings.websocketSettings.Host = "0.0.0.0";
		init_logging(settings.logSettings);
		BOOST_LOG_TRIVIAL(info) << "WebsocketServer is starting";

		BOOST_LOG_TRIVIAL(debug) << "Initializing Matrox Image Library";
		// Allocate MilSystem
		MIL_ID MilId = MappAlloc(M_DEFAULT, M_NULL);
		BOOST_LOG_TRIVIAL(debug) << "Matrox Image library was successfully initialized";
		if (M_NULL == MilId)
		{
			throw std::exception("MapAlloc failed");
		}

		auto sharedMemoryAccess = createSharedMemoryAccess(settings.dummySharedMemory);
		SvOws::ServerRequestHandler requestHandler(sharedMemoryAccess.get());
		SvRpc::RPCServer rpcServer(&requestHandler);
		boost::asio::io_service IoService {1};

		SvHttp::WebsocketServer Server(settings.websocketSettings, IoService, &rpcServer);
		Server.start();
		std::thread ServerThread([&] { IoService.run(); });

		//Wait until service should stop
		::WaitForSingleObject(gServiceStopEvent, INFINITE);

		Server.stop();
		IoService.stop();
		ServerThread.join();

		sharedMemoryAccess.reset();

		if (M_NULL != MilId)
		{
			MappFree(MilId);
		}
	}
	catch (std::exception& e)
	{
		BOOST_LOG_TRIVIAL(error) << e.what();
	}
	Exception.setMessage(SVMSG_SVWebSrv_2_GENERAL_INFORMATIONAL, SvStl::Tid_Stopped, SvStl::SourceFileParams(StdMessageParams));
}

int main(int argc, _TCHAR* argv[])
{
	int Result{0};
	SvStl::MessageMgrStd Exception(SvStl::LogOnly);
	Exception.setMessage(SVMSG_SVWebSrv_2_GENERAL_INFORMATIONAL, SvStl::Tid_Started, SvStl::SourceFileParams(StdMessageParams));

	if(CheckCommandLineArgs(argc, argv, _T("/cmd")))
	{
		StartWebServer(argc, argv);
	}
	else
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

	Exception.setMessage(SVMSG_SVWebSrv_2_GENERAL_INFORMATIONAL, SvStl::Tid_Stopped, SvStl::SourceFileParams(StdMessageParams));
	return Result;
}
