//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLptExports
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "SVLptIOExports.h"
#include "SVLptIOImpl.h"
#include "TriggerHandling/TriggerDispatcher.h"
#pragma endregion Includes

long g_lRefCount = 0;
SVLptIOImpl g_Lpt;

static HRESULT WINAPI SVLPTDestroy(bool bClose)
{
	HRESULT hr = S_OK;

	if (bClose || ::InterlockedDecrement(&g_lRefCount) <= 0)
	{
		::InterlockedExchange(&g_lRefCount, 0);
		hr = g_Lpt.Initialize(false);
	}
	return hr;
}

HRESULT WINAPI SVCreate()
{
	HRESULT hr = S_OK;

	if (1 == ::InterlockedIncrement( &g_lRefCount))
	{
		hr = g_Lpt.Initialize(true);

		g_Lpt.m_lLptTriggerEdge = 0;
		g_Lpt.m_lIOBrdTriggerEdge = 0;
		if (S_OK == hr)
		{
			// Trigger Section

			// Input Section
			if (S_OK == hr)
			{
			}

			// Output Section
			if (S_OK == hr)
			{
			}

			if (S_OK != hr)
			{
				SVLPTDestroy(true);
			}
		}
	}
	return hr;
}

HRESULT WINAPI SVDestroy()
{
	HRESULT hr = S_OK;
	SVLPTDestroy(false);
	return hr;
}

HRESULT WINAPI SVInputGetCount(unsigned long* pulCount)
{
	HRESULT hr = S_FALSE;

	if (nullptr != pulCount)
	{
		*pulCount = g_Lpt.GetInputCount();
		hr = S_OK;
	}
	return hr;
}

HRESULT WINAPI SVInputGetValue( unsigned long ulChannel, bool* pbValue )
{ 
	HRESULT hr = S_FALSE;

	if (nullptr != pbValue)
	{
		if (0 <= ulChannel && ulChannel < g_Lpt.GetInputCount())
		{
			bool bState;
			hr = g_Lpt.GetInputBit(ulChannel, bState);

			if (S_OK == hr)
			{
				*pbValue = bState;
			}
		}
	}
	return hr;
}

HRESULT WINAPI SVInputSetValue(unsigned long ulChannel, bool bValue)
{
	HRESULT hr = S_FALSE;
	return hr;
}

HRESULT WINAPI SVInputGetPortCount(unsigned long* pulCount)
{
	HRESULT hr = S_FALSE;

	if (nullptr != pulCount)
	{
		*pulCount = g_Lpt.GetPortCount();
		hr = S_OK;
	}
	return hr;
}

// ulPort represents the physical port number (LPT 1/2/3/4)
HRESULT WINAPI SVInputGetPortValue(unsigned long ulPort, unsigned long* pulValue)
{
	HRESULT hr = S_FALSE;

	if (nullptr != pulValue)
	{
		if (0 <= ulPort && ulPort < g_Lpt.GetNumPorts())
		{
			unsigned long lState = 0;
			hr = g_Lpt.GetPortInputValue(ulPort, &lState);
			if (S_OK == hr)
			{
				*pulValue = lState;
			}
		}
	}
	return hr;
}

HRESULT WINAPI SVInputSetPortValue(unsigned long ulPort, unsigned long ulValue)
{
	HRESULT hr = S_FALSE;
	return hr;
}

HRESULT WINAPI SVOutputGetCount(unsigned long* pulCount)
{
	HRESULT hr = S_FALSE;

	if (nullptr != pulCount)
	{
		*pulCount = g_Lpt.GetOutputCount();
		hr = S_OK;
	}
	return hr;
}

HRESULT WINAPI SVOutputGetValue(unsigned long ulChannel, bool* pbValue)
{
	if (nullptr != pbValue)
	{
		long port = ulChannel / 8;
		*pbValue = (g_Lpt.m_nPreviousOutputs[port] & ( 1 << (ulChannel % 8) )) != 0;
	}
	return S_OK;
}

HRESULT WINAPI SVOutputSetValue(unsigned long ulChannel, bool bValue)
{
	HRESULT hr = S_FALSE;

	if (0 <= ulChannel && ulChannel < g_Lpt.GetOutputCount())
	{
		hr = g_Lpt.SetOutputBit(ulChannel, bValue);
	}
	return hr;
}

HRESULT WINAPI SVOutputGetPortCount(unsigned long* pulCount)
{
	HRESULT hr = S_FALSE;

	if (nullptr != pulCount)
	{
		*pulCount = g_Lpt.GetPortCount();
		hr = S_OK;
	}
	return hr;
}

HRESULT WINAPI SVOutputGetPortValue( unsigned long ulPort, unsigned long* pulValue )
{
	HRESULT hr = S_FALSE;

	if (0 <= ulPort && ulPort < g_Lpt.GetNumPorts())
	{
		*pulValue = g_Lpt.m_nPreviousOutputs[0] | 
			(g_Lpt.m_nPreviousOutputs[1] << 8) | 
			(g_Lpt.m_nPreviousOutputs[2] << 16);
	}
	return hr;
}

HRESULT WINAPI SVOutputSetPortValue(unsigned long ulPort, unsigned long ulValue)
{
	HRESULT hr = S_FALSE;

	if (0 <= ulPort && ulPort < g_Lpt.GetPortCount())
	{
		hr = g_Lpt.SetPortOutputValue(ulPort, ulValue);
	}
	return hr;
}

