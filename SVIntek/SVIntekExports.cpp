// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIntekExports
// * .File Name       : $Workfile:   SVIntekExports.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   26 Apr 2013 14:52:46  $
// ******************************************************************************

#include "StdAfx.h"
#include "SVIntekExports.h"
#include "SVIntek.h"
#include "SVImageLibrary/SVImageEnums.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVImageLibrary/SVLut.h"
#include "SVOMFCLibrary/SVCallbackStruct.h"

// General Export Functions

HRESULT WINAPI SVCreate()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	l_hrOk = g_svTheApp.m_svSystem.Create();

	return l_hrOk;
}

HRESULT WINAPI SVDestroy()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	l_hrOk = g_svTheApp.m_svSystem.Destroy();

	return l_hrOk;
}

// Trigger Export Functions

HRESULT WINAPI SVTriggerGetCount( unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	if ( p_pulCount != NULL )
	{
		l_hr = g_svTheApp.m_svSystem.TriggerGetCount( *p_pulCount );
	}
	return l_hr;
}

HRESULT WINAPI SVTriggerGetHandle( unsigned long *p_pulHandle, unsigned long p_ulIndex )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	if ( p_pulHandle != NULL )
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

	if ( p_pbstrName != NULL )
	{
		l_hr = S_OK;

		if ( *p_pbstrName != NULL )
		{
			::SysFreeString( *p_pbstrName );

			*p_pbstrName = NULL;
		}
		l_hr = g_svTheApp.m_svSystem.TriggerGetName(p_ulHandle, *p_pbstrName);
	} 
	return l_hr;
}

