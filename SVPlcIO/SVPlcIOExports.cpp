//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLptExports
//******************************************************************************

#include "StdAfx.h"
#include "SVPlcIOExports.h"
#include "SVPlcIOImpl.h"

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

HRESULT WINAPI SVInputGetCount(unsigned long* pulCount)
{
	HRESULT result {E_FAIL};

	if (nullptr != pulCount)
	{
		*pulCount = gPlc.GetInputCount();
		result = S_OK;
	}
	return result;
}

HRESULT WINAPI SVInputGetValue( unsigned long ulChannel, bool* pbValue )
{ 
	HRESULT result {E_FAIL};

	if (nullptr != pbValue)
	{
		if (ulChannel < gPlc.GetInputCount())
		{
			bool bState;
			result = gPlc.GetInputBit(ulChannel, bState);

			if (S_OK == result)
			{
				*pbValue = bState;
			}
		}
	}
	return result;
}

HRESULT WINAPI SVInputGetValues(unsigned long* pValue)
{
	HRESULT result {E_FAIL};

	if (nullptr != pValue)
	{
		result = gPlc.GetInputValue(pValue);
	}
	return result;
}

HRESULT WINAPI SVOutputGetCount(unsigned long* pCount)
{
	HRESULT result {E_FAIL};

	if (nullptr != pCount)
	{
		*pCount = gPlc.GetOutputCount();
		result = S_OK;
	}
	return result;
}

HRESULT WINAPI SVOutputSetValue(unsigned long channel, bool bValue)
{
	HRESULT result {E_FAIL};

	if (channel < gPlc.GetOutputCount())
	{
		result = gPlc.SetOutputBit(channel, bValue);
	}
	return result;
}

HRESULT WINAPI SVOutputGetPortCount(unsigned long* pCount)
{
	HRESULT result {E_FAIL};

	if (nullptr != pCount)
	{
		*pCount = gPlc.GetPortCount();
		result = S_OK;
	}
	return result;
}

HRESULT WINAPI SVOutputSetPortValue(unsigned long port, unsigned long value)
{
	HRESULT result {E_FAIL};

	if (port < gPlc.GetPortCount())
	{
		result = gPlc.SetOutputValue(value);
	}
	return result;
}

HRESULT WINAPI SVOutputSetData(unsigned long triggerIndex, const SvTh::IntVariantMap& rData)
{
	HRESULT result {E_FAIL};

	if (SvPlc::cMaxPlcTriggers >= triggerIndex)
	{
		result = gPlc.SetOutputData(triggerIndex, rData);
	}
	return result;
}

HRESULT WINAPI SVTriggerGetCount(unsigned long* pCount)
{
	HRESULT result {E_FAIL};

	if (nullptr != pCount)
	{
		*pCount = gPlc.GetTriggerCount();
		result = S_OK;
	}
	return result;
}

HRESULT WINAPI SVTriggerGetHandle(unsigned long* pTriggerchannel, unsigned long index)
{
	HRESULT result {E_FAIL};

	if (nullptr != pTriggerchannel)
	{
		*pTriggerchannel = gPlc.GetTriggerHandle(index);
		result = S_OK;
	}
	return result;
}

HRESULT WINAPI SVTriggerGetName(unsigned long triggerIndex, BSTR* pName)
{
	HRESULT result {E_FAIL};

	if (nullptr != pName && 0 < triggerIndex && SvPlc::cMaxPlcTriggers >= triggerIndex)
	{
		// free any data on input...
		if (nullptr != *pName)
		{
			::SysFreeString(*pName);
		}

		*pName = gPlc.GetTriggerName(triggerIndex);
		
		if (nullptr != *pName)
		{
			result = S_OK;
		}
	} 
	return result;
}

HRESULT WINAPI SVTriggerRegister(unsigned long triggerIndex, const SvTh::TriggerDispatcher &rDispatcher)
{
	HRESULT result {E_FAIL};

	if ( rDispatcher.hasCallback() && 0 < triggerIndex && SvPlc::cMaxPlcTriggers >= triggerIndex)
	{
		result = S_OK;

		gPlc.AddDispatcher(triggerIndex, rDispatcher);
	} 
	return result;
}

HRESULT WINAPI SVTriggerUnregister(unsigned long triggerIndex, const SvTh::TriggerDispatcher &rDispatcher)
{
	HRESULT result {E_FAIL};

	if (rDispatcher.hasCallback() && 0 < triggerIndex && SvPlc::cMaxPlcTriggers >= triggerIndex)
	{
		result = gPlc.RemoveDispatcher(triggerIndex, rDispatcher);
	} 
	return result;
}

HRESULT WINAPI SVTriggerUnregisterAll(unsigned long triggerIndex)
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && SvPlc::cMaxPlcTriggers >= triggerIndex)
	{
		result = gPlc.RemoveAllDispatchers(triggerIndex);
	} 
	return result;
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

HRESULT WINAPI SVTriggerGetParameterCount(unsigned long triggerIndex, unsigned long *pCount)
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && SvPlc::cMaxPlcTriggers >= triggerIndex)
	{
		result = gPlc.TriggerGetParameterCount(triggerIndex, pCount);
	}
	return result;
}

HRESULT WINAPI SVTriggerGetParameterName(unsigned long triggerIndex, unsigned long index, BSTR* pName)
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && SvPlc::cMaxPlcTriggers >= triggerIndex)
	{
		result = gPlc.TriggerGetParameterName(triggerIndex, index, pName);
	}
	return result;
}

HRESULT WINAPI SVTriggerGetParameterValue(unsigned long triggerIndex, unsigned long ulIndex, VARIANT* pValue)
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && SvPlc::cMaxPlcTriggers >= triggerIndex)
	{
		result = gPlc.TriggerGetParameterValue(triggerIndex, ulIndex, pValue);
	}
	return result;
}

HRESULT WINAPI SVTriggerSetParameterValue(unsigned long triggerIndex, unsigned long ulIndex, VARIANT* pValue)
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && SvPlc::cMaxPlcTriggers >= triggerIndex)
	{
		result = gPlc.TriggerSetParameterValue(triggerIndex, ulIndex, pValue);
	}
	return result;
}

HRESULT WINAPI SVGetParameterCount(unsigned long* pCount)
{
	return gPlc.GetParameterCount(pCount);
}

HRESULT WINAPI SVGetParameterName(unsigned long index, BSTR* pName)
{
	return gPlc.GetParameterName(index, pName);
}

HRESULT WINAPI SVGetParameterValue(unsigned long index, VARIANT* pValue)
{
	return gPlc.GetParameterValue(index, pValue);
}

HRESULT WINAPI SVSetParameterValue(unsigned long index, VARIANT* pValue)
{
	return gPlc.SetParameterValue(index, pValue);
}

