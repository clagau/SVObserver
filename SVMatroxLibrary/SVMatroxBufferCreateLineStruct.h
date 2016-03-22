// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVBufferCreateLineStruct
// * .File Name       : $Workfile:   SVMatroxBufferCreateLineStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:01:42  $
// ******************************************************************************

#pragma once

#include "SVMatroxEnums.h"

/**
@SVObjectName Matrox Buffer Create Line Structure

@SVObjectOverview This class is a data holder for a creating a 1d buffer or a line buffer.

@SVObjectOperations This data class has a set image depth function.

*/
class SVMatroxBufferCreateLineStruct  
{
public:
	SVMatroxBufferCreateLineStruct();
	virtual ~SVMatroxBufferCreateLineStruct();
	bool SetImageDepth( long p_lDepth);

	// See help on MbufAlloc1d.
	long					m_lSizeX;
	SVMatroxBufferTypeEnum	m_eType;
	SVMatroxBufferAttributeEnum	m_eAttribute;

};


