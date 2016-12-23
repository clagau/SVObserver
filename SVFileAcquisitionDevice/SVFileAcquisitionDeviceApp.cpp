//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileAcquisitionDeviceApp
//* .File Name       : $Workfile:   SVFileAcquisitionDeviceApp.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:28:44  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "CameraLibrary\SVDeviceParams.h"
#include "SVFileAcquisitionDeviceApp.h"
#pragma endregion Includes

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
// SVFileAcquisitionDeviceApp

BEGIN_MESSAGE_MAP(SVFileAcquisitionDeviceApp, CWinApp)
	//{{AFX_MSG_MAP(SVFileAcquisitionDeviceApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVFileAcquisitionDeviceApp construction

SVFileAcquisitionDeviceApp::SVFileAcquisitionDeviceApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

SVFileAcquisitionDeviceApp::~SVFileAcquisitionDeviceApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only SVFileAcquisitionDeviceApp object

SVFileAcquisitionDeviceApp g_svTheApp;

BOOL SVFileAcquisitionDeviceApp::InitInstance() 
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if ( S_OK == m_fileAcqDevice.Open() )
	{
		return CWinApp::InitInstance();
	}
	return false;
}

int SVFileAcquisitionDeviceApp::ExitInstance() 
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_fileAcqDevice.Close();

	return CWinApp::ExitInstance();
}

