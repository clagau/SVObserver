//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLptExports
//******************************************************************************

#include "StdAfx.h"
#include "SVPlcIOExports.h"
#include "SVPlcIOImpl.h"
#include "SVIOLibrary/SVIOParameterEnum.h"

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

HRESULT WINAPI SVInputSetValue(unsigned long ulChannel, bool bValue)
{
	return E_FAIL;
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

HRESULT WINAPI SVOutputGetValue(unsigned long channel, bool* pValue)
{
	if (nullptr != pValue)
	{
	}
	return S_OK;
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

HRESULT WINAPI SVOutputGetPortValue( unsigned long port, unsigned long* pValue )
{
	HRESULT result {E_FAIL};

	if (port < SvPlc::g_Plc.GetNumPorts())
	{
		
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

HRESULT WINAPI SVOutputSetData(unsigned long triggerchannel, const SvTh::IntVariantMap& rData)
{
	HRESULT result {E_FAIL};

	if (0 < triggerchannel && SvPlc::cNumberTriggers >= triggerchannel)
	{
		result = SvPlc::g_Plc.SetOutputData(triggerchannel, rData);
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

HRESULT WINAPI SVTriggerGetName(unsigned long triggerchannel, BSTR* pName)
{
	HRESULT result {E_FAIL};

	if (nullptr != pName && 0 < triggerchannel && SvPlc::cNumberTriggers >= triggerchannel)
	{
		// free any data on input...
		if (nullptr != *pName)
		{
			::SysFreeString(*pName);
		}

		*pName = SvPlc::g_Plc.GetTriggerName(triggerchannel);
		
		if (nullptr != *pName)
		{
			result = S_OK;
		}
	} 
	return result;
}

HRESULT WINAPI SVTriggerRegister(unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher)
{
	HRESULT result {E_FAIL};

	if ( rDispatcher.hasCallback() && 0 < triggerchannel && SvPlc::cNumberTriggers >= triggerchannel)
	{
		result = S_OK;

		SvPlc::g_Plc.AddDispatcher(triggerchannel, rDispatcher);
	} 
	return result;
}

HRESULT WINAPI SVTriggerUnregister(unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher)
{
	HRESULT result {E_FAIL};

	if (rDispatcher.hasCallback() && 0 < triggerchannel && SvPlc::cNumberTriggers >= triggerchannel)
	{
		result = SvPlc::g_Plc.RemoveDispatcher(triggerchannel, rDispatcher);
	} 
	return result;
}

HRESULT WINAPI SVTriggerUnregisterAll(unsigned long triggerchannel)
{
	HRESULT result {E_FAIL};

	if (0 < triggerchannel && SvPlc::cNumberTriggers >= triggerchannel)
	{
		result = SvPlc::g_Plc.RemoveAllDispatchers(triggerchannel);
	} 
	return result;
}

HRESULT WINAPI SVTriggerStart(unsigned long triggerchannel)
{
	HRESULT result {E_FAIL};

	if (0 < triggerchannel && SvPlc::cNumberTriggers >= triggerchannel)
	{
		result = SvPlc::g_Plc.StartTrigger(triggerchannel);
	}
	return result;
}

HRESULT WINAPI SVTriggerStop(unsigned long triggerchannel)
{
	HRESULT result {E_FAIL};

	if (0 < triggerchannel && SvPlc::cNumberTriggers >= triggerchannel)
	{
		result = SvPlc::g_Plc.StopTrigger(triggerchannel);
	} 
	return result;
}

// SVTriggerGetParameterCount
// Gets the number of parameters
// for the trigger represented by triggerchannel
HRESULT WINAPI SVTriggerGetParameterCount(unsigned long triggerchannel, unsigned long *pCount)
{
	HRESULT result {E_FAIL};

	if (0 < triggerchannel && SvPlc::cNumberTriggers >= triggerchannel)
	{
		result = SvPlc::g_Plc.TriggerGetParameterCount(triggerchannel, pCount);
	}
	return result;
}

// SVTriggerGetParameterName
// Gets the parameter name.
// The caller is responsible for freeing the BSTR.
HRESULT WINAPI SVTriggerGetParameterName(unsigned long triggerchannel, unsigned long index, BSTR* pName)
{
	HRESULT result {E_FAIL};

	if (SvPlc::cNumberTriggers >= triggerchannel)
	{
		result = SvPlc::g_Plc.TriggerGetParameterName(triggerchannel, index, pName);
	}
	return result;
}

// SVTriggerGetParameterValue
// Gets the parameter value for
// the trigger represented by triggerchannel.
HRESULT WINAPI SVTriggerGetParameterValue(unsigned long triggerchannel, unsigned long ulIndex, VARIANT* pValue)
{
	HRESULT result {E_FAIL};

	if (SvPlc::cNumberTriggers >= triggerchannel)
	{
		result = SvPlc::g_Plc.TriggerGetParameterValue(triggerchannel, ulIndex, pValue);
	}
	return result;
}

// SVTriggerSetParameterValue
// Sets the parameter value for
// the trigger represented by triggerchannel.
HRESULT WINAPI SVTriggerSetParameterValue(unsigned long triggerchannel, unsigned long ulIndex, VARIANT* pValue)
{
	HRESULT result {E_FAIL};

	if (SvPlc::cNumberTriggers >= triggerchannel)
	{
		result = SvPlc::g_Plc.TriggerSetParameterValue(triggerchannel, ulIndex, pValue);
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

