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
#include "Definitions/BandEnums.h"
#include "SVDrawObject.h"
#include "SVTADlgColorThresholdBasePage.h"
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#include "SVLibrary/SVValueBaseNormalizerClass.h"
#include "SVOGui/ValuesAccessor.h"
#include "SVOGui/DataController.h"
#pragma endregion Includes

namespace SvOp
{
class SVColorThresholdClass;
}
namespace SvVol
{
class SVLongValueObjectClass;
class SVBoolValueObjectClass;
}

class SVTADlgColorThresholdAdjustment : public SVTADlgColorThresholdBasePage
{
	DECLARE_DYNCREATE(SVTADlgColorThresholdAdjustment)

// Construction
public:
	SVTADlgColorThresholdAdjustment();
	virtual ~SVTADlgColorThresholdAdjustment();

	void SetInspectionData();
	void setBand(SvDef::BandEnum band){ m_band = band; }

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
	BOOL	m_Enabled;
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
	SvOp::SVColorThresholdClass* m_pThreshold{nullptr};
	SVValueBaseNormalizerClass m_Normalizer;

private:
	typedef SvOg::DataController<SvOg::ValuesAccessor, SvOg::ValuesAccessor::value_type> Controller;
	Controller m_Values;

	SvDef::BandEnum m_band{SvDef::Band0};
	SvOi::SVImageBufferHandlePtr m_histogramImage;
	SVDrawObjectClass m_graphFigure;
	SVDrawObjectListClass m_thresholdBarFigures;
	__int64 m_histogramResultID{0LL};
};

