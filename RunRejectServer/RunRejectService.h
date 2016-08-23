//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file RunRejectService.h
//*****************************************************************************
/// This file is the entry points for the service and its corresponding thread
//******************************************************************************
#pragma once

#pragma region Includes
#include <Windows.h>
#include <tchar.h>
#pragma endregion Includes

#pragma region Declarations
#define cServiceName _T("Run Reject Server")
typedef void (*ServiceWorkerFunction)( DWORD argc, LPWSTR  *argv );
#pragma endregion Declarations

void WINAPI ServiceMain( DWORD argc, LPTSTR *argv );
void WINAPI ControlHandler( DWORD ControlCode );
DWORD WINAPI ServiceThread( LPVOID lpParam );

extern HANDLE g_ServiceStopEvent;
extern ServiceWorkerFunction gp_StartThreads;