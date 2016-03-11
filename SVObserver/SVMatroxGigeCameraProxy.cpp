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

#include "stdafx.h"
#pragma warning (push)
#pragma warning (disable : 4996)
#include <deque>
#include <string>
#include <comdef.h>
#include <boost/config.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/algorithm/string.hpp>
#include "SVMatroxGigeCameraProxy.h"
#include "SVGigeCameraParametersLibrary/SVGigeEnums.h"
#include "SVMessage/SVMessage.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVOMFCLibrary/SVStringValueDeviceParam.h"
#include "SVOMFCLibrary/SVLongValueDeviceParam.h"
#include "SVOMFCLibrary/SVBoolValueDeviceParam.h"
#include "SVOMFCLibrary/SVCustomDeviceParam.h"
#include "SVImageLibrary/SVDigitizerLoadLibraryClass.h"
#include "SVMatroxGigeAcquisitionClass.h"
#include "ObjectInterfaces/ErrorNumbers.h"
#include "TextDefinesSvO.h"
#pragma warning (pop)

typedef std::map<SVDeviceParamEnum, SVGigeParameterEnum> DeviceParamToGigeParamAssoc;
static DeviceParamToGigeParamAssoc DeviceParamEnumToGigeParamEnum = boost::assign::map_list_of<SVDeviceParamEnum, SVGigeParameterEnum>
(DeviceParamShutter, SVGigeParameterFeatureShutter)
(DeviceParamGamma, SVGigeParameterFeatureGamma)
(DeviceParamBrightness, SVGigeParameterFeatureBrightness)
(DeviceParamGain, SVGigeParameterFeatureGain)
(DeviceParamWhiteBalanceUB, SVGigeParameterFeatureWhiteBalanceU)
(DeviceParamWhiteBalanceVR, SVGigeParameterFeatureWhiteBalanceV)
(DeviceParamSharpness, SVGigeParameterFeatureSharpness)
(DeviceParamHue, SVGigeParameterFeatureHue)
(DeviceParamSaturation, SVGigeParameterFeatureSaturation)
(DeviceParamGigeTriggerSource, SVGigeParameterTriggerSource)
(DeviceParamGigeTriggerEdge, SVGigeParameterTriggerEdge)
(DeviceParamGigeTriggerEnable, SVGigeParameterTriggerEnable)
(DeviceParamGigeTriggerLine, SVGigeParameterTriggerLine)
(DeviceParamGigeStrobeSource, SVGigeParameterStrobeSource)
(DeviceParamGigeStrobeEdge, SVGigeParameterStrobeEdge)
(DeviceParamGigeStrobeEnable, SVGigeParameterStrobeEnable)
(DeviceParamStrobePulseDelay, SVGigeParameterStrobeDelay)
(DeviceParamStrobePulseDuration, SVGigeParameterStrobeDuration)
(DeviceParamGigeStrobeLine, SVGigeParameterStrobeLine)
(DeviceParamGigePacketSize, SVGigeParameterPacketSize)
(DeviceParamGigeCustom1, SVGigeParameterCustom1)
(DeviceParamGigeCustom2, SVGigeParameterCustom2)
(DeviceParamGigeCustom3, SVGigeParameterCustom3)
(DeviceParamGigeCustom4, SVGigeParameterCustom4)
(DeviceParamGigeCustom5, SVGigeParameterCustom5)
(DeviceParamGigeCustom6, SVGigeParameterCustom6)
(DeviceParamGigeCustom7, SVGigeParameterCustom7)
(DeviceParamGigeCustom8, SVGigeParameterCustom8)
(DeviceParamGigeCustom9, SVGigeParameterCustom9)
(DeviceParamGigeCustom10, SVGigeParameterCustom10)
(DeviceParamGigeCustom11, SVGigeParameterCustom11)
(DeviceParamGigeCustom12, SVGigeParameterCustom12)
(DeviceParamGigeCustom13, SVGigeParameterCustom13)
(DeviceParamGigeCustom14, SVGigeParameterCustom14)
(DeviceParamGigeCustom15, SVGigeParameterCustom15)
(DeviceParamGigeCustom16, SVGigeParameterCustom16)
(DeviceParamGigeCustom17, SVGigeParameterCustom17)
(DeviceParamGigeCustom18, SVGigeParameterCustom18)
(DeviceParamGigeCustom19, SVGigeParameterCustom19)
(DeviceParamGigeCustom20, SVGigeParameterCustom20)
(DeviceParamGigeCustom21, SVGigeParameterCustom21)
(DeviceParamGigeCustom22, SVGigeParameterCustom22)
(DeviceParamGigeCustom23, SVGigeParameterCustom23)
(DeviceParamGigeCustom24, SVGigeParameterCustom24)
(DeviceParamGigeCustom25, SVGigeParameterCustom25)
(DeviceParamGigeCustom26, SVGigeParameterCustom26)
(DeviceParamGigeCustom27, SVGigeParameterCustom27)
(DeviceParamGigeCustom28, SVGigeParameterCustom28)
(DeviceParamGigeCustom29, SVGigeParameterCustom29)
(DeviceParamGigeCustom30, SVGigeParameterCustom30)
(DeviceParamHorizontalBinning, SVGigeParameterHorizontalBinning)
(DeviceParamVerticalBinning, SVGigeParameterVerticalBinning)
;

