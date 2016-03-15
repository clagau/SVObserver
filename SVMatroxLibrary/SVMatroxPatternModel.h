// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxPatternModel
// * .File Name       : $Workfile:   SVMatroxPatternModel.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:26:46  $
// ******************************************************************************

#ifndef SV_MATROX_PATTERN_MODEL_H
#define SV_MATROX_PATTERN_MODEL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVMatroxTypedefs.h"

class SVMatroxPatternInterface;

/**
@SVObjectName Matrox Pattern Model

@SVObjectOverview This is a data class that holds the identifier for a matrox pattern model.

@SVObjectOperations This class has a function to detect if it is empty.  When this class is destroyed the matrox handle will be freed.

*/
class SVMatroxPatternModel  
{
	friend SVMatroxPatternInterface;
public:
	SVMatroxPatternModel();
	virtual ~SVMatroxPatternModel();

	bool empty() const;
	bool operator<(const SVMatroxPatternModel& p_rBuf) const {return m_ModelId < p_rBuf.m_ModelId;};

private:
	SVMatroxIdentifier m_ModelId;

protected:
	void LocalClear();

};

#endif // #ifndef SV_MATROX_PATTERN_MODEL_H

