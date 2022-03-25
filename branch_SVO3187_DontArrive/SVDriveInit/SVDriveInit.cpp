//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDriveInitApp
//* .File Name       : $Workfile:   SVDriveInit.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   30 Sep 2013 14:58:52  $
//******************************************************************************

// SVDriveInit.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SVDriveInit.h"
#include "SVDriveInitDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVDriveInitApp

BEGIN_MESSAGE_MAP(SVDriveInitApp, CWinApp)
	//{{AFX_MSG_MAP(SVDriveInitApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVDriveInitApp construction

SVDriveInitApp::SVDriveInitApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only SVDriveInitApp object

SVDriveInitApp theApp;

/////////////////////////////////////////////////////////////////////////////
// SVDriveInitApp initialization

BOOL SVDriveInitApp::InitInstance()
{
	AfxEnableControlContainer();

	SVDriveInitDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

