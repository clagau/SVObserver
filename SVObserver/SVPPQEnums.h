//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPPQEnums
//* .File Name       : $Workfile:   SVPPQEnums.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:32:54  $
//******************************************************************************

#ifndef SVPPQENUMS_H
#define SVPPQENUMS_H

enum SVPPQOutputModeEnum
{
	SVPPQUnknownMode                          = -1,

	SVPPQNextTriggerMode                      = 0,  // Resets outputs on trigger. Writes outputs immediately.

	SVPPQTimeDelayMode                        = 1,  // Resets outputs on trigger. Writes outputs after delay time is over, even if data isn't complete.
	                                                // Aborts waiting if new trigger occurs.

	SVPPQTimeDelayAndDataCompleteMode         = 2,  // Resets outputs on trigger. Writes outputs after delay time is over and data is complete.
	                                                // Aborts waiting if new trigger occurs.

	SVPPQExtendedTimeDelayMode                = 3,  // Doesn't reset outputs on trigger. Writes outputs after delay time is over, even if data isn't complete.
	                                                // Aborts waiting if product leaves PPQ.

	SVPPQExtendedTimeDelayAndDataCompleteMode = 4,  // Doesn't reset outputs on trigger. Writes outputs after delay time is over and data is complete.
	                                                // Aborts waiting if product leaves PPQ.
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPPQEnums.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:32:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Feb 2010 10:43:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new files to separate PPQ functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
