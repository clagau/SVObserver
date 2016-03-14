//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CCPassDlg
//* .File Name       : $Workfile:   CPassDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 08:33:10  $
//******************************************************************************

#if !defined(AFX_CPASSDLG_H__13DC2ECF_F70B_11D6_9C9C_00AACA00180E__INCLUDED_)
#define AFX_CPASSDLG_H__13DC2ECF_F70B_11D6_9C9C_00AACA00180E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CPassDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCPassDlg dialog

class CCPassDlg : public CDialog
{
// Construction
public:
	CCPassDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCPassDlg)
	enum { IDD = IDD_PASSWORD };
	CString	m_password;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCPassDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCPassDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CPASSDLG_H__13DC2ECF_F70B_11D6_9C9C_00AACA00180E__INCLUDED_)

