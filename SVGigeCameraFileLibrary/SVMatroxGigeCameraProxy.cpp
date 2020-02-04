//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeCameraProxy.cpp
//* .File Name       : $Workfile:   SVMatroxGigeCameraProxy.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   03 Dec 2014 19:51:22  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#pragma warning (push)
#pragma warning (disable : 4996)
//Moved to precompiled header: #include <deque>
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/algorithm/string.hpp>
#include "SVMatroxGigeCameraProxy.h"
#include "CameraLibrary/SVStringValueDeviceParam.h"
#include "CameraLibrary/SVLongValueDeviceParam.h"
#include "CameraLibrary/SVBoolValueDeviceParam.h"
#include "CameraLibrary/SVCustomDeviceParam.h"
#include "Definitions/StringTypeDef.h"
#include "Definitions/SVGigeEnums.h"
#include "InspectionEngine/SVMatroxGigeAcquisitionClass.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "TriggerHandling/SVDigitizerLoadLibraryClass.h"
#pragma warning (pop)
#pragma region Includes

typedef std::map<SVDeviceParamEnum, SvDef::SVGigeParameterEnum> DeviceParamToGigeParamAssoc;
static DeviceParamToGigeParamAssoc cDeviceParamEnumToGigeParamEnum
{
	// Device parameter enum			Gige parameter enum
	{DeviceParamShutter,				SvDef::SVGigeParameterFeatureShutter},
	{DeviceParamGamma,					SvDef::SVGigeParameterFeatureGamma},
	{DeviceParamBrightness,				SvDef::SVGigeParameterFeatureBrightness},
	{DeviceParamGain,					SvDef::SVGigeParameterFeatureGain},
	{DeviceParamWhiteBalanceUB,			SvDef::SVGigeParameterFeatureWhiteBalanceU},
	{DeviceParamWhiteBalanceVR,			SvDef::SVGigeParameterFeatureWhiteBalanceV},
	{DeviceParamSharpness,				SvDef::SVGigeParameterFeatureSharpness},
	{DeviceParamHue,					SvDef::SVGigeParameterFeatureHue},
	{DeviceParamSaturation,				SvDef::SVGigeParameterFeatureSaturation},
	{DeviceParamGigeTriggerSource,		SvDef::SVGigeParameterTriggerSource},
	{DeviceParamGigeTriggerEdge,		SvDef::SVGigeParameterTriggerEdge},
	{DeviceParamGigeTriggerEnable,		SvDef::SVGigeParameterTriggerEnable},
	{DeviceParamGigeTriggerLine,		SvDef::SVGigeParameterTriggerLine},
	{DeviceParamGigeStrobeSource,		SvDef::SVGigeParameterStrobeSource},
	{DeviceParamGigeStrobeEdge,			SvDef::SVGigeParameterStrobeEdge},
	{DeviceParamGigeStrobeEnable,		SvDef::SVGigeParameterStrobeEnable},
	{DeviceParamStrobePulseDelay,		SvDef::SVGigeParameterStrobeDelay},
	{DeviceParamStrobePulseDuration,	SvDef::SVGigeParameterStrobeDuration},
	{DeviceParamGigeStrobeLine,			SvDef::SVGigeParameterStrobeLine},
	{DeviceParamGigePacketSize,			SvDef::SVGigeParameterPacketSize},
	{DeviceParamGigeCustom1,			SvDef::SVGigeParameterCustom1},
	{DeviceParamGigeCustom2,			SvDef::SVGigeParameterCustom2},
	{DeviceParamGigeCustom3,			SvDef::SVGigeParameterCustom3},
	{DeviceParamGigeCustom4,			SvDef::SVGigeParameterCustom4},
	{DeviceParamGigeCustom5,			SvDef::SVGigeParameterCustom5},
	{DeviceParamGigeCustom6,			SvDef::SVGigeParameterCustom6},
	{DeviceParamGigeCustom7,			SvDef::SVGigeParameterCustom7},
	{DeviceParamGigeCustom8,			SvDef::SVGigeParameterCustom8},
	{DeviceParamGigeCustom9,			SvDef::SVGigeParameterCustom9},
	{DeviceParamGigeCustom10,			SvDef::SVGigeParameterCustom10},
	{DeviceParamGigeCustom11,			SvDef::SVGigeParameterCustom11},
	{DeviceParamGigeCustom12,			SvDef::SVGigeParameterCustom12},
	{DeviceParamGigeCustom13,			SvDef::SVGigeParameterCustom13},
	{DeviceParamGigeCustom14,			SvDef::SVGigeParameterCustom14},
	{DeviceParamGigeCustom15,			SvDef::SVGigeParameterCustom15},
	{DeviceParamGigeCustom16,			SvDef::SVGigeParameterCustom16},
	{DeviceParamGigeCustom17,			SvDef::SVGigeParameterCustom17},
	{DeviceParamGigeCustom18,			SvDef::SVGigeParameterCustom18},
	{DeviceParamGigeCustom19,			SvDef::SVGigeParameterCustom19},
	{DeviceParamGigeCustom20,			SvDef::SVGigeParameterCustom20},
	{DeviceParamGigeCustom21,			SvDef::SVGigeParameterCustom21},
	{DeviceParamGigeCustom22,			SvDef::SVGigeParameterCustom22},
	{DeviceParamGigeCustom23,			SvDef::SVGigeParameterCustom23},
	{DeviceParamGigeCustom24,			SvDef::SVGigeParameterCustom24},
	{DeviceParamGigeCustom25,			SvDef::SVGigeParameterCustom25},
	{DeviceParamGigeCustom26,			SvDef::SVGigeParameterCustom26},
	{DeviceParamGigeCustom27,			SvDef::SVGigeParameterCustom27},
	{DeviceParamGigeCustom28,			SvDef::SVGigeParameterCustom28},
	{DeviceParamGigeCustom29,			SvDef::SVGigeParameterCustom29},
	{DeviceParamGigeCustom30,			SvDef::SVGigeParameterCustom30},
	{DeviceParamHorizontalBinning,		SvDef::SVGigeParameterHorizontalBinning},
	{DeviceParamVerticalBinning,		SvDef::SVGigeParameterVerticalBinning},
	{DeviceParamCameraDefaultSettings,	SvDef::SVGigeParameterCameraDefaultSettings},
};

