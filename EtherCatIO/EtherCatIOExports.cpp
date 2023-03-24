//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file SVPlcIOExports.cpp
/// \brief Exported functions of the DLL
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "EtherCatIOExports.h"
#include "EtherCatIOImpl.h"
#pragma endregion Includes

std::atomic_ulong gRefCount{0UL};
SvEcat::EthercatIOImpl gEtherCat;

HRESULT WINAPI SVCreate()
{
	HRESULT result {S_OK};

	gRefCount++;
	if (1 == gRefCount)
	{
		result = gEtherCat.Initialize(true);
	}
	return result;
}

HRESULT WINAPI SVDestroy()
{
	HRESULT result {S_OK};

	gRefCount--;
	if (0 == gRefCount)
	{
		result = gEtherCat.Initialize(false);
	}
	return result;
}

unsigned long WINAPI SVInputGetCount()
{
	return gEtherCat.GetInputCount();
}

bool WINAPI SVInputGetValue(unsigned long index)
{ 
	return gEtherCat.GetInputBit(index);
}

unsigned long WINAPI SVInputGetValues()
{
	return gEtherCat.GetInputValue();
}

unsigned long WINAPI SVOutputGetCount()
{
	return gEtherCat.GetOutputCount();
}

HRESULT WINAPI SVOutputSetValue(unsigned long index, bool state)
{
	return gEtherCat.SetOutputBit(index, state);
}

unsigned long WINAPI SVOutputGetPortCount()
{
	return 0UL;
}

HRESULT WINAPI SVOutputSetPortValue(unsigned long , unsigned long )
{
	return S_OK;
}

HRESULT WINAPI SVOutputSetData(const SvTrig::ResultData&)
{
	return S_OK;
}

unsigned long WINAPI SVTriggerGetCount()
{
	return gEtherCat.GetTriggerCount();
}

unsigned long WINAPI SVTriggerGetHandle(unsigned long index)
{
	return gEtherCat.GetTriggerHandle(index);
}

_variant_t WINAPI SVTriggerGetName(unsigned long triggerIndex)
{
	_variant_t result;

	if (0 < triggerIndex && SvEcat::cMaxEtherCatTriggers >= triggerIndex)
	{
		result = gEtherCat.GetTriggerName(triggerIndex);
	} 
	return result;
}

HRESULT WINAPI SVTriggerRegister(unsigned long triggerIndex, SvTrig::TriggerCallBack pTriggerCallback)
{
	if (0 < triggerIndex && SvEcat::cMaxEtherCatTriggers >= triggerIndex)
	{
		return gEtherCat.RegisterCallback(triggerIndex, pTriggerCallback);
	} 
	return E_FAIL;
}

HRESULT WINAPI SVTriggerUnregister(unsigned long triggerIndex)
{
	if (0 < triggerIndex && SvEcat::cMaxEtherCatTriggers >= triggerIndex)
	{
		return gEtherCat.UnRegisterCallback(triggerIndex);
	} 
	return E_FAIL;
}

HRESULT WINAPI SVTriggerStart(unsigned long triggerIndex)
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && SvEcat::cMaxEtherCatTriggers >= triggerIndex)
	{
		result = gEtherCat.StartTrigger(triggerIndex);
	}
	return result;
}

HRESULT WINAPI SVTriggerStop(unsigned long triggerIndex)
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && SvEcat::cMaxEtherCatTriggers >= triggerIndex)
	{
		result = gEtherCat.StopTrigger(triggerIndex);
	} 
	return result;
}

unsigned long WINAPI SVTriggerGetParameterCount(unsigned long triggerIndex)
{
	unsigned long result{ 0UL };

	if (0 < triggerIndex && SvEcat::cMaxEtherCatTriggers >= triggerIndex)
	{
		result = gEtherCat.TriggerGetParameterCount(triggerIndex);
	}
	return result;
}

_variant_t WINAPI SVTriggerGetParameterName(unsigned long triggerIndex, unsigned long index)
{
	_variant_t result;

	if (0 < triggerIndex && SvEcat::cMaxEtherCatTriggers >= triggerIndex)
	{
		result = gEtherCat.TriggerGetParameterName(triggerIndex, index);
	}
	return result;
}

_variant_t WINAPI SVTriggerGetParameterValue(unsigned long triggerIndex, unsigned long index)
{
	_variant_t result;

	if (0 < triggerIndex && SvEcat::cMaxEtherCatTriggers >= triggerIndex)
	{
		result = gEtherCat.TriggerGetParameterValue(triggerIndex, index);
	}
	return result;
}

HRESULT WINAPI SVTriggerSetParameterValue(unsigned long triggerIndex, unsigned long ulIndex,  _variant_t rValue)
{
	HRESULT result {E_FAIL};

	if (0 < triggerIndex && SvEcat::cMaxEtherCatTriggers >= triggerIndex)
	{
		result = gEtherCat.TriggerSetParameterValue(triggerIndex, ulIndex, rValue);
	}
	return result;
}

unsigned long WINAPI SVGetParameterCount()
{
	return gEtherCat.GetParameterCount();
}

_variant_t WINAPI SVGetParameterName(unsigned long index)
{
	return gEtherCat.GetParameterName(index);
}

_variant_t WINAPI SVGetParameterValue(unsigned long index)
{
	return gEtherCat.GetParameterValue(index);
}

HRESULT WINAPI SVSetParameterValue(unsigned long index, const _variant_t& rValue)
{
	return gEtherCat.SetParameterValue(index, rValue);
}

