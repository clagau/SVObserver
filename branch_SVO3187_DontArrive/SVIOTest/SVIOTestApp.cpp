// ******************************************************************************
// * COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIOTEST
// * .File Name       : $Workfile:   SVIOTEST.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   01 Oct 2013 09:21:32  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVIOTestApp.h"
#include "SVIOTestDlg.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVLibrary/InitialInformation.h"
#include "SVLibrary/SVOIniLoader.h"
#include "SVStatusLibrary/GlobalPath.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BOOL SVIOInitOutputs();

/////////////////////////////////////////////////////////////////////////////
// SVIOTestApp

BEGIN_MESSAGE_MAP(SVIOTestApp, CWinApp)
	//{{AFX_MSG_MAP(SVIOTestApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVIOTestApp construction

SVIOTestApp::SVIOTestApp()
{
	SVIOConfigurationInterfaceClass::Instance().Shutdown();
}

SVIOTestApp::~SVIOTestApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only SVIOTestApp object

SVIOTestApp theApp;

/////////////////////////////////////////////////////////////////////////////
// SVIOTestApp initialization

BOOL SVIOTestApp::InitInstance()
{
	TCHAR l_szSystemDir[ MAX_PATH + 1 ];
	CString l_csSystemDir;

	::GetSystemDirectory( l_szSystemDir, MAX_PATH + 1 );
	l_csSystemDir.Format( "%s\\OEMINFO.INI", l_szSystemDir );

	SvLib::InitialInformation initialInfo;
	SvLib::SVOIniLoader l_iniLoader(initialInfo);

	l_iniLoader.LoadIniFiles(SvStl::GlobalPath::Inst().GetSVIMIniPath(), l_csSystemDir,  SvStl::GlobalPath::Inst().GetHardwareIniPath());

	bool l_bOk = true;

	l_bOk = S_OK == m_svTriggers.Open( l_iniLoader.GetInitialInfo().m_TriggerDLL.c_str() ) && l_bOk;

	l_bOk = S_OK == SVIOConfigurationInterfaceClass::Instance().OpenDigital(l_iniLoader.GetInitialInfo().m_DigitalIODLL.c_str() ) && l_bOk;

	if ( ! l_bOk )
	{
		AfxMessageBox("Dll Not Connected");
	}
	else
	{
		SVIOTestDlg dlg;

		m_pMainWnd = &dlg;

		dlg.m_psvTriggers = &m_svTriggers;
		dlg.m_csDigital = l_iniLoader.GetInitialInfo().m_DigitalIODLL.c_str();
		dlg.m_csTrigger = l_iniLoader.GetInitialInfo().m_TriggerDLL.c_str();
		dlg.m_lSystemType = atol(l_iniLoader.GetInitialInfo().m_IOBoard.c_str());

		dlg.DoModal();
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

