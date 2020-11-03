//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTestGigeCameraProxy.cpp
//* .File Name       : $Workfile:   SVTestGigeCameraProxy.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   03 Dec 2014 19:49:50  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#pragma warning (push)
#pragma warning (disable : 4996)
//Moved to precompiled header: #include <deque>
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/algorithm/string.hpp>
#include "SVTestGigeCameraProxy.h"
#include "Definitions/SVGigeEnums.h"
#include "CameraLibrary/SVBoolValueDeviceParam.h"
#include "CameraLibrary/SVCustomDeviceParam.h"
#include "CameraLibrary/SVLongValueDeviceParam.h"
#include "CameraLibrary/SVStringValueDeviceParam.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "TriggerHandling/SVDigitizerLoadLibraryClass.h"
#include "SVTestAcquisitionClass.h"
#pragma warning (pop)
#pragma endregion Includes

constexpr int SVMSG_SVO_20_INCORRECT_CAMERA_FILE = 20;

typedef std::map<SVDeviceParamEnum, SvDef::SVGigeParameterEnum> DeviceParamToGigeParamAssoc;
static const DeviceParamToGigeParamAssoc cDeviceParamEnumToGigeParamEnum
{
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
	{DeviceParamCameraDefaultSettings, SvDef::SVGigeParameterCameraDefaultSettings}
};

SVTestGigeCameraProxy::SVTestGigeCameraProxy()
{
	m_pAcquisition = nullptr;
}

SVTestGigeCameraProxy::~SVTestGigeCameraProxy()
{
}

void SVTestGigeCameraProxy::SetOwner(SVTestAcquisitionClass* pAcquisition)
{
	m_pAcquisition = pAcquisition;
}

