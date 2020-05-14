//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLptExports
//******************************************************************************

#include "StdAfx.h"
#include "SVPlcIOExports.h"
#include "SVPlcIOImpl.h"

std::atomic_long g_RefCount = 0;

HRESULT WINAPI SVCreate()
{
	HRESULT result {S_OK};

	g_RefCount++;
	if (1 == g_RefCount)
	{
		result = SvPlc::g_Plc.Initialize(true);
	}
	return result;
}

HRESULT WINAPI SVDestroy()
{
	HRESULT result {S_OK};

	g_RefCount--;
	if (0 >= g_RefCount)
	{
		result = SvPlc::g_Plc.Initialize(false);
	}
	return result;
}

HRESULT WINAPI SVInputGetCount(unsigned long* pulCount)
{
	HRESULT result {E_FAIL};

	if (nullptr != pulCount)
	{
		*pulCount = SvPlc::g_Plc.GetInputCount();
		result = S_OK;
	}
	return result;
}

HRESULT WINAPI SVInputGetValue( unsigned long ulChannel, bool* pbValue )
{ 
	HRESULT result {E_FAIL};

	if (nullptr != pbValue)
	{
		if (ulChannel < SvPlc::g_Plc.GetInputCount())
		{
			bool bState;
			result = SvPlc::g_Plc.GetInputBit(ulChannel, bState);

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
		result = SvPlc::g_Plc.GetInputValue(pValue);
	}
	return result;
}

HRESULT WINAPI SVOutputGetCount(unsigned long* pCount)
{
	HRESULT result {E_FAIL};

	if (nullptr != pCount)
	{
		*pCount = SvPlc::g_Plc.GetOutputCount();
		result = S_OK;
	}
	return result;
}

HRESULT WINAPI SVOutputSetValue(unsigned long channel, bool bValue)
{
	HRESULT result {E_FAIL};

	if (channel < SvPlc::g_Plc.GetOutputCount())
	{
		result = SvPlc::g_Plc.SetOutputBit(channel, bValue);
	}
	return result;
}

HRESULT WINAPI SVOutputGetPortCount(unsigned long* pCount)
{
	HRESULT result {E_FAIL};

	if (nullptr != pCount)
	{
		*pCount = SvPlc::g_Plc.GetPortCount();
		result = S_OK;
	}
	return result;
}

HRESULT WINAPI SVOutputSetPortValue(unsigned long port, unsigned long value)
{
	HRESULT result {E_FAIL};

	if (port < SvPlc::g_Plc.GetPortCount())
	{
		result = SvPlc::g_Plc.SetOutputValue(value);
	}
	return result;
}

HRESULT WINAPI SVOutputSetData(unsigned long triggerIndex, const SvTh::IntVariantMap& rData)
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && SvPlc::cNumberTriggers >= triggerIndex)
	{
		result = SvPlc::g_Plc.SetOutputData(triggerIndex, rData);
	}
	return result;
}

HRESULT WINAPI SVTriggerGetCount(unsigned long* pCount)
{
	HRESULT result {E_FAIL};

	if (nullptr != pCount)
	{
		*pCount = SvPlc::g_Plc.GetTriggerCount();
		result = S_OK;
	}
	return result;
}

HRESULT WINAPI SVTriggerGetHandle(unsigned long* pTriggerchannel, unsigned long index)
{
	HRESULT result {E_FAIL};

	if (nullptr != pTriggerchannel)
	{
		*pTriggerchannel = SvPlc::g_Plc.GetTriggerHandle(index);
		result = S_OK;
	}
	return result;
}

