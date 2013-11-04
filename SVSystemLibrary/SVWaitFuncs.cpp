//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVWaitFuncs
//* .File Name       : $Workfile:   SVWaitFuncs.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:04:16  $
//******************************************************************************
#include "stdafx.h"
#include "SVWaitFuncs.h"

DWORD SVWaitFuncs::WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds)
{
	return SVWaitFuncs::WaitForMultipleObjectsEx(1, &hHandle, false, dwMilliseconds, false);
}

DWORD SVWaitFuncs::WaitForSingleObjectEx(HANDLE hHandle, DWORD dwMilliseconds, bool bAlertable)
{
	return SVWaitFuncs::WaitForMultipleObjectsEx(1, &hHandle, false, dwMilliseconds, bAlertable);
}

DWORD SVWaitFuncs::WaitForMultipleObjects(DWORD nCount, LPHANDLE lpHandles, bool bWaitAll, DWORD dwMilliseconds)
{
	return SVWaitFuncs::WaitForMultipleObjectsEx(nCount, lpHandles, bWaitAll, dwMilliseconds, false);
}

DWORD SVWaitFuncs::WaitForMultipleObjectsEx(DWORD nCount, LPHANDLE lpHandles, bool fWaitAll, DWORD dwMilliseconds, bool bAlertable)
{
	DWORD dwRetval(WAIT_FAILED);
	DWORD dwStartTime = GetTickCount();		// this is the start time of the function 
	DWORD dwRemaining = dwMilliseconds;		// this variable is the time left to wait before returning to caller
	DWORD dwFlags = (bAlertable ? MWMO_ALERTABLE : 0);

	for (DWORD i = 0;i < nCount;)
	{	
		if (fWaitAll)
		{
			// wait for each HANDLE in the array
			// NB: note that using the flag MWO_WAITALL requires that all the objects in the array are 
			// signalled AS WELL AS there is a message in the input queue. Therefore if this thread is 
			// not a message processing thread we may never get a message and  
			// MsgWaitForMultipleObjectsEx() may never return.
			dwRetval = ::MsgWaitForMultipleObjectsEx(1, &lpHandles[i], dwRemaining, QS_ALLINPUT, dwFlags);
	
			// if object is signalled we must wait for the next object in the array
			if (dwRetval == WAIT_OBJECT_0)
			{
				i++;
			}
			dwRetval = WAIT_OBJECT_0 + nCount;
		}
		else
		{
			// wait for any of the objects in the array of HANDLEs
			dwRetval = ::MsgWaitForMultipleObjectsEx(nCount, lpHandles, dwRemaining, QS_ALLINPUT, dwFlags);
		}

		if (dwRetval == WAIT_OBJECT_0 + nCount)
		{
			//messages in queue need to be processed
			MSG msg;
			while (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}

			// if we are not waiting indefinitely calculate the remaining time we need to wait
			if(INFINITE != dwRemaining)
			{
				// calculate the time which has elapsed since entering the function
				DWORD dwElapsed = ::GetTickCount() - dwStartTime;

				if (dwMilliseconds > dwElapsed)
				{
					// calculate any remaining time we need to wait 
					dwRemaining = dwMilliseconds - dwElapsed;
				}
				else
				{
					// no need to wait any longer
					//dwRetval = WAIT_TIMEOUT; 
					dwRetval = ::WaitForMultipleObjectsEx( nCount, lpHandles, fWaitAll, 0, bAlertable );

					// escape the loop
					i = nCount;			
				}
			}
		}
		else
		{
			// something else happenned - escape the loop
			i = nCount;
		}
	}
	return dwRetval;
}

bool SVWaitFuncs::SleepEx(DWORD dwMilliseconds, bool bProcessMsgs)
{
	DWORD dwRetval(WAIT_TIMEOUT);
	if (bProcessMsgs)
	{
		// We want to do a non blocking sleep for dwMilliseconds.
		// So we create a dummy event object which we will never signal
		// This is because MsgWaitForMultipleObjectsEx requires a non NULL 
		// Win32 object to wait upon
		HANDLE hWaitObject = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		
		dwRetval = SVWaitFuncs::WaitForSingleObject(hWaitObject, dwMilliseconds);

		//destroy the event object
		::CloseHandle(hWaitObject);
	}
	else
	{
		// do a normal sleep call
		::SleepEx(dwMilliseconds, false);
	}
	return dwRetval == WAIT_TIMEOUT ? true : false;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSystemLibrary\SVWaitFuncs.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 18:04:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jul 2012 16:08:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
