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

#pragma once

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

