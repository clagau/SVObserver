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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSystemLibrary\SVWaitFuncs.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 18:04:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jul 2012 16:07:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

