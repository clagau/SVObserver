// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIOTEST
// * .File Name       : $Workfile:   SVIOTEST.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:50:16  $
// ******************************************************************************

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVIOLibrary/SVIOTriggerLoadLibraryClass.h"

static int SVMEBoardNumber = 0;

class CSVIOTESTApp : public CWinApp
{
public:
	CSVIOTESTApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVIOTESTApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSVIOTESTApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	SVIOTriggerLoadLibraryClass m_svTriggers;
};
