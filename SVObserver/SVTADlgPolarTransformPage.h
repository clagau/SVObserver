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
#include "Definitions/ObjectDefines.h"
#include "SVMFCControls/SVEnumerateCombo.h"
#pragma endregion Includes

#pragma region Declarations
namespace SvOi
{
	class IObjectClass;
}
namespace SvOp
{
class SVEvaluate;
}
namespace SvVol
{
class SVBoolValueObjectClass;
class SVDoubleValueObjectClass;
class SVEnumerateValueObjectClass;
}
class SVToolAdjustmentDialogSheetClass;
#pragma endregion Declarations

class SVTADlgPolarTransformPage : public CPropertyPage
{
public:
	SVTADlgPolarTransformPage( uint32_t inspectionId, uint32_t taskObjectId, SVToolAdjustmentDialogSheetClass* Parent );
	virtual ~SVTADlgPolarTransformPage();
	void SetInspectionData();

	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVTADlgPolarTransformPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL
	
protected:
	void refresh();

	// Generated message map functions
	//{{AFX_MSG(SVTADlgPolarTransformPage)
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
	//{{AFX_DATA(SVTADlgPolarTransformPage)
	enum { IDD = IDD_TA_POLARTRANSFORM_DIALOG };
	SvMc::SVEnumerateCombo	m_AngularMethodCombo;
	SvMc::SVEnumerateCombo	interpolationComboCtrl;
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
	SvOi::IObjectClass* m_pTaskObject;

	uint32_t m_ImagePolarTransformID = SvDef::InvalidObjectId;

	SvOp::SVEvaluate* m_pEvaluateCenterX;
	SvVol::SVDoubleValueObjectClass* m_pCenterXResult;

	SvOp::SVEvaluate* m_pEvaluateCenterY;
	SvVol::SVDoubleValueObjectClass* m_pCenterYResult;

	SvOp::SVEvaluate* m_pEvaluateStartRadius;
	SvVol::SVDoubleValueObjectClass* m_pStartRadiusResult;

	SvOp::SVEvaluate* m_pEvaluateEndRadius;
	SvVol::SVDoubleValueObjectClass* m_pEndRadiusResult;

	SvOp::SVEvaluate* m_pEvaluateStartAngle;
	SvVol::SVDoubleValueObjectClass* m_pStartAngleResult;

	SvOp::SVEvaluate* m_pEvaluateEndAngle;
	SvVol::SVDoubleValueObjectClass* m_pEndAngleResult;

	SvVol::SVBoolValueObjectClass*	m_pUseFormula;
	SvVol::SVEnumerateValueObjectClass* m_pAngleMethod = nullptr;
	SvVol::SVEnumerateValueObjectClass* m_pInterpolationMode;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

