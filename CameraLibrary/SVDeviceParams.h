// ******************************************************************************
// * COPYRIGHT (c) 2002 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#pragma endregion Includes

//This comment is to avoid that the SVDeviceParamCollection include is marked as forward declaration due to TheDeviceParamFactory

#define DEVICE_PARAM_CREATE_FN(x) x##::CreateNew
#define REGISTER_DEVICE_PARAM(x,y,fn) const bool bRegister##x = TheDeviceParamFactory::Instance().Register(x, y, DEVICE_PARAM_CREATE_FN(fn));

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
	std::string m_Description;
	TDeviceOption() {};
	explicit TDeviceOption(const TYPE& rValue) : m_Value {rValue} {}
	explicit TDeviceOption(const TYPE& rValue, const std::string& rDescription) : m_Value {rValue}, m_Description {rDescription} {}
	operator TYPE() const { return m_Value; };
};

template<typename OptionType>
class OptionDescMatch
{
private:
	const std::string& m_rDescription;

public:
	explicit OptionDescMatch(const std::string& rDescription) : m_rDescription(rDescription) {}

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
	typedef typename TDeviceOption<TYPE> OptionType;
	typedef std::vector<OptionType> OptionsType;
	
	OptionsType options;

	TDeviceParamInfo() {}
	TDeviceParamInfo(const TDeviceParamInfo<TYPE>& rhs) { *this = rhs; }
	const TDeviceParamInfo<TYPE>& operator= (const TDeviceParamInfo<TYPE>& rhs);	// defined below

	bool OptionExists(LPCTSTR pDescription) const
	{
		bool bRet = false;
		typename OptionsType::const_iterator it = std::find_if(options.begin(), options.end(), OptionDescMatch<OptionType>(pDescription));
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

template<> 
struct TDeviceParamInfo<long>
{
	long min {0L};
	long max {LONG_MAX};
	long vendorId {0L};
	unsigned long mask {0UL};	// for register params
	long offset {0L};
	double multiplier {1.0};
	double unit_divisor {1.0};
	std::string sUnits {};
	typedef TDeviceOption<long>     OptionType;
	typedef std::vector<OptionType> OptionsType;
	OptionsType options;

	TDeviceParamInfo() = default;
	TDeviceParamInfo(const TDeviceParamInfo& rhs);
	const TDeviceParamInfo& operator= (const TDeviceParamInfo& rhs);

	bool OptionExists(const std::string& rDescription) const
	{
		bool bRet = false;
		OptionsType::const_iterator it = std::find_if(options.begin(), options.end(), OptionDescMatch<OptionType>(rDescription));
		if (it != options.end())
		{
			bRet = true;
		}
		return bRet;
	}
};

template<>
struct TDeviceParamInfo<__int64>
{
	__int64 min {0LL};
	__int64 max {_I64_MAX};
	__int64 offset {0LL};
	double multiplier {1.0};
	std::string sUnits{};
	typedef TDeviceOption<__int64>     OptionType;
	typedef std::vector<OptionType> OptionsType;
	OptionsType options;

	TDeviceParamInfo() = default;
	TDeviceParamInfo(const TDeviceParamInfo& rhs);
	const TDeviceParamInfo& operator= (const TDeviceParamInfo& rhs);

	bool OptionExists(const std::string& rDescription) const
	{
		bool bRet = false;
		OptionsType::const_iterator it = std::find_if(options.begin(), options.end(), OptionDescMatch<OptionType>(rDescription));
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
HRESULT ToVariant( const std::string& sValue, VARIANT& rv );

HRESULT FromVariant( long& rlValue, const VARIANT& rv );
HRESULT FromVariant( __int64& riValue, const VARIANT& rv );
HRESULT FromVariant( bool& rlValue, const VARIANT& rv );
HRESULT FromVariant( std::string& rlValue, const VARIANT& rv );
