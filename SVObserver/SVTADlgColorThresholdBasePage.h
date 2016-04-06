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

class SVTADlgColorThresholdSheet;
class SVColorToolClass;
class SVColorThresholdClass;
class SVDoubleValueObjectClass;

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
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	SVTADlgColorThresholdSheet* m_pSheet;
protected:

	SVColorToolClass*         mpTool;
	SVColorThresholdClass*    mpThreshold;
	SVDoubleValueObjectClass* mpExtentLeft;
	SVDoubleValueObjectClass* mpExtentTop;
	SVDoubleValueObjectClass* mpExtentWidth;
	SVDoubleValueObjectClass* mpExtentHeight;

	// Generated message map functions
	//{{AFX_MSG(SVTADlgColorThresholdBasePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboZoom();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

