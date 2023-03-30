//******************************************************************************
/// \copyright (c) 2017,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
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

//Moved to precompiled header: #include <boost/log/trivial.hpp>
//Moved to precompiled header: #include <conio.h>

#include <mil.h>

#include "SharedMemoryAccess.h"
#include "ServerRequestHandler.h"
#include "Settings.h"
#include "SettingsLoader.h"
#include "SVOGatewayService.h"
#include "WebAppVersionLoader.h"
#include "SVAuthLibrary/AuthManager.h"
#include "SVAuthLibrary/RestHandler.h"
#include "SVHttpLibrary/HttpServer.h"
#include "SVMatroxLibrary/SVMatroxApplicationInterface.h"
#include "SVLogLibrary/Logging.h"
#include "SVMessage/SVMessage.h"
#include "SVRPCLibrary/Router.h"
#include "SVRPCLibrary/RPCServer.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/MessageTextGenerator.h"
#include "SVSystemLibrary/SVVersionInfo.h"
#include "ObjectInterfaces/ITriggerRecordControllerR.h"

constexpr const char* cLocalHost(_T("127.0.0.1"));

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

void StartWebServer(DWORD argc, LPTSTR* argv)
{
	SvLog::bootstrap_logging();
	SvStl::MessageManager Exception(SvStl::MsgType::Log);
	Exception.setMessage(SVMSG_SVGateway_2_GENERAL_INFORMATIONAL, SvStl::Tid_Started, SvStl::SourceFileParams(StdMessageParams));
	try
	{
		std::mutex mutex;
		std::condition_variable cv;
		bool workDone = false;
		SvOgw::Settings settings;
		SvOgw::SettingsLoader settingsLoader;
		settingsLoader.loadFromIni(settings);
		settings.httpSettings.ServerVersionString = SvSyl::SVVersionInfo::GetVersion();
		settings.httpSettings.Host = "0.0.0.0";

		try
		{
			SvLog::init_logging(settings.logSettings);
		}
		catch (std::runtime_error& rRuntimeError)
		{
			SV_LOG_GLOBAL(error) << std::string(rRuntimeError.what());
		}
		
		SV_LOG_GLOBAL(info) << "SVOGatewayIniPath: " << settingsLoader.GetIni();
		SV_LOG_GLOBAL(info) << "WebsocketServer is starting";

		boost::asio::io_service IoService {1};
		boost::asio::io_service::work work(IoService);
#ifdef _DEBUG
		// Enable MIL error message to be displayed by MIL
		MappControl(M_ERROR, M_PRINT_ENABLE);
#else
		// Disable MIL error message to be displayed by MIL
		MappControl(M_ERROR, M_PRINT_DISABLE);
#endif // DEBUG

		// create client, but only start connecting when we also have the router running
		SvRpc::RPCClient rpcClient(settings.observerSetting);

		SvAuth::AuthManager authManager(settings.authSettings);
		SvAuth::RestHandler restHandler(authManager);

		SvOgw::WebAppVersionLoader webAppVersionLoader(settings.httpSettings);

		SvOgw::SharedMemoryAccess sharedMemoryAccess(IoService, settings.shareControlSettings, webAppVersionLoader, rpcClient, authManager.getUserDatabase(), settings.skipPermissionChecks);
		SvStl::MessageManager::setNotificationFunction(
			[&sharedMemoryAccess](const SvStl::MessageContainer& rMessage, int messageType)
			{
				sharedMemoryAccess.onMessageContainer(rMessage, messageType);
			});

		SvOgw::ServerRequestHandler requestHandler(&sharedMemoryAccess, &authManager);
		SvRpc::RPCServer rpcServer(&requestHandler);
		settings.httpSettings.pEventHandler = &rpcServer;
		auto refHandler{ std::ref(restHandler) };
		auto httpRequestHandler = [refHandler](const SvHttp::HttpRequest& req, SvHttp::HttpResponse& res) { return refHandler.get().onRestRequest(req, res); };
		settings.httpSettings.HttpRequestHandler = httpRequestHandler;

		std::unique_ptr<SvHttp::HttpServer> pServer {nullptr};
		std::thread ServerThread([&] { IoService.run(); });

		// automatically start/stop the server based on the connection status of SVObserver client
		rpcClient.addStatusListener([&](SvRpc::ClientStatus status)
		{
			if (status == SvRpc::ClientStatus::Connected && nullptr == pServer)
			{
				SV_LOG_GLOBAL(info) << SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_Gateway_SVObserverConnected);
				SvStl::MessageManager Exception(SvStl::MsgType::Notify);
				Exception.setMessage(SVMSG_SVGateway_2_GENERAL_INFORMATIONAL, SvStl::Tid_Gateway_SVObserverConnected, SvStl::SourceFileParams(StdMessageParams));

				pServer = std::make_unique<SvHttp::HttpServer>(settings.httpSettings, IoService);
				pServer->start();
			}
			else if (status == SvRpc::ClientStatus::Disconnected && pServer.get())
			{
				SV_LOG_GLOBAL(info) << SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_Gateway_SVObserverDisconnected);
				SvStl::MessageManager Exception(SvStl::MsgType::Notify);
				Exception.setMessage(SVMSG_SVGateway_2_GENERAL_INFORMATIONAL, SvStl::Tid_Gateway_SVObserverDisconnected, SvStl::SourceFileParams(StdMessageParams));

				IoService.post([&]{
					rpcServer.disconnect_all_connections();
					{
						std::lock_guard<std::mutex> lock(mutex);
						workDone = true;
					}
					cv.notify_one();
				});
				{
					std::unique_lock<std::mutex> lock(mutex);
					cv.wait(lock, [&]{ return workDone; });
				}
				pServer->stop();
				pServer.reset(nullptr);
			}
		});

		// now start the router. this will pass all unhandled requests & streams to the
		// SVObserver client. it will also take care of actually connecting the client and
		// re-connecting it when the connection was lost.
		SvRpc::Router SVObserverRouter(rpcClient, requestHandler);

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

		if (pServer)
		{
			pServer->stop();
		}

		///Stop work
		IoService.stop();
		ServerThread.join();

		//Should be done before destroyTriggerRecordController
		sharedMemoryAccess.unsubscribe_from_trc();
	}
	catch (std::exception& e)
	{
		SV_LOG_GLOBAL(error) << e.what();
	}
	try
	{
		SvOi::destroyTriggerRecordController();
	}
	catch (std::exception& e)
	{
		SV_LOG_GLOBAL(error) << e.what();
	}

	Exception.setMessage(SVMSG_SVGateway_2_GENERAL_INFORMATIONAL, SvStl::Tid_Stopped, SvStl::SourceFileParams(StdMessageParams));
}

