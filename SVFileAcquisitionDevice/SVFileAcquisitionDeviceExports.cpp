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
#include "SVFileAcquisitionDeviceApp.h"
#include "SVFileAcquisitionDeviceExports.h"
#include "SVFileAcquisitionDeviceParamEnum.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVOMFCLibrary/SVLongValueDeviceParam.h"
#include "SVOMFCLibrary/SVStringValueDeviceParam.h"
#include "TriggerHandling/SVCallbackStruct.h"

// General Export Functions

HRESULT WINAPI SVCreate()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = g_svTheApp.m_fileAcqDevice.Create();
	return l_hrOk;
}

HRESULT WINAPI SVDestroy()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = g_svTheApp.m_fileAcqDevice.Destroy();
	return l_hrOk;
}

// Trigger Export Functions

HRESULT WINAPI SVTriggerGetCount( unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	if ( nullptr != p_pulCount )
	{
		l_hr = g_svTheApp.m_fileAcqDevice.TriggerGetCount( *p_pulCount );
	}
	return l_hr;
}

HRESULT WINAPI SVTriggerGetHandle( unsigned long *p_pulHandle, unsigned long p_ulIndex )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	if ( nullptr != p_pulHandle )
	{
		*p_pulHandle = g_svTheApp.m_fileAcqDevice.TriggerGetHandle(p_ulIndex);
		l_hr = S_OK;
	} 
	return l_hr;
}

HRESULT WINAPI SVTriggerGetName( unsigned long p_ulHandle, BSTR *p_pbstrName )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	if ( nullptr != p_pbstrName )
	{
		l_hr = S_OK;

		if ( nullptr != *p_pbstrName )
		{
			::SysFreeString( *p_pbstrName );

			*p_pbstrName = nullptr;
		}
		l_hr = g_svTheApp.m_fileAcqDevice.TriggerGetName(p_ulHandle, *p_pbstrName);
	} 
	return l_hr;
}

HRESULT WINAPI SVTriggerRegister( unsigned long p_ulHandle, SVCallbackPtr p_pCallback, void *p_pOwner, void *p_pData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	if ( nullptr != p_pCallback )
	{
		SvTh::SVCallbackStruct l_Callback;

		l_Callback.m_pCallback = p_pCallback;
		l_Callback.m_pOwner = p_pOwner;
		l_Callback.m_pData = p_pData;

		l_hr = g_svTheApp.m_fileAcqDevice.TriggerRegisterCallback(p_ulHandle, l_Callback);
	} 
	return l_hr;
}

HRESULT WINAPI SVTriggerUnregister( unsigned long p_ulHandle, SVCallbackPtr p_pCallback, void *p_pOwner, void *p_pData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_OK;

	if ( nullptr != p_pCallback )
	{
		SvTh::SVCallbackStruct l_Callback;

		l_Callback.m_pCallback = p_pCallback;
		l_Callback.m_pOwner = p_pOwner;
		l_Callback.m_pData = p_pData;

		if ( S_OK != g_svTheApp.m_fileAcqDevice.TriggerUnregisterCallback(p_ulHandle, l_Callback) )
		{
			l_hr = S_FALSE;
		}
	} 
	return l_hr;
}

HRESULT WINAPI SVTriggerUnregisterAll( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_OK;

	if ( S_OK != g_svTheApp.m_fileAcqDevice.TriggerUnregisterAllCallbacks(p_ulHandle) )
	{
		l_hr = S_FALSE;
	}
	return l_hr;
}

HRESULT WINAPI SVTriggerStart( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_fileAcqDevice.TriggerStart(p_ulHandle);

	return l_hr;
}

HRESULT WINAPI SVTriggerStop( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_OK;

	if ( S_OK != g_svTheApp.m_fileAcqDevice.TriggerStop(p_ulHandle) )
	{
		l_hr = S_FALSE;
	} 
	return l_hr;
}

HRESULT WINAPI SVTriggerGetParameterCount( unsigned long p_ulHandle, unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	HRESULT l_hrOk = g_svTheApp.m_fileAcqDevice.TriggerGetParameterCount(p_ulHandle, p_pulCount);
	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetParameterName( unsigned long p_ulHandle, unsigned long p_ulIndex, BSTR *p_pbstrName )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	HRESULT l_hrOk = g_svTheApp.m_fileAcqDevice.TriggerGetParameterName(p_ulHandle, p_ulIndex, p_pbstrName);
	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	HRESULT l_hrOk = g_svTheApp.m_fileAcqDevice.TriggerGetParameterValue(p_ulHandle, p_ulIndex, p_pvarValue);
	return l_hrOk;
}

