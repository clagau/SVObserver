// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxDisplay
// * .File Name       : $Workfile:   SVMatroxDisplay.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:12:32  $
// ******************************************************************************

#ifndef SV_MATROX_DISPLAY_H
#define SV_MATROX_DISPLAY_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVMatroxTypedefs.h"

class SVMatroxDisplayInterface;
class SVMatroxBufferInterface;

/**
@SVObjectName Matrox Display

@SVObjectOverview This class is a data holder for a matrox display buffer id.

@SVObjectOperations

*/
class SVMatroxDisplay
{
	friend SVMatroxDisplayInterface;
	friend SVMatroxBufferInterface;

public:

	SVMatroxDisplay();

	virtual ~SVMatroxDisplay();

	bool empty() const;
	bool operator<(const SVMatroxDisplay& p_rBuf) const {return m_DisplayIdentifier < p_rBuf.m_DisplayIdentifier;};

protected:
	SVMatroxIdentifier m_DisplayIdentifier;

private:
	void LocalClear();

};

#endif // SV_MATROX_DISPLAY_H

