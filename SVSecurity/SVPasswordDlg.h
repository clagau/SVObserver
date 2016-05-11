//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPasswordDlg.h
//* .File Name       : $Workfile:   SVPasswordDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:01:06  $
//******************************************************************************

#pragma once

class SVPasswordDlg : public CDialog
{
public:
	SVPasswordDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVPasswordDlg();

	//{{AFX_DATA(SVPasswordDlg)
	enum { IDD = IDD_PASSWORD_DLG };
	CString	m_strPassword;
	CString	m_strUser;
	CString	m_strStatus;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVPasswordDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	// Generated message map functions
	//{{AFX_MSG(SVPasswordDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

