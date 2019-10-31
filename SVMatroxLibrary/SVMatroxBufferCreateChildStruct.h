// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVBufferCreateChildStruct
// * .File Name       : $Workfile:   SVMatroxBufferCreateChildStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:00:16  $
// ******************************************************************************

#pragma once

class SVMatroxBuffer;



struct MatroxBufferChildDataStruct
{
	// Specifies the color band of the parent data buffer from which to allocate 
	// the child data buffer. The specified color band should be valid in the 
	// parent buffer. 
	long m_lBand = 0;

	// Specifies the number of bands of the child buffer to create.
	long m_lParentBandCount = 1;

	// Specifies the horizontal offset of the child buffer, relative to the 
	// parent buffer's top-left pixel. The offset must be within the width of 
	// the parent buffer
	long m_lOffX = 0;

	// Specifies the vertical offset of the child buffer, relative to the parent 
	// buffer's top-left pixel. The offset must be within the height of the 
	// parent buffer.
	long m_lOffY = 0;

	// Specifies the width of the child buffer. 
	long m_lSizeX = 10;

	// Specifies the height of the child buffer.
	long m_lSizeY = 10;
};

/**
@SVObjectName Matrox Buffer Create Child Struct

@SVObjectOverview This class is a data holder for a creating child buffers.

@SVObjectOperations This data class has no operations.

*/
struct SVMatroxBufferCreateChildStruct  
{
public:
	explicit SVMatroxBufferCreateChildStruct(const SVMatroxBuffer& rBuf);
	virtual ~SVMatroxBufferCreateChildStruct();

	// Specifies the identifier of the parent buffer.
	const SVMatroxBuffer& m_rParentBufId;

	MatroxBufferChildDataStruct m_data;

};