SVMatroxGigeCameraProxy::SVMatroxGigeCameraProxy()
{
	m_pAcquisition = NULL;
}

SVMatroxGigeCameraProxy::~SVMatroxGigeCameraProxy()
{
}

void SVMatroxGigeCameraProxy::SetOwner(SVMatroxGigeAcquisitionClass* pAcquisition)
{
	m_pAcquisition = pAcquisition;
}

HRESULT SVMatroxGigeCameraProxy::InitializeDevice( const SVDeviceParamWrapper& rwParam, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer )
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
				hr = SetDigitizerParameter( rwParam, hDigitizer, pDigitizer );
			}
		}
	}
	return hr;
}

HRESULT SVMatroxGigeCameraProxy::InitializeDevice( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer )
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
			hr = SetDigitizerParameters( rDeviceParams, hDigitizer, pDigitizer );
		}
	}
	return hr;
}

HRESULT SVMatroxGigeCameraProxy::SetGigeFeatureOverrides(const SVString& xmlData, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer)
{
	HRESULT hr = S_OK;

	if (pDigitizer != NULL && hDigitizer != NULL)
	{
		_variant_t l_oValue = xmlData.c_str(); // does this convert from ansi to wide ?
		hr = pDigitizer->ParameterSetValue(hDigitizer, SVGigeParameterFeatureOverrides, 0, &l_oValue);
	}
	return hr;
}

// this is where the SV Camera Device Parameters are mapped to the Gige Parameter defines (mostly)
// exceptions to this are the Internal ones for things like Trigger and Strobe...
// SEJ - all parameters must pass through here to translate DeviceParam to GigeParamter tags
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

