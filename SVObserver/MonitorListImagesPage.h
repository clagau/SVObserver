//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidendader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : MonitorListImagesPage
//* .File Name       : $Workfile:   MonitorListImagesPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Apr 2014 16:23:34  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "MonitorListBasePage.h"
#pragma endregion Includes

/////////////////////////////////////////////////////////////////////////////
// MonitorListImagesPage dialog

class MonitorListSheet;

class MonitorListImagesPage : public MonitorListBasePage
{
	DECLARE_DYNAMIC(MonitorListImagesPage)

#pragma region Public
public:
	MonitorListImagesPage(const SvUl::NameSelectionList& rList, MonitorListSheet* pParentSheet, LPCTSTR Caption, int id = IDD);
	virtual ~MonitorListImagesPage();

	// ISVPropertyPageDialog
	virtual bool QueryAllowExit() override;
	virtual void OnOK() override;

// Dialog Data
	//{{AFX_DATA(MonitorListImagesPage)
	enum { IDD = IDD_MONITOR_LIST_PAGE };
	//}}AFX_DATA
#pragma endregion Public

#pragma region Protected
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(MonitorListImagesPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(MonitorListImagesPage)
	virtual BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void OnSelchangeListSelected() override;

	virtual void InitPage() override;
#pragma endregion Protected
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

