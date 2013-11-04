//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSoftwareTriggerDeviceExports
//* .File Name       : $Workfile:   SVSoftwareTriggerDeviceExports.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:30:46  $
//******************************************************************************

#include "StdAfx.h"
#include "SVSoftwareTriggerDevice.h"
#include "SVSoftwareTriggerDeviceExports.h"

long g_lRefCount = 0;

SVSoftwareTriggerDevice g_softwareTrigger;

HRESULT WINAPI SVSoftwareTriggerDestroy(bool p_bClose)
{
	HRESULT l_hrOk = S_OK;

	if ( p_bClose || ::InterlockedDecrement( &g_lRefCount ) <= 0 )
	{
		::InterlockedExchange( &g_lRefCount, 0 );

		l_hrOk = g_softwareTrigger.Initialize( false );
	}

	return l_hrOk;
}

HRESULT WINAPI SVCreate()
{
	HRESULT l_hrOk = S_OK;

	if ( ::InterlockedIncrement( &g_lRefCount ) == 1 )
	{
		l_hrOk = g_softwareTrigger.Initialize( true );

		if ( l_hrOk == S_OK )
		{
			if ( l_hrOk != S_OK )
			{
				SVSoftwareTriggerDestroy( true );
			}
		}
	}
	return l_hrOk;
}

HRESULT WINAPI SVDestroy()
{
	HRESULT l_hrOk = S_OK;

	SVSoftwareTriggerDestroy(false);

	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetCount( unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pulCount != NULL )
	{
		*p_pulCount = g_softwareTrigger.GetTriggerCount();

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetHandle( unsigned long *p_pulHandle, unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pulHandle != NULL )
	{
		*p_pulHandle = reinterpret_cast<unsigned long>(g_softwareTrigger.GetTriggerHandle(p_ulIndex));
		l_hrOk = S_OK;
	}
	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetName( unsigned long p_ulHandle, BSTR *p_pbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pbstrName != NULL && p_ulHandle != NULL)
	{
		// free any crap on input...
		if (*p_pbstrName != NULL)
			::SysFreeString(*p_pbstrName);

		*p_pbstrName = g_softwareTrigger.GetTriggerName(reinterpret_cast<HANDLE>(p_ulHandle));
		
		if (*p_pbstrName != NULL)
			l_hrOk = S_OK;
	} 
	return l_hrOk;
}

HRESULT WINAPI SVTriggerRegister( unsigned long p_ulHandle, SVTriggerCallbackPtr p_pCallback, void *p_pOwner, void *p_pData )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pCallback != NULL && 0 < p_ulHandle)
	{
		l_hrOk = S_OK;

		g_softwareTrigger.AddTriggerCallback(reinterpret_cast<HANDLE>(p_ulHandle), p_pCallback, p_pOwner, p_pData);
	} 
	return l_hrOk;
}

HRESULT WINAPI SVTriggerUnregister( unsigned long p_ulHandle, SVTriggerCallbackPtr p_pCallback, void *p_pOwner, void *p_pData )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pCallback != NULL && 0 < p_ulHandle )
	{
		l_hrOk = g_softwareTrigger.RemoveTriggerCallback(reinterpret_cast<HANDLE>(p_ulHandle), p_pCallback);
	} 
	return l_hrOk;
}

HRESULT WINAPI SVTriggerUnregisterAll( unsigned long p_ulHandle )
{
	HRESULT l_hrOk = S_FALSE;

	if ( 0 < p_ulHandle )
	{
		l_hrOk = g_softwareTrigger.RemoveAllTriggerCallbacks(reinterpret_cast<HANDLE>(p_ulHandle));
	} 
	return l_hrOk;
}

HRESULT WINAPI SVTriggerStart( unsigned long p_ulHandle )
{
	HRESULT l_hrOk = S_FALSE;

	if ( 0 < p_ulHandle)
	{
		l_hrOk = g_softwareTrigger.StartTrigger(reinterpret_cast<HANDLE>(p_ulHandle));
	} 
	return l_hrOk;
}

HRESULT WINAPI SVTriggerStop( unsigned long p_ulHandle )
{
	HRESULT l_hrOk = S_FALSE;

	if ( 0 < p_ulHandle )
	{
		l_hrOk = g_softwareTrigger.StopTrigger(reinterpret_cast<HANDLE>(p_ulHandle));
	} 
	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetParameterCount( unsigned long p_ulHandle, unsigned long *p_pulCount )
{
	HRESULT l_hrOk = g_softwareTrigger.TriggerGetParameterCount(reinterpret_cast<HANDLE>(p_ulHandle), p_pulCount);
	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetParameterName( unsigned long p_ulHandle, unsigned long p_ulIndex, BSTR *p_pbstrName )
{
	HRESULT l_hrOk = g_softwareTrigger.TriggerGetParameterName(reinterpret_cast<HANDLE>(p_ulHandle), p_ulIndex, p_pbstrName);
	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = g_softwareTrigger.TriggerGetParameterValue(reinterpret_cast<HANDLE>(p_ulHandle), p_ulIndex, p_pvarValue);
	return l_hrOk;
}

HRESULT WINAPI SVTriggerSetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = g_softwareTrigger.TriggerSetParameterValue(reinterpret_cast<HANDLE>(p_ulHandle), p_ulIndex, p_pvarValue);
	return l_hrOk;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSoftwareTriggerDevice\SVSoftwareTriggerDeviceExports.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:30:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 10:25:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
