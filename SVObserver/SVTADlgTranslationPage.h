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
#include "SVTaskObjectValueInterface.h"
#include "SVEnumerateCombo.h"
#pragma endregion

#pragma region Declarations
class SVToolAdjustmentDialogSheetClass;
class SVToolClass;
class SVEvaluateClass;
class SVDoubleValueObjectClass;
class SVBoolValueObjectClass;
#pragma endregion

class SVToolAdjustmentDialogTranslationPageClass : public CPropertyPage, public SVTaskObjectValueInterface
{
public:
	SVToolAdjustmentDialogTranslationPageClass( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* Parent );
	virtual ~SVToolAdjustmentDialogTranslationPageClass();

#pragma region Protected Methods
protected:
#pragma region MFC Methods
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVToolAdjustmentDialogTranslationPageClass)
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnSetActive();
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(SVToolAdjustmentDialogTranslationPageClass)
	afx_msg void OnXFormulaButton();
	afx_msg void OnYFormulaButton();
	afx_msg void OnPerformTranslation();
	afx_msg void OnSelChangeInterpolationModeCombo();
	//}}AFX_MSG
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
	SVEnumerateComboClass	m_cbInterpolation;
	CString	m_strTranslationXValue;
	CString	m_strTranslationYValue;
	BOOL	m_performTranslation;
	//}}AFX_DATA
#pragma endregion Data Elements

protected:
	SVToolAdjustmentDialogSheetClass* m_pParentDialog;
	SVToolClass* m_pTool;

	SVEvaluateClass* m_pEvaluateTranslationX;
	SVEvaluateClass* m_pEvaluateTranslationY;
	SVEnumerateValueObjectClass* m_pInterpolationMode;
#pragma endregion
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

