//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLptIOExports
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <comdef.h>

namespace SvTh
{
class TriggerDispatcher;
}

#pragma region exports

HRESULT WINAPI SVCreate();
HRESULT WINAPI SVDestroy();

HRESULT WINAPI SVInputGetCount(unsigned long *pulCount);
HRESULT WINAPI SVInputGetValue(unsigned long ulChannel, bool *pbValue);
HRESULT WINAPI SVInputGetValues(unsigned long *pulValue);

HRESULT WINAPI SVOutputGetCount(unsigned long *pulCount);
HRESULT WINAPI SVOutputSetValue(unsigned long ulChannel, bool bValue);
HRESULT WINAPI SVOutputGetPortCount(unsigned long *pulCount);
HRESULT WINAPI SVOutputSetPortValue(unsigned long ulPort, unsigned long ulValue);

HRESULT WINAPI SVTriggerGetCount(unsigned long *pulCount);
HRESULT WINAPI SVTriggerGetHandle(unsigned long *pTriggerchannel, unsigned long ulIndex);
HRESULT WINAPI SVTriggerGetName(unsigned long triggerchannel, BSTR *pbstrName);
HRESULT WINAPI SVTriggerRegister(unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher );
HRESULT WINAPI SVTriggerUnregister(unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher );
HRESULT WINAPI SVTriggerUnregisterAll(unsigned long triggerchannel);
HRESULT WINAPI SVTriggerStart(unsigned long triggerchannel);
HRESULT WINAPI SVTriggerStop(unsigned long triggerchannel);
HRESULT WINAPI SVTriggerGetParameterCount(unsigned long triggerchannel, unsigned long *pulCount);
HRESULT WINAPI SVTriggerGetParameterName(unsigned long triggerchannel, unsigned long ulIndex, BSTR *pbstrName);
HRESULT WINAPI SVTriggerGetParameterValue(unsigned long triggerchannel, unsigned long ulIndex, VARIANT *pvarValue);
HRESULT WINAPI SVTriggerSetParameterValue(unsigned long triggerchannel, unsigned long ulIndex, VARIANT *pvarValue);
// Basic Parameters not associated with handles...
HRESULT WINAPI SVGetParameterCount(unsigned long *pulCount);
HRESULT WINAPI SVGetParameterName(unsigned long ulIndex, BSTR *pbstrName);
HRESULT WINAPI SVGetParameterValue(unsigned long ulIndex, VARIANT *pvarValue);
HRESULT WINAPI SVSetParameterValue(unsigned long ulIndex, VARIANT *pvarValue);
#pragma endregion exports

