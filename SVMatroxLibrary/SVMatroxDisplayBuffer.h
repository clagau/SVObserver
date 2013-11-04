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

#ifndef SV_MATROX_DISPLAY_BUFFER_H
#define SV_MATROX_DISPLAY_BUFFER_H

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

#endif // SV_MATROX_DISPLAY_BUFFER_H

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxDisplayBuffer.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:12:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 Jan 2011 10:36:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
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
