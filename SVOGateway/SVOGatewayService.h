//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file RunRejectService.h
//*****************************************************************************
/// This file is the entry points for the service and its corresponding thread
//******************************************************************************
#pragma once

#pragma region Includes
#include <tchar.h>
#pragma endregion Includes

#pragma region Declarations
#define cServiceName _T("SVO Gateway")
typedef void (*ServiceWorkerFunction)( DWORD argc, LPTSTR  *argv );
#pragma endregion Declarations

void WINAPI ServiceMain( DWORD argc, LPTSTR *argv );
DWORD WINAPI ControlHandler(DWORD ControlCode, DWORD eventType, void *pEventData, void *pContext);

extern HANDLE gServiceStopEvent;
extern ServiceWorkerFunction gpStartThread;