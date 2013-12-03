// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIONIDAQ
// * .File Name       : $Workfile:   SVNIDAQ.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 16:26:24  $
// ******************************************************************************

#include "StdAfx.h"
#include "SVNIDAQ.h"

typedef short (WINAPI *SVIONIConfigDAQEventMessagePtr)( short, short, char *, short, 
                                                          long, long, unsigned long, 
																								          unsigned long, unsigned long,
																                          HWND, short, unsigned long );
typedef short (WINAPI *SVIONIDIGBlockCheckPtr)( short, short, unsigned long * );
typedef short (WINAPI *SVIONIDIGBlockClearPtr)( short, short );
typedef short (WINAPI *SVIONIDIGBlockInPtr)( short, short, short *, unsigned long );
typedef short (WINAPI *SVIONIDIGBlockOutPtr)( short, short, short *, unsigned long );
typedef short (WINAPI *SVIONIDIGChangeMessageConfigPtr)( short, short, char *, char *, 
                                                         HWND, short, unsigned long );
typedef short (WINAPI *SVIONIDIGChangeMessageControlPtr)( short, short );
typedef short (WINAPI *SVIONIDIGFilterConfigPtr)( short, short, char *, double );
typedef short (WINAPI *SVIONIDIGInLinePtr)( short, short, short, short * );
typedef short (WINAPI *SVIONIDIGInPrtPtr)( short, short, long * );
typedef short (WINAPI *SVIONIDIGLineConfigPtr)( short, short, short, short );
typedef short (WINAPI *SVIONIDIGOutLinePtr)( short, short, short,	short );
typedef short (WINAPI *SVIONIDIGOutPrtPtr)( short, short, long );
typedef short (WINAPI *SVIONIDIGPrtConfigPtr)( short, short, short, short );
typedef short (WINAPI *SVIONIDIGPrtStatusPtr)( short, short, short * );
typedef short (WINAPI *SVIONIDIGSCANSetupPtr)( short, short, short,	short *, short );
typedef short (WINAPI *SVIONIGetDAQDeviceInfoPtr)( short, unsigned long,
															                       unsigned long * );
typedef short (WINAPI *SVIONIGetNIDAQVersionPtr)( unsigned long * );
typedef short (WINAPI *SVIONIInitDABrdsPtr)( short, short * );


SVIONIConfigDAQEventMessagePtr g_pNIConfigDAQEventMessage = NULL;
SVIONIDIGBlockCheckPtr g_pNIDIGBlockCheck = NULL;
SVIONIDIGBlockClearPtr g_pNIDIGBlockClear = NULL;
SVIONIDIGBlockInPtr g_pNIDIGBlockIn = NULL;
SVIONIDIGBlockOutPtr g_pNIDIGBlockOut = NULL;
SVIONIDIGChangeMessageConfigPtr g_pNIDIGChangeMessageConfig = NULL;
SVIONIDIGChangeMessageControlPtr g_pNIDIGChangeMessageControl = NULL;
SVIONIDIGFilterConfigPtr g_pNIDIGFilterConfig = NULL;
SVIONIDIGInLinePtr g_pNIDIGInLine = NULL;
SVIONIDIGInPrtPtr g_pNIDIGInPrt = NULL;
SVIONIDIGLineConfigPtr g_pNIDIGLineConfig = NULL;
SVIONIDIGOutLinePtr g_pNIDIGOutLine = NULL;
SVIONIDIGOutPrtPtr g_pNIDIGOutPrt = NULL;
SVIONIDIGPrtConfigPtr g_pNIDIGPrtConfig = NULL;
SVIONIDIGPrtStatusPtr g_pNIDIGPrtStatus = NULL;
SVIONIDIGSCANSetupPtr g_pNIDIGSCANSetup = NULL;
SVIONIGetDAQDeviceInfoPtr g_pNIGetDAQDeviceInfo = NULL;
SVIONIGetNIDAQVersionPtr g_pNIGetNIDAQVersion = NULL;
SVIONIInitDABrdsPtr g_pNIInitDABrds = NULL;

