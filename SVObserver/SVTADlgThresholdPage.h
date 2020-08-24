//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogThresholdPage
//* .File Name       : $Workfile:   SVTADlgThresholdPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   26 Jun 2014 18:21:16  $
//******************************************************************************

#pragma once
#pragma region Includes
#include "SVMFCControls\EditNumbers.h"
#include "SVMFCControls\SVHistogram.h"
//TODO: MZA(10.Nov 2014): Move this files to SVOGui project and then remove folder from include and Namespace add-on add PictureDisplay declaration.
#include "SVOGui/PictureDisplay.h"
#include "SVOGui/ImageController.h"
#pragma endregion Includes

namespace SvOi
{
	class IObjectClass;
}
namespace SvOp
{
class SVThresholdClass;
}
namespace SvTo
{
class SVToolClass;
}
namespace SvVol
{
class SVBoolValueObjectClass;
}
class SVToolAdjustmentDialogSheetClass;

// Threshold Dialog
class SVToolAdjustmentDialogThresholdPageClass : public CPropertyPage
{
	//******************************************************************************
	// Class Wizard Generated Message Map Entries:
	//******************************************************************************
	// Generated message map functions
	//{{AFX_MSG(SVToolAdjustmentDialogThresholdPageClass)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnChangeUpperEdit();
	afx_msg void OnChangeLowerEdit();
	afx_msg void OnUpperActivateCheck();
	afx_msg void OnLowerActivateCheck();
	afx_msg void OnCheck1();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnAutoThresholdCheck();
	afx_msg void OnWhiteBackgroundRadio();
	afx_msg void OnBlackBackgroundRadio();
	afx_msg void OnChangeAutothresholdEdit();
	afx_msg void OnUTFormulaButton();
	afx_msg void OnATMFormulaButton();
	afx_msg void OnLTFormulaButton();
	afx_msg void OnUseExternATMCheck();
	afx_msg void OnUseExternLTCheck();
	afx_msg void OnUseExternUTCheck();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg LRESULT OnBoundChange(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	void UpdateLowerThresholdFromFormula();
	void UpdateUpperThresholdFromFoumula();

public:
	SVToolAdjustmentDialogThresholdPageClass(uint32_t inspectionId, uint32_t taskObjectId, SVToolAdjustmentDialogSheetClass* pParent);
	virtual ~SVToolAdjustmentDialogThresholdPageClass();

	HRESULT SetInspectionData();

	//************************************
	// Method:    setImages
	// Description: Set image to the activeX-control. 
	// Access:    public 
	// Returns:   void
	//************************************
	void setImages();

	void initThreshold();

	//******************************************************************************
	// Class Wizard Generated Virtual Function(s):
	//******************************************************************************
	//{{AFX_VIRTUAL(SVToolAdjustmentDialogThresholdPageClass)
public:
	virtual BOOL OnSetActive() override;

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV
	//}}AFX_VIRTUAL

	//******************************************************************************
	// Class Wizard Generated Dialog Data:
	//******************************************************************************
protected:
	//{{AFX_DATA(SVToolAdjustmentDialogThresholdPageClass)
	enum { IDD = IDD_TA_THRESHOLD_DIALOG };
	SvMc::EditNumbers	m_editLowerThres;
	SvMc::EditNumbers	m_editUpperThres;
	SvMc::CEditNumbersFloat	m_editAutoThreshold;
	CButton	m_autoThresholdWhiteRadio;
	CButton	m_autoThresholdBlackRadio;
	CSliderCtrl	m_autoThresholdCtrl;
	SvOg::PictureDisplay m_dialogImage;
	CSliderCtrl	m_lowerThreshold;
	CSliderCtrl	m_upperThreshold;
	BOOL	m_upperThresholdActive;
	BOOL	m_thresholdActive;
	BOOL	m_lowerThresholdActive;
	CString	m_strWhitePixel;
	BOOL	m_bUseAutoThreshold;
	CString	m_csEditAutoThreshold;
	CString	m_csUpperThres;
	CString	m_csLowerThres;
	BOOL	m_bUseExternATM;
	BOOL	m_bUseExternLT;
	BOOL	m_bUseExternUT;
	//}}AFX_DATA

public:
	SVToolAdjustmentDialogSheetClass* m_pParentDialog;
	BOOL m_isEdit;

protected:
	SvOi::IObjectClass* m_pTaskObject;
	SvOp::SVThresholdClass* m_pCurrentThreshold;

	SvVol::SVBoolValueObjectClass* m_pUseExternATM;
	SvVol::SVBoolValueObjectClass* m_pUseExternLT;
	SvVol::SVBoolValueObjectClass* m_pUseExternUT;

private:
	long m_lo;		// storage of slider pos, if deactivated...
	long m_up;		// storage of slider pos, if deactivated...
	double m_dAutoThreshold;
	long m_upperThres;	// current slider pos...
	long m_lowerThres;	// current slider pos...
	long m_histState;
	SvOg::ImageController m_ImageController;
	uint32_t m_resultImageID = SvDef::InvalidObjectId;

public:
	SvMc::SVHistogram m_histogram;
};

////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// DevStudio inserts additional declarations immediate in front of the preceding line
////////////////////////////////////////////////////////////////////////////////