HRESULT SVTestGigeCameraProxy::InitializeDevice( const SVDeviceParamWrapper& rwParam, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer )
{
	HRESULT hr = S_OK;

	if ( !CameraMatchesCameraFile( rDeviceParams, hDigitizer, pDigitizer ) )
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

HRESULT SVTestGigeCameraProxy::InitializeDevice( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer )
{
	HRESULT hr = S_OK;

	if( !CameraMatchesCameraFile( rDeviceParams, hDigitizer, pDigitizer ) )
	{
		hr = SVMSG_SVO_20_INCORRECT_CAMERA_FILE;
	}

//	UpdateLightReferenceAttributes( rDeviceParams );

	if ( S_OK == hr )
	{
		if ( CameraMatchesCameraFile( rDeviceParams, hDigitizer, pDigitizer ) )
		{
			hr = SetDigitizerParameters( rDeviceParams, hDigitizer, pDigitizer );
		}
	}
	return hr;
}

HRESULT SVTestGigeCameraProxy::SetGigeFeatureOverrides(const std::string& rXmlData, unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer)
{
	HRESULT hr = S_OK;

	if (nullptr != pDigitizer && 0 < hDigitizer)
	{
		_variant_t value;
		value.SetString(rXmlData.c_str());
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

HRESULT SVTestGigeCameraProxy::SetStandardCameraParameter( const SVDeviceParamWrapper& rw, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer )
{
	HRESULT hr = S_OK;

	if ( nullptr != pDigitizer && 0 < hDigitizer )
	{
		_variant_t value;

		SVDeviceParamEnum l_eType = rw->Type();

		switch ( l_eType )
		{
			case DeviceParamVendorName:
			case DeviceParamModelName:
			case DeviceParamFirmware:
			case DeviceParamSerialNumberString:
				break;

			case DeviceParamCameraDefaultSettings:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					value = pDigitizer->ParameterGetValue(hDigitizer, SvDef::SVGigeParameterCameraDefaultSettings);
					//! First check to see if already set as this setting takes a long time
					if(VT_EMPTY != value.vt)
					{
						std::string StringValue(SvUl::createStdString(value));
						if(pParam->strValue != StringValue)
						{
							value.SetString(pParam->strValue.c_str());
							hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterCameraDefaultSettings, value);
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
					value = pParam->lValue;
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterFeatureShutter, value);
				}
				break;
			}

			case DeviceParamGamma:
			{
				// should check if parameter exists
				// need to put in camera file if the camera supports it.
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr !=  pParam )
				{
					value = pParam->GetNormalizedValue();
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterFeatureGamma, value);
				}
				break;
			}

			case DeviceParamBrightness:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					value = pParam->GetNormalizedValue();
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterFeatureBrightness, value);
				}
				break;
			}

			case DeviceParamGain:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					value = pParam->GetNormalizedValue();
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterFeatureGain, value);
				}
				break;
			}

			case DeviceParamWhiteBalanceUB:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					value = pParam->GetNormalizedValue();
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterFeatureWhiteBalanceU, value);
				}
				break;
			}

			case DeviceParamWhiteBalanceVR:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					value = pParam->GetNormalizedValue();
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterFeatureWhiteBalanceV, value);
				}
				break;
			}

			case DeviceParamSharpness:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					value = pParam->GetNormalizedValue();
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterFeatureSharpness, value);
				}
				break;
			}

			case DeviceParamHue:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue(pParam);
				if (nullptr != pParam)
				{
					value = pParam->GetNormalizedValue();
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterFeatureHue, value);
				}
				break;
			}

			case DeviceParamSaturation:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					value = pParam->GetNormalizedValue();
					hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterFeatureSaturation, value);
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
					value.SetString(pParam->strValue.c_str());
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterTriggerSource, value);
				}
				break;
			}

			// These parameters are nor sent via the DLL
			case DeviceParamAcquisitionTriggerEdge:
			case DeviceParamShutterDelay:
			case DeviceParamIOTriggerInvert:
			case DeviceParamAcquisitionStrobeEdge:
			case DeviceParamIOStrobeInvert:
			{
				hr = S_OK;
				break;
			}

			case DeviceParamGigeTriggerEdge:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					value.SetString(pParam->strValue.c_str());
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterTriggerEdge, value);
				}
				break;
			}

			case DeviceParamGigeTriggerEnable:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					value.SetString(pParam->strValue.c_str());
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterTriggerEnable, value);
				}
				break;
			}

			case DeviceParamGigeTriggerLine:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					value.SetString(pParam->strValue.c_str());
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterTriggerLine, value);
				}
				break;
			}

			case DeviceParamGigeStrobeSource:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					value.SetString(pParam->strValue.c_str());
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterStrobeSource, value);
				}
				break;
			}

			case DeviceParamGigeStrobeEdge:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					value.SetString(pParam->strValue.c_str());
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterStrobeEdge, value);
				}
				break;
			}

			case DeviceParamGigeStrobeEnable:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					value.SetString(pParam->strValue.c_str());
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterStrobeEnable, value);
				}
				break;
			}

			case DeviceParamStrobePulseDelay:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					value = pParam->lValue;
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterStrobeDelay, value);
				}
				break;
			}

			case DeviceParamStrobePulseDuration:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					value = pParam->lValue;
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterStrobeDuration, value);
				}
				break;
			}

			case DeviceParamGigeStrobeLine:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					value.SetString(pParam->strValue.c_str());
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterStrobeLine, value);
				}
				break;
			}

			case DeviceParamGigePacketSize:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					value = pParam->lValue;
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterPacketSize, value);
				}
				break;
			}

			case DeviceParamLut:
			{
				const SVLutDeviceParam* pLutParam = rw.DerivedValue( pLutParam );
				assert( pLutParam );

				// convert to safearray
				hr = SVLUTToSafeArray(pLutParam->lut, value);
				if (S_OK == hr)
				{
					// LUT is only enabled when the transform operation type is not "Normal"
					const SVLutTransformOperation* pOperation = pLutParam->lut.Info().GetTransformOperation();
					SVLutTransformOperationEnum TransformOperationEnum = SVLutTransform::GetEquivalentType(pOperation);

					bool EnableLUT = LutTransformTypeNormal != TransformOperationEnum;

					_variant_t EnableLUTVariant( EnableLUT );
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterLutEnable, EnableLUTVariant);
					if (S_OK == hr)
					{
						// Set LUT values
						hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterLutArray, value);
					}
				}
			}
			break;

			case DeviceParamCameraInput:
			{
				const SVStringValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					value.SetString(pParam->strValue.c_str());
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterLineInput, value);
				}
				break;
			}

			case DeviceParamVerticalBinning:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					value = pParam->lValue;
					hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterVerticalBinning, value);
				}
				break;
			}

			case DeviceParamHorizontalBinning:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if (nullptr != pParam)
				{
					value = pParam->lValue;
					hr = pDigitizer->ParameterSetValue( hDigitizer, SvDef::SVGigeParameterHorizontalBinning, value);
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
						switch (pHeldParam->DataType())
						{
							case DeviceDataTypeLong:
								hr = pHeldParam->GetValue(value);
								if (S_OK == hr)
								{
									const SVLongValueDeviceParam* pLongParam = rDeviceParamWrapper.DerivedValue(pLongParam);
									if (pLongParam)
									{
										value = pLongParam->GetNormalizedValue();
									}
								}
								break;

							case DeviceDataTypei64:
								hr = pHeldParam->GetValue(value);
								break;

							case DeviceDataTypeBool:
								hr = pHeldParam->GetValue(value);
								break;

							case DeviceDataTypeString:
								hr = pHeldParam->GetValue(value);
								break;
						}
						if (S_OK == hr)
						{
							hr = pDigitizer->ParameterSetValue(hDigitizer, cDeviceParamEnumToGigeParamEnum.at(l_eType), value);
						}
					}
				}
				break;
			}
			#pragma endregion

			#pragma region default
			default:
			{
				hr = E_FAIL;
				break;
			}
			#pragma endregion
		}
	}

	if( S_OK == hr )
	{
		rDeviceParams.SetParameter(rw);
	}
	return hr;
}