int main(int argc, _TCHAR* argv[])
{
	//this command is to send the assert to a messagebox (otherwise the assert aborts the application)
	_set_error_mode(_OUT_TO_MSGBOX);
	int Result {0};
	SvStl::MessageManager Exception(SvStl::MsgType::Log);
	Exception.setMessage(SVMSG_SVGateway_2_GENERAL_INFORMATIONAL, SvStl::Tid_Started, SvStl::SourceFileParams(StdMessageParams));

	// Startup Matrox App
	SVMatroxApplicationInterface::Startup();

	SvOi::createTriggerRecordControllerInstance(SvOi::TRC_DataType::Reader);

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
			{const_cast<LPSTR> (cServiceName), (LPSERVICE_MAIN_FUNCTION)ServiceMain},
			{nullptr, nullptr}
		};

		if (!StartServiceCtrlDispatcher(ServiceTable))
		{
			SV_LOG_GLOBAL(error) << "StartServiceCtrlDispatcher returned error";
			Result = GetLastError();
			// running as console (for Debug?)
			if (ERROR_FAILED_SERVICE_CONTROLLER_CONNECT == Result)
			{
				StartWebServer(argc, argv);
			}
		}
	}

	// Shutdown MIL
	SVMatroxApplicationInterface::Shutdown();

	Exception.setMessage(SVMSG_SVGateway_2_GENERAL_INFORMATIONAL, SvStl::Tid_Stopped, SvStl::SourceFileParams(StdMessageParams));
	return Result;
}
