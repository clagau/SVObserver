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
#include <utility>
#include <deque>
#include "Hlb.h"
#include "ISVPropertyPageDialog.h"
#include "NameSelectionTreeCtrl.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

class MonitorListSheet;

class MonitorListBasePage : public CPropertyPage, public ISVPropertyPageDialog
{
	DECLARE_DYNAMIC(MonitorListBasePage)

#pragma region Public
public:
	MonitorListBasePage(const NameSelectionList& rList, MonitorListSheet* pParentSheet, const CString& szCaption, int id);
	virtual ~MonitorListBasePage();

	// ISVPropertyPageDialog
	virtual bool QueryAllowExit();

	const NameSelectionList& GetNameSelectionList() const;

	// output of OnOK
	bool m_bObjectsChanged;

// Dialog Data
	//{{AFX_DATA(MonitorListBasePage)
	enum { IDD = IDD_MONITOR_LIST_PAGE };
	NameSelectionTreeCtrl m_Tree;
	CButton     m_btnRemoveAll;
	CButton     m_btnRemove;
	SVHorizListBox   m_lbSelected;
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
	NameSelectionList m_nameList;
#pragma endregion Protected
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\MonitorListBasePage.h_v  $
 * 
 *    Rev 1.1   15 Aug 2014 15:22:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised to use SVHorizListBox.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Apr 2014 16:22:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
