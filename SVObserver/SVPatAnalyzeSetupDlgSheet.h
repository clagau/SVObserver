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

#if !defined(AFX_SVPATAANALYZESETUPDLGSHEETCLASS_H__38DF47B5_F364_11D3_A896_00106F021742__INCLUDED_)
#define AFX_SVPATAANALYZESETUPDLGSHEETCLASS_H__38DF47B5_F364_11D3_A896_00106F021742__INCLUDED_
class SVPatternAnalyzerClass;

class SVPatAnalyzeSetupDlgSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(SVPatAnalyzeSetupDlgSheet)

public:
	SVPatAnalyzeSetupDlgSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	SVPatAnalyzeSetupDlgSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~SVPatAnalyzeSetupDlgSheet();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVPatAnalyzeSetupDlgSheet)
	public:
	virtual BOOL OnInitDialog();
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

#endif // !defined(AFX_SVPATAANALYZESETUPDLGSHEETCLASS_H__38DF47B5_F364_11D3_A896_00106F021742__INCLUDED_)