SVMatroxGigeCameraProxy::SVMatroxGigeCameraProxy()
: m_pAcquisition(nullptr)
{
}

SVMatroxGigeCameraProxy::~SVMatroxGigeCameraProxy()
{
}

void SVMatroxGigeCameraProxy::SetOwner(SvIe::SVMatroxGigeAcquisitionClass* pAcquisition)
{
	m_pAcquisition = pAcquisition;
}

HRESULT SVMatroxGigeCameraProxy::InitializeDevice( const SVDeviceParamWrapper& rwParam, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer )
{
	HRESULT hr = S_OK;

	if( !CameraMatchesCameraFile( rDeviceParams, hDigitizer, pDigitizer ) )
	{
		hr = SVMSG_SVO_20_INCORRECT_CAMERA_FILE;
	}

	if ( S_OK == hr )
	{
		if ( CameraMatchesCameraFile( rDeviceParams, hDigitizer, pDigitizer ) )
		{
			if ( rwParam.IsValid() )
			{
				hr = SetDigitizerParameter( rwParam, hDigitizer, pDigitizer );
			}
		}
	}
	return hr;
}

HRESULT SVMatroxGigeCameraProxy::InitializeDevice( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer )
{
	HRESULT hr = S_OK;

	if( !CameraMatchesCameraFile( rDeviceParams, hDigitizer, pDigitizer ) )
	{
		hr = SVMSG_SVO_20_INCORRECT_CAMERA_FILE;
	}

	if ( S_OK == hr )
	{
		if ( CameraMatchesCameraFile( rDeviceParams, hDigitizer, pDigitizer ) )
		{
			hr = SetDigitizerParameters( rDeviceParams, hDigitizer, pDigitizer );
		}
	}
	return hr;
}

HRESULT SVMatroxGigeCameraProxy::SetGigeFeatureOverrides(const std::string& xmlData, unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer)
{
	HRESULT hr = S_OK;

	if (nullptr != pDigitizer && 0 != hDigitizer)
	{
		_variant_t l_oValue = xmlData.c_str(); // does this convert from ansi to wide ?
		hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterFeatureOverrides, 0, &l_oValue);
	}
	return hr;
}

