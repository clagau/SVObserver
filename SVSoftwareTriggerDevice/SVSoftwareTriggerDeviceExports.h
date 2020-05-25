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



HRESULT WINAPI SVCreate();
HRESULT WINAPI SVDestroy();

HRESULT WINAPI SVTriggerGetCount( unsigned long *pCount );
HRESULT WINAPI SVTriggerGetHandle( unsigned long *pTriggerIndex, unsigned long Index );
HRESULT WINAPI SVTriggerGetName( unsigned long triggerIndex, BSTR *pName );
HRESULT WINAPI SVTriggerRegister( unsigned long triggerIndex, const SvTh::TriggerDispatcher &rDispatcher );
HRESULT WINAPI SVTriggerUnregister( unsigned long triggerIndex, const SvTh::TriggerDispatcher &rDispatcher);
HRESULT WINAPI SVTriggerUnregisterAll( unsigned long triggerIndex );
HRESULT WINAPI SVTriggerStart( unsigned long triggerIndex );
HRESULT WINAPI SVTriggerStop( unsigned long triggerIndex );
HRESULT WINAPI SVTriggerGetParameterCount( unsigned long triggerIndex, unsigned long *p_pulCount );
HRESULT WINAPI SVTriggerGetParameterName( unsigned long triggerIndex, unsigned long p_ulIndex, BSTR *pName );
HRESULT WINAPI SVTriggerGetParameterValue( unsigned long triggerIndex, unsigned long p_ulIndex, VARIANT *pValue );
HRESULT WINAPI SVTriggerSetParameterValue( unsigned long triggerIndex, unsigned long p_ulIndex, VARIANT *pValue );

