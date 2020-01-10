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

#include "SVImageLibrary/SVImagingDeviceParams.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HRESULT WINAPI SVCreate()
{
	HRESULT l_hr = g_matroxAcqDevice.Create();
	return l_hr;
}

HRESULT WINAPI SVDestroy()
{
	HRESULT l_hr = g_matroxAcqDevice.Destroy();
	return l_hr;
}

HRESULT WINAPI SVDigitizerGetCount( unsigned long *p_pulCount )
{
	HRESULT l_hr = S_FALSE;

	if ( nullptr != p_pulCount )
	{
		l_hr = g_matroxAcqDevice.CameraGetCount( *p_pulCount );
	}

	return l_hr;
}

HRESULT WINAPI SVDigitizerGetHandle( unsigned long *pTriggerchannel, unsigned long p_ulIndex )
{
	HRESULT l_hr = S_FALSE;

	if ( nullptr != pTriggerchannel )
	{
		unsigned long l_ulCount = 0;

		l_hr = g_matroxAcqDevice.CameraGetCount( l_ulCount );

		if ( S_OK == l_hr )
		{
			if ( p_ulIndex < l_ulCount )
			{
				l_hr = S_OK;

				*pTriggerchannel = g_matroxAcqDevice.GetDigitizerHandle(p_ulIndex);
			}
			else
			{
				*pTriggerchannel = 0;
			}
		}
	} 

	return l_hr;
}

HRESULT WINAPI SVDigitizerGetName( unsigned long triggerchannel, BSTR *p_pbstrName )
{
	HRESULT l_hr = S_FALSE;

	if ( nullptr != p_pbstrName )
	{
		if ( nullptr != *p_pbstrName )
		{
			::SysFreeString( *p_pbstrName );

			*p_pbstrName = nullptr;
		}

		l_hr = g_matroxAcqDevice.CameraGetName( triggerchannel, *p_pbstrName );
	} 

	return l_hr;
}

HRESULT WINAPI SVDigitizerLoadCameraFiles( unsigned long triggerchannel, SAFEARRAY* p_psaFileNames )
{
	HRESULT l_hr = g_matroxAcqDevice.CameraLoadFiles(triggerchannel, p_psaFileNames);

	return l_hr;
}

HRESULT WINAPI SVDigitizerGetBufferWidth( unsigned long triggerchannel, unsigned long *p_pulWidth )
{
	HRESULT l_hr = S_FALSE;

	if ( nullptr != p_pulWidth )
	{
		*p_pulWidth = 0;

		l_hr = g_matroxAcqDevice.CameraGetWidth( triggerchannel, *p_pulWidth );
	}

	return l_hr;
}

HRESULT WINAPI SVDigitizerGetBufferHeight( unsigned long triggerchannel, unsigned long *p_pulHeight )
{
	HRESULT l_hr = S_FALSE;

	if ( nullptr != p_pulHeight )
	{
		*p_pulHeight = 0;

		l_hr = g_matroxAcqDevice.CameraGetHeight( triggerchannel, *p_pulHeight );
	}

	return l_hr;
}

HRESULT WINAPI SVDigitizerGetBufferFormat( unsigned long triggerchannel, int *p_piFormat )
{
	HRESULT l_hr = S_FALSE;

	if ( nullptr != p_piFormat )
	{
		*p_piFormat = SvDef::SVImageFormatUnknown;

		l_hr = g_matroxAcqDevice.CameraGetFormat( triggerchannel, *p_piFormat );
	}

	return l_hr;
}

HRESULT WINAPI SVDigitizerCreateBuffers( unsigned long triggerchannel )
{
	HRESULT l_hr = g_matroxAcqDevice.CameraBufferCreateAll( triggerchannel );

	return l_hr;
}

HRESULT WINAPI SVDigitizerRegisterBufferInterface( unsigned long triggerchannel, SVAcquisitionBufferInterface* p_pInterface )
{
	HRESULT l_hr = g_matroxAcqDevice.CameraRegisterBufferInterface( triggerchannel, p_pInterface );

	return l_hr;
}

HRESULT WINAPI SVDigitizerStart( unsigned long triggerchannel )
{
	HRESULT l_hr = g_matroxAcqDevice.CameraStart( triggerchannel );

	return l_hr;
}

HRESULT WINAPI SVDigitizerStop( unsigned long triggerchannel )
{
	HRESULT l_hr = g_matroxAcqDevice.CameraStop( triggerchannel );

	return l_hr;
}

HRESULT WINAPI SVDigitizerUnregisterBufferInterface( unsigned long triggerchannel )
{
	HRESULT l_hr = g_matroxAcqDevice.CameraUnregisterBufferInterface( triggerchannel );

	return l_hr;
}

HRESULT WINAPI SVDigitizerInternalTriggerEnable( unsigned long triggerchannel )
{
	HRESULT l_hr = g_matroxAcqDevice.InternalTriggerEnable( triggerchannel );
	
	return l_hr;
}

