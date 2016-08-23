// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIOTEST
// * .File Name       : $Workfile:   SVIOTEST.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   01 Oct 2013 09:21:32  $
// ******************************************************************************

#include "stdafx.h"

#include "SVIOTEST.h"
#include "SVIOTESTDlg.h"
#include "SVOMFCLibrary/SVOINIClass.h"
#include "SVOMFCLibrary/SVOIniLoader.h"
#include "SVStatusLibrary/GlobalPath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BOOL SVIOInitOutputs();

/////////////////////////////////////////////////////////////////////////////
// CSVIOTESTApp

BEGIN_MESSAGE_MAP(CSVIOTESTApp, CWinApp)
	//{{AFX_MSG_MAP(CSVIOTESTApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSVIOTESTApp construction

CSVIOTESTApp::CSVIOTESTApp()
{
	SVIOConfigurationInterfaceClass::Instance().Shutdown();
}

CSVIOTESTApp::~CSVIOTESTApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSVIOTESTApp object

CSVIOTESTApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSVIOTESTApp initialization

BOOL CSVIOTESTApp::InitInstance()
{
	SVOINIClass l_svINI;

	BSTR l_bstrValue = nullptr;

	CString l_csTrigger;
	CString l_csDigital;
	CString l_csDDE;


	TCHAR l_szSystemDir[ MAX_PATH + 1 ];
	CString l_csSystemDir;

	::GetSystemDirectory( l_szSystemDir, MAX_PATH + 1 );
	l_csSystemDir.Format( "%s\\OEMINFO.INI", l_szSystemDir );

	SVOIniLoader l_iniLoader;
	HRESULT l_hrOk = l_iniLoader.Load(SvStl::GlobalPath::Inst().GetSVIMIniPath(), l_csSystemDir,  SvStl::GlobalPath::Inst().GetHardwareIniPath());

	bool l_bOk = true;

	l_bOk = S_OK == m_svTriggers.Open( l_iniLoader.m_csTriggerDLL ) && l_bOk;
	l_bOk = S_OK == SVIOConfigurationInterfaceClass::Instance().OpenDigital( l_iniLoader.m_csDigitalDLL ) && l_bOk;

	if ( ! l_bOk )
	{
		AfxMessageBox("Dll Not Connected");
	}
	else
	{
		CSVIOTESTDlg dlg;

		m_pMainWnd = &dlg;

		dlg.m_psvTriggers = &m_svTriggers;
		dlg.m_csDigital = l_iniLoader.m_csDigitalDLL;
		dlg.m_csTrigger = l_iniLoader.m_csTrigger;
		dlg.m_lSystemType = atol(l_iniLoader.m_csIOBoard);

		dlg.DoModal();
	}

	#ifdef _DEBUG	
		l_svINI.SetValue( "IO", "DebugTriggerDLLName", l_csTrigger, "./SVIOTEST.INI" );
		l_svINI.SetValue( "IO", "DebugDigitalDLLName", l_csDigital, "./SVIOTEST.INI" );
	#else // _DEBUG
		l_svINI.SetValue( "IO", "TriggerDLLName", l_csTrigger, "./SVIOTEST.INI" );
		l_svINI.SetValue( "IO", "DigitalDLLName", l_csDigital, "./SVIOTEST.INI" );
	#endif // _DEBUG



	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

