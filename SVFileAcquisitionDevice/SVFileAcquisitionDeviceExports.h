//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileAcquisitionDevcieExports
//* .File Name       : $Workfile:   SVFileAcquisitionDeviceExports.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:28:50  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <comdef.h>
#include "CameraLibrary/SVDeviceParam.h"

class SVAcquisitionBufferInterface;
class SVDeviceParamCollection;

// General Exports

HRESULT WINAPI SVCreate();
HRESULT WINAPI SVDestroy();

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

_variant_t WINAPI SVDigitizerParameterGetName(unsigned long digitizerHandle, int parameterID);
_variant_t WINAPI SVDigitizerParameterGetValue(unsigned long digitizerHandle, int parameterID);
HRESULT WINAPI SVDigitizerParameterSetValue(unsigned long digitizerHandle, int parameterID, const _variant_t& rValue);
_variant_t WINAPI SVDigitizerParameterGetList(unsigned long digitizerHandle);
HRESULT WINAPI SVDigitizerSetParameters(unsigned long digitizerHandle, const SVDeviceParamCollection& rParameters);
