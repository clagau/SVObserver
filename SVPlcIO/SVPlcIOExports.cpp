//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \file SVPlcIOExports.cpp
/// \brief Exported functions of the DLL
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "SVPlcIOExports.h"
#include "SVPlcIOImpl.h"
#pragma endregion Includes

std::atomic_ulong gRefCount{0UL};
SvPlc::SVPlcIOImpl gPlc;

HRESULT WINAPI SVCreate()
{
	HRESULT result {S_OK};

	gRefCount++;
	if (1 == gRefCount)
	{
		result = gPlc.Initialize(true);
	}
	return result;
}

HRESULT WINAPI SVDestroy()
{
	HRESULT result {S_OK};

	gRefCount--;
	if (0 == gRefCount)
	{
		result = gPlc.Initialize(false);
	}
	return result;
}

unsigned long WINAPI SVInputGetCount()
{
	return gPlc.GetInputCount();
}

bool WINAPI SVInputGetValue(unsigned long)
{ 
	return false;
}

unsigned long WINAPI SVInputGetValues()
{
	return 0UL;
}

unsigned long WINAPI SVOutputGetCount()
{
	return gPlc.GetOutputCount();
}

HRESULT WINAPI SVOutputSetValue(unsigned long, bool )
{
	return S_OK;
}

unsigned long WINAPI SVOutputGetPortCount()
{
	return 0UL;
}

HRESULT WINAPI SVOutputSetPortValue(unsigned long , unsigned long )
{
	return S_OK;
}

HRESULT WINAPI SVOutputSetData(unsigned long triggerIndex, const SvTrig::TriggerData& rData)
{
	HRESULT result {E_FAIL};

	if (SvPlc::cMaxPlcTriggers >= triggerIndex)
	{
		result = gPlc.SetOutputData(triggerIndex, rData);
	}
	return result;
}

unsigned long WINAPI SVTriggerGetCount()
{
	return gPlc.GetTriggerCount();
}

unsigned long WINAPI SVTriggerGetHandle(unsigned long index)
{
	return gPlc.GetTriggerHandle(index);
}

_variant_t WINAPI SVTriggerGetName(unsigned long triggerIndex)
{
	_variant_t result;

	if (0 < triggerIndex && SvPlc::cMaxPlcTriggers >= triggerIndex)
	{
		result = gPlc.GetTriggerName(triggerIndex);
	} 
	return result;
}

HRESULT WINAPI SVTriggerRegister(unsigned long triggerIndex, SvTrig::TriggerCallBack pTriggerCallback)
{
	if (0 < triggerIndex && SvPlc::cMaxPlcTriggers >= triggerIndex)
	{
		return gPlc.RegisterCallback(triggerIndex, pTriggerCallback);
	} 
	return E_FAIL;
}

HRESULT WINAPI SVTriggerUnregister(unsigned long triggerIndex)
{
	if (0 < triggerIndex && SvPlc::cMaxPlcTriggers >= triggerIndex)
	{
		return gPlc.UnRegisterCallback(triggerIndex);
	} 
	return E_FAIL;
}

HRESULT WINAPI SVTriggerStart(unsigned long triggerIndex)
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && SvPlc::cMaxPlcTriggers >= triggerIndex)
	{
		result = gPlc.StartTrigger(triggerIndex);
	}
	return result;
}

HRESULT WINAPI SVTriggerStop(unsigned long triggerIndex)
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && SvPlc::cMaxPlcTriggers >= triggerIndex)
	{
		result = gPlc.StopTrigger(triggerIndex);
	} 
	return result;
}

unsigned long WINAPI SVTriggerGetParameterCount(unsigned long triggerIndex)
{
	unsigned long result{ 0UL };

	if (0 < triggerIndex && SvPlc::cMaxPlcTriggers >= triggerIndex)
	{
		result = gPlc.TriggerGetParameterCount(triggerIndex);
	}
	return result;
}

_variant_t WINAPI SVTriggerGetParameterName(unsigned long triggerIndex, unsigned long index)
{
	_variant_t result;

	if (0 < triggerIndex && SvPlc::cMaxPlcTriggers >= triggerIndex)
	{
		result = gPlc.TriggerGetParameterName(triggerIndex, index);
	}
	return result;
}

_variant_t WINAPI SVTriggerGetParameterValue(unsigned long triggerIndex, unsigned long index)
{
	_variant_t result;

	if (0 < triggerIndex && SvPlc::cMaxPlcTriggers >= triggerIndex)
	{
		result = gPlc.TriggerGetParameterValue(triggerIndex, index);
	}
	return result;
}

HRESULT WINAPI SVTriggerSetParameterValue(unsigned long triggerIndex, unsigned long ulIndex,  _variant_t rValue)
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && SvPlc::cMaxPlcTriggers >= triggerIndex)
	{
		result = gPlc.TriggerSetParameterValue(triggerIndex, ulIndex, rValue);
	}
	return result;
}

unsigned long WINAPI SVGetParameterCount()
{
	return gPlc.GetParameterCount();
}

_variant_t WINAPI SVGetParameterName(unsigned long index)
{
	return gPlc.GetParameterName(index);
}

_variant_t WINAPI SVGetParameterValue(unsigned long index)
{
	return gPlc.GetParameterValue(index);
}

HRESULT WINAPI SVSetParameterValue(unsigned long index, const _variant_t& rValue)
{
	return gPlc.SetParameterValue(index, rValue);
}

