// ******************************************************************************
// * COPYRIGHT (c) 2007 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxImageRotateStruct
// * .File Name       : $Workfile:   SVMatroxImageRotateStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:21:30  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVMatroxSimpleEnums.h"
#pragma endregion Includes

class SVMatroxBuffer;

/**
@SVObjectName Matrox Image Rotate Struct

@SVObjectOverview This class is used to hold data being sent to the ImageRotate function.

@SVObjectOperations This data class has no operations.

*/
struct SVMatroxImageRotateStruct  
{
public:
	explicit SVMatroxImageRotateStruct(SVMatroxBuffer& p_rSource);
	virtual ~SVMatroxImageRotateStruct();

	SVMatroxBuffer&			m_rSource;
	double					m_dAngle; 
	double					m_dSrcCenX;
	double					m_dSrcCenY;
	double					m_dDstCenX;
	double					m_dDstCenY;
	SVImageOperationTypeEnum m_eInterpolation;
};

