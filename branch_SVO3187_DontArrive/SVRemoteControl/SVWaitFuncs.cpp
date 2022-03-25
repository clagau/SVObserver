//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVWaitFuncs
//* .File Name       : $Workfile:   SVWaitFuncs.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:41:06  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVWaitFuncs.h"
#pragma endregion Includes

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
			if (WAIT_OBJECT_0 == dwRetval)
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

		if ( WAIT_OBJECT_0 + nCount == dwRetval )
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
		HANDLE hWaitObject = ::CreateEvent(nullptr, true, false, nullptr);
		
		dwRetval = SVWaitFuncs::WaitForSingleObject(hWaitObject, dwMilliseconds);

		//destroy the event object
		::CloseHandle(hWaitObject);
	}
	else
	{
		// do a normal sleep call
		::SleepEx(dwMilliseconds, false);
	}
	return WAIT_TIMEOUT == dwRetval ? true : false;
}
