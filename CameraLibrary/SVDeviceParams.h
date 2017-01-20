// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDeviceParams
// * .File Name       : $Workfile:   SVDeviceParams.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   15 May 2013 19:42:32  $
// ******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <algorithm>
#include "SVDeviceParamCollection.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

class SVDeviceParamStructTestCases
{
public:
	SVDeviceParamStructTestCases( SVDeviceParamCollection& rDeviceStruct );
};

#define DEVICE_PARAM_CREATE_FN(x) x##::CreateNew
#define REGISTER_DEVICE_PARAM(x,y,fn) const bool bRegister##x = TheDeviceParamFactory::Instance().Register(x, y, DEVICE_PARAM_CREATE_FN(fn));

enum SVDeviceParamDataTypeEnum
{
	DeviceDataTypeLong,
	DeviceDataTypei64,
	DeviceDataTypeBool,
	DeviceDataTypeString,
	DeviceDataTypeComplex,
	DeviceDataTypeCustom, // Holder of Long/I64/Bool/String types
	DeviceDataTypeUnknown
};

enum SVDeviceParamEnum	// do not change exitsing param numbers!
{
	DeviceParamInvalid = 0, 

	// SVO camera identification params
	DeviceParamDeviceDigNum         = 100,
	DeviceParamDeviceName           = 101,
	DeviceParamDeviceChannelName    = 102,

	DeviceParamVendorId             = 103,
	DeviceParamVendorName           = 104,
	DeviceParamModelName            = 105,
	DeviceParamFirmware             = 106,
	DeviceParamIPAddress			= 107,  //new for MIL 9.0 - used for GigE cameras
	DeviceParamCameraDefaultSettings= 108,  //Place this in the front so that it is done first

	// FileAcquistion params
	DeviceParamFileAcqImageFileName = 200,
	DeviceParamFileAcqImageDirName	= 201,
	DeviceParamFileAcqLoadingMode	= 202,
	DeviceParamFileAcqImageWidth	= 203,
	DeviceParamFileAcqImageHeight	= 204,
	DeviceParamFileAcqImageFormat	= 205,

	// physical camera params
	DeviceParamSerialNumber         = 1000,
	DeviceParamSerialNumberString   = 1001,
	DeviceParamShutter              = 1002,
	DeviceParamGamma                = 1003,
	DeviceParamBrightness           = 1004,
	DeviceParamGain                 = 1005,
	DeviceParamRegisters            = 1006,
	DeviceParamSizeBands            = 1009,
	DeviceParamFormatStrings        = 1011,
	DeviceParamFormat7Support       = 1012,
	DeviceParamTriggerEdge          = 1013,
	DeviceParamOutputStrobePulse    = 1014,
	DeviceParamStrobePulseDelay     = 1015,
	DeviceParamStrobePulseDuration  = 1016,
	DeviceParamCameraTriggerPin     = 1017,
	DeviceParamCameraStrobePin      = 1018,
	DeviceParamExposure             = 1019,
	DeviceParamTriggerMode          = 1021,
	DeviceParamNumCameraQueuedTriggers = 1022,
	DeviceParamTestPattern          = 1023,
	DeviceParamShutterDelay			= 1024,
	DeviceParamStrobeStartMode      = 1025,
	DeviceParamTriggerQueueMode     = 1026,
	DeviceParamStrobePulseAdvance   = 1027,
	DeviceParamWhiteBalanceUB       = 1028,
	DeviceParamWhiteBalanceVR       = 1029,
	DeviceParamSharpness            = 1030,
	DeviceParamHue                  = 1031,
	DeviceParamSaturation           = 1032,
	DeviceParamStrobePulseMultiplier= 1033,
	DeviceParamNumCameraBuffers     = 1034,

	DeviceParamHorizontalBinning	= 1100, // must be less than DeviceParamCameraFormats (so it gets set before)
	DeviceParamVerticalBinning		= 1101, // must be less than DeviceParamCameraFormats (so it gets set before)