// this is where the SV Camera Device Parameters are mapped to the Gige Parameter defines (mostly)
// exceptions to this are the Internal ones for things like Trigger and Strobe...
// All parameters must pass through here to translate DeviceParam to GigeParamter tags
/* Tags - DeviceParam to GigeFeatureParam
DeviceParamVendorName				- SVGigeParameterVendorName
DeviceParamModelName				- SVGigeParameterModelName
DeviceParamFirmware					-
DeviceParamSerialNumberString		- SVGigeParameterSerialNumber
DeviceParamShutter					- SVGigeParameterFeatureShutter
DeviceParamGamma					- SVGigeParameterFeatureGamma
DeviceParamBrightness				- SVGigeParameterFeatureBrightness
DeviceParamGain						- SVGigeParameterFeatureGain
DeviceParamWhiteBalanceUB			- SVGigeParameterFeatureWhiteBalanceU (needs implemented on DLL side)
DeviceParamWhiteBalanceVR			- SVGigeParameterFeatureWhiteBalanceV (needs implemented on DLL side)
DeviceParamSharpness				- SVGigeParameterFeatureSharpness (needs implemented on DLL side)
DeviceParamHue						- SVGigeParameterFeatureHue (needs implemented on DLL side)
DeviceParamSaturation				- SVGigeParameterFeatureSaturation (needs implemented on DLL side)
DeviceParamCameraFormats			- SVGigeParameterColorFormat (needs implemented)
									- SVGigeParameterXSize
									- SVGigeParameterYSize
									- SVGigeParameterXOffset
									- SVGigeParameterYOffset

DeviceParamGigeTriggerSource		- SVGigeParameterTriggerSource
DeviceParamGigeTriggerEdge			- SVGigeParameterTriggerEdge
DeviceParamGigeTriggerEnable		- SVGigeParameterTriggerEnable
DeviceParamShutterDelay				- SVGigeParameterTriggerDelay (needs implemented)
DeviceParamIOTriggerInvert			- SVGigeParameterTriggerInvert (needs implemented)
DeviceParamGigeTriggerLine			- SVGigeParameterTriggerLine (needs implemented)

DeviceParamGigeStrobeSource			- SVGigeParameterStrobeSource (needs implemented)
DeviceParamGigeStrobeEdge			- SVGigeParameterStrobeEdge (needs implemented)
DeviceParamGigeStrobeEnable			- SVGigeParameterStrobeEnable (needs implemented)
DeviceParamIOStrobeInvert			- SVGigeParameterStrobeInvert (needs implemented)
DeviceParamStrobePulseDelay			- SVGigeParameterStrobeDelay (needs implemented)
DeviceParamStrobePulseDuration		- SVGigeParameterStrobeDuration (needs implemented)
DeviceParamCameraStrobeLine			- SVGigeParameterStrobeLine (needs implemented)

DeviceParamCameraTriggerMode		- SVGigeParameterExposureMode (needs implemented)
DeviceParamTestPattern				- SVGigeParameterBayerPattern (needs implemented)
DeviceParamCameraInput				- SVGigeParameterInputLine

DeviceParamExposure					- SVGigeParameterFeatureAutoExposure (needs implemented on DLL side)

DeviceParamHorizontalBinning		- SVGigeParameterHorizontalBinning
DeviceParamVerticalBinning			- SVGigeParameterVerticalBinning
DeviceParamGigePacketSize			- SVGigeParameterPacketSize
*/

HRESULT SVMatroxGigeCameraProxy::SetStandardCameraParameter( const SVDeviceParamWrapper& rw, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer )
{
	HRESULT hr = S_OK;

	if ( nullptr != pDigitizer && 0 != hDigitizer )
	{
		_variant_t l_oValue;

		int l_iParameterID = 0;

		SVDeviceParamEnum l_eType = rw->Type();

		switch ( l_eType )
		{
			#pragma region DeviceParamVendorName
			case DeviceParamVendorName:
			{
			}
			break;
			#pragma endregion

			#pragma region DeviceParamModelName
			case DeviceParamModelName:
			{
			}
			break;
			#pragma endregion

			#pragma region DeviceParamFirmware
			case DeviceParamFirmware:
			{
			}
			break;
			#pragma endregion

			#pragma region DeviceParamIPAddress
			case DeviceParamIPAddress:
			{
			}
			break;

			#pragma endregion

			#pragma region DeviceParamSerialNumberString
			case DeviceParamSerialNumberString:
				break;
			#pragma endregion

			#pragma region DeviceParamCameraDefaultSettings
			case DeviceParamCameraDefaultSettings:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					//! First check to see if already set as this setting takes a long time
					if( S_OK == pDigitizer->ParameterGetValue( hDigitizer, SvDef::SVGigeParameterCameraDefaultSettings, 0, &l_oValue ) )
					{
						_bstr_t StringValue( l_oValue.bstrVal );
						if( pParam->strValue != static_cast<LPCTSTR> (StringValue) )
						{
							l_oValue = pParam->strValue.c_str();
							hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterCameraDefaultSettings, 0, &l_oValue );
						}
					}
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamShutter
			case DeviceParamShutter:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					l_oValue = pParam->lValue;

					hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterFeatureShutter, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamGamma
			case DeviceParamGamma:
			{
				// should check if parameter exists
				// need to put in camera file if the camera supports it.
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					l_oValue = pParam->GetNormalizedValue();
					hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterFeatureGamma, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamBrightness
			case DeviceParamBrightness:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					l_oValue = pParam->GetNormalizedValue();
					hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterFeatureBrightness, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamGain
			case DeviceParamGain:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					l_oValue = pParam->GetNormalizedValue();
					hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterFeatureGain, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion 

			#pragma region DeviceParamWhiteBalanceUB
			case DeviceParamWhiteBalanceUB:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					l_oValue = pParam->GetNormalizedValue();
					hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterFeatureWhiteBalanceU, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamWhiteBalanceVR
			case DeviceParamWhiteBalanceVR:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					l_oValue = pParam->GetNormalizedValue();
					hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterFeatureWhiteBalanceV, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamSharpness
			case DeviceParamSharpness:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					l_oValue = pParam->GetNormalizedValue();
					hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterFeatureSharpness, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamHue
			case DeviceParamHue:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					l_oValue = pParam->GetNormalizedValue();
					hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterFeatureHue, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamSaturation
			case DeviceParamSaturation:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					l_oValue = pParam->GetNormalizedValue();
					hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterFeatureSaturation, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamCameraFormats
			case DeviceParamCameraFormats:
			{
				if ( !IsOnline() )
				{
					const SVCameraFormatsDeviceParam* pParam = rw.DerivedValue( pParam );
					hr = SetCameraFormatParameters(hDigitizer, pDigitizer, pParam);
				}
				else
				{
					hr = E_FAIL;
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamGigeTriggerSource
			case DeviceParamGigeTriggerSource:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					_bstr_t value = pParam->strValue.c_str();
					l_oValue = value.Detach();
					hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterTriggerSource, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamAcquisitionTriggerEdge
			// This parameter doesn't get sent over to the DLL
			case DeviceParamAcquisitionTriggerEdge:
			{
				hr = S_OK;
				break;
			}
			#pragma endregion

			#pragma region DeviceParamGigeTriggerEdge
			case DeviceParamGigeTriggerEdge:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					_bstr_t value = pParam->strValue.c_str();
					l_oValue = value.Detach();
					hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterTriggerEdge, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamGigeTriggerEnable
			case DeviceParamGigeTriggerEnable:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					_bstr_t value = pParam->strValue.c_str();
					l_oValue = value.Detach();
					hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterTriggerEnable, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamShutterDelay
			case DeviceParamShutterDelay:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					l_oValue = pParam->lValue;
					hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterTriggerDelay, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamIOTriggerInvert
			// This parameter doesn't get sent over to the DLL
			case DeviceParamIOTriggerInvert:
			{
				hr = S_OK;
				break;
			}
			#pragma endregion

			#pragma region DeviceParamGigeTriggerLine
			case DeviceParamGigeTriggerLine:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					_bstr_t value = pParam->strValue.c_str();
					l_oValue = value.Detach();

					hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterTriggerLine, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamGigeStrobeSource
			case DeviceParamGigeStrobeSource:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					_bstr_t value = pParam->strValue.c_str();
					l_oValue = value.Detach();

					hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterStrobeSource, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamAcquisitionStrobeEdge
			// This parameter doesn't get sent over to the DLL
			case DeviceParamAcquisitionStrobeEdge:
			{
				hr = S_OK;
				break;
			}
			#pragma endregion

			#pragma region DeviceParamGigeStrobeEdge
			case DeviceParamGigeStrobeEdge:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					_bstr_t value = pParam->strValue.c_str();
					l_oValue = value.Detach();
					hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterStrobeEdge, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamGigeStrobeEnable
			case DeviceParamGigeStrobeEnable:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					_bstr_t value = pParam->strValue.c_str();
					l_oValue = value.Detach();
					hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterStrobeEnable, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamIOStrobeInvert
			// This parameter doesn't get sent over to the DLL
			case DeviceParamIOStrobeInvert:
			{
				hr = S_OK;
				break;
			}
			#pragma endregion

			#pragma region DeviceParamStrobePulseDelay
			case DeviceParamStrobePulseDelay:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					l_oValue = pParam->lValue;
					hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterStrobeDelay, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamStrobePulseDuration
			case DeviceParamStrobePulseDuration:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					l_oValue = pParam->lValue;
					hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterStrobeDuration, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamGigeStrobeLine
			case DeviceParamGigeStrobeLine:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					_bstr_t value = pParam->strValue.c_str();
					l_oValue = value.Detach();
					hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterStrobeLine, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamGigePacketSize
			case DeviceParamGigePacketSize:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					l_oValue = pParam->lValue;
					hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterPacketSize, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamLut
			case DeviceParamLut:
			{
				const SVLutDeviceParam* pLutParam = rw.DerivedValue( pLutParam );
				assert( pLutParam );

				// why plus one?
				int l_FileMaxValue = pLutParam->lut.Info().MaxValue() + 1;
				int l_FileBandSize = pLutParam->lut.Info().BandSize();

				// convert to safearray
				hr = SVLUTToSafeArray(pLutParam->lut, l_oValue);
				if (S_OK == hr)
				{
					// LUT is only enabled when the transform operation type is not "Normal"
					const SVLutTransformOperation* pOperation = pLutParam->lut.Info().GetTransformOperation();
					SVLutTransformOperationEnum TransformOperationEnum = SVLutTransform::GetEquivalentType(pOperation);
					
					bool EnableLUT = LutTransformTypeNormal != TransformOperationEnum;

					_variant_t EnableLUTVariant( EnableLUT );
					hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterLutEnable, 0, &EnableLUTVariant );
					if (S_OK == hr  && EnableLUT )
					{
						// Set LUT values
						hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterLutArray, 0, &l_oValue );
					}
				}
			}
			break;
			#pragma endregion

			#pragma region DeviceParamCameraInput
			case DeviceParamCameraInput:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					l_oValue.SetString(pParam->strValue.c_str());
					hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterLineInput, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamVerticalBinning
			case DeviceParamVerticalBinning:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					l_oValue = pParam->lValue;
					hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterVerticalBinning, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamHorizontalBinning
			case DeviceParamHorizontalBinning:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					l_oValue = pParam->lValue;

					hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterHorizontalBinning, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamGigeCustomNNN
			case DeviceParamGigeCustom1:
			case DeviceParamGigeCustom2:
			case DeviceParamGigeCustom3:
			case DeviceParamGigeCustom4:
			case DeviceParamGigeCustom5:
			case DeviceParamGigeCustom6:
			case DeviceParamGigeCustom7:
			case DeviceParamGigeCustom8:
			case DeviceParamGigeCustom9:
			case DeviceParamGigeCustom10:
			case DeviceParamGigeCustom11:
			case DeviceParamGigeCustom12:
			case DeviceParamGigeCustom13:
			case DeviceParamGigeCustom14:
			case DeviceParamGigeCustom15:
			case DeviceParamGigeCustom16:
			case DeviceParamGigeCustom17:
			case DeviceParamGigeCustom18:
			case DeviceParamGigeCustom19:
			case DeviceParamGigeCustom20:
			case DeviceParamGigeCustom21:
			case DeviceParamGigeCustom22:
			case DeviceParamGigeCustom23:
			case DeviceParamGigeCustom24:
			case DeviceParamGigeCustom25:
			case DeviceParamGigeCustom26:
			case DeviceParamGigeCustom27:
			case DeviceParamGigeCustom28:
			case DeviceParamGigeCustom29:
			case DeviceParamGigeCustom30:
			{
				const SVCustomDeviceParam* pParam = rw.DerivedValue(pParam);
				if (pParam)
				{
					const SVDeviceParamWrapper& rDeviceParamWrapper = pParam->GetHeldParam();
					const SVDeviceParam* pHeldParam = rDeviceParamWrapper.DerivedValue(pHeldParam); 
					if (pHeldParam)
					{
						_variant_t l_oValue;

						switch (pHeldParam->DataType())
						{
							case DeviceDataTypeLong:
								hr = pHeldParam->GetValue(l_oValue);
								if (S_OK == hr)
								{
									const SVLongValueDeviceParam* pLongParam = rDeviceParamWrapper.DerivedValue(pLongParam);
									if (pLongParam)
									{
										l_oValue = pLongParam->GetNormalizedValue();
									}
								}
								break;

							case DeviceDataTypei64:
								hr = pHeldParam->GetValue(l_oValue);
								break;

							case DeviceDataTypeBool:
								hr = pHeldParam->GetValue(l_oValue);
								break;

							case DeviceDataTypeString:
								hr = pHeldParam->GetValue(l_oValue);
								break;
						}
						if (S_OK == hr)
						{
							hr = pDigitizer->ParameterSetValue(hDigitizer, cDeviceParamEnumToGigeParamEnum[l_eType], 0, &l_oValue);
						}
					}
				}
				break;
			}
			#pragma endregion

			#pragma region default
			default:
			{
				hr = S_FALSE;
				break;
			}
			#pragma endregion
		}
	}

	if( S_OK == hr )
	{
		rDeviceParams.SetParameter( rw );
	}
	return hr;
}

