//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgColorThresholdOutput
//* .File Name       : $Workfile:   SVTADlgColorThresholdOutput.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   26 Jun 2014 18:17:58  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVTADlgColorThresholdBasePage.h"
#pragma endregion Includes

class SVBoolValueObjectClass;

class SVTADlgColorThresholdOutput : public SVTADlgColorThresholdBasePage
{
	DECLARE_DYNCREATE(SVTADlgColorThresholdOutput)

// Construction
public:
	SVTADlgColorThresholdOutput();
	virtual ~SVTADlgColorThresholdOutput();

	//{{AFX_DATA(SVTADlgColorThresholdOutput)
	enum { IDD = IDD_TA_COLOR_THRESHOLD_OUTPUT_DIALOG };
	BOOL	m_Enabled;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(SVTADlgColorThresholdOutput)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVTADlgColorThresholdOutput)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnEnabledThreshold();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	SVBoolValueObjectClass* m_pEnabled;
};