HRESULT WINAPI SVTriggerGetName(unsigned long triggerIndex, BSTR* pName)
{
	HRESULT result {E_FAIL};

	if (nullptr != pName && 0 < triggerIndex && SvPlc::cNumberTriggers >= triggerIndex)
	{
		// free any data on input...
		if (nullptr != *pName)
		{
			::SysFreeString(*pName);
		}

		*pName = SvPlc::g_Plc.GetTriggerName(triggerIndex);
		
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

	if ( rDispatcher.hasCallback() && 0 < triggerIndex && SvPlc::cNumberTriggers >= triggerIndex)
	{
		result = S_OK;

		SvPlc::g_Plc.AddDispatcher(triggerIndex, rDispatcher);
	} 
	return result;
}

HRESULT WINAPI SVTriggerUnregister(unsigned long triggerIndex, const SvTh::TriggerDispatcher &rDispatcher)
{
	HRESULT result {E_FAIL};

	if (rDispatcher.hasCallback() && 0 < triggerIndex && SvPlc::cNumberTriggers >= triggerIndex)
	{
		result = SvPlc::g_Plc.RemoveDispatcher(triggerIndex, rDispatcher);
	} 
	return result;
}

HRESULT WINAPI SVTriggerUnregisterAll(unsigned long triggerIndex)
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && SvPlc::cNumberTriggers >= triggerIndex)
	{
		result = SvPlc::g_Plc.RemoveAllDispatchers(triggerIndex);
	} 
	return result;
}

HRESULT WINAPI SVTriggerStart(unsigned long triggerIndex)
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && SvPlc::cNumberTriggers >= triggerIndex)
	{
		result = SvPlc::g_Plc.StartTrigger(triggerIndex);
	}
	return result;
}

HRESULT WINAPI SVTriggerStop(unsigned long triggerIndex)
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && SvPlc::cNumberTriggers >= triggerIndex)
	{
		result = SvPlc::g_Plc.StopTrigger(triggerIndex);
	} 
	return result;
}

// SVTriggerGetParameterCount
// Gets the number of parameters
// for the trigger represented by triggerchannel
HRESULT WINAPI SVTriggerGetParameterCount(unsigned long triggerIndex, unsigned long *pCount)
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && SvPlc::cNumberTriggers >= triggerIndex)
	{
		result = SvPlc::g_Plc.TriggerGetParameterCount(triggerIndex, pCount);
	}
	return result;
}

// SVTriggerGetParameterName
// Gets the parameter name.
// The caller is responsible for freeing the BSTR.
HRESULT WINAPI SVTriggerGetParameterName(unsigned long triggerIndex, unsigned long index, BSTR* pName)
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && SvPlc::cNumberTriggers >= triggerIndex)
	{
		result = SvPlc::g_Plc.TriggerGetParameterName(triggerIndex, index, pName);
	}
	return result;
}

// SVTriggerGetParameterValue
// Gets the parameter value for
// the trigger represented by triggerIndex.
HRESULT WINAPI SVTriggerGetParameterValue(unsigned long triggerIndex, unsigned long ulIndex, VARIANT* pValue)
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && SvPlc::cNumberTriggers >= triggerIndex)
	{
		result = SvPlc::g_Plc.TriggerGetParameterValue(triggerIndex, ulIndex, pValue);
	}
	return result;
}

// SVTriggerSetParameterValue
// Sets the parameter value for
// the trigger represented by triggerIndex.
HRESULT WINAPI SVTriggerSetParameterValue(unsigned long triggerIndex, unsigned long ulIndex, VARIANT* pValue)
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && SvPlc::cNumberTriggers >= triggerIndex)
	{
		result = SvPlc::g_Plc.TriggerSetParameterValue(triggerIndex, ulIndex, pValue);
	}
	return result;
}

// Digital I/O Parameters..No handles...

// SVGetParameterCount
// This function returns the number of available parameters.
HRESULT WINAPI SVGetParameterCount(unsigned long* pCount)
{
	return SvPlc::g_Plc.GetParameterCount(pCount);
}

// SVGetParameterName
// This function creates a BSTR with the name of the parameter
// It is the responsibility of the caller to free the BSTR.
HRESULT WINAPI SVGetParameterName(unsigned long index, BSTR* pName)
{
	return SvPlc::g_Plc.GetParameterName(index, pName);
}

// SVGetParameterValue
// This function Gets the parameter value specified by ulIndex.
HRESULT WINAPI SVGetParameterValue(unsigned long index, VARIANT* pValue)
{
	return SvPlc::g_Plc.GetParameterValue(index, pValue);
}

// SVSetParameterValue
// This function sets the value specified by ulIndex.
HRESULT WINAPI SVSetParameterValue(unsigned long index, VARIANT* pValue)
{
	return SvPlc::g_Plc.SetParameterValue(index, pValue);
}

