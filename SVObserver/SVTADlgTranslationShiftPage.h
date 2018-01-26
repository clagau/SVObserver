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
#include "SVRPropertyTree/SVRPropTree.h"
#include "SVUtilityLibrary/SVGuid.h"
#include "SVOGui/ValuesAccessor.h"
#include "SVOGui/DataController.h"
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

class SVTADlgTranslationShiftPageClass : public CPropertyPage
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

	SVToolAdjustmentDialogSheetClass* pParentDialog;
	SVToolClass* m_pTool;

	SVEvaluateClass* pEvaluateTranslationX;
	SVEvaluateClass* pEvaluateTranslationY;


public:
	BOOL m_performTranslation;
	afx_msg void OnBnClickedPerformTranslation();
	afx_msg void OnBnClickedTranslationXFormulaButton();
	afx_msg void OnBnClickedTranslationYFormulaButton();
	CString m_TranslationXValue;
	CString m_TranslationYValue;
	SvMc::SVEnumerateComboClass m_ctlShiftModeCombo;

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
	typedef SvOg::ValuesAccessor<SvOg::BoundValues> ValueCommand;
	typedef SvOg::DataController<ValueCommand, ValueCommand::value_type> Controller;
	std::unique_ptr<Controller> m_pValues{ nullptr };

	long m_lShiftType;
};


