//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileAcquisitionDeviceExports
//* .File Name       : $Workfile:   SVFileAcquisitionDeviceExports.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:28:48  $
//******************************************************************************

#include "StdAfx.h"
#include "SVFileAcquisitionDevice.h"
#include "SVFileAcquisitionDeviceExports.h"
#include "SVFileAcquisitionDeviceParamEnum.h"
#include "CameraLibrary/SVDeviceParamCollection.h"
#include "CameraLibrary/SVLongValueDeviceParam.h"
#include "CameraLibrary/SVStringValueDeviceParam.h"

static SVFileAcquisitionDevice g_fileAcqDevice;

HRESULT WINAPI SVCreate()
{
	return g_fileAcqDevice.Create();
}

HRESULT WINAPI SVDestroy()
{
	return g_fileAcqDevice.Destroy();
}

unsigned long WINAPI SVDigitizerGetCount()
{
	return g_fileAcqDevice.CameraGetCount();
}

unsigned long WINAPI SVDigitizerGetHandle(unsigned long index)
{
	unsigned long result{ 0UL };

	unsigned long count = g_fileAcqDevice.CameraGetCount();
	if (index < count)
	{
		result = index + 1;
	}
	return result;
}

_variant_t SVDigitizerGetName(unsigned long digitizerHandle)
{
	return g_fileAcqDevice.CameraGetName(digitizerHandle - 1);
}

unsigned long WINAPI SVDigitizerGetBufferWidth(unsigned long digitizerHandle)
{
	return g_fileAcqDevice.CameraGetWidth(digitizerHandle - 1);
}

unsigned long WINAPI SVDigitizerGetBufferHeight(unsigned long digitizerHandle)
{
	return  g_fileAcqDevice.CameraGetHeight(digitizerHandle - 1);
}

int WINAPI SVDigitizerGetBufferFormat(unsigned long digitizerHandle)
{
	return g_fileAcqDevice.CameraGetFormat(digitizerHandle - 1);
}

HRESULT WINAPI SVDigitizerCreateBuffers(unsigned long digitizerHandle)
{
	return g_fileAcqDevice.CameraBufferCreateAll(digitizerHandle - 1);
}

HRESULT WINAPI SVDigitizerDestroyBuffers(unsigned long digitizerHandle)
{
	return g_fileAcqDevice.CameraBufferDestroyAll(digitizerHandle - 1);
}


HRESULT WINAPI SVDigitizerRegisterBufferInterface(unsigned long digitizerHandle, SVAcquisitionBufferInterface* pInterface)
{
	return g_fileAcqDevice.CameraRegisterBufferInterface(digitizerHandle - 1, pInterface);
}

HRESULT WINAPI SVDigitizerUnregisterBufferInterface(unsigned long digitizerHandle)
{
	return g_fileAcqDevice.CameraUnregisterBufferInterface(digitizerHandle - 1);
}

HRESULT WINAPI SVDigitizerStart(unsigned long digitizerHandle)
{
	return g_fileAcqDevice.CameraStart(digitizerHandle - 1);
}

HRESULT WINAPI SVDigitizerStop(unsigned long digitizerHandle)
{
	return g_fileAcqDevice.CameraStop(digitizerHandle - 1);
}

HRESULT WINAPI SVDigitizerInternalTrigger(unsigned long digitizerHandle, LPCTSTR pAcquisitionFile)
{
	return g_fileAcqDevice.InternalTrigger(digitizerHandle - 1, pAcquisitionFile);
}

_variant_t WINAPI SVDigitizerParameterGetName(unsigned long digitizerHandle, int parameterID)
{
	return g_fileAcqDevice.CameraGetParameterName(digitizerHandle - 1, parameterID);
}

_variant_t WINAPI SVDigitizerParameterGetValue(unsigned long digitizerHandle, int parameterID)
{
	return g_fileAcqDevice.CameraGetParameter(digitizerHandle - 1, parameterID);
}

HRESULT WINAPI SVDigitizerParameterSetValue(unsigned long digitizerHandle, int parameter, const _variant_t& rValue)
{
	return g_fileAcqDevice.CameraSetParameter(digitizerHandle - 1, parameter, rValue);
}

_variant_t WINAPI SVDigitizerParameterGetList(unsigned long digitizerHandle)
{
	return g_fileAcqDevice.CameraGetParameterList(digitizerHandle - 1);
}

HRESULT WINAPI SVDigitizerSetParameters(unsigned long digitizerHandle, const SVDeviceParamCollection& rParameters)
{
	HRESULT result{ E_FAIL };

	if (digitizerHandle <= g_fileAcqDevice.CameraGetCount())
	{
		for (const auto& rParameter : rParameters.mapParameters)
		{
			const SVDeviceParamWrapper& w = rParameter.second;
			if (nullptr != ((const SVDeviceParam*)w))
			{
				switch (w->Type())
				{
					case DeviceParamFileAcqImageFileName:
					{
						const SVStringValueDeviceParam* pFilename = w.DerivedValue(pFilename);
						_variant_t value;
						pFilename->GetValue(value);
						result = g_fileAcqDevice.CameraSetParameter(digitizerHandle - 1, SVFileAcquisitionParameterFilename, value);
						break;
					}

					case DeviceParamFileAcqImageDirName:
					{
						const SVStringValueDeviceParam* pDirname = w.DerivedValue(pDirname);
						_variant_t value;
						pDirname->GetValue(value);
						result = g_fileAcqDevice.CameraSetParameter(digitizerHandle - 1, SVFileAcquisitionParameterDirectory, value);
						break;
					}

					case DeviceParamFileAcqLoadingMode:
					{
						const SVLongValueDeviceParam* pLoadingMode = w.DerivedValue(pLoadingMode);
						_variant_t value;
						pLoadingMode->GetValue(value);
						result = g_fileAcqDevice.CameraSetParameter(digitizerHandle - 1, SVFileAcquisitionParameterLoadingMode, value);
						break;
					}

					case DeviceParamFileAcqImageWidth:
					{
						const SVLongValueDeviceParam* pImageWidth = w.DerivedValue(pImageWidth);
						_variant_t value;
						pImageWidth->GetValue(value);
						result = g_fileAcqDevice.CameraSetParameter(digitizerHandle - 1, SVFileAcquisitionParameterImageWidth, value);
						break;
					}

					case DeviceParamFileAcqImageHeight:
					{
						const SVLongValueDeviceParam* pImageHeight = w.DerivedValue(pImageHeight);
						_variant_t value;
						pImageHeight->GetValue(value);
						result = g_fileAcqDevice.CameraSetParameter(digitizerHandle - 1, SVFileAcquisitionParameterImageHeight, value);
						break;
					}

					case DeviceParamFileAcqImageFormat:
					{
						const SVLongValueDeviceParam* pImageFormat = w.DerivedValue(pImageFormat);
						_variant_t value;
						pImageFormat->GetValue(value);
						result = g_fileAcqDevice.CameraSetParameter(digitizerHandle - 1, SVFileAcquisitionParameterImageFormat, value);
						break;
					}

					default:
					{
						result = E_FAIL;
						break;
					}
				}
				if (S_OK != result)
				{
					break;
				}
			}
		}
	}
	return result;
}


