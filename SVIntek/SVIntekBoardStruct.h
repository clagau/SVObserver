// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIntekBoardStruct
// * .File Name       : $Workfile:   SVIntekBoardStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   26 Apr 2013 14:50:42  $
// ******************************************************************************

#ifndef SVINTEKBOARDSTRUCT_H
#define SVINTEKBOARDSTRUCT_H

#include "SVContainerLibrary/SVVector.h"
#include "SVCFireBus.h"

struct SVIntekBoardStruct  
{
public:
	SVIntekBoardStruct();
	virtual ~SVIntekBoardStruct();

	CString m_csName;
	CString m_csFireStackName;

	FBCARDID m_ulCardId;

	CFireBus *m_pFireBus;

	HANDLE m_hThread;

	bool m_bFirstBusResetStarted;
	bool m_bFirstBusResetComplete;
};

typedef SVVector< SVIntekBoardStruct* > SVIntekBoardStructPtrCArray;

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVIntek\SVIntekBoardStruct.h_v  $
 * 
 *    Rev 1.0   26 Apr 2013 14:50:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Nov 2010 15:44:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   23 Mar 2006 08:18:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  552
 * SCR Title:  Fix problem with Intek driver when attaching multiple cameras to one board
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new variables to board structure to monitor initial bus reset.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Nov 2005 12:29:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial code to implement Intek 1394 driver for SVObserver and test applications.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
