//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgColorThresholdAdjustment
//* .File Name       : $Workfile:   SVTADlgColorThresholdAdjustment.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   26 Jun 2014 18:17:56  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVTADlgColorThresholdBasePage.h"
#include "SVValueBaseNormalizerClass.h"
#include "SVTaskObjectValueInterface.h"
#pragma endregion Includes

class SVIPDoc;
class SVColorToolClass;
class SVColorThresholdClass;
class SVLongValueObjectClass;
class SVBoolValueObjectClass;

/////////////////////////////////////////////////////////////////////////////
// SVTADlgColorThresholdAdjustment dialog

class SVTADlgColorThresholdAdjustment : public SVTADlgColorThresholdBasePage, public SVTaskObjectValueInterface
{
	DECLARE_DYNCREATE(SVTADlgColorThresholdAdjustment)

// Construction
public:
	SVTADlgColorThresholdAdjustment();
	virtual ~SVTADlgColorThresholdAdjustment();

	HRESULT SetInspectionData();

// Dialog Data
	//{{AFX_DATA(SVTADlgColorThresholdAdjustment)
	enum { IDD = IDD_TA_COLOR_THRESHOLD_ADJUST_DIALOG };
	CComboBox	m_cbZoom;
	CSliderCtrl	UpperSliderCtrl;
	CSliderCtrl	LowerSliderCtrl;
	CString	StrUpper;
	CString	StrLower;
	BYTE	m_lowerThreshold;
	BYTE	m_upperThreshold;
	BOOL	m_exclude;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVTADlgColorThresholdAdjustment)
	public:
	virtual BOOL OnSetActive() override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVTADlgColorThresholdAdjustment)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnExcludeColorThreshold();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void getThresholdParams();
	void setThresholdParams();
	void updateControls();
	void setScrollRange( CSliderCtrl* pSliderCtrl, int min, int max );
	void setScrollPos( CSliderCtrl* pSliderCtrl, int pos );
	void updateGraphDisplay();
	void setInitialValues();
	void RefreshProperties();

public:
	int m_BandNumber;

protected:
	SVIPDoc*                m_pCurrentDocument;
	SVColorThresholdClass*  m_pThreshold;
	SVLongValueObjectClass* m_pUpperThreshold;
	SVLongValueObjectClass* m_pLowerThreshold;
	SVBoolValueObjectClass* m_pExclude;

	SVValueBaseNormalizerClass m_Normalizer;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
