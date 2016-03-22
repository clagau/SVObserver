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

#include "SVTestIOCallback.h"

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
HRESULT WINAPI SVTriggerRegister( unsigned long p_ulHandle, SVTestIOCallbackPtr p_pCallback, void *p_pOwner, void *p_pData );
HRESULT WINAPI SVTriggerUnregister( unsigned long p_ulHandle, SVTestIOCallbackPtr p_pCallback, void *p_pOwner, void *p_pData );
HRESULT WINAPI SVTriggerUnregisterAll( unsigned long p_ulHandle );
HRESULT WINAPI SVTriggerStart( unsigned long p_ulHandle );
HRESULT WINAPI SVTriggerStop( unsigned long p_ulHandle );
HRESULT WINAPI SVTriggerGetParameterCount( unsigned long p_ulHandle, unsigned long *p_pulCount );
HRESULT WINAPI SVTriggerGetParameterName( unsigned long p_ulHandle, unsigned long p_ulIndex, BSTR *p_pbstrName );
HRESULT WINAPI SVTriggerGetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue );
HRESULT WINAPI SVTriggerSetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue );


