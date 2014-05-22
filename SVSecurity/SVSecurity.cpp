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
#include "SVSecurity.h"

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
// CSVSecurityApp

BEGIN_MESSAGE_MAP(CSVSecurityApp, CWinApp)
	//{{AFX_MSG_MAP(CSVSecurityApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CSVSecurityApp::CSVSecurityApp()
{
	free((void*)m_pszHelpFilePath);
	m_pszHelpFilePath = _tcsdup(_T("C:\\SVObserver\\bin\\SVObserver.chm"));
	EnableHtmlHelp();	
}

CSVSecurityApp::~CSVSecurityApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSVSecurityApp object

CSVSecurityApp theApp;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSecurity\SVSecurity.cpp_v  $
 * 
 *    Rev 1.1   20 May 2014 10:27:14   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  904
 * SCR Title:  Update SVObserver to use HtmlHelp instead of WinHelp
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Enabled the class to use HtmlHelp instead of WinHelp
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 17:01:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Sep 2012 18:43:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Sep 2012 17:35:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Jun 2005 15:06:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added PVCS Header
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
