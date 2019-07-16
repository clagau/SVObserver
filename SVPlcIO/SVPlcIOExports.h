//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPlcIOExports
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <comdef.h>
#include "TriggerHandling/TriggerBasics.h"

#pragma region exports

HRESULT WINAPI SVCreate();
HRESULT WINAPI SVDestroy();

HRESULT WINAPI SVInputGetCount(unsigned long *pCount);
HRESULT WINAPI SVInputGetValue(unsigned long channel, bool *pValue);
HRESULT WINAPI SVInputSetValue(unsigned long channel, bool bValue);
HRESULT WINAPI SVInputGetPortCount(unsigned long *pCount );
HRESULT WINAPI SVInputGetPortValue(unsigned long port, unsigned long *pValue);
HRESULT WINAPI SVInputSetPortValue(unsigned long port, unsigned long value);

HRESULT WINAPI SVOutputGetCount(unsigned long *pCount);
HRESULT WINAPI SVOutputGetValue(unsigned long channel, bool *pValue);
HRESULT WINAPI SVOutputSetValue(unsigned long channel, bool value);
HRESULT WINAPI SVOutputGetPortCount(unsigned long *pCount);
HRESULT WINAPI SVOutputGetPortValue(unsigned long port, unsigned long *pValue);
HRESULT WINAPI SVOutputSetPortValue(unsigned long port, unsigned long value);
HRESULT WINAPI SVOutputSetData(unsigned long channel, const SvTh::IntVariantMap& rData);

HRESULT WINAPI SVTriggerGetCount(unsigned long *pCount);
HRESULT WINAPI SVTriggerGetHandle(unsigned long *pTriggerChannel, unsigned long index);
HRESULT WINAPI SVTriggerGetName(unsigned long triggerChannel, BSTR *pName);
HRESULT WINAPI SVTriggerRegister(unsigned long triggerChannel, const SvTh::TriggerDispatcher &rDispatcher );
HRESULT WINAPI SVTriggerUnregister(unsigned long triggerChannel, const SvTh::TriggerDispatcher &rDispatcher );
HRESULT WINAPI SVTriggerUnregisterAll(unsigned long triggerChannel);
HRESULT WINAPI SVTriggerStart(unsigned long triggerChannel);
HRESULT WINAPI SVTriggerStop(unsigned long triggerChannel);
HRESULT WINAPI SVTriggerGetParameterCount(unsigned long triggerChannel, unsigned long *pCount);
HRESULT WINAPI SVTriggerGetParameterName(unsigned long triggerChannel, unsigned long index, BSTR *pName);
HRESULT WINAPI SVTriggerGetParameterValue(unsigned long triggerChannel, unsigned long index, VARIANT *pValue);
HRESULT WINAPI SVTriggerSetParameterValue(unsigned long triggerChannel, unsigned long index, VARIANT *pValue);
// Basic Parameters not associated with handles...
HRESULT WINAPI SVGetParameterCount(unsigned long *pCount);
HRESULT WINAPI SVGetParameterName(unsigned long index, BSTR *pName);
HRESULT WINAPI SVGetParameterValue(unsigned long index, VARIANT *pValue);
HRESULT WINAPI SVSetParameterValue(unsigned long index, VARIANT *pValue);
#pragma endregion exports

