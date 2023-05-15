//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVMatroxGigeCameraProxy.h"
#include "CameraLibrary/SVStringValueDeviceParam.h"
#include "CameraLibrary/SVLongValueDeviceParam.h"
#include "CameraLibrary/SVBoolValueDeviceParam.h"
#include "CameraLibrary/SVCustomDeviceParam.h"
#include "Definitions/StringTypeDef.h"
#include "Definitions/SVGigeEnums.h"
#include "Definitions/TriggerType.h"
#include "InspectionEngine/SVMatroxGigeAcquisitionClass.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "Triggering/SVDigitizerLoadLibraryClass.h"
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

HRESULT SVMatroxGigeCameraProxy::InitializeDevice( const SVDeviceParamWrapper& rwParam, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer )
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

HRESULT SVMatroxGigeCameraProxy::InitializeDevice( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer )
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

HRESULT SVMatroxGigeCameraProxy::SetGigeFeatureOverrides(const std::string& xmlData, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer)
{
	HRESULT hr = S_OK;

	if (nullptr != pDigitizer && 0 != hDigitizer)
	{
		_variant_t value;
		value.SetString(xmlData.c_str());
		hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterFeatureOverrides, value);
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

HRESULT SVMatroxGigeCameraProxy::SetStandardCameraParameter( const SVDeviceParamWrapper& rw, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer )
{
	HRESULT hr = S_OK;

	if ( nullptr != pDigitizer && 0 != hDigitizer )
	{
		SVDeviceParamEnum l_eType = rw->Type();

		switch ( l_eType )
		{
			case DeviceParamVendorName:
			case DeviceParamModelName:
			case DeviceParamFirmware:
			case DeviceParamIPAddress:
			case DeviceParamSerialNumberString:
				break;

			case DeviceParamCameraDefaultSettings:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					//! First check to see if already set as this setting takes a long time
					_variant_t value = pDigitizer->ParameterGetValue(hDigitizer, SvDef::SVGigeParameterCameraDefaultSettings);
					if(VT_EMPTY != value.vt)
					{
						if(pParam->strValue !=  SvUl::createStdString(value))
						{
							_variant_t varValue;
							varValue.SetString(pParam->strValue.c_str());
							hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterCameraDefaultSettings, varValue);
						}
					}
				}
				break;
			}

			case DeviceParamShutter:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					_variant_t varValue = pParam->lValue;
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterFeatureShutter, varValue);
				}
				break;
			}

			case DeviceParamGamma:
			{
				// should check if parameter exists
				// need to put in camera file if the camera supports it.
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					_variant_t varValue = pParam->GetNormalizedValue();
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterFeatureGamma, varValue);
				}
				break;
			}

			case DeviceParamBrightness:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					_variant_t varValue = pParam->GetNormalizedValue();
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterFeatureBrightness, varValue );
				}
				break;
			}

			case DeviceParamGain:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					_variant_t varValue = pParam->GetNormalizedValue();
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterFeatureGain, varValue);
				}
				break;
			}

			case DeviceParamWhiteBalanceUB:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					_variant_t varValue = pParam->GetNormalizedValue();
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterFeatureWhiteBalanceU, varValue);
				}
				break;
			}

			case DeviceParamWhiteBalanceVR:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					_variant_t varValue = pParam->GetNormalizedValue();
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterFeatureWhiteBalanceV, varValue);
				}
				break;
			}

			case DeviceParamSharpness:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					_variant_t varValue = pParam->GetNormalizedValue();
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterFeatureSharpness, varValue);
				}
				break;
			}

			case DeviceParamHue:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					_variant_t varValue = pParam->GetNormalizedValue();
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterFeatureHue, varValue);
				}
				break;
			}

			case DeviceParamSaturation:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					_variant_t varValue = pParam->GetNormalizedValue();
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterFeatureSaturation, varValue);
				}
				break;
			}

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

			case DeviceParamGigeTriggerSource:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					_variant_t varValue;
					varValue.SetString(pParam->strValue.c_str());
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterTriggerSource, varValue);
				}
				break;
			}

			// These parameters are nor sent via the DLL
			case DeviceParamAcquisitionTriggerEdge:
			case DeviceParamIOStrobeInvert:
			case DeviceParamIOTriggerInvert:
			case DeviceParamAcquisitionStrobeEdge:
			{
				hr = S_OK;
				break;
			}

			case DeviceParamGigeTriggerEdge:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					_variant_t varValue;
					varValue.SetString(pParam->strValue.c_str());
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterTriggerEdge, varValue);
				}
				break;
			}

			case DeviceParamGigeTriggerEnable:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					_variant_t varValue;
					varValue.SetString(pParam->strValue.c_str());
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterTriggerEnable, varValue);
				}
				break;
			}

			case DeviceParamShutterDelay:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					_variant_t varValue = pParam->lValue;
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterTriggerDelay, varValue);
				}
				break;
			}

			case DeviceParamGigeTriggerLine:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					_variant_t varValue;
					varValue.SetString(pParam->strValue.c_str());
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterTriggerLine, varValue);
				}
				break;
			}

			case DeviceParamGigeStrobeSource:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					_variant_t varValue;
					varValue.SetString(pParam->strValue.c_str());
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterStrobeSource, varValue);
				}
				break;
			}

			case DeviceParamGigeStrobeEdge:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					_variant_t varValue;
					varValue.SetString(pParam->strValue.c_str());
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterStrobeEdge, varValue);
				}
				break;
			}

			case DeviceParamGigeStrobeEnable:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					_variant_t varValue;
					varValue.SetString(pParam->strValue.c_str());
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterStrobeEnable, varValue);
				}
				break;
			}

			case DeviceParamStrobePulseDelay:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					_variant_t varValue = pParam->lValue;
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterStrobeDelay, varValue);
				}
				break;
			}

			case DeviceParamStrobePulseDuration:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					_variant_t varValue = pParam->lValue;
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterStrobeDuration, varValue);
				}
				break;
			}

			case DeviceParamGigeStrobeLine:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					_variant_t varValue;
					varValue.SetString(pParam->strValue.c_str());
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterStrobeLine, varValue);
				}
				break;
			}

			case DeviceParamGigePacketSize:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					_variant_t varValue = pParam->lValue;
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterPacketSize, varValue);
				}
				break;
			}

			case DeviceParamLut:
			{
				const SVLutDeviceParam* pLutParam = rw.DerivedValue( pLutParam );
				Log_Assert( pLutParam );

				_variant_t varValue;
				// convert to safearray
				hr = SVLUTToSafeArray(pLutParam->lut, varValue);
				if (S_OK == hr)
				{
					// LUT is only enabled when the transform operation type is not "Normal"
					const SVLutTransformOperation* pOperation = pLutParam->lut.Info().GetTransformOperation();
					SVLutTransformOperationEnum TransformOperationEnum = SVLutTransform::GetEquivalentType(pOperation);
					
					bool EnableLUT = LutTransformTypeNormal != TransformOperationEnum;

					_variant_t EnableLUTVariant( EnableLUT );
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterLutEnable, EnableLUTVariant);
					if (S_OK == hr  && EnableLUT )
					{
						// Set LUT values
						hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterLutArray, varValue);
					}
				}
				break;
			}

			case DeviceParamCameraInput:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					_variant_t varValue;
					varValue.SetString(pParam->strValue.c_str());
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterLineInput, varValue);
				}
				break;
			}

			case DeviceParamVerticalBinning:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					_variant_t varValue = pParam->lValue;
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterVerticalBinning, varValue);
				}
				break;
			}

			case DeviceParamHorizontalBinning:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					_variant_t varValue = pParam->lValue;

					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterHorizontalBinning, varValue);
				}
				break;
			}

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
				if (nullptr != pParam)
				{
					const SVDeviceParamWrapper& rDeviceParamWrapper = pParam->GetHeldParam();
					const SVDeviceParam* pHeldParam = rDeviceParamWrapper.DerivedValue(pHeldParam); 
					if (pHeldParam)
					{
						_variant_t varValue;
						switch (pHeldParam->DataType())
						{
							case DeviceDataTypeLong:
								hr = pHeldParam->GetValue(varValue);
								if (S_OK == hr)
								{
									const SVLongValueDeviceParam* pLongParam = rDeviceParamWrapper.DerivedValue(pLongParam);
									if (pLongParam)
									{
										varValue = pLongParam->GetNormalizedValue();
									}
								}
								break;

							case DeviceDataTypei64:
								hr = pHeldParam->GetValue(varValue);
								break;

							case DeviceDataTypeBool:
								hr = pHeldParam->GetValue(varValue);
								break;

							case DeviceDataTypeString:
								hr = pHeldParam->GetValue(varValue);
								break;
						}
						if (S_OK == hr)
						{
							hr = pDigitizer->ParameterSetValue(hDigitizer, cDeviceParamEnumToGigeParamEnum[l_eType], varValue);
						}
					}
				}
				break;
			}

			default:
			{
				hr = E_FAIL;
				break;
			}
		}
	}

	if( S_OK == hr )
	{
		rDeviceParams.SetParameter( rw );
	}
	return hr;
}

