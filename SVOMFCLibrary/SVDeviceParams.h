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

#ifndef __SVDEVICEPARAMS_H__
#define __SVDEVICEPARAMS_H__

#include <vector>
#include <map>
#include <algorithm>
#include "SVDeviceParamCollection.h"

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
	//DeviceParamWidth                = 1007,	// obsolete
	//DeviceParamHeight               = 1008,	// obsolete
	DeviceParamSizeBands            = 1009,
	//DeviceParamNumFormats           = 1010,	// obsolete
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

	DeviceParamCameraRegister1      = 3001,
	DeviceParamCameraRegister2      = 3002,
	DeviceParamCameraRegister3      = 3003,
	DeviceParamCameraRegister4      = 3004,
	DeviceParamCameraRegister5      = 3005,
	DeviceParamCameraRegister6      = 3006,
	DeviceParamCameraRegister7      = 3007,
	DeviceParamCameraRegister8      = 3008,
	DeviceParamCameraRegister9      = 3009,
	DeviceParamCameraRegister10     = 3010,
	DeviceParamCameraRegister11     = 3011,
	DeviceParamCameraRegister12     = 3012,
	DeviceParamCameraRegister13     = 3013,
	DeviceParamCameraRegister14     = 3014,
	DeviceParamCameraRegister15     = 3015,
	DeviceParamCameraRegister16     = 3016,
	DeviceParamCameraRegister17     = 3017,
	DeviceParamCameraRegister18     = 3018,
	DeviceParamCameraRegister19     = 3019,
	DeviceParamCameraRegister20     = 3020,

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



	// physical camera params
LPCTSTR const DeviceParamSerialNumber_String         = (_T("SerialNumber"));
LPCTSTR const DeviceParamSerialNumberString_String   = (_T("SerialNumberString"));
LPCTSTR const DeviceParamShutter_String              = (_T("Shutter"));
LPCTSTR const DeviceParamGamma_String                = (_T("Gamma"));
LPCTSTR const DeviceParamBrightness_String           = (_T("Brightness"));
LPCTSTR const DeviceParamGain_String                 = (_T("Gain"));
LPCTSTR const DeviceParamRegisters_String            = (_T("Registers"));
//LPCTSTR const DeviceParamWidth_String                = (_T(""));	// obsolete
//LPCTSTR const DeviceParamHeight_String               = (_T(""));	// obsolete
LPCTSTR const DeviceParamSizeBands_String            = (_T("BandSize"));
//LPCTSTR const DeviceParamNumFormats_String           = (_T(""));	// obsolete
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

LPCTSTR const DeviceParamCameraRegister1_String      = (_T("Register1"));
LPCTSTR const DeviceParamCameraRegister2_String      = (_T("Register2"));
LPCTSTR const DeviceParamCameraRegister3_String      = (_T("Register3"));
LPCTSTR const DeviceParamCameraRegister4_String      = (_T("Register4"));
LPCTSTR const DeviceParamCameraRegister5_String      = (_T("Register5"));
LPCTSTR const DeviceParamCameraRegister6_String      = (_T("Register6"));
LPCTSTR const DeviceParamCameraRegister7_String      = (_T("Register7"));
LPCTSTR const DeviceParamCameraRegister8_String      = (_T("Register8"));
LPCTSTR const DeviceParamCameraRegister9_String      = (_T("Register9"));
LPCTSTR const DeviceParamCameraRegister10_String     = (_T("Register10"));
LPCTSTR const DeviceParamCameraRegister11_String     = (_T("Register11"));
LPCTSTR const DeviceParamCameraRegister12_String     = (_T("Register12"));
LPCTSTR const DeviceParamCameraRegister13_String     = (_T("Register13"));
LPCTSTR const DeviceParamCameraRegister14_String     = (_T("Register14"));
LPCTSTR const DeviceParamCameraRegister15_String     = (_T("Register15"));
LPCTSTR const DeviceParamCameraRegister16_String     = (_T("Register16"));
LPCTSTR const DeviceParamCameraRegister17_String     = (_T("Register17"));
LPCTSTR const DeviceParamCameraRegister18_String     = (_T("Register18"));
LPCTSTR const DeviceParamCameraRegister19_String     = (_T("Register19"));
LPCTSTR const DeviceParamCameraRegister20_String     = (_T("Register20"));

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
LPCTSTR const DeviceParamCameraFormats_String          = (_T("CameraFormats"));
LPCTSTR const DeviceParamLightReference_String         = (_T("LightReference"));
LPCTSTR const DeviceParamLut_String                    = (_T("Lut"));

	// Acquisition I/O parameters