HMODULE g_hLibrary = NULL;

HRESULT g_NIOpen()
{
	HRESULT l_hrOk = S_OK;

	if ( g_hLibrary == NULL )
	{
		g_hLibrary = ::LoadLibrary( "nidaq32.dll" );
		// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
		Sleep(0);

		if ( g_hLibrary != NULL )
		{
			g_pNIConfigDAQEventMessage = (SVIONIConfigDAQEventMessagePtr)::GetProcAddress( g_hLibrary, "Config_DAQ_Event_Message" );
			g_pNIDIGBlockCheck = (SVIONIDIGBlockCheckPtr)::GetProcAddress( g_hLibrary, "DIG_Block_Check" );
			g_pNIDIGBlockClear = (SVIONIDIGBlockClearPtr)::GetProcAddress( g_hLibrary, "DIG_Block_Clear" );
			g_pNIDIGBlockIn = (SVIONIDIGBlockInPtr)::GetProcAddress( g_hLibrary, "DIG_Block_In" );
			g_pNIDIGBlockOut = (SVIONIDIGBlockOutPtr)::GetProcAddress( g_hLibrary, "DIG_Block_Out" );
			g_pNIDIGChangeMessageConfig = (SVIONIDIGChangeMessageConfigPtr)::GetProcAddress( g_hLibrary, "DIG_Change_Message_Config" );
			g_pNIDIGChangeMessageControl = (SVIONIDIGChangeMessageControlPtr)::GetProcAddress( g_hLibrary, "DIG_Change_Message_Control" );
			g_pNIDIGFilterConfig = (SVIONIDIGFilterConfigPtr)::GetProcAddress( g_hLibrary, "DIG_Filter_Config" );
			g_pNIDIGInLine = (SVIONIDIGInLinePtr)::GetProcAddress( g_hLibrary, "DIG_In_Line" );
			g_pNIDIGInPrt = (SVIONIDIGInPrtPtr)::GetProcAddress( g_hLibrary, "DIG_In_Prt" );
			g_pNIDIGLineConfig = (SVIONIDIGLineConfigPtr)::GetProcAddress( g_hLibrary, "DIG_Line_Config" );
			g_pNIDIGOutLine = (SVIONIDIGOutLinePtr)::GetProcAddress( g_hLibrary, "DIG_Out_Line" );
			g_pNIDIGOutPrt = (SVIONIDIGOutPrtPtr)::GetProcAddress( g_hLibrary, "DIG_Out_Prt" );
			g_pNIDIGPrtConfig = (SVIONIDIGPrtConfigPtr)::GetProcAddress( g_hLibrary, "DIG_Prt_Config" );
			g_pNIDIGPrtStatus = (SVIONIDIGPrtStatusPtr)::GetProcAddress( g_hLibrary, "DIG_Prt_Status" );
			g_pNIDIGSCANSetup = (SVIONIDIGSCANSetupPtr)::GetProcAddress( g_hLibrary, "DIG_SCAN_Setup" );
			g_pNIGetDAQDeviceInfo = (SVIONIGetDAQDeviceInfoPtr)::GetProcAddress( g_hLibrary, "Get_DAQ_Device_Info" );
			g_pNIGetNIDAQVersion = (SVIONIGetNIDAQVersionPtr)::GetProcAddress( g_hLibrary, "Get_NI_DAQ_Version" );
			g_pNIInitDABrds = (SVIONIInitDABrdsPtr)::GetProcAddress( g_hLibrary, "Init_DA_Brds" );

			if ( g_pNIConfigDAQEventMessage != NULL &&
			     g_pNIDIGBlockCheck != NULL &&
			     g_pNIDIGBlockClear != NULL &&
			     g_pNIDIGBlockIn != NULL &&
			     g_pNIDIGBlockOut != NULL &&
			     g_pNIDIGChangeMessageConfig != NULL &&
			     g_pNIDIGChangeMessageControl != NULL &&
			     g_pNIDIGFilterConfig != NULL &&
			     g_pNIDIGInLine != NULL &&
			     g_pNIDIGInPrt != NULL &&
			     g_pNIDIGLineConfig != NULL &&
			     g_pNIDIGOutLine != NULL &&
			     g_pNIDIGOutPrt != NULL &&
			     g_pNIDIGPrtConfig != NULL &&
			     g_pNIDIGPrtStatus != NULL &&
			     g_pNIDIGSCANSetup != NULL &&
			     g_pNIGetDAQDeviceInfo != NULL &&
			     g_pNIGetNIDAQVersion != NULL &&
			     g_pNIInitDABrds != NULL )
			{
				l_hrOk = S_OK;
			}
			else
			{
				l_hrOk = S_FALSE;

				g_NIClose();
			}
		}
	}

	return l_hrOk;
}

