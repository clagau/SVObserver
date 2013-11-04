//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDataManagerGlobals
//* .File Name       : $Workfile:   SVDataManagerGlobals.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 07:53:46  $
//******************************************************************************

#ifndef SVDATAMANAGERGLOBALS_H
#define SVDATAMANAGERGLOBALS_H

#define SV_DATAMANAGER_MAXNAME_LENGTH 128
#define MAXIMUM_DATAMANAGER_DEPTH   50000
#define SVDM_ERROR_CONDITION          0x80000000

//MACROS
#define SV_NBRCHARS(x)  sizeof (x) /  sizeof (TCHAR)


#endif // #ifndef SVDATAMANAGERGLOBALS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVDataManagerLibrary\SVDataManagerGlobals.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 07:53:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Sep 2009 16:47:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved code from SVLibrary to SVDataManagerLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