LPCTSTR const DeviceParamAcquisitionTriggerSelection_String     = (_T("TriggerSelection"));
LPCTSTR const DeviceParamAcquisitionTriggerType_String          = (_T("TriggerSource"));
LPCTSTR const DeviceParamAcquisitionTriggerName_String          = (_T("TriggerName"));
LPCTSTR const DeviceParamAcquisitionTriggerEdge_String          = (_T("ExpectedTriggerEdge"));

LPCTSTR const DeviceParamAcquisitionStrobeEdge_String           = (_T("ExpectedStrobeEdge"));

LPCTSTR const DeviceParamIOStrobeInvert_String           = (_T("StrobeInvert"));
LPCTSTR const DeviceParamIOTriggerInvert_String          = (_T("TriggerInvert"));

LPCTSTR const DeviceParamCameraTriggerMode_String          = (_T("CameraTriggerMode"));

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
LPCTSTR const DeviceParamCameraInput_String		= (_T("Input"));
LPCTSTR const DeviceParamGigeTriggerLine_String		= (_T("TriggerLine"));
LPCTSTR const DeviceParamGigeTriggerEnable_String	= (_T("TriggerEnable"));
LPCTSTR const DeviceParamGigeStrobeSource_String	= (_T("StrobeSource"));
LPCTSTR const DeviceParamGigeStrobeEdge_String      = (_T("StrobeEdge"));
LPCTSTR const DeviceParamGigeStrobeLine_String      = (_T("StrobeLine"));
LPCTSTR const DeviceParamGigeStrobeEnable_String	= (_T("StrobeEnable"));
LPCTSTR const DeviceParamGigePacketSize_String		= (_T("PacketSize"));

LPCTSTR const DeviceParamHorizontalBinning_String	= (_T("HorizontalBinning"));
LPCTSTR const DeviceParamVerticalBinning_String		= (_T("VerticalBinning"));

template<typename TYPE> struct TDeviceOption	// used for discrete selectable values
{
	TYPE value;
	SVString strDescription;
	TDeviceOption(){};
	TDeviceOption(TYPE t) {value = t;}
	TDeviceOption(TYPE t, SVString s) {value = t; strDescription = s;}
	operator TYPE() const {return value;}
};

template<typename OptionType>
class OptionDescMatch
{
private:
	SVString m_description;

public:
	OptionDescMatch(SVString description)
	: m_description(description) {}

	bool operator()(const OptionType& option) const
	{
		return option.strDescription == m_description;
	}
};

template <typename ValueType, typename OptionType, typename EvalFunc>
class OptionValueMatch
{
private:
	ValueType m_value;
	EvalFunc m_evalFunc;

public:
	OptionValueMatch(const EvalFunc& evalFunc, const ValueType& value)
	: m_evalFunc(evalFunc), m_value(value) {}

	bool operator()(const OptionType& option) const
	{
		return m_evalFunc(option, m_value);
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
			bRet = true;
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
	CString sUnits;
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
			bRet = true;
		return bRet;
	}
};