HRESULT WINAPI SVTriggerSetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	HRESULT l_hrOk = g_svTheApp.m_fileAcqDevice.TriggerSetParameterValue(p_ulHandle, p_ulIndex, p_pvarValue);
	return l_hrOk;
}

// Digitizer Export Functions
HRESULT WINAPI SVDigitizerGetCount( unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulCount )
	{
		l_hrOk = g_svTheApp.m_fileAcqDevice.CameraGetCount( *p_pulCount );
	}
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetHandle( unsigned long *p_pulHandle, unsigned long p_ulIndex )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulHandle )
	{
		unsigned long l_ulCount = 0;

		l_hrOk = g_svTheApp.m_fileAcqDevice.CameraGetCount( l_ulCount );

		if ( S_OK == l_hrOk )
		{
			if ( p_ulIndex < l_ulCount )
			{
				l_hrOk = S_OK;

				*p_pulHandle = p_ulIndex + 1;
			}
			else
			{
				*p_pulHandle = 0;
			}
		}
	} 
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetName( unsigned long p_ulHandle, BSTR *p_pbstrName )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pbstrName )
	{
		if ( nullptr != *p_pbstrName )
		{
			::SysFreeString( *p_pbstrName );

			*p_pbstrName = nullptr;
		}
		l_hrOk = g_svTheApp.m_fileAcqDevice.CameraGetName( p_ulHandle - 1, *p_pbstrName );
	} 
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerLoadCameraFiles( unsigned long p_ulHandle, SAFEARRAY* p_psaFileNames )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = g_svTheApp.m_fileAcqDevice.CameraLoadFiles(p_ulHandle - 1, p_psaFileNames);
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetBufferWidth( unsigned long p_ulHandle, unsigned long *p_pulWidth )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulWidth )
	{
		*p_pulWidth = 0;

		l_hrOk = g_svTheApp.m_fileAcqDevice.CameraGetWidth( p_ulHandle - 1, *p_pulWidth );
	}
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetBufferHeight( unsigned long p_ulHandle, unsigned long *p_pulHeight )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulHeight )
	{
		*p_pulHeight = 0;

		l_hrOk = g_svTheApp.m_fileAcqDevice.CameraGetHeight( p_ulHandle - 1, *p_pulHeight );
	}
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetBufferFormat( unsigned long p_ulHandle, int *p_piFormat )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_piFormat )
	{
		*p_piFormat = SVImageFormatUnknown;

		l_hrOk = g_svTheApp.m_fileAcqDevice.CameraGetFormat( p_ulHandle - 1, *p_piFormat );
	}
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerCreateBuffers( unsigned long p_ulHandle, unsigned long p_ulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = g_svTheApp.m_fileAcqDevice.CameraBufferCreateAll( p_ulHandle - 1, p_ulCount );
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetLightReference( unsigned long p_ulHandle, int p_iType, SAFEARRAY *p_psaData )
{
	HRESULT l_hrOk = S_FALSE;
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerSetLightReference( unsigned long p_ulHandle, int p_iType, SAFEARRAY p_saData )
{
	HRESULT l_hrOk = S_FALSE;
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetLightReferenceBand( unsigned long p_ulHandle, unsigned long p_ulBand, int p_iType, VARIANT *p_psaData )
{
	HRESULT l_hrOk = S_FALSE;
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerSetLightReferenceBand( unsigned long p_ulHandle, unsigned long p_ulBand, int p_iType, VARIANT p_saData )
{
	HRESULT l_hrOk = S_FALSE;
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetLut( unsigned long p_ulHandle, SAFEARRAY *p_psaData )
{
	HRESULT l_hrOk = S_FALSE;
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerSetLut( unsigned long p_ulHandle, SAFEARRAY p_saData )
{
	HRESULT l_hrOk = S_FALSE;
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetLutBand( unsigned long p_ulHandle, unsigned long p_ulBand, SAFEARRAY *p_psaData )
{
	HRESULT l_hrOk = S_FALSE;
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerSetLutBand( unsigned long p_ulHandle, unsigned long p_ulBand, SAFEARRAY p_saData )
{
	HRESULT l_hrOk = S_FALSE;
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerRegisterBufferInterface( unsigned long p_ulHandle, SVAcquisitionBufferInterface* p_pInterface )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	l_hrOk = g_svTheApp.m_fileAcqDevice.CameraRegisterBufferInterface( p_ulHandle - 1, p_pInterface );

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerStart( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = g_svTheApp.m_fileAcqDevice.CameraStart( p_ulHandle - 1 );
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerStop( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = g_svTheApp.m_fileAcqDevice.CameraStop( p_ulHandle - 1 );
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerUnregisterBufferInterface( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	l_hrOk = g_svTheApp.m_fileAcqDevice.CameraUnregisterBufferInterface( p_ulHandle - 1 );

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerInternalTriggerEnable( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = g_svTheApp.m_fileAcqDevice.InternalTriggerEnable( p_ulHandle - 1 );
	return l_hrOk; 
}

HRESULT WINAPI SVDigitizerInternalTrigger( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = g_svTheApp.m_fileAcqDevice.InternalTrigger( p_ulHandle - 1 );
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerInternalTriggerRegister( unsigned long p_ulHandle, SVFileAcquisitionCallbackPtr p_pCallback, void *p_pOwner, void *p_pData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	unsigned long l_ulCount = 0;
	HRESULT l_hrOk = g_svTheApp.m_fileAcqDevice.CameraGetCount( l_ulCount );
	
	if ( S_OK == l_hrOk && p_ulHandle <= l_ulCount )
	{
		SvTh::SVCallbackStruct l_Callback;
		l_Callback.m_pCallback = p_pCallback;
		l_Callback.m_pOwner = p_pOwner;
		l_Callback.m_pData = p_pData;

		l_hrOk = g_svTheApp.m_fileAcqDevice.RegisterInternalTriggerCallback( p_ulHandle - 1, l_Callback );
	}
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerInternalTriggerUnregister( unsigned long p_ulHandle, SVFileAcquisitionCallbackPtr p_pCallback, void *p_pOwner, void *p_pData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	unsigned long l_ulCount = 0;
	HRESULT l_hrOk = g_svTheApp.m_fileAcqDevice.CameraGetCount( l_ulCount );
	
	if ( S_OK == l_hrOk && p_ulHandle <= l_ulCount )
	{
		SvTh::SVCallbackStruct l_Callback;
		l_Callback.m_pCallback = p_pCallback;
		l_Callback.m_pOwner = p_pOwner;
		l_Callback.m_pData = p_pData;

		l_hrOk = g_svTheApp.m_fileAcqDevice.UnregisterInternalTriggerCallback( p_ulHandle - 1, l_Callback );
	}
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerInternalTriggerUnregisterAll( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	unsigned long l_ulCount = 0;
	HRESULT l_hrOk = g_svTheApp.m_fileAcqDevice.CameraGetCount( l_ulCount );
	
	if ( S_OK == l_hrOk && p_ulHandle <= l_ulCount )
	{
		l_hrOk = g_svTheApp.m_fileAcqDevice.UnregisterAllInternalTriggerCallbacks(p_ulHandle - 1);
	}
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerDestroyBuffers( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = g_svTheApp.m_fileAcqDevice.CameraBufferDestroyAll( p_ulHandle - 1 );
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerUnloadCameraFile( unsigned long p_ulHandle )
{
	HRESULT l_hrOk = S_FALSE;
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerParameterGetList( unsigned long p_ulHandle, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = g_svTheApp.m_fileAcqDevice.CameraGetParameterList( p_ulHandle - 1, p_pvarValue );
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerParameterGetName( unsigned long p_ulHandle, unsigned long p_ulParameter, BSTR *p_pbstrName )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pbstrName )
	{
		if ( nullptr != *p_pbstrName )
		{
			::SysFreeString( *p_pbstrName );

			*p_pbstrName = nullptr;
		}
		l_hrOk = g_svTheApp.m_fileAcqDevice.CameraGetParameterName( p_ulHandle - 1, p_ulParameter, *p_pbstrName );
	}
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerParameterGetValue( unsigned long p_ulHandle, int p_iParameter, int *p_piParameterType, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = g_svTheApp.m_fileAcqDevice.CameraGetParameter( p_ulHandle - 1, p_iParameter, p_piParameterType, p_pvarValue );
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerParameterSetValue( unsigned long p_ulHandle, int p_iParameter, int p_iParameterType, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = g_svTheApp.m_fileAcqDevice.CameraSetParameter( p_ulHandle - 1, p_iParameter, p_iParameterType, p_pvarValue );
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerSetParameters( unsigned long p_ulHandle, const SVDeviceParamCollection* p_pParameters )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	unsigned long l_ulCount = 0;

	HRESULT l_hrOk = g_svTheApp.m_fileAcqDevice.CameraGetCount( l_ulCount );
	
	if ( S_OK == l_hrOk && p_ulHandle <= l_ulCount )
	{
		SVDeviceParamMap::const_iterator iter;
		for (iter = p_pParameters->mapParameters.begin(); iter != p_pParameters->mapParameters.end(); ++iter)
		{
			const SVDeviceParamWrapper& w = iter->second;
			if ( nullptr != ((const SVDeviceParam*) w) )
			{
				SVDigitizerSetParameter( p_ulHandle, &w );
			}
		}
	}
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerSetParameter( unsigned long p_ulHandle, const SVDeviceParamWrapper* p_pParameter )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	unsigned long l_ulCount = 0;

	HRESULT l_hrOk = g_svTheApp.m_fileAcqDevice.CameraGetCount( l_ulCount );
	
	if ( S_OK == l_hrOk && p_ulHandle <= l_ulCount )
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
						l_hrOk = g_svTheApp.m_fileAcqDevice.CameraSetParameter( p_ulHandle - 1, SVFileAcquisitionParameterFilename, 0, &value );
						break;
					}

					case DeviceParamFileAcqImageDirName:
					{
						const SVStringValueDeviceParam* pDirname = rw.DerivedValue( pDirname );
						_variant_t value;
						pDirname->GetValue(value);
						l_hrOk = g_svTheApp.m_fileAcqDevice.CameraSetParameter( p_ulHandle - 1, SVFileAcquisitionParameterDirectory, 0, &value );
						break;
					}

					case DeviceParamFileAcqLoadingMode:
					{
						const SVLongValueDeviceParam* pLoadingMode = rw.DerivedValue( pLoadingMode );
						_variant_t value;
						pLoadingMode->GetValue(value);
						l_hrOk = g_svTheApp.m_fileAcqDevice.CameraSetParameter( p_ulHandle - 1, SVFileAcquisitionParameterLoadingMode, 0, &value );
						break;
					}

					case DeviceParamFileAcqImageWidth:
					{
						const SVLongValueDeviceParam* pImageWidth = rw.DerivedValue( pImageWidth );
						_variant_t value;
						pImageWidth->GetValue(value);
						l_hrOk = g_svTheApp.m_fileAcqDevice.CameraSetParameter( p_ulHandle - 1, SVFileAcquisitionParameterImageWidth, 0, &value );
						break;
					}

					case DeviceParamFileAcqImageHeight:
					{
						const SVLongValueDeviceParam* pImageHeight = rw.DerivedValue( pImageHeight );
						_variant_t value;
						pImageHeight->GetValue(value);
						l_hrOk = g_svTheApp.m_fileAcqDevice.CameraSetParameter( p_ulHandle - 1, SVFileAcquisitionParameterImageHeight, 0, &value );
						break;
					}

					case DeviceParamFileAcqImageFormat:
					{
						const SVLongValueDeviceParam* pImageFormat = rw.DerivedValue( pImageFormat );
						_variant_t value;
						pImageFormat->GetValue(value);
						l_hrOk = g_svTheApp.m_fileAcqDevice.CameraSetParameter( p_ulHandle - 1, SVFileAcquisitionParameterImageFormat, 0, &value );
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

HRESULT WINAPI SVDigitizerGetParameter( unsigned long p_ulHandle, SVDeviceParamEnum p_eParameter, SVDeviceParamWrapper** p_ppParameter )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	unsigned long l_ulCount = 0;

	HRESULT l_hrOk = g_svTheApp.m_fileAcqDevice.CameraGetCount( l_ulCount );
	
	if ( S_OK == l_hrOk && p_ulHandle <= l_ulCount )
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
					l_hrOk = g_svTheApp.m_fileAcqDevice.CameraGetParameter( p_ulHandle - 1, SVFileAcquisitionParameterFilename, 0, &value );
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
					l_hrOk = g_svTheApp.m_fileAcqDevice.CameraGetParameter( p_ulHandle - 1, SVFileAcquisitionParameterDirectory, 0, &value );
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
					l_hrOk = g_svTheApp.m_fileAcqDevice.CameraGetParameter( p_ulHandle - 1, SVFileAcquisitionParameterLoadingMode, 0, &value );
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
					l_hrOk = g_svTheApp.m_fileAcqDevice.CameraGetParameter( p_ulHandle - 1, SVFileAcquisitionParameterImageWidth, 0, &value );
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
					l_hrOk = g_svTheApp.m_fileAcqDevice.CameraGetParameter( p_ulHandle - 1, SVFileAcquisitionParameterImageHeight, 0, &value );
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
					l_hrOk = g_svTheApp.m_fileAcqDevice.CameraGetParameter( p_ulHandle - 1, SVFileAcquisitionParameterImageFormat, 0, &value );
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

HRESULT WINAPI SVDigitizerDestroyParameter( unsigned long p_ulHandle, SVDeviceParamWrapper* p_pParameter )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	SVDeviceParamWrapper& rw = *p_pParameter;
	rw.Clear();
	return l_hrOk;
}

