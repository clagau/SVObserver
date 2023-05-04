//******************************************************************************
//* COPYRIGHT (c) 2006 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//*****************************************************************************
//* .Module Name     : TADlgColorThresholdBasePage
//* .File Name       : $Workfile:   TADlgColorThresholdBasePage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:19:56  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVDlgImageEditor.h"
#pragma endregion Includes

namespace SvOp
{
class SVColorThreshold;
}
namespace SvTo
{
class SVColorToolClass;
}
namespace SvVol
{
class SVDoubleValueObjectClass;
}
class TADlgColorThresholdSheet;

class TADlgColorThresholdBasePage : public CPropertyPage
{
	DECLARE_DYNCREATE(TADlgColorThresholdBasePage)

// Construction
public:
	TADlgColorThresholdBasePage() {}	// dummy for DYNCREATE macros
	explicit TADlgColorThresholdBasePage(UINT nIDTemplate);
	virtual ~TADlgColorThresholdBasePage();

// Dialog Data
	//{{AFX_DATA(TADlgColorThresholdBasePage)
	enum { IDD = IDD_TA_COLOR_THRESHOLD_ROI };
	CComboBox	m_cbZoom;
	//}}AFX_DATA
	SVDlgImageEditor  m_svDlgImage;


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(TADlgColorThresholdBasePage)
	public:
	virtual BOOL OnKillActive() override;
	virtual BOOL OnSetActive() override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	TADlgColorThresholdSheet* m_pSheet;

protected:
	SvTo::SVColorToolClass* m_pTool;
	SvOp::SVColorThreshold* m_pThreshold {nullptr};
	SvVol::SVDoubleValueObjectClass* m_pExtentLeft;
	SvVol::SVDoubleValueObjectClass* m_pExtentTop;
	SvVol::SVDoubleValueObjectClass* m_pExtentWidth;
	SvVol::SVDoubleValueObjectClass* m_pExtentHeight;

	// Generated message map functions
	//{{AFX_MSG(TADlgColorThresholdBasePage)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSelchangeComboZoom();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

