// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxDisplayBuffer
// * .File Name       : $Workfile:   SVMatroxDisplayBuffer.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:12:42  $
// ******************************************************************************

#pragma once

#include "SVMatroxBufferInterface.h"

/**
@SVObjectName Matrox Display

@SVObjectOverview This class is a data holder for a matrox display buffer id.

@SVObjectOperations

*/
class SVMatroxDisplayBuffer
{
public:
	friend class SVMatroxDisplayInterface;
	friend class SVMatroxBufferInterface;
	friend class SVMatroxBuffer;

	SVMatroxDisplayBuffer();

	virtual ~SVMatroxDisplayBuffer();

	bool empty() const;

	void clear();

	bool operator<( const SVMatroxDisplayBuffer& p_rBuf ) const;

protected:
	SVMatroxIdentifier GetIdentifier() const;

	SVMatroxBufferPtr m_DisplayBufferPtr;

};

