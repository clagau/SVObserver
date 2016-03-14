//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSecuritySetupPage
//* .File Name       : $Workfile:   SVSecuritySetupPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:03:54  $
//******************************************************************************

#ifndef SECURITYSETUPPAGE_H
#define SECURITYSETUPPAGE_H

class SVAccessClass;
class SVSecuritySetupSheet;

class SVSecuritySetupPage : public CPropertyPage
{
	DECLARE_DYNCREATE(SVSecuritySetupPage)

// Construction
public:
	
	SVSecuritySetupPage();
	
	SVSecuritySetupPage(UINT nIDTemplate, UINT nIDCaption=0);
	
	~SVSecuritySetupPage();
	void BuildTree( long& p_NodeIndex, HTREEITEM hParent, bool p_Root );

// Dialog Data
	//{{AFX_DATA(SVSecuritySetupPage)
	enum { IDD = IDD_SECURITY_PAGE };
	CTreeCtrl	m_AccessPointTree;
	CListBox	m_lbNetGroups;
	BOOL	m_bForcePrompt;
	BOOL	m_bLogOnMode;
	long	m_lTimeout;
	CString	m_strGroup;
	//}}AFX_DATA
	
	CString	m_strApp;
	
	int		m_nParent;
	
	int		m_nChild;
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVSecuritySetupPage)
	public:
	virtual BOOL OnApply();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVSecuritySetupPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeNetGroups();
	afx_msg void OnSelchangedAccessTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusGroupEdit();
	afx_msg void OnForcePrompt();
	afx_msg void OnClearGrp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void SetData();

	int GetNodeIndexFromTree();

	CString GetGroups(void);
	
	void  SetGroups(CString groups);
	
	void  ResetGroups(void);


protected:	
	
	SVSecuritySetupSheet*   pPropSheet;
	SVAccessClass*          m_pAccess;
	
//	SVDlgTemplate		DlgTemplate;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SECURITYSETUPPAGE_H__FB876D1A_4765_11D4_A8FD_00106F0309AB__INCLUDED_)

