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
#include "SVMatroxGigeApp.h"

#include "SVHBitmapUtilitiesLibrary/SVImageFormatEnum.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVImageLibrary/SVLut.h"
#pragma endregion Includes

// General Export Functions

HRESULT WINAPI SVCreate()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.Create();
	return l_hr;
}

HRESULT WINAPI SVDestroy()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.Destroy();
	return l_hr;
}

// Trigger Export Functions

HRESULT WINAPI SVTriggerGetCount( unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	if ( nullptr != p_pulCount )
	{
		l_hr = g_svTheApp.m_svSystem.TriggerGetCount( *p_pulCount );
	}
	return l_hr;
}

HRESULT WINAPI SVTriggerGetHandle( unsigned long *p_pulHandle, unsigned long p_ulIndex )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	if ( nullptr != p_pulHandle )
	{
		*p_pulHandle = g_svTheApp.m_svSystem.TriggerGetHandle(p_ulIndex);
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
		l_hr = g_svTheApp.m_svSystem.TriggerGetName(p_ulHandle, *p_pbstrName);
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

		l_hr = g_svTheApp.m_svSystem.TriggerRegisterCallback(p_ulHandle, l_Callback);
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

		if ( S_OK != g_svTheApp.m_svSystem.TriggerUnregisterCallback(p_ulHandle, l_Callback) )
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

	if ( S_OK != g_svTheApp.m_svSystem.TriggerUnregisterAllCallbacks(p_ulHandle) )
	{
		l_hr = S_FALSE;
	}
	return l_hr;
}

HRESULT WINAPI SVTriggerStart( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.TriggerStart(p_ulHandle);

	return l_hr;
}

HRESULT WINAPI SVTriggerStop( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_OK;

	if ( S_OK != g_svTheApp.m_svSystem.TriggerStop(p_ulHandle) )
	{
		l_hr = S_FALSE;
	} 
	return l_hr;
}

HRESULT WINAPI SVTriggerGetParameterCount( unsigned long p_ulHandle, unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	HRESULT l_hrOk = g_svTheApp.m_svSystem.TriggerGetParameterCount(p_ulHandle, p_pulCount);
	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetParameterName( unsigned long p_ulHandle, unsigned long p_ulIndex, BSTR *p_pbstrName )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	HRESULT l_hrOk = g_svTheApp.m_svSystem.TriggerGetParameterName(p_ulHandle, p_ulIndex, p_pbstrName);
	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	HRESULT l_hrOk = g_svTheApp.m_svSystem.TriggerGetParameterValue(p_ulHandle, p_ulIndex, p_pvarValue);
	return l_hrOk;
}

HRESULT WINAPI SVTriggerSetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	HRESULT l_hrOk = g_svTheApp.m_svSystem.TriggerSetParameterValue(p_ulHandle, p_ulIndex, p_pvarValue);
	return l_hrOk;
}

// Digitizer Export Functions

HRESULT WINAPI SVDigitizerGetCount( unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	if ( nullptr != p_pulCount )
	{
		l_hr = g_svTheApp.m_svSystem.CameraGetCount( *p_pulCount );
	}

	return l_hr;
}

HRESULT WINAPI SVDigitizerGetHandle( unsigned long *p_pulHandle, unsigned long p_ulIndex )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	if ( nullptr != p_pulHandle )
	{
		unsigned long l_ulCount = 0;

		l_hr = g_svTheApp.m_svSystem.CameraGetCount( l_ulCount );

		if ( S_OK == l_hr )
		{
			if ( p_ulIndex < l_ulCount )
			{
				l_hr = S_OK;

				*p_pulHandle = g_svTheApp.m_svSystem.GetDigitizerHandle(p_ulIndex);
			}
			else
			{
				*p_pulHandle = 0;
			}
		}
	} 

	return l_hr;
}

HRESULT WINAPI SVDigitizerGetName( unsigned long p_ulHandle, BSTR *p_pbstrName )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	if ( nullptr != p_pbstrName )
	{
		if ( nullptr != *p_pbstrName )
		{
			::SysFreeString( *p_pbstrName );

			*p_pbstrName = nullptr;
		}

		l_hr = g_svTheApp.m_svSystem.CameraGetName( p_ulHandle, *p_pbstrName );
	} 

	return l_hr;
}

