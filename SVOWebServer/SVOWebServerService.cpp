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
SERVICE_STATUS        gServiceStatus = {0};
SERVICE_STATUS_HANDLE gStatusHandle( nullptr );
HANDLE gServiceStopEvent(INVALID_HANDLE_VALUE);
ServiceWorkerFunction gpStartThread( nullptr );

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

void WINAPI ServiceMain( DWORD argc, LPTSTR *argv )
{
    DWORD Status( E_FAIL );

    gStatusHandle = RegisterServiceCtrlHandler( cServiceName, ControlHandler );

    if( nullptr != gStatusHandle )
	{

		// Tell the service controller we are starting
		ZeroMemory( &gServiceStatus, sizeof( gServiceStatus ) );
		gServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		gServiceStatus.dwControlsAccepted = 0;
		gServiceStatus.dwCurrentState = SERVICE_START_PENDING;
		gServiceStatus.dwWin32ExitCode = 0;
		gServiceStatus.dwServiceSpecificExitCode = 0;
		gServiceStatus.dwCheckPoint = 0;

		if( !::SetServiceStatus( gStatusHandle, &gServiceStatus ) ) 
		{
			SvStl::MessageMgrStd Exception(SvStl::LogOnly);
			Exception.setMessage(SVMSG_SVWebSrv_0_GENERAL_ERROR, SvStl::Tid_Error_SetServiceStatus, SvStl::SourceFileParams(StdMessageParams));
		}

		gServiceStopEvent = ::CreateEvent(nullptr, true, false, nullptr);
		if (nullptr == gServiceStopEvent)
		{
			SvStl::MessageMgrStd Exception(SvStl::LogOnly);
			Exception.setMessage(SVMSG_SVWebSrv_0_GENERAL_ERROR, SvStl::Tid_Error_CreateEvent, SvStl::SourceFileParams(StdMessageParams));
			gServiceStatus.dwControlsAccepted = 0;
			gServiceStatus.dwCurrentState = SERVICE_STOPPED;
			gServiceStatus.dwWin32ExitCode = GetLastError();
			gServiceStatus.dwCheckPoint = 1;

			if (!::SetServiceStatus(gStatusHandle, &gServiceStatus))
			{
				Exception.setMessage(SVMSG_SVWebSrv_0_GENERAL_ERROR, SvStl::Tid_Error_SetServiceStatus, SvStl::SourceFileParams(StdMessageParams));
			}
		}

		// Tell the service controller we have started
		gServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
		gServiceStatus.dwCurrentState = SERVICE_RUNNING;
		gServiceStatus.dwWin32ExitCode = 0;
		gServiceStatus.dwCheckPoint = 0;

		if( !::SetServiceStatus( gStatusHandle, &gServiceStatus ) )
		{
			SvStl::MessageMgrStd Exception(SvStl::LogOnly);
			Exception.setMessage(SVMSG_SVWebSrv_0_GENERAL_ERROR, SvStl::Tid_Error_SetServiceStatus, SvStl::SourceFileParams(StdMessageParams));
		}

		// Start the worker threads
		if( nullptr != gpStartThread )
		{
			gpStartThread( argc, argv	);
		}
    
		gServiceStatus.dwControlsAccepted = 0;
		gServiceStatus.dwCurrentState = SERVICE_STOPPED;
		gServiceStatus.dwWin32ExitCode = 0;
		gServiceStatus.dwCheckPoint = 3;

		if( !::SetServiceStatus( gStatusHandle, &gServiceStatus ) )
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
			if( SERVICE_RUNNING == gServiceStatus.dwCurrentState )
			{
				gServiceStatus.dwControlsAccepted = 0;
				gServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
				gServiceStatus.dwWin32ExitCode = 0;
				gServiceStatus.dwCheckPoint = 4;

				if( !::SetServiceStatus( gStatusHandle, &gServiceStatus ) )
				{
					SvStl::MessageMgrStd Exception(SvStl::LogOnly);
					Exception.setMessage(SVMSG_SVWebSrv_0_GENERAL_ERROR, SvStl::Tid_Error_SetServiceStatus, SvStl::SourceFileParams(StdMessageParams));
				}

				// This will signal the worker thread to start shutting down
				::SetEvent(gServiceStopEvent);
			}
		}
		break;

     default:
         break;
    }
}
