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
	long						m_lSizeBand = 1;
	long						m_lSizeX = 10;
	long						m_lSizeY = 10;
	SVMatroxBufferTypeEnum		m_eType = SVUnknownBufferType;
	SVMatroxBufferAttributeEnum	m_eAttribute = SVBufAttUnknown;

	bool operator ==(const SVMatroxBufferCreateStruct& other) const;
	bool operator <(const SVMatroxBufferCreateStruct& other) const;
};

bool SetImageDepth(SVMatroxBufferCreateStruct& rBuffer, long p_lDepth);

int getBufferSize(const SVMatroxBufferCreateStruct& rBuffer);
