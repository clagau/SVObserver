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

// This is an example of an exported variable
SVMESSAGE_API unsigned long g_LastDllMainReason=0;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	g_LastDllMainReason = ul_reason_for_call;

	return TRUE;
}

TCHAR g_szMessageFile[] = _T( "C:\\SVObserver\\bin\\SVMessage.dll" );
DWORD g_dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE; 
DWORD g_dwCatagoryCount = 23;

HRESULT AddMessageKeys( TCHAR *p_pszBuf )
{
	HKEY hk; 
	DWORD dwDisp;
	TCHAR szBuf[MAX_PATH]; 

	// Create the event source as a subkey of the log. 

	_stprintf( szBuf, _T( "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\%s" ),	p_pszBuf ); 

	if( ::RegCreateKeyEx( HKEY_LOCAL_MACHINE, szBuf, 
		0, NULL, REG_OPTION_NON_VOLATILE,
		KEY_WRITE, NULL, &hk, &dwDisp ) ) 
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

	if( ::RegSetValueEx( hk,              // subkey handle 
		_T( "CategoryMessageFile" ),     // value name 
		0,                         // must be zero 
		REG_EXPAND_SZ,             // value type 
		(LPBYTE) g_szMessageFile,        // pointer to value data 
		(DWORD) lstrlen( g_szMessageFile ) + 1 ) ) // length of value data 
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
		(LPBYTE) g_szMessageFile,        // pointer to value data 
		(DWORD) lstrlen( g_szMessageFile ) + 1 ) ) // length of value data 
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
		(LPBYTE) g_szMessageFile,        // pointer to value data 
		(DWORD) lstrlen( g_szMessageFile ) + 1 ) ) // length of value data 
	{
		printf( "Could not set the event message file." ); 

		::RegCloseKey( hk ); 

		return S_FALSE;
	}

	::RegCloseKey(hk); 

	return S_OK;
}

void DeleteMessageKeys( TCHAR *p_pszBuf )
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

	if( l_Status == S_OK )
	{
		l_Status = AddMessageKeys( _T( "SVException" ) );
	}

	if( l_Status == S_OK )
	{
		l_Status = AddMessageKeys( _T( "SVSecurity" ) );
	}

	if( l_Status == S_OK )
	{
		l_Status = AddMessageKeys( _T( "SVAccess" ) );
	}

	return S_OK;
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVMessage\SVMessage.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:56:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Jun 2007 09:13:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include removal of all VC6 constraints, project files, and workspace files.  The new VC8 project and solution files will replace the VC6 files with the new tighter compilation rules.
 * 
 * Also added funtions to DLL to self register.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

