// ******************************************************************************
// * COPYRIGHT (c) 2002 by K�rber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDeviceParams
// * .File Name       : $Workfile:   SVDeviceParams.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   15 May 2013 19:42:28  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVDeviceParams.h"
#include "SVNullDeviceParam.h"
#include "SVBoolValueDeviceParam.h"
#include "SVi64ValueDeviceParam.h"
#include "SVLongValueDeviceParam.h"
#include "SVStringValueDeviceParam.h"
#include "SVDeviceParamCollection.h"
#include "SVCustomDeviceParam.h"
#include "SVParamListDeviceParam.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace	// create unnamed namespace for the variables to live in
{
	REGISTER_DEVICE_PARAM( DeviceParamInvalid,             DeviceParamInvalid_String,                 SVNullDeviceParam );
	const bool bRegisterDefault = TheDeviceParamFactory::Instance().SetDefault( DeviceParamInvalid );

	REGISTER_DEVICE_PARAM( DeviceParamDeviceDigNum,        DeviceParamDeviceDigNum_String,            SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamDeviceName,          DeviceParamDeviceName_String,              SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamDeviceChannelName,   DeviceParamDeviceChannelName_String,       SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamVendorId,            DeviceParamVendorId_String,                SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamVendorName,          DeviceParamVendorName_String,              SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamModelName,           DeviceParamModelName_String,               SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamFirmware,            DeviceParamFirmware_String,                SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamIPAddress,           DeviceParamIPAddress_String,               SVStringValueDeviceParam );

	REGISTER_DEVICE_PARAM( DeviceParamSerialNumber,        DeviceParamSerialNumber_String,            SVi64ValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamSerialNumberString,  DeviceParamSerialNumberString_String,      SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamShutter,             DeviceParamShutter_String,                 SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGamma,               DeviceParamGamma_String,                   SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamBrightness,          DeviceParamBrightness_String,              SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGain,                DeviceParamGain_String,                    SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamWhiteBalanceUB,      DeviceParamWhiteBalanceUB_String,          SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamWhiteBalanceVR,      DeviceParamWhiteBalanceVR_String,          SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamSharpness,           DeviceParamSharpness_String,               SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamHue,                 DeviceParamHue_String,                     SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamSaturation,          DeviceParamSaturation_String,              SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamRegisters,           DeviceParamRegisters_String,               SVParamListDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamSizeBands,           DeviceParamSizeBands_String,               SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamFormatStrings,       DeviceParamFormatStrings_String,           SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamFormat7Support,      DeviceParamFormat7Support_String,          SVBoolValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamTriggerEdge,         DeviceParamTriggerEdge_String,             SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamOutputStrobePulse,   DeviceParamOutputStrobePulse_String,       SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamStrobePulseDelay,    DeviceParamStrobePulseDelay_String,        SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamStrobePulseDuration, DeviceParamStrobePulseDuration_String,     SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamStrobePulseMultiplier, DeviceParamStrobePulseMultiplier_String, SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamCameraTriggerPin,    DeviceParamCameraTriggerPin_String,        SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamCameraStrobePin,     DeviceParamCameraStrobePin_String,         SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamExposure,            DeviceParamExposure_String,                SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamTriggerMode,         DeviceParamTriggerMode_String,             SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamTestPattern,         DeviceParamTestPattern_String,             SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamShutterDelay,        DeviceParamShutterDelay_String,            SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamStrobeStartMode,     DeviceParamStrobeStartMode_String,         SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamTriggerQueueMode,    DeviceParamTriggerQueueMode_String,        SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamStrobePulseAdvance,  DeviceParamStrobePulseAdvance_String,      SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamNumCameraBuffers,    DeviceParamNumCameraBuffers_String,        SVLongValueDeviceParam );
	
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom1,	DeviceParamGigeCustom1_String,  SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom2,	DeviceParamGigeCustom2_String,  SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom3,	DeviceParamGigeCustom3_String,  SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom4,	DeviceParamGigeCustom4_String,  SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom5,	DeviceParamGigeCustom5_String,  SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom6,	DeviceParamGigeCustom6_String,  SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom7,	DeviceParamGigeCustom7_String,  SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom8,	DeviceParamGigeCustom8_String,  SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom9,	DeviceParamGigeCustom9_String,  SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom10,	DeviceParamGigeCustom10_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom11,	DeviceParamGigeCustom11_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom12,	DeviceParamGigeCustom12_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom13,	DeviceParamGigeCustom13_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom14,	DeviceParamGigeCustom14_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom15,	DeviceParamGigeCustom15_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom16,	DeviceParamGigeCustom16_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom17,	DeviceParamGigeCustom17_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom18,	DeviceParamGigeCustom18_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom19,	DeviceParamGigeCustom19_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom20,	DeviceParamGigeCustom20_String,	SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom21,	DeviceParamGigeCustom21_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom22,	DeviceParamGigeCustom22_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom23,	DeviceParamGigeCustom23_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom24,	DeviceParamGigeCustom24_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom25,	DeviceParamGigeCustom25_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom26,	DeviceParamGigeCustom26_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom27,	DeviceParamGigeCustom27_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom28,	DeviceParamGigeCustom28_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom29,	DeviceParamGigeCustom29_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom30,	DeviceParamGigeCustom30_String,	SVCustomDeviceParam );

	REGISTER_DEVICE_PARAM( DeviceParamAcquisitionTriggerSelection, DeviceParamAcquisitionTriggerSelection_String, SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamAcquisitionTriggerType,      DeviceParamAcquisitionTriggerType_String,      SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamAcquisitionTriggerName,      DeviceParamAcquisitionTriggerName_String,      SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamAcquisitionTriggerEdge,      DeviceParamAcquisitionTriggerEdge_String,      SVBoolValueDeviceParam );

	REGISTER_DEVICE_PARAM( DeviceParamAcquisitionStrobeEdge,       DeviceParamAcquisitionStrobeEdge_String,       SVBoolValueDeviceParam );

	REGISTER_DEVICE_PARAM( DeviceParamIOStrobeInvert,              DeviceParamIOStrobeInvert_String,              SVBoolValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamIOTriggerInvert,             DeviceParamIOTriggerInvert_String,             SVBoolValueDeviceParam );
	
	REGISTER_DEVICE_PARAM( DeviceParamCameraTriggerMode, DeviceParamCameraTriggerMode_String, SVLongValueDeviceParam );
	
	REGISTER_DEVICE_PARAM( DeviceParamCameraInput, DeviceParamCameraInput_String, SVStringValueDeviceParam );

	// File Acquisition parameters
	REGISTER_DEVICE_PARAM( DeviceParamFileAcqImageFileName, DeviceParamFileAcqImageFileName_String, SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamFileAcqImageDirName,  DeviceParamFileAcqImageDirName_String,  SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamFileAcqLoadingMode,   DeviceParamFileAcqLoadingMode_String,   SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamFileAcqImageWidth,    DeviceParamFileAcqImageWidth_String,    SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamFileAcqImageHeight,   DeviceParamFileAcqImageHeight_String,   SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamFileAcqImageFormat,   DeviceParamFileAcqImageFormat_String,   SVLongValueDeviceParam );

	// GIGE Acquisition parameters
	REGISTER_DEVICE_PARAM( DeviceParamGigeTriggerSource,	DeviceParamAcquisitionTriggerType_String,	SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeTriggerEdge,		DeviceParamTriggerEdge_String,				SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeTriggerLine,		DeviceParamGigeTriggerLine_String,			SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeTriggerEnable,	DeviceParamGigeTriggerEnable_String,		SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeStrobeSource,		DeviceParamGigeStrobeSource_String,			SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeStrobeEdge,		DeviceParamGigeStrobeEdge_String,			SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeStrobeLine,		DeviceParamGigeStrobeLine_String,			SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeStrobeEnable,		DeviceParamGigeStrobeEnable_String,			SVStringValueDeviceParam );
	
	REGISTER_DEVICE_PARAM( DeviceParamGigePacketSize,		DeviceParamGigePacketSize_String,			SVLongValueDeviceParam );

	REGISTER_DEVICE_PARAM( DeviceParamHorizontalBinning,	DeviceParamHorizontalBinning_String, 		SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamVerticalBinning,		DeviceParamVerticalBinning_String, 			SVLongValueDeviceParam );

	REGISTER_DEVICE_PARAM( DeviceParamCameraDefaultSettings,DeviceParamCameraDefaultSettings_String,	SVStringValueDeviceParam );
}// end unnamed namespace