HRESULT WINAPI SVTriggerGetCount(unsigned long* pulCount)
{
	HRESULT hr = S_FALSE;

	if (nullptr != pulCount)
	{
		*pulCount = g_Lpt.GetTriggerCount();
		hr = S_OK;
	}
	return hr;
}

HRESULT WINAPI SVTriggerGetHandle(unsigned long* pTriggerchannel, unsigned long ulIndex)
{
	HRESULT hr = S_FALSE;

	if (nullptr != pTriggerchannel)
	{
		*pTriggerchannel = g_Lpt.GetTriggerHandle(ulIndex);
		hr = S_OK;
	}
	return hr;
}

HRESULT WINAPI SVTriggerGetName(unsigned long triggerchannel, BSTR* pbstrName)
{
	HRESULT hr = S_FALSE;

	if (nullptr != pbstrName && 0 < triggerchannel)
	{
		// free any data on input...
		if (nullptr != *pbstrName)
		{
			::SysFreeString(*pbstrName);
		}

		*pbstrName = g_Lpt.GetTriggerName(triggerchannel);
		
		if (nullptr != *pbstrName)
		{
			hr = S_OK;
		}
	} 
	return hr;
}

HRESULT WINAPI SVTriggerRegister(unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher)
{
	HRESULT hr = S_FALSE;

	if ( rDispatcher.hasCallback() && 0 < triggerchannel)
	{
		hr = S_OK;

		g_Lpt.AddDispatcher(triggerchannel, rDispatcher);
	} 
	return hr;
}

HRESULT WINAPI SVTriggerUnregister(unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher)
{
	HRESULT hr = S_FALSE;

	if (rDispatcher.hasCallback() && 0 < triggerchannel)
	{
		hr = g_Lpt.RemoveDispatcher(triggerchannel, rDispatcher);
	} 
	return hr;
}

HRESULT WINAPI SVTriggerUnregisterAll(unsigned long triggerchannel)
{
	HRESULT hr = S_FALSE;

	if (0 < triggerchannel)
	{
		hr = g_Lpt.RemoveAllDispatchers(triggerchannel);
	} 
	return hr;
}

HRESULT WINAPI SVTriggerStart(unsigned long triggerchannel)
{
	HRESULT hr = S_FALSE;

	if (0 < triggerchannel)
	{
		hr = g_Lpt.StartTrigger(triggerchannel);
	} 
	return hr;
}

HRESULT WINAPI SVTriggerStop(unsigned long triggerchannel)
{
	HRESULT hr = S_FALSE;

	if (0 < triggerchannel)
	{
		hr = g_Lpt.StopTrigger(triggerchannel);
	} 
	return hr;
}

// SVTriggerGetParameterCount
// Gets the number of parameters
// for the trigger represented by triggerchannel
HRESULT WINAPI SVTriggerGetParameterCount(unsigned long triggerchannel, unsigned long *pulCount)
{
	return g_Lpt.TriggerGetParameterCount(triggerchannel, pulCount);
}

// SVTriggerGetParameterName
// Gets the parameter name.
// The caller is responsible for freeing the BSTR.
HRESULT WINAPI SVTriggerGetParameterName(unsigned long triggerchannel, unsigned long ulIndex, BSTR* pbstrName)
{
	return g_Lpt.TriggerGetParameterName( triggerchannel, ulIndex, pbstrName);
}

// SVTriggerGetParameterValue
// Gets the parameter value for
// the trigger represented by triggerchannel.
HRESULT WINAPI SVTriggerGetParameterValue(unsigned long triggerchannel, unsigned long ulIndex, VARIANT* pvarValue)
{
	return g_Lpt.TriggerGetParameterValue(triggerchannel, ulIndex, pvarValue);
}

// SVTriggerSetParameterValue
// Sets the parameter value for
// the trigger represented by triggerchannel.
HRESULT WINAPI SVTriggerSetParameterValue(unsigned long triggerchannel, unsigned long ulIndex, VARIANT* pvarValue)
{
	return g_Lpt.TriggerSetParameterValue(triggerchannel, ulIndex, pvarValue);
}

// Digital I/O Parameters..No handles...

// SVGetParameterCount
// This function returns the number of available parameters.
HRESULT WINAPI SVGetParameterCount(unsigned long* pulCount)
{
	return g_Lpt.GetParameterCount(pulCount);
}

// SVGetParameterName
// This function creates a BSTR with the name of the parameter
// It is the responsibility of the caller to free the BSTR.
HRESULT WINAPI SVGetParameterName(unsigned long ulIndex, BSTR* pbstrName)
{
	return g_Lpt.GetParameterName(ulIndex, pbstrName);
}

// SVGetParameterValue
// This function Gets the parameter value specified by ulIndex.
HRESULT WINAPI SVGetParameterValue(unsigned long ulIndex, VARIANT* pvarValue)
{
	return g_Lpt.GetParameterValue(ulIndex, pvarValue);
}

// SVSetParameterValue
// This function sets the value specified by ulIndex.
HRESULT WINAPI SVSetParameterValue(unsigned long ulIndex, VARIANT* pvarValue)
{
	return g_Lpt.SetParameterValue(ulIndex, pvarValue);
}