	DeviceParamGigeCustom1      = 3050,
	DeviceParamGigeCustom2      = 3051,
	DeviceParamGigeCustom3      = 3052,
	DeviceParamGigeCustom4      = 3053,
	DeviceParamGigeCustom5      = 3054,
	DeviceParamGigeCustom6      = 3055,
	DeviceParamGigeCustom7      = 3056,
	DeviceParamGigeCustom8      = 3057,
	DeviceParamGigeCustom9      = 3058,
	DeviceParamGigeCustom10     = 3059,
	DeviceParamGigeCustom11     = 3060,
	DeviceParamGigeCustom12     = 3061,
	DeviceParamGigeCustom13     = 3062,
	DeviceParamGigeCustom14     = 3063,
	DeviceParamGigeCustom15     = 3064,
	DeviceParamGigeCustom16     = 3065,
	DeviceParamGigeCustom17     = 3066,
	DeviceParamGigeCustom18     = 3067,
	DeviceParamGigeCustom19     = 3068,
	DeviceParamGigeCustom20     = 3069,
	DeviceParamGigeCustom21     = 3070,
	DeviceParamGigeCustom22     = 3071,
	DeviceParamGigeCustom23     = 3072,
	DeviceParamGigeCustom24     = 3073,
	DeviceParamGigeCustom25     = 3074,
	DeviceParamGigeCustom26     = 3075,
	DeviceParamGigeCustom27     = 3076,
	DeviceParamGigeCustom28     = 3077,
	DeviceParamGigeCustom29     = 3078,
	DeviceParamGigeCustom30     = 3079,

	// Imaging parameters
	DeviceParamCameraFormats  = 4000,
	DeviceParamLightReference = 4001,
	DeviceParamLut            = 4002,

	// Acquisition I/O parameters
	DeviceParamAcquisitionTriggerSelection = 5000,	// ?? not used
	DeviceParamAcquisitionTriggerType      = 5001,	// external, software, software grab
	DeviceParamAcquisitionTriggerName      = 5002,	// NOT USED YET; will when connecting triggers with acquisition devices

	DeviceParamAcquisitionTriggerEdge      = 5003,  // Internal Parameters Only - Rising or Falling
	DeviceParamAcquisitionStrobeEdge       = 5013,  // Internal Parameters Only - Rising or Falling

	DeviceParamCameraInput	= 5050,

	DeviceParamIOStrobeInvert = 5100,
	DeviceParamIOTriggerInvert = 5101,

	DeviceParamCameraTriggerMode = 5200,

	// GIGE Acquisition parameters
	DeviceParamGigeTriggerSource		= 6000,
	DeviceParamGigeTriggerEdge			= 6001,
	DeviceParamGigeTriggerLine			= 6002,
	DeviceParamGigeTriggerEnable		= 6003,
	
	DeviceParamGigeStrobeSource			= 6050,
	DeviceParamGigeStrobeEdge			= 6051,
	DeviceParamGigeStrobeLine			= 6052,
	DeviceParamGigeStrobeEnable			= 6053,

	DeviceParamGigePacketSize			= 6100,
	
	// be sure to register the create function in SVDeviceParams.cpp if you add a parameter
};

LPCTSTR const DeviceParamInvalid_String         = (_T("Invalid"));

	// SVO camera identification params
LPCTSTR const DeviceParamDeviceDigNum_String          = (_T("DigNum"));
LPCTSTR const DeviceParamDeviceName_String            = (_T("DeviceName"));
LPCTSTR const DeviceParamDeviceChannelName_String     = (_T("ChannelName"));

LPCTSTR const DeviceParamVendorId_String        = (_T("VendorID"));
LPCTSTR const DeviceParamVendorName_String      = (_T("VendorName"));
LPCTSTR const DeviceParamModelName_String       = (_T("CameraModel"));
LPCTSTR const DeviceParamFirmware_String        = (_T("Firmware"));
LPCTSTR const DeviceParamIPAddress_String		= (_T("IPAddress"));

	// physical camera params