HRESULT SVMatroxGigeCameraProxy::SetCameraFormatParameters(unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer, const SVCameraFormatsDeviceParam* pParam)
{
	HRESULT hr = S_OK;
	long horizontalBinning = 1;
	long verticalBinning = 1;
	const SVCameraFormat& rcf = pParam->options.find(pParam->strValue)->second;
	long l_Width = rcf.m_lWidth;
	long l_Height = rcf.m_lHeight;

	_variant_t l_oValue;
	int l_ParType;

	// Get Binning parameters
	HRESULT l_Temp = pDigitizer->ParameterGetValue( hDigitizer, SvDef::SVGigeParameterHorizontalBinning, &l_ParType, &l_oValue);
	if (S_OK == l_Temp)
	{
		if (l_oValue.lVal > 0)
		{
			horizontalBinning = l_oValue.lVal;
		}
	}
	l_Temp = pDigitizer->ParameterGetValue( hDigitizer, SvDef::SVGigeParameterVerticalBinning, &l_ParType, &l_oValue);
	if (S_OK == l_Temp)
	{
		if (l_oValue.lVal > 0)
		{
			verticalBinning = l_oValue.lVal;
		}
	}
	// Check if width/height exceeds binned width/height and adjust
	// Must be a multiple of step size
	long maxWidth = rcf.m_lWidthMax / horizontalBinning;
	if (rcf.m_lHPosStep > 1)
	{
		maxWidth -= (maxWidth % rcf.m_lHPosStep);
	}
	long maxHeight = rcf.m_lHeightMax / verticalBinning;
	if (rcf.m_lVPosStep > 1)
	{
		maxHeight -= (maxHeight % rcf.m_lVPosStep);
	}

	if (l_Height > maxHeight)
	{
		l_Height = maxHeight;
	}
	if (l_Width > maxWidth)
	{
		l_Width = maxWidth;
	}

	long l_XPrevSize=0;
	long l_YPrevSize=0;
	long l_XPrevOffset=0;
	long l_YPrevOffset=0;

	// Read previous parameters so we can make an intelligent decision on which parameters to set first.
	l_Temp = pDigitizer->ParameterGetValue( hDigitizer, SvDef::SVGigeParameterXSize, &l_ParType, &l_oValue);
	if (S_OK == l_Temp)
	{ 
		l_XPrevSize = l_oValue.lVal;
		l_oValue.Clear();
	}
	l_Temp = pDigitizer->ParameterGetValue( hDigitizer, SvDef::SVGigeParameterYSize, &l_ParType, &l_oValue);
	if (S_OK == l_Temp)
	{ 
		l_YPrevSize = l_oValue.lVal;
		l_oValue.Clear();
	}
	l_Temp = pDigitizer->ParameterGetValue( hDigitizer, SvDef::SVGigeParameterXOffset, &l_ParType, &l_oValue);
	if (S_OK == l_Temp)
	{ 
		l_XPrevOffset = l_oValue.lVal;
		l_oValue.Clear();
	}
	l_Temp = pDigitizer->ParameterGetValue( hDigitizer, SvDef::SVGigeParameterYOffset, &l_ParType, &l_oValue);
	if (S_OK == l_Temp)
	{ 
		l_YPrevOffset = l_oValue.lVal;
		l_oValue.Clear();
	}

	// Decide if we should adjust Width or X offset first to prevent errors.
	if( l_XPrevSize - l_Width > l_XPrevOffset - rcf.m_lHPos)
	{
		l_oValue = l_Width;
		l_Temp = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterXSize, 0, &l_oValue );
		l_oValue.Clear();

		if( S_OK == hr && S_OK != l_Temp )
		{
			hr = l_Temp;
		}
		l_oValue = rcf.m_lHPos;
		l_Temp = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterXOffset, 0, &l_oValue );
		l_oValue.Clear();

		if( S_OK == hr && S_OK != l_Temp )
		{
			hr = l_Temp;
		}
	}
	else
	{
		l_oValue = rcf.m_lHPos;
		l_Temp = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterXOffset, 0, &l_oValue );
		l_oValue.Clear();

		if( S_OK == hr && S_OK != l_Temp )
		{
			hr = l_Temp;
		}
		l_oValue = l_Width;
		l_Temp = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterXSize, 0, &l_oValue );
		l_oValue.Clear();

		if( S_OK == hr && S_OK != l_Temp )
		{
			hr = l_Temp;
		}
	}

	// Decide if we should adjust Height or Y offset first to prevent errors.
	if( l_YPrevSize - l_Height > l_YPrevOffset - rcf.m_lVPos)
	{
		l_oValue = l_Height;
		l_Temp = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterYSize, 0, &l_oValue );
		l_oValue.Clear();

		if( S_OK == hr && S_OK != l_Temp )
		{
			hr = l_Temp;
		}

		l_oValue = rcf.m_lVPos;
		l_Temp = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterYOffset, 0, &l_oValue );
		l_oValue.Clear();

		if( S_OK == hr && S_OK != l_Temp )
		{
			hr = l_Temp;
		}
	}
	else
	{
		l_oValue = rcf.m_lVPos;
		l_Temp = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterYOffset, 0, &l_oValue );
		l_oValue.Clear();

		if( S_OK == hr && S_OK != l_Temp )
		{
			hr = l_Temp;
		}

		l_oValue = l_Height;
		l_Temp = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterYSize, 0, &l_oValue );
		l_oValue.Clear();

		if( S_OK == hr && S_OK != l_Temp )
		{
			hr = l_Temp;
		}
	}

	typedef std::deque<std::string> split_container_type;
	split_container_type splitContainer;
	boost::algorithm::split(splitContainer, std::string(rcf.m_strName), boost::algorithm::is_any_of("_X"), boost::algorithm::token_compress_on);
	if (splitContainer.size() > 3)
	{
		l_oValue = splitContainer[3].c_str();
		l_Temp = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterColorFormat, 0, &l_oValue );
		l_oValue.Clear();
	}
	if( S_OK == hr && FAILED( l_Temp ) ) // This is allowed to be S_FALSE
	{
		hr = l_Temp;
	}
	return hr;
}