HRESULT WINAPI SVDigitizerInternalTrigger( unsigned long triggerchannel )
{
	HRESULT l_hr = g_matroxAcqDevice.InternalTrigger( triggerchannel );
	
	return l_hr;
}

HRESULT WINAPI SVDigitizerDestroyBuffers( unsigned long triggerchannel )
{
	HRESULT l_hr = g_matroxAcqDevice.CameraBufferDestroyAll( triggerchannel );

	return l_hr;
}

HRESULT WINAPI SVDigitizerUnloadCameraFile( unsigned long triggerchannel )
{
	HRESULT l_hr = S_FALSE;

	return l_hr;
}

HRESULT WINAPI SVDigitizerParameterGetList( unsigned long triggerchannel, VARIANT *p_pvarValue )
{
	HRESULT l_hr = g_matroxAcqDevice.CameraGetParameterList( triggerchannel, p_pvarValue );

	return l_hr;
}

HRESULT WINAPI SVDigitizerParameterGetName( unsigned long triggerchannel, unsigned long p_ulParameter, BSTR *p_pbstrName )
{
	HRESULT l_hr = g_matroxAcqDevice.CameraGetParameterName( triggerchannel, p_ulParameter, p_pbstrName );

	return l_hr;
}

HRESULT WINAPI SVDigitizerParameterGetValue( unsigned long triggerchannel, int p_iParameter, int *p_piParameterType, VARIANT *p_pvarValue )
{
	HRESULT l_hr = g_matroxAcqDevice.CameraGetParameter( triggerchannel, p_iParameter, p_piParameterType, p_pvarValue );

	return l_hr;
}

HRESULT WINAPI SVDigitizerParameterSetValue( unsigned long triggerchannel, int p_iParameter, int p_iParameterType, VARIANT *p_pvarValue )
{
	HRESULT l_hr = g_matroxAcqDevice.CameraSetParameter( triggerchannel, p_iParameter, p_iParameterType, p_pvarValue );

	return l_hr;
}

HRESULT WINAPI SVDigitizerSetParameters( unsigned long triggerchannel, const SVDeviceParamCollection* p_pParameters )
{
	HRESULT l_hr = S_OK;
	
	SVDeviceParamMap::const_iterator iter;
	for (iter = p_pParameters->mapParameters.begin(); iter != p_pParameters->mapParameters.end(); ++iter)
	{
		const SVDeviceParamWrapper& w = iter->second;
		if ( nullptr != ((const SVDeviceParam*) w) )
		{
			SVDigitizerSetParameter( triggerchannel, &w );
		}
	}

	return l_hr;
}

HRESULT WINAPI SVDigitizerSetParameter( unsigned long triggerchannel, const SVDeviceParamWrapper* p_pParameter )
{
	HRESULT l_hr = S_OK;

	if ( nullptr != p_pParameter )
	{
		const SVDeviceParamWrapper& rw = *p_pParameter;
		if ( nullptr != ((const SVDeviceParam*) rw) )
		{
			switch ( rw->Type() )
			{
				case DeviceParamLightReference:
				{
					const SVLightReferenceDeviceParam* pLR = rw.DerivedValue( pLR );
					l_hr = S_FALSE;
					break;
				}

				case DeviceParamLut:
				{
					const SVLutDeviceParam* pLut = rw.DerivedValue( pLut );
					l_hr = S_FALSE;
					break;
				}

				default:
				{
					l_hr = S_FALSE;
					break;
				}
			}
		}
	}
	return l_hr;
}

HRESULT WINAPI SVDigitizerGetParameter( unsigned long triggerchannel, SVDeviceParamEnum p_eParameter, SVDeviceParamWrapper** p_ppParameter )
{
	HRESULT l_hr = S_OK;
	
	if ( nullptr != p_ppParameter )
	{
		*p_ppParameter = new SVDeviceParamWrapper;
		SVDeviceParamWrapper& rw = **p_ppParameter;
		switch ( p_eParameter )
		{
			case DeviceParamLightReference:
			{
				rw = SVLightReferenceDeviceParam();
				SVLightReferenceDeviceParam* pLR = rw.DerivedValue( pLR );
				l_hr = S_FALSE;
				break;
			}

			case DeviceParamLut:
			{
				rw = SVLutDeviceParam();
				SVLutDeviceParam* pParam = rw.DerivedValue( pParam );
				l_hr = S_FALSE;
				break;
			}

			default:
			{
				l_hr = S_FALSE;
				break;
			}
		}
	}
	return l_hr;
}

HRESULT WINAPI SVDigitizerDestroyParameter( unsigned long triggerchannel, SVDeviceParamWrapper* p_pParameter )
{
	SVDeviceParamWrapper& rw = *p_pParameter;
	rw.Clear();

	return S_OK;
}

HRESULT WINAPI SVDigitizerScanForCameras()
{
	g_matroxAcqDevice.ScanForCameras();

	return S_OK;
}

