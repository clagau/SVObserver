//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : InitializeIOSubsystem.cpp
//* .File Name       : $Workfile:   InitializeIOSubsystem.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   18 Sep 2014 11:35:20  $
//******************************************************************************

#include "stdafx.h"
#include "InitializeIOSubsystem.h"
#include "InitializeIOSubsystemDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CInitializeIOSubsystemApp

BEGIN_MESSAGE_MAP(CInitializeIOSubsystemApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// CInitializeIOSubsystemApp construction

CInitializeIOSubsystemApp::CInitializeIOSubsystemApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CInitializeIOSubsystemApp object

CInitializeIOSubsystemApp theApp;

// CInitializeIOSubsystemApp initialization

BOOL CInitializeIOSubsystemApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();
		
	CInitializeIOSubsystemDlg dlg;
	m_pMainWnd = &dlg;
	CString CommandLine( m_lpCmdLine );
	CommandLine.MakeUpper();
	if( -1 != CommandLine.Find( _T("-B") ) )
	{
		dlg.m_bShutdown = true;
	}
	if( -1 != CommandLine.Find( _T("-T") ) )
	{
		dlg.m_bToggle = true;
	}
	if( -1 != CommandLine.Find( _T("-?") ) )
	{
		MessageBox( NULL, _T("Command Line Arg -b or -B to automatically Reboot!\nCommand Line Arg -t or -T to Toggle output"), _T("Help"), MB_OK );
	}
	INT_PTR nResponse = dlg.DoModal();
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
