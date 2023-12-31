//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgTranslationPage
//* .File Name       : $Workfile:   TADlgTranslationPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   15 Jan 2014 16:51:54  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVMFCControls/SVEnumerateCombo.h"
#include "SVOGuiUtility/DataController.h"
#pragma endregion

#pragma region Declarations
namespace SvOp
{
class SVEvaluate;
}
#pragma endregion

class TADlgTranslationPage : public CPropertyPage
{
public:
	TADlgTranslationPage(uint32_t inspectionId, uint32_t taskObjectId);
	virtual ~TADlgTranslationPage();

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
	//{{AFX_DATA(TADlgTranslationPage)
public:
	enum { IDD = IDD_TA_TRANSLATION_DIALOG };
	SvMc::SVEnumerateCombo	m_cbInterpolation;
	CString	m_TranslationXValue;
	CString	m_TranslationYValue;
	BOOL	m_performTranslation{ false };
	//}}AFX_DATA
#pragma endregion Data Elements

protected:
	SvOp::SVEvaluate* m_pEvaluateTranslationX{ nullptr };
	SvOp::SVEvaluate* m_pEvaluateTranslationY{ nullptr };

	const uint32_t m_InspectionID;
	const uint32_t m_TaskObjectID;
	SvOgu::ValueController m_Values;
#pragma endregion
};
