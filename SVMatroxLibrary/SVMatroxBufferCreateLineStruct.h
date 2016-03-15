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

#ifndef SV_MATROX_BUFFER_CREATE_LINE_STRUCT_H
#define SV_MATROX_BUFFER_CREATE_LINE_STRUCT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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

#endif // #ifndef SV_MATROX_BUFFER_CREATE_LINE_STRUCT_H


