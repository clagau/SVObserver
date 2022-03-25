//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeEnums
//* .File Name       : $Workfile:   SVGigeEnums.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   12 Jun 2013 15:01:36  $
//******************************************************************************

#pragma once

namespace SvDef
{
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
		SVGigeParameterCameraDefaultSettings,	// 105
		SVTriggerType,							// 106 - Trigger type setting for the camera
		SVCameraSequenceFile,							// 107 - Internal use only

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
} //namespace SvDef


