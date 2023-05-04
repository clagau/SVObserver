//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgColorThresholdOutput
//* .File Name       : $Workfile:   TADlgColorThresholdOutput.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   26 Jun 2014 18:17:58  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "TADlgColorThresholdBasePage.h"
#pragma endregion Includes

namespace SvVol
{
class SVBoolValueObjectClass;
}

class TADlgColorThresholdOutput : public TADlgColorThresholdBasePage
{
	DECLARE_DYNCREATE(TADlgColorThresholdOutput)

// Construction
public:
	TADlgColorThresholdOutput();
	virtual ~TADlgColorThresholdOutput();

	//{{AFX_DATA(TADlgColorThresholdOutput)
	enum { IDD = IDD_TA_COLOR_THRESHOLD_OUTPUT_DIALOG };
	BOOL	m_Enabled;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(TADlgColorThresholdOutput)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(TADlgColorThresholdOutput)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnEnabledThreshold();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	SvVol::SVBoolValueObjectClass* m_pEnabled;
};
