// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxBufferCreateExtStruct
// * .File Name       : $Workfile:   SVMatroxBufferCreateExtStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 14:59:20  $
// ******************************************************************************

#pragma once

#include "SVMatroxSimpleEnums.h"

/**
@SVObjectName Matrox Buffer Create External Structure

@SVObjectOverview This class is a data holder for a creating an external buffer.

@SVObjectOperations This data class has no operations.

*/
class SVMatroxBufferCreateExtStruct  
{
public:
	SVMatroxBufferCreateExtStruct();
	virtual ~SVMatroxBufferCreateExtStruct();

	// Member Variables 
	// See Matrox help on MbufCreateColor
	long					m_lSizeBand;
	long					m_lSizeX;
	long					m_lSizeY;
	SVMatroxBufferTypeEnum	m_eType;
	SVMatroxBufferAttributeEnum	m_eAttribute;
	SVMatroxBufferInfoEnum	m_eControlFlag;
	long					m_PitchWidth;
	void **		m_ppArrayOfDataPtr;

	// Functions.
	bool SetImageDepth( long p_lDepth);
};


