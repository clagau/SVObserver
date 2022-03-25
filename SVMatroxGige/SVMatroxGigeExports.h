//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeExports
//* .File Name       : $Workfile:   SVMatroxGigeExports.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:44:18  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "CameraLibrary/SVDeviceParam.h"
#pragma endregion Includes

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
HRESULT WINAPI SVDigitizerStart(unsigned long digitizerHandle );
HRESULT WINAPI SVDigitizerStop(unsigned long digitizerHandle );

HRESULT WINAPI SVDigitizerInternalTrigger(unsigned long digitizerHandle, LPCTSTR pAcquisitionFile);

_variant_t WINAPI SVDigitizerParameterGetName(unsigned long digitizerHandle, int parameterID);
_variant_t WINAPI SVDigitizerParameterGetValue(unsigned long digitizerHandle, int parameterID);
HRESULT WINAPI SVDigitizerParameterSetValue(unsigned long digitizerHandle, int parameterID, const _variant_t& rValue);
_variant_t WINAPI SVDigitizerParameterGetList(unsigned long digitizerHandle);
/// Note SVDigitizerSetParameters is not used by hardware cameras

void WINAPI SVDigitizerScanForCameras();

