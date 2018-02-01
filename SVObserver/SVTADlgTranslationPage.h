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
#include "SVUtilityLibrary/SVGuid.h"
#include "SVOGui/ValuesAccessor.h"
#include "SVOGui/DataController.h"
#pragma endregion

#pragma region Declarations
class SVEvaluateClass;
class SVEnumerateValueObjectClass;
#pragma endregion

class SVToolAdjustmentDialogTranslationPageClass : public CPropertyPage
{
public:
	SVToolAdjustmentDialogTranslationPageClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID);
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
	HRESULT SetInspectionData();
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
	SVEvaluateClass* m_pEvaluateTranslationX{ nullptr };
	SVEvaluateClass* m_pEvaluateTranslationY{ nullptr };

	SVGUID m_InspectionID;
	SVGUID m_TaskObjectID;
	typedef SvOg::ValuesAccessor<SvOg::BoundValues> ValueCommand;
	typedef SvOg::DataController<ValueCommand, ValueCommand::value_type> Controller;
	std::unique_ptr<Controller> m_pValues{ nullptr };
#pragma endregion
};
