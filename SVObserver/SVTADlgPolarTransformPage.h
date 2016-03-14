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
#include "SVEnumerateCombo.h"
#include "SVTaskObjectInterfaceClass.h"
#pragma endregion Includes

class SVToolAdjustmentDialogSheetClass;
class SVToolClass;
class SVEvaluateClass;

class SVToolAdjustmentDialogPolarTransformPageClass : public CPropertyPage, public SVTaskObjectInterfaceClass
{
public:
	SVToolAdjustmentDialogPolarTransformPageClass( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* Parent );
	virtual ~SVToolAdjustmentDialogPolarTransformPageClass();
	virtual HRESULT SetInspectionData() override;

	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVToolAdjustmentDialogPolarTransformPageClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
protected:
	void refresh();

	// Generated message map functions
	//{{AFX_MSG(SVToolAdjustmentDialogPolarTransformPageClass)
	virtual BOOL OnInitDialog();
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
	SVEnumerateComboClass	m_AngularMethodCombo;
	SVEnumerateComboClass	interpolationComboCtrl;
	CString	StrCenterXValue;
	CString	StrCenterYValue;
	CString	StrStartRadiusValue;
	CString	StrEndRadiusValue;
	CString	StrStartAngleValue;
	CString	StrEndAngleValue;
	BOOL	bUseFormula;
	//}}AFX_DATA

protected:
	SVToolAdjustmentDialogSheetClass* pParentDialog;
	SVToolClass* pTool;

	SVEvaluateClass*			pEvaluateCenterX;
	SVDoubleValueObjectClass*	pCenterXResult;

	SVEvaluateClass*			pEvaluateCenterY;
	SVDoubleValueObjectClass*	pCenterYResult;

	SVEvaluateClass*			pEvaluateStartRadius;
	SVDoubleValueObjectClass*	pStartRadiusResult;

	SVEvaluateClass*			pEvaluateEndRadius;
	SVDoubleValueObjectClass*	pEndRadiusResult;

	SVEvaluateClass*			pEvaluateStartAngle;
	SVDoubleValueObjectClass*	pStartAngleResult;

	SVEvaluateClass*			pEvaluateEndAngle;
	SVDoubleValueObjectClass*	pEndAngleResult;

	SVBoolValueObjectClass*		 pUseFormula;
	SVEnumerateValueObjectClass* m_pAngleMethod;
	SVEnumerateValueObjectClass* pInterpolationMode;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

