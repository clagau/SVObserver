//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgPolarTransform
//* .File Name       : $Workfile:   SVTADlgPolarTransformPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:16:58  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVMFCControls/SVEnumerateCombo.h"
#include "SVUtilityLibrary/SVGuid.h"
#pragma endregion Includes

#pragma region Declarations
class SVToolAdjustmentDialogSheetClass;
class SVToolClass;
class SVEvaluateClass;
class SVBoolValueObjectClass;
class SVDoubleValueObjectClass;
class SVEnumerateValueObjectClass;
#pragma endregion Declarations

class SVToolAdjustmentDialogPolarTransformPageClass : public CPropertyPage
{
public:
	SVToolAdjustmentDialogPolarTransformPageClass( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* Parent );
	virtual ~SVToolAdjustmentDialogPolarTransformPageClass();
	void SetInspectionData();

	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVToolAdjustmentDialogPolarTransformPageClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL
	
protected:
	void refresh();

	// Generated message map functions
	//{{AFX_MSG(SVToolAdjustmentDialogPolarTransformPageClass)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnCenterXFormulaButton();
	afx_msg void OnCenterYFormulaButton();
	afx_msg void OnStartRadiusFormulaButton();
	afx_msg void OnEndRadiusFormulaButton();
	afx_msg void OnStartAngleFormulaButton();
	afx_msg void OnEndAngleFormulaButton();
	afx_msg void OnUseFormulaCheck();
	afx_msg void OnSelchangeInterpolationModeCombo();
	afx_msg void OnSelchangeAngularMethod();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//******************************************************************************
// Data Element(s):
//******************************************************************************
public:
	//{{AFX_DATA(SVToolAdjustmentDialogPolarTransformPageClass)
	enum { IDD = IDD_TA_POLARTRANSFORM_DIALOG };
	SvMc::SVEnumerateComboClass	m_AngularMethodCombo;
	SvMc::SVEnumerateComboClass	interpolationComboCtrl;
	CString	m_CenterXValue;
	CString	m_CenterYValue;
	CString	m_StartRadiusValue;
	CString	m_StrEndRadiusValue;
	CString	m_StartAngleValue;
	CString	m_EndAngleValue;
	BOOL	bUseFormula;
	//}}AFX_DATA

protected:
	SVToolAdjustmentDialogSheetClass* m_pParentDialog;
	SVToolClass* m_pTool;

	SVGUID m_ImagePolarTransformID;

	SVEvaluateClass*			m_pEvaluateCenterX;
	SVDoubleValueObjectClass*	m_pCenterXResult;

	SVEvaluateClass*			m_pEvaluateCenterY;
	SVDoubleValueObjectClass*	m_pCenterYResult;

	SVEvaluateClass*			m_pEvaluateStartRadius;
	SVDoubleValueObjectClass*	m_pStartRadiusResult;

	SVEvaluateClass*			m_pEvaluateEndRadius;
	SVDoubleValueObjectClass*	m_pEndRadiusResult;

	SVEvaluateClass*			m_pEvaluateStartAngle;
	SVDoubleValueObjectClass*	m_pStartAngleResult;

	SVEvaluateClass*			m_pEvaluateEndAngle;
	SVDoubleValueObjectClass*	m_pEndAngleResult;

	SVBoolValueObjectClass*		 m_pUseFormula;
	SVEnumerateValueObjectClass* m_pAngleMethod;
	SVEnumerateValueObjectClass* m_pInterpolationMode;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

