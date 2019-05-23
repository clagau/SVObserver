//******************************************************************************
//* COPYRIGHT (c) 2006 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageTest.cpp
//* .File Name       : $Workfile:   SVImageTest.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 08:25:58  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVImageTest.h"
#include "SVImageTestDlg.h"
#include "SVTestAcquisitionSubsystem.h"
#include "SVTestGigeAcquisitionSubsystem.h"
#include "SVUnloadDeviceDialog.h"
#include "Definitions/TextDefineSVDef.h"
#include "SVMatroxLibrary/SVMatroxApplicationInterface.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVUtilityLibrary/StringHelper.h"
#include <google\protobuf\stubs\common.h>
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CWnd g_ImageTestDesktopWindow;

bool IsGigeDigitizer(LPCTSTR ProductName)
{
	std::string Name = ProductName;
	bool l_bOk = ( 0 == SvUl::CompareNoCase( Name, std::string(SvDef::SVO_PRODUCT_KONTRON_X2_GD1A ) ) ||
					0 == SvUl::CompareNoCase( Name, std::string(SvDef::SVO_PRODUCT_KONTRON_X2_GD2A ) ) ||
					0 == SvUl::CompareNoCase( Name, std::string(SvDef::SVO_PRODUCT_KONTRON_X2_GD4A ) ) ||
					0 == SvUl::CompareNoCase( Name, std::string(SvDef::SVO_PRODUCT_KONTRON_X2_GD8A ) ) ||
					0 == SvUl::CompareNoCase( Name, std::string(SvDef::SVO_PRODUCT_KONTRON_X2_GD8A_NONIO ) ) ) ? true : false;

	return l_bOk;
}

bool CSVImageTestApp::IsGigeSystem() const
{
	return IsGigeDigitizer(m_iniLoader.GetInitialInfo().m_ProductName.c_str());
}

/////////////////////////////////////////////////////////////////////////////
// CSVImageTestApp

BEGIN_MESSAGE_MAP(CSVImageTestApp, CWinApp)
	//{{AFX_MSG_MAP(CSVImageTestApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSVImageTestApp construction

CSVImageTestApp::CSVImageTestApp()
: m_pSubsystem(nullptr), m_iniLoader(m_iniFileInfo)
{
	m_svimIniFile = SvStl::GlobalPath::Inst().GetSVIMIniPath();
	m_hardwareIniFile = SvStl::GlobalPath::Inst().GetHardwareIniPath();

	TCHAR SystemDir[ MAX_PATH + 1 ];

	::GetSystemDirectory( SystemDir, MAX_PATH + 1 );
	m_oemIniFile = SvUl::Format( _T("%s\\OEMINFO.INI"), SystemDir);
}

CSVImageTestApp::~CSVImageTestApp()
{
}
/////////////////////////////////////////////////////////////////////////////
// The one and only CSVImageTestApp object

CSVImageTestApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSVImageTestApp initialization

BOOL CSVImageTestApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	// Startup Matrox App
	SVMatroxApplicationInterface::Startup();

	// Used to hold a cwnd that is attached to the desktop.
	if( nullptr == g_ImageTestDesktopWindow.GetSafeHwnd() )
	{
		g_ImageTestDesktopWindow.Attach( ::GetDesktopWindow() );
	}

	// Read model number
	if (ReadSVIMModelNo())
	{
		if ( nullptr == LoadDigitizer() )
		{
			AfxMessageBox("Dll Not Connected");
		}
		else
		{
			CSVImageTestDlg dlg;

			dlg.m_pSubsystem = m_pSubsystem;

			m_pMainWnd = &dlg;
			dlg.DoModal();

			m_pMainWnd = nullptr;
		}
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CSVImageTestApp::ExitInstance() 
{
	UnLoadDigitizer();

	//Delete all global objects allocated by libprotobuf.
	google::protobuf::ShutdownProtobufLibrary();

	// Shutdown Matrox App
	SVMatroxApplicationInterface::Shutdown();

	if( nullptr != g_ImageTestDesktopWindow.GetSafeHwnd() )
	{
		g_ImageTestDesktopWindow.Detach();
	}

	return CWinApp::ExitInstance();
}

bool CSVImageTestApp::ReadSVIMModelNo()
{
	HRESULT hr = m_iniLoader.LoadIniFiles( m_svimIniFile.c_str(), m_oemIniFile.c_str(), m_hardwareIniFile.c_str() );
	return (S_OK == hr) ? true : false;
}

SVTestAcquisitionSubsystem* CSVImageTestApp::LoadDigitizer()
{
	if (m_pSubsystem)
	{
		delete m_pSubsystem;

		m_pSubsystem = nullptr;
	}

	if (IsGigeSystem())
	{
		m_pSubsystem = new SVTestGigeAcquisitionSubsystem;
	}

	if (m_pSubsystem)
	{
		if( !m_pSubsystem->Create( CString( m_iniLoader.GetInitialInfo().m_DigitizerDLL.c_str() ) ) )
		{
			delete m_pSubsystem;

			m_pSubsystem = nullptr;
		}
	}

	return m_pSubsystem;
}

void CSVImageTestApp::UnLoadDigitizer()
{
	SVUnloadDeviceDialog dlg;
	dlg.Show( &g_ImageTestDesktopWindow );
	
	if (m_pSubsystem)
	{
		m_pSubsystem->Destroy();
		delete m_pSubsystem;
		m_pSubsystem = nullptr;
	}

	dlg.Destroy();
}

