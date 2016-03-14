//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLptIOExports
//* .File Name       : $Workfile:   SVLptIOExports.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Sep 2014 15:57:36  $
//******************************************************************************

#pragma once

#include <comdef.h>
#include "SVLptIOCallback.h"

#pragma region exports
typedef HRESULT (CALLBACK *SVLptCallbackPtr)(void * /* Owner */, void * /* Data */); 

HRESULT WINAPI SVCreate();
HRESULT WINAPI SVDestroy();

HRESULT WINAPI SVInputGetCount(unsigned long *pulCount);
HRESULT WINAPI SVInputGetValue(unsigned long ulChannel, bool *pbValue);
HRESULT WINAPI SVInputSetValue(unsigned long ulChannel, bool bValue);
HRESULT WINAPI SVInputGetPortCount(unsigned long *pulCount );
HRESULT WINAPI SVInputGetPortValue(unsigned long ulPort, unsigned long *pulValue);
HRESULT WINAPI SVInputSetPortValue(unsigned long ulPort, unsigned long ulValue);

HRESULT WINAPI SVOutputGetCount(unsigned long *pulCount);
HRESULT WINAPI SVOutputGetValue(unsigned long ulChannel, bool *pbValue);
HRESULT WINAPI SVOutputSetValue(unsigned long ulChannel, bool bValue);
HRESULT WINAPI SVOutputGetPortCount(unsigned long *pulCount);
HRESULT WINAPI SVOutputGetPortValue(unsigned long ulPort, unsigned long *pulValue);
HRESULT WINAPI SVOutputSetPortValue(unsigned long ulPort, unsigned long ulValue);

HRESULT WINAPI SVTriggerGetCount(unsigned long *pulCount);
HRESULT WINAPI SVTriggerGetHandle(unsigned long *pulHandle, unsigned long ulIndex);
HRESULT WINAPI SVTriggerGetName(unsigned long ulHandle, BSTR *pbstrName);
HRESULT WINAPI SVTriggerRegister(unsigned long ulHandle, SVLptCallbackPtr pCallback, void *pOwner, void *pData);
HRESULT WINAPI SVTriggerUnregister(unsigned long ulHandle, SVLptCallbackPtr pCallback, void *pOwner, void *pData);
HRESULT WINAPI SVTriggerUnregisterAll(unsigned long ulHandle);
HRESULT WINAPI SVTriggerStart(unsigned long ulHandle);
HRESULT WINAPI SVTriggerStop(unsigned long ulHandle);
HRESULT WINAPI SVTriggerGetParameterCount(unsigned long ulHandle, unsigned long *pulCount);
HRESULT WINAPI SVTriggerGetParameterName(unsigned long ulHandle, unsigned long ulIndex, BSTR *pbstrName);
HRESULT WINAPI SVTriggerGetParameterValue(unsigned long ulHandle, unsigned long ulIndex, VARIANT *pvarValue);
HRESULT WINAPI SVTriggerSetParameterValue(unsigned long ulHandle, unsigned long ulIndex, VARIANT *pvarValue);
// Basic Parameters not associated with handles...
HRESULT WINAPI SVGetParameterCount(unsigned long *pulCount);
HRESULT WINAPI SVGetParameterName(unsigned long ulIndex, BSTR *pbstrName);
HRESULT WINAPI SVGetParameterValue(unsigned long ulIndex, VARIANT *pvarValue);
HRESULT WINAPI SVSetParameterValue(unsigned long ulIndex, VARIANT *pvarValue);
#pragma endregion exports

