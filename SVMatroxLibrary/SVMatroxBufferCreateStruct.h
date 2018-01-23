// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxBufferCreateStruct
// * .File Name       : $Workfile:   SVMatroxBufferCreateStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   01 Oct 2013 11:08:10  $
// ******************************************************************************

#pragma once

#include "SVMatroxEnums.h"
#include "SVMatroxTypedefs.h"

/**
@SVObjectName Matrox Buffer Create Structure

@SVObjectOverview This class is a data holder for a creating a buffer.

@SVObjectOperations This data class has a set image depth function.

*/
class SVMatroxBufferCreateStruct  
{
public:
	SVMatroxBufferCreateStruct();
	virtual ~SVMatroxBufferCreateStruct();

	// See help on MbufAllocColor.
	long						m_lSizeBand;
	long						m_lSizeX;
	long						m_lSizeY;
	SVMatroxBufferTypeEnum		m_eType;
	SVMatroxBufferAttributeEnum	m_eAttribute;
	bool SetImageDepth( long p_lDepth);
	bool operator ==(const SVMatroxBufferCreateStruct& other) const;
};


