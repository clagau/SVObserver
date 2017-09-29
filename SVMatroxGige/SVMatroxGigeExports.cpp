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

#include "SVImageLibrary/SVImagingDeviceParams.h"

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

HRESULT WINAPI SVTriggerGetHandle( unsigned long *pTriggerchannel, unsigned long p_ulIndex )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	if ( nullptr != pTriggerchannel )
	{
		*pTriggerchannel = g_svTheApp.m_svSystem.TriggerGetHandle(p_ulIndex);
		l_hr = S_OK;
	} 
	return l_hr;
}

HRESULT WINAPI SVTriggerGetName( unsigned long triggerchannel, BSTR *p_pbstrName )
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
		l_hr = g_svTheApp.m_svSystem.TriggerGetName(triggerchannel, *p_pbstrName);
	} 
	return l_hr;
}

HRESULT WINAPI SVTriggerRegister( unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	if ( rDispatcher.hasCallback() )
	{
		l_hr = g_svTheApp.m_svSystem.TriggerRegisterCallback(triggerchannel, rDispatcher);
	} 
	return l_hr;
}

HRESULT WINAPI SVTriggerUnregister( unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_OK;

	if ( rDispatcher.hasCallback() )
	{
		if ( S_OK != g_svTheApp.m_svSystem.TriggerUnregisterCallback(triggerchannel, rDispatcher) )
		{
			l_hr = S_FALSE;
		}
	} 
	return l_hr;
}

HRESULT WINAPI SVTriggerUnregisterAll( unsigned long triggerchannel )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_OK;

	if ( S_OK != g_svTheApp.m_svSystem.TriggerUnregisterAllCallbacks(triggerchannel) )
	{
		l_hr = S_FALSE;
	}
	return l_hr;
}

HRESULT WINAPI SVTriggerStart( unsigned long triggerchannel )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.TriggerStart(triggerchannel);

	return l_hr;
}

HRESULT WINAPI SVTriggerStop( unsigned long triggerchannel )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_OK;

	if ( S_OK != g_svTheApp.m_svSystem.TriggerStop(triggerchannel) )
	{
		l_hr = S_FALSE;
	} 
	return l_hr;
}

HRESULT WINAPI SVTriggerGetParameterCount( unsigned long triggerchannel, unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	HRESULT l_hrOk = g_svTheApp.m_svSystem.TriggerGetParameterCount(triggerchannel, p_pulCount);
	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetParameterName( unsigned long triggerchannel, unsigned long p_ulIndex, BSTR *p_pbstrName )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	HRESULT l_hrOk = g_svTheApp.m_svSystem.TriggerGetParameterName(triggerchannel, p_ulIndex, p_pbstrName);
	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetParameterValue( unsigned long triggerchannel, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	HRESULT l_hrOk = g_svTheApp.m_svSystem.TriggerGetParameterValue(triggerchannel, p_ulIndex, p_pvarValue);
	return l_hrOk;
}

HRESULT WINAPI SVTriggerSetParameterValue( unsigned long triggerchannel, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	HRESULT l_hrOk = g_svTheApp.m_svSystem.TriggerSetParameterValue(triggerchannel, p_ulIndex, p_pvarValue);
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

HRESULT WINAPI SVDigitizerGetHandle( unsigned long *pTriggerchannel, unsigned long p_ulIndex )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	if ( nullptr != pTriggerchannel )
	{
		unsigned long l_ulCount = 0;

		l_hr = g_svTheApp.m_svSystem.CameraGetCount( l_ulCount );

		if ( S_OK == l_hr )
		{
			if ( p_ulIndex < l_ulCount )
			{
				l_hr = S_OK;

				*pTriggerchannel = g_svTheApp.m_svSystem.GetDigitizerHandle(p_ulIndex);
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
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	if ( nullptr != p_pbstrName )
	{
		if ( nullptr != *p_pbstrName )
		{
			::SysFreeString( *p_pbstrName );

			*p_pbstrName = nullptr;
		}

		l_hr = g_svTheApp.m_svSystem.CameraGetName( triggerchannel, *p_pbstrName );
	} 

	return l_hr;
}

HRESULT WINAPI SVDigitizerLoadCameraFiles( unsigned long triggerchannel, SAFEARRAY* p_psaFileNames )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.CameraLoadFiles(triggerchannel, p_psaFileNames);

	return l_hr;
}

HRESULT WINAPI SVDigitizerGetBufferWidth( unsigned long triggerchannel, unsigned long *p_pulWidth )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	if ( nullptr != p_pulWidth )
	{
		*p_pulWidth = 0;

		l_hr = g_svTheApp.m_svSystem.CameraGetWidth( triggerchannel, *p_pulWidth );
	}

	return l_hr;
}

HRESULT WINAPI SVDigitizerGetBufferHeight( unsigned long triggerchannel, unsigned long *p_pulHeight )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	if ( nullptr != p_pulHeight )
	{
		*p_pulHeight = 0;

		l_hr = g_svTheApp.m_svSystem.CameraGetHeight( triggerchannel, *p_pulHeight );
	}

	return l_hr;
}

HRESULT WINAPI SVDigitizerGetBufferFormat( unsigned long triggerchannel, int *p_piFormat )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	if ( nullptr != p_piFormat )
	{
		*p_piFormat = SVImageFormatUnknown;

		l_hr = g_svTheApp.m_svSystem.CameraGetFormat( triggerchannel, *p_piFormat );
	}

	return l_hr;
}

