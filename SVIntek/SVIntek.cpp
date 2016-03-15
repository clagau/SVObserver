// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : CSVIntekApp
// * .File Name       : $Workfile:   SVIntek.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:32:58  $
// ******************************************************************************

#include "stdafx.h"
#include "SVIntek.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// The one and only CSVIntekApp object

CSVIntekApp g_svTheApp;

/////////////////////////////////////////////////////////////////////////////
// CSVIntekApp

BEGIN_MESSAGE_MAP(CSVIntekApp, CWinApp)
	//{{AFX_MSG_MAP(CSVIntekApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSVIntekApp construction

CSVIntekApp::CSVIntekApp()
{
}

BOOL CSVIntekApp::InitInstance() 
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	BOOL l_bOk = FALSE;

	if ( m_svSystem.Open() == S_OK )
	{
		l_bOk = CWinApp::InitInstance();
	}

	return l_bOk;
}

int CSVIntekApp::ExitInstance() 
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_svSystem.Close();

	return CWinApp::ExitInstance();
}

