//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEmptyIO
//* .File Name       : $Workfile:   SVEmptyIO.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:13:30  $
//******************************************************************************

#include "stdafx.h"
#include "SVEmptyIO.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
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


// CSVEmptyIOApp

BEGIN_MESSAGE_MAP(CSVEmptyIOApp, CWinApp)
END_MESSAGE_MAP()


// CSVEmptyIOApp construction

CSVEmptyIOApp::CSVEmptyIOApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CSVEmptyIOApp object

CSVEmptyIOApp theApp;


// CSVEmptyIOApp initialization

BOOL CSVEmptyIOApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVEmptyIO\SVEmptyIO.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 09:13:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/