//******************************************************************************
//* COPYRIGHT (c) 2014 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLptIOExports
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
#include "Triggering/SVTriggerInfoStruct.h"
#pragma endregion Includes

#pragma region exports

HRESULT WINAPI SVCreate();
HRESULT WINAPI SVDestroy();

unsigned long WINAPI SVInputGetCount();
bool WINAPI SVInputGetValue(unsigned long channel);
unsigned long WINAPI SVInputGetValues();

unsigned long WINAPI SVOutputGetCount();
HRESULT WINAPI SVOutputSetValue(unsigned long channel, bool Value);
unsigned long WINAPI SVOutputGetPortCount();
HRESULT WINAPI SVOutputSetPortValue(unsigned long port, unsigned long Value);

unsigned long WINAPI SVTriggerGetCount();
unsigned long WINAPI SVTriggerGetHandle(unsigned long index);
_variant_t WINAPI SVTriggerGetName(unsigned long triggerIndex);
HRESULT WINAPI SVTriggerRegister(unsigned long triggerIndex, SvTrig::TriggerCallBack pTriggerCallback);
HRESULT WINAPI SVTriggerUnregister(unsigned long triggerIndex);
HRESULT WINAPI SVTriggerStart(unsigned long triggerIndex);
HRESULT WINAPI SVTriggerStop(unsigned long triggerIndex);
unsigned long WINAPI SVTriggerGetParameterCount(unsigned long triggerIndex);
_variant_t WINAPI SVTriggerGetParameterName(unsigned long triggerIndex, unsigned long index);
_variant_t WINAPI SVTriggerGetParameterValue(unsigned long triggerIndex, unsigned long index);
HRESULT WINAPI SVTriggerSetParameterValue(unsigned long triggerIndex, unsigned long index, const _variant_t& rValue);
// Basic Parameters not associated with handles...
unsigned long WINAPI SVGetParameterCount();
_variant_t WINAPI SVGetParameterName(unsigned long index);
_variant_t WINAPI SVGetParameterValue(unsigned long index);
HRESULT WINAPI SVSetParameterValue(unsigned long index, const _variant_t&  rValue);
#pragma endregion exports

