// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : CSVIntekApp
// * .File Name       : $Workfile:   SVIntek.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:33:06  $
// ******************************************************************************

#if !defined(AFX_SVINTEK_H__B673BBD5_74B0_480C_ADEF_33E56FB9391E__INCLUDED_)
#define AFX_SVINTEK_H__B673BBD5_74B0_480C_ADEF_33E56FB9391E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "SVIntekFireStackClass.h"

/////////////////////////////////////////////////////////////////////////////
// CSVIntekApp
// See SVIntek.cpp for the implementation of this class
//

class CSVIntekApp : public CWinApp
{
public:
	CSVIntekApp();

	SVIntekFireStackClass m_svSystem;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVIntekApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSVIntekApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CSVIntekApp g_svTheApp;


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVINTEK_H__B673BBD5_74B0_480C_ADEF_33E56FB9391E__INCLUDED_)

