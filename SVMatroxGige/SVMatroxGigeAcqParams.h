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
#include "Definitions/SVImageFormatEnum.h"
#include "Definitions/TriggerType.h"
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerGrabEnums.h"
#pragma endregion Includes

struct SVMatroxGigeAcqParams
{
	unsigned long Width {640UL};		// X-Dimension
	unsigned long Height {480UL};		// Y-Dimension
	unsigned long Format {SvDef::SVImageFormatMono8}; // Actual ColorFormat (pixel depth ?)
	unsigned long XOffsetPos {0UL};		// X-Offset
	unsigned long YOffsetPos {0UL};		// Y-Offset
	UINT8 TriggerMode {0};				// Triggermode (Bulb mode ?)
	UINT8 BayerPattern {0};				// Used bayer pattern

	SvDef::TriggerType TriggerType {SvDef::HardwareTrigger};
	SVMatroxDigitizerGrab::SVGrabTriggerModeEnum TriggerEdge{SVMatroxDigitizerGrab::SVEdgeRising};

	SVMatroxGigeAcqParams() = default;
};

