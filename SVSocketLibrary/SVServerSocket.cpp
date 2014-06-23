//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVServerSocket
//* .File Name       : $Workfile:   SVServerSocket.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   19 Jun 2014 15:48:04  $
//******************************************************************************

#include "stdafx.h"
#include "SVServerSocket.h"

// Create must be called prior to calling Listen

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSocketLibrary\SVServerSocket.cpp_v  $
 * 
 *    Rev 1.1   19 Jun 2014 15:48:04   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Merged changes from SVRemoteControl project.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 17:15:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Mar 2013 16:11:38   jhanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed Bind to bind with INADDR_ANY.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Feb 2012 09:27:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated functionality to remove bugs associated with the remote input tool and communications to that tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Dec 2011 13:54:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
