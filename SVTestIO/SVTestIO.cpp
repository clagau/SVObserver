//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVTestIOApp
//* .File Name       : $Workfile:   SVTestIO.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:27:36  $
//******************************************************************************

#include "stdafx.h"
#include "SVTestIO.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CSVTestIOApp

BEGIN_MESSAGE_MAP(CSVTestIOApp, CWinApp)
END_MESSAGE_MAP()

// CSVTestIOApp construction

CSVTestIOApp::CSVTestIOApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CSVTestIOApp object

CSVTestIOApp theApp;

// CSVTestIOApp initialization

BOOL CSVTestIOApp::InitInstance()
{
	CWinApp::InitInstance();

	m_pTestIODlg = new SVTestIODlg;
	if( m_pTestIODlg != NULL )
	{
		CWnd DTWin;
		DTWin.Attach( ::GetDesktopWindow());
		m_pTestIODlg->Create(IDD_TEST_IO_DLG, &DTWin );
		DTWin.Detach( );
		m_pTestIODlg->ModifyStyleEx( 0, WS_EX_TOPMOST);
		//m_pTestIODlg->ShowWindow( SW_SHOW );
	}

	return TRUE;
}

BOOL CSVTestIOApp::ExitInstance()
{

	if( m_pTestIODlg )
	{
		m_pTestIODlg->DestroyWindow();
		delete m_pTestIODlg;
		m_pTestIODlg = NULL;
	}


	return CWinApp::ExitInstance();

}

