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
: m_Inspection( nullptr )
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	pdlg = nullptr;
}

SVOFileConfigApp::~SVOFileConfigApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	m_pMainWnd = nullptr;

	if ( pdlg )
	{
		delete pdlg;
		pdlg = nullptr;
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
	CDocTemplate* pDocTemplate = nullptr;
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
	CDocTemplate* pDocTemplate = nullptr;
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

