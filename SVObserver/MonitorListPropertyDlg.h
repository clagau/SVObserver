//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : MonitorListPropertyDlg
//* .File Name       : $Workfile:   MonitorListPropertyDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   26 Aug 2014 17:19:22  $
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
	PROP_MONITOR_LIST_ISACTIVE   = 103,
};

#pragma endregion ENUMS

// MonitorListPropertyDlg dialog

class MonitorListPropertyDlg : public CDialog
{
	DECLARE_DYNAMIC(MonitorListPropertyDlg)

#pragma region Public
public:
	MonitorListPropertyDlg(RemoteMonitorList& MonitorList, CString sName, CWnd* pParent = nullptr);   // standard constructor
	virtual ~MonitorListPropertyDlg();

	CString GetMonitorListName() const;
	int GetMonitorListRejectQueueDepth() const;
	bool GetIsMonitorListActive() const;
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
	bool m_IsMonitorListActive;
	void SetupMonitorListProperties();
	bool IsValidListName(const CString& name, const CString& originalName) const;
	void ValidateLabelText(CString& newText) const;

#pragma endregion Private
};