HRESULT g_NIClose()
{
	HRESULT l_hrOk = S_OK;

	g_pNIConfigDAQEventMessage = NULL;
	g_pNIDIGBlockCheck = NULL;
	g_pNIDIGBlockClear = NULL;
	g_pNIDIGBlockIn = NULL;
	g_pNIDIGBlockOut = NULL;
	g_pNIDIGChangeMessageConfig = NULL;
	g_pNIDIGChangeMessageControl = NULL;
	g_pNIDIGFilterConfig = NULL;
	g_pNIDIGInLine = NULL;
	g_pNIDIGInPrt = NULL;
	g_pNIDIGLineConfig = NULL;
	g_pNIDIGOutLine = NULL;
	g_pNIDIGOutPrt = NULL;
	g_pNIDIGPrtConfig = NULL;
	g_pNIDIGPrtStatus = NULL;
	g_pNIDIGSCANSetup = NULL;
	g_pNIGetDAQDeviceInfo = NULL;
	g_pNIGetNIDAQVersion = NULL;
	g_pNIInitDABrds = NULL;

	if ( g_hLibrary != NULL )
	{
		if ( ::FreeLibrary( g_hLibrary ) )
		{
			// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
			Sleep(0);
			g_hLibrary = NULL;
		}
		else
		{
			l_hrOk = S_FALSE;
		}
	}
	
	return l_hrOk;
}

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
																	 unsigned long p_ulCallbackAddr )
{
	HRESULT l_hrOk = S_FALSE;

	if ( g_pNIConfigDAQEventMessage != NULL )
	{
		char l_szChanStr[1024];

		if ( p_szChanStr == NULL )
		{
			strcpy( l_szChanStr, "" );
		}
		else
		{
			strcpy( l_szChanStr, p_szChanStr );
		}

		short sStatus = g_pNIConfigDAQEventMessage( p_sSlot, p_sMode, l_szChanStr, 
		                                            p_sDAQEvent, p_lTrigVal0, p_lTrigVal1,
																								p_ulSkipCnt, p_ulPreTrigScans, 
																								p_ulPostTrigScans, p_hwndHandle, 
																								p_sMsg, p_ulCallbackAddr );

		if ( sStatus == 0 )
		{
			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

HRESULT g_NIDIGBlockCheck( short p_sSlot, short p_sGrp,
													 unsigned long *p_pulRemaining )
{
	HRESULT l_hrOk = S_FALSE;

	if ( g_pNIDIGBlockCheck != NULL )
	{
		short sStatus = g_pNIDIGBlockCheck( p_sSlot, p_sGrp, p_pulRemaining );

		if ( sStatus == 0 )
		{
			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

HRESULT g_NIDIGBlockClear( short p_sSlot, short p_sGrp )
{
	HRESULT l_hrOk = S_FALSE;

	if ( g_pNIDIGBlockClear != NULL )
	{
		short sStatus = g_pNIDIGBlockClear( p_sSlot, p_sGrp );

		if ( sStatus == 0 )
		{
			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

HRESULT g_NIDIGBlockIn( short p_sSlot, short p_sGrp, short *p_psBuffer,
												unsigned long p_ulCnt )
{
	HRESULT l_hrOk = S_FALSE;

	if ( g_pNIDIGBlockIn != NULL )
	{
		short sStatus = g_pNIDIGBlockIn( p_sSlot, p_sGrp, p_psBuffer, p_ulCnt );

		if ( sStatus == 0 )
		{
			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

HRESULT g_NIDIGBlockOut( short p_sSlot, short p_sGrp, short *p_psBuffer,
												 unsigned long p_ulCnt )
{
	HRESULT l_hrOk = S_FALSE;

	if ( g_pNIDIGBlockOut != NULL )
	{
		short sStatus = g_pNIDIGBlockOut( p_sSlot, p_sGrp, p_psBuffer, p_ulCnt );

		if ( sStatus == 0 )
		{
			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

HRESULT g_NIDIGChangeMessageConfig( short p_sDeviceNumber, 
                                    short p_sOperation, 
																	  char *p_szRiseChanStr, 
																	  char *p_szFallChanStr, 
																	  HWND p_hwndHandle,
																	  short p_sMsg,
																	  unsigned long p_ulCallbackAddr )
{
	HRESULT l_hrOk = S_FALSE;

	if ( g_pNIDIGChangeMessageConfig != NULL )
	{
		char l_szRise[1024];
		char l_szFall[1024];

		if ( p_szRiseChanStr == NULL )
		{
			strcpy( l_szRise, "" );
		}
		else
		{
			strcpy( l_szRise, p_szRiseChanStr );
		}

		if ( p_szFallChanStr == NULL )
		{
			strcpy( l_szFall, "" );
		}
		else
		{
			strcpy( l_szFall, p_szFallChanStr );
		}

		short sStatus = g_pNIDIGChangeMessageConfig( p_sDeviceNumber, 
                                                 p_sOperation, 
																	               l_szRise, 
																	               l_szFall, 
																	               p_hwndHandle,
																	               p_sMsg,
																	               p_ulCallbackAddr );

		if ( sStatus == 0 )
		{
			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

HRESULT g_NIDIGChangeMessageControl( short p_sDeviceNumber, short p_sCtrlCode )
{
	HRESULT l_hrOk = S_FALSE;

	if ( g_pNIDIGChangeMessageControl != NULL )
	{
		short sStatus = g_pNIDIGChangeMessageControl( p_sDeviceNumber, p_sCtrlCode );

		if ( sStatus == 0 )
		{
			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

HRESULT g_NIDIGFilterConfig( short p_sDeviceNumber, short p_sMode, 
														 char *p_szChanStr, double p_dInterval )
{
	HRESULT l_hrOk = S_FALSE;

	if ( g_pNIDIGFilterConfig != NULL )
	{
		char l_szChanStr[1024];

		if ( p_szChanStr == NULL )
		{
			strcpy( l_szChanStr, "" );
		}
		else
		{
			strcpy( l_szChanStr, p_szChanStr );
		}

		short sStatus = g_pNIDIGFilterConfig( p_sDeviceNumber, p_sMode, 
														              l_szChanStr, p_dInterval );

		if ( sStatus == 0 )
		{
			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

HRESULT g_NIDIGInLine( short p_sSlot, short p_sPort, short p_sLineNum,
											 short *p_psState )
{
	HRESULT l_hrOk = S_FALSE;

	if ( g_pNIDIGInLine != NULL )
	{
		short sStatus = g_pNIDIGInLine( p_sSlot, p_sPort, p_sLineNum, p_psState );

		if ( sStatus == 0 )
		{
			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

HRESULT g_NIDIGInPrt( short p_sSlot, short p_sPort, long *p_plPattern )
{
	HRESULT l_hrOk = S_FALSE;

	if ( g_pNIDIGInPrt != NULL )
	{
		short sStatus = g_pNIDIGInPrt( p_sSlot, p_sPort, p_plPattern );

		if ( sStatus == 0 )
		{
			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

HRESULT g_NIDIGLineConfig( short p_sSlot, short p_sPort, short p_sLineNum, 
                           short p_psDirection )
{
	HRESULT l_hrOk = S_FALSE;

	if ( g_pNIDIGLineConfig != NULL )
	{
		short sStatus = g_pNIDIGLineConfig( p_sSlot, p_sPort, p_sLineNum, p_psDirection );

		if ( sStatus == 0 )
		{
			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

HRESULT g_NIDIGOutLine( short p_sSlot, short p_sPort, short p_sLineNum,
												short p_sState )
{
	HRESULT l_hrOk = S_FALSE;

	if ( g_pNIDIGOutLine != NULL )
	{
		short sStatus = g_pNIDIGOutLine( p_sSlot, p_sPort, p_sLineNum, p_sState );

		if ( sStatus == 0 )
		{
			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

HRESULT g_NIDIGOutPrt( short p_sSlot, short p_sPort, long p_lPattern )
{
	HRESULT l_hrOk = S_FALSE;

	if ( g_pNIDIGOutPrt != NULL )
	{
		short sStatus = g_pNIDIGOutPrt( p_sSlot, p_sPort, p_lPattern );

		if ( sStatus == 0 )
		{
			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

HRESULT g_NIDIGPrtConfig( short p_sSlot, short p_sPort,	short p_sLatchMode,
													short p_sDirection )
{
	HRESULT l_hrOk = S_FALSE;

	if ( g_pNIDIGPrtConfig != NULL )
	{
		short sStatus = g_pNIDIGPrtConfig( p_sSlot, p_sPort, p_sLatchMode, p_sDirection );

		if ( sStatus == 0 )
		{
			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

HRESULT g_NIDIGPrtStatus( short p_sSlot, short p_sPort, short *p_psStatus )
{
	HRESULT l_hrOk = S_FALSE;

	if ( g_pNIDIGPrtStatus != NULL )
	{
		short sStatus = g_pNIDIGPrtStatus( p_sSlot, p_sPort, p_psStatus );

		if ( sStatus == 0 )
		{
			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

HRESULT g_NIDIGSCANSetup( short p_sSlot, short p_sGrp, short p_sNumPorts,
													short *p_psPortList, short p_sDirection )
{
	HRESULT l_hrOk = S_FALSE;

	if ( g_pNIDIGSCANSetup != NULL )
	{
		short sStatus = g_pNIDIGSCANSetup( p_sSlot, p_sGrp, p_sNumPorts,
													             p_psPortList, p_sDirection );

		if ( sStatus == 0 )
		{
			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

HRESULT g_NIGetDAQDeviceInfo( short p_sDeviceNumber, unsigned long p_ulInfoType,
															unsigned long *p_ulInfoVal )
{
	HRESULT l_hrOk = S_FALSE;

	if ( g_pNIGetDAQDeviceInfo != NULL )
	{
		short sStatus = g_pNIGetDAQDeviceInfo( p_sDeviceNumber, p_ulInfoType, p_ulInfoVal );

		if ( sStatus == 0 )
		{
			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

HRESULT g_NIGetNIDAQVersion( unsigned long *p_pulVersion )
{
	HRESULT l_hrOk = S_FALSE;

	if ( g_pNIGetNIDAQVersion != NULL )
	{
		short sStatus = g_pNIGetNIDAQVersion( p_pulVersion );

		if ( sStatus == 0 )
		{
			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

HRESULT g_NIInitDABrds( short p_sSlot, short *p_psBrdCode )
{
	HRESULT l_hrOk = S_FALSE;

	if ( g_pNIInitDABrds != NULL )
	{
		short sStatus = g_pNIInitDABrds( p_sSlot, p_psBrdCode );

		if ( sStatus == 0 )
		{
			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVNI\SVNIDAQ.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 16:26:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   29 Oct 2007 16:07:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Sleep after Load and Free Library to help system resources.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Jan 2004 09:48:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated filter function to remove a crash due to issues when passing a string to the NI DLL.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Dec 2003 07:55:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated prt operation functions from short to long.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 14:31:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   New project added to interface I/O and Image processing for the National Instruments DAQ Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/