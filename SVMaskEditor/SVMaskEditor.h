// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMaskEditor
// * .File Name       : $Workfile:   SVMaskEditor.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   26 Apr 2013 14:06:48  $
// ******************************************************************************

#if !defined(AFX_SVMASKEDITOR_H__C87C9B7A_E6DC_11D2_A77B_00106F000C7D__INCLUDED_)
#define AFX_SVMASKEDITOR_H__C87C9B7A_E6DC_11D2_A77B_00106F000C7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// SVMaskEditor.h : main header file for SVMASKEDITOR.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols

#include "SVOMFCLibrary\SVGraphix.h"   // Loacted in SVObserver Source Code!
#include "SVMatroxLibrary\SVMatroxLibrary.h"


#pragma comment( lib, "C:/Program Files/Matrox Imaging/Mil/LIB/mil" )
#pragma comment( lib, "C:/Program Files/Matrox Imaging/Mil/LIB/milim" )

/////////////////////////////////////////////////////////////////////////////
// SVMaskEditorApp : See SVMaskEditor.cpp for implementation.

class SVMaskEditorApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;


// Global Functions...
SVMatroxBuffer SVBitmapToMilBuffer( HBITMAP HBM );













//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVMASKEDITOR_H__C87C9B7A_E6DC_11D2_A77B_00106F000C7D__INCLUDED)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVMaskEditor\SVMaskEditor.h_v  $
 * 
 *    Rev 1.0   26 Apr 2013 14:06:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   24 Jul 2007 10:18:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to Matrox Imaging Library 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