HRESULT SVTestGigeCameraProxy::SetCameraFormatParameters(unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer, const SVCameraFormatsDeviceParam* pParam)
{
	if (nullptr == pDigitizer || nullptr == pParam)
	{
		return E_POINTER;
	}
	HRESULT hr = S_OK;
	long horizontalBinning = 1;
	long verticalBinning = 1;
	const SVCameraFormat& rcf = pParam->options.find(pParam->strValue)->second;
	long l_Width = rcf.m_lWidth;
	long l_Height = rcf.m_lHeight;

	_variant_t value;

	value = pDigitizer->ParameterGetValue(hDigitizer, SvDef::SVGigeParameterHorizontalBinning);
	// Get Binning parameters
	if(VT_EMPTY != value.vt && value.lVal > 0)
	{
		horizontalBinning = value.lVal;
	}
	value = pDigitizer->ParameterGetValue(hDigitizer, SvDef::SVGigeParameterVerticalBinning);
	if (VT_EMPTY != value.vt && value.lVal > 0)
	{
		verticalBinning = value.lVal;
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

	long l_XPrevSize = 0;
	long l_YPrevSize = 0;
	long l_XPrevOffset = 0;
	long l_YPrevOffset = 0;

	// Read previous parameters so we can make an intelligent decision on which parameters to set first.
	value = pDigitizer->ParameterGetValue(hDigitizer, SvDef::SVGigeParameterXSize);
	if(VT_EMPTY != value.vt)
	{ 
		l_XPrevSize = value.lVal;
	}
	value = pDigitizer->ParameterGetValue(hDigitizer, SvDef::SVGigeParameterYSize);
	if (VT_EMPTY != value.vt)
	{
		l_YPrevSize = value.lVal;
	}
	value = pDigitizer->ParameterGetValue(hDigitizer, SvDef::SVGigeParameterXOffset);
	if (VT_EMPTY != value.vt)
	{
		l_XPrevOffset = value.lVal;
	}
	value = pDigitizer->ParameterGetValue(hDigitizer, SvDef::SVGigeParameterYOffset);
	if (VT_EMPTY != value.vt)
	{
		l_YPrevOffset = value.lVal;
	}

	HRESULT l_Temp{ S_OK };
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
	boost::algorithm::split(splitContainer, rcf.m_strName, boost::algorithm::is_any_of( _T("_X") ), boost::algorithm::token_compress_on);
	if (splitContainer.size() > 3)
	{
		value.SetString(splitContainer[3].c_str());
		pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeParameterColorFormat, value);
	}
	return hr;
}

