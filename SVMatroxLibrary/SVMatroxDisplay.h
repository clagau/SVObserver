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

#include "SVMatroxLibrary/SVMatroxTypedefs.h"

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

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxDisplay.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:12:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Jul 2007 15:20:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
