
// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxImageResult
// * .File Name       : $Workfile:   SVMatroxImageResult.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:21:18  $
// ******************************************************************************

#pragma once

#include "SVMatroxTypedefs.h"

class SVMatroxImageInterface;

/**
@SVObjectName Matrox Image Result

@SVObjectOverview This is a data class that holds a image result identifier.

@SVObjectOperations This class has a function to detect if it is empty.  When this class is destroyed the matrox handle will be freed.

*/
class SVMatroxImageResult  
{
	friend SVMatroxImageInterface;
public:
	SVMatroxImageResult();
	virtual ~SVMatroxImageResult();
	bool empty() const;
	bool operator<(const SVMatroxImageResult& p_rBuf) const {return m_ImageResultID < p_rBuf.m_ImageResultID;};

protected:
	SVMatroxIdentifier m_ImageResultID;

private:
	void LocalClear();
};

