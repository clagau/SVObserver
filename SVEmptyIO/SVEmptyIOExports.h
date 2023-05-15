// ******************************************************************************
// * COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVEmptyIOExports
// * .File Name       : $Workfile:   SVEmptyIOExports.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 09:13:44  $
// ******************************************************************************

#pragma once
#pragma region Includes
#include "Triggering/SVTriggerInfoStruct.h"
#include "CameraLibrary\SVDeviceParam.h"
#pragma endregion Includes


class SVAcquisitionBufferInterface;
class SVDeviceParamCollection;

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
_variant_t WINAPI SVTriggerGetName(unsigned long triggerchannel);
HRESULT WINAPI SVTriggerRegister(unsigned long triggerchannel, SvTrig::TriggerCallBack pTriggerCallback);
HRESULT WINAPI SVTriggerUnregister(unsigned long triggerchannel);
HRESULT WINAPI SVTriggerStart(unsigned long triggerchannel );
HRESULT WINAPI SVTriggerStop(unsigned long triggerchannel );
unsigned long WINAPI SVTriggerGetParameterCount(unsigned long triggerchannel);
_variant_t WINAPI SVTriggerGetParameterName(unsigned long triggerchannel, unsigned long index);
_variant_t WINAPI SVTriggerGetParameterValue(unsigned long triggerchannel, unsigned long index);
HRESULT WINAPI SVTriggerSetParameterValue(unsigned long triggerchannel, unsigned long index, const _variant_t rValue);

// Digitizer Exports

unsigned long WINAPI SVDigitizerGetCount();
unsigned long WINAPI SVDigitizerGetHandle(unsigned long index);
_variant_t WINAPI SVDigitizerGetName(unsigned long digitizerHandle);
unsigned long WINAPI SVDigitizerGetBufferWidth(unsigned long digitizerHandle);
unsigned long WINAPI SVDigitizerGetBufferHeight(unsigned long digitizerHandle);
int WINAPI SVDigitizerGetBufferFormat(unsigned long digitizerHandle);
HRESULT WINAPI SVDigitizerCreateBuffers(unsigned long digitizerHandle);
HRESULT WINAPI SVDigitizerDestroyBuffers(unsigned long digitizerHandle);
HRESULT WINAPI SVDigitizerRegisterBufferInterface(unsigned long digitizerHandle, SVAcquisitionBufferInterface* pInterface);
HRESULT WINAPI SVDigitizerUnregisterBufferInterface(unsigned long digitizerHandle);
HRESULT WINAPI SVDigitizerStart(unsigned long digitizerHandle);
HRESULT WINAPI SVDigitizerStop(unsigned long digitizerHandle);

HRESULT WINAPI SVDigitizerInternalTrigger(unsigned long digitizerHandle, LPCTSTR pAcquisitionFile, DWORD preAcqPause);
