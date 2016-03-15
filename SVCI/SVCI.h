// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : CSVCIApp
// * .File Name       : $Workfile:   SVCI.h  $
// * ----------------------------------------------------------------------------
// * .Module Type     :
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 17:58:58  $
// ******************************************************************************

#ifndef SVCI_H
#define SVCI_H

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/*
This class manages the interface that controls the DLL interface to the application and the operation system.
*/
class CSVCIApp : public CWinApp
{
public:
	CSVCIApp();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVCIApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSVCIApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