LPCTSTR const DeviceParamSerialNumber_String         = (_T("SerialNumber"));
LPCTSTR const DeviceParamSerialNumberString_String   = (_T("SerialNumberString"));
LPCTSTR const DeviceParamShutter_String              = (_T("Shutter"));
LPCTSTR const DeviceParamGamma_String                = (_T("Gamma"));
LPCTSTR const DeviceParamBrightness_String           = (_T("Brightness"));
LPCTSTR const DeviceParamGain_String                 = (_T("Gain"));
LPCTSTR const DeviceParamRegisters_String            = (_T("Registers"));
LPCTSTR const DeviceParamSizeBands_String            = (_T("BandSize"));
LPCTSTR const DeviceParamFormatStrings_String        = (_T("FormatStrings"));
LPCTSTR const DeviceParamFormat7Support_String       = (_T("Format7Support"));
LPCTSTR const DeviceParamTriggerEdge_String          = (_T("TriggerEdge"));
LPCTSTR const DeviceParamOutputStrobePulse_String    = (_T("StrobePulse"));
LPCTSTR const DeviceParamStrobePulseDelay_String     = (_T("StrobePulseDelay"));
LPCTSTR const DeviceParamStrobePulseDuration_String  = (_T("StrobePulseDuration"));
LPCTSTR const DeviceParamCameraTriggerPin_String     = (_T("TriggerPin"));
LPCTSTR const DeviceParamCameraStrobePin_String      = (_T("StrobePin"));
LPCTSTR const DeviceParamExposure_String             = (_T("Exposure"));
LPCTSTR const DeviceParamTriggerMode_String          = (_T("TriggerMode"));
LPCTSTR const DeviceParamCameraQueuedTriggers_String = (_T("CameraQueuedTriggers"));
LPCTSTR const DeviceParamTestPattern_String          = (_T("TestPattern"));
LPCTSTR const DeviceParamShutterDelay_String         = (_T("ShutterDelay"));
LPCTSTR const DeviceParamStrobeStartMode_String      = (_T("StrobeStartMode"));
LPCTSTR const DeviceParamTriggerQueueMode_String     = (_T("TriggerQueueMode"));
LPCTSTR const DeviceParamStrobePulseAdvance_String   = (_T("StrobePulseAdvance"));
LPCTSTR const DeviceParamWhiteBalanceUB_String       = (_T("WhiteBalanceUB"));
LPCTSTR const DeviceParamWhiteBalanceVR_String       = (_T("WhiteBalanceVR"));
LPCTSTR const DeviceParamSharpness_String            = (_T("Sharpness"));
LPCTSTR const DeviceParamHue_String                  = (_T("Hue"));
LPCTSTR const DeviceParamSaturation_String           = (_T("Saturation"));
LPCTSTR const DeviceParamStrobePulseMultiplier_String = (_T("StrobePulseMultiplier"));
LPCTSTR const DeviceParamNumCameraBuffers_String     = (_T("NumCameraBuffers"));

LPCTSTR const DeviceParamGigeCustom1_String      = (_T("Custom1"));
LPCTSTR const DeviceParamGigeCustom2_String      = (_T("Custom2"));
LPCTSTR const DeviceParamGigeCustom3_String      = (_T("Custom3"));
LPCTSTR const DeviceParamGigeCustom4_String      = (_T("Custom4"));
LPCTSTR const DeviceParamGigeCustom5_String      = (_T("Custom5"));
LPCTSTR const DeviceParamGigeCustom6_String      = (_T("Custom6"));
LPCTSTR const DeviceParamGigeCustom7_String      = (_T("Custom7"));
LPCTSTR const DeviceParamGigeCustom8_String      = (_T("Custom8"));
LPCTSTR const DeviceParamGigeCustom9_String      = (_T("Custom9"));
LPCTSTR const DeviceParamGigeCustom10_String     = (_T("Custom10"));
LPCTSTR const DeviceParamGigeCustom11_String     = (_T("Custom11"));
LPCTSTR const DeviceParamGigeCustom12_String     = (_T("Custom12"));
LPCTSTR const DeviceParamGigeCustom13_String     = (_T("Custom13"));
LPCTSTR const DeviceParamGigeCustom14_String     = (_T("Custom14"));
LPCTSTR const DeviceParamGigeCustom15_String     = (_T("Custom15"));
LPCTSTR const DeviceParamGigeCustom16_String     = (_T("Custom16"));
LPCTSTR const DeviceParamGigeCustom17_String     = (_T("Custom17"));
LPCTSTR const DeviceParamGigeCustom18_String     = (_T("Custom18"));
LPCTSTR const DeviceParamGigeCustom19_String     = (_T("Custom19"));
LPCTSTR const DeviceParamGigeCustom20_String     = (_T("Custom20"));
LPCTSTR const DeviceParamGigeCustom21_String     = (_T("Custom21"));
LPCTSTR const DeviceParamGigeCustom22_String     = (_T("Custom22"));
LPCTSTR const DeviceParamGigeCustom23_String     = (_T("Custom23"));
LPCTSTR const DeviceParamGigeCustom24_String     = (_T("Custom24"));
LPCTSTR const DeviceParamGigeCustom25_String     = (_T("Custom25"));
LPCTSTR const DeviceParamGigeCustom26_String     = (_T("Custom26"));
LPCTSTR const DeviceParamGigeCustom27_String     = (_T("Custom27"));
LPCTSTR const DeviceParamGigeCustom28_String     = (_T("Custom28"));
LPCTSTR const DeviceParamGigeCustom29_String     = (_T("Custom29"));
LPCTSTR const DeviceParamGigeCustom30_String     = (_T("Custom30"));

	// Imaging parameters
