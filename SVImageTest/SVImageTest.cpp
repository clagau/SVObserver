//******************************************************************************
//* COPYRIGHT (c) 2006 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageTest.cpp
//* .File Name       : $Workfile:   SVImageTest.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 11:08:38  $
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
			int nResponse = dlg.DoModal();

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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageTest\SVImageTest.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 11:08:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   13 Aug 2012 11:34:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  773
 * SCR Title:  Update X2 GigE Model
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed X2B-GD8A model/configuration information from application.  Converted source code to use X2-GD8A instead of X2B-GD8A.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   09 Jul 2012 13:17:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  774
 * SCR Title:  Update SVObserver to include the new SVIM X2B Product
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added X2_GD4A for compatibility with legacy product.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   02 Jul 2012 14:17:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  774
 * SCR Title:  Update SVObserver to include the new SVIM X2B Product
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new configuration type SVIM X2B-GD8A.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   02 Jul 2012 12:44:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  773
 * SCR Title:  Update X2 GigE Model
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new configuration type SVIM X2-GD8A.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   15 Dec 2009 11:43:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   17 Jun 2009 08:35:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added IsGigeSystem method
 * Revised IsDigitalDigitizer method
 * Revised InitInstance to start matrox framework
 * Revised ExitInstance to stop matrox framework
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   09 Jun 2008 09:39:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Removed 3d controls section that is obsolete
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   05 Mar 2008 09:41:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  622
 * SCR Title:  Update SVImageTest Application to Acquire on Multiple Cameras Simultaneously
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Added functionality for processing multiple camera acquisitions at the same time.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Feb 2007 08:53:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  592
 * SCR Title:  Add  X3 product type to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Support for X3 Product Type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Nov 2006 12:42:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use Acquisiton class.
 * Revised to read SVIM Model number.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
