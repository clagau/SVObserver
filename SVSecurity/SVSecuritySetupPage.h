//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
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
	
	explicit SVSecuritySetupPage(UINT nIDTemplate, UINT nIDCaption = 0);
	
	virtual ~SVSecuritySetupPage() = default;
	void BuildTree( long& p_NodeIndex, HTREEITEM hParent, bool p_Root );

	enum { IDD = IDD_SECURITY_PAGE };
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
	CTreeCtrl	m_AccessPointTree;
	CListBox	m_lbNetGroups;
	BOOL	m_bForcePrompt {false};
	BOOL	m_bLogOnMode {false};
	long	m_lTimeout {0L};
	CString	m_Group;

	int		m_nParent {0};
	int		m_nChild {0};

	SVSecuritySetupSheet*   pPropSheet {nullptr};
	SVAccessClass*          m_pAccess {nullptr};
};


