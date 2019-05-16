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

#pragma once

#pragma region Includes 
#include "resource.h"
#pragma endregion Includes

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
	CString	m_Group;
	//}}AFX_DATA
	
	int		m_nParent;
	
	int		m_nChild;
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVSecuritySetupPage)
	public:
	virtual BOOL OnApply() override;
	virtual void OnOK() override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVSecuritySetupPage)
	virtual BOOL OnInitDialog() override;
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


	void  SetGroups(CString groups);
	
	void  ResetGroups();


protected:	
	
	SVSecuritySetupSheet*   pPropSheet;
	SVAccessClass*          m_pAccess;
};


