// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIOMEExports
// * .File Name       : $Workfile:   SVMEExports.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:43:34  $
// ******************************************************************************

#ifndef _SVIOMEEXPORTS_H
#define _SVIOMEEXPORTS_H

typedef HRESULT ( CALLBACK *SVMECallbackPtr )( void *,   /* Owner */
                                               void * ); /* Data */ 

HRESULT WINAPI SVCreate();
HRESULT WINAPI SVDestroy();

HRESULT WINAPI SVInputGetCount( unsigned long *p_pulCount );
HRESULT WINAPI SVInputGetValue( unsigned long p_ulChannel, bool *p_pbValue );
HRESULT WINAPI SVInputSetValue( unsigned long p_ulChannel, bool p_bValue );
HRESULT WINAPI SVInputGetPortCount( unsigned long *p_pulCount );
HRESULT WINAPI SVInputGetPortValue( unsigned long p_ulPort, unsigned long *p_pulValue );
HRESULT WINAPI SVInputSetPortValue( unsigned long p_ulPort, unsigned long p_ulValue );

HRESULT WINAPI SVOutputGetCount( unsigned long *p_pulCount );
HRESULT WINAPI SVOutputGetValue( unsigned long p_ulChannel, bool *p_pbValue );
HRESULT WINAPI SVOutputSetValue( unsigned long p_ulChannel, bool p_bValue );
HRESULT WINAPI SVOutputGetPortCount( unsigned long *p_pulCount );
HRESULT WINAPI SVOutputGetPortValue( unsigned long p_ulPort, unsigned long *p_pulValue );
HRESULT WINAPI SVOutputSetPortValue( unsigned long p_ulPort, unsigned long p_ulValue );

HRESULT WINAPI SVTriggerGetCount( unsigned long *p_pulCount );
HRESULT WINAPI SVTriggerGetHandle( unsigned long *p_pulHandle, unsigned long p_ulIndex );
HRESULT WINAPI SVTriggerGetName( unsigned long p_ulHandle, BSTR *p_pbstrName );
HRESULT WINAPI SVTriggerRegister( unsigned long p_ulHandle, SVMECallbackPtr p_pCallback, void *p_vOwner, void *p_vData );
HRESULT WINAPI SVTriggerUnregister( unsigned long p_ulHandle, SVMECallbackPtr p_pCallback, void *p_vOwner, void *p_vData );
HRESULT WINAPI SVTriggerUnregisterAll( unsigned long p_ulHandle );
HRESULT WINAPI SVTriggerStart( unsigned long p_ulHandle );
HRESULT WINAPI SVTriggerStop( unsigned long p_ulHandle );

#endif    // _SVIOMEEXPORTS_H

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVME\SVMEExports.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:43:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Oct 2003 12:42:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated exposed function prototypes to change parameters from a class to individual items.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 14:27:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   New project added to interface I/O and Image processing for the Meilhaus Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/