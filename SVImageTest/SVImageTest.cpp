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

#include "stdafx.h"
#include "SVImageTest.h"
#include "SVMatroxLibrary/SVMatroxApplicationInterface.h"
#include "SVImageTestDlg.h"
#include "SVTestAcquisitionClass.h"
#include "SVTestIntekAcquisitionClass.h"
#include "SVTestIntekAcquisitionSubsystem.h"
#include "SVTestGigeAcquisitionSubsystem.h"
#include "SV1394CameraFileLibrary/SVDCamFactoryRegistrar.h"
#include "SVUnloadDeviceDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CWnd g_ImageTestDesktopWindow;

bool IsAnalogDigitizer(LPCTSTR p_productName) 
{ 
	CString l_productName = p_productName;
	bool l_bOk = (l_productName.CompareNoCase( SVO_PRODUCT_CORECO_A4_MONO ) == 0 ||
					l_productName.CompareNoCase( SVO_PRODUCT_CORECO_A2_MONO ) == 0 ||
					l_productName.CompareNoCase( SVO_PRODUCT_CORECO_A1_RGB ) == 0) ? true : false;

	return l_bOk;
}

bool IsDigitalDigitizer(LPCTSTR p_productName)
{
	CString l_productName = p_productName;
	bool l_bOk = (l_productName.CompareNoCase( SVO_PRODUCT_MATROX_D1 ) == 0 ||
					l_productName.CompareNoCase( SVO_PRODUCT_MATROX_D2 ) == 0 ||
					l_productName.CompareNoCase( SVO_PRODUCT_MATROX_D3 ) == 0 ||
					l_productName.CompareNoCase( SVO_PRODUCT_INTEK_D1 ) == 0 ||
					l_productName.CompareNoCase( SVO_PRODUCT_INTEK_D3 ) == 0 ||
					l_productName.CompareNoCase( SVO_PRODUCT_KONTRON_X2 ) == 0 ||
					l_productName.CompareNoCase( SVO_PRODUCT_KONTRON_X2_GD2A ) == 0 ||
					l_productName.CompareNoCase( SVO_PRODUCT_KONTRON_X2_GD4A ) == 0 ||
					l_productName.CompareNoCase( SVO_PRODUCT_KONTRON_X2_GD8A ) == 0 ) ? true : false;

	return l_bOk;
}

bool IsGigeDigitizer(LPCTSTR p_productName)
{
	CString l_productName = p_productName;
	bool l_bOk = ( l_productName.CompareNoCase( SVO_PRODUCT_KONTRON_X2_GD2A ) == 0 ||
					l_productName.CompareNoCase( SVO_PRODUCT_KONTRON_X2_GD4A ) == 0 ||
					l_productName.CompareNoCase( SVO_PRODUCT_KONTRON_X2_GD8A ) == 0 ) ? true : false;

	return l_bOk;
}

bool CSVImageTestApp::IsDigitizerAnalog() const
{
	return IsAnalogDigitizer(m_iniLoader.m_csProductName);
}

bool CSVImageTestApp::IsGigeSystem() const
{
	return IsGigeDigitizer(m_iniLoader.m_csProductName);
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
{
	m_svimIniFile = "C:\\SVObserver\\bin\\SVIM.INI";
	m_hardwareIniFile = "C:\\SVObserver\\bin\\HARDWARE.INI";

	TCHAR l_szSystemDir[ MAX_PATH + 1 ];

	::GetSystemDirectory( l_szSystemDir, MAX_PATH + 1 );
	m_oemIniFile.Format( "%s\\OEMINFO.INI", l_szSystemDir );

	m_pSubsystem = NULL;
	SVDCamFactoryRegistrar::Register();
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
	if( g_ImageTestDesktopWindow.GetSafeHwnd() == NULL )
	{
		g_ImageTestDesktopWindow.Attach( ::GetDesktopWindow() );
	}

	// Read model number
	if (ReadSVIMModelNo())
	{
		if ( LoadDigitizer() == NULL )
		{
			AfxMessageBox("Dll Not Connected");
		}
		else
		{
			CSVImageTestDlg dlg;

			dlg.m_pSubsystem = m_pSubsystem;

			m_pMainWnd = &dlg;
			dlg.DoModal();

			m_pMainWnd = NULL;
		}
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CSVImageTestApp::ExitInstance() 
{
	UnLoadDigitizer();

	// Shutdown Matrox App
	SVMatroxApplicationInterface::Shutdown();

	if( g_ImageTestDesktopWindow.GetSafeHwnd() != NULL )
	{
		g_ImageTestDesktopWindow.Detach();
	}

	return CWinApp::ExitInstance();
}

bool CSVImageTestApp::ReadSVIMModelNo()
{
	HRESULT hr = m_iniLoader.Load(m_svimIniFile, m_oemIniFile, m_hardwareIniFile);
	return (hr == S_OK) ? true : false;
}

SVTestAcquisitionSubsystem* CSVImageTestApp::LoadDigitizer()
{
	if (m_pSubsystem)
	{
		delete m_pSubsystem;

		m_pSubsystem = NULL;
	}

	if (IsDigitizerAnalog())
	{
		m_pSubsystem = new SVTestAcquisitionSubsystem;
	}
	else
	{
		if (IsGigeSystem())
		{
			m_pSubsystem = new SVTestGigeAcquisitionSubsystem;
		}
		else
		{
			m_pSubsystem = new SVTestIntekAcquisitionSubsystem;
		}
	}

	if (m_pSubsystem)
	{
		if( !m_pSubsystem->Create(m_iniLoader.m_csDigitizerDLL ) )
		{
			delete m_pSubsystem;

			m_pSubsystem = NULL;
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
		m_pSubsystem = NULL;
	}

	dlg.Destroy();
}

