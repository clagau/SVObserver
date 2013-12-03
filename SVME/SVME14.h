// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIOME14
// * .File Name       : $Workfile:   SVME14.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:44:04  $
// ******************************************************************************

#ifndef _SVIOME14_H
#define _SVIOME14_H

// -------------------- Function Prototypes --------------------------------

typedef void (_stdcall *SVME14CallbackPtr) (void);

HRESULT g_ME14Open();                   // initialize function
HRESULT g_ME14Close();                  // shut down function

int g_ME14GetBoardVersion(int iBoardNumber, int *piVersion);
int g_ME14GetDLLVersion();
int g_ME14DIOSetMode(int iBoardNumber, int iPio, int iMode);
int g_ME14DIOSetPortDirection(int iBoardNumber, int iPort, int iDir);
int g_ME14DIGetBit(int iBoardNumber, int iPortNo, int iBitNo, int *piValue);
int g_ME14DIGetByte(int iBoardNumber, int iPortNo, int *piValue);
int g_ME14DOSetBit(int iBoardNumber, int iPortNo, int iBitNo, int iBitValue);
int g_ME14DOSetByte(int iBoardNumber, int iPortNo, int iValue);
int g_ME14CntRead(int iBoardNumber, int iCounter, int* piValue);
int g_ME14CntWrite(int iBoardNumber, int iCounter,int iMode, int iValue);
int g_ME14DisableInt(int iBoardNumber, int iServiceNo);
int g_ME14EnableInt(int iBoardNumber, SVME14CallbackPtr IrqFunc, int iServiceNo);
int g_ME14GetDrvErrMess(char *pcErrorText);

#endif   // _SVIOME14_H

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVME\SVME14.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:44:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 14:27:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   New project added to interface I/O and Image processing for the Meilhaus Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/