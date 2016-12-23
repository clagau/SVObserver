// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVEmptyIOExports
// * .File Name       : $Workfile:   SVEmptyIOExports.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 09:13:42  $
// ******************************************************************************

#include "StdAfx.h"
#include "SVEmptyIOExports.h"
#include "SVHBitmapUtilitiesLibrary\SVImageFormatEnum.h"
#include "CameraLibrary/SVDeviceParams.h"

HRESULT WINAPI SVCreate()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	return l_hrOk;
}

HRESULT WINAPI SVDestroy()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	return l_hrOk;
}

HRESULT WINAPI SVInputGetCount( unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulCount )
	{
		*p_pulCount = 8;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVInputGetValue( unsigned long p_ulChannel, bool *p_pbValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pbValue )
	{
		*p_pbValue = false;
	}

	return l_hrOk;
}

HRESULT WINAPI SVInputSetValue( unsigned long p_ulChannel, bool p_bValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVInputGetPortCount( unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulCount )
	{
		*p_pulCount = 0;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVInputGetPortValue( unsigned long p_ulPort, unsigned long *p_pulValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulValue )
	{
		*p_pulValue = 0;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVInputSetPortValue( unsigned long p_ulPort, unsigned long p_ulValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVOutputGetCount( unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulCount )
	{
		*p_pulCount = 16;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVOutputGetValue( unsigned long p_ulChannel, bool *p_pbValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVOutputSetValue( unsigned long p_ulChannel, bool p_bValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;;

	return l_hrOk;
}

HRESULT WINAPI SVOutputGetPortCount( unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulCount )
	{
		*p_pulCount = 0;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVOutputGetPortValue( unsigned long p_ulPort, unsigned long *p_pulValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVOutputSetPortValue( unsigned long p_ulPort, unsigned long p_ulValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetCount( unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulCount )
	{
		*p_pulCount = 4;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetHandle( unsigned long *pTriggerchannel, unsigned long p_ulIndex )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != pTriggerchannel )
	{
		*pTriggerchannel = p_ulIndex + 1;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetName( unsigned long triggerchannel, BSTR *p_pbstrName )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pbstrName && 0 < triggerchannel && triggerchannel <= 4 )
	{
		l_hrOk = S_OK;

		if ( nullptr != *p_pbstrName )
		{
			::SysFreeString( *p_pbstrName );

			*p_pbstrName = nullptr;
		}

		CString l_csName;
		
		l_csName.Format( "IO_Board_1.Dig_%d", triggerchannel - 1 );

		*p_pbstrName = l_csName.AllocSysString();
	} 

	return l_hrOk;
}

HRESULT WINAPI SVTriggerRegister( unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( rDispatcher.hasCallback() && 0 < triggerchannel && triggerchannel <= 4 )
	{
		l_hrOk = S_OK;
	} 

	return l_hrOk;
}

HRESULT WINAPI SVTriggerUnregister( unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( rDispatcher.hasCallback() && 0 < triggerchannel && triggerchannel <= 4 )
	{
		l_hrOk = S_OK;
	} 

	return l_hrOk;
}

HRESULT WINAPI SVTriggerUnregisterAll( unsigned long triggerchannel )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( 0 < triggerchannel && triggerchannel <= 4 )
	{
		l_hrOk = S_OK;
	} 

	return l_hrOk;
}

HRESULT WINAPI SVTriggerStart( unsigned long triggerchannel )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( 0 < triggerchannel && triggerchannel <= 4 )
	{
		l_hrOk = S_OK;
	} 

	return l_hrOk;
}

HRESULT WINAPI SVTriggerStop( unsigned long triggerchannel )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( 0 < triggerchannel && triggerchannel <= 4 )
	{
		l_hrOk = S_OK;
	} 

	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetParameterCount( unsigned long triggerchannel, unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulCount )
	{
		if ( 0 < triggerchannel && triggerchannel <= 4 )
		{
			*p_pulCount = 0;

			l_hrOk = S_OK;
		}
		else
		{
			*p_pulCount = 0;
		}
	}

	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetParameterName( unsigned long triggerchannel, unsigned long p_ulIndex, BSTR *p_pbstrName )
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

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetParameterValue( unsigned long triggerchannel, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pvarValue )
	{
		if ( S_OK == ::VariantClear( p_pvarValue ) )
		{
			if ( 0 < triggerchannel && triggerchannel <= 4 )
			{
				l_hrOk = S_OK;
			}
		}
	}

	return l_hrOk;
}

HRESULT WINAPI SVTriggerSetParameterValue( unsigned long triggerchannel, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( 0 < triggerchannel && triggerchannel <= 4 )
	{
		if ( nullptr != p_pvarValue )
		{
			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

// Digitizer Export Functions

HRESULT WINAPI SVDigitizerGetCount( unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulCount )
	{
		*p_pulCount = 0;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetHandle( unsigned long *pTriggerchannel, unsigned long p_ulIndex )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != pTriggerchannel )
	{
		*pTriggerchannel = 0;
	} 

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetName( unsigned long triggerchannel, BSTR *p_pbstrName )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pbstrName )
	{
		l_hrOk = S_OK;

		if ( nullptr != *p_pbstrName )
		{
			::SysFreeString( *p_pbstrName );

			*p_pbstrName = nullptr;
		}
	} 

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerLoadCameraFiles( unsigned long triggerchannel, SAFEARRAY* p_psaFileNames )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetBufferWidth( unsigned long triggerchannel, unsigned long *p_pulWidth )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulWidth )
	{
		*p_pulWidth = 0;
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetBufferHeight( unsigned long triggerchannel, unsigned long *p_pulHeight )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulHeight )
	{
		*p_pulHeight = 0;
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetBufferFormat( unsigned long triggerchannel, int *p_piFormat )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_piFormat )
	{
		*p_piFormat = SVImageFormatUnknown;
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerCreateBuffers( unsigned long triggerchannel, unsigned long p_ulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetLightReference( unsigned long triggerchannel, int p_iType, SAFEARRAY *p_psaData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerSetLightReference( unsigned long triggerchannel, int p_iType, SAFEARRAY p_saData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetLightReferenceBand( unsigned long triggerchannel, unsigned long p_ulBand, int p_iType, VARIANT *p_psaData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerSetLightReferenceBand( unsigned long triggerchannel, unsigned long p_ulBand, int p_iType, VARIANT p_saData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetLut( unsigned long triggerchannel, SAFEARRAY *p_psaData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerSetLut( unsigned long triggerchannel, SAFEARRAY p_saData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetLutBand( unsigned long triggerchannel, unsigned long p_ulBand, SAFEARRAY *p_psaData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerSetLutBand( unsigned long triggerchannel, unsigned long p_ulBand, SAFEARRAY p_saData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerRegisterBufferInterface( unsigned long triggerchannel, SVAcquisitionBufferInterface* p_pInterface )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerStart( unsigned long triggerchannel )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerStop( unsigned long triggerchannel )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerUnregisterBufferInterface( unsigned long triggerchannel )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerInternalTriggerEnable( unsigned long triggerchannel )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerInternalTrigger( unsigned long triggerchannel )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerInternalTriggerRegister( unsigned long triggerchannel, SvTh::TriggerDispatcher& )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return S_FALSE;
}

HRESULT WINAPI SVDigitizerInternalTriggerUnregister( unsigned long triggerchannel, SvTh::TriggerDispatcher& )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return S_FALSE;
}

HRESULT WINAPI SVDigitizerInternalTriggerUnregisterAll( unsigned long triggerchannel )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return S_FALSE;
}

HRESULT WINAPI SVDigitizerDestroyBuffers( unsigned long triggerchannel )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerUnloadCameraFile( unsigned long triggerchannel )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetRawParameter( unsigned long triggerchannel, unsigned long p_ulParameter, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pvarValue )
	{
		::VariantClear( p_pvarValue );
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerSetRawParameter( unsigned long triggerchannel, unsigned long p_ulParameter, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerSetParameters( unsigned long triggerchannel, const SVDeviceParamCollection* p_pParameters )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerSetParameter( unsigned long triggerchannel, const SVDeviceParamWrapper* p_pParameter )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetParameter( unsigned long triggerchannel, SVDeviceParamEnum p_eParameter, SVDeviceParamWrapper** p_ppParameter )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	return l_hrOk;
}// end SVDigitizerGetParameter

HRESULT WINAPI SVDigitizerDestroyParameter( unsigned long triggerchannel, SVDeviceParamWrapper* p_pParameter )
{
	HRESULT l_hrOk = S_OK;

	SVDeviceParamWrapper& rw = *p_pParameter;
	rw.Clear();

	return l_hrOk;
}

