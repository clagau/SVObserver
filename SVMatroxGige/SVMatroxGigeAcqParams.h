//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeAcqParams
//* .File Name       : $Workfile:   SVMatroxGigeAcqParams.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:38:50  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVHBitmapUtilitiesLibrary/SVImageFormatEnum.h"
#pragma endregion Includes

namespace SVMatroxGigeTrigger
{
	enum TriggerSource
	{
		HardwareTrigger = 0,
		SoftwareTrigger
	};
}

struct SVMatroxGigeAcqParams
{
	unsigned long Width;		// X-Dimension
	unsigned long Height;		// Y-Dimension
	unsigned long Format;		// Actual ColorFormat (pixel depth ?)
	unsigned long XOffsetPos;	// X-Offset
	unsigned long YOffsetPos;	// Y-Offset
	UINT8 TriggerMode;			// Triggermode (Bulb mode ?)
	UINT8 BayerPattern;			// Used bayer pattern
	bool bAcquisitionTriggered;	//

	SVMatroxGigeTrigger::TriggerSource TriggerType;				// Trigger Source (Hardware/Software)
	SVMatroxDigitizerGrab::SVGrabTriggerModeEnum TriggerEdge;	// Trigger Edge(Rising/Falling)

	SVMatroxGigeAcqParams()
	: Width(640)
	, Height(480)
	, XOffsetPos(0)
	, YOffsetPos(0)
	, Format(SVImageFormatMono8)
	, TriggerMode(0)
	, BayerPattern(0)
	, TriggerType(SVMatroxGigeTrigger::HardwareTrigger)
	, TriggerEdge(SVMatroxDigitizerGrab::SVEdgeRising)
	, bAcquisitionTriggered(false)
	{}
};

