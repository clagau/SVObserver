//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEventMsg.cpp
//* .File Name       : $Workfile:   SVEventMsg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   10 Sep 2014 11:37:28  $
//******************************************************************************

#include "stdafx.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SVEVENTMSG_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SVEVENTMSG_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef SVEVENTMSG_EXPORTS
#define SVEVENTMSG_API __declspec(dllexport)
#else
#define SVEVENTMSG_API __declspec(dllimport)
#endif

DWORD g_dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE; 
DWORD g_dwCatagoryCount = 1;

HMODULE g_hModule = nullptr;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			// Update location of g_szMessageFile
			g_hModule = hModule;
			break;

		case DLL_PROCESS_DETACH:
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;
	}
	return TRUE;
}

HRESULT AddMessageKeys(LPCTSTR pSource, LPCTSTR pMsgDllPath)
{
	HKEY hk; 
	DWORD dwDisp;
	TCHAR szBuf[MAX_PATH]; 

	// Create the event source as a subkey of the log. 
	_stprintf(szBuf, _T("SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\%s"), pSource); 

	if (::RegCreateKeyEx(HKEY_LOCAL_MACHINE, szBuf, 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &hk, &dwDisp)) 
	{
		_tprintf(_T("Could not create the registry key.")); 
		return S_FALSE;
	}
	// Set the supported event types. 
	if (::RegSetValueEx(hk, _T( "TypesSupported" ), 0, REG_DWORD, (LPBYTE)(&g_dwData), sizeof(DWORD)))
	{
		_tprintf(_T("Could not set the supported types."));
		::RegCloseKey(hk); 
		return S_FALSE;
	}
	// Set the catagory count. 
	if (::RegSetValueEx(hk, _T("CategoryCount"), 0, REG_DWORD, (LPBYTE)(&g_dwCatagoryCount), sizeof(DWORD)))
	{
		_tprintf(_T("Could not set the category count."));
		::RegCloseKey(hk); 
		return S_FALSE;
	}
	// Set the category message file and number of categories.
	if (::RegSetValueEx(hk, _T("CategoryMessageFile"), 0, REG_EXPAND_SZ, (LPBYTE)pMsgDllPath, static_cast<DWORD>((_tcslen(pMsgDllPath) + 1) * sizeof(TCHAR)))) // length of value data 
	{
		_tprintf(_T("Could not set the category message file."));
		::RegCloseKey(hk); 
		return S_FALSE;
	}
	// Set the parameter message file and number of categories.
	if (::RegSetValueEx(hk, _T("ParameterMessageFile"), 0, REG_EXPAND_SZ, (LPBYTE)pMsgDllPath, static_cast<DWORD>((_tcslen(pMsgDllPath) + 1) * sizeof(TCHAR)))) // length of value data 
	{
		_tprintf(_T("Could not set the parameter message file.")); 
		::RegCloseKey(hk); 
		return S_FALSE;
	}
	// Set the name of the message file. 
	if (::RegSetValueEx(hk, _T("EventMessageFile"), 0, REG_EXPAND_SZ, (LPBYTE)pMsgDllPath, static_cast<DWORD>((_tcslen(pMsgDllPath) + 1) * sizeof(TCHAR)))) // length of value data 
	{
		_tprintf(_T("Could not set the event message file.") );
		::RegCloseKey( hk ); 
		return S_FALSE;
	}
	::RegCloseKey(hk); 
	return S_OK;
}

void DeleteMessageKeys(LPCTSTR pSource)
{
	TCHAR szBuf[MAX_PATH]; 

	// Create the event source as a subkey of the log. 
	_stprintf(szBuf, _T("SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\%s"), pSource); 
	RegDeleteKeyEx(HKEY_LOCAL_MACHINE, szBuf, KEY_WRITE, 0);
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllRegisterServer(void)
{
	TCHAR filePath[MAX_PATH * 2] = {0};
	GetModuleFileName(g_hModule, filePath, sizeof(filePath) / sizeof(TCHAR));
	HRESULT l_Status = AddMessageKeys(_T("SVRemoteControl"), filePath);
	return l_Status;
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	DeleteMessageKeys(_T("SVRemoteControl"));
	return S_OK;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVEventMsg\SVEventMsg.cpp_v  $
 * 
 *    Rev 1.0   10 Sep 2014 11:37:28   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: sJones; Steve Jones
 * Change Description: Initial Checkin
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/
