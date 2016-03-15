// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTestIntekDCamDriverProxy.cpp
// * .File Name       : $Workfile:   SVTestIntekDCamDriverProxy.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   19 Nov 2014 03:38:36  $
// ******************************************************************************

#include "stdafx.h"
#include <comdef.h>
#include "SVTestIntekDCamDriverProxy.h"
#include "SV1394CameraFileLibrary/SVDCamDriver.h"
#include "SVImageLibrary/SVDigitizerLoadLibraryClass.h"
#include "SVIntek/SVIntekEnums.h"
#include "SVTestAcquisitionClass.h"
#include "SVOMFCLibrary/SVLongValueDeviceParam.h"

SVTestIntekDCamDriverProxy::SVTestIntekDCamDriverProxy()
: SVDCamDriverProxy(), m_pAcquisition( nullptr )
{
}

SVTestIntekDCamDriverProxy::~SVTestIntekDCamDriverProxy()
{
}

void SVTestIntekDCamDriverProxy::SetOwner(SVTestAcquisitionClass* pAcquisition)
{
	m_pAcquisition = pAcquisition;
}

void SVTestIntekDCamDriverProxy::Init(SVDCamDriver* pCameraDriver)
{
	m_pCameraDriver = pCameraDriver;
}

HRESULT SVTestIntekDCamDriverProxy::InitializeDevice( const SVDeviceParamWrapper& rwParam, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer )
{
	HRESULT hr = S_OK;

	if( !CameraMatchesCameraFile( rDeviceParams, hDigitizer, pDigitizer ) )
	{
		hr = SVMSG_SVO_20_INCORRECT_CAMERA_FILE;
	}

	if ( hr == S_OK )
	{
//		UpdateLightReferenceAttributes( rwParam );

		if ( CameraMatchesCameraFile( rDeviceParams, hDigitizer, pDigitizer ) )
		{
			if ( rwParam.IsValid() )
			{
				if ( rwParam->Type() == DeviceParamCameraFormats )
				{
					hr = m_pCameraDriver->SetParameter( rwParam );
					// special case for CameraFormat... it resets the dig, so we need to send all of our params again
					InitializeDevice( rDeviceParams, hDigitizer, pDigitizer );
				}
				else
				{
					hr = m_pCameraDriver->SetParameter( rwParam );
				}
			}
		}
	}

	return hr;
}

HRESULT SVTestIntekDCamDriverProxy::InitializeDevice( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer )
{
	HRESULT hr = S_OK;

	if( !CameraMatchesCameraFile( rDeviceParams, hDigitizer, pDigitizer ) )
	{
		hr = SVMSG_SVO_20_INCORRECT_CAMERA_FILE;
	}

//	UpdateLightReferenceAttributes( rDeviceParams );

	if ( hr == S_OK )
	{
		if ( CameraMatchesCameraFile( rDeviceParams, hDigitizer, pDigitizer ) )
		{
			if (m_pCameraDriver)
			{
				// Set Software Async Trigger register address and value
				ULONG ulRegister;
				ULONG ulValue;
				if (m_pCameraDriver->GetSoftwareTriggerRegisterAndValue(ulRegister, ulValue) == S_OK)
				{
					_variant_t l_ValueUseAndClear;

					// set them in the SVIntek camera info
					l_ValueUseAndClear = ulRegister;
					pDigitizer->ParameterSetValue( hDigitizer, SVIntekSoftwareTriggerRegisterAddress, 0, &l_ValueUseAndClear );
					l_ValueUseAndClear.Clear();
					
					l_ValueUseAndClear = ulValue;
					pDigitizer->ParameterSetValue( hDigitizer, SVIntekSoftwareTriggerRegisterValue, 0, &l_ValueUseAndClear );
					l_ValueUseAndClear.Clear();
				}
			}

			// special case for CameraFormat... it resets the dig, so send it first
			if ( rDeviceParams.ParameterExists( DeviceParamCameraFormats ) )
			{
				const SVCameraFormatsDeviceParam* pParam = rDeviceParams.Parameter( DeviceParamCameraFormats ).DerivedValue( pParam );
				hr = m_pCameraDriver->SetParameter( *pParam );

				hr = m_pCameraDriver->SetParameters( rDeviceParams );	// do all not just what are in rDeviceParams
			}
			else
			{
				if ( m_pCameraDriver )
				{
					hr = m_pCameraDriver->SetParameters( rDeviceParams );
				}
				else
				{
					hr = S_OK;
				}
			}
		}
	}

	return hr;
}

