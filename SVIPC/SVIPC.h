// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIPC
// * .File Name       : $Workfile:   SVIPC.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:09:54  $
// ******************************************************************************

#if !defined(AFX_SVIPC_H__00F9EF49_EAEC_11D3_A886_00106F000C7C__INCLUDED_)
#define AFX_SVIPC_H__00F9EF49_EAEC_11D3_A886_00106F000C7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSVIPCApp
// See SVIPC.cpp for the implementation of this class
//

//##ModelId=385E57E6033C
class CSVIPCApp : public CWinApp
{
public:
	//##ModelId=385E5E3501A5
	CSVIPCApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVIPCApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSVIPCApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVIPC_H__00F9EF49_EAEC_11D3_A886_00106F000C7C__INCLUDED_)

