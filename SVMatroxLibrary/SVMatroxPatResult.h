// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxPatResult
// * .File Name       : $Workfile:   SVMatroxPatResult.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:25:16  $
// ******************************************************************************

#pragma once

#include "SVMatroxTypedefs.h"

class SVMatroxPatternInterface;

/**
@SVObjectName Matrox Pattern Result

@SVObjectOverview This is a data class that holds the identifier for a matrox pattern result.

@SVObjectOperations This class has a function to detect if it is empty.  When this class is destroyed the matrox handle will be freed.

*/
class SVMatroxPatResult  
{
	friend SVMatroxPatternInterface;
public:
	SVMatroxPatResult();
	virtual ~SVMatroxPatResult();

	bool empty() const;
	bool operator<(const SVMatroxPatResult& p_rBuf) const {return m_PatResultId < p_rBuf.m_PatResultId;};

protected:
	SVMatroxIdentifier m_PatResultId;

private:
	void LocalClear();

};