HRESULT SVMatroxGigeCameraProxy::SetCameraFormatParameters(unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer, const SVCameraFormatsDeviceParam* pParam)
{
	HRESULT hr = S_OK;
	long horizontalBinning = 1;
	long verticalBinning = 1;
	const SVCameraFormat& rcf = pParam->options.find(pParam->strValue)->second;
	long l_Width = rcf.m_lWidth;
	long l_Height = rcf.m_lHeight;

	// Get Binning parameters
	_variant_t value = pDigitizer->ParameterGetValue( hDigitizer, SvDef::SVGigeParameterHorizontalBinning);
	if (VT_EMPTY != value.vt)
	{
		if (value.lVal > 0)
		{
			horizontalBinning = value.lVal;
		}
	}
	value = pDigitizer->ParameterGetValue( hDigitizer, SvDef::SVGigeParameterVerticalBinning);
	if (VT_EMPTY != value.vt)
	{
		if (value.lVal > 0)
		{
			verticalBinning = value.lVal;
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
	value = pDigitizer->ParameterGetValue( hDigitizer, SvDef::SVGigeParameterXSize);
	if (VT_EMPTY != value.vt)
	{
		l_XPrevSize = value.lVal;
	}
	value = pDigitizer->ParameterGetValue( hDigitizer, SvDef::SVGigeParameterYSize);
	if (VT_EMPTY != value.vt)
	{
		l_YPrevSize = value.lVal;
	}
	value = pDigitizer->ParameterGetValue( hDigitizer, SvDef::SVGigeParameterXOffset);
	if (VT_EMPTY != value.vt)
	{
		l_XPrevOffset = value.lVal;
	}
	value = pDigitizer->ParameterGetValue(hDigitizer, SvDef::SVGigeParameterYOffset);
	if (VT_EMPTY != value.vt)
	{
		l_YPrevOffset = value.lVal;
	}

	HRESULT l_Temp{S_OK};
	// Decide if we should adjust Width or X offset first to prevent errors.
	if( l_XPrevSize - l_Width > l_XPrevOffset - rcf.m_lHPos)
	{
		value = l_Width;
		l_Temp = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterXSize, value);
		if( S_OK == hr && S_OK != l_Temp )
		{
			hr = l_Temp;
		}
		value = rcf.m_lHPos;
		l_Temp = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterXOffset, value);
		if( S_OK == hr && S_OK != l_Temp )
		{
			hr = l_Temp;
		}
	}
	else
	{
		value = rcf.m_lHPos;
		l_Temp = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterXOffset, value);
		if( S_OK == hr && S_OK != l_Temp )
		{
			hr = l_Temp;
		}
		value = l_Width;
		l_Temp = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterXSize, value);
		if( S_OK == hr && S_OK != l_Temp )
		{
			hr = l_Temp;
		}
	}

	// Decide if we should adjust Height or Y offset first to prevent errors.
	if( l_YPrevSize - l_Height > l_YPrevOffset - rcf.m_lVPos)
	{
		value = l_Height;
		l_Temp = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterYSize, value);
		if( S_OK == hr && S_OK != l_Temp )
		{
			hr = l_Temp;
		}
		value = rcf.m_lVPos;
		l_Temp = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterYOffset, value);
		if( S_OK == hr && S_OK != l_Temp )
		{
			hr = l_Temp;
		}
	}
	else
	{
		value = rcf.m_lVPos;
		l_Temp = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterYOffset, value);
		if( S_OK == hr && S_OK != l_Temp )
		{
			hr = l_Temp;
		}
		value = l_Height;
		l_Temp = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterYSize, value);
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
		value.SetString(splitContainer[3].c_str());
		pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterColorFormat, value);
	}
	return hr;
}

