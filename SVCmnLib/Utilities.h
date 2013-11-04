// ******************************************************************************
// * COPYRIGHT (c) 2001 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : Utilities.h
// * .File Name       : $Workfile:   Utilities.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 18:10:12  $
// ******************************************************************************

#ifndef __UTILITIES_H
#define __UTILITIES_H

#include <errno.h>

BOOL CreateDirPath(CString szPath);
HRESULT GetSystemErrorText(DWORD dwError, CString & szMsg);
DWORD GetLastSystemErrorText(CString & szMsg);
void DisplayLastSystemError();
BOOL CopyDir(LPCTSTR szOrigPath, LPCTSTR szNewPath);
BOOL DeleteDir(LPCTSTR szPath);

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCmnLib\Utilities.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 18:10:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Jul 2001 21:20:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  215
 * SCR Title:  Fix SVObserver remote command processing problem with MS-DOS version SVFocus
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added DeleteDir and CopyDir functions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/