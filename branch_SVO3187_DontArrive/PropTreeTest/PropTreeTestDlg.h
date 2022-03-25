//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : PropTreeTest
//* .File Name       : $Workfile:   PropTreeTestDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   03 Oct 2013 14:43:30  $
//******************************************************************************

#pragma once

#include "SVRPropertyTree/SVRPropTree.h"
#include "resource.h"


// PropTreeTestDlg dialog
class PropTreeTestDlg : public CDialog
{
// Construction
public:
	PropTreeTestDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PROPTREETEST_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;	// DDX/DDV support
	void AddItemEdit( CString p_Name, CString p_Value, UINT p_ID );
	void AddItemCombo( CString p_Name, CString p_Value, UINT p_ID );
	void SetupTrigger();
	afx_msg void OnItemChanged( NMHDR* p_NotifyStruct, LRESULT* p_lResult );

// Implementation
	HICON m_hIcon;
	SVRPropTree m_Tree;

	// Generated message map functions
	virtual BOOL OnInitDialog() override;
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};

