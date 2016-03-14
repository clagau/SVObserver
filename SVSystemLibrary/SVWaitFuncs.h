//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVWaitFuncs
//* .File Name       : $Workfile:   SVWaitFuncs.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:04:30  $
//******************************************************************************

#pragma once

namespace SVWaitFuncs
{
	DWORD WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);
	DWORD WaitForSingleObjectEx(HANDLE hHandle, DWORD dwMilliseconds, bool bAlertable);
	DWORD WaitForMultipleObjects(DWORD nCount, LPHANDLE lpHandles, bool bWaitAll, DWORD dwMilliseconds);
	DWORD WaitForMultipleObjectsEx(DWORD nCount, LPHANDLE lpHandles, bool fWaitAll, DWORD dwMilliseconds, bool bAlertable);

	bool SleepEx(DWORD dwMilliseconds, bool bProcessMsgs=true);
}

