//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDriveInitApp
//* .File Name       : $Workfile:   SVDriveInit.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 08:33:20  $
//******************************************************************************

// SVDriveInit.h : main header file for the SVDRIVEINIT application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// SVDriveInitApp:
// See SVDriveInit.cpp for the implementation of this class
//

class SVDriveInitApp : public CWinApp
{
public:
	SVDriveInitApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVDriveInitApp)
	public:
	virtual BOOL InitInstance() override;
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(SVDriveInitApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