HRESULT SVMatroxGigeCameraProxy::SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer )
{
	HRESULT hr = S_OK;
	// Send notification to start tracking main camera parameters
	if ( nullptr != pDigitizer)
	{
		_variant_t dummy;
		hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeBeginTrackParameters, dummy);
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

HRESULT SVMatroxGigeCameraProxy::IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer)
{
	if( 0 != hDigitizer && nullptr != pDigitizer && rDeviceParams.ParameterExists(DeviceParamVendorName))
	{
		_variant_t value = pDigitizer->ParameterGetValue(hDigitizer, SvDef::SVGigeParameterVendorName);
		if(VT_EMPTY != value.vt)
		{
			std::string venderNameHardware = SvUl::createStdString(value);
			std::string venderName( StringValue( rDeviceParams.Parameter( DeviceParamVendorName ) ) );

			if( venderNameHardware == venderName )
			{
				value = pDigitizer->ParameterGetValue(hDigitizer, SvDef::SVGigeParameterModelName);
				if( rDeviceParams.ParameterExists(DeviceParamModelName) && VT_EMPTY != value.vt)
				{
					std::string sHardwareModel(SvUl::createStdString(value));

					std::string sModel = StringValue(rDeviceParams.Parameter(DeviceParamModelName));

					bool sameModel = 0 == SvUl::CompareNoCase(sHardwareModel, sModel);
					if ( !sHardwareModel.empty() && !sModel.empty() && false == sameModel)
					{
						SvDef::StringVector msgList;
						msgList.push_back(sModel);
						msgList.push_back(sHardwareModel);
						SvStl::MessageManager Exception(SvStl::MsgType::Data);
						Exception.setMessage( SVMSG_SVO_87_GOONLINE_CAMERA_ERROR, SvStl::Tid_Error_WrongCameraModel, msgList, SvStl::SourceFileParams(StdMessageParams));
						Exception.Throw();
					}
				}
			}
			else
			{
				SvDef::StringVector msgList;
				msgList.push_back(venderName);
				msgList.push_back(venderNameHardware);
				SvStl::MessageManager Exception(SvStl::MsgType::Data);
				Exception.setMessage( SVMSG_SVO_87_GOONLINE_CAMERA_ERROR, SvStl::Tid_Error_WrongCameraModel, msgList, SvStl::SourceFileParams(StdMessageParams));
				Exception.Throw();
			}
		}
	}
	return S_OK;
}

