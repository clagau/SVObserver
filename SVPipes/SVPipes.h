// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVPipes
// * .File Name       : $Workfile:   SVPipes.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 14:02:02  $
// ******************************************************************************

#if !defined(AFX_SVPIPES_H__F7A37B08_CDC6_11D3_A859_00106F000C7C__INCLUDED_)
#define AFX_SVPIPES_H__F7A37B08_CDC6_11D3_A859_00106F000C7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSVPipesApp
// See SVPipes.cpp for the implementation of this class
//

//##ModelId=38B581340000
class CSVPipesApp : public CWinApp
{
public:
	//##ModelId=38B581340010
	CSVPipesApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVPipesApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSVPipesApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVPIPES_H__F7A37B08_CDC6_11D3_A859_00106F000C7C__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVPipes\SVPipes.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 14:02:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Mar 22 2000 17:29:34   mike
 * Initial revision.
*/