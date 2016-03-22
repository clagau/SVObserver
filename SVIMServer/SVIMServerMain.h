// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIMServerMain
// * .File Name       : $Workfile:   SVIMServerMain.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:07:46  $
// ******************************************************************************

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSVIMServerApp
// See SVIMServer.cpp for the implementation of this class
//

//##ModelId=38E8FF8901E4
class CSVIMServerApp : public CWinApp
{
public:
	//##ModelId=38E8FF890233
	CSVIMServerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVIMServerApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSVIMServerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


