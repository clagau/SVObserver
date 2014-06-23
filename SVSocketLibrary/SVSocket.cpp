//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSocket
//* .File Name       : $Workfile:   SVSocket.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   19 Jun 2014 15:48:06  $
//******************************************************************************

#include "Stdafx.h"
#include "SVSocket.h"

volatile long g_sn = 1;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSocketLibrary\SVSocket.cpp_v  $
 * 
 *    Rev 1.2   19 Jun 2014 15:48:06   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Merged changes from SVRemoteControl project.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 Aug 2013 11:11:40   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  841
 * SCR Title:  Change configuration print report format to xml
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Fixed disconnect problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 17:15:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   21 Feb 2012 14:20:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed issues with partial packets and transfer issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Feb 2012 09:27:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated functionality to remove bugs associated with the remote input tool and communications to that tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Dec 2011 13:54:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
