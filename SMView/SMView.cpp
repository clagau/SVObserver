
//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file SMView.cpp 
/// All Rights Reserved 
//*****************************************************************************

///  This is the main header file for the application.  It includes other
//project specific headers (including Resource.h) and declares the
//CShareViewApp application class.
//******************************************************************************

#include "stdafx.h"
#include "SMView.h"
#include "SMViewDlg.h"
#define M_MIL_USE_SAFE_TYPE 0
#include <mil.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CShareViewApp

BEGIN_MESSAGE_MAP(CSMViewApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CShareViewApp construction

CSMViewApp::CSMViewApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CShareViewApp object

CSMViewApp theApp;


// CShareViewApp initialization

BOOL CSMViewApp::InitInstance()
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

	/// Allocate MilSystem
	SVMatroxInt AppId = MappAlloc(M_DEFAULT, M_NULL);
	if (AppId == M_NULL)
	{
		AfxMessageBox(_T("Mil System could not be allocated. SMView will be closed"));
		return FALSE;
	}

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CSMViewDlg dlg;
	m_pMainWnd = &dlg;
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

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	MappFree(AppId);

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

