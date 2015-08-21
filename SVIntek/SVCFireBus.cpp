// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCFireBus
// * .File Name       : $Workfile:   SVCFireBus.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:32:28  $
// ******************************************************************************

#include "StdAfx.h"
#include "SVCFireBus.h"
#include "SVIntekBoardStruct.h"
#include "Intek\firebus.cpp"

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVIntek\SVCFireBus.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 12:32:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Apr 2007 10:45:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  582
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.15
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added #include for SVIntekBoardStruct.h
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Nov 2005 12:28:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial code to implement Intek 1394 driver for SVObserver and test applications.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/