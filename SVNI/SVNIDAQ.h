// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIONIDAQ
// * .File Name       : $Workfile:   SVNIDAQ.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 16:26:30  $
// ******************************************************************************

#ifndef _SVIONIDAQ_H
#define _SVIONIDAQ_H

HRESULT g_NIOpen();
HRESULT g_NIClose();

// Callback Prototype
// void mycallback(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

HRESULT g_NIConfigDAQEventMessage( short p_sSlot, 
                                   short p_sMode, 
																	 char *p_szChanStr, 
																	 short p_sDAQEvent, 
																	 long p_lTrigVal0, 
																	 long p_lTrigVal1,
																	 unsigned long p_ulSkipCnt,
																	 unsigned long p_ulPreTrigScans,
																	 unsigned long p_ulPostTrigScans,
																	 HWND p_hwndHandle,
																	 short p_sMsg,
																	 unsigned long p_ulCallbackAddr );

HRESULT g_NIDIGBlockCheck( short p_sSlot, short p_sGrp,
													 unsigned long *p_pulRemaining );

HRESULT g_NIDIGBlockClear( short p_sSlot, short p_sGrp );

HRESULT g_NIDIGBlockIn( short p_sSlot, short p_sGrp, short *p_psBuffer,
												unsigned long p_ulCnt );

HRESULT g_NIDIGBlockOut( short p_sSlot, short p_sGrp, short *p_psBuffer,
												 unsigned long p_ulCnt );

// Callback Prototype
// void mycallback(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

HRESULT g_NIDIGChangeMessageConfig( short p_sDeviceNumber, 
                                    short p_sOperation, 
																	  char *p_szRiseChanStr, 
																	  char *p_szFallChanStr, 
																	  HWND p_hwndHandle,
																	  short p_sMsg,
																	  unsigned long p_ulCallbackAddr );

HRESULT g_NIDIGChangeMessageControl( short p_sDeviceNumber, short p_sCtrlCode );

HRESULT g_NIDIGFilterConfig( short p_sDeviceNumber, short p_sMode, 
														 char *p_szChanStr, double p_dInterval );

HRESULT g_NIDIGInLine( short p_sSlot, short p_sPort, short p_sLineNum, 
                       short *p_psState );

HRESULT g_NIDIGInPrt( short p_sSlot, short p_sPort, long *p_plPattern );

HRESULT g_NIDIGLineConfig( short p_sSlot, short p_sPort, short p_sLineNum, 
                           short p_psDirection );

HRESULT g_NIDIGOutLine( short p_sSlot, short p_sPort,	short p_sLineNum,	
                        short p_sState );

HRESULT g_NIDIGOutPrt( short p_sSlot, short p_sPort, long p_lPattern );

HRESULT g_NIDIGPrtConfig( short p_sSlot, short p_sPort,	short p_sLatchMode,
													short p_sDirection );

HRESULT g_NIDIGPrtStatus( short p_sSlot, short p_sPort,	short *p_psStatus );

HRESULT g_NIDIGSCANSetup( short p_sSlot, short p_sGrp,	short p_sNumPorts,
													short *p_psPortList, short p_sDirection );

HRESULT g_NIGetDAQDeviceInfo( short p_sDeviceNumber, unsigned long p_ulInfoType,
															unsigned long *p_ulInfoVal );

HRESULT g_NIGetNIDAQVersion( unsigned long *p_pulVersion );

HRESULT g_NIInitDABrds( short p_sSlot, short *p_psBrdCode );

#endif   // _SVIONIDAQ_H

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVNI\SVNIDAQ.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 16:26:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Dec 2003 07:55:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated prt operation functions from short to long.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 14:31:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   New project added to interface I/O and Image processing for the National Instruments DAQ Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/