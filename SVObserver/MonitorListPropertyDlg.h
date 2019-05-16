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
#include "SVRPropertyTree/SVRPropTree.h"
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
	MonitorListPropertyDlg(RemoteMonitorListMap& MonitorList, LPCTSTR Name, CWnd* pParent = nullptr);   // standard constructor
	virtual ~MonitorListPropertyDlg();

	const std::string& GetMonitorListName() const;
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
	RemoteMonitorListMap& m_MonitorList;
	std::string m_MonitorListName;	// original Monitor List name
	std::string m_DisplayName;		// current valid Monitor List name (starts out as original Monitor List name)
	int m_MonitorListRejectQueueDepth;
	std::string m_sPPQ;
	bool m_IsMonitorListActive;
	void SetupMonitorListProperties();
	bool IsValidListName(const std::string& rName, const std::string& rOriginalName) const;
	std::string ValidateLabelText(const std::string& rNewText) const;

#pragma endregion Private
};

