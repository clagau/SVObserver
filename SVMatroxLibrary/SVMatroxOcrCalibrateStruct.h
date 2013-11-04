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

#include "SVMatroxLibrary/SVMatroxTypedefs.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"


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

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxOcrCalibrateStruct.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:24:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Jul 2007 15:21:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
