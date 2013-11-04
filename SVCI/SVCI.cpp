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

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCI\SVCI.cpp_v  $
 * 
 *    Rev 1.0   18 Apr 2013 17:58:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 May 2011 09:50:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  702
 * SCR Title:  Updated Coreco Acquisition Functionality to Include Xcelera-AN Board
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the Dalsa/Coreco libraries directly, change the acquisition mode to CORXFER_VAL_CYCLE_MODE_SYNCHRONOUS_WITH_TRASH, and change the buffer allocation to CORBUFFER_VAL_TYPE_SCATTER_GATHER.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Feb 2010 08:40:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
