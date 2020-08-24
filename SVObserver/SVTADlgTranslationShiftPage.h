//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgTranslationShiftPage
//* .File Name       : $Workfile:   SVTADlgTranslationShiftPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:24:52  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVMFCControls/SVEnumerateCombo.h"
#include "SVOGui/DataController.h"
#include "SVRPropertyTree/SVRPropTree.h"
#pragma endregion Includes

namespace SvOp
{
class SVEvaluate;
}
namespace SvTo
{
class SVShiftTool;
}
namespace SvVol
{
class SVEnumerateValueObjectClass;
}

class SVToolAdjustmentDialogSheetClass;

enum SVShiftResultEnum
{
	PROP_SHIFT_TRANS_X		= 100,
	PROP_SHIFT_TRANS_Y		= 101,
	PROP_SHIFT_DISP_X		= 102,
	PROP_SHIFT_DISP_Y		= 103,
	PROP_SHIFT_RESULT_TOP	= 104,
	PROP_SHIFT_RESULT_LEFT	= 105,
};

class SVTADlgTranslationShiftPageClass : public CPropertyPage
{
//	DECLARE_DYNAMIC(SVTADlgTranslationShiftPage)

public:
	SVTADlgTranslationShiftPageClass( uint32_t inspectionId, uint32_t taskObjectId, SVToolAdjustmentDialogSheetClass* Parent );
	virtual ~SVTADlgTranslationShiftPageClass();

	HRESULT SetInspectionData();

// Dialog Data
	enum { IDD = IDD_TA_TRANSLATION_SHIFT_DIALOG };

	virtual BOOL OnSetActive() override; 
protected:
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	SVRPropTree	m_Tree;


	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnLearn();
	afx_msg void OnBnClickedBtnNormalize();

protected:
	void refresh();
	void UpdateControls();
	void SetupShiftPropertyTree();
	void FillShiftProperties();

	SVToolAdjustmentDialogSheetClass* m_pParentDialog;
	SvTo::SVShiftTool* m_pTool;

	SvOp::SVEvaluate* pEvaluateTranslationX;
	SvOp::SVEvaluate* pEvaluateTranslationY;


public:
	BOOL m_performTranslation;
	afx_msg void OnBnClickedPerformTranslation();
	afx_msg void OnBnClickedTranslationXFormulaButton();
	afx_msg void OnBnClickedTranslationYFormulaButton();
	CString m_TranslationXValue;
	CString m_TranslationYValue;
	SvMc::SVEnumerateCombo m_ctlShiftModeCombo;

public:
	SvVol::SVEnumerateValueObjectClass *m_pvoShiftMode;
	afx_msg void OnCbnSelchangeComboShiftMode();
	CButton m_btnFormulaX;
	CButton m_btnFormulaY;
	CButton m_btnSetReference;
	CButton m_btnNormalize;
	CListBox m_ctlShiftValueList;
	CStatic m_ShiftValueStatic;

private:
	SvOg::ValueController m_Values;

	long m_lShiftType;
};


