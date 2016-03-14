//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidendader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : MonitorListValuesPage
//* .File Name       : $Workfile:   MonitorListValuesPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 Jul 2014 09:13:58  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "MonitorListBasePage.h"
#pragma endregion Includes

class MonitorListSheet;

class MonitorListValuesPage : public MonitorListBasePage
{
	DECLARE_DYNAMIC(MonitorListValuesPage)

#pragma region Public
public:
	MonitorListValuesPage(const SvUl::NameSelectionList& rList, bool bWholeArray, MonitorListSheet* pParentSheet, const CString& szCaption, int id = IDD);
	virtual ~MonitorListValuesPage();

	// ISVPropertyPageDialog
	virtual bool QueryAllowExit();
	virtual void OnOK();

// Dialog Data
	//{{AFX_DATA(MonitorListValuesPage)
	enum { IDD = IDD_MONITOR_LIST_PAGE };
	//}}AFX_DATA
#pragma endregion Public

#pragma region Protected
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(MonitorListValuesPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(MonitorListValuesPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void OnSelchangeListSelected();

	virtual void InitPage();
	bool m_bAllowWholeArray;
#pragma endregion Protected
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

