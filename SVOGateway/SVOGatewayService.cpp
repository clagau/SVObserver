//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file RunRejectService.cpp
//*****************************************************************************
/// This file is the entry points for the service and its corresponding thread
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVOGatewayService.h"
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

    gStatusHandle = RegisterServiceCtrlHandlerEx(cServiceName, ControlHandler, nullptr);

    if( nullptr != gStatusHandle )
	{

		// Tell the service controller we are starting
		ZeroMemory( &gServiceStatus, sizeof( gServiceStatus ) );
		gServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		gServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PRESHUTDOWN;
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
			gServiceStatus.dwCurrentState = SERVICE_STOPPED;
			gServiceStatus.dwWin32ExitCode = GetLastError();
			gServiceStatus.dwCheckPoint = 1;

			if (!::SetServiceStatus(gStatusHandle, &gServiceStatus))
			{
				Exception.setMessage(SVMSG_SVWebSrv_0_GENERAL_ERROR, SvStl::Tid_Error_SetServiceStatus, SvStl::SourceFileParams(StdMessageParams));
			}
		}

		// Tell the service controller we have started
		gServiceStatus.dwControlsAccepted |= (SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PRESHUTDOWN);
		gServiceStatus.dwCurrentState = SERVICE_RUNNING;
		gServiceStatus.dwCheckPoint = 2;

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
    
		gServiceStatus.dwControlsAccepted &= ~(SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PRESHUTDOWN);
		gServiceStatus.dwCurrentState = SERVICE_STOPPED;
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

DWORD WINAPI ControlHandler(DWORD ControlCode, DWORD eventType, void *pEventData, void *pContext)
{
    switch( ControlCode ) 
	{
		case SERVICE_CONTROL_INTERROGATE:
			break;

		case SERVICE_CONTROL_PRESHUTDOWN:
		case SERVICE_CONTROL_STOP:
		{
			gServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
			gServiceStatus.dwCheckPoint = 4;

			if( !::SetServiceStatus( gStatusHandle, &gServiceStatus ) )
			{
				SvStl::MessageMgrStd Exception(SvStl::LogOnly);
				Exception.setMessage(SVMSG_SVWebSrv_0_GENERAL_ERROR, SvStl::Tid_Error_SetServiceStatus, SvStl::SourceFileParams(StdMessageParams));
			}
			::SetEvent(gServiceStopEvent);
			break;
		}

		default:
			::SetServiceStatus(gStatusHandle, &gServiceStatus);
			break;
    }
	return NO_ERROR;
}
