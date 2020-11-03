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


class SVAcquisitionBufferInterface;


HRESULT WINAPI SVCreate();
HRESULT WINAPI SVDestroy();

unsigned long WINAPI SVInputGetCount();
bool WINAPI SVInputGetValue(unsigned long channel);
unsigned long WINAPI SVInputGetValues();

unsigned long WINAPI SVOutputGetCount();
HRESULT WINAPI SVOutputSetValue(unsigned long channel, bool value);
unsigned long WINAPI SVOutputGetPortCount();
HRESULT WINAPI SVOutputSetPortValue(unsigned long port, unsigned long value);

HRESULT WINAPI SVTriggerGetCount( unsigned long *p_pulCount );
HRESULT WINAPI SVTriggerGetHandle( unsigned long *pTriggerchannel, unsigned long p_ulIndex );
HRESULT WINAPI SVTriggerGetName( unsigned long triggerchannel, BSTR *p_pbstrName );
HRESULT WINAPI SVTriggerRegister( unsigned long triggerchannel, SvTi::TriggerCallBack pTriggerCallback);
HRESULT WINAPI SVTriggerUnregister( unsigned long triggerchannel);
HRESULT WINAPI SVTriggerStart( unsigned long triggerchannel );
HRESULT WINAPI SVTriggerStop( unsigned long triggerchannel );
HRESULT WINAPI SVTriggerGetParameterCount( unsigned long triggerchannel, unsigned long *p_pulCount );
HRESULT WINAPI SVTriggerGetParameterName( unsigned long triggerchannel, unsigned long p_ulIndex, BSTR *p_pbstrName );
HRESULT WINAPI SVTriggerGetParameterValue( unsigned long triggerchannel, unsigned long p_ulIndex, VARIANT *p_pvarValue );
HRESULT WINAPI SVTriggerSetParameterValue( unsigned long triggerchannel, unsigned long p_ulIndex, VARIANT *p_pvarValue );


