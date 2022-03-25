//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLptExports
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "SVLptIOExports.h"
#include "SVLptIOImpl.h"
#pragma endregion Includes

std::atomic_ulong gRefCount{0UL};
SVLptIOImpl gLpt;

HRESULT WINAPI SVCreate()
{
	HRESULT result {S_OK};

	gRefCount++;
	if (1 == gRefCount)
	{
		result = gLpt.Initialize(true);
	}
	return result;
}

HRESULT WINAPI SVDestroy()
{
	HRESULT result {S_OK};

	gRefCount--;
	if (0 == gRefCount)
	{
		result = gLpt.Initialize(false);
	}
	return result;
}

unsigned long WINAPI SVInputGetCount()
{
	return gLpt.GetInputCount();
}

bool WINAPI SVInputGetValue( unsigned long channel)
{ 
	bool result{false};

	if (channel < gLpt.GetInputCount())
	{
		result = gLpt.GetInputBit(channel);
	}
	return result;
}

// ulPort represents the physical port number (LPT 1/2/3/4)
unsigned long WINAPI SVInputGetValues()
{
	unsigned long result{0UL};

	result = gLpt.GetInputValue();
	return result;
}

unsigned long WINAPI SVOutputGetCount()
{
	return gLpt.GetOutputCount();
}

HRESULT WINAPI SVOutputSetValue(unsigned long channel, bool Value)
{
	HRESULT result {E_FAIL};

	if (channel < gLpt.GetOutputCount())
	{
		result = gLpt.SetOutputBit(channel, Value);
	}
	return result;
}

unsigned long WINAPI SVOutputGetPortCount()
{
	return gLpt.GetPortCount();
}

HRESULT WINAPI SVOutputSetPortValue(unsigned long port, unsigned long Value)
{
	HRESULT result {E_FAIL};

	if (port < gLpt.GetPortCount())
	{
		result = gLpt.SetPortOutputValue(port, Value);
	}
	return result;
}

unsigned long WINAPI SVTriggerGetCount()
{
	return gLpt.GetTriggerCount();
}

unsigned long WINAPI SVTriggerGetHandle(unsigned long Index)
{
	return gLpt.GetTriggerHandle(Index);
}

_variant_t WINAPI SVTriggerGetName(unsigned long triggerIndex)
{
	_variant_t result;

	if (0 < triggerIndex && cMaxLptTriggers >= triggerIndex)
	{
		result = gLpt.GetTriggerName(triggerIndex);
	} 
	return result;
}

HRESULT WINAPI SVTriggerRegister(unsigned long triggerIndex, SvTrig::TriggerCallBack pTriggerCallback)
{
	if (0 < triggerIndex && cMaxLptTriggers >= triggerIndex)
	{
		return gLpt.RegisterCallback(triggerIndex, pTriggerCallback);
	} 
	return E_FAIL;
}

HRESULT WINAPI SVTriggerUnregister(unsigned long triggerIndex)
{
	if (0 < triggerIndex && cMaxLptTriggers >= triggerIndex)
	{
		return gLpt.UnRegisterCallback(triggerIndex);
	} 
	return E_FAIL;
}

HRESULT WINAPI SVTriggerStart(unsigned long triggerIndex)
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && cMaxLptTriggers >= triggerIndex)
	{
		result = gLpt.StartTrigger(triggerIndex);
	} 
	return result;
}

HRESULT WINAPI SVTriggerStop(unsigned long triggerIndex)
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && cMaxLptTriggers >= triggerIndex)
	{
		result = gLpt.StopTrigger(triggerIndex);
	} 
	return result;
}

unsigned long WINAPI SVTriggerGetParameterCount(unsigned long triggerIndex)
{
	unsigned long result {0UL};

	if (0 < triggerIndex && cMaxLptTriggers >= triggerIndex)
	{
		result = gLpt.TriggerGetParameterCount(triggerIndex);
	}
	return result;
}

_variant_t WINAPI SVTriggerGetParameterName(unsigned long triggerIndex, unsigned long index)
{
	_variant_t result;

	if (0 < triggerIndex && cMaxLptTriggers >= triggerIndex)
	{
		result = gLpt.TriggerGetParameterName(triggerIndex, index);
	}
	return result;
}

_variant_t WINAPI SVTriggerGetParameterValue(unsigned long triggerIndex, unsigned long index)
{
	_variant_t result;

	if (0 < triggerIndex && cMaxLptTriggers >= triggerIndex)
	{
		result = gLpt.TriggerGetParameterValue(triggerIndex, index);
	}
	return result;
}

HRESULT WINAPI SVTriggerSetParameterValue(unsigned long triggerIndex, unsigned long index, const _variant_t& rValue)
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && cMaxLptTriggers >= triggerIndex)
	{
		result = gLpt.TriggerSetParameterValue(triggerIndex, index, rValue);
	}
	return result;
}

unsigned long WINAPI SVGetParameterCount()
{
	return gLpt.GetParameterCount();
}

_variant_t WINAPI SVGetParameterName(unsigned long index)
{
	return gLpt.GetParameterName(index);
}

_variant_t WINAPI SVGetParameterValue(unsigned long index)
{
	return gLpt.GetParameterValue(index);
}

HRESULT WINAPI SVSetParameterValue(unsigned long index, const _variant_t& rValue)
{
	return gLpt.SetParameterValue(index, rValue);
}

