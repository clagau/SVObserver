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
	TCHAR szSystemDir[ MAX_PATH + 1 ];
	CString csSystemDir;

	::GetSystemDirectory( szSystemDir, MAX_PATH + 1 );
	csSystemDir.Format( "%s\\OEMINFO.INI", szSystemDir );

	SvLib::InitialInformation initialInfo;
	SvLib::SVOIniLoader iniLoader(initialInfo);

	iniLoader.LoadIniFiles(SvStl::GlobalPath::Inst().GetSVIMIniPath(), csSystemDir,  SvStl::GlobalPath::Inst().GetHardwareIniPath());

	bool bOk = true;
	HMODULE dllHandle = ::LoadLibrary(iniLoader.GetInitialInfo().m_TriggerResultDll.c_str());
	// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
	Sleep(0);

	bOk = S_OK == m_svTriggers.Open(dllHandle) && bOk;

	bOk = S_OK == SVIOConfigurationInterfaceClass::Instance().OpenDigital(dllHandle) && bOk;

	if ( ! bOk )
	{
		AfxMessageBox("Dll Not Connected");
	}
	else
	{
		SVIOTestDlg dlg;

		m_pMainWnd = &dlg;

		dlg.m_psvTriggers = &m_svTriggers;
		dlg.m_csDigital = iniLoader.GetInitialInfo().m_TriggerResultDll.c_str();
		dlg.m_csTrigger = iniLoader.GetInitialInfo().m_TriggerResultDll.c_str();
		dlg.m_lSystemType = atol(iniLoader.GetInitialInfo().m_IOBoard.c_str());

		dlg.DoModal();
	}
	SVIOConfigurationInterfaceClass::Instance().Shutdown();

	m_svTriggers.Close();
	::FreeLibrary(dllHandle);

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