HRESULT SVMatroxGigeCameraProxy::SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer )
{
	HRESULT hr = S_OK;
	// Send notification to start tracking main camera parameters
	if ( nullptr != pDigitizer)
	{
		_variant_t dummy;
		hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeBeginTrackParameters, 0, &dummy);
	}
	if ( S_OK == hr )
	{
		hr = SVGigeCameraProxy::SetDeviceParameters( rDeviceParams, m_pAcquisition->m_DeviceParams );
	}
	if ( S_OK == hr )
	{
		hr = InitializeDevice( rDeviceParams, hDigitizer, pDigitizer );
	}
	return hr;
}

HRESULT SVMatroxGigeCameraProxy::IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer)
{
	if( 0 != hDigitizer && nullptr != pDigitizer &&
		rDeviceParams.ParameterExists( DeviceParamVendorName ) )
	{
		_variant_t l_oValue;

		if( nullptr != pDigitizer && S_OK == pDigitizer->ParameterGetValue( hDigitizer, SvDef::SVGigeParameterVendorName, 0, &l_oValue ) )
		{
			std::string venderNameHardware = SvUl::createStdString( l_oValue.bstrVal );
			std::string venderName( StringValue( rDeviceParams.Parameter( DeviceParamVendorName ) ) );

			if( venderNameHardware == venderName )
			{
				if( rDeviceParams.ParameterExists( DeviceParamModelName ) &&
					S_OK == pDigitizer->ParameterGetValue( hDigitizer, SvDef::SVGigeParameterModelName, 0, &l_oValue ) )
				{
					std::string sHardwareModel(SvUl::createStdString(l_oValue.bstrVal));

					std::string sModel = StringValue(rDeviceParams.Parameter( DeviceParamModelName ));

					bool sameModel = 0 == SvUl::CompareNoCase(sHardwareModel, sModel);
					if ( !sHardwareModel.empty() && !sModel.empty() && false == sameModel)
					{
						SvDef::StringVector msgList;
						msgList.push_back(sModel);
						msgList.push_back(sHardwareModel);
						SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
						Exception.setMessage( SVMSG_SVO_87_GOONLINE_CAMERA_ERROR, SvStl::Tid_Error_WrongCameraModel, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10026_GoOnline_WrongCameraModel );
						Exception.Throw();
					}
				}
			}
			else
			{
				SvDef::StringVector msgList;
				msgList.push_back(venderName);
				msgList.push_back(venderNameHardware);
				SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
				Exception.setMessage( SVMSG_SVO_87_GOONLINE_CAMERA_ERROR, SvStl::Tid_Error_WrongCameraModel, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10027_GoOnline_WrongCameraVender );
				Exception.Throw();
			}
		}
	}
	return S_OK;
}

