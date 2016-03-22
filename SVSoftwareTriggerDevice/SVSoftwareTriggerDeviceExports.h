//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSoftwareTriggerDeviceExports
//* .File Name       : $Workfile:   SVSoftwareTriggerDeviceExports.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:31:02  $
//******************************************************************************

#pragma once

#include "SVTriggerLibrary/SVTriggerCallbackPtr.h"

HRESULT WINAPI SVCreate();
HRESULT WINAPI SVDestroy();

HRESULT WINAPI SVTriggerGetCount( unsigned long *p_pulCount );
HRESULT WINAPI SVTriggerGetHandle( unsigned long *p_pulHandle, unsigned long p_ulIndex );
HRESULT WINAPI SVTriggerGetName( unsigned long p_ulHandle, BSTR *p_pbstrName );
HRESULT WINAPI SVTriggerRegister( unsigned long p_ulHandle, SVTriggerCallbackPtr p_pCallback, void *p_pOwner, void *p_pData );
HRESULT WINAPI SVTriggerUnregister( unsigned long p_ulHandle, SVTriggerCallbackPtr p_pCallback, void *p_pOwner, void *p_pData );
HRESULT WINAPI SVTriggerUnregisterAll( unsigned long p_ulHandle );
HRESULT WINAPI SVTriggerStart( unsigned long p_ulHandle );
HRESULT WINAPI SVTriggerStop( unsigned long p_ulHandle );
HRESULT WINAPI SVTriggerGetParameterCount( unsigned long p_ulHandle, unsigned long *p_pulCount );
HRESULT WINAPI SVTriggerGetParameterName( unsigned long p_ulHandle, unsigned long p_ulIndex, BSTR *p_pbstrName );
HRESULT WINAPI SVTriggerGetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue );
HRESULT WINAPI SVTriggerSetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue );

