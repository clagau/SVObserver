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
#include <boost/asio.hpp>
#pragma endregion Includes

#pragma region Declarations
#define cServiceName _T("SVO Web Server")
typedef void (*ServiceWorkerFunction)( DWORD argc, LPTSTR  *argv );
#pragma endregion Declarations

void WINAPI ServiceMain( DWORD argc, LPTSTR *argv );
void WINAPI ControlHandler( DWORD ControlCode );

extern boost::asio::io_service gIoService;
extern ServiceWorkerFunction gpStartThread;