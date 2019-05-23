// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxPolarTransformStruct
// * .File Name       : $Workfile:   SVMatroxPolarTransformStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:26:58  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVMatroxSimpleEnums.h"
#pragma endregion Includes

class SVMatroxBuffer;

/**
@SVObjectName Matrox Polar Transform Struct

@SVObjectOverview This class is used to hold data being sent to the PolarTransform function.

@SVObjectOperations This data class has no operations.

*/
struct SVMatroxPolarTransformStruct  
{
public:
	SVMatroxPolarTransformStruct(SVMatroxBuffer& p_rSource);
	virtual ~SVMatroxPolarTransformStruct();

	SVMatroxBuffer&				m_rSource;
	double						m_dCenterX;
	double						m_dCenterY;
	double						m_dStartRadius;
	double						m_dEndRadius;
	double						m_dStartAngle;
	double						m_dEndAngle;
	SVImageOperationTypeEnum	m_eOpMode;
	SVImageOperationTypeEnum	m_eInterpMode;
	double						m_dDestSizeX;
	double						m_dDestSizeY;

};