HRESULT SVTestIntekDCamDriverProxy::WriteCameraRegister( unsigned long ulAddress, unsigned long ulValue, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer )
{
	HRESULT l_hrOk = S_FALSE;

	if ( pDigitizer != NULL)
	{
		_variant_t l_ValueUseAndClear;

		l_ValueUseAndClear = (long)( ulAddress | 0xF0F00000 );
		l_hrOk = pDigitizer->ParameterSetValue( hDigitizer, SVIntekParameterDCamRegisterAddress, 0, &l_ValueUseAndClear );
		l_ValueUseAndClear.Clear();

		l_ValueUseAndClear = (long)ulValue;
		l_hrOk = pDigitizer->ParameterSetValue( hDigitizer, SVIntekParameterDCamRegisterDataQuad, 0, &l_ValueUseAndClear );
		l_ValueUseAndClear.Clear();
	}

	return l_hrOk;
}

HRESULT SVTestIntekDCamDriverProxy::ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer )
{
	HRESULT l_hrOk = S_FALSE;

	if ( pDigitizer != NULL )
	{
		_variant_t l_oValue;

		int l_iType = 0;

		l_oValue = (long)( ulAddress | 0xF0F00000 );

		l_hrOk = pDigitizer->ParameterSetValue( hDigitizer, SVIntekParameterDCamRegisterAddress, 0, &l_oValue );

		l_hrOk = pDigitizer->ParameterGetValue( hDigitizer, SVIntekParameterDCamRegisterDataQuad, &l_iType, &l_oValue );

		rulValue = (long)l_oValue;
	}

	return l_hrOk;
}

HRESULT SVTestIntekDCamDriverProxy::WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer )
{
	HRESULT l_hrOk = S_FALSE;

	if ( pDigitizer != NULL )
	{
		_variant_t l_oValue;
		_variant_t l_oArray;

		long l_lCount = ulCount * sizeof(unsigned long);
		
		SAFEARRAYBOUND l_psabData[1];

		l_psabData[0].lLbound = 0;
		l_psabData[0].cElements = l_lCount;

		l_oArray.parray = ::SafeArrayCreate( VT_UI1, 1, l_psabData );

		if( l_oArray.parray != NULL )
		{
			l_oArray.vt = VT_ARRAY | VT_UI1;

			l_oValue = (long)( ulAddress | 0xF0F00000 );

			l_hrOk = pDigitizer->ParameterSetValue( hDigitizer, SVIntekParameterDCamRegisterAddress, 0, &l_oValue );

			unsigned char *l_pucItem = (unsigned char *)( paulValue );

			for( long l = 0; l_hrOk == S_OK && l < l_lCount; l++ )
			{
				long l_plIndex[1];

				l_plIndex[0] = l;

				l_hrOk = ::SafeArrayPutElement( l_oArray.parray, l_plIndex, (void *)(l_pucItem + l) );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = pDigitizer->ParameterSetValue( hDigitizer, SVIntekParameterDCamRegisterDataBlock, 0, &l_oArray );
			}
		}
	}

	return l_hrOk;
}

HRESULT SVTestIntekDCamDriverProxy::ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer )
{
	HRESULT l_hrOk = S_FALSE;

	if ( pDigitizer != NULL )
	{
		_variant_t l_oValue;
		_variant_t l_oArray;
		
		SAFEARRAYBOUND l_psabData[1];

		long l_lCount = ulCount * sizeof(unsigned long);
		
		l_psabData[0].lLbound = 0;
		l_psabData[0].cElements = l_lCount;

		l_oArray.parray = ::SafeArrayCreate( VT_UI1, 1, l_psabData );

		if( l_oArray.parray != NULL )
		{
			l_oArray.vt = VT_ARRAY | VT_UI1;

			int l_iType = 0; // change type to DCam Block

			l_oValue = (long)( ulAddress | 0xF0F00000 );

			l_hrOk = pDigitizer->ParameterSetValue( hDigitizer, SVIntekParameterDCamRegisterAddress, 0, &l_oValue );

			l_hrOk = pDigitizer->ParameterGetValue( hDigitizer, SVIntekParameterDCamRegisterDataBlock, &l_iType, &l_oArray );

			unsigned char *l_pucItem = (unsigned char *)( paulValue );

			for( long l = 0; l_hrOk == S_OK && l < l_lCount; l++ )
			{
				long l_plIndex[1];

				l_plIndex[0] = l;

				l_hrOk = ::SafeArrayGetElement( l_oArray.parray, l_plIndex, l_pucItem + l );
			}
		}
	}

	return l_hrOk;
}

