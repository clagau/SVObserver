//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file RunRejectService.cpp
//*****************************************************************************
/// This file is the entry points for the service and its corresponding thread
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "RunRejectService.h"
#pragma endregion Includes

#pragma region Declarations
SERVICE_STATUS        g_ServiceStatus = {0};
SERVICE_STATUS_HANDLE g_StatusHandle( nullptr );
HANDLE                g_ServiceStopEvent( INVALID_HANDLE_VALUE );
ServiceWorkerFunction gp_StartThreads( nullptr );

#ifdef _DEBUG
#define new DEBUG_NEW
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
			OutputDebugString(_T("SetServiceStatus returned error"));
		}

		g_ServiceStopEvent = ::CreateEvent( nullptr, true, false, nullptr );
		if( nullptr == g_ServiceStopEvent )
		{
			OutputDebugString(_T("CreateEvent(g_ServiceStopEvent) returned error"));

			g_ServiceStatus.dwControlsAccepted = 0;
			g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
			g_ServiceStatus.dwWin32ExitCode = GetLastError();
			g_ServiceStatus.dwCheckPoint = 1;

			if( !::SetServiceStatus (g_StatusHandle, &g_ServiceStatus) )
			{
				OutputDebugString( _T("SetServiceStatus returned error") );
			}
		}

		// Tell the service controller we are started
		g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
		g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
		g_ServiceStatus.dwWin32ExitCode = 0;
		g_ServiceStatus.dwCheckPoint = 0;

		if( !::SetServiceStatus( g_StatusHandle, &g_ServiceStatus ) )
		{
			OutputDebugString(_T("SetServiceStatus returned error"));
		}

		// Start the worker threads
		if( nullptr != gp_StartThreads )
		{
			gp_StartThreads( argc, argv	);
		}
    
		::CloseHandle( g_ServiceStopEvent );

		g_ServiceStatus.dwControlsAccepted = 0;
		g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		g_ServiceStatus.dwWin32ExitCode = 0;
		g_ServiceStatus.dwCheckPoint = 3;

		if( !::SetServiceStatus( g_StatusHandle, &g_ServiceStatus ) )
		{
			OutputDebugString( _T("SetServiceStatus returned error") );
		}
	}
	else
	{
		OutputDebugString(_T("RegisterServiceCtrlHandler returned error"));
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
					OutputDebugString(_T("SetServiceStatus returned error"));
				}

				// This will signal the worker thread to start shutting down
				::SetEvent( g_ServiceStopEvent );
			}
		}
		break;

     default:
         break;
    }
}
