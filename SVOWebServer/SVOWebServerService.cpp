//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file RunRejectService.cpp
//*****************************************************************************
/// This file is the entry points for the service and its corresponding thread
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVOWebServerService.h"
#include "SVMessage\SVMessage.h"
#include "SVStatusLibrary\MessageManager.h"
#pragma endregion Includes

#pragma region Declarations
SERVICE_STATUS        g_ServiceStatus = {0};
SERVICE_STATUS_HANDLE g_StatusHandle( nullptr );
boost::asio::io_service gIoService{1};
ServiceWorkerFunction gpStartThread( nullptr );

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

void WINAPI ServiceMain( DWORD argc, LPTSTR *argv )
{
    DWORD Status( E_FAIL );

    g_StatusHandle = RegisterServiceCtrlHandler( cServiceName, ControlHandler );

    if( nullptr != g_StatusHandle )
	{

		// Tell the service controller we are starting
		ZeroMemory( &g_ServiceStatus, sizeof( g_ServiceStatus ) );
		g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		g_ServiceStatus.dwControlsAccepted = 0;
		g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
		g_ServiceStatus.dwWin32ExitCode = 0;
		g_ServiceStatus.dwServiceSpecificExitCode = 0;
		g_ServiceStatus.dwCheckPoint = 0;

		if( !::SetServiceStatus( g_StatusHandle, &g_ServiceStatus ) ) 
		{
			SvStl::MessageMgrStd Exception(SvStl::LogOnly);
			Exception.setMessage(SVMSG_SVWebSrv_0_GENERAL_ERROR, SvStl::Tid_Error_SetServiceStatus, SvStl::SourceFileParams(StdMessageParams));
		}

		// Tell the service controller we are started
		g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
		g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
		g_ServiceStatus.dwWin32ExitCode = 0;
		g_ServiceStatus.dwCheckPoint = 0;

		if( !::SetServiceStatus( g_StatusHandle, &g_ServiceStatus ) )
		{
			SvStl::MessageMgrStd Exception(SvStl::LogOnly);
			Exception.setMessage(SVMSG_SVWebSrv_0_GENERAL_ERROR, SvStl::Tid_Error_SetServiceStatus, SvStl::SourceFileParams(StdMessageParams));
		}

		// Start the worker threads
		if( nullptr != gpStartThread )
		{
			gpStartThread( argc, argv	);
		}
    
		g_ServiceStatus.dwControlsAccepted = 0;
		g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		g_ServiceStatus.dwWin32ExitCode = 0;
		g_ServiceStatus.dwCheckPoint = 3;

		if( !::SetServiceStatus( g_StatusHandle, &g_ServiceStatus ) )
		{
			SvStl::MessageMgrStd Exception(SvStl::LogOnly);
			Exception.setMessage(SVMSG_SVWebSrv_0_GENERAL_ERROR, SvStl::Tid_Error_SetServiceStatus, SvStl::SourceFileParams(StdMessageParams));
		}
	}
	else
	{
		SvStl::MessageMgrStd Exception(SvStl::LogOnly);
		Exception.setMessage(SVMSG_SVWebSrv_0_GENERAL_ERROR, SvStl::Tid_Error_RegisterServiceHandler, SvStl::SourceFileParams(StdMessageParams));
	}
}

void WINAPI ControlHandler( DWORD ControlCode )
{
    switch( ControlCode ) 
	{
	case SERVICE_CONTROL_STOP :
		{
			if( SERVICE_RUNNING == g_ServiceStatus.dwCurrentState )
			{
				g_ServiceStatus.dwControlsAccepted = 0;
				g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
				g_ServiceStatus.dwWin32ExitCode = 0;
				g_ServiceStatus.dwCheckPoint = 4;

				if( !::SetServiceStatus( g_StatusHandle, &g_ServiceStatus ) )
				{
					SvStl::MessageMgrStd Exception(SvStl::LogOnly);
					Exception.setMessage(SVMSG_SVWebSrv_0_GENERAL_ERROR, SvStl::Tid_Error_SetServiceStatus, SvStl::SourceFileParams(StdMessageParams));
				}

				// This will signal the worker thread to start shutting down
				gIoService.reset();
			}
		}
		break;

     default:
         break;
    }
}
