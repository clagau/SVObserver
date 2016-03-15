// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCIExports
// * .File Name       : $Workfile:   SVCIExports.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 17:59:04  $
// ******************************************************************************

#include "StdAfx.h"
#include "SVCIExports.h"

#include "SVCISapera.h"

#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVImageLibrary/SVLut.h"

unsigned long HandleToIndex( unsigned long h )
{
	return h - 1;
}

bool IsValidImageHandle( unsigned long h )
{
	return ( 0 < h && h <= (unsigned long)g_caBuffers.GetSize() );
}

bool IsValidDigitizerHandle( unsigned long h )
{
	return ( 0 < h && h <= (unsigned long)g_caAcqs.size() );
}

// General Export Functions

HRESULT WINAPI SVCreate()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	if ( g_CICreate() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT WINAPI SVDestroy()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	if ( g_CIDestroy() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

// Trigger Export Functions

HRESULT WINAPI SVTriggerGetCount( unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pulCount != NULL )
	{
		*p_pulCount = g_caAcqs.size();

		l_hrOk = S_OK;
	}
	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetHandle( unsigned long *p_pulHandle, unsigned long p_ulIndex )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pulHandle != NULL )
	{
		if ( p_ulIndex < (unsigned long)g_caAcqs.size() )
		{
			l_hrOk = S_OK;

			*p_pulHandle = p_ulIndex + 1;
		}
		else
		{
			*p_pulHandle = NULL;
		}
	} 
	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetName( unsigned long p_ulHandle, BSTR *p_pbstrName )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pbstrName != NULL && 
	     0 < p_ulHandle && p_ulHandle <= (unsigned long)g_caAcqs.size() )
	{
		l_hrOk = S_OK;

		if ( *p_pbstrName != NULL )
		{
			::SysFreeString( *p_pbstrName );

			*p_pbstrName = NULL;
		}

		*p_pbstrName = g_caAcqs[p_ulHandle - 1].m_csFullName.AllocSysString();
	} 
	return l_hrOk;
}

HRESULT WINAPI SVTriggerRegister( unsigned long p_ulHandle, SVCICallbackPtr p_pCallback, void *p_pOwner, void *p_pData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pCallback != NULL && 
	     0 < p_ulHandle && p_ulHandle <= (unsigned long)g_caAcqs.size() )
	{
		SVCallbackStruct l_Callback;

		l_Callback.m_pCallback = p_pCallback;
		l_Callback.m_pOwner = p_pOwner;
		l_Callback.m_pData = p_pData;

		l_hrOk = g_CIAcqTriggerRegister( p_ulHandle - 1, l_Callback );
	} 
	return l_hrOk;
}

HRESULT WINAPI SVTriggerUnregister( unsigned long p_ulHandle, SVCICallbackPtr p_pCallback, void *p_pOwner, void *p_pData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	if ( p_pCallback != NULL && 
	     0 < p_ulHandle && p_ulHandle <= (unsigned long)g_caAcqs.size() )
	{
		SVCallbackStruct l_Callback;

		l_Callback.m_pCallback = p_pCallback;
		l_Callback.m_pOwner = p_pOwner;
		l_Callback.m_pData = p_pData;

		if ( g_CIAcqTriggerUnregister( p_ulHandle - 1, l_Callback ) != S_OK )
		{
			l_hrOk = S_FALSE;
		}
	} 
	return l_hrOk;
}

HRESULT WINAPI SVTriggerUnregisterAll( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_caAcqs.size() )
	{
		if ( g_CIAcqTriggerUnregisterAll( p_ulHandle - 1 ) != S_OK )
		{
			l_hrOk = S_FALSE;
		}
	} 
	return l_hrOk;
}

HRESULT WINAPI SVTriggerStart( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_caAcqs.size() )
	{
		l_hrOk = g_CIAcqStart( p_ulHandle - 1 );
	} 
	return l_hrOk;
}

