// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOFileConfig
// * .File Name       : $Workfile:   SVOFileConfig.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   02 Oct 2013 08:43:42  $
// ******************************************************************************

// SVOFileConfig.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SVOFileConfig.h"
#include "SVOFileConfigDlg.h"

#include "SVOIODocClass.h"
#include "SVOIOFrameWndClass.h"
#include "SVOIOViewClass.h"

#include "SVOIPDocClass.h"
#include "SVOIPFrameWndClass.h"
#include "SVOIPViewClass.h"

#include <crtdbg.h>

static const CLSID clsid =
{ 0xf4c4d491, 0xd660, 0x11d0, { 0x9b, 0x52, 0x0, 0x80, 0x5f, 0x71, 0x7d, 0xff } };

IMPLEMENT_SERIAL( SVOFileConfigApp, CWinApp, 0 );

/////////////////////////////////////////////////////////////////////////////
// SVOFileConfigApp

BEGIN_MESSAGE_MAP(SVOFileConfigApp, CWinApp)
	//{{AFX_MSG_MAP(SVOFileConfigApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVOFileConfigApp construction

SVOFileConfigApp::SVOFileConfigApp()
: m_Inspection( NULL )
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	pdlg = NULL;
}

SVOFileConfigApp::~SVOFileConfigApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	m_pMainWnd = NULL;

	if ( pdlg )
	{
		delete pdlg;
		pdlg = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// The one and only SVOFileConfigApp object

SVOFileConfigApp theApp;

/////////////////////////////////////////////////////////////////////////////
// SVOFileConfigApp initialization

BOOL SVOFileConfigApp::InitInstance()
{
	// OLE-Bibliotheken initialisieren
	if( ! AfxOleInit() )
	{
//		AfxMessageBox( IDP_OLE_INIT_FAILED );
		return FALSE;
	}

	CMultiDocTemplate *pDocTemp = new CMultiDocTemplate( IDR_IODOC_MENU,
												                               RUNTIME_CLASS( SVOIODocClass ),
												                               RUNTIME_CLASS( SVOIOFrameWndClass ),
												                               RUNTIME_CLASS( SVOIOViewClass ) );
	
	pDocTemp->SetContainerInfo( IDR_IODOC_MENU );

	AddDocTemplate( pDocTemp );
	
	CMultiDocTemplate *pDocTemp1 = new CMultiDocTemplate( IDR_IPDOC_MENU,
												                                RUNTIME_CLASS( SVOIPDocClass ),
												                                RUNTIME_CLASS( SVOIPFrameWndClass ),
												                                RUNTIME_CLASS( SVOIPViewClass ) );
	
	pDocTemp1->SetContainerInfo( IDR_IPDOC_MENU );

	AddDocTemplate( pDocTemp1 );
	
	pdlg = new SVOFileConfigDlg;
	m_pMainWnd = pdlg;
	INT_PTR nResponse = pdlg->DoModal();
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

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

void SVOFileConfigApp::Serialize(CArchive& ar) 
{
	CWinApp::Serialize( ar );
}

void SVOFileConfigApp::LoadIODoc(LPCTSTR szFileName)
{
	CDocTemplate* pDocTemplate = NULL;
	POSITION pos = GetFirstDocTemplatePosition();
	if( pos )
	{
		pDocTemplate = GetNextDocTemplate( pos );
		if( pDocTemplate )
		{
		   pDocTemplate->OpenDocumentFile( szFileName, FALSE ) ;
		}
	}
}

void SVOFileConfigApp::LoadIPDoc(LPCTSTR szFileName)
{
	CDocTemplate* pDocTemplate = NULL;
	POSITION pos = GetFirstDocTemplatePosition();
	if( pos )
	{
		pDocTemplate = GetNextDocTemplate( pos );
		if( pDocTemplate )
		{
			pDocTemplate = GetNextDocTemplate( pos );
			if( pDocTemplate )
			{
			  pDocTemplate->OpenDocumentFile( szFileName, FALSE ) ;
			}
		}
	}
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVOFileConfig\SVOFileConfig.cpp_v  $
 * 
 *    Rev 1.1   02 Oct 2013 08:43:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 11:31:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   18 Sep 2012 18:06:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Jun 2007 10:11:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include removal of all VC6 constraints, project files, and workspace files.  The new VC8 project and solution files will replace the VC6 files with the new tighter compilation rules.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Jun 2007 14:02:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Nov 2002 10:00:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/