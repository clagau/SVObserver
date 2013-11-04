//******************************************************************************
//* .Module Name     : Enable Driver
//* .File Name       : $Workfile:   FCTLEnable.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   29 Apr 2013 10:07:54  $
//******************************************************************************

//* This function overrides the licence file validation and enables the driver 
//* to function with any camera.

/*******************************************************************************
  Global enable function for FireStack.
  intek, c. kuehnel, 8.12.2005
*******************************************************************************/

#ifndef FCTLENABLE_H
#define FCTLENABLE_H

#ifdef __cplusplus
extern "C" {
#endif

UINT32 __cdecl EnableDriver(CARDHANDLE hCard);

#ifdef __cplusplus
}
#endif

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\Intek\FCTLEnable.h_v  $
 * 
 *    Rev 1.0   29 Apr 2013 10:07:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Dec 2005 09:39:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added these files to the project to allow for unlimited licencing and to handle the start frame logic.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
