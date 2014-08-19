//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : MonitorListPropertyDlg
//* .File Name       : $Workfile:   MonitorListPropertyDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   18 Aug 2014 16:10:58  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "afxwin.h"
#include "PropertyTree/PropTree.h"
#include "RemoteMonitorList.h"
#pragma endregion Includes

#pragma region ENUMS
enum MonitorListPropertyEnum
{
	PROP_MONITOR_LIST_NAME		= 101,
	PROP_MONITOR_LIST_DEPTH		= 102,
};

#pragma endregion ENUMS

// MonitorListPropertyDlg dialog

class MonitorListPropertyDlg : public CDialog
{
	DECLARE_DYNAMIC(MonitorListPropertyDlg)

#pragma region Public
public:
	MonitorListPropertyDlg(RemoteMonitorList& MonitorList, CString sName, CWnd* pParent = NULL);   // standard constructor
	virtual ~MonitorListPropertyDlg();

	CString GetMonitorListName() const;
	int GetMonitorListRejectQueueDepth() const;

// Dialog Data
	enum { IDD = IDD_DLG_PROP_MON_LIST };

#pragma endregion Public

#pragma region Protected
protected:

// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(MonitorListPropertyDlg)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

	afx_msg void OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
	void OnOK();

	DECLARE_MESSAGE_MAP()

	SVRPropTree	m_Tree;
#pragma endregion Protected

#pragma region Private
	RemoteMonitorList& m_MonitorList;
	CString m_MonitorListName;	// original Monitor List name
	CString m_DisplayName;		// current valid Monitor List name (starts out as original Monitor List name)
	int m_MonitorListRejectQueueDepth;
	CString m_sPPQ;
	void SetupMonitorListProperties();
	bool IsValidListName(const CString& name) const;
	void ValidateLabelText(CString& newText) const;

#pragma endregion Private
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\MonitorListPropertyDlg.h_v  $
 * 
 *    Rev 1.1   18 Aug 2014 16:10:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added IsValidName method.
 * Added ValidateLabelText method.
 * Removed m_PreviousName member variable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Apr 2014 16:23:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/