LPCTSTR const DeviceParamCameraFormats_String    = (_T("CameraFormats"));
LPCTSTR const DeviceParamLightReference_String   = (_T("LightReference"));
LPCTSTR const DeviceParamLut_String              = (_T("Lut"));

	// Acquisition I/O parameters
LPCTSTR const DeviceParamAcquisitionTriggerSelection_String = (_T("TriggerSelection"));
LPCTSTR const DeviceParamAcquisitionTriggerType_String      = (_T("TriggerSource"));
LPCTSTR const DeviceParamAcquisitionTriggerName_String      = (_T("TriggerName"));
LPCTSTR const DeviceParamAcquisitionTriggerEdge_String      = (_T("ExpectedTriggerEdge"));
LPCTSTR const DeviceParamAcquisitionStrobeEdge_String       = (_T("ExpectedStrobeEdge"));

LPCTSTR const DeviceParamIOStrobeInvert_String           = (_T("StrobeInvert"));
LPCTSTR const DeviceParamIOTriggerInvert_String          = (_T("TriggerInvert"));

LPCTSTR const DeviceParamCameraTriggerMode_String        = (_T("CameraTriggerMode"));

	// File Acquisition parameters
LPCTSTR const DeviceParamFileAcqImageFileName_String	= (_T("Image Filename"));
LPCTSTR const DeviceParamFileAcqImageDirName_String		= (_T("Image Directory"));
LPCTSTR const DeviceParamFileAcqLoadingMode_String		= (_T("File Loading Mode"));
LPCTSTR const DeviceParamFileAcqSingleFile_String		= (_T("Single File"));
LPCTSTR const DeviceParamFileAcqContinuousLoad_String	= (_T("Continuous Load"));
LPCTSTR const DeviceParamFileAcqSingleIteration_String	= (_T("Single Iteration"));
LPCTSTR const DeviceParamFileAcqImageWidth_String		= (_T("Image Width"));
LPCTSTR const DeviceParamFileAcqImageHeight_String		= (_T("Image Height"));
LPCTSTR const DeviceParamFileAcqImageFormat_String		= (_T("Image Format"));

	// GIGE Acquisition parameters
LPCTSTR const DeviceParamCameraInput_String			= (_T("Input"));
LPCTSTR const DeviceParamGigeTriggerLine_String		= (_T("TriggerLine"));
LPCTSTR const DeviceParamGigeTriggerEnable_String	= (_T("TriggerEnable"));
LPCTSTR const DeviceParamGigeStrobeSource_String	= (_T("StrobeSource"));
LPCTSTR const DeviceParamGigeStrobeEdge_String      = (_T("StrobeEdge"));
LPCTSTR const DeviceParamGigeStrobeLine_String      = (_T("StrobeLine"));
LPCTSTR const DeviceParamGigeStrobeEnable_String	= (_T("StrobeEnable"));
LPCTSTR const DeviceParamGigePacketSize_String		= (_T("PacketSize"));

LPCTSTR const DeviceParamHorizontalBinning_String	= (_T("HorizontalBinning"));
LPCTSTR const DeviceParamVerticalBinning_String		= (_T("VerticalBinning"));

LPCTSTR const DeviceParamCameraDefaultSettings_String= (_T("DefaultSettings"));