HRESULT WINAPI SVDigitizerLoadCameraFiles( unsigned long p_ulHandle, SAFEARRAY* p_psaFileNames )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.CameraLoadFiles(p_ulHandle, p_psaFileNames);

	return l_hr;
}

HRESULT WINAPI SVDigitizerGetBufferWidth( unsigned long p_ulHandle, unsigned long *p_pulWidth )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	if ( nullptr != p_pulWidth )
	{
		*p_pulWidth = 0;

		l_hr = g_svTheApp.m_svSystem.CameraGetWidth( p_ulHandle, *p_pulWidth );
	}

	return l_hr;
}

HRESULT WINAPI SVDigitizerGetBufferHeight( unsigned long p_ulHandle, unsigned long *p_pulHeight )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	if ( nullptr != p_pulHeight )
	{
		*p_pulHeight = 0;

		l_hr = g_svTheApp.m_svSystem.CameraGetHeight( p_ulHandle, *p_pulHeight );
	}

	return l_hr;
}

HRESULT WINAPI SVDigitizerGetBufferFormat( unsigned long p_ulHandle, int *p_piFormat )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	if ( nullptr != p_piFormat )
	{
		*p_piFormat = SVImageFormatUnknown;

		l_hr = g_svTheApp.m_svSystem.CameraGetFormat( p_ulHandle, *p_piFormat );
	}

	return l_hr;
}

HRESULT WINAPI SVDigitizerCreateBuffers( unsigned long p_ulHandle, unsigned long p_ulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.CameraBufferCreateAll( p_ulHandle, p_ulCount );

	return l_hr;
}

HRESULT WINAPI SVDigitizerGetLightReference( unsigned long p_ulHandle, int p_iType, SAFEARRAY *p_psaData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	return l_hr;
}

HRESULT WINAPI SVDigitizerSetLightReference( unsigned long p_ulHandle, int p_iType, SAFEARRAY p_saData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	return l_hr;
}

HRESULT WINAPI SVDigitizerGetLightReferenceBand( unsigned long p_ulHandle, unsigned long p_ulBand, int p_iType, VARIANT *p_psaData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	return l_hr;
}

HRESULT WINAPI SVDigitizerSetLightReferenceBand( unsigned long p_ulHandle, unsigned long p_ulBand, int p_iType, VARIANT p_saData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	return l_hr;
}

HRESULT WINAPI SVDigitizerGetLut( unsigned long p_ulHandle, SAFEARRAY *p_psaData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	return l_hr;
}

HRESULT WINAPI SVDigitizerSetLut( unsigned long p_ulHandle, SAFEARRAY p_saData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	return l_hr;
}

HRESULT WINAPI SVDigitizerGetLutBand( unsigned long p_ulHandle, unsigned long p_ulBand, SAFEARRAY *p_psaData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	return l_hr;
}

HRESULT WINAPI SVDigitizerSetLutBand( unsigned long p_ulHandle, unsigned long p_ulBand, SAFEARRAY p_saData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	return l_hr;
}

HRESULT WINAPI SVDigitizerRegisterBufferInterface( unsigned long p_ulHandle, SVAcquisitionBufferInterface* p_pInterface )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.CameraRegisterBufferInterface( p_ulHandle, p_pInterface );

	return l_hr;
}

HRESULT WINAPI SVDigitizerStart( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.CameraStart( p_ulHandle );

	return l_hr;
}

HRESULT WINAPI SVDigitizerStop( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.CameraStop( p_ulHandle );

	return l_hr;
}

HRESULT WINAPI SVDigitizerUnregisterBufferInterface( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.CameraUnregisterBufferInterface( p_ulHandle );

	return l_hr;
}

HRESULT WINAPI SVDigitizerInternalTriggerEnable( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.InternalTriggerEnable( p_ulHandle );
	
	return l_hr;
}