// these conversion routines can be moved to another file
HRESULT ToVariant( long lValue, VARIANT& rv )
{
	HRESULT hr = S_OK;

	::VariantClear( &rv );

	rv.vt = VT_I4;
	rv.lVal = lValue;

	return hr;
}

HRESULT ToVariant( __int64 iValue, VARIANT& rv )
{
	HRESULT hr = S_OK;

	::VariantClear( &rv );

	// no real support for VT_I8 yet; stuff into a string; see also FromVariant
	rv.vt = VT_BSTR;
	rv.bstrVal = _bstr_t(std::format("{:d}", iValue).c_str()).Detach();

	return hr;
}

HRESULT ToVariant( bool bValue, VARIANT& rv )
{
	HRESULT hr = S_OK;

	::VariantClear( &rv );

	rv.vt = VT_BOOL;
	rv.boolVal = bValue ? VARIANT_TRUE : VARIANT_FALSE;

	return hr;
}

HRESULT ToVariant( const std::string& sValue, VARIANT& rv )
{
	HRESULT hr = S_OK;

	::VariantClear( &rv );

	rv.vt = VT_BSTR;
	rv.bstrVal = _bstr_t(sValue.c_str()).Detach();

	return hr;
}

HRESULT FromVariant( long& rlValue, const VARIANT& rv )
{
	HRESULT hr = S_OK;

	switch ( rv.vt )
	{
		case VT_I4:
			rlValue = rv.lVal;
			break;
		case VT_UI4:
			rlValue = (long) rv.ulVal;
			break;
		case VT_I2:
			rlValue = (long) rv.iVal;
			break;
		case VT_UI2:
			rlValue = (long) rv.uiVal;
			break;
		default:
			hr = S_FALSE;
	}

	return hr;
}