HRESULT SVMatroxGigeCameraProxy::SetStandardCameraParameter( const SVDeviceParamWrapper& rw, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer )
{
	HRESULT hr = S_OK;

	if ( pDigitizer != NULL && hDigitizer != NULL )
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

			#pragma region DeviceParamSerialNumberString
			case DeviceParamSerialNumberString:
				break;
			#pragma endregion

			#pragma region DeviceParamShutter
			case DeviceParamShutter:
			{
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					l_oValue = pParam->lValue;

					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterFeatureShutter, 0, &l_oValue );
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
					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterFeatureGamma, 0, &l_oValue );
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
					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterFeatureBrightness, 0, &l_oValue );
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
					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterFeatureGain, 0, &l_oValue );
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
					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterFeatureWhiteBalanceU, 0, &l_oValue );
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
					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterFeatureWhiteBalanceV, 0, &l_oValue );
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
					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterFeatureSharpness, 0, &l_oValue );
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
					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterFeatureHue, 0, &l_oValue );
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
					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterFeatureSaturation, 0, &l_oValue );
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
					hr = SV_FALSE;
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

					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterTriggerSource, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamAcquisitionTriggerEdge
			// This parameter doesn't get sent over to the DLL
			case DeviceParamAcquisitionTriggerEdge:
			{
				/*
				const SVBoolValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					l_oValue = pParam->bValue;
					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterTriggerEdge, 0, &l_oValue );
				}
				*/
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

					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterTriggerEdge, 0, &l_oValue );
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

					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterTriggerEnable, 0, &l_oValue );
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

					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterTriggerDelay, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamIOTriggerInvert
			// This parameter doesn't get sent over to the DLL
			case DeviceParamIOTriggerInvert:
			{
				/*
				const SVBoolValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					l_oValue = pParam->bValue;
					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterTriggerInvert, 0, &l_oValue );
				}
				*/
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

					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterTriggerLine, 0, &l_oValue );
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

					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterStrobeSource, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamAcquisitionStrobeEdge
			// This parameter doesn't get sent over to the DLL
			case DeviceParamAcquisitionStrobeEdge:
			{
				/*
				const SVBoolValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					l_oValue = pParam->bValue;
					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterStrobeEdge, 0, &l_oValue );
				}
				*/
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

					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterStrobeEdge, 0, &l_oValue );
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

					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterStrobeEnable, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamIOStrobeInvert
			// This parameter doesn't get sent over to the DLL
			case DeviceParamIOStrobeInvert:
			{
				/*
				const SVBoolValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					l_oValue = pParam->bValue;
					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterStrobeInvert, 0, &l_oValue );
				}
				*/
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

					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterStrobeDelay, 0, &l_oValue );
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

					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterStrobeDuration, 0, &l_oValue );
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

					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterStrobeLine, 0, &l_oValue );
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

					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterPacketSize, 0, &l_oValue );
				}
				break;
			}
			#pragma endregion

			#pragma region DeviceParamLut
			case DeviceParamLut:
			{
				const SVLutDeviceParam* pLutParam = rw.DerivedValue( pLutParam );
				ASSERT( pLutParam );

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
					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterLutEnable, 0, &EnableLUTVariant );
					if (S_OK == hr  && EnableLUT )
					{
						// Set LUT values
						hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterLutArray, 0, &l_oValue );
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

					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterLineInput, 0, &l_oValue );
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
					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterVerticalBinning, 0, &l_oValue );
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

					hr = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterHorizontalBinning, 0, &l_oValue );
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
						if (hr == S_OK)
						{
							hr = pDigitizer->ParameterSetValue(hDigitizer, DeviceParamEnumToGigeParamEnum[l_eType], 0, &l_oValue);
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

	if( hr == S_OK )
	{
		rDeviceParams.SetParameter( rw );
	}
	return hr;
}

