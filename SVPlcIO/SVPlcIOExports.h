//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPlcIOExports
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <comdef.h>
#include "TriggerHandling/TriggerDispatcher.h"

#pragma region exports

HRESULT WINAPI SVCreate();
HRESULT WINAPI SVDestroy();

HRESULT WINAPI SVInputGetCount(unsigned long *pCount);
HRESULT WINAPI SVInputGetValue(unsigned long channel, bool *pValue);
HRESULT WINAPI SVInputGetValues(unsigned long *pValue);

HRESULT WINAPI SVOutputGetCount(unsigned long *pCount);
HRESULT WINAPI SVOutputSetValue(unsigned long channel, bool value);
HRESULT WINAPI SVOutputGetPortCount(unsigned long *pCount);
HRESULT WINAPI SVOutputSetPortValue(unsigned long port, unsigned long value);
HRESULT WINAPI SVOutputSetData(unsigned long triggerIndex, const SvTh::IntVariantMap& rData);

HRESULT WINAPI SVTriggerGetCount(unsigned long *pCount);
HRESULT WINAPI SVTriggerGetHandle(unsigned long *pTriggerIndex, unsigned long index);
HRESULT WINAPI SVTriggerGetName(unsigned long triggerIndex, BSTR *pName);
HRESULT WINAPI SVTriggerRegister(unsigned long triggerIndex, const SvTh::TriggerDispatcher &rDispatcher );
HRESULT WINAPI SVTriggerUnregister(unsigned long triggerIndex, const SvTh::TriggerDispatcher &rDispatcher );
HRESULT WINAPI SVTriggerUnregisterAll(unsigned long triggerIndex);
HRESULT WINAPI SVTriggerStart(unsigned long triggerIndex);
HRESULT WINAPI SVTriggerStop(unsigned long triggerIndex);
HRESULT WINAPI SVTriggerGetParameterCount(unsigned long triggerIndex, unsigned long *pCount);
HRESULT WINAPI SVTriggerGetParameterName(unsigned long triggerIndex, unsigned long index, BSTR *pName);
HRESULT WINAPI SVTriggerGetParameterValue(unsigned long triggerIndex, unsigned long index, VARIANT *pValue);
HRESULT WINAPI SVTriggerSetParameterValue(unsigned long triggerIndex, unsigned long index, VARIANT *pValue);
// Basic Parameters not associated with handles...
HRESULT WINAPI SVGetParameterCount(unsigned long *pCount);
HRESULT WINAPI SVGetParameterName(unsigned long index, BSTR *pName);
HRESULT WINAPI SVGetParameterValue(unsigned long index, VARIANT *pValue);
HRESULT WINAPI SVSetParameterValue(unsigned long index, VARIANT *pValue);
#pragma endregion exports

