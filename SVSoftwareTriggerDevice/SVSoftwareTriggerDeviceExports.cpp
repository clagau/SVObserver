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

		if ( S_OK == l_hrOk )
		{
			if ( S_OK != l_hrOk )
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

	if ( nullptr != p_pulCount )
	{
		*p_pulCount = g_softwareTrigger.GetTriggerCount();

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetHandle( unsigned long *p_pulHandle, unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulHandle )
	{
		*p_pulHandle = reinterpret_cast<unsigned long>(g_softwareTrigger.GetTriggerHandle(p_ulIndex));
		l_hrOk = S_OK;
	}
	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetName( unsigned long p_ulHandle, BSTR *p_pbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pbstrName && 0 < p_ulHandle)
	{
		if (nullptr != *p_pbstrName)
		{
			::SysFreeString(*p_pbstrName);
		}
		*p_pbstrName = g_softwareTrigger.GetTriggerName(reinterpret_cast<HANDLE>(p_ulHandle));
		
		if (nullptr != *p_pbstrName)
		{
			l_hrOk = S_OK;
		}
	} 
	return l_hrOk;
}

HRESULT WINAPI SVTriggerRegister( unsigned long p_ulHandle, SVTriggerCallbackPtr p_pCallback, void *p_pOwner, void *p_pData )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pCallback && 0 < p_ulHandle)
	{
		l_hrOk = S_OK;

		g_softwareTrigger.AddTriggerCallback(reinterpret_cast<HANDLE>(p_ulHandle), p_pCallback, p_pOwner, p_pData);
	} 
	return l_hrOk;
}

HRESULT WINAPI SVTriggerUnregister( unsigned long p_ulHandle, SVTriggerCallbackPtr p_pCallback, void *p_pOwner, void *p_pData )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pCallback && 0 < p_ulHandle )
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