HRESULT SVMatroxGigeCameraProxy::SetCameraFormatParameters(unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer, const SVCameraFormatsDeviceParam* pParam)
{
	HRESULT hr = S_OK;
	HRESULT l_Temp = S_OK;
	long horizontalBinning = 1;
	long verticalBinning = 1;
	const SVCameraFormat& rcf = pParam->options.find(pParam->strValue)->second;
	long l_Width = rcf.lWidth;
	long l_Height = rcf.lHeight;

	_variant_t l_oValue;
	int l_ParType;
	bool l_bOffsetsFirst = false;

	// Get Binning parameters
	l_Temp = pDigitizer->ParameterGetValue( hDigitizer, SVGigeParameterHorizontalBinning, &l_ParType, &l_oValue);
	if (l_Temp == S_OK)
	{
		if (l_oValue.lVal > 0)
		{
			horizontalBinning = l_oValue.lVal;
		}
	}
	l_Temp = pDigitizer->ParameterGetValue( hDigitizer, SVGigeParameterVerticalBinning, &l_ParType, &l_oValue);
	if (l_Temp == S_OK)
	{
		if (l_oValue.lVal > 0)
		{
			verticalBinning = l_oValue.lVal;
		}
	}
	// Check if width/height exceeds binned width/height and adjust
	// Must be a multiple of step size
	long maxWidth = rcf.lWidthMax / horizontalBinning;
	if (rcf.lHPosStep > 1)
	{
		maxWidth -= (maxWidth % rcf.lHPosStep);
	}
	long maxHeight = rcf.lHeightMax / verticalBinning;
	if (rcf.lVPosStep > 1)
	{
		maxHeight -= (maxHeight % rcf.lVPosStep);
	}

	if (l_Height > maxHeight)
	{
		l_Height = maxHeight;
	}
	if (l_Width > maxWidth)
	{
		l_Width = maxWidth;
	}

	// if Height and Width are set to max then the offsets must be cleared first.
	if( maxHeight == l_Height && maxWidth == l_Width)
	{
		l_bOffsetsFirst = true;
	}

	long l_XPrevSize=0;
	long l_YPrevSize=0;
	long l_XPrevOffset=0;
	long l_YPrevOffset=0;

	// Read previous parameters so we can make an intelligent decision on which parameters to set first.
	l_Temp = pDigitizer->ParameterGetValue( hDigitizer, SVGigeParameterXSize, &l_ParType, &l_oValue);
	if( l_Temp == S_OK ) 
	{ 
		l_XPrevSize = l_oValue.lVal;
		l_oValue.Clear();
	}
	l_Temp = pDigitizer->ParameterGetValue( hDigitizer, SVGigeParameterYSize, &l_ParType, &l_oValue);
	if( l_Temp == S_OK ) 
	{ 
		l_YPrevSize = l_oValue.lVal;
		l_oValue.Clear();
	}
	l_Temp = pDigitizer->ParameterGetValue( hDigitizer, SVGigeParameterXOffset, &l_ParType, &l_oValue);
	if( l_Temp == S_OK ) 
	{ 
		l_XPrevOffset = l_oValue.lVal;
		l_oValue.Clear();
	}
	l_Temp = pDigitizer->ParameterGetValue( hDigitizer, SVGigeParameterYOffset, &l_ParType, &l_oValue);
	if( l_Temp == S_OK ) 
	{ 
		l_YPrevOffset = l_oValue.lVal;
		l_oValue.Clear();
	}

	// Decide if we should adjust Width or X offset first to prevent errors.
	if( l_XPrevSize - l_Width > l_XPrevOffset - rcf.lHPos)
	{
		l_oValue = l_Width;
		l_Temp = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterXSize, 0, &l_oValue );
		l_oValue.Clear();

		if( hr == S_OK && l_Temp != S_OK )
		{
			hr = l_Temp;
		}
		l_oValue = rcf.lHPos;
		l_Temp = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterXOffset, 0, &l_oValue );
		l_oValue.Clear();

		if( hr == S_OK && l_Temp != S_OK )
		{
			hr = l_Temp;
		}
	}
	else
	{
		l_oValue = rcf.lHPos;
		l_Temp = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterXOffset, 0, &l_oValue );
		l_oValue.Clear();

		if( hr == S_OK && l_Temp != S_OK )
		{
			hr = l_Temp;
		}
		l_oValue = l_Width;
		l_Temp = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterXSize, 0, &l_oValue );
		l_oValue.Clear();

		if( hr == S_OK && l_Temp != S_OK )
		{
			hr = l_Temp;
		}
	}

	// Decide if we should adjust Height or Y offset first to prevent errors.
	if( l_YPrevSize - l_Height > l_YPrevOffset - rcf.lVPos)
	{
		l_oValue = l_Height;
		l_Temp = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterYSize, 0, &l_oValue );
		l_oValue.Clear();

		if( hr == S_OK && l_Temp != S_OK )
		{
			hr = l_Temp;
		}

		l_oValue = rcf.lVPos;
		l_Temp = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterYOffset, 0, &l_oValue );
		l_oValue.Clear();

		if( hr == S_OK && l_Temp != S_OK )
		{
			hr = l_Temp;
		}
	}
	else
	{
		l_oValue = rcf.lVPos;
		l_Temp = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterYOffset, 0, &l_oValue );
		l_oValue.Clear();

		if( hr == S_OK && l_Temp != S_OK )
		{
			hr = l_Temp;
		}

		l_oValue = l_Height;
		l_Temp = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterYSize, 0, &l_oValue );
		l_oValue.Clear();

		if( hr == S_OK && l_Temp != S_OK )
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
		l_Temp = pDigitizer->ParameterSetValue( hDigitizer, SVGigeParameterColorFormat, 0, &l_oValue );
		l_oValue.Clear();
	}
	if( hr == S_OK && FAILED( l_Temp ) ) // This is allowed to be S_FALSE
	{
		hr = l_Temp;
	}
	return hr;
}

HRESULT SVMatroxGigeCameraProxy::SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer )
{
	HRESULT hr = S_OK;
	// SEJ - send notification to start tracking main camera parameters
	if ( pDigitizer != NULL )
	{
		_variant_t dummy;
		hr = pDigitizer->ParameterSetValue(hDigitizer, SVGigeBeginTrackParameters, 0, &dummy);
	}
	if ( hr == S_OK )
	{
		hr = SVGigeCameraProxy::SetDeviceParameters( rDeviceParams, m_pAcquisition->m_DeviceParams );
	}
	if ( hr == S_OK )
	{
		hr = InitializeDevice( rDeviceParams, hDigitizer, pDigitizer );
	}
	return hr;
}

