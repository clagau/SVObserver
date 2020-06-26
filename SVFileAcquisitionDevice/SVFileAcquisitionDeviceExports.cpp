//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
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
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "CameraLibrary/SVDeviceParamCollection.h"
#include "CameraLibrary/SVLongValueDeviceParam.h"
#include "CameraLibrary/SVStringValueDeviceParam.h"

static SVFileAcquisitionDevice g_fileAcqDevice;

HRESULT WINAPI SVCreate()
{
	HRESULT l_hrOk = g_fileAcqDevice.Create();
	return l_hrOk;
}

HRESULT WINAPI SVDestroy()
{
	HRESULT l_hrOk = g_fileAcqDevice.Destroy();
	return l_hrOk;
}

// Digitizer Export Functions
HRESULT WINAPI SVDigitizerGetCount( unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulCount )
	{
		l_hrOk = g_fileAcqDevice.CameraGetCount( *p_pulCount );
	}
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetHandle( unsigned long *pTriggerchannel, unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != pTriggerchannel )
	{
		unsigned long l_ulCount = 0;

		l_hrOk = g_fileAcqDevice.CameraGetCount( l_ulCount );

		if ( S_OK == l_hrOk )
		{
			if ( p_ulIndex < l_ulCount )
			{
				l_hrOk = S_OK;

				*pTriggerchannel = p_ulIndex + 1;
			}
			else
			{
				*pTriggerchannel = 0;
			}
		}
	} 
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetName( unsigned long triggerchannel, BSTR *p_pbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pbstrName )
	{
		if ( nullptr != *p_pbstrName )
		{
			::SysFreeString( *p_pbstrName );

			*p_pbstrName = nullptr;
		}
		l_hrOk = g_fileAcqDevice.CameraGetName( triggerchannel - 1, *p_pbstrName );
	} 
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerLoadCameraFiles( unsigned long triggerchannel, SAFEARRAY* p_psaFileNames )
{
	HRESULT l_hrOk = g_fileAcqDevice.CameraLoadFiles(triggerchannel - 1, p_psaFileNames);
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetBufferWidth( unsigned long triggerchannel, unsigned long *p_pulWidth )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulWidth )
	{
		*p_pulWidth = 0;

		l_hrOk = g_fileAcqDevice.CameraGetWidth( triggerchannel - 1, *p_pulWidth );
	}
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetBufferHeight( unsigned long triggerchannel, unsigned long *p_pulHeight )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulHeight )
	{
		*p_pulHeight = 0;

		l_hrOk = g_fileAcqDevice.CameraGetHeight( triggerchannel - 1, *p_pulHeight );
	}
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetBufferFormat( unsigned long triggerchannel, int *p_piFormat )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_piFormat )
	{
		*p_piFormat = SvDef::SVImageFormatUnknown;

		l_hrOk = g_fileAcqDevice.CameraGetFormat( triggerchannel - 1, *p_piFormat );
	}
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerCreateBuffers( unsigned long triggerchannel )
{
	HRESULT l_hrOk = g_fileAcqDevice.CameraBufferCreateAll( triggerchannel - 1 );
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerRegisterBufferInterface( unsigned long triggerchannel, SVAcquisitionBufferInterface* p_pInterface )
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = g_fileAcqDevice.CameraRegisterBufferInterface( triggerchannel - 1, p_pInterface );

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerStart( unsigned long triggerchannel )
{
	HRESULT l_hrOk = g_fileAcqDevice.CameraStart( triggerchannel - 1 );
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerStop( unsigned long triggerchannel )
{
	HRESULT l_hrOk = g_fileAcqDevice.CameraStop( triggerchannel - 1 );
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerUnregisterBufferInterface( unsigned long triggerchannel )
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = g_fileAcqDevice.CameraUnregisterBufferInterface( triggerchannel - 1 );

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerInternalTriggerEnable( unsigned long triggerchannel )
{
	HRESULT l_hrOk = g_fileAcqDevice.InternalTriggerEnable( triggerchannel - 1 );
	return l_hrOk; 
}

HRESULT WINAPI SVDigitizerInternalTrigger( unsigned long triggerchannel )
{
	HRESULT l_hrOk = g_fileAcqDevice.InternalTrigger( triggerchannel - 1 );
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerDestroyBuffers( unsigned long triggerchannel )
{
	HRESULT l_hrOk = g_fileAcqDevice.CameraBufferDestroyAll( triggerchannel - 1 );
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerUnloadCameraFile( unsigned long  )
{
	return S_FALSE;
}

HRESULT WINAPI SVDigitizerParameterGetList( unsigned long triggerchannel, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = g_fileAcqDevice.CameraGetParameterList( triggerchannel - 1, p_pvarValue );
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerParameterGetName( unsigned long triggerchannel, unsigned long p_ulParameter, BSTR *p_pbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pbstrName )
	{
		if ( nullptr != *p_pbstrName )
		{
			::SysFreeString( *p_pbstrName );

			*p_pbstrName = nullptr;
		}
		l_hrOk = g_fileAcqDevice.CameraGetParameterName( triggerchannel - 1, p_ulParameter, *p_pbstrName );
	}
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerParameterGetValue( unsigned long triggerchannel, int p_iParameter, int *p_piParameterType, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = g_fileAcqDevice.CameraGetParameter( triggerchannel - 1, p_iParameter, p_piParameterType, p_pvarValue );
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerParameterSetValue( unsigned long triggerchannel, int p_iParameter, int p_iParameterType, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = g_fileAcqDevice.CameraSetParameter( triggerchannel - 1, p_iParameter, p_iParameterType, p_pvarValue );
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerSetParameters( unsigned long triggerchannel, const SVDeviceParamCollection* p_pParameters )
{
	unsigned long l_ulCount = 0;

	HRESULT l_hrOk = g_fileAcqDevice.CameraGetCount( l_ulCount );
	
	if ( S_OK == l_hrOk && triggerchannel <= l_ulCount )
	{
		SVDeviceParamMap::const_iterator iter;
		for (iter = p_pParameters->mapParameters.begin(); iter != p_pParameters->mapParameters.end(); ++iter)
		{
			const SVDeviceParamWrapper& w = iter->second;
			if ( nullptr != ((const SVDeviceParam*) w) )
			{
				SVDigitizerSetParameter( triggerchannel, &w );
			}
		}
	}
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerSetParameter( unsigned long triggerchannel, const SVDeviceParamWrapper* p_pParameter )
{
	unsigned long l_ulCount = 0;

	HRESULT l_hrOk = g_fileAcqDevice.CameraGetCount( l_ulCount );
	
	if ( S_OK == l_hrOk && triggerchannel <= l_ulCount )
	{
		if ( nullptr != p_pParameter )
		{
			const SVDeviceParamWrapper& rw = *p_pParameter;
			if ( nullptr != ((const SVDeviceParam*) rw) )
			{
				switch ( rw->Type() )
				{
					case DeviceParamFileAcqImageFileName:
					{
						const SVStringValueDeviceParam* pFilename = rw.DerivedValue( pFilename );
						_variant_t value;
						pFilename->GetValue(value);
						l_hrOk = g_fileAcqDevice.CameraSetParameter( triggerchannel - 1, SVFileAcquisitionParameterFilename, 0, &value );
						break;
					}

					case DeviceParamFileAcqImageDirName:
					{
						const SVStringValueDeviceParam* pDirname = rw.DerivedValue( pDirname );
						_variant_t value;
						pDirname->GetValue(value);
						l_hrOk = g_fileAcqDevice.CameraSetParameter( triggerchannel - 1, SVFileAcquisitionParameterDirectory, 0, &value );
						break;
					}

					case DeviceParamFileAcqLoadingMode:
					{
						const SVLongValueDeviceParam* pLoadingMode = rw.DerivedValue( pLoadingMode );
						_variant_t value;
						pLoadingMode->GetValue(value);
						l_hrOk = g_fileAcqDevice.CameraSetParameter( triggerchannel - 1, SVFileAcquisitionParameterLoadingMode, 0, &value );
						break;
					}

					case DeviceParamFileAcqImageWidth:
					{
						const SVLongValueDeviceParam* pImageWidth = rw.DerivedValue( pImageWidth );
						_variant_t value;
						pImageWidth->GetValue(value);
						l_hrOk = g_fileAcqDevice.CameraSetParameter( triggerchannel - 1, SVFileAcquisitionParameterImageWidth, 0, &value );
						break;
					}

					case DeviceParamFileAcqImageHeight:
					{
						const SVLongValueDeviceParam* pImageHeight = rw.DerivedValue( pImageHeight );
						_variant_t value;
						pImageHeight->GetValue(value);
						l_hrOk = g_fileAcqDevice.CameraSetParameter( triggerchannel - 1, SVFileAcquisitionParameterImageHeight, 0, &value );
						break;
					}

					case DeviceParamFileAcqImageFormat:
					{
						const SVLongValueDeviceParam* pImageFormat = rw.DerivedValue( pImageFormat );
						_variant_t value;
						pImageFormat->GetValue(value);
						l_hrOk = g_fileAcqDevice.CameraSetParameter( triggerchannel - 1, SVFileAcquisitionParameterImageFormat, 0, &value );
						break;
					}
					
					case DeviceParamLightReference:
					{
						l_hrOk = S_FALSE;
						break;
					}
					case DeviceParamLut:
					{
						l_hrOk = S_FALSE;
						break;
					}

					default:
					{
						l_hrOk = S_FALSE;
						break;
					}
				}
			}
		}
	}
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetParameter( unsigned long triggerchannel, SVDeviceParamEnum p_eParameter, SVDeviceParamWrapper** p_ppParameter )
{
	unsigned long l_ulCount = 0;

	HRESULT l_hrOk = g_fileAcqDevice.CameraGetCount( l_ulCount );
	
	if ( S_OK == l_hrOk && triggerchannel <= l_ulCount )
	{
		if ( nullptr != p_ppParameter )
		{
			*p_ppParameter = new SVDeviceParamWrapper;
			SVDeviceParamWrapper& rw = **p_ppParameter;
			switch ( p_eParameter )
			{
				case DeviceParamFileAcqImageFileName:
				{
					SVStringValueDeviceParam* pFilename = rw.DerivedValue( pFilename );
					_variant_t value;
					l_hrOk = g_fileAcqDevice.CameraGetParameter( triggerchannel - 1, SVFileAcquisitionParameterFilename, 0, &value );
					if (S_OK == l_hrOk)
					{
						pFilename->SetValue(value);
					}
					break;
				}

				case DeviceParamFileAcqImageDirName:
				{
					SVStringValueDeviceParam* pDirname = rw.DerivedValue( pDirname );
					_variant_t value;
					l_hrOk = g_fileAcqDevice.CameraGetParameter( triggerchannel - 1, SVFileAcquisitionParameterDirectory, 0, &value );
					if (S_OK == l_hrOk)
					{
						pDirname->SetValue(value);
					}
					break;
				}

				case DeviceParamFileAcqLoadingMode:
				{
					SVLongValueDeviceParam* pLoadingMode = rw.DerivedValue( pLoadingMode );
					_variant_t value;
					l_hrOk = g_fileAcqDevice.CameraGetParameter( triggerchannel - 1, SVFileAcquisitionParameterLoadingMode, 0, &value );
					if (S_OK == l_hrOk)
					{
						pLoadingMode->SetValue(value);
					}
					break;
				}

				case DeviceParamFileAcqImageWidth:
				{
					SVLongValueDeviceParam* pImageWidth = rw.DerivedValue( pImageWidth );
					_variant_t value;
					l_hrOk = g_fileAcqDevice.CameraGetParameter( triggerchannel - 1, SVFileAcquisitionParameterImageWidth, 0, &value );
					if (S_OK == l_hrOk)
					{
						pImageWidth->SetValue(value);
					}
					break;
				}

				case DeviceParamFileAcqImageHeight:
				{
					SVLongValueDeviceParam* pImageHeight = rw.DerivedValue( pImageHeight );
					_variant_t value;
					l_hrOk = g_fileAcqDevice.CameraGetParameter( triggerchannel - 1, SVFileAcquisitionParameterImageHeight, 0, &value );
					if (S_OK == l_hrOk)
					{
						pImageHeight->SetValue(value);
					}
					break;
				}

				case DeviceParamFileAcqImageFormat:
				{
					SVLongValueDeviceParam* pImageFormat = rw.DerivedValue( pImageFormat );
					_variant_t value;
					l_hrOk = g_fileAcqDevice.CameraGetParameter( triggerchannel - 1, SVFileAcquisitionParameterImageFormat, 0, &value );
					if (S_OK == l_hrOk)
					{
						pImageFormat->SetValue(value);
					}
					break;
				}

				case DeviceParamLightReference:
				{
					//rw = SVLightReferenceDeviceParam();
					//SVLightReferenceDeviceParam* pLR = rw.DerivedValue( pLR );
					l_hrOk = S_FALSE;
					break;
				}

				case DeviceParamLut:
				{
					//rw = SVLutDeviceParam();
					//SVLutDeviceParam* pParam = rw.DerivedValue( pParam );
					l_hrOk = S_FALSE;
					break;
				}

				default:
				{
					l_hrOk = S_FALSE;
					break;
				}
			}
		}
	}
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerDestroyParameter( unsigned long, SVDeviceParamWrapper* p_pParameter )
{
	HRESULT l_hrOk = S_OK;

	SVDeviceParamWrapper& rw = *p_pParameter;
	rw.Clear();
	return l_hrOk;
}
