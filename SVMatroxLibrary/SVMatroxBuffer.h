//******************************************************************************
//* COPYRIGHT (c) 2007 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxBuffer
//* .File Name       : $Workfile:   SVMatroxBuffer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:59:52  $
//******************************************************************************

#ifndef SV_MATROX_BUFFER_H
#define SV_MATROX_BUFFER_H

#include "SVMatroxBufferFacade.h"

/**
@SVObjectName Matrox Buffer

@SVObjectOverview This class is a data holder for a matrox buffer id.

@SVObjectOperations The empty function is used to detect if the handle is empty.  The freeing of the matrox handle held by this class is controlled externally to this class at the moment.

*/
class SVMatroxBuffer
{
public:
	friend class SVExternalToolTask;
	friend class SVMatroxBufferArray;
	friend class SVMatroxBufferInterface;
	friend class SVMatroxBarCodeInterface;
	friend class SVMatroxBlobInterface;
	friend class SVMatroxDigitizerInterface;
	friend class SVMatroxDisplayInterface;
	friend class SVMatroxGraphicsInterface;
	friend class SVMatroxImageInterface;
	friend class SVMatroxOcrInterface;
	friend class SVMatroxPatternInterface;
	friend class SVMatroxDisplayBuffer;

	SVMatroxBuffer();
	SVMatroxBuffer(const SVMatroxBuffer& p_rBuf);
	SVMatroxBuffer(const SVMatroxDisplayBuffer& p_rBuf);

	virtual ~SVMatroxBuffer();

	bool empty() const;

	void clear();

	void swap( SVMatroxBuffer& p_rBuf );

	const SVMatroxBuffer& operator=( const SVMatroxBuffer& p_rBuf );

	bool operator<( const SVMatroxBuffer& p_rBuf ) const;

	bool IsParent( const SVMatroxBuffer& p_rParent ) const;

protected:
	SVMatroxIdentifier GetIdentifier() const;

	SVMatroxBufferPtr m_BufferPtr;

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxBuffer.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:59:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   10 Feb 2011 13:57:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   27 Jan 2011 10:31:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Jun 2009 16:00:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Export method to be const
 * Moved code from header to implementation file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jun 2009 15:52:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Export method to be const.
 * Moved code from header to implementation file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Jul 2007 15:18:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
