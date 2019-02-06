//******************************************************************************
//* COPYRIGHT (c) 2006 by SVResearch, Harrisburg
//* All Rights Reserved
//*****************************************************************************
//* .Module Name     : SVTADlgColorThresholdBasePage
//* .File Name       : $Workfile:   SVTADlgColorThresholdBasePage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:19:56  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVDlgImageEditor.h"
#include "SVROIFigureEditor.h"
#pragma endregion Includes

namespace SvOp
{
class SVColorThresholdClass;
}
namespace SvTo
{
class SVColorToolClass;
}
namespace SvVol
{
class SVDoubleValueObjectClass;
}
class SVTADlgColorThresholdSheet;

class SVTADlgColorThresholdBasePage : public CPropertyPage
{
	DECLARE_DYNCREATE(SVTADlgColorThresholdBasePage)

// Construction
public:
	SVTADlgColorThresholdBasePage() {}	// dummy for DYNCREATE macros
	SVTADlgColorThresholdBasePage(UINT nIDTemplate);
	virtual ~SVTADlgColorThresholdBasePage();

// Dialog Data
	//{{AFX_DATA(SVTADlgColorThresholdBasePage)
	enum { IDD = IDD_TA_COLOR_THRESHOLD_ROI };
	CComboBox	m_cbZoom;
	//}}AFX_DATA
	SVDlgImageEditor  m_svDlgImage;


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVTADlgColorThresholdBasePage)
	public:
	virtual BOOL OnKillActive() override;
	virtual BOOL OnSetActive() override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	SVTADlgColorThresholdSheet* m_pSheet;

protected:
	SvTo::SVColorToolClass* m_pTool;
	SvOp::SVColorThresholdClass* m_pThreshold;
	SvVol::SVDoubleValueObjectClass* m_pExtentLeft;
	SvVol::SVDoubleValueObjectClass* m_pExtentTop;
	SvVol::SVDoubleValueObjectClass* m_pExtentWidth;
	SvVol::SVDoubleValueObjectClass* m_pExtentHeight;

	// Generated message map functions
	//{{AFX_MSG(SVTADlgColorThresholdBasePage)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSelchangeComboZoom();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