HRESULT WINAPI SVTriggerRegister( unsigned long p_ulHandle, SVCallbackPtr p_pCallback, void *p_pOwner, void *p_pData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hr = S_FALSE;

	if ( p_pCallback != NULL )
	{
		SVCallbackStruct l_Callback;

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

	if ( p_pCallback != NULL )
	{
		SVCallbackStruct l_Callback;

		l_Callback.m_pCallback = p_pCallback;
		l_Callback.m_pOwner = p_pOwner;
		l_Callback.m_pData = p_pData;

		if ( g_svTheApp.m_svSystem.TriggerUnregisterCallback(p_ulHandle, l_Callback) != S_OK )
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

	if ( g_svTheApp.m_svSystem.TriggerUnregisterAllCallbacks(p_ulHandle) != S_OK )
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

	if ( g_svTheApp.m_svSystem.TriggerStop(p_ulHandle) != S_OK )
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

	HRESULT l_hrOk = S_FALSE;

	if ( p_pulCount != NULL )
	{
		l_hrOk = g_svTheApp.m_svSystem.CameraGetCount( *p_pulCount );
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetHandle( unsigned long *p_pulHandle, unsigned long p_ulIndex )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pulHandle != NULL )
	{
		unsigned long l_ulCount = 0;

		l_hrOk = g_svTheApp.m_svSystem.CameraGetCount( l_ulCount );

		if ( l_hrOk == S_OK )
		{
			if ( p_ulIndex < l_ulCount )
			{
				l_hrOk = S_OK;

				*p_pulHandle = p_ulIndex + 1;
			}
			else
			{
				*p_pulHandle = NULL;
			}
		}
	} 

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetName( unsigned long p_ulHandle, BSTR *p_pbstrName )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pbstrName != NULL )
	{
		if ( *p_pbstrName != NULL )
		{
			::SysFreeString( *p_pbstrName );

			*p_pbstrName = NULL;
		}

		l_hrOk = g_svTheApp.m_svSystem.CameraGetName( p_ulHandle - 1, *p_pbstrName );
	} 

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerLoadCameraFiles( unsigned long p_ulHandle, SAFEARRAY* p_psaFileNames )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = g_svTheApp.m_svSystem.CameraLoadFiles(p_ulHandle - 1, p_psaFileNames);

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetBufferWidth( unsigned long p_ulHandle, unsigned long *p_pulWidth )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pulWidth != NULL )
	{
		*p_pulWidth = 0;

		l_hrOk = g_svTheApp.m_svSystem.CameraGetWidth( p_ulHandle - 1, *p_pulWidth );
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetBufferHeight( unsigned long p_ulHandle, unsigned long *p_pulHeight )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pulHeight != NULL )
	{
		*p_pulHeight = 0;

		l_hrOk = g_svTheApp.m_svSystem.CameraGetHeight( p_ulHandle - 1, *p_pulHeight );
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetBufferFormat( unsigned long p_ulHandle, int *p_piFormat )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_piFormat != NULL )
	{
		*p_piFormat = SVImageFormatUnknown;

		l_hrOk = g_svTheApp.m_svSystem.CameraGetFormat( p_ulHandle - 1, *p_piFormat );


	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerCreateBuffers( unsigned long p_ulHandle, unsigned long p_ulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	l_hrOk = g_svTheApp.m_svSystem.CameraBufferCreateAll( p_ulHandle - 1, p_ulCount );

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetLightReference( unsigned long p_ulHandle, int p_iType, SAFEARRAY *p_psaData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerSetLightReference( unsigned long p_ulHandle, int p_iType, SAFEARRAY p_saData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetLightReferenceBand( unsigned long p_ulHandle, unsigned long p_ulBand, int p_iType, VARIANT *p_psaData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerSetLightReferenceBand( unsigned long p_ulHandle, unsigned long p_ulBand, int p_iType, VARIANT p_saData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetLut( unsigned long p_ulHandle, SAFEARRAY *p_psaData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerSetLut( unsigned long p_ulHandle, SAFEARRAY p_saData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetLutBand( unsigned long p_ulHandle, unsigned long p_ulBand, SAFEARRAY *p_psaData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerSetLutBand( unsigned long p_ulHandle, unsigned long p_ulBand, SAFEARRAY p_saData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerRegisterBufferInterface( unsigned long p_ulHandle, SVAcquisitionBufferInterface* p_pInterface )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	l_hrOk = g_svTheApp.m_svSystem.CameraRegisterBufferInterface( p_ulHandle - 1, p_pInterface );

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerStart( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	l_hrOk = g_svTheApp.m_svSystem.CameraStart( p_ulHandle - 1 );

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerStop( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	l_hrOk = g_svTheApp.m_svSystem.CameraStop( p_ulHandle - 1 );

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerUnregisterBufferInterface( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	l_hrOk = g_svTheApp.m_svSystem.CameraUnregisterBufferInterface( p_ulHandle - 1 );

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerInternalTriggerEnable( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	HRESULT l_hrOk = g_svTheApp.m_svSystem.InternalTriggerEnable( p_ulHandle - 1 );
	return l_hrOk; 
}

HRESULT WINAPI SVDigitizerInternalTrigger( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = g_svTheApp.m_svSystem.InternalTrigger( p_ulHandle - 1 );
	
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerInternalTriggerRegister( unsigned long p_ulHandle, SVIntekCallbackPtr p_pCallback, void *p_pOwner, void *p_pData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	unsigned long l_ulCount = 0;
	HRESULT l_hrOk = g_svTheApp.m_svSystem.CameraGetCount( l_ulCount );
	
	if ( l_hrOk == S_OK && p_ulHandle <= l_ulCount )
	{
		SVCallbackStruct l_Callback;
		l_Callback.m_pCallback = p_pCallback;
		l_Callback.m_pOwner = p_pOwner;
		l_Callback.m_pData = p_pData;

		l_hrOk = g_svTheApp.m_svSystem.RegisterInternalTriggerCallback( p_ulHandle - 1, l_Callback );
	}
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerInternalTriggerUnregister( unsigned long p_ulHandle, SVIntekCallbackPtr p_pCallback, void *p_pOwner, void *p_pData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	unsigned long l_ulCount = 0;
	HRESULT l_hrOk = g_svTheApp.m_svSystem.CameraGetCount( l_ulCount );
	
	if ( l_hrOk == S_OK && p_ulHandle <= l_ulCount )
	{
		SVCallbackStruct l_Callback;
		l_Callback.m_pCallback = p_pCallback;
		l_Callback.m_pOwner = p_pOwner;
		l_Callback.m_pData = p_pData;

		l_hrOk = g_svTheApp.m_svSystem.UnregisterInternalTriggerCallback( p_ulHandle - 1, l_Callback );
	}
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerInternalTriggerUnregisterAll( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	unsigned long l_ulCount = 0;
	HRESULT l_hrOk = g_svTheApp.m_svSystem.CameraGetCount( l_ulCount );
	
	if ( l_hrOk == S_OK && p_ulHandle <= l_ulCount )
	{
		l_hrOk = g_svTheApp.m_svSystem.UnregisterAllInternalTriggerCallbacks(p_ulHandle - 1);
	}
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerDestroyBuffers( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	l_hrOk = g_svTheApp.m_svSystem.CameraBufferDestroyAll( p_ulHandle - 1 );

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerUnloadCameraFile( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerParameterGetList( unsigned long p_ulHandle, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	l_hrOk = g_svTheApp.m_svSystem.CameraGetParameterList( p_ulHandle - 1, p_pvarValue );

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerParameterGetName( unsigned long p_ulHandle, unsigned long p_ulParameter, BSTR *p_pbstrName )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pbstrName != NULL )
	{
		if ( *p_pbstrName != NULL )
		{
			::SysFreeString( *p_pbstrName );

			*p_pbstrName = NULL;
		}

		l_hrOk = g_svTheApp.m_svSystem.CameraGetParameterName( p_ulHandle - 1, p_ulParameter, *p_pbstrName );
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerParameterGetValue( unsigned long p_ulHandle, int p_iParameter, int *p_piParameterType, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	l_hrOk = g_svTheApp.m_svSystem.CameraGetParameter( p_ulHandle - 1, p_iParameter, p_piParameterType, p_pvarValue );

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerParameterSetValue( unsigned long p_ulHandle, int p_iParameter, int p_iParameterType, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	l_hrOk = g_svTheApp.m_svSystem.CameraSetParameter( p_ulHandle - 1, p_iParameter, p_iParameterType, p_pvarValue );

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerSetParameters( unsigned long p_ulHandle, const SVDeviceParamCollection* p_pParameters )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	unsigned long l_ulCount = 0;

	HRESULT l_hrOk = g_svTheApp.m_svSystem.CameraGetCount( l_ulCount );
	
	if ( l_hrOk == S_OK && p_ulHandle <= l_ulCount )
	{
		SVDeviceParamMap::const_iterator iter;
		for (iter = p_pParameters->mapParameters.begin(); iter != p_pParameters->mapParameters.end(); ++iter)
		{
			const SVDeviceParamWrapper& w = iter->second;
			if ( ((const SVDeviceParam*) w) != NULL )
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

	HRESULT l_hrOk = g_svTheApp.m_svSystem.CameraGetCount( l_ulCount );
	
	if ( l_hrOk == S_OK && p_ulHandle <= l_ulCount )
	{
		if ( p_pParameter != NULL )
		{
			const SVDeviceParamWrapper& rw = *p_pParameter;
			if ( ((const SVDeviceParam*) rw) != NULL )
			{
				switch ( rw->Type() )
				{
					case DeviceParamLightReference:
					{
						const SVLightReferenceDeviceParam* pLR = rw.DerivedValue( pLR );
						l_hrOk = S_FALSE; // g_CIAcqSetLightReference( HandleToIndex( p_ulHandle ), pLR->lr );
						break;
					}
					case DeviceParamLut:
					{
						const SVLutDeviceParam* pLut = rw.DerivedValue( pLut );
						l_hrOk = S_FALSE; // g_CIAcqSetLut( HandleToIndex( p_ulHandle ), pLut->lut );
						break;
					}
					default:
					{
						l_hrOk = S_FALSE;
						break;
					}
				}// end switch ( rw->Type() )
			}// end if ( ((const SVDeviceParam*) rw) != NULL )
		}// end if ( p_pParameter != NULL )
	}// end if ( IsValidHandle( p_ulHandle ) )

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetParameter( unsigned long p_ulHandle, SVDeviceParamEnum p_eParameter, SVDeviceParamWrapper** p_ppParameter )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	unsigned long l_ulCount = 0;

	HRESULT l_hrOk = g_svTheApp.m_svSystem.CameraGetCount( l_ulCount );
	
	if ( l_hrOk == S_OK && p_ulHandle <= l_ulCount )
	{
		if ( p_ppParameter != NULL )
		{
			*p_ppParameter = new SVDeviceParamWrapper;
			SVDeviceParamWrapper& rw = **p_ppParameter;
			switch ( p_eParameter )
			{
				case DeviceParamLightReference:
				{
					rw = SVLightReferenceDeviceParam();
					SVLightReferenceDeviceParam* pLR = rw.DerivedValue( pLR );
					l_hrOk = S_FALSE; // g_CIAcqGetLightReference( HandleToIndex( p_ulHandle ), pLR->lr );
					break;
				}
				case DeviceParamLut:
				{
					rw = SVLutDeviceParam();
					SVLutDeviceParam* pParam = rw.DerivedValue( pParam );
					l_hrOk = S_FALSE; // g_CIAcqGetLut( HandleToIndex( p_ulHandle ), pParam->lut );
					break;
				}
				default:
				{
					l_hrOk = S_FALSE;
					break;
				}
			}// end switch ( rw->Type() )

		}// end if ( p_pParameter != NULL )

	}// end if ( IsValidHandle( p_ulHandle ) )

	return l_hrOk;
}// end SVDigitizerGetParameter

HRESULT WINAPI SVDigitizerDestroyParameter( unsigned long p_ulHandle, SVDeviceParamWrapper* p_pParameter )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	SVDeviceParamWrapper& rw = *p_pParameter;
	rw.Clear();

	return l_hrOk;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVIntek\SVIntekExports.cpp_v  $
 * 
 *    Rev 1.0   26 Apr 2013 14:52:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   21 Jan 2013 09:50:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added methods for trigger interface
 * Revised to support Acquisition Triggering
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   04 Oct 2012 09:11:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Dec 2009 12:14:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   29 Jul 2009 16:10:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Sep 2008 10:04:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Interface for internal triggering
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1.1.0   14 Jul 2009 10:00:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to include the new buffer acquisition methodology and signal processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Nov 2006 17:39:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  582
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.15
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised SVDigitizerLoadCameraFiles()
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Nov 2005 13:32:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial code to implement Intek 1394 driver for SVObserver and test applications.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