HRESULT WINAPI SVDigitizerInternalTrigger( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.InternalTrigger( p_ulHandle );
	
	return l_hr;
}

HRESULT WINAPI SVDigitizerInternalTriggerRegister( unsigned long p_ulHandle, SVMatroxGigeCallbackPtr p_pCallback, void *p_pOwner, void *p_pData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	
	SvTh::SVCallbackStruct l_Callback;
	l_Callback.m_pCallback = p_pCallback;
	l_Callback.m_pOwner = p_pOwner;
	l_Callback.m_pData = p_pData;

	HRESULT l_hr = g_svTheApp.m_svSystem.RegisterInternalTriggerCallback( p_ulHandle, l_Callback );

	return l_hr;
}

HRESULT WINAPI SVDigitizerInternalTriggerUnregister( unsigned long p_ulHandle, SVMatroxGigeCallbackPtr p_pCallback, void *p_pOwner, void *p_pData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	SvTh::SVCallbackStruct l_Callback;
	l_Callback.m_pCallback = p_pCallback;
	l_Callback.m_pOwner = p_pOwner;
	l_Callback.m_pData = p_pData;

	HRESULT l_hr = g_svTheApp.m_svSystem.UnregisterInternalTriggerCallback( p_ulHandle, l_Callback );

	return l_hr;
}

HRESULT WINAPI SVDigitizerInternalTriggerUnregisterAll( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.UnregisterAllInternalTriggerCallbacks(p_ulHandle);

	return l_hr;
}

HRESULT WINAPI SVDigitizerDestroyBuffers( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.CameraBufferDestroyAll( p_ulHandle );

	return l_hr;
}

HRESULT WINAPI SVDigitizerUnloadCameraFile( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	return l_hr;
}

HRESULT WINAPI SVDigitizerParameterGetList( unsigned long p_ulHandle, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.CameraGetParameterList( p_ulHandle, p_pvarValue );

	return l_hr;
}

HRESULT WINAPI SVDigitizerParameterGetName( unsigned long p_ulHandle, unsigned long p_ulParameter, BSTR *p_pbstrName )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.CameraGetParameterName( p_ulHandle, p_ulParameter, p_pbstrName );

	return l_hr;
}

HRESULT WINAPI SVDigitizerParameterGetValue( unsigned long p_ulHandle, int p_iParameter, int *p_piParameterType, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.CameraGetParameter( p_ulHandle, p_iParameter, p_piParameterType, p_pvarValue );

	return l_hr;
}

HRESULT WINAPI SVDigitizerParameterSetValue( unsigned long p_ulHandle, int p_iParameter, int p_iParameterType, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.CameraSetParameter( p_ulHandle, p_iParameter, p_iParameterType, p_pvarValue );

	return l_hr;
}

HRESULT WINAPI SVDigitizerSetParameters( unsigned long p_ulHandle, const SVDeviceParamCollection* p_pParameters )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_OK;
	
	SVDeviceParamMap::const_iterator iter;
	for (iter = p_pParameters->mapParameters.begin(); iter != p_pParameters->mapParameters.end(); ++iter)
	{
		const SVDeviceParamWrapper& w = iter->second;
		if ( nullptr != ((const SVDeviceParam*) w) )
		{
			SVDigitizerSetParameter( p_ulHandle, &w );
		}
	}

	return l_hr;
}

HRESULT WINAPI SVDigitizerSetParameter( unsigned long p_ulHandle, const SVDeviceParamWrapper* p_pParameter )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

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

HRESULT WINAPI SVDigitizerGetParameter( unsigned long p_ulHandle, SVDeviceParamEnum p_eParameter, SVDeviceParamWrapper** p_ppParameter )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

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

HRESULT WINAPI SVDigitizerDestroyParameter( unsigned long p_ulHandle, SVDeviceParamWrapper* p_pParameter )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_OK;

	SVDeviceParamWrapper& rw = *p_pParameter;
	rw.Clear();

	return l_hr;
}

HRESULT WINAPI SVDigitizerScanForCameras()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_OK;

	g_svTheApp.m_svSystem.ScanForCameras();

	return l_hr;
}

