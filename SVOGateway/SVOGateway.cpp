//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file SVOGateway.cpp
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
#include <conio.h>

#include "SVLogLibrary/Logging.h"
#include "WebsocketLibrary/Definition.h"
#include "SharedMemoryAccess.h"
#include "ServerRequestHandler.h"
#include "SettingsLoader.h"
#include "SVAuthLibrary/AuthManager.h"
#include "SVAuthLibrary/RestHandler.h"
#include "SVHttpLibrary/HttpServer.h"
#include "SVRPCLibrary/Router.h"
#include "SVRPCLibrary/RPCServer.h"
#include "SVOGatewayService.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVSystemLibrary/SVVersionInfo.h"

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

bool on_http_request(SvAuth::RestHandler& rRestHandler, const SvHttp::HttpRequest& req, SvHttp::HttpResponse& res)
{
	if (rRestHandler.onRestRequest(req, res))
	{
		return true;
	}

	return false;
}

void StartWebServer(DWORD argc, LPTSTR  *argv)
{
	SvLog::bootstrap_logging();

	SvStl::MessageMgrStd Exception(SvStl::LogOnly);
	Exception.setMessage(SVMSG_SVGateway_2_GENERAL_INFORMATIONAL, SvStl::Tid_Started, SvStl::SourceFileParams(StdMessageParams));
	try
	{
		SvOgw::Settings settings;
		SvOgw::SettingsLoader settingsLoader;
		settingsLoader.loadFromIni(settings);
		settings.httpSettings.ServerVersionString = SvSyl::SVVersionInfo::GetVersion();
		settings.httpSettings.Host = "0.0.0.0";
		SvLog::init_logging(settings.logSettings);
		SV_LOG_GLOBAL(info) << "WebsocketServer is starting";

		SV_LOG_GLOBAL(debug) << "Initializing Matrox Image Library";
		// Allocate MilSystem
		MIL_ID MilId = MappAlloc(M_DEFAULT, M_NULL);
		SV_LOG_GLOBAL(debug) << "Matrox Image library was successfully initialized";
		if (M_NULL == MilId)
		{
			throw std::exception("MapAlloc failed");
		}

		SvAuth::AuthManager authManager(settings.authSettings);
		SvAuth::RestHandler restHandler(authManager);

		auto sharedMemoryAccess = std::make_unique<SvOgw::SharedMemoryAccess>();
		SvOgw::ServerRequestHandler requestHandler(sharedMemoryAccess.get());
		SvRpc::Router SVObserverRouter {settings.observerSetting, &requestHandler};
		SvRpc::RPCServer rpcServer(&requestHandler);
		settings.httpSettings.pEventHandler = &rpcServer;
		settings.httpSettings.HttpRequestHandler = std::bind(&on_http_request, std::ref(restHandler), std::placeholders::_1, std::placeholders::_2);
		boost::asio::io_service IoService {1};

		SvHttp::HttpServer Server(settings.httpSettings, IoService);
		Server.start();
		std::thread ServerThread([&] { IoService.run(); });

		if (CheckCommandLineArgs(argc, argv, _T("/cmd")))
		{
			bool exit {false};
			while (!exit)
			{
				exit = ::WaitForSingleObject(gServiceStopEvent, 100) == WAIT_OBJECT_0;
				if (_kbhit())
				{
					exit = ('x' == _getch());
				}
			}
		}
		else
		{
			//Wait until service should stop
			::WaitForSingleObject(gServiceStopEvent, INFINITE);
		}

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
		SV_LOG_GLOBAL(error) << e.what();
	}
	Exception.setMessage(SVMSG_SVGateway_2_GENERAL_INFORMATIONAL, SvStl::Tid_Stopped, SvStl::SourceFileParams(StdMessageParams));
}

int main(int argc, _TCHAR* argv[])
{
	int Result {0};
	SvStl::MessageMgrStd Exception(SvStl::LogOnly);
	Exception.setMessage(SVMSG_SVGateway_2_GENERAL_INFORMATIONAL, SvStl::Tid_Started, SvStl::SourceFileParams(StdMessageParams));

	if (CheckCommandLineArgs(argc, argv, _T("/cmd")))
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
