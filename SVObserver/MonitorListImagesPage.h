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
	MonitorListImagesPage(const SvUl::NameSelectionList& rList, MonitorListSheet* pParentSheet, const CString& szCaption, int id = IDD);
	virtual ~MonitorListImagesPage();

	// ISVPropertyPageDialog
	virtual bool QueryAllowExit();
	virtual void OnOK();

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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(MonitorListImagesPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void OnSelchangeListSelected();

	virtual void InitPage();
#pragma endregion Protected
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\MonitorListImagesPage.h_v  $
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