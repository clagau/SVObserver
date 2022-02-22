//******************************************************************************
//* COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSecurityGeneralPage.cpp
//* .File Name       : $Workfile:   SVSecurityGeneralPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:02:58  $
//******************************************************************************
#pragma once

#pragma region Includes 
#include "resource.h"
#pragma endregion Includes

#define INFO_BUFFER_SIZE 32767

class SVAccessClass;

class SVSecurityGeneralPage : public CPropertyPage
{
	DECLARE_DYNCREATE(SVSecurityGeneralPage)

public:
	SVSecurityGeneralPage();
	explicit SVSecurityGeneralPage(UINT nIDTemplate, UINT nIDCaption = 0);
	virtual ~SVSecurityGeneralPage();

	//{{AFX_DATA(SVSecurityGeneralPage)
	enum { IDD = IDD_GENERAL_SECURITY_PAGE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVSecurityGeneralPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	SVAccessClass* m_pAccess;

	// Generated message map functions
	//{{AFX_MSG(SVSecurityGeneralPage)
	afx_msg void OnUserMgrBtn();
	afx_msg void OnEventViewerBtn();
	afx_msg void OnLoginBtn();
	virtual BOOL OnInitDialog() override;
	afx_msg void OnNoSecurityBtn();
	afx_msg void OnTraditionalSecurityBtn();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