HRESULT SVTestIntekDCamDriverProxy::SetStandardCameraParameter( const SVDeviceParamWrapper& rw, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer )
{
	HRESULT l_hrOk = S_OK;

	if ( pDigitizer != NULL )
	{
		if ( hDigitizer != NULL )
		{
			int l_iParameterID = 0;

			SVDeviceParamEnum l_eType = rw->Type();

			switch ( l_eType )
			{
				case DeviceParamShutter:
				{
					const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
					if ( pParam )
					{
						_variant_t l_oValue = pParam->lValue;

						l_hrOk = pDigitizer->ParameterSetValue( hDigitizer, SVIntekParameterDCamFeatureShutter, 0, &l_oValue );
					}

					break;
				}
				case DeviceParamGamma:
				{
					const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
					if ( pParam )
					{
						_variant_t l_oValue = pParam->lValue;

						l_hrOk = pDigitizer->ParameterSetValue( hDigitizer, SVIntekParameterDCamFeatureGamma, 0, &l_oValue );
					}

					break;
				}
				case DeviceParamBrightness:
				{
					const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
					if ( pParam )
					{
						_variant_t l_oValue = pParam->lValue;

						l_hrOk = pDigitizer->ParameterSetValue( hDigitizer, SVIntekParameterDCamFeatureBrightness, 0, &l_oValue );
					}

					break;
				}
				case DeviceParamGain:
				{
					const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
					if ( pParam )
					{
						_variant_t l_oValue = pParam->lValue;

						l_hrOk = pDigitizer->ParameterSetValue( hDigitizer, SVIntekParameterDCamFeatureGain, 0, &l_oValue );
					}

					break;
				}
				case DeviceParamWhiteBalanceUB:
				{
					const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
					if ( pParam )
					{
						_variant_t l_oValue = pParam->lValue;

						l_hrOk = pDigitizer->ParameterSetValue( hDigitizer, SVIntekParameterDCamFeatureWhiteBalanceU, 0, &l_oValue );
					}

					break;
				}
				case DeviceParamWhiteBalanceVR:
				{
					const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
					if ( pParam )
					{
						_variant_t l_oValue = pParam->lValue;

						l_hrOk = pDigitizer->ParameterSetValue( hDigitizer, SVIntekParameterDCamFeatureWhiteBalanceV, 0, &l_oValue );
					}

					break;
				}
				case DeviceParamSharpness:
				{
					const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
					if ( pParam )
					{
						_variant_t l_oValue = pParam->lValue;

						l_hrOk = pDigitizer->ParameterSetValue( hDigitizer, SVIntekParameterDCamFeatureSharpness, 0, &l_oValue );
					}

					break;
				}
				case DeviceParamHue:
				{
					const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
					if ( pParam )
					{
						_variant_t l_oValue = pParam->lValue;

						l_hrOk = pDigitizer->ParameterSetValue( hDigitizer, SVIntekParameterDCamFeatureHue, 0, &l_oValue );
					}

					break;
				}
				case DeviceParamSaturation:
				{
					const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
					if ( pParam )
					{
						_variant_t l_oValue = pParam->lValue;

						l_hrOk = pDigitizer->ParameterSetValue( hDigitizer, SVIntekParameterDCamFeatureSaturation, 0, &l_oValue );
					}

					break;
				}
				case DeviceParamCameraFormats:
				{
					if ( !IsOnline() )
					{
						const SVCameraFormatsDeviceParam* pParam = rw.DerivedValue( pParam );

						const SVCameraFormat& rcf = pParam->options.find(pParam->strValue)->second;

						_variant_t l_ValueUseAndClear = rcf.m_lDCamFormat;
						l_hrOk = pDigitizer->ParameterSetValue( hDigitizer, SVIntekParameterDCamFormat, 0, &l_ValueUseAndClear );
						l_ValueUseAndClear.Clear();

						l_ValueUseAndClear = rcf.m_lDCamMode;
						l_hrOk = pDigitizer->ParameterSetValue( hDigitizer, SVIntekParameterDCamMode, 0, &l_ValueUseAndClear );
						l_ValueUseAndClear.Clear();

						if( rcf.m_lDCamFormat == 7 )
						{
							l_ValueUseAndClear = rcf.m_lDCamColorFormat;
							l_hrOk = pDigitizer->ParameterSetValue( hDigitizer,SVIntekParameterDCamFormat7ColorFormat, 0, &l_ValueUseAndClear );
							l_ValueUseAndClear.Clear();

							l_ValueUseAndClear = rcf.lHPos;
							l_hrOk = pDigitizer->ParameterSetValue( hDigitizer, SVIntekParameterDCamFormat7XOffset, 0, &l_ValueUseAndClear );
							l_ValueUseAndClear.Clear();

							l_ValueUseAndClear = rcf.lVPos;
							l_hrOk = pDigitizer->ParameterSetValue( hDigitizer, SVIntekParameterDCamFormat7YOffset, 0, &l_ValueUseAndClear );
							l_ValueUseAndClear.Clear();

							l_ValueUseAndClear = rcf.lWidth;
							l_hrOk = pDigitizer->ParameterSetValue( hDigitizer, SVIntekParameterDCamFormat7XSize, 0, &l_ValueUseAndClear );
							l_ValueUseAndClear.Clear();

							l_ValueUseAndClear = rcf.lHeight;
							l_hrOk = pDigitizer->ParameterSetValue( hDigitizer, SVIntekParameterDCamFormat7YSize, 0, &l_ValueUseAndClear );
							l_ValueUseAndClear.Clear();
						}
						else
						{
							l_ValueUseAndClear = rcf.m_lDCamFrameRate;
							l_hrOk = pDigitizer->ParameterSetValue( hDigitizer, SVIntekParameterDCamRate, 0, &l_ValueUseAndClear );
							l_ValueUseAndClear.Clear();
						}
					}
					else
					{
						l_hrOk = SV_FALSE;
					}

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

	if( l_hrOk == S_OK )
	{
		rDeviceParams.SetParameter( rw );
	}
	return l_hrOk;
}

HRESULT SVTestIntekDCamDriverProxy::SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer )
{
	HRESULT hr = S_OK;

	// SEJ - send notification to start tracking main camera parameters
	if ( pDigitizer != NULL )
	{
		_variant_t dummy;
		hr = pDigitizer->ParameterSetValue(hDigitizer, SVIntekBeginTrackParameters, 0, &dummy);
	}
	
	if ( hr == S_OK )
	{
		hr = SVDCamDriverProxy::SetDeviceParameters( rDeviceParams, m_pAcquisition->m_DeviceParams );
	}

	if ( hr == S_OK )
	{
		hr = InitializeDevice( rDeviceParams, hDigitizer, pDigitizer );
	}
	return hr;
}

HRESULT SVTestIntekDCamDriverProxy::IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer)
{
	HRESULT l_hrOk = S_OK;

	if( hDigitizer != NULL && pDigitizer != NULL &&
		rDeviceParams.ParameterExists( DeviceParamVendorId ) )
	{
		_variant_t l_oValue;

		if( pDigitizer != NULL && pDigitizer->ParameterGetValue( hDigitizer, SVIntekParameterDCamVendorId, 0, &l_oValue ) == S_OK )
		{
			SVString l_csVenderId = SvUl_SF::createSVString( l_oValue.bstrVal );

			if( l_csVenderId == StringValue( rDeviceParams.Parameter( DeviceParamVendorId ) ) )
			{
				if( rDeviceParams.ParameterExists( DeviceParamModelName ) &&
					pDigitizer->ParameterGetValue( hDigitizer, SVIntekParameterDCamModelName, 0, &l_oValue ) == S_OK )
				{
					CString sHardwareModel = l_oValue.bstrVal;

					CString sModel = StringValue(rDeviceParams.Parameter( DeviceParamModelName )).c_str();

					if ( sHardwareModel != _T("") && sModel != _T("") && sHardwareModel.CompareNoCase(sModel) != 0 )
					{
						l_hrOk = S_FALSE;
					}
				}
			}
			else
			{
				l_hrOk = S_FALSE;
			}
		}
	}

	return l_hrOk;
}

bool SVTestIntekDCamDriverProxy::CameraMatchesCameraFile(const SVDeviceParamCollection& rCameraFileDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer)
{
	bool l_bOk = true;
	
	if( hDigitizer != NULL && pDigitizer != NULL &&
		rCameraFileDeviceParams.ParameterExists( DeviceParamVendorId ) )
	{
		_variant_t l_oValue;

		if( pDigitizer->ParameterGetValue( hDigitizer, SVIntekParameterDCamVendorId, 0, &l_oValue ) == S_OK )
		{
			SVString l_csVenderId = SvUl_SF::createSVString(l_oValue.bstrVal );

			l_bOk = l_csVenderId == StringValue( rCameraFileDeviceParams.Parameter( DeviceParamVendorId ) );

			if (l_bOk)
			{
				if( rCameraFileDeviceParams.ParameterExists( DeviceParamModelName ) &&
					pDigitizer->ParameterGetValue( hDigitizer, SVIntekParameterDCamModelName, 0, &l_oValue ) == S_OK )
				{
					CString sHardwareModel( l_oValue.bstrVal, ::SysStringLen( l_oValue.bstrVal ) );

					CString sModel = StringValue(rCameraFileDeviceParams.Parameter( DeviceParamModelName )).c_str();

					if ( sHardwareModel.CompareNoCase(sModel) != 0 )
					{
						l_bOk = false;
					}
				}
			}
		}
	}

	return l_bOk;
}

bool SVTestIntekDCamDriverProxy::IsOnline()
{
	bool bRetVal = false;

	if (m_pAcquisition)
		bRetVal = m_pAcquisition->IsOnline();

	return bRetVal;
}

