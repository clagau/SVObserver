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
#include "WebsocketLibrary/Definition.h"
#include "SharedMemoryAccess.h"
#include "ServerRequestHandler.h"
#include "SettingsLoader.h"
#include "SVHttpLibrary/HttpServer.h"
#include "SVRPCLibrary/Router.h"
#include "SVRPCLibrary/RPCServer.h"
#include "SVOGatewayService.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"

static const std::string cLocalHost(_T("127.0.0.1"));

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
	Exception.setMessage(SVMSG_SVGateway_2_GENERAL_INFORMATIONAL, SvStl::Tid_Started, SvStl::SourceFileParams(StdMessageParams));
	try
	{
		SvOgw::Settings settings;
		SvOgw::SettingsLoader settingsLoader;
		settingsLoader.loadFromRegistry(settings);
		//@Todo[MEC][8.00] [08.03.2018] could also be in Registry
		settings.httpSettings.Host = "0.0.0.0";
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

		auto sharedMemoryAccess = std::make_unique<SvOgw::SharedMemoryAccess>();
		SvOgw::ServerRequestHandler requestHandler(sharedMemoryAccess.get());
		SvRpc::Router SVObserverRouter{cLocalHost, SvWsl::Default_SecondPort, &requestHandler};
		SvRpc::RPCServer rpcServer(&requestHandler);
		settings.httpSettings.pEventHandler = &rpcServer;
		boost::asio::io_service IoService {1};

		SvHttp::HttpServer Server(settings.httpSettings, IoService);
		Server.start();
		std::thread ServerThread([&] { IoService.run(); });

		//Wait until service should stop
		::WaitForSingleObject(gServiceStopEvent, INFINITE);
		::CloseHandle(gServiceStopEvent);
		gServiceStopEvent = nullptr;

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
	Exception.setMessage(SVMSG_SVGateway_2_GENERAL_INFORMATIONAL, SvStl::Tid_Stopped, SvStl::SourceFileParams(StdMessageParams));
}

int main(int argc, _TCHAR* argv[])
{
	int Result{0};
	SvStl::MessageMgrStd Exception(SvStl::LogOnly);
	Exception.setMessage(SVMSG_SVGateway_2_GENERAL_INFORMATIONAL, SvStl::Tid_Started, SvStl::SourceFileParams(StdMessageParams));

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

	Exception.setMessage(SVMSG_SVGateway_2_GENERAL_INFORMATIONAL, SvStl::Tid_Stopped, SvStl::SourceFileParams(StdMessageParams));
	return Result;
}
