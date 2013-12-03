// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIOMEApp
// * .File Name       : $Workfile:   SVME.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:44:10  $
// ******************************************************************************

#include "stdafx.h"
#include "SVME.h"
#include "SVME14.h"

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

BEGIN_MESSAGE_MAP(SVIOMEApp, CWinApp)
	//{{AFX_MSG_MAP(SVIOMEApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVIOMEApp theApp;

SVIOMEApp::SVIOMEApp()
{
}

BOOL SVIOMEApp::InitInstance() 
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if ( g_ME14Open() == S_OK )
	{
		return CWinApp::InitInstance();
	}

	return FALSE;
}

int SVIOMEApp::ExitInstance() 
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	g_ME14Close();

	return CWinApp::ExitInstance();
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVME\SVME.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:44:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 14:27:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   New project added to interface I/O and Image processing for the Meilhaus Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/