bool SVMatroxGigeCameraProxy::CameraMatchesCameraFile(const SVDeviceParamCollection& rCameraFileDeviceParams, unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer)
{
	bool l_bOk = true;
	
	if( 0 != hDigitizer && nullptr != pDigitizer &&
		rCameraFileDeviceParams.ParameterExists( DeviceParamVendorName ) )
	{
		_variant_t l_oValue;

		// Check Vendor Name
		if( S_OK == pDigitizer->ParameterGetValue( hDigitizer, SvDef::SVGigeParameterVendorName, 0, &l_oValue ) )
		{
			std::string l_csVenderName = SvUl::createStdString(l_oValue.bstrVal);

			const SVDeviceParamWrapper param = rCameraFileDeviceParams.Parameter( DeviceParamVendorName );
			l_bOk = l_csVenderName == StringValue( param );
		}
		// if Vendor Name matched, check Model Name
		if ( l_bOk )
		{
			// Check Model Name
			if( S_OK == pDigitizer->ParameterGetValue( hDigitizer, SvDef::SVGigeParameterModelName, 0, &l_oValue ) )
			{
				std::string l_csModelName = SvUl::createStdString(l_oValue.bstrVal);

				const SVDeviceParamWrapper param = rCameraFileDeviceParams.Parameter( DeviceParamModelName );
				l_bOk = l_csModelName == StringValue( param );
			}
		}
	}
	return l_bOk;
}

