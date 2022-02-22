//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSoftwareTriggerDeviceExports
//* .File Name       : $Workfile:   SVSoftwareTriggerDeviceExports.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:31:02  $
//******************************************************************************

#pragma once

#include "Triggering/SVTriggerInfoStruct.h"


HRESULT WINAPI SVCreate();
HRESULT WINAPI SVDestroy();

unsigned long WINAPI SVTriggerGetCount();
unsigned long WINAPI SVTriggerGetHandle(unsigned long Index );
_variant_t WINAPI SVTriggerGetName(unsigned long triggerIndex);
HRESULT WINAPI SVTriggerRegister(unsigned long triggerIndex, SvTrig::TriggerCallBack pTriggerCallback);
HRESULT WINAPI SVTriggerUnregister(unsigned long triggerIndex);
HRESULT WINAPI SVTriggerStart(unsigned long triggerIndex);
HRESULT WINAPI SVTriggerStop(unsigned long triggerIndex);
unsigned long WINAPI SVTriggerGetParameterCount( unsigned long triggerIndex);
_variant_t WINAPI SVTriggerGetParameterName(unsigned long triggerIndex, unsigned long index);
_variant_t WINAPI SVTriggerGetParameterValue(unsigned long triggerIndex, unsigned long index);
HRESULT WINAPI SVTriggerSetParameterValue(unsigned long triggerIndex, unsigned long p_ulIndex, const _variant_t& rValue);

