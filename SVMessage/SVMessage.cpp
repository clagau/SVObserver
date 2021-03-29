//******************************************************************************
//* COPYRIGHT (c) 2006 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMessage
//* .File Name       : $Workfile:   SVMessage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:56:26  $
//******************************************************************************

#include "stdafx.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SVMESSAGE_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SVMESSAGE_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef SVMESSAGE_EXPORTS
#define SVMESSAGE_API __declspec(dllexport)
#else
#define SVMESSAGE_API __declspec(dllimport)
#endif

namespace SvStl
{
	class MessageContainer;
}

//! These functor objects must match those in MessageContainer
typedef std::function<INT_PTR ( HWND, LPCTSTR, LPCTSTR, UINT )> ShowDisplayFunctor;
typedef std::function<void(const SvStl::MessageContainer&, int)> MessageNotifyFunctor;

extern "C"
{
	// This is an example of an exported variable
	SVMESSAGE_API unsigned long g_LastDllMainReason=0;
	//Global Show display and Notify functor objects
	SVMESSAGE_API ShowDisplayFunctor g_ShowDisplay{ nullptr };
	SVMESSAGE_API MessageNotifyFunctor g_Notify{ nullptr };
};

BOOL APIENTRY DllMain( HMODULE,
	DWORD  ul_reason_for_call,
	LPVOID
	)
{
	g_LastDllMainReason = ul_reason_for_call;

	return TRUE;
}

DWORD g_dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE; 
DWORD g_dwCatagoryCount = 23;

HRESULT AddMessageKeys(const TCHAR* p_pszBuf )
{
	//@Todo MEC_04_27 regsrv32 does not work this code is probably never called	
	HKEY hk; 
	DWORD dwDisp;
	TCHAR szBuf[MAX_PATH]; 

	// Create the event source as a subkey of the log. 

	_stprintf( szBuf, _T( "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\%s" ),	p_pszBuf ); 

	if( ::RegCreateKeyEx( HKEY_LOCAL_MACHINE, szBuf, 
		0, nullptr, REG_OPTION_NON_VOLATILE,
		KEY_WRITE, nullptr, &hk, &dwDisp ) ) 
	{
		printf( "Could not create the registry key." ); 

		return S_FALSE;
	}

	// Set the supported event types. 

	if( ::RegSetValueEx(hk,      // subkey handle 
		_T( "TypesSupported" ),  // value name 
		0,                 // must be zero 
		REG_DWORD,         // value type 
		(LPBYTE)( &g_dwData ),  // pointer to value data 
		sizeof( DWORD ) ) )    // length of value data 
	{
		printf( "Could not set the supported types." ); 

		::RegCloseKey( hk ); 

		return S_FALSE;
	}

	// Set the catagory count. 

	if( ::RegSetValueEx( hk,      // subkey handle 
		_T( "CategoryCount" ),   // value name 
		0,                 // must be zero 
		REG_DWORD,         // value type 
		(LPBYTE)( &g_dwCatagoryCount ),   // pointer to value data 
		sizeof( DWORD ) ) )    // length of value data 
	{
		printf( "Could not set the category count." ); 

		::RegCloseKey( hk ); 

		return S_FALSE;
	}

	// Set the category message file and number of categories.

	TCHAR MessageFile[1024];
	::GetCurrentDirectory(1024,MessageFile);
	wcscat_s(MessageFile,1024,_T("\\SVMessage.dll"));

	if( ::RegSetValueEx( hk,              // subkey handle 
		_T( "CategoryMessageFile" ),     // value name 
		0,                         // must be zero 
		REG_EXPAND_SZ,             // value type 
		(LPBYTE) MessageFile,        // pointer to value data 
		(DWORD) lstrlen( MessageFile ) + 1 ) ) // length of value data 
	{
		printf( "Could not set the category message file." ); 

		::RegCloseKey( hk ); 

		return S_FALSE;
	}

	// Set the parameter message file and number of categories.

	if( ::RegSetValueEx( hk,              // subkey handle 
		_T( "ParameterMessageFile" ),     // value name 
		0,                         // must be zero 
		REG_EXPAND_SZ,             // value type 
		(LPBYTE) MessageFile,        // pointer to value data 
		(DWORD) lstrlen( MessageFile ) + 1 ) ) // length of value data 
	{
		printf( "Could not set the parameter message file." ); 

		::RegCloseKey( hk ); 

		return S_FALSE;
	}

	// Set the name of the message file. 

	if( ::RegSetValueEx( hk,              // subkey handle 
		_T( "EventMessageFile" ),        // value name 
		0,                         // must be zero 
		REG_EXPAND_SZ,             // value type 
		(LPBYTE) MessageFile,        // pointer to value data 
		(DWORD) lstrlen( MessageFile ) + 1 ) ) // length of value data 
	{
		printf( "Could not set the event message file." ); 

		::RegCloseKey( hk ); 

		return S_FALSE;
	}

	::RegCloseKey(hk); 

	return S_OK;
}

void DeleteMessageKeys(const TCHAR* p_pszBuf)
{
	TCHAR szBuf[MAX_PATH]; 

	// Create the event source as a subkey of the log. 

	_stprintf( szBuf, _T( "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\%s" ),	p_pszBuf ); 

	RegDeleteKeyEx( HKEY_LOCAL_MACHINE, szBuf, KEY_WRITE, 0 );
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllRegisterServer(void)
{
	HRESULT l_Status( S_OK );

	if( S_OK == l_Status )
	{
		l_Status = AddMessageKeys( _T( "SVException" ) );
	}

	if( S_OK == l_Status )
	{
		l_Status = AddMessageKeys( _T( "SVSecurity" ) );
	}

	if( S_OK == l_Status )
	{
		l_Status = AddMessageKeys( _T( "SVAccess" ) );
	}

	return l_Status;
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	DeleteMessageKeys( _T( "SVException" ) );
	DeleteMessageKeys( _T( "SVSecurity" ) );
	DeleteMessageKeys( _T( "SVAccess" ) );

	return S_OK;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

