//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgTranslationPage
//* .File Name       : $Workfile:   SVTADlgTranslationPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   15 Jan 2014 16:51:54  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVMFCControls/SVEnumerateCombo.h"
#include "SVOGui/ValuesAccessor.h"
#include "SVOGui/DataController.h"
#pragma endregion

#pragma region Declarations
namespace SvOp
{
class SVEvaluateClass;
}
#pragma endregion

class SVToolAdjustmentDialogTranslationPageClass : public CPropertyPage
{
public:
	SVToolAdjustmentDialogTranslationPageClass(uint32_t inspectionId, uint32_t taskObjectId);
	virtual ~SVToolAdjustmentDialogTranslationPageClass();

#pragma region Protected Methods
protected:
#pragma region MFC Methods
protected:
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual BOOL OnSetActive() override;

	afx_msg void OnXFormulaButton();
	afx_msg void OnYFormulaButton();
	afx_msg void OnPerformTranslation();
	afx_msg void OnSelChangeInterpolationModeCombo();
	DECLARE_MESSAGE_MAP()
#pragma endregion MFC Methods
	void SetInspectionData();
	void refresh();
#pragma endregion

#pragma region Member variables
#pragma region Data Elements
	//{{AFX_DATA(SVToolAdjustmentDialogTranslationPageClass)
public:
	enum { IDD = IDD_TA_TRANSLATION_DIALOG };
	SvMc::SVEnumerateComboClass	m_cbInterpolation;
	CString	m_TranslationXValue;
	CString	m_TranslationYValue;
	BOOL	m_performTranslation{ false };
	//}}AFX_DATA
#pragma endregion Data Elements

protected:
	SvOp::SVEvaluateClass* m_pEvaluateTranslationX{ nullptr };
	SvOp::SVEvaluateClass* m_pEvaluateTranslationY{ nullptr };

	const uint32_t m_InspectionID;
	const uint32_t m_TaskObjectID;
	typedef SvOg::DataController<SvOg::ValuesAccessor, SvOg::ValuesAccessor::value_type> Controller;
	Controller m_Values;
#pragma endregion
};
