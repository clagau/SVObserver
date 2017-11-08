// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxOcrCalibrateStruct
// * .File Name       : $Workfile:   SVMatroxOcrCalibrateStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:24:28  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVMatroxTypedefs.h"
#include "SVMatroxBuffer.h"

#pragma endregion Includes

/**
@SVObjectName Matrox Ocr Calibrate Structure

@SVObjectOverview This is a data structure that holds information to calibrate a Matrox font object.

@SVObjectOperations This class does not have any operations.

*/
struct SVMatroxOcrCalibrateStruct  
{
	SVMatroxOcrCalibrateStruct();
	virtual ~SVMatroxOcrCalibrateStruct();

	// To do:
	// Parameratized constructor
	// Copy Constructor
	// Operator=

	SVMatroxBuffer m_ImageBuff;
	std::string m_strCalString;
	double m_TargetCharSizeXMin;
	double m_TargetCharSizeXMax;
	double m_TargetCharSizeXStep;
	double m_TargetCharSizeYMin;
	double m_TargetCharSizeYMax; 
	double m_TargetCharSizeYStep;
};

