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

HRESULT WINAPI SVTriggerGetHandle( unsigned long *pTriggerchannel, unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != pTriggerchannel )
	{
		*pTriggerchannel = g_softwareTrigger.GetTriggerHandle(p_ulIndex);
		l_hrOk = S_OK;
	}
	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetName( unsigned long triggerchannel, BSTR *p_pbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pbstrName && 0 < triggerchannel)
	{
		if (nullptr != *p_pbstrName)
		{
			::SysFreeString(*p_pbstrName);
		}
		*p_pbstrName = g_softwareTrigger.GetTriggerName(triggerchannel);
		
		if (nullptr != *p_pbstrName)
		{
			l_hrOk = S_OK;
		}
	} 
	return l_hrOk;
}

HRESULT WINAPI SVTriggerRegister( unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher )
{
	HRESULT l_hrOk = S_FALSE;

	if ( rDispatcher.hasCallback() && 0 < triggerchannel)
	{
		l_hrOk = S_OK;

		g_softwareTrigger.AddDispatcher(triggerchannel, rDispatcher);
	} 
	return l_hrOk;
}

HRESULT WINAPI SVTriggerUnregister( unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher)
{
	HRESULT l_hrOk = S_FALSE;

	if ( rDispatcher.hasCallback() && 0 < triggerchannel )
	{
		l_hrOk = g_softwareTrigger.RemoveDispatcher(triggerchannel, rDispatcher);
	} 
	return l_hrOk;
}

HRESULT WINAPI SVTriggerUnregisterAll( unsigned long triggerchannel )
{
	HRESULT l_hrOk = S_FALSE;

	if ( 0 < triggerchannel )
	{
		l_hrOk = g_softwareTrigger.RemoveAllDispatchers(triggerchannel);
	} 
	return l_hrOk;
}

HRESULT WINAPI SVTriggerStart( unsigned long triggerchannel )
{
	HRESULT l_hrOk = S_FALSE;

	if ( 0 < triggerchannel)
	{
		l_hrOk = g_softwareTrigger.StartTrigger(triggerchannel);
	} 
	return l_hrOk;
}

HRESULT WINAPI SVTriggerStop( unsigned long triggerchannel )
{
	HRESULT l_hrOk = S_FALSE;

	if ( 0 < triggerchannel )
	{
		l_hrOk = g_softwareTrigger.StopTrigger(triggerchannel);
	} 
	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetParameterCount( unsigned long triggerchannel, unsigned long *p_pulCount )
{
	HRESULT l_hrOk = g_softwareTrigger.TriggerGetParameterCount(triggerchannel, p_pulCount);
	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetParameterName( unsigned long triggerchannel, unsigned long p_ulIndex, BSTR *p_pbstrName )
{
	HRESULT l_hrOk = g_softwareTrigger.TriggerGetParameterName(triggerchannel, p_ulIndex, p_pbstrName);
	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetParameterValue( unsigned long triggerchannel, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = g_softwareTrigger.TriggerGetParameterValue(triggerchannel, p_ulIndex, p_pvarValue);
	return l_hrOk;
}

HRESULT WINAPI SVTriggerSetParameterValue( unsigned long triggerchannel, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = g_softwareTrigger.TriggerSetParameterValue(triggerchannel, p_ulIndex, p_pvarValue);
	return l_hrOk;
}