HRESULT SVMatroxGigeCameraProxy::IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer)
{
	if( hDigitizer != NULL && pDigitizer != NULL &&
		rDeviceParams.ParameterExists( DeviceParamVendorName ) )
	{
		_variant_t l_oValue;

		if( pDigitizer != NULL && pDigitizer->ParameterGetValue( hDigitizer, SVGigeParameterVendorName, 0, &l_oValue ) == S_OK )
		{
			SVString venderNameHardware = SvUl_SF::createSVString( l_oValue.bstrVal );
			SVString venderName( StringValue( rDeviceParams.Parameter( DeviceParamVendorName ) ) );

			if( venderNameHardware == venderName )
			{
				if( rDeviceParams.ParameterExists( DeviceParamModelName ) &&
					pDigitizer->ParameterGetValue( hDigitizer, SVGigeParameterModelName, 0, &l_oValue ) == S_OK )
				{
					CString sHardwareModel( l_oValue.bstrVal );

					CString sModel = StringValue(rDeviceParams.Parameter( DeviceParamModelName )).c_str();

					if ( sHardwareModel != _T("") && sModel != _T("") && sHardwareModel.CompareNoCase(sModel) != 0 )
					{
						SVString Message = SvUl_SF::Format(SvO::Error_WrongCameraModel, sModel, sHardwareModel);
						SvStl::MessageMgrNoDisplay Exception(SvStl::DataOnly);
						Exception.setMessage( SVMSG_SVO_87_GOONLINE_CAMERA_ERROR, Message.c_str(), StdMessageParams, SvOi::Err_10026_GoOnline_WrongCameraModel );
						Exception.Throw();
					}
				}
			}
			else
			{
				SVString Message = SvUl_SF::Format(SvO::Error_WrongCameraVendor, venderName.c_str(), venderNameHardware.c_str());
				SvStl::MessageMgrNoDisplay Exception(SvStl::DataOnly);
				Exception.setMessage( SVMSG_SVO_87_GOONLINE_CAMERA_ERROR, Message.c_str(), StdMessageParams, SvOi::Err_10027_GoOnline_WrongCameraVender );
				Exception.Throw();
			}
		}
	}
	return S_OK;
}

bool SVMatroxGigeCameraProxy::CameraMatchesCameraFile(const SVDeviceParamCollection& rCameraFileDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer)
{
	bool l_bOk = true;
	
	if( hDigitizer != NULL && pDigitizer != NULL &&
		rCameraFileDeviceParams.ParameterExists( DeviceParamVendorName ) )
	{
		_variant_t l_oValue;

		// Check Vendor Name
		if( pDigitizer->ParameterGetValue( hDigitizer, SVGigeParameterVendorName, 0, &l_oValue ) == S_OK )
		{
			SVString l_csVenderName = SvUl_SF::createSVString(l_oValue.bstrVal);

			const SVDeviceParamWrapper param = rCameraFileDeviceParams.Parameter( DeviceParamVendorName );
			l_bOk = l_csVenderName == StringValue( param );
		}
		// if Vendor Name matched, check Model Name
		if ( l_bOk )
		{
			// Check Model Name
			if( pDigitizer->ParameterGetValue( hDigitizer, SVGigeParameterModelName, 0, &l_oValue ) == S_OK )
			{
				SVString l_csModelName = SvUl_SF::createSVString(l_oValue.bstrVal);

				const SVDeviceParamWrapper param = rCameraFileDeviceParams.Parameter( DeviceParamModelName );
				l_bOk = l_csModelName == StringValue( param );
			}
		}
	}
	return l_bOk;
}

