// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : CSVCIApp
// * .File Name       : $Workfile:   SVCI.cpp  $
// * ----------------------------------------------------------------------------
// * .Module Type     :
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 17:58:58  $
// ******************************************************************************

#include "stdafx.h"
#include "SVCI.h"

#include "SVCISapera.h"

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
// CSVCIApp

BEGIN_MESSAGE_MAP(CSVCIApp, CWinApp)
	//{{AFX_MSG_MAP(CSVCIApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*
This constructor performs not operation at this time.  It is just a placeholder.
*/
CSVCIApp::CSVCIApp()
{
}

// The one and only CSVCIApp object
CSVCIApp theApp;

/*
This method handles the start-up issues when the DLL is loaded.
*/
BOOL CSVCIApp::InitInstance() 
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return CWinApp::InitInstance();
}

/*
This method handles the stutdown issues when the DLL is unloaded.
*/
int CSVCIApp::ExitInstance() 
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	g_CIDestroy( true );

	return CWinApp::ExitInstance();
}