bool SVMatroxGigeCameraProxy::CameraMatchesCameraFile(const SVDeviceParamCollection& rCameraFileDeviceParams, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer)
{
	bool result{ true };
	
	if( 0 != hDigitizer && nullptr != pDigitizer && rCameraFileDeviceParams.ParameterExists(DeviceParamVendorName))
	{
		_variant_t value = pDigitizer->ParameterGetValue(hDigitizer, SvDef::SVGigeParameterVendorName);
		if(VT_EMPTY != value.vt)
		{
			std::string vendorName = SvUl::createStdString(value);
			const SVDeviceParamWrapper param = rCameraFileDeviceParams.Parameter( DeviceParamVendorName );
			result = vendorName == StringValue(param);
		}
		// if Vendor Name matched, check Model Name
		if ( result )
		{
			value = pDigitizer->ParameterGetValue(hDigitizer, SvDef::SVGigeParameterModelName);
			// Check Model Name
			if(VT_EMPTY != value.vt)
			{
				std::string l_csModelName = SvUl::createStdString(value);

				const SVDeviceParamWrapper param = rCameraFileDeviceParams.Parameter( DeviceParamModelName );
				result = l_csModelName == StringValue( param );
			}
		}
	}
	return result;
}

HRESULT SVMatroxGigeCameraProxy::GoOnline(unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer)
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

HRESULT SVMatroxGigeCameraProxy::GoOffline(unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer)
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
HRESULT SVMatroxGigeCameraProxy::SetDigitizerParameters( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer )
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

HRESULT SVMatroxGigeCameraProxy::SetDigitizerParameter( const SVDeviceParamWrapper& rw, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer )
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
HRESULT SVMatroxGigeCameraProxy::EnableSoftwareTrigger(unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer)
{
	_variant_t value{ static_cast<long> (SvDef::TriggerType::SoftwareTrigger) };
	return pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVTriggerType, value);
}

// For SingleGrabHelper
HRESULT SVMatroxGigeCameraProxy::SoftwareTrigger(unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer)
{
	return pDigitizer->InternalTrigger(hDigitizer, nullptr, 0U);
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
