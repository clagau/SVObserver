// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxOcrCreateStruct
// * .File Name       : $Workfile:   SVMatroxOcrCreateStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:24:40  $
// ******************************************************************************

#ifndef SV_MATROX_OCR_CREATE_STRUCT_H
#define SV_MATROX_OCR_CREATE_STRUCT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVMatroxTypedefs.h"
#include "SVMatroxEnums.h"


/**
@SVObjectName Matrox Ocr Create Structure

@SVObjectOverview This is a data structure that holds information for creating a Matrox font object.

@SVObjectOperations This class does not have any operations.

*/
struct SVMatroxOcrCreateStruct  
{
	SVMatroxOcrCreateStruct();
	virtual ~SVMatroxOcrCreateStruct();

	// To do:
	// Parameratized constructor
	// Copy Constructor
	// Operator=

	SVOcrTypeEnum m_eFontType;
	long m_lCharNumber;
	long m_lCharCellSizeX;
	long m_lCharCellSizeY;
	long m_lCharOffsetX;
	long m_lCharOffsetY;
	long m_lCharSizeX;
	long m_lCharSizeY;
	long m_lCharThickness;
	long m_lStringLength;
	SVOcrInitFlag m_eInitFlag;

};

#endif // #ifndef SV_MATROX_OCR_CREATE_STRUCT_H

