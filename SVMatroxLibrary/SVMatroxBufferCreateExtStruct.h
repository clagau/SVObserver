// ******************************************************************************
// * COPYRIGHT (c) 2007 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxBufferCreateExtStruct
// * .File Name       : $Workfile:   SVMatroxBufferCreateExtStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 14:59:20  $
// ******************************************************************************

#pragma once

#include "Definitions\SVMatroxSimpleEnums.h"

/**
@SVObjectName Matrox Buffer Create External Structure

@SVObjectOverview This class is a data holder for a creating an external buffer.

@SVObjectOperations This data class has no operations.

*/
struct SVMatroxBufferCreateExtStruct  
{
public:
	SVMatroxBufferCreateExtStruct();
	virtual ~SVMatroxBufferCreateExtStruct();

	// Member Variables 
	// See Matrox help on MbufCreateColor
	long					m_lSizeBand;
	long					m_lSizeX;
	long					m_lSizeY;
	SVMatroxBufferTypeEnum	m_eType {SV8BitUnsigned};
	SVMatroxBufferAttributeEnum	m_eAttribute {SVBufAttUnknown};
	SVMatroxBufferInfoEnum	m_eControlFlag {SVBufInfoUnknown};
	long					m_PitchWidth;
	void **		m_ppArrayOfDataPtr;

	// Functions.
	bool SetImageDepth( long p_lDepth);
};


