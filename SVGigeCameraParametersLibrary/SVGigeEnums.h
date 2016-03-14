//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeEnums
//* .File Name       : $Workfile:   SVGigeEnums.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   12 Jun 2013 15:01:36  $
//******************************************************************************

#ifndef SVGIGEENUMS_H
#define SVGIGEENUMS_H

/*
	DeviceParamDeviceDigNum         = 100,  // SVObserver Parameter
	DeviceParamDeviceName           = 101,  // SVObserver Parameter
	DeviceParamDeviceChannelName    = 102,  // SVObserver Parameter

	DeviceParamVendorId             = 103,  // DLL Parameter
	DeviceParamVendorName           = 104,  // DLL Parameter
	DeviceParamModelName            = 105,  // DLL Parameter

	// physical camera params
	DeviceParamSerialNumber         = 1000,  // DLL Parameter
	DeviceParamSerialNumberString   = 1001,  // DLL Parameter
	DeviceParamRegisters            = 1006,
	DeviceParamSizeBands            = 1009,
	DeviceParamFormatStrings        = 1011,
	DeviceParamFormat7Support       = 1012,
	DeviceParamOutputStrobePulse    = 1014,
	DeviceParamStrobePulseDelay     = 1015,
	DeviceParamStrobePulseDuration  = 1016,
	DeviceParamCameraTriggerPin     = 1017,
	DeviceParamCameraStrobePin      = 1018,
	DeviceParamTriggerMode          = 1021,
	DeviceParamNumCameraQueuedTriggers = 1022,
	DeviceParamTestPattern          = 1023,
	DeviceParamShutterDelay			= 1024,
	DeviceParamStrobeStartMode      = 1025,
	DeviceParamTriggerQueueMode     = 1026,
	DeviceParamStrobePulseAdvance   = 1027,
	DeviceParamStrobePulseMultiplier= 1033,
	DeviceParamNumCameraBuffers     = 1034,

	// Custom parameters
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

	DeviceParamCameraInput				= 5050,

	DeviceParamIOStrobeInvert = 5100,
	DeviceParamIOTriggerInvert = 5101,

	DeviceParamCameraTriggerMode = 5200,

	DeviceParamGigeTriggerSource		= 6000,
	DeviceParamGigeTriggerEdge			= 6001,
	DeviceParamGigeTriggerLine			= 6002,
	DeviceParamGigeTriggerEnable		= 6003,
	
	DeviceParamGigeStrobeSource			= 6050,
	DeviceParamGigeStrobeEdge			= 6051,
	DeviceParamGigeStrobeLine			= 6052,
	DeviceParamGigeStrobeEnable			= 6053,

	DeviceParamGigePacketSize			= 6100,
*/

enum SVGigeParameterEnum
{
	SVGigeParameterStart = 0,				// marker ?
	SVGigeParameterExposureMode = 0,		// 00
	SVGigeParameterXOffset,					// 01
	SVGigeParameterYOffset,					// 02
	SVGigeParameterXSize,					// 03
	SVGigeParameterYSize,					// 04
	SVGigeParameterColorFormat,				// 05
	SVGigeParameterBayerPattern,			// 06
	SVGigeParameterFeatureBrightness,		// 07
	SVGigeParameterFeatureAutoExposure,		// 08
	SVGigeParameterFeatureSharpness,		// 09
	SVGigeParameterFeatureWhiteBalanceU,	// 10
	SVGigeParameterFeatureWhiteBalanceV,	// 11
	SVGigeParameterFeatureHue,				// 12
	SVGigeParameterFeatureSaturation,		// 13
	SVGigeParameterFeatureGamma,			// 14
	SVGigeParameterFeatureShutter,			// 15
	SVGigeParameterFeatureGain,				// 16
	SVGigeParameterTriggerSource,			// 17
	SVGigeParameterTriggerEdge,				// 18
	SVGigeParameterTriggerInvert,			// 19
	SVGigeParameterTriggerLine,				// 20
	SVGigeParameterTriggerDelay,			// 21
	SVGigeParameterTriggerEnable,			// 22
	SVGigeParameterStrobeSource,			// 23
	SVGigeParameterStrobeEdge,				// 24
	SVGigeParameterStrobeInvert,			// 25
	SVGigeParameterStrobeDelay,				// 26
	SVGigeParameterStrobeDuration,			// 27
	SVGigeParameterStrobeLine,				// 28
	SVGigeParameterStrobeEnable,			// 29
	SVGigeParameterPacketSize,				// 30
	SVGigeParameterSize,					// marker (31)
	SVGigeParameterLutEnable,				// 32
	SVGigeParameterLutArray,				// 33
	SVGigeParameterLineInput,				// 34
	SVGigeParameterInputEvent,				// 35 - Internal use only
	SVGigeParameterInputEventName,			// 36 - Internal use only

	SVGigeParameterVerticalBinning = 50,	// 50
	SVGigeParameterHorizontalBinning,		// 51

	SVGigeParameterVendorName = 100,		// 100 - Read Only parameter
	SVGigeParameterModelName,				// 101 - Read Only parameter
	SVGigeParameterSerialNumber,			// 102 - Read Only parameter
	SVGigeParameterFeatureOverrides,		// 103 - Internal use only
	SVGigeParameterIPAddress,				// 104 - Read Only parameter

	SVGigeParameterCustomBase = 200,		// 200 - Starting Point for Custom Parameters
	SVGigeParameterCustom1 = SVGigeParameterCustomBase,
	SVGigeParameterCustom2,
	SVGigeParameterCustom3,
	SVGigeParameterCustom4,
	SVGigeParameterCustom5,
	SVGigeParameterCustom6,
	SVGigeParameterCustom7,
	SVGigeParameterCustom8,
	SVGigeParameterCustom9,
	SVGigeParameterCustom10,
	SVGigeParameterCustom11,
	SVGigeParameterCustom12,
	SVGigeParameterCustom13,
	SVGigeParameterCustom14,
	SVGigeParameterCustom15,
	SVGigeParameterCustom16,
	SVGigeParameterCustom17,
	SVGigeParameterCustom18,
	SVGigeParameterCustom19,
	SVGigeParameterCustom20,
	SVGigeParameterCustom21,
	SVGigeParameterCustom22,
	SVGigeParameterCustom23,
	SVGigeParameterCustom24,
	SVGigeParameterCustom25,
	SVGigeParameterCustom26,
	SVGigeParameterCustom27,
	SVGigeParameterCustom28,
	SVGigeParameterCustom29,
	SVGigeParameterCustom30,

	// for save/restore of camera settings
	SVGigeBeginTrackParameters	= 99998,	// Internal use only
	SVGigeEndTrackParameters	= 99999		// Internal use only
};

#endif

