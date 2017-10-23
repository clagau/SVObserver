// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOFileConfig
// * .File Name       : $Workfile:   SVOFileConfig.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   02 Oct 2013 08:43:42  $
// ******************************************************************************

// SVOFileConfig.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SVOFileConfig.h"
#include "SVOFileConfigDlg.h"

static const CLSID clsid =
{ 0xf4c4d491, 0xd660, 0x11d0, { 0x9b, 0x52, 0x0, 0x80, 0x5f, 0x71, 0x7d, 0xff } };

IMPLEMENT_SERIAL( SVOFileConfigApp, CWinApp, 0 );

/////////////////////////////////////////////////////////////////////////////
// SVOFileConfigApp

BEGIN_MESSAGE_MAP(SVOFileConfigApp, CWinApp)
	//{{AFX_MSG_MAP(SVOFileConfigApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVOFileConfigApp construction

SVOFileConfigApp::SVOFileConfigApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	pdlg = NULL;
}

SVOFileConfigApp::~SVOFileConfigApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	m_pMainWnd = NULL;

	if ( pdlg )
	{
		delete pdlg;
		pdlg = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// The one and only SVOFileConfigApp object

SVOFileConfigApp theApp;

/////////////////////////////////////////////////////////////////////////////
// SVOFileConfigApp initialization

BOOL SVOFileConfigApp::InitInstance()
{
	// OLE-Bibliotheken initialisieren
	if( ! AfxOleInit() )
	{
		return FALSE;
	}

	pdlg = new SVOFileConfigDlg;
	m_pMainWnd = pdlg;
	INT_PTR nResponse = pdlg->DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

void SVOFileConfigApp::Serialize(CArchive& ar) 
{
	CWinApp::Serialize( ar );
}

