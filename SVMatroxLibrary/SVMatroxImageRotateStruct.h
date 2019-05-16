// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxImageRotateStruct
// * .File Name       : $Workfile:   SVMatroxImageRotateStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:21:30  $
// ******************************************************************************

#pragma once

#include "SVMatroxBuffer.h"
#include "SVMatroxSimpleEnums.h"

/**
@SVObjectName Matrox Image Rotate Struct

@SVObjectOverview This class is used to hold data being sent to the ImageRotate function.

@SVObjectOperations This data class has no operations.

*/
class SVMatroxImageRotateStruct  
{
public:
	SVMatroxImageRotateStruct(SVMatroxBuffer& p_rSource);
	virtual ~SVMatroxImageRotateStruct();

	SVMatroxBuffer&			m_rSource;
	double					m_dAngle; 
	double					m_dSrcCenX;
	double					m_dSrcCenY;
	double					m_dDstCenX;
	double					m_dDstCenY;
	SVImageOperationTypeEnum m_eInterpolation;
};

