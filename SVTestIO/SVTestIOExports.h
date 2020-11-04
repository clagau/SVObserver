// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTestIOExports
// * .File Name       : $Workfile:   SVTestIOExports.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   16 Jul 2013 13:22:16  $
// ******************************************************************************

#pragma once
#include "TriggerInformation/SVTriggerInfoStruct.h"


HRESULT WINAPI SVCreate();
HRESULT WINAPI SVDestroy();

unsigned long WINAPI SVInputGetCount();
bool WINAPI SVInputGetValue(unsigned long channel);
unsigned long WINAPI SVInputGetValues();

unsigned long WINAPI SVOutputGetCount();
HRESULT WINAPI SVOutputSetValue(unsigned long channel, bool value);
unsigned long WINAPI SVOutputGetPortCount();
HRESULT WINAPI SVOutputSetPortValue(unsigned long port, unsigned long value);

unsigned long WINAPI SVTriggerGetCount();
unsigned long WINAPI SVTriggerGetHandle(unsigned long index);
_variant_t WINAPI SVTriggerGetName(unsigned long triggerIndex);
HRESULT WINAPI SVTriggerRegister(unsigned long triggerIndex, SvTi::TriggerCallBack pTriggerCallback);
HRESULT WINAPI SVTriggerUnregister(unsigned long triggerIndex);
HRESULT WINAPI SVTriggerStart(unsigned long triggerIndex);
HRESULT WINAPI SVTriggerStop(unsigned long triggerIndex);
unsigned long WINAPI SVTriggerGetParameterCount( unsigned long triggerIndex);
_variant_t WINAPI SVTriggerGetParameterName( unsigned long triggerIndex, unsigned long index);
_variant_t WINAPI SVTriggerGetParameterValue( unsigned long triggerIndex, unsigned long index);
HRESULT WINAPI SVTriggerSetParameterValue( unsigned long triggerIndex, unsigned long index, const _variant_t rValue);
