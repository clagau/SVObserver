// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVEmptyIOExports
// * .File Name       : $Workfile:   SVEmptyIOExports.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 09:13:44  $
// ******************************************************************************

#pragma once

#include "SVOMFCLibrary/SVDeviceParamCollection.h"
#include "TriggerHandling/TriggerBasics.h"

class SVAcquisitionBufferInterface;

HRESULT WINAPI SVCreate();
HRESULT WINAPI SVDestroy();

HRESULT WINAPI SVInputGetCount( unsigned long *p_pulCount );
HRESULT WINAPI SVInputGetValue( unsigned long p_ulChannel, bool *p_pbValue );
HRESULT WINAPI SVInputSetValue( unsigned long p_ulChannel, bool p_bValue );
HRESULT WINAPI SVInputGetPortCount( unsigned long *p_pulCount );
HRESULT WINAPI SVInputGetPortValue( unsigned long p_ulPort, unsigned long *p_pulValue );
HRESULT WINAPI SVInputSetPortValue( unsigned long p_ulPort, unsigned long p_ulValue );

HRESULT WINAPI SVOutputGetCount( unsigned long *p_pulCount );
HRESULT WINAPI SVOutputGetValue( unsigned long p_ulChannel, bool *p_pbValue );
HRESULT WINAPI SVOutputSetValue( unsigned long p_ulChannel, bool p_bValue );
HRESULT WINAPI SVOutputGetPortCount( unsigned long *p_pulCount );
HRESULT WINAPI SVOutputGetPortValue( unsigned long p_ulPort, unsigned long *p_pulValue );
HRESULT WINAPI SVOutputSetPortValue( unsigned long p_ulPort, unsigned long p_ulValue );

HRESULT WINAPI SVTriggerGetCount( unsigned long *p_pulCount );
HRESULT WINAPI SVTriggerGetHandle( unsigned long *p_pulHandle, unsigned long p_ulIndex );
HRESULT WINAPI SVTriggerGetName( unsigned long p_ulHandle, BSTR *p_pbstrName );
HRESULT WINAPI SVTriggerRegister( unsigned long p_ulHandle, SvTh::TriggerCallbackInformation triggerCallbackInfo);
HRESULT WINAPI SVTriggerUnregister( unsigned long p_ulHandle, SvTh::TriggerCallbackInformation triggerCallbackInfo);
HRESULT WINAPI SVTriggerUnregisterAll( unsigned long p_ulHandle );
HRESULT WINAPI SVTriggerStart( unsigned long p_ulHandle );
HRESULT WINAPI SVTriggerStop( unsigned long p_ulHandle );
HRESULT WINAPI SVTriggerGetParameterCount( unsigned long p_ulHandle, unsigned long *p_pulCount );
HRESULT WINAPI SVTriggerGetParameterName( unsigned long p_ulHandle, unsigned long p_ulIndex, BSTR *p_pbstrName );
HRESULT WINAPI SVTriggerGetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue );
HRESULT WINAPI SVTriggerSetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue );

// Digitizer Exports

HRESULT WINAPI SVDigitizerGetCount( unsigned long *p_pulCount );
HRESULT WINAPI SVDigitizerGetHandle( unsigned long *p_pulHandle, unsigned long p_ulIndex );
HRESULT WINAPI SVDigitizerGetName( unsigned long p_ulHandle, BSTR *p_pbstrName );
HRESULT WINAPI SVDigitizerLoadCameraFiles( unsigned long p_ulHandle, SAFEARRAY* p_psaFileNames );
HRESULT WINAPI SVDigitizerGetBufferWidth( unsigned long p_ulHandle, unsigned long *p_pulWidth );
HRESULT WINAPI SVDigitizerGetBufferHeight( unsigned long p_ulHandle, unsigned long *p_pulHeight );
HRESULT WINAPI SVDigitizerGetBufferFormat( unsigned long p_ulHandle, int *p_piFormat );
HRESULT WINAPI SVDigitizerCreateBuffers( unsigned long p_ulHandle, unsigned long p_ulCount );
HRESULT WINAPI SVDigitizerRegisterBufferInterface( unsigned long p_ulHandle, SVAcquisitionBufferInterface* p_pInterface );
HRESULT WINAPI SVDigitizerStart( unsigned long p_ulHandle );
HRESULT WINAPI SVDigitizerStop( unsigned long p_ulHandle );
HRESULT WINAPI SVDigitizerUnregisterBufferInterface( unsigned long p_ulHandle );

HRESULT WINAPI SVDigitizerInternalTriggerEnable( unsigned long p_ulHandle );
HRESULT WINAPI SVDigitizerInternalTrigger( unsigned long p_ulHandle );
HRESULT WINAPI SVDigitizerInternalTriggerRegister( unsigned long p_ulHandle, const SvTh::TriggerCallbackInformation &rTriggerCallbackInfo );
HRESULT WINAPI SVDigitizerInternalTriggerUnregister( unsigned long p_ulHandle, const SvTh::TriggerCallbackInformation &rTriggerCallbackInfo );
HRESULT WINAPI SVDigitizerInternalTriggerUnregisterAll( unsigned long p_ulHandle );

HRESULT WINAPI SVDigitizerDestroyBuffers( unsigned long p_ulHandle );
HRESULT WINAPI SVDigitizerUnloadCameraFile( unsigned long p_ulHandle );

HRESULT WINAPI SVDigitizerGetRawParameter( unsigned long p_ulHandle, unsigned long p_ulParameter, VARIANT *p_pvarValue );
HRESULT WINAPI SVDigitizerSetRawParameter( unsigned long p_ulHandle, unsigned long p_ulParameter, VARIANT *p_pvarValue );

HRESULT WINAPI SVDigitizerSetParameters( unsigned long p_ulHandle, const SVDeviceParamCollection* p_pParameters );
HRESULT WINAPI SVDigitizerSetParameter( unsigned long p_ulHandle, const SVDeviceParamWrapper* p_pParameter );
HRESULT WINAPI SVDigitizerGetParameter( unsigned long p_ulHandle, SVDeviceParamEnum p_eParameter, SVDeviceParamWrapper** p_ppParameter );
HRESULT WINAPI SVDigitizerDestroyParameter( unsigned long p_ulHandle, SVDeviceParamWrapper* p_pParameter );