HRESULT SVTestGigeCameraProxy::SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer )
{
	HRESULT hr = S_OK;
	// Send notification to start tracking main camera parameters
	if ( nullptr != pDigitizer )
	{
		_variant_t dummy;
		hr = pDigitizer->ParameterSetValue(hDigitizer, SvDef::SVGigeBeginTrackParameters, dummy);
	}
	if ( S_OK == hr )
	{
		hr = m_pAcquisition->SetDeviceParameters(rDeviceParams);
	}
	if ( S_OK == hr )
	{
		hr = InitializeDevice( rDeviceParams, hDigitizer, pDigitizer );
	}
	return hr;
}

HRESULT SVTestGigeCameraProxy::IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer)
{
	HRESULT l_hrOk = S_OK;

	if( 0 < hDigitizer && nullptr != pDigitizer && rDeviceParams.ParameterExists( DeviceParamVendorName ) )
	{
		_variant_t value = pDigitizer->ParameterGetValue(hDigitizer, SvDef::SVGigeParameterVendorName);

		if(VT_EMPTY != value.vt)
		{
			std::string l_csVenderName  = SvUl::createStdString(value);

			if( l_csVenderName == StringValue( rDeviceParams.Parameter( DeviceParamVendorName ) ) )
			{
				value = pDigitizer->ParameterGetValue(hDigitizer, SvDef::SVGigeParameterModelName);
				if( rDeviceParams.ParameterExists( DeviceParamModelName ) && VT_EMPTY != value.vt)
				{
					std::string HardwareModel = SvUl::createStdString(value);

					const std::string& rModel = StringValue( rDeviceParams.Parameter( DeviceParamModelName ) );

					if ( !HardwareModel.empty() && !rModel.empty() && SvUl::CompareNoCase( HardwareModel, rModel) != 0 )
					{
						l_hrOk = E_FAIL;
					}
				}
			}
			else
			{
				l_hrOk = E_FAIL;
			}
		}
	}
	return l_hrOk;
}

bool SVTestGigeCameraProxy::CameraMatchesCameraFile(const SVDeviceParamCollection& rCameraFileDeviceParams, unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer)
{
	bool l_bOk = true;
	
	if( 0 < hDigitizer && nullptr != pDigitizer && rCameraFileDeviceParams.ParameterExists( DeviceParamVendorName ) )
	{
		// Check Vendor Name
		_variant_t value = pDigitizer->ParameterGetValue( hDigitizer, SvDef::SVGigeParameterVendorName);
		if(VT_EMPTY != value.vt)
		{
			std::string vendorName = SvUl::createStdString(value);
			const SVDeviceParamWrapper param = rCameraFileDeviceParams.Parameter( DeviceParamVendorName );
			l_bOk = vendorName == StringValue(param);
		}
		// if Vendor Name matched, check Model Name
		if ( l_bOk )
		{
			// Check Model Name
			value = pDigitizer->ParameterGetValue(hDigitizer, SvDef::SVGigeParameterModelName);
			if(VT_EMPTY != value.vt)
			{
				std::string modelName = SvUl::createStdString(value);

				const SVDeviceParamWrapper param = rCameraFileDeviceParams.Parameter( DeviceParamModelName );
				l_bOk = modelName == StringValue( param );
			}
		}
	}
	return l_bOk;
}