HRESULT SVMatroxGigeCameraProxy::GoOnline(unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer)
{
	HRESULT hr = S_OK;

	std::string value( _T( "Enable" ) );

	// Set Trigger and Strobe
	// trigger first, then strobe
	SVDeviceParamWrapper& triggerEnable = GetCameraDeviceParamNonConst( DeviceParamGigeTriggerEnable );
	SVStringValueDeviceParam* pTriggerParam = triggerEnable.DerivedValue( pTriggerParam );
	pTriggerParam->strValue = value.c_str();
	
	SetDigitizerParameter(triggerEnable, hDigitizer, pDigitizer );

	SVDeviceParamWrapper& strobeEnable = GetCameraDeviceParamNonConst( DeviceParamGigeStrobeEnable );
	SVStringValueDeviceParam* pStrobeParam = strobeEnable.DerivedValue( pStrobeParam );
	pStrobeParam->strValue = value.c_str();
	SetDigitizerParameter( strobeEnable, hDigitizer, pDigitizer );

	return hr;
}

HRESULT SVMatroxGigeCameraProxy::GoOffline(unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer)
{
	HRESULT hr = S_OK;

	std::string value( _T( "Disable" ) );

	// Stop Trigger and Strobe 
	// strobe first, then trigger
	SVDeviceParamWrapper& strobeEnable = GetCameraDeviceParamNonConst( DeviceParamGigeStrobeEnable );
	SVStringValueDeviceParam* pStrobeParam = strobeEnable.DerivedValue( pStrobeParam );
	pStrobeParam->strValue = value.c_str();
	SetDigitizerParameter( strobeEnable, hDigitizer, pDigitizer );

	SVDeviceParamWrapper& triggerEnable = GetCameraDeviceParamNonConst( DeviceParamGigeTriggerEnable );
	SVStringValueDeviceParam* pTriggerParam = triggerEnable.DerivedValue( pTriggerParam );
	pTriggerParam->strValue = value.c_str();
	SetDigitizerParameter(triggerEnable, hDigitizer, pDigitizer );
	
	return hr;
}

bool SVMatroxGigeCameraProxy::IsOnline() const
{
	bool bRetVal = false;

	if (m_pAcquisition)
	{
		bRetVal = m_pAcquisition->IsOnline();
	}
	return bRetVal;
}

//
HRESULT SVMatroxGigeCameraProxy::SetDigitizerParameters( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer )
{
	HRESULT hr = S_OK;

	SVDeviceParamMap::const_iterator iter;
	for ( iter = rDeviceParams.mapParameters.begin(); iter != rDeviceParams.mapParameters.end(); ++iter )
	{
		const SVDeviceParamWrapper& rw = iter->second;
		if ( rw.IsValid() )
		{
			hr = SetDigitizerParameter( rw, hDigitizer, pDigitizer );
		}
	}
	return hr;
}

HRESULT SVMatroxGigeCameraProxy::SetDigitizerParameter( const SVDeviceParamWrapper& rw, unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer )
{
	HRESULT hr = S_FALSE;

	if (nullptr != pDigitizer)
	{
		const SVDeviceParam* pParam = (const SVDeviceParam*) rw;
		if ( nullptr != pParam )
		{
			// Translate DeviceParameterEnum to GigeParameterEnum
			hr = SetStandardCameraParameter( rw, m_pAcquisition->m_DeviceParams, hDigitizer, pDigitizer );
		}
	}
	return hr;
}

SVDeviceParamWrapper& SVMatroxGigeCameraProxy::GetCameraDeviceParamNonConst( SVDeviceParamEnum e )
{
	return m_pAcquisition->m_DeviceParams.GetParameter( e );
}

bool SVMatroxGigeCameraProxy::CanSoftwareTrigger() const
{
	// check if the Software Trigger option is available in the device parameters ?
	return true;
}

// For SingleGrabHelper
HRESULT SVMatroxGigeCameraProxy::EnableSoftwareTrigger(unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer)
{
	return pDigitizer->InternalTriggerEnable(hDigitizer);
}

// For SingleGrabHelper
HRESULT SVMatroxGigeCameraProxy::SoftwareTrigger(unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer)
{
	return pDigitizer->InternalTrigger(hDigitizer, _variant_t());
}

HRESULT SVMatroxGigeCameraProxy::SVLUTToSafeArray(const SVLut& lut, _variant_t& output)
{
	SAFEARRAY* psaBandData(nullptr);

	bool bRetVal = lut.GetBandData(psaBandData);
	if (bRetVal)
	{
		output.vt = VT_I4 | VT_ARRAY;
		output.parray = psaBandData;
	}
	return (bRetVal) ? S_OK : S_FALSE;
}
