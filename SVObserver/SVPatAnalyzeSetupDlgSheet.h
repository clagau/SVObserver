//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPatAnalyzeSetupDlgSheet
//* .File Name       : $Workfile:   SVPatAnalyzeSetupDlgSheet.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:19:02  $
//******************************************************************************

#pragma once

class SVPatAnalyzeSetupDlgSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(SVPatAnalyzeSetupDlgSheet)

public:
	explicit SVPatAnalyzeSetupDlgSheet(UINT nIDCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
	explicit SVPatAnalyzeSetupDlgSheet(LPCTSTR pszCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
	virtual ~SVPatAnalyzeSetupDlgSheet();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVPatAnalyzeSetupDlgSheet)
	public:
	virtual BOOL OnInitDialog() override;
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(SVPatAnalyzeSetupDlgSheet)
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
