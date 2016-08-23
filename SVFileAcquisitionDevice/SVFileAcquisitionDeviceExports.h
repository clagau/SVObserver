//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
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
#include "SVOMFCLibrary/SVDeviceParamCollection.h"


class SVAcquisitionBufferInterface;

// General Exports

HRESULT WINAPI SVCreate();
HRESULT WINAPI SVDestroy();

// Trigger Exports

HRESULT WINAPI SVTriggerGetCount( unsigned long *p_pulCount );
HRESULT WINAPI SVTriggerGetHandle( unsigned long *pTriggerchannel, unsigned long p_ulIndex );
HRESULT WINAPI SVTriggerGetName( unsigned long triggerchannel, BSTR *p_pbstrName );
HRESULT WINAPI SVTriggerRegister( unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher );
HRESULT WINAPI SVTriggerUnregister( unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher);
HRESULT WINAPI SVTriggerUnregisterAll( unsigned long triggerchannel );
HRESULT WINAPI SVTriggerStart( unsigned long triggerchannel );
HRESULT WINAPI SVTriggerStop( unsigned long triggerchannel );
HRESULT WINAPI SVTriggerGetParameterCount( unsigned long triggerchannel, unsigned long *p_pulCount );
HRESULT WINAPI SVTriggerGetParameterName( unsigned long triggerchannel, unsigned long p_ulIndex, BSTR *p_pbstrName );
HRESULT WINAPI SVTriggerGetParameterValue( unsigned long triggerchannel, unsigned long p_ulIndex, VARIANT *p_pvarValue );
HRESULT WINAPI SVTriggerSetParameterValue( unsigned long triggerchannel, unsigned long p_ulIndex, VARIANT *p_pvarValue );

// Digitizer Exports

HRESULT WINAPI SVDigitizerGetCount( unsigned long *p_pulCount );
HRESULT WINAPI SVDigitizerGetHandle( unsigned long *pTriggerchannel, unsigned long p_ulIndex );
HRESULT WINAPI SVDigitizerGetName( unsigned long triggerchannel, BSTR *p_pbstrName );
HRESULT WINAPI SVDigitizerLoadCameraFiles( unsigned long triggerchannel, SAFEARRAY* p_psaFileNames );
HRESULT WINAPI SVDigitizerGetBufferWidth( unsigned long triggerchannel, unsigned long *p_pulWidth );
HRESULT WINAPI SVDigitizerGetBufferHeight( unsigned long triggerchannel, unsigned long *p_pulHeight );
HRESULT WINAPI SVDigitizerGetBufferFormat( unsigned long triggerchannel, int *p_piFormat );
HRESULT WINAPI SVDigitizerCreateBuffers( unsigned long triggerchannel, unsigned long p_ulCount );
HRESULT WINAPI SVDigitizerRegisterBufferInterface( unsigned long triggerchannel, SVAcquisitionBufferInterface* p_pInterface );
HRESULT WINAPI SVDigitizerStart( unsigned long triggerchannel );
HRESULT WINAPI SVDigitizerStop( unsigned long triggerchannel );
HRESULT WINAPI SVDigitizerUnregisterBufferInterface( unsigned long triggerchannel );
HRESULT WINAPI SVDigitizerDestroyBuffers( unsigned long triggerchannel );
HRESULT WINAPI SVDigitizerUnloadCameraFile( unsigned long triggerchannel );

HRESULT WINAPI SVDigitizerParameterGetList( unsigned long triggerchannel, VARIANT *p_pvarValue );
HRESULT WINAPI SVDigitizerParameterGetName( unsigned long triggerchannel, int p_iParameterID, BSTR *p_pbstrName );
HRESULT WINAPI SVDigitizerParameterGetValue( unsigned long triggerchannel, int p_iParameterID, int *p_piParameterTypeID, VARIANT *p_pvarValue );
HRESULT WINAPI SVDigitizerParameterSetValue( unsigned long triggerchannel, int p_iParameterID, int p_iParameterTypeID, VARIANT *p_pvarValue );

HRESULT WINAPI SVDigitizerSetParameters( unsigned long triggerchannel, const SVDeviceParamCollection* p_pParameters );
HRESULT WINAPI SVDigitizerSetParameter( unsigned long triggerchannel, const SVDeviceParamWrapper* p_pParameter );
HRESULT WINAPI SVDigitizerGetParameter( unsigned long triggerchannel, SVDeviceParamEnum p_eParameter, SVDeviceParamWrapper** p_ppParameter );
HRESULT WINAPI SVDigitizerDestroyParameter( unsigned long triggerchannel, SVDeviceParamWrapper* p_pParameter );

HRESULT WINAPI SVDigitizerInternalTriggerEnable( unsigned long triggerchannel );
HRESULT WINAPI SVDigitizerInternalTrigger( unsigned long triggerchannel );
HRESULT WINAPI SVDigitizerInternalTriggerRegister( unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher);
HRESULT WINAPI SVDigitizerInternalTriggerUnregister( unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher);
HRESULT WINAPI SVDigitizerInternalTriggerUnregisterAll( unsigned long triggerchannel );