HRESULT WINAPI SVTriggerStop( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_caAcqs.size() )
	{
		if ( g_CIAcqStop( p_ulHandle - 1 ) != S_OK )
		{
			l_hrOk = S_FALSE;
		}
	} 
	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetParameterCount( unsigned long p_ulHandle, unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	HRESULT l_hrOk = g_CIAcqTriggerGetParameterCount(p_ulHandle, p_pulCount);
	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetParameterName( unsigned long p_ulHandle, unsigned long p_ulIndex, BSTR *p_pbstrName )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	HRESULT l_hrOk = g_CIAcqTriggerGetParameterName(p_ulHandle, p_ulIndex, p_pbstrName);
	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	HRESULT l_hrOk = g_CIAcqTriggerGetParameterValue(p_ulHandle, p_ulIndex, p_pvarValue);
	return l_hrOk;
}

HRESULT WINAPI SVTriggerSetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	HRESULT l_hrOk = g_CIAcqTriggerSetParameterValue(p_ulHandle, p_ulIndex, p_pvarValue);
	return l_hrOk;
}

// Digitizer Export Functions

HRESULT WINAPI SVDigitizerGetCount( unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pulCount != NULL )
	{
		*p_pulCount = g_caAcqs.size();

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetHandle( unsigned long *p_pulHandle, unsigned long p_ulIndex )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pulHandle != NULL )
	{
		if ( p_ulIndex < (unsigned long)g_caAcqs.size() )
		{
			l_hrOk = S_OK;

			*p_pulHandle = p_ulIndex + 1;
		}
		else
		{
			*p_pulHandle = NULL;
		}
	} 

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetName( unsigned long p_ulHandle, BSTR *p_pbstrName )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pbstrName != NULL && 
	     0 < p_ulHandle && p_ulHandle <= (unsigned long)g_caAcqs.size() )
	{
		l_hrOk = S_OK;

		if ( *p_pbstrName != NULL )
		{
			::SysFreeString( *p_pbstrName );

			*p_pbstrName = NULL;
		}

		*p_pbstrName = g_caAcqs[p_ulHandle - 1].m_csFullName.AllocSysString();
	} 

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerLoadCameraFiles( unsigned long p_ulHandle, SAFEARRAY* p_psaFileNames )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_caAcqs.size() )
	{
		l_hrOk = g_CIAcqLoadCameraFiles( p_ulHandle - 1, p_psaFileNames );
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetBufferWidth( unsigned long p_ulHandle, unsigned long *p_pulWidth )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pulWidth != NULL )
	{
		*p_pulWidth = 0;

		if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_caAcqs.size() )
		{
			*p_pulWidth = g_caAcqs[ p_ulHandle - 1 ].m_ulWidth;

			l_hrOk = S_OK;
		}
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

		if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_caAcqs.size() )
		{
			*p_pulHeight = g_caAcqs[ p_ulHandle - 1 ].m_ulHeight;

			l_hrOk = S_OK;
		}
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

		if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_caAcqs.size() )
		{
			switch ( g_caAcqs[ p_ulHandle - 1 ].m_ulFormat )
			{
				case CORACQ_VAL_OUTPUT_FORMAT_MONO8:
				{
					*p_piFormat = SVImageFormatMono8;

					l_hrOk = S_OK;

					break;
				}
				case CORACQ_VAL_OUTPUT_FORMAT_RGB8888:
				{
					*p_piFormat = SVImageFormatRGB8888;

					l_hrOk = S_OK;

					break;
				}
				default:
				{
					break;
				}
			}
		}
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerCreateBuffers( unsigned long p_ulHandle, unsigned long p_ulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_caAcqs.size() )
	{
		l_hrOk = g_CIAcqBufferCreateAll( p_ulHandle - 1, p_ulCount );
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetLightReference( unsigned long p_ulHandle, int p_iType, SAFEARRAY *p_psaData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_caAcqs.size() )
	{
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerSetLightReference( unsigned long p_ulHandle, int p_iType, SAFEARRAY p_saData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_caAcqs.size() )
	{
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetLightReferenceBand( unsigned long p_ulHandle, unsigned long p_ulBand, int p_iType, VARIANT *p_psaData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_caAcqs.size() )
	{
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerSetLightReferenceBand( unsigned long p_ulHandle, unsigned long p_ulBand, int p_iType, VARIANT p_saData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_caAcqs.size() )
	{
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetLut( unsigned long p_ulHandle, SAFEARRAY *p_psaData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_caAcqs.size() )
	{
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerSetLut( unsigned long p_ulHandle, SAFEARRAY p_saData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_caAcqs.size() )
	{
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetLutBand( unsigned long p_ulHandle, unsigned long p_ulBand, SAFEARRAY *p_psaData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_caAcqs.size() )
	{
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerSetLutBand( unsigned long p_ulHandle, unsigned long p_ulBand, SAFEARRAY p_saData )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_caAcqs.size() )
	{
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerRegisterBufferInterface( unsigned long p_ulHandle, SVAcquisitionBufferInterface* p_pInterface )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	l_hrOk = g_CIAcqRegisterBufferInterface( p_ulHandle - 1, p_pInterface );

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerStart( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_caAcqs.size() )
	{
		l_hrOk = g_CIAcqStart( p_ulHandle - 1 );
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerStop( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_caAcqs.size() )
	{
		if ( g_CIAcqStop( p_ulHandle - 1 ) != S_OK )
		{
			l_hrOk = S_FALSE;
		}
	} 

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerUnregisterBufferInterface( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	l_hrOk = g_CIAcqUnregisterBufferInterface( p_ulHandle - 1 );

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerInternalTriggerEnable( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_caAcqs.size() )
	{
		l_hrOk =  g_CIAcqEnableSoftwareTrigger( p_ulHandle - 1);
	} 
	return l_hrOk;
}

HRESULT WINAPI SVDigitizerInternalTrigger( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( IsValidDigitizerHandle( p_ulHandle ) )
	{
		l_hrOk = g_CIAcqSoftwareTrigger( p_ulHandle - 1 );
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerInternalTriggerRegister( unsigned long p_ulHandle, SVCICallbackPtr p_pCallback, void *p_pOwner, void *p_pData )
{
	// SEJ - not needed as we don't do any data manipulation here
	//AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return SVTriggerRegister(p_ulHandle, p_pCallback, p_pOwner, p_pData);
}

HRESULT WINAPI SVDigitizerInternalTriggerUnregister( unsigned long p_ulHandle, SVCICallbackPtr p_pCallback, void *p_pOwner, void *p_pData )
{
	// SEJ - not needed as we don't do any data manipulation here
	//AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return SVTriggerUnregister(p_ulHandle, p_pCallback, p_pOwner, p_pData);
}

HRESULT WINAPI SVDigitizerInternalTriggerUnregisterAll( unsigned long p_ulHandle )
{
	// SEJ - not needed as we don't do any data manipulation here
	//AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return SVTriggerUnregisterAll(p_ulHandle);
}

HRESULT WINAPI SVDigitizerDestroyBuffers( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_caAcqs.size() )
	{
		if ( g_CIAcqBufferDestroyAll( p_ulHandle - 1 ) != S_OK )
		{
			l_hrOk = S_FALSE;
		}
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerUnloadCameraFile( unsigned long p_ulHandle )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_caAcqs.size() )
	{
		if ( g_CIAcqUnloadCameraFiles( p_ulHandle - 1 ) != S_OK )
		{
			l_hrOk = S_FALSE;
		}
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetRawParameter( unsigned long p_ulHandle, unsigned long p_ulParameter, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pvarValue != NULL )
	{
		::VariantClear( p_pvarValue );

		if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_caAcqs.size() )
		{
			switch ( p_ulParameter )
			{
			case SVDigitizerParameterBrightness:
				{
					p_pvarValue->vt = VT_I4;
					p_pvarValue->lVal = 0;

					l_hrOk = g_CIAcqGetLightReference( p_ulHandle - 1,
																						 SVLightReferenceTypeBrightness,
																						 p_pvarValue->lVal );

					break;
				}
			case SVDigitizerParameterBrightnessMax:
				{
					p_pvarValue->vt = VT_I4;

					p_pvarValue->lVal = 800;

					l_hrOk = S_OK;

					break;
				}
			case SVDigitizerParameterBrightnessMin:
				{
					p_pvarValue->vt = VT_I4;

					p_pvarValue->lVal = -6700;

					l_hrOk = S_OK;

					break;
				}
			case SVDigitizerParameterBrightnessStep:
				{
					p_pvarValue->vt = VT_I4;

					p_pvarValue->ulVal = 1;

					l_hrOk = S_OK;

					break;
				}
			case SVDigitizerParameterBrightnessRed:
				{
					p_pvarValue->vt = VT_I4;
					p_pvarValue->lVal = 0;

					l_hrOk = g_CIAcqGetLightReference( p_ulHandle - 1,
																						 SVLightReferenceTypeBrightnessRed,
																						 p_pvarValue->lVal );
					
					break;
				}
			case SVDigitizerParameterBrightnessRedMax:
				{
					p_pvarValue->vt = VT_I4;

					p_pvarValue->lVal = 2371;

					l_hrOk = S_OK;

					break;
				}
			case SVDigitizerParameterBrightnessRedMin:
				{
					p_pvarValue->vt = VT_I4;

					p_pvarValue->lVal = -2462;

					l_hrOk = S_OK;

					break;
				}
			case SVDigitizerParameterBrightnessRedStep:
				{
					p_pvarValue->vt = VT_I4;

					p_pvarValue->lVal = 1;

					l_hrOk = S_OK;

					break;
				}
			case SVDigitizerParameterBrightnessGreen:
				{
					p_pvarValue->vt = VT_I4;
					p_pvarValue->lVal = 0;

					l_hrOk = g_CIAcqGetLightReference( p_ulHandle - 1,
																						 SVLightReferenceTypeBrightnessGreen,
																						 p_pvarValue->lVal );

					break;
				}
			case SVDigitizerParameterBrightnessGreenMax:
				{
					p_pvarValue->vt = VT_I4;

					p_pvarValue->lVal = 2371;

					l_hrOk = S_OK;

					break;
				}
			case SVDigitizerParameterBrightnessGreenMin:
				{
					p_pvarValue->vt = VT_I4;

					p_pvarValue->lVal = -2462;

					l_hrOk = S_OK;

					break;
				}
			case SVDigitizerParameterBrightnessGreenStep:
				{
					p_pvarValue->vt = VT_I4;

					p_pvarValue->lVal = 1;

					l_hrOk = S_OK;

					break;
				}
			case SVDigitizerParameterBrightnessBlue:
				{
					p_pvarValue->vt = VT_I4;
					p_pvarValue->lVal = 0;

					l_hrOk = g_CIAcqGetLightReference( p_ulHandle - 1,
																						 SVLightReferenceTypeBrightnessBlue,
																						 p_pvarValue->lVal );

					break;
				}
			case SVDigitizerParameterBrightnessBlueMax:
				{
					p_pvarValue->vt = VT_I4;

					p_pvarValue->lVal = 2371;

					l_hrOk = S_OK;

					break;
				}
			case SVDigitizerParameterBrightnessBlueMin:
				{
					p_pvarValue->vt = VT_I4;

					p_pvarValue->lVal = -2462;

					l_hrOk = S_OK;

					break;
				}
			case SVDigitizerParameterBrightnessBlueStep:
				{
					p_pvarValue->vt = VT_I4;

					p_pvarValue->lVal = 1;

					l_hrOk = S_OK;

					break;
				}
			case SVDigitizerParameterContrast:
				{
					p_pvarValue->vt = VT_I4;
					p_pvarValue->lVal = 0;

					l_hrOk = g_CIAcqGetLightReference( p_ulHandle - 1,
																						 SVLightReferenceTypeContrast,
																						 p_pvarValue->lVal );

					break;
				}
			case SVDigitizerParameterContrastMax:
				{
					p_pvarValue->vt = VT_I4;

					p_pvarValue->lVal = 10900;

					l_hrOk = S_OK;

					break;
				}
			case SVDigitizerParameterContrastMin:
				{
					p_pvarValue->vt = VT_I4;

					p_pvarValue->lVal = 8400;

					l_hrOk = S_OK;

					break;
				}
			case SVDigitizerParameterContrastStep:
				{
					p_pvarValue->vt = VT_I4;

					p_pvarValue->lVal = 1;

					l_hrOk = S_OK;

					break;
				}
			case SVDigitizerParameterContrastRed:
				{
					p_pvarValue->vt = VT_I4;
					p_pvarValue->lVal = 0;

					l_hrOk = g_CIAcqGetLightReference( p_ulHandle - 1,
																						 SVLightReferenceTypeContrastRed,
																						 p_pvarValue->lVal );

					break;
				}
			case SVDigitizerParameterContrastRedMax:
				{
					p_pvarValue->vt = VT_I4;

					p_pvarValue->lVal = 15305;

					l_hrOk = S_OK;

					break;
				}
			case SVDigitizerParameterContrastRedMin:
				{
					p_pvarValue->vt = VT_I4;

					p_pvarValue->lVal = 8024;

					l_hrOk = S_OK;

					break;
				}
			case SVDigitizerParameterContrastRedStep:
				{
					p_pvarValue->vt = VT_I4;

					p_pvarValue->lVal = 1;

					l_hrOk = S_OK;

					break;
				}
			case SVDigitizerParameterContrastGreen:
				{
					p_pvarValue->vt = VT_I4;
					p_pvarValue->lVal = 0;

					l_hrOk = g_CIAcqGetLightReference( p_ulHandle - 1,
																						 SVLightReferenceTypeContrastGreen,
																						 p_pvarValue->lVal );

					break;
				}
			case SVDigitizerParameterContrastGreenMax:
				{
					p_pvarValue->vt = VT_I4;

					p_pvarValue->lVal = 15305;

					l_hrOk = S_OK;

					break;
				}
			case SVDigitizerParameterContrastGreenMin:
				{
					p_pvarValue->vt = VT_I4;

					p_pvarValue->lVal = 8024;

					l_hrOk = S_OK;

					break;
				}
			case SVDigitizerParameterContrastGreenStep:
				{
					p_pvarValue->vt = VT_I4;

					p_pvarValue->lVal = 1;

					l_hrOk = S_OK;

					break;
				}
			case SVDigitizerParameterContrastBlue:
				{
					p_pvarValue->vt = VT_I4;
					p_pvarValue->lVal = 0;

					l_hrOk = g_CIAcqGetLightReference( p_ulHandle - 1,
																						 SVLightReferenceTypeContrastBlue,
																						 p_pvarValue->lVal );

					break;
				}
			case SVDigitizerParameterContrastBlueMax:
				{
					p_pvarValue->vt = VT_I4;

					p_pvarValue->lVal = 15305;

					l_hrOk = S_OK;

					break;
				}
			case SVDigitizerParameterContrastBlueMin:
				{
					p_pvarValue->vt = VT_I4;

					p_pvarValue->lVal = 8024;

					l_hrOk = S_OK;

					break;
				}
			case SVDigitizerParameterContrastBlueStep:
				{
					p_pvarValue->vt = VT_I4;

					p_pvarValue->lVal = 1;

					l_hrOk = S_OK;

					break;
				}
			case SVDigitizerParameterLUT:
				{
					l_hrOk = g_CIAcqGetLut( p_ulHandle - 1, *p_pvarValue );

					break;
				}
				default:
				{
					break;
				}
			}
		}
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerSetRawParameter( unsigned long p_ulHandle, unsigned long p_ulParameter, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( p_pvarValue != NULL )
	{
		if ( 0 < p_ulHandle && p_ulHandle <= (unsigned long)g_caAcqs.size() )
		{
			switch ( p_ulParameter )
			{
			case SVDigitizerParameterBrightness:
				{
					l_hrOk = g_CIAcqSetLightReference( p_ulHandle - 1,
																						 SVLightReferenceTypeContrastRed,
																						 p_pvarValue->lVal );

					break;
				}
			case SVDigitizerParameterBrightnessRed:
				{
					l_hrOk = g_CIAcqSetLightReference( p_ulHandle - 1,
																						 SVLightReferenceTypeContrastRed,
																						 p_pvarValue->lVal );

					break;
				}
			case SVDigitizerParameterBrightnessBlue:
				{
					l_hrOk = g_CIAcqSetLightReference( p_ulHandle - 1,
																						 SVLightReferenceTypeContrastRed,
																						 p_pvarValue->lVal );

					break;
				}
			case SVDigitizerParameterBrightnessGreen:
				{
					l_hrOk = g_CIAcqSetLightReference( p_ulHandle - 1,
																						 SVLightReferenceTypeContrastRed,
																						 p_pvarValue->lVal );

					break;
				}
			case SVDigitizerParameterContrast:
				{
					l_hrOk = g_CIAcqSetLightReference( p_ulHandle - 1,
																						 SVLightReferenceTypeContrastRed,
																						 p_pvarValue->lVal );

					break;
				}
			case SVDigitizerParameterContrastRed:
				{
					l_hrOk = g_CIAcqSetLightReference( p_ulHandle - 1,
																						 SVLightReferenceTypeContrastRed,
																						 p_pvarValue->lVal );

					break;
				}
			case SVDigitizerParameterContrastBlue:
				{
					l_hrOk = g_CIAcqSetLightReference( p_ulHandle - 1,
																						 SVLightReferenceTypeContrastRed,
																						 p_pvarValue->lVal );

					break;
				}
			case SVDigitizerParameterContrastGreen:
				{
					l_hrOk = g_CIAcqSetLightReference( p_ulHandle - 1,
																						 SVLightReferenceTypeContrastRed,
																						 p_pvarValue->lVal );

					break;
				}
			case SVDigitizerParameterLUT:
				{
					l_hrOk = g_CIAcqSetLut( p_ulHandle - 1, *p_pvarValue );

					break;
				}
				default:
				{
					break;
				}
			}
		}
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerSetParameters( unsigned long p_ulHandle, const SVDeviceParamCollection* p_pParameters )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	if ( IsValidDigitizerHandle( p_ulHandle ) )
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

	HRESULT l_hrOk = S_OK;

	if ( IsValidDigitizerHandle( p_ulHandle ) )
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
						l_hrOk = g_CIAcqSetLightReference( HandleToIndex( p_ulHandle ), pLR->lr );
						break;
					}
					case DeviceParamLut:
					{
						const SVLutDeviceParam* pLut = rw.DerivedValue( pLut );
						l_hrOk = g_CIAcqSetLut( HandleToIndex( p_ulHandle ), pLut->lut );
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

	HRESULT l_hrOk = S_OK;

	if ( IsValidDigitizerHandle( p_ulHandle ) )
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
					l_hrOk = g_CIAcqGetLightReference( HandleToIndex( p_ulHandle ), pLR->lr );
					break;
				}
				case DeviceParamLut:
				{
					rw = SVLutDeviceParam();
					SVLutDeviceParam* pParam = rw.DerivedValue( pParam );
					l_hrOk = g_CIAcqGetLut( HandleToIndex( p_ulHandle ), pParam->lut );
					break;
				}
			}// end switch ( rw->Type() )

		}// end if ( p_pParameter != NULL )

	}// end if ( IsValidHandle( p_ulHandle ) )

	return l_hrOk;
}// end SVDigitizerGetParameter

HRESULT WINAPI SVDigitizerDestroyParameter( unsigned long p_ulHandle, SVDeviceParamWrapper* p_pParameter )
{
	HRESULT l_hrOk = S_OK;

	SVDeviceParamWrapper& rw = *p_pParameter;
	rw.Clear();

	return l_hrOk;
}

