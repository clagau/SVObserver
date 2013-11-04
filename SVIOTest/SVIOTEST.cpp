// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIOTEST
// * .File Name       : $Workfile:   SVIOTEST.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   06 May 2013 19:20:20  $
// ******************************************************************************

#include "stdafx.h"
#include "SVIOTEST.h"
#include "SVIOTESTDlg.h"

#include "SVOMFCLibrary/SVOINIClass.h"
#include "SVOMFCLibrary/SVOIniLoader.h"

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

/////////////////////////////////////////////////////////////////////////////
// The one and only CSVIOTESTApp object

CSVIOTESTApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSVIOTESTApp initialization

BOOL CSVIOTESTApp::InitInstance()
{
	SVOINIClass l_svINI;

	BSTR l_bstrValue = NULL;

	CString l_csTrigger;
	CString l_csDigital;
	CString l_csDDE;


	TCHAR l_szSystemDir[ MAX_PATH + 1 ];
	CString l_csSystemDir;

	::GetSystemDirectory( l_szSystemDir, MAX_PATH + 1 );
	l_csSystemDir.Format( "%s\\OEMINFO.INI", l_szSystemDir );

	SVOIniLoader l_iniLoader;
	HRESULT l_hrOk = l_iniLoader.Load("C:\\SVObserver\\bin\\SVIM.INI", l_csSystemDir, "C:\\SVObserver\\bin\\HARDWARE.INI");

	bool l_bOk = true;

	l_bOk = m_svTriggers.Open( l_iniLoader.m_csTriggerDLL ) == S_OK && l_bOk;
	l_bOk = SVIOConfigurationInterfaceClass::Instance().OpenDigital( l_iniLoader.m_csDigitalDLL ) == S_OK && l_bOk;

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

		int nResponse = dlg.DoModal();
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

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVIOTest\SVIOTEST.cpp_v  $
 * 
 *    Rev 1.1   06 May 2013 19:20:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   16 Apr 2013 14:07:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 12:50:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   15 Aug 2012 14:27:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new Singlton version of the SVIOConfigurationInterfaceClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   31 Jul 2012 11:54:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  776
 * SCR Title:  Modify SVIOTest to use the same parameters as SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified SVIOTest to use the SVOIniLoader to get the dll name instead of the SVIOTest.ini.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   31 Jul 2012 11:50:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  776
 * SCR Title:  Modify SVIOTest to use the same parameters as SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added code to use SVOIniLoader to get io dll instead of using the SVIOTest.ini.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   15 Dec 2009 12:22:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   16 Oct 2009 12:10:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  666
 * SCR Title:  Change command format for  X-Series IO
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated test application to remove the special debug cases for the DLL names.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   11 Sep 2008 08:25:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised for Software Trigger
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   22 Jun 2007 09:48:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include removal of all VC6 constraints, project files, and workspace files.  The new VC8 project and solution files will replace the VC6 files with the new tighter compilation rules.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   20 Jun 2007 13:09:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   27 Jun 2006 09:31:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  564
 * SCR Title:  Add configurability to SVTVicLpt DLL to set the system type
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   New functionality in Test App to test new functionality in SVParallelIO board.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   13 Dec 2005 14:04:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  513
 * SCR Title:  Add Parallel Port I/O to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated default DLLs to use the Parallel Port DLL instead of the Mielhaus DLL.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Sep 2003 16:56:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Change trigger to use SVIOTriggerLoadLibraryClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Aug 2003 15:08:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated constructor to fix ini interface function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