template<typename TYPE> struct TDeviceOption	// used for discrete selectable values
{
	TYPE m_Value;
	SVString m_Description;
	TDeviceOption() {};
	TDeviceOption(const TYPE& rValue) { value = rValue; };
	TDeviceOption(const TYPE& rValue, const SVString& rDescription) { m_Value = rValue; m_Description = rDescription; };
	operator TYPE() const { return m_Value; };
};

template<typename OptionType>
class OptionDescMatch
{
private:
	const SVString& m_rDescription;

public:
	OptionDescMatch(const SVString& rDescription)
		: m_rDescription(rDescription) {}

	bool operator()(const OptionType& option) const
	{
		return option.m_Description == m_rDescription;
	}
};

template <typename ValueType, typename OptionType, typename EvalFunc>
class OptionValueMatch
{
private:
	const ValueType& m_rValue;
	const EvalFunc& m_rEvalFunc;

public:
	OptionValueMatch(const EvalFunc& evalFunc, const ValueType& rValue)
		: m_rEvalFunc(evalFunc), m_rValue(rValue) {}

	bool operator()(const OptionType& option) const
	{
		return m_rEvalFunc(option, m_rValue);
	}
};

template<typename TYPE> struct TDeviceParamInfo
{
	typedef TDeviceOption<TYPE> OptionType;
	typedef std::vector<OptionType> OptionsType;
	
	OptionsType options;

	TDeviceParamInfo() {}
	TDeviceParamInfo(const TDeviceParamInfo<TYPE>& rhs) { *this = rhs; }
	const TDeviceParamInfo<TYPE>& operator= (const TDeviceParamInfo<TYPE>& rhs);	// defined below

	bool OptionExists(SVString description) const
	{
		bool bRet = false;
		OptionsType::const_iterator it = std::find_if(options.begin(), options.end(), OptionDescMatch<OptionType>(description));
		if (it != options.end())
		{
			bRet = true;
		}
		return bRet;
	}
};

template<typename TYPE>
inline const TDeviceParamInfo<TYPE>& TDeviceParamInfo<TYPE>::operator= (const TDeviceParamInfo<TYPE>& rhs)
{
	if ( this != &rhs )
	{
		options = rhs.options;
	}
	return *this;
}

template<> struct TDeviceParamInfo<long>
{
	long min;
	long max;
	long vendorId;
	unsigned long mask;	// for register params
	long offset;
	double multiplier;
	double unit_divisor;
	SVString sUnits;
	typedef TDeviceOption<long>     OptionType;
	typedef std::vector<OptionType> OptionsType;
	OptionsType options;

	TDeviceParamInfo();
	TDeviceParamInfo(const TDeviceParamInfo& rhs);
	const TDeviceParamInfo& operator= (const TDeviceParamInfo& rhs);

	bool OptionExists(SVString description) const
	{
		bool bRet = false;
		OptionsType::const_iterator it = std::find_if(options.begin(), options.end(), OptionDescMatch<OptionType>(description));
		if (it != options.end())
		{
			bRet = true;
		}
		return bRet;
	}
};

template<> struct TDeviceParamInfo<__int64>
{
	__int64 min;
	__int64 max;
	__int64 offset;
	double multiplier;
	SVString sUnits;
	typedef TDeviceOption<__int64>     OptionType;
	typedef std::vector<OptionType> OptionsType;
	OptionsType options;

	TDeviceParamInfo() ;
	TDeviceParamInfo(const TDeviceParamInfo& rhs);
	const TDeviceParamInfo& operator= (const TDeviceParamInfo& rhs);

	bool OptionExists(SVString description) const
	{
		bool bRet = false;
		OptionsType::const_iterator it = std::find_if(options.begin(), options.end(), OptionDescMatch<OptionType>(description));
		if (it != options.end())
		{
			bRet = true;
		}
		return bRet;
	}
};

// these conversion routines can be moved to another file
HRESULT ToVariant( long lValue, VARIANT& rv );
HRESULT ToVariant( __int64 lValue, VARIANT& rv );
HRESULT ToVariant( bool bValue, VARIANT& rv );
HRESULT ToVariant( const SVString& sValue, VARIANT& rv );

HRESULT FromVariant( long& rlValue, const VARIANT& rv );
HRESULT FromVariant( __int64& riValue, const VARIANT& rv );
HRESULT FromVariant( bool& rlValue, const VARIANT& rv );
HRESULT FromVariant( SVString& rlValue, const VARIANT& rv );