HRESULT FromVariant( __int64& riValue, const VARIANT& rv )
{
	HRESULT hr = S_OK;

	switch ( rv.vt )
	{
		// no real support for VT_I8 yet; we'll use strings for now; see also ToVariant
		case VT_I8:
			hr = S_FALSE;
			break;
		case VT_UI8:
			hr = S_FALSE;
			break;
		case VT_BSTR:
			{
				if (wcslen(rv.bstrVal) > 0 )
				{
					swscanf(rv.bstrVal, L"%I64d", &riValue);
				}
				else
				{
					hr = S_FALSE;
				}
			}
			break;
		default:
			hr = S_FALSE;
	}

	return hr;
}

HRESULT FromVariant( bool& rbValue, const VARIANT& rv )
{
	HRESULT hr = S_OK;

	switch ( rv.vt )
	{
		case VT_BOOL:
			rbValue = rv.boolVal != 0;
		case VT_I4:
			rbValue = rv.lVal != 0;
			break;
		case VT_UI4:
			rbValue = rv.ulVal != 0;
			break;
		case VT_I2:
			rbValue = rv.iVal != 0;
			break;
		case VT_UI2:
			rbValue = rv.uiVal != 0;
			break;
		default:
			hr = S_FALSE;
	}

	return hr;
}

HRESULT FromVariant( std::string& rsValue, const VARIANT& rv )
{
	HRESULT hr = S_OK;

	switch ( rv.vt )
	{
		case VT_BSTR:
			{
				rsValue = SvUl::createStdString( _bstr_t( rv.bstrVal ) );
			}
			break;
		default:
			hr = S_FALSE;
	}

	return hr;
}

///////////////////////////////////////////////////////////////////////////////////

// long Specialization TDeviceParamInfo Implementation ....
TDeviceParamInfo<long>::TDeviceParamInfo(const TDeviceParamInfo<long>& rhs)
{
	*this=rhs;
}

const TDeviceParamInfo<long>& TDeviceParamInfo<long>::operator= (const TDeviceParamInfo<long>& rhs)
{
	if ( this != &rhs )
	{
		vendorId = rhs.vendorId;
		mask = rhs.mask;
		options = rhs.options;
		min = rhs.min;
		max = rhs.max;
		offset = rhs.offset;
		multiplier = rhs.multiplier;
		unit_divisor = rhs.unit_divisor;
		sUnits = rhs.sUnits;
	}
	return *this;
}

// __in64 Specialization TDeviceParamInfo Implementation ....
TDeviceParamInfo<__int64>::TDeviceParamInfo(const TDeviceParamInfo<__int64>& rhs)
{
	*this=rhs;
}

const TDeviceParamInfo<__int64>& TDeviceParamInfo<__int64>::operator= (const TDeviceParamInfo<__int64>& rhs)
{
	if ( this != &rhs )
	{
		options = rhs.options;
		min = rhs.min;
		max = rhs.max;
		offset = rhs.offset;
		multiplier = rhs.multiplier;
		sUnits = rhs.sUnits;
	}
	return *this;
}

///////////////////////////////////////////////////////////////////////////////////

// **********************************************
HRESULT SVDeviceParam::SetMetadata(const SVDeviceParam* )
{
	HRESULT hr = E_NOTIMPL;

	return hr;
}

