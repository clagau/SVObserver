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

std::atomic_ulong gRefCount{0UL};
SVLptIOImpl gLpt;

HRESULT WINAPI SVCreate()
{
	HRESULT result {S_OK};

	gRefCount++;
	if (1 == gRefCount)
	{
		result = gLpt.Initialize(true);
		gLpt.m_lLptTriggerEdge = 0;
		gLpt.m_lIOBrdTriggerEdge = 0;
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

HRESULT WINAPI SVInputGetCount(unsigned long* pCount)
{
	HRESULT result {E_FAIL};

	if (nullptr != pCount)
	{
		*pCount = gLpt.GetInputCount();
		result = S_OK;
	}
	return result;
}

HRESULT WINAPI SVInputGetValue( unsigned long channel, bool* pValue )
{ 
	HRESULT result {E_FAIL};

	if (nullptr != pValue)
	{
		if (channel < gLpt.GetInputCount())
		{
			bool bState;
			result = gLpt.GetInputBit(channel, bState);

			if (S_OK == result)
			{
				*pValue = bState;
			}
		}
	}
	return result;
}

// ulPort represents the physical port number (LPT 1/2/3/4)
HRESULT WINAPI SVInputGetValues( unsigned long* pValue)
{
	HRESULT result {E_FAIL};

	if (nullptr != pValue)
	{
		unsigned long lState = 0;
		result = gLpt.GetInputValue(&lState);
		if (S_OK == result)
		{
			*pValue = lState;
		}
	}
	return result;
}

HRESULT WINAPI SVOutputGetCount(unsigned long* pCount)
{
	HRESULT result {E_FAIL};

	if (nullptr != pCount)
	{
		*pCount = gLpt.GetOutputCount();
		result = S_OK;
	}
	return result;
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

HRESULT WINAPI SVOutputGetPortCount(unsigned long* pCount)
{
	HRESULT result {E_FAIL};

	if (nullptr != pCount)
	{
		*pCount = gLpt.GetPortCount();
		result = S_OK;
	}
	return result;
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

HRESULT WINAPI SVTriggerGetCount(unsigned long* pCount)
{
	HRESULT result {E_FAIL};

	if (nullptr != pCount)
	{
		*pCount = gLpt.GetTriggerCount();
		result = S_OK;
	}
	return result;
}

HRESULT WINAPI SVTriggerGetHandle(unsigned long* pTriggerIndex, unsigned long Index)
{
	HRESULT result {E_FAIL};

	if (nullptr != pTriggerIndex)
	{
		*pTriggerIndex = gLpt.GetTriggerHandle(Index);
		result = S_OK;
	}
	return result;
}

HRESULT WINAPI SVTriggerGetName(unsigned long triggerIndex, BSTR* pName)
{
	HRESULT result {E_FAIL};

	if (nullptr != pName && 0 < triggerIndex && cMaxLptTriggers >= triggerIndex)
	{
		// free any data on input...
		if (nullptr != *pName)
		{
			::SysFreeString(*pName);
		}

		*pName = gLpt.GetTriggerName(triggerIndex);
		
		if (nullptr != *pName)
		{
			result = S_OK;
		}
	} 
	return result;
}

HRESULT WINAPI SVTriggerRegister(unsigned long triggerIndex, const SvTh::TriggerDispatcher& rDispatcher)
{
	HRESULT result {E_FAIL};

	if ( rDispatcher.hasCallback() && 0 < triggerIndex && cMaxLptTriggers >= triggerIndex)
	{
		result = S_OK;

		gLpt.AddDispatcher(triggerIndex, rDispatcher);
	} 
	return result;
}

HRESULT WINAPI SVTriggerUnregister(unsigned long triggerIndex, const SvTh::TriggerDispatcher& rDispatcher)
{
	HRESULT result {E_FAIL};

	if (rDispatcher.hasCallback() && 0 < triggerIndex && cMaxLptTriggers >= triggerIndex)
	{
		result = gLpt.RemoveDispatcher(triggerIndex, rDispatcher);
	} 
	return result;
}

HRESULT WINAPI SVTriggerUnregisterAll(unsigned long triggerIndex)
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && cMaxLptTriggers >= triggerIndex)
	{
		result = gLpt.RemoveAllDispatchers(triggerIndex);
	} 
	return result;
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

HRESULT WINAPI SVTriggerGetParameterCount(unsigned long triggerIndex, unsigned long *pCount)
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && cMaxLptTriggers >= triggerIndex)
	{
		result = gLpt.TriggerGetParameterCount(triggerIndex, pCount);
	}
	return result;
}

HRESULT WINAPI SVTriggerGetParameterName(unsigned long triggerIndex, unsigned long Index, BSTR* pName)
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && cMaxLptTriggers >= triggerIndex)
	{
		result = gLpt.TriggerGetParameterName(triggerIndex, Index, pName);
	}
	return result;
}

HRESULT WINAPI SVTriggerGetParameterValue(unsigned long triggerIndex, unsigned long Index, VARIANT* pValue)
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && cMaxLptTriggers >= triggerIndex)
	{
		result = gLpt.TriggerGetParameterValue(triggerIndex, Index, pValue);
	}
	return result;
}

HRESULT WINAPI SVTriggerSetParameterValue(unsigned long triggerIndex, unsigned long Index, VARIANT* pValue)
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && cMaxLptTriggers >= triggerIndex)
	{
		result = gLpt.TriggerSetParameterValue(triggerIndex, Index, pValue);
	}
	return result;
}

HRESULT WINAPI SVGetParameterCount(unsigned long* pCount)
{
	return gLpt.GetParameterCount(pCount);
}

HRESULT WINAPI SVGetParameterName(unsigned long Index, BSTR* pName)
{
	return gLpt.GetParameterName(Index, pName);
}

HRESULT WINAPI SVGetParameterValue(unsigned long Index, VARIANT* pValue)
{
	return gLpt.GetParameterValue(Index, pValue);
}

HRESULT WINAPI SVSetParameterValue(unsigned long Index, VARIANT* pValue)
{
	return gLpt.SetParameterValue(Index, pValue);
}

