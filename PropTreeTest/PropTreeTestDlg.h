//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : PropTreeTest
//* .File Name       : $Workfile:   PropTreeTestDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   03 Oct 2013 14:43:30  $
//******************************************************************************

#ifndef PROPTREETESTDLG_H
#define PROPTREETESTDLG_H

#include "PropertyTree/PROPTREE.H" // for SVRPropTree

// CPropTreeTestDlg dialog
class CPropTreeTestDlg : public CDialogEx
{
// Construction
public:
	CPropTreeTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PROPTREETEST_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void AddItemEdit( CString p_Name, CString p_Value, UINT p_ID );
	void AddItemCombo( CString p_Name, CString p_Value, UINT p_ID );
	void SetupTrigger();
	afx_msg void OnItemChanged( NMHDR* p_NotifyStruct, LRESULT* p_lResult );

// Implementation
	HICON m_hIcon;
	SVRPropTree m_Tree;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\PropTreeTest\PropTreeTestDlg.h_v  $
 * 
 *    Rev 1.0   03 Oct 2013 14:43:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
