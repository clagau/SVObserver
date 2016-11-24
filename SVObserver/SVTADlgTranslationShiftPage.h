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
#include "SVEnumerateCombo.h"
#include "SVTaskObjectValueInterface.h"
#include "PropertyTree/PropTree.h"
#pragma endregion Includes

class SVToolAdjustmentDialogSheetClass;
class SVToolClass;
class SVEvaluateClass;
class SVDoubleValueObjectClass;
class SVBoolValueObjectClass;
class SVEnumerateValueObjectClass;

enum SVShiftResultEnum
{
	PROP_SHIFT_TRANS_X		= 100,
	PROP_SHIFT_TRANS_Y		= 101,
	PROP_SHIFT_DISP_X		= 102,
	PROP_SHIFT_DISP_Y		= 103,
	PROP_SHIFT_RESULT_TOP	= 104,
	PROP_SHIFT_RESULT_LEFT	= 105,
};

class SVTADlgTranslationShiftPageClass : public CPropertyPage, public SVTaskObjectValueInterface
{
//	DECLARE_DYNAMIC(SVTADlgTranslationShiftPage)

public:
	SVTADlgTranslationShiftPageClass( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* Parent );
	virtual ~SVTADlgTranslationShiftPageClass();

	HRESULT SetInspectionData();

// Dialog Data
	enum { IDD = IDD_TA_TRANSLATION_SHIFT_DIALOG };

	virtual BOOL OnSetActive() override; 
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

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

	SVToolAdjustmentDialogSheetClass* pParentDialog;
	SVToolClass* pTool;

	SVEvaluateClass* pEvaluateTranslationX;
	SVEvaluateClass* pEvaluateTranslationY;


public:
	BOOL m_performTranslation;
	afx_msg void OnBnClickedPerformTranslation();
	afx_msg void OnBnClickedTranslationXFormulaButton();
	afx_msg void OnBnClickedTranslationYFormulaButton();
	CString StrTranslationXValue;
	CString StrTranslationYValue;
	SVEnumerateComboClass m_ctlShiftModeCombo;

public:
	SVEnumerateValueObjectClass *m_pvoShiftMode;
	afx_msg void OnCbnSelchangeComboShiftMode();
	CButton m_btnFormulaX;
	CButton m_btnFormulaY;
	CButton m_btnSetReference;
	CButton m_btnNormalize;
	CListBox m_ctlShiftValueList;
	CStatic m_ShiftValueStatic;

private:
	long m_lShiftType;
};


