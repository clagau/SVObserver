//******************************************************************************
//* COPYRIGHT (c) 2006 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVImageTestApp.h"
#include "SVImageTestDlg.h"
#include "SVTestAcquisitionSubsystem.h"
#include "SVTestGigeAcquisitionSubsystem.h"
#include "SVUnloadDeviceDialog.h"
#include "Definitions/TextDefinesSvDef.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVUtilityLibrary/StringHelper.h"
#include <google\protobuf\stubs\common.h>
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const TCHAR* const cCameraMapping = _T("Camera Mapping");
const TCHAR* const cGigeCameraCount = _T("GigeCameraCount");

CWnd g_ImageTestDesktopWindow;

bool IsGigeDigitizer(LPCTSTR ProductName)
{
	std::string Name = ProductName;
	bool l_bOk = ( 0 == SvUl::CompareNoCase(Name, std::string(SvDef::SVO_PRODUCT_SVIM_X2_GD1A)) ||
					0 == SvUl::CompareNoCase(Name, std::string(SvDef::SVO_PRODUCT_SVIM_X2_GD2A)) ||
					0 == SvUl::CompareNoCase(Name, std::string(SvDef::SVO_PRODUCT_SVIM_X2_GD4A)) ||
					0 == SvUl::CompareNoCase(Name, std::string(SvDef::SVO_PRODUCT_SVIM_X2_GD8A)) ||
					0 == SvUl::CompareNoCase(Name, std::string(SvDef::SVO_PRODUCT_SVIM_NEO)) ) ? true : false;

	return l_bOk;
}

bool SVImageTestApp::IsGigeSystem() const
{
	return IsGigeDigitizer(m_productName.c_str());
}

/////////////////////////////////////////////////////////////////////////////
// SVImageTestApp

BEGIN_MESSAGE_MAP(SVImageTestApp, CWinApp)
	//{{AFX_MSG_MAP(SVImageTestApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVImageTestApp construction

SVImageTestApp::SVImageTestApp()
: m_iniLoader(m_iniFileInfo)
{
	m_svimIniFile = SvStl::GlobalPath::Inst().GetSVIMIniPath();
	m_hardwareIniFile = SvStl::GlobalPath::Inst().GetHardwareIniPath();

	TCHAR SystemDir[ MAX_PATH + 1 ];

	::GetSystemDirectory( SystemDir, MAX_PATH + 1 );
	m_oemIniFile = std::format( _T("{}\\OEMINFO.INI"), SystemDir);
}

SVImageTestApp::~SVImageTestApp()
{
}
/////////////////////////////////////////////////////////////////////////////
// The one and only SVImageTestApp object

SVImageTestApp theApp;

/////////////////////////////////////////////////////////////////////////////
// SVImageTestApp initialization

BOOL SVImageTestApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	// Used to hold a cwnd that is attached to the desktop.
	if( nullptr == g_ImageTestDesktopWindow.GetSafeHwnd() )
	{
		g_ImageTestDesktopWindow.Attach( ::GetDesktopWindow() );
	}

	// Read model number
	m_iniLoader.LoadIniFiles(m_svimIniFile.c_str(), m_oemIniFile.c_str(), m_hardwareIniFile.c_str());
	if (m_iniLoader.isModelNumberDecodable())
	{
		m_productName = m_iniLoader.GetInitialInfo().m_ProductName;

		int CameraCount = GetPrivateProfileInt(cCameraMapping, cGigeCameraCount, cCameraCount, SvStl::GlobalPath::Inst().GetSVIMIniPath());

		for (int i = 0; i < CameraCount; i++)
		{
			std::string CameraName;
			TCHAR pBuffer[128];
			memset(pBuffer, 0, 128);

			CameraName = std::format(_T("{}{}"), SvDef::cCameraFixedName, i + 1);
			GetPrivateProfileString(cCameraMapping, CameraName.c_str(), "", pBuffer, 128, SvStl::GlobalPath::Inst().GetSVIMIniPath());
			if (i < 4)
			{
				m_CameraIpAddress[i] = pBuffer;
			}
		}

		if ( nullptr == LoadDigitizer() )
		{
			AfxMessageBox("Dll Not Connected");
		}
		else
		{
			CSVImageTestDlg dlg;

			dlg.m_pSubsystem = m_pSubsystem;
			if (0 == SvUl::CompareNoCase(m_productName, std::string(SvDef::SVO_PRODUCT_SVIM_NEO)))
			{
				dlg.h_plcIODll = ::LoadLibrary(m_iniLoader.GetInitialInfo().m_CameraTriggerDLL.c_str());
			}

			m_pMainWnd = &dlg;
			dlg.DoModal();

			m_pMainWnd = nullptr;
		}
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int SVImageTestApp::ExitInstance() 
{
	UnLoadDigitizer();

	//Delete all global objects allocated by libprotobuf.
	google::protobuf::ShutdownProtobufLibrary();

	if( nullptr != g_ImageTestDesktopWindow.GetSafeHwnd() )
	{
		g_ImageTestDesktopWindow.Detach();
	}

	return CWinApp::ExitInstance();
}

SVTestAcquisitionSubsystem* SVImageTestApp::LoadDigitizer()
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

void SVImageTestApp::UnLoadDigitizer()
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