HRESULT SVMatroxGigeCameraProxy::GoOnline(unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer)
{
	HRESULT hr = S_OK;

	SVString value( _T( "Enable" ) );

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

HRESULT SVMatroxGigeCameraProxy::GoOffline(unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer)
{
	HRESULT hr = S_OK;

	SVString value( _T( "Disable" ) );

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
HRESULT SVMatroxGigeCameraProxy::SetDigitizerParameters( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer )
{
	HRESULT hr = S_OK;
/*SEJ(GIGE)
	// store camera file firmware
	if ( rDeviceParams.ParameterExists( DeviceParamFirmware ) )
	{
		const SVDeviceParamWrapper& w = rDeviceParams.Parameter( DeviceParamFirmware );
		const SVStringValueDeviceParam* psv = w.DerivedValue(psv);
		ASSERT( psv );
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

HRESULT SVMatroxGigeCameraProxy::SetDigitizerParameter( const SVDeviceParamWrapper& rw, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer )
{
	HRESULT hr = S_FALSE;

	if (pDigitizer != NULL)
	{
		const SVDeviceParam* pParam = (const SVDeviceParam*) rw;
		if ( pParam != NULL )
		{
			// Translate DeviceParameterEnum to GigeParameterEnum
			hr = SetStandardCameraParameter( rw, m_pAcquisition->m_DeviceParams, hDigitizer, pDigitizer );
		}
	}
	return hr;
}

// Get the Device Parameter from the Parameter Collection in the Acquisition Class 
// (As opposed to the Real Acquisition DLL)
const SVDeviceParamWrapper& SVMatroxGigeCameraProxy::GetCameraDeviceParam( SVDeviceParamEnum e ) const
{
	return m_pAcquisition->m_DeviceParams.Parameter( e );
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
HRESULT SVMatroxGigeCameraProxy::EnableSoftwareTrigger(unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer)
{
	return pDigitizer->InternalTriggerEnable(hDigitizer);
}

// For SingleGrabHelper
HRESULT SVMatroxGigeCameraProxy::SoftwareTrigger(unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer)
{
	return pDigitizer->InternalTrigger(hDigitizer);
}

HRESULT SVMatroxGigeCameraProxy::SVLUTToSafeArray(const SVLut& lut, _variant_t& output)
{
	SAFEARRAY* psaBandData(NULL);

	bool bRetVal = lut.GetBandData(psaBandData);
	if (bRetVal)
	{
		output.vt = VT_I4 | VT_ARRAY;
		output.parray = psaBandData;
	}
	return (bRetVal) ? S_OK : S_FALSE;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVMatroxGigeCameraProxy.cpp_v  $
 * 
 *    Rev 1.4   03 Dec 2014 19:51:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  974
 * SCR Title:  Revise Gain handling for Baumer cameras (SVO-401)
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised SetStandardCameraParameter to call GetNormalizedValue for all Light Reference (SVLongDeviceParam) values to use the unit_divisor if specified via the camera file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   18 Jun 2013 19:23:10   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 019.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   31 May 2013 10:57:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  828
 * SCR Title:  Camera Binning
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised SetCameraFormatParameters to ensure the binned width and height are a multiple of the respective step size.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Jun 2013 15:26:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   21 May 2013 14:34:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  828
 * SCR Title:  Camera Binning
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed dead code
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   20 May 2013 10:49:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  828
 * SCR Title:  Camera Binning
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised SetCameraFormatParameters to correct an issue where the setting of the pixel format fails
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   16 May 2013 18:28:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  828
 * SCR Title:  Camera Binning
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to fix binning when applied to a single axis (both axises were binned).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 May 2013 19:46:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   10 May 2013 10:26:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  828
 * SCR Title:  Camera Binning
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added support for binning
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:33:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   01 Apr 2013 16:05:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised SetStandardCameraParameter to support custom device parameters
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   21 Jan 2013 11:08:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added support for DeviceParamCameraInput
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   04 Oct 2012 10:45:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   29 Jun 2012 16:52:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  772
 * SCR Title:  Implement GigE Camera Reconnect Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to add the needed functionality to track parameter information and reset the camera to the desired parameters on reconnect.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   17 Apr 2012 11:38:04   tBair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  758
 * SCR Title:  Change ROI Parameter Order for GigE cameras
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified logic to set Width, Height, and Offsets in SetCameraFormatParameters.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   22 Mar 2012 10:55:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  759
 * SCR Title:  Fix Gige camera parameter Shutter Delay
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified SetStandardCameraParameter to assign the value of ShutterDelay.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   22 Mar 2012 10:52:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  758
 * SCR Title:  Change ROI Parameter Order for GigE cameras
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Change order of parameter setting for width, height, XOffset, and YOffset in SetCameraFormatParameters
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   23 Aug 2011 15:47:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  735
 * SCR Title:  Add Gige Color System
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added code to set 'Pixel Format' in SetCameraFormatParameters function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   18 Feb 2011 09:58:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to remove duplicate string class, and fixed string conversion issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   13 Dec 2010 11:25:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Change the usage of _variant_t to clear the variable after use to eliminate invalid variant type for assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   07 Jul 2010 16:10:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to make Single Grab functionality work for IEEE1394 and GigE.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Jun 2010 08:27:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes to allow for GigE Light Reference and LUT to work.  Added functionality to read GigE camera database from camera file to allow for changes to camera functionality to be externalized.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Dec 2009 12:00:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Oct 2009 19:32:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix issues with const correctness.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:50:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
