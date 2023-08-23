//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgColorThresholdAdjustment
//* .File Name       : $Workfile:   TADlgColorThresholdAdjustment.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   26 Jun 2014 18:17:56  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "Definitions/BandEnums.h"
#include "TADlgColorThresholdBasePage.h"
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#include "SVLibrary/SVValueBaseNormalizerClass.h"
#include "SVOGuiUtility/DataController.h"
#include "SVDrawObject.h"
#pragma endregion Includes

namespace SvOp
{
class SVColorThreshold;
}
namespace SvVol
{
class SVLongValueObjectClass;
class SVBoolValueObjectClass;
}

class TADlgColorThresholdAdjustment : public TADlgColorThresholdBasePage
{
	DECLARE_DYNCREATE(TADlgColorThresholdAdjustment)

// Construction
public:
	TADlgColorThresholdAdjustment();
	virtual ~TADlgColorThresholdAdjustment();

	void SetInspectionData();
	void setBand(SvDef::BandEnum band){ m_band = band; }

// Dialog Data
	//{{AFX_DATA(TADlgColorThresholdAdjustment)
	enum { IDD = IDD_TA_COLOR_THRESHOLD_ADJUST_DIALOG };
	CSliderCtrl	UpperSliderCtrl;
	CSliderCtrl	LowerSliderCtrl;
	CString	StrUpper;
	CString	StrLower;
	BYTE	m_lowerThreshold;
	BYTE	m_upperThreshold;
	BOOL	m_exclude;
	BOOL	m_Enabled;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(TADlgColorThresholdAdjustment)
	public:
	virtual BOOL OnSetActive() override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(TADlgColorThresholdAdjustment)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnExcludeColorThreshold();
	afx_msg void OnEnabledThreshold();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void setThresholdParams();
	void updateControls();
	void setScrollRange( CSliderCtrl* pSliderCtrl, int min, int max );
	void setScrollPos( CSliderCtrl* pSliderCtrl, int pos );
	void updateGraphDisplay();
	void setInitialValues();
	void RefreshProperties();
	void updateHistogram();
	void updateThresholdBars();

	bool initHistogram();

public:

protected:
	SVValueBaseNormalizerClass m_Normalizer;

private:
	SvOgu::ValueController m_Values;

	SvDef::BandEnum m_band{SvDef::Band0};
	SvOi::SVImageBufferHandlePtr m_histogramImage;
	SVDrawObject m_graphFigure;
	SVDrawObjectList m_thresholdBarFigures;
	__int64 m_histogramResultID{0LL};
};

