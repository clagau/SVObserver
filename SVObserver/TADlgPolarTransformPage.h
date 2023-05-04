//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgPolarTransform
//* .File Name       : $Workfile:   TADlgPolarTransformPage.h  $
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
class TADlgSheetClass;
#pragma endregion Declarations

class TADlgPolarTransformPage : public CPropertyPage
{
public:
	TADlgPolarTransformPage( uint32_t inspectionId, uint32_t taskObjectId, TADlgSheetClass* Parent );
	virtual ~TADlgPolarTransformPage();
	void SetInspectionData();

	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(TADlgPolarTransformPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL
	
protected:
	void refresh();

	// Generated message map functions
	//{{AFX_MSG(TADlgPolarTransformPage)
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
	//{{AFX_DATA(TADlgPolarTransformPage)
	enum { IDD = IDD_TA_POLARTRANSFORM_DIALOG };
	SvMc::SVEnumerateCombo	m_AngularMethodCombo;
	SvMc::SVEnumerateCombo	interpolationComboCtrl;
	CString	m_CenterXValue = _T("");
	CString	m_CenterYValue = _T("");
	CString	m_StartRadiusValue = _T("");
	CString	m_StrEndRadiusValue = _T("");
	CString	m_StartAngleValue = _T("");
	CString	m_EndAngleValue = _T("");
	BOOL	bUseFormula = FALSE;
	//}}AFX_DATA

protected:
	TADlgSheetClass* m_pParentDialog;
	SvOi::IObjectClass* m_pTaskObject = nullptr;

	uint32_t m_ImagePolarTransformID = SvDef::InvalidObjectId;

	SvOp::SVEvaluate* m_pEvaluateCenterX = nullptr;
	SvVol::SVDoubleValueObjectClass* m_pCenterXResult = nullptr;

	SvOp::SVEvaluate* m_pEvaluateCenterY = nullptr;
	SvVol::SVDoubleValueObjectClass* m_pCenterYResult = nullptr;

	SvOp::SVEvaluate* m_pEvaluateStartRadius = nullptr;
	SvVol::SVDoubleValueObjectClass* m_pStartRadiusResult = nullptr;

	SvOp::SVEvaluate* m_pEvaluateEndRadius = nullptr;
	SvVol::SVDoubleValueObjectClass* m_pEndRadiusResult = nullptr;

	SvOp::SVEvaluate* m_pEvaluateStartAngle = nullptr;
	SvVol::SVDoubleValueObjectClass* m_pStartAngleResult = nullptr;

	SvOp::SVEvaluate* m_pEvaluateEndAngle = nullptr;
	SvVol::SVDoubleValueObjectClass* m_pEndAngleResult = nullptr;

	SvVol::SVBoolValueObjectClass*	m_pUseFormula = nullptr;
	SvVol::SVEnumerateValueObjectClass* m_pAngleMethod = nullptr;
	SvVol::SVEnumerateValueObjectClass* m_pInterpolationMode = nullptr;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

