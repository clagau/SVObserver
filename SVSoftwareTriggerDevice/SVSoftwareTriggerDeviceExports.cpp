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

std::atomic_long gRefCount = 0;
SVSoftwareTriggerDevice gSoftwareTrigger;

HRESULT WINAPI SVCreate()
{
	HRESULT result {S_OK};

	gRefCount++;
	if (1 == gRefCount)
	{
		result = gSoftwareTrigger.Initialize(true);
	}
	return result;
}

HRESULT WINAPI SVDestroy()
{
	HRESULT result {S_OK};

	gRefCount--;
	if (0 >= gRefCount)
	{
		result = gSoftwareTrigger.Initialize(false);
	}
	return result;
}

HRESULT WINAPI SVTriggerGetCount( unsigned long *pCount )
{
	HRESULT result {E_FAIL};

	if ( nullptr != pCount )
	{
		*pCount = gSoftwareTrigger.GetTriggerCount();

		result = S_OK;
	}

	return result;
}

HRESULT WINAPI SVTriggerGetHandle( unsigned long* pTriggerIndex, unsigned long Index )
{
	HRESULT result {E_FAIL};

	if (nullptr != pTriggerIndex)
	{
		*pTriggerIndex = gSoftwareTrigger.GetTriggerHandle(Index);
		result = S_OK;
	}
	return result;
}

HRESULT WINAPI SVTriggerGetName( unsigned long triggerIndex, BSTR *pName )
{
	HRESULT result {E_FAIL};

	if ( nullptr != pName && 0 < triggerIndex && cMaxSoftwareTriggers >= triggerIndex)
	{
		if (nullptr != *pName)
		{
			::SysFreeString(*pName);
		}
		*pName = gSoftwareTrigger.GetTriggerName(triggerIndex);
		
		if (nullptr != *pName)
		{
			result = S_OK;
		}
	} 
	return result;
}

HRESULT WINAPI SVTriggerRegister( unsigned long triggerIndex, const SvTh::TriggerDispatcher &rDispatcher )
{
	HRESULT result {E_FAIL};

	if (rDispatcher.hasCallback() && 0 < triggerIndex && cMaxSoftwareTriggers >= triggerIndex)
	{
		result = S_OK;

		gSoftwareTrigger.AddDispatcher(triggerIndex, rDispatcher);
	} 
	return result;
}

HRESULT WINAPI SVTriggerUnregister( unsigned long triggerIndex, const SvTh::TriggerDispatcher &rDispatcher)
{
	HRESULT result {E_FAIL};

	if (rDispatcher.hasCallback() && 0 < triggerIndex && cMaxSoftwareTriggers >= triggerIndex)
	{
		result = gSoftwareTrigger.RemoveDispatcher(triggerIndex, rDispatcher);
	} 
	return result;
}

HRESULT WINAPI SVTriggerUnregisterAll( unsigned long triggerIndex )
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && cMaxSoftwareTriggers >= triggerIndex)
	{
		result = gSoftwareTrigger.RemoveAllDispatchers(triggerIndex);
	} 
	return result;
}

HRESULT WINAPI SVTriggerStart( unsigned long triggerIndex )
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && cMaxSoftwareTriggers >= triggerIndex)
	{
		result = gSoftwareTrigger.StartTrigger(triggerIndex);
	} 
	return result;
}

HRESULT WINAPI SVTriggerStop( unsigned long triggerIndex )
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && cMaxSoftwareTriggers >= triggerIndex)
	{
		result = gSoftwareTrigger.StopTrigger(triggerIndex);
	} 
	return result;
}

HRESULT WINAPI SVTriggerGetParameterCount( unsigned long triggerIndex, unsigned long *pCount )
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && cMaxSoftwareTriggers >= triggerIndex)
	{
		result = gSoftwareTrigger.TriggerGetParameterCount(triggerIndex, pCount);
	}
	return result;
}

HRESULT WINAPI SVTriggerGetParameterName( unsigned long triggerIndex, unsigned long p_ulIndex, BSTR *pName )
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && cMaxSoftwareTriggers >= triggerIndex)
	{
		result = gSoftwareTrigger.TriggerGetParameterName(triggerIndex, p_ulIndex, pName);
	}
	return result;
}

HRESULT WINAPI SVTriggerGetParameterValue( unsigned long triggerIndex, unsigned long p_ulIndex, VARIANT *pValue )
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && cMaxSoftwareTriggers >= triggerIndex)
	{
		result = gSoftwareTrigger.TriggerGetParameterValue(triggerIndex, p_ulIndex, pValue);
	}
	return result;
}

HRESULT WINAPI SVTriggerSetParameterValue( unsigned long triggerIndex, unsigned long p_ulIndex, VARIANT *pValue )
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && cMaxSoftwareTriggers >= triggerIndex)
	{
		result = gSoftwareTrigger.TriggerSetParameterValue(triggerIndex, p_ulIndex, pValue);
	}
	return result;
}

