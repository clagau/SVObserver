//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidendader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : MonitorListBasePage
//* .File Name       : $Workfile:   MonitorListBasePage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   15 Aug 2014 15:22:46  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <utility>
//Moved to precompiled header: #include <deque>
#include "SVMFCControls\SVHorizListBox.h"
#include "SVOGui\ISVPropertyPageDialog.h"
#include "SVMFCControls\NameSelectionTreeCtrl.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

class MonitorListSheet;

class MonitorListBasePage : public CPropertyPage, public SvOg::ISVPropertyPageDialog
{
	DECLARE_DYNAMIC(MonitorListBasePage)

#pragma region Public
public:
	MonitorListBasePage(const SvUl::NameSelectionList& rList, MonitorListSheet* pParentSheet, const CString& szCaption, int id);
	virtual ~MonitorListBasePage();

	// ISVPropertyPageDialog
	virtual bool QueryAllowExit();

	const SvUl::NameSelectionList& GetNameSelectionList() const;

	// output of OnOK
	bool m_bObjectsChanged;

// Dialog Data
	//{{AFX_DATA(MonitorListBasePage)
	enum { IDD = IDD_MONITOR_LIST_PAGE };
	SvMc::NameSelectionTreeCtrl m_Tree;
	CButton     m_btnRemoveAll;
	CButton     m_btnRemove;
	SvMc::SVHorizListBox   m_lbSelected;
	//}}AFX_DATA

#pragma endregion Public
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(MonitorListBasePage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

#pragma region Protected
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(MonitorListBasePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListSelected_Base();
	afx_msg void OnBtnRemove();
	afx_msg void OnBtnRemoveAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void OnSelchangeListSelected() {};
	virtual void InitPage() = 0;
	bool CanSelectObjectCallback( bool bCurrentState, int p_iIndex );
	void ShowSelectionText();

	CString m_selectedItemText;
	MonitorListSheet* m_pSheet;
	//SVString m_ppqName;
	SvUl::NameSelectionList m_nameList;
#pragma endregion Protected
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

