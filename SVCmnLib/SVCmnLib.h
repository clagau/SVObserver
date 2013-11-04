// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCmnLib
// * .File Name       : $Workfile:   SVCmnLib.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 18:10:04  $
// ******************************************************************************

#include "SVRegistry.h"
#include "SVException.h"
#include "SVPackedFile.h"

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCmnLib\SVCmnLib.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 18:10:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   13 Jun 2001 13:56:46   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Inital Release of the common library containing XML classes and utility functions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   05 Dec 2000 09:23:08   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  75
 * SCR Title:  New Library Classes
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Added new classes to the library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   12 Oct 2000 10:57:14   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  58
 * SCR Title:  Add Version control to SVFocus Splash screen
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Added SVVersionInfo class to SVCmnLib.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Jul 2000 12:53:36   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  11
 * SCR Title:  Stage 2: Security
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Added SecuredObject module and modified SVCmnLib.h to include the new header file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   04 Apr 2000 17:10:06   Joe
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  1
 * SCR Title:  Baseline Development: Run Mode
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to add required functionality to SVFocus and other projects associated with SVocus.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Mar 22 2000 17:28:14   mike
 * Initial revision.
*/