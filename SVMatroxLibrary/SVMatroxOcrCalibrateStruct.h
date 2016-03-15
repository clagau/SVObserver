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

#ifndef SV_MATROX_OCR_CALIBRATE_STRUCT_H
#define SV_MATROX_OCR_CALIBRATE_STRUCT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVMatroxTypedefs.h"
#include "SVMatroxBuffer.h"


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
	SVMatroxString m_strCalString;
	double m_TargetCharSizeXMin;
	double m_TargetCharSizeXMax;
	double m_TargetCharSizeXStep;
	double m_TargetCharSizeYMin;
	double m_TargetCharSizeYMax; 
	double m_TargetCharSizeYStep;
};

#endif // #ifndef SV_MATROX_OCR_CALIBRATE_STRUCT_H

