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
#include "SVRPropertyTree/SVRPropTree.h"
#include "SVUtilityLibrary/SVString.h"
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
	MonitorListPropertyDlg(RemoteMonitorList& MonitorList, LPCTSTR Name, CWnd* pParent = nullptr);   // standard constructor
	virtual ~MonitorListPropertyDlg();

	const SVString& GetMonitorListName() const;
	int GetMonitorListRejectQueueDepth() const;
	bool GetIsMonitorListActive() const;
// Dialog Data
	enum { IDD = IDD_DLG_PROP_MON_LIST };

#pragma endregion Public

#pragma region Protected
protected:

// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(MonitorListPropertyDlg)
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual BOOL OnInitDialog() override;
	//}}AFX_VIRTUAL

	afx_msg void OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
	void OnOK();

	DECLARE_MESSAGE_MAP()

	SVRPropTree	m_Tree;
#pragma endregion Protected

#pragma region Private
	RemoteMonitorList& m_MonitorList;
	SVString m_MonitorListName;	// original Monitor List name
	SVString m_DisplayName;		// current valid Monitor List name (starts out as original Monitor List name)
	int m_MonitorListRejectQueueDepth;
	SVString m_sPPQ;
	bool m_IsMonitorListActive;
	void SetupMonitorListProperties();
	bool IsValidListName(const SVString& rName, const SVString& rOriginalName) const;
	SVString ValidateLabelText(const SVString& rNewText) const;

#pragma endregion Private
};