HRESULT SVTestGigeCameraProxy::GoOnline(unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer)
{
	HRESULT hr = S_OK;

	std::string value( _T( "Enable" ) );

	// Set Trigger and Strobe
	// trigger first, then strobe
	SVDeviceParamWrapper& triggerEnable = GetCameraDeviceParamNonConst( DeviceParamGigeTriggerEnable );
	SVStringValueDeviceParam* pTriggerParam = triggerEnable.DerivedValue( pTriggerParam );
	if(nullptr != pTriggerParam)
	{
		pTriggerParam->strValue = value.c_str();
	
		SetDigitizerParameter(triggerEnable, hDigitizer, pDigitizer );

		SVDeviceParamWrapper& strobeEnable = GetCameraDeviceParamNonConst( DeviceParamGigeStrobeEnable );
		SVStringValueDeviceParam* pStrobeParam = strobeEnable.DerivedValue( pStrobeParam );
		pStrobeParam->strValue = value.c_str();
		SetDigitizerParameter( strobeEnable, hDigitizer, pDigitizer );
	}
	else
	{
		hr = E_FAIL;
	}

	return hr;
}

HRESULT SVTestGigeCameraProxy::GoOffline(unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer)
{
	HRESULT hr = S_OK;

	std::string value( _T( "Disable" ) );

	// Stop Trigger and Strobe 
	// strobe first, then trigger
	SVDeviceParamWrapper& strobeEnable = GetCameraDeviceParamNonConst( DeviceParamGigeStrobeEnable );
	SVStringValueDeviceParam* pStrobeParam = strobeEnable.DerivedValue( pStrobeParam );
	if (nullptr != pStrobeParam)
	{
		pStrobeParam->strValue = value.c_str();
		SetDigitizerParameter( strobeEnable, hDigitizer, pDigitizer );

		SVDeviceParamWrapper& triggerEnable = GetCameraDeviceParamNonConst( DeviceParamGigeTriggerEnable );
		SVStringValueDeviceParam* pTriggerParam = triggerEnable.DerivedValue( pTriggerParam );
		pTriggerParam->strValue = value.c_str();
		SetDigitizerParameter(triggerEnable, hDigitizer, pDigitizer );
	}
	else
	{
		hr = E_FAIL;
	}

	
	return hr;
}

bool SVTestGigeCameraProxy::IsOnline() const
{
	bool bRetVal = false;

	if (m_pAcquisition)
	{
		bRetVal = m_pAcquisition->IsOnline();
	}
	return bRetVal;
}

//
HRESULT SVTestGigeCameraProxy::SetDigitizerParameters( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer )
{
	HRESULT hr = S_OK;
/*SEJ(GIGE)
	// store camera file firmware
	if ( rDeviceParams.ParameterExists( DeviceParamFirmware ) )
	{
		const SVDeviceParamWrapper& w = rDeviceParams.Parameter( DeviceParamFirmware );
		const SVStringValueDeviceParam* psv = w.DerivedValue(psv);
		assert( psv );
		m_strCameraFileFirmware = *psv;
	}
*/
	SVDeviceParamMap::const_iterator iter;
	for ( iter = rDeviceParams.mapParameters.begin(); iter != rDeviceParams.mapParameters.end(); ++iter)
	{
		const SVDeviceParamWrapper& rw = iter->second;
		if ( rw.IsValid() )
		{
			hr = SetDigitizerParameter( rw, hDigitizer, pDigitizer );
		}
	}
	return hr;
}

HRESULT SVTestGigeCameraProxy::SetDigitizerParameter( const SVDeviceParamWrapper& rw, unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer )
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

SVDeviceParamWrapper& SVTestGigeCameraProxy::GetCameraDeviceParamNonConst( SVDeviceParamEnum e )
{
	return m_pAcquisition->m_DeviceParams.GetParameter( e );
}

HRESULT SVTestGigeCameraProxy::SVLUTToSafeArray(const SVLut& lut, _variant_t& output)
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