template<> struct TDeviceParamInfo<__int64>
{
	__int64 min;
	__int64 max;
	__int64 offset;
	double multiplier;
	CString sUnits;
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
			bRet = true;
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
HRESULT FromVariant( CString& rlValue, const VARIANT& rv );

#endif //#ifndef __SVDEVICEPARAMS_H__

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVDeviceParams.h_v  $
 * 
 *    Rev 1.1   15 May 2013 19:42:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   10 May 2013 10:11:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  828
 * SCR Title:  Camera Binning
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added DeviceParamHorizontalBinning and DevcieParamVerticalBinning
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 13:05:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   01 Apr 2013 13:46:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added enums and strings for the custom Gige device parameters
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   18 Jan 2013 16:02:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added DeviceParamCameraInput
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   29 Jun 2012 16:47:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  772
 * SCR Title:  Implement GigE Camera Reconnect Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to add the needed functionality to track parameter information and reset the camera to the desired parameters on reconnect.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   18 Feb 2011 09:49:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to remove duplicate string class, and fixed string conversion issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   28 May 2010 14:22:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  694
 * SCR Title:  Upgrade SVObserver version for Chromis release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with includes and make them consistant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   16 Jun 2009 17:32:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Gige device parameters
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   03 Jun 2009 10:04:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Moved the Derived SVDeviceParam classes to their own modules
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   05 Dec 2008 15:22:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Loading Modes for File Acquisition
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   16 Sep 2008 13:11:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Device Parameters for FileAcquisition
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   10 Sep 2008 12:45:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added capability to check if an option exists
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   20 Jun 2007 14:48:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   23 Jun 2006 09:35:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  568
 * SCR Title:  Add Register/Value Parameter capability to 1394 camera files and SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added CameraRegister parameters
 * added mask to TDeviceParamInfo<long>
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   23 Jun 2006 09:24:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  568
 * SCR Title:  Add Register/Value Parameter capability to 1394 camera files and SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added CameraRegister parameters
 * added mask to TDeviceParamInfo<long>
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   28 Mar 2006 13:47:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  555
 * SCR Title:  Implement new Flea features
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added DeviceParamFirmware
 * added TDeviceParamInfo<long>::unit_divisor
 * removed DeviceParamBulbModeTriggerEdge
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   21 Mar 2006 14:08:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  555
 * SCR Title:  Fix bulb mode for Flea
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added DeviceParamBulbModeTriggerEdge
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   21 Mar 2006 14:04:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  555
 * SCR Title:  Fix bulb mode for Flea
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added DeviceParamBulbModeTriggerEdge
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   10 Aug 2005 13:17:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  475
 * SCR Title:  Increase Hub capabilities
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved base class code to header file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   10 Aug 2005 13:17:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  475
 * SCR Title:  Increase Hub capabilities
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved base class code to header file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   23 Jul 2004 08:16:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  435
 * SCR Title:  Add Bulb Mode Operation to the Digital SVIM for Point Gray Dragonfly Cameras
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new camera parameter CameraTriggerMode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   15 Jul 2004 09:17:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  436
 * SCR Title:  Update ISG camera handler
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added DeviceParamNumCameraBuffers
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   25 May 2004 08:11:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  421
 * SCR Title:  Allow SVObserver to get the Trigger Edge and Strobe Edge from the Camera File
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added two new device parameters to handle the trigger edge and strobe edge data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   03 Feb 2004 17:17:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added support for named device params
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   23 Jan 2004 09:56:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added ModelName and VendorName
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   21 Jan 2004 15:24:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   renamed an unused paramter
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   16 Jan 2004 07:23:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added StrobePulseMultiplier and IOTriggerInvert
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   12 Jan 2004 11:10:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  404
 * SCR Title:  Change Camera Parameters that represent time to use standard units
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added SetMetaData( ..pBaseParam), GetScaledParam(, SetScaledParam( long ), GetScaledMin(, GetScaledMax(
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   21 Nov 2003 09:50:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed CameraStrobePolarity to IOStrobeInvert
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   12 Nov 2003 09:24:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added DeviceParamVendorId
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   03 Nov 2003 16:20:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added DeviceParamDeviceChannelName
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   22 Oct 2003 08:07:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for WhiteBalance, Sharpness, Hue, and Saturation
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   14 Oct 2003 09:26:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added StrobePulseAdvance
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   17 Sep 2003 08:56:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added device parameters
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   29 Aug 2003 13:11:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for NullDeviceParam
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   27 Aug 2003 10:08:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added device param enums
 * added DEFINE_VISITABLE()
 * added automatic type conversion for SVDeviceOption template
 * fixed int64 option type
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Aug 2003 12:32:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   renumbered params
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   31 Jul 2003 08:28:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved Lut, Light Reference, and CameraFormats to SVImageLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Jul 2003 08:41:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added CameraFormats param
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 10:19:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   transfered from SVObserver project
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
