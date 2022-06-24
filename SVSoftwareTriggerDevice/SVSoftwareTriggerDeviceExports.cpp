//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
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
		result = gSoftwareTrigger.Initialize();
	}
	return result;
}

HRESULT WINAPI SVDestroy()
{
	HRESULT result {S_OK};

	gRefCount--;
	if (0 >= gRefCount)
	{
		result = gSoftwareTrigger.Destroy();
	}
	return result;
}

unsigned long WINAPI SVTriggerGetCount()
{
	return gSoftwareTrigger.GetTriggerCount();
}

unsigned long WINAPI SVTriggerGetHandle(unsigned long index)
{
	return gSoftwareTrigger.GetTriggerHandle(index);
}

_variant_t WINAPI SVTriggerGetName( unsigned long triggerIndex)
{
	_variant_t result;

	if (0 < triggerIndex && cMaxSoftwareTriggers >= triggerIndex)
	{
		result = gSoftwareTrigger.GetTriggerName(triggerIndex);
	} 
	return result;
}

HRESULT WINAPI SVTriggerRegister( unsigned long triggerIndex, SvTrig::TriggerCallBack pTriggerCallback)
{
	if (0 < triggerIndex && cMaxSoftwareTriggers >= triggerIndex)
	{
		return gSoftwareTrigger.RegisterCallback(triggerIndex, pTriggerCallback);
	} 
	return E_FAIL;
}

HRESULT WINAPI SVTriggerUnregister( unsigned long triggerIndex)
{
	if (0 < triggerIndex && cMaxSoftwareTriggers >= triggerIndex)
	{
		return gSoftwareTrigger.UnRegisterCallback(triggerIndex);
	} 
	return E_FAIL;
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

unsigned long WINAPI SVTriggerGetParameterCount(unsigned long triggerIndex)
{
	unsigned long result {0UL};

	if (0 < triggerIndex && cMaxSoftwareTriggers >= triggerIndex)
	{
		result = gSoftwareTrigger.TriggerGetParameterCount(triggerIndex);
	}
	return result;
}

_variant_t WINAPI SVTriggerGetParameterName( unsigned long triggerIndex, unsigned long index)
{
	_variant_t result;

	if (0 < triggerIndex && cMaxSoftwareTriggers >= triggerIndex)
	{
		result = gSoftwareTrigger.TriggerGetParameterName(triggerIndex, index);
	}
	return result;
}

_variant_t WINAPI SVTriggerGetParameterValue( unsigned long triggerIndex, unsigned long index)
{
	_variant_t result;

	if (0 < triggerIndex && cMaxSoftwareTriggers >= triggerIndex)
	{
		result = gSoftwareTrigger.TriggerGetParameterValue(triggerIndex, index);
	}
	return result;
}

HRESULT WINAPI SVTriggerSetParameterValue( unsigned long triggerIndex, unsigned long p_ulIndex, const _variant_t& rValue)
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && cMaxSoftwareTriggers >= triggerIndex)
	{
		result = gSoftwareTrigger.TriggerSetParameterValue(triggerIndex, p_ulIndex, rValue);
	}
	return result;
}

