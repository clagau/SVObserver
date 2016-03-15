// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIntekEnums
// * .File Name       : $Workfile:   SVIntekEnums.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   26 Apr 2013 14:52:32  $
// ******************************************************************************

#ifndef _SVINTEKENUMS_H
#define _SVINTEKENUMS_H

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

	DeviceParamIOStrobeInvert = 5100,
	DeviceParamIOTriggerInvert = 5101,

	DeviceParamCameraTriggerMode = 5200,
//*/

enum SVIntekParameterDCamEnum
{
	SVIntekParameterDCamStart = 0,
	SVIntekParameterDCamTriggerMode = 0,        // 00
	SVIntekParameterDCamIsoSpeed,               // 01
	SVIntekParameterDCamFormat,                 // 02
	SVIntekParameterDCamMode,                   // 03
	SVIntekParameterDCamRate,                   // 04
	SVIntekParameterDCamYIsRawMode,             // 05
	SVIntekParameterDCamBayerPattern,           // 06
	SVIntekParameterDCamY16InverseOrder,        // 07
	SVIntekParameterDCamFormat7XOffset,         // 08
	SVIntekParameterDCamFormat7YOffset,         // 09
	SVIntekParameterDCamFormat7XSize,           // 10
	SVIntekParameterDCamFormat7YSize,           // 11
	SVIntekParameterDCamFormat7ColorFormat,     // 12
	SVIntekParameterDCamFormat7IsoPacketSize,   // 13
	SVIntekParameterDCamFeatureBrightness,      // 14
	SVIntekParameterDCamFeatureAutoExposure,    // 15
	SVIntekParameterDCamFeatureSharpness,       // 16
	SVIntekParameterDCamFeatureWhiteBalanceU,   // 17
	SVIntekParameterDCamFeatureWhiteBalanceV,   // 18
	SVIntekParameterDCamFeatureHue,             // 19
	SVIntekParameterDCamFeatureSaturation,      // 20
	SVIntekParameterDCamFeatureGamma,           // 21
	SVIntekParameterDCamFeatureShutter,         // 22
	SVIntekParameterDCamFeatureGain,            // 23
	SVIntekParameterDCamFeatureIris,            // 24
	SVIntekParameterDCamFeatureFocus,           // 25
	SVIntekParameterDCamFeatureTemperature,     // 26
	SVIntekParameterDCamFeatureZoom,            // 27
	SVIntekParameterDCamFeaturePan,             // 28
	SVIntekParameterDCamFeatureTilt,            // 29
	SVIntekParameterDCamFeatureOpticalFilter,   // 30
	SVIntekParameterDCamSize,

	SVIntekParameterDCamVendorId = 100,         // 100 - Read Only parameter
	SVIntekParameterDCamVendorName,             // 101 - Read Only parameter
	SVIntekParameterDCamModelName,              // 102 - Read Only parameter
	SVIntekParameterDCamSerialNumberHigh,       // 103 - Read Only parameter
	SVIntekParameterDCamSerialNumberLow,        // 104 - Read Only parameter
	SVIntekParameterDCamSerialNumberString,     // 105 - Read Only parameter
	SVIntekParameterDCamColorFormat,            // 106 - Read Only parameter
	SVIntekParameterDCamIsoPacketSize,          // 107 - Read Only parameter
	SVIntekParameterDCamIsoPacketCount,         // 108 - Read Only parameter

	// for Software trigger (internal)
	SVIntekSoftwareTriggerRegisterAddress = 190, // 190 - Address of Software Trigger Register
	SVIntekSoftwareTriggerRegisterValue,         // 191 - Value for Software Trigger Register

	SVIntekParameterDCamRegisterAddress = 200,  // 200
	SVIntekParameterDCamRegisterDataQuad,       // 201
	SVIntekParameterDCamRegisterDataBlock,      // 202

	// for save/restore of camera register settings
	SVIntekBeginTrackParameters = 99998,
	SVIntekEndTrackParameters	= 99999
};

#endif // _SVINTEKENUMS_H

