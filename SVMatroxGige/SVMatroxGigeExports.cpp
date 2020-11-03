//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeExports
//* .File Name       : $Workfile:   SVMatroxGigeExports.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:44:12  $
//******************************************************************************
#pragma region Includes
#include "StdAfx.h"
#include "SVMatroxGigeExports.h"
#include "SVMatroxGige.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HRESULT WINAPI SVCreate()
{
	return g_matroxAcqDevice.Create();
}

HRESULT WINAPI SVDestroy()
{
	return g_matroxAcqDevice.Destroy();
}

unsigned long WINAPI SVDigitizerGetCount()
{
	return g_matroxAcqDevice.CameraGetCount();
}

unsigned long WINAPI SVDigitizerGetHandle(unsigned long index )
{
	unsigned long result{0UL};

	unsigned long count = g_matroxAcqDevice.CameraGetCount();
	if ( index < count)
	{
		result = g_matroxAcqDevice.GetDigitizerHandle(index);
	}

	return result;
}

_variant_t SVDigitizerGetName(unsigned long digitizerHandle)
{
	return g_matroxAcqDevice.CameraGetName(digitizerHandle);
}

unsigned long WINAPI SVDigitizerGetBufferWidth(unsigned long digitizerHandle)
{
	return g_matroxAcqDevice.CameraGetWidth(digitizerHandle);
}

unsigned long WINAPI SVDigitizerGetBufferHeight(unsigned long digitizerHandle)
{
	return  g_matroxAcqDevice.CameraGetHeight(digitizerHandle);
}

int WINAPI SVDigitizerGetBufferFormat(unsigned long digitizerHandle)
{
	return g_matroxAcqDevice.CameraGetFormat(digitizerHandle);
}

HRESULT WINAPI SVDigitizerCreateBuffers(unsigned long digitizerHandle)
{
	return g_matroxAcqDevice.CameraBufferCreateAll(digitizerHandle);
}

HRESULT WINAPI SVDigitizerDestroyBuffers(unsigned long digitizerHandle)
{
	return g_matroxAcqDevice.CameraBufferDestroyAll(digitizerHandle);
}


HRESULT WINAPI SVDigitizerRegisterBufferInterface(unsigned long digitizerHandle, SVAcquisitionBufferInterface* pInterface)
{
	return g_matroxAcqDevice.CameraRegisterBufferInterface(digitizerHandle, pInterface);
}

HRESULT WINAPI SVDigitizerUnregisterBufferInterface(unsigned long digitizerHandle)
{
	return g_matroxAcqDevice.CameraUnregisterBufferInterface(digitizerHandle);
}

HRESULT WINAPI SVDigitizerStart(unsigned long digitizerHandle)
{
	return g_matroxAcqDevice.CameraStart(digitizerHandle);
}

HRESULT WINAPI SVDigitizerStop(unsigned long digitizerHandle)
{
	return g_matroxAcqDevice.CameraStop(digitizerHandle);
}

HRESULT WINAPI SVDigitizerInternalTriggerEnable(unsigned long digitizerHandle )
{
	return g_matroxAcqDevice.InternalTriggerEnable(digitizerHandle);
}

HRESULT WINAPI SVDigitizerInternalTrigger(unsigned long digitizerHandle)
{
	return g_matroxAcqDevice.InternalTrigger(digitizerHandle);
}

_variant_t WINAPI SVDigitizerParameterGetName(unsigned long digitizerHandle, int parameterID)
{
	return g_matroxAcqDevice.CameraGetParameterName(digitizerHandle, parameterID);
}

_variant_t WINAPI SVDigitizerParameterGetValue(unsigned long digitizerHandle, int parameterID)
{
	return g_matroxAcqDevice.CameraGetParameter(digitizerHandle, parameterID);
}

HRESULT WINAPI SVDigitizerParameterSetValue(unsigned long digitizerHandle, int parameter, const _variant_t& rValue)
{
	return g_matroxAcqDevice.CameraSetParameter(digitizerHandle, parameter, rValue);
}

_variant_t WINAPI SVDigitizerParameterGetList(unsigned long digitizerHandle)
{
	return g_matroxAcqDevice.CameraGetParameterList(digitizerHandle);
}

void WINAPI SVDigitizerScanForCameras()
{
	g_matroxAcqDevice.ScanForCameras();
}