HRESULT WINAPI SVDigitizerCreateBuffers( unsigned long triggerchannel, unsigned long p_ulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.CameraBufferCreateAll( triggerchannel, p_ulCount );

	return l_hr;
}

HRESULT WINAPI SVDigitizerGetLightReference( unsigned long triggerchannel, int p_iType, SAFEARRAY *p_psaData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	return l_hr;
}

HRESULT WINAPI SVDigitizerSetLightReference( unsigned long triggerchannel, int p_iType, SAFEARRAY p_saData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	return l_hr;
}

HRESULT WINAPI SVDigitizerGetLightReferenceBand( unsigned long triggerchannel, unsigned long p_ulBand, int p_iType, VARIANT *p_psaData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	return l_hr;
}

HRESULT WINAPI SVDigitizerSetLightReferenceBand( unsigned long triggerchannel, unsigned long p_ulBand, int p_iType, VARIANT p_saData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	return l_hr;
}

HRESULT WINAPI SVDigitizerGetLut( unsigned long triggerchannel, SAFEARRAY *p_psaData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	return l_hr;
}

HRESULT WINAPI SVDigitizerSetLut( unsigned long triggerchannel, SAFEARRAY p_saData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	return l_hr;
}

HRESULT WINAPI SVDigitizerGetLutBand( unsigned long triggerchannel, unsigned long p_ulBand, SAFEARRAY *p_psaData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	return l_hr;
}

HRESULT WINAPI SVDigitizerSetLutBand( unsigned long triggerchannel, unsigned long p_ulBand, SAFEARRAY p_saData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	return l_hr;
}

HRESULT WINAPI SVDigitizerRegisterBufferInterface( unsigned long triggerchannel, SVAcquisitionBufferInterface* p_pInterface )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.CameraRegisterBufferInterface( triggerchannel, p_pInterface );

	return l_hr;
}

HRESULT WINAPI SVDigitizerStart( unsigned long triggerchannel )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.CameraStart( triggerchannel );

	return l_hr;
}

HRESULT WINAPI SVDigitizerStop( unsigned long triggerchannel )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.CameraStop( triggerchannel );

	return l_hr;
}

HRESULT WINAPI SVDigitizerUnregisterBufferInterface( unsigned long triggerchannel )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.CameraUnregisterBufferInterface( triggerchannel );

	return l_hr;
}

HRESULT WINAPI SVDigitizerInternalTriggerEnable( unsigned long triggerchannel )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.InternalTriggerEnable( triggerchannel );
	
	return l_hr;
}

HRESULT WINAPI SVDigitizerInternalTrigger( unsigned long triggerchannel )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.InternalTrigger( triggerchannel );
	
	return l_hr;
}

HRESULT WINAPI SVDigitizerInternalTriggerRegister( unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	
	HRESULT l_hr = g_svTheApp.m_svSystem.RegisterInternalTriggerCallback( triggerchannel, rDispatcher );

	return l_hr;
}

HRESULT WINAPI SVDigitizerInternalTriggerUnregister( unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.UnregisterInternalTriggerCallback( triggerchannel, rDispatcher );

	return l_hr;
}

HRESULT WINAPI SVDigitizerInternalTriggerUnregisterAll( unsigned long triggerchannel )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.UnregisterAllInternalTriggerCallbacks(triggerchannel);

	return l_hr;
}

HRESULT WINAPI SVDigitizerDestroyBuffers( unsigned long triggerchannel )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.CameraBufferDestroyAll( triggerchannel );

	return l_hr;
}

HRESULT WINAPI SVDigitizerUnloadCameraFile( unsigned long triggerchannel )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	return l_hr;
}

HRESULT WINAPI SVDigitizerParameterGetList( unsigned long triggerchannel, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.CameraGetParameterList( triggerchannel, p_pvarValue );

	return l_hr;
}

HRESULT WINAPI SVDigitizerParameterGetName( unsigned long triggerchannel, unsigned long p_ulParameter, BSTR *p_pbstrName )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.CameraGetParameterName( triggerchannel, p_ulParameter, p_pbstrName );

	return l_hr;
}

HRESULT WINAPI SVDigitizerParameterGetValue( unsigned long triggerchannel, int p_iParameter, int *p_piParameterType, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.CameraGetParameter( triggerchannel, p_iParameter, p_piParameterType, p_pvarValue );

	return l_hr;
}

HRESULT WINAPI SVDigitizerParameterSetValue( unsigned long triggerchannel, int p_iParameter, int p_iParameterType, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = g_svTheApp.m_svSystem.CameraSetParameter( triggerchannel, p_iParameter, p_iParameterType, p_pvarValue );

	return l_hr;
}

HRESULT WINAPI SVDigitizerSetParameters( unsigned long triggerchannel, const SVDeviceParamCollection* p_pParameters )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

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

HRESULT WINAPI SVDigitizerGetParameter( unsigned long triggerchannel, SVDeviceParamEnum p_eParameter, SVDeviceParamWrapper** p_ppParameter )
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

HRESULT WINAPI SVDigitizerDestroyParameter( unsigned long triggerchannel, SVDeviceParamWrapper* p_pParameter )
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

