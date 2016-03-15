// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTCPIP
// * .File Name       : $Workfile:   SVTCPIP.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 18:11:10  $
// ******************************************************************************

#if !defined(AFX_SVTCPIP_H__A3F9E7F6_B6F2_11D3_A843_00106F000C7C__INCLUDED_)
#define AFX_SVTCPIP_H__A3F9E7F6_B6F2_11D3_A843_00106F000C7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


/////////////////////////////////////////////////////////////////////////////
// CSVTCPIPApp
// See SVTCPIP.cpp for the implementation of this class
//

//##ModelId=38BD69AD038A
class CSVTCPIPApp : public CWinApp
{
public:
	//##ModelId=38BD69AD039B
	CSVTCPIPApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVTCPIPApp)
	public:
	//##ModelId=38BD69AD039A
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSVTCPIPApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVTCPIP_H__A3F9E7F6_B6F2_11D3_A843_00106F000C7C__INCLUDED_)

