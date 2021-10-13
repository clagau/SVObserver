//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSecurity.cpp
//* .File Name       : $Workfile:   SVSecurity.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   20 May 2014 10:27:14  $
//******************************************************************************

#include "stdafx.h"
#include "SVSecurityApp.h"

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
// SVSecurityApp

BEGIN_MESSAGE_MAP(SVSecurityApp, CWinApp)
	//{{AFX_MSG_MAP(SVSecurityApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVSecurityApp::SVSecurityApp()
{
	free((void*)m_pszHelpFilePath);
	m_pszHelpFilePath = _tcsdup(_T("C:\\SVObserver\\bin\\SVObserver.chm"));
	EnableHtmlHelp();	
}

SVSecurityApp::~SVSecurityApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only SVSecurityApp object

SVSecurityApp theApp;

