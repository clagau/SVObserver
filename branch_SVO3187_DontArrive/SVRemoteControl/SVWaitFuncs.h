//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVWaitFuncs
//* .File Name       : $Workfile:   SVWaitFuncs.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:40:44  $
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSARCH65\PROJECTFILES\ARCHIVES\SVREMOTECONTROL\SVREMOTECONTROL\SVWaitFuncs.h_v  $
 * 
 *    Rev 1.0   24 Apr 2012 15:40:44   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 1
 * SCR Title: Initial check-in
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial check-in.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

