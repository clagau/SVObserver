//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgRotationPage
//* .File Name       : $Workfile:   SVTADlgRotationPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   15 Jan 2014 16:47:56  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVTaskObjectInterfaceClass.h"
#include "SVEnumerateCombo.h"
#pragma endregion

#pragma region Declarations
class SVEvaluateClass;
class SVToolAdjustmentDialogSheetClass;
class SVToolClass;
class SVDoubleValueObjectClass;
class SVBoolValueObjectClass;
#pragma endregion;

class SVToolAdjustmentDialogRotationPageClass : public CPropertyPage, public SVTaskObjectInterfaceClass
{
public:
	SVToolAdjustmentDialogRotationPageClass( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* Parent );
	virtual ~SVToolAdjustmentDialogRotationPageClass();

#pragma region Protected Methods
protected:
#pragma region MFC Methods
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVToolAdjustmentDialogRotationPageClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnSetActive();
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(SVToolAdjustmentDialogRotationPageClass)
	virtual BOOL OnInitDialog();
	afx_msg void OnAngleFormulaButton();
	afx_msg void OnXFormulaButton();
	afx_msg void OnYFormulaButton();
	afx_msg void OnPerformRotation();
	afx_msg void OnSelChangeInterpolationModeCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
#pragma endregion MFC Methods

	virtual HRESULT SetInspectionData() override;
	void refresh();
#pragma endregion

#pragma region Member variables
#pragma region Data Elements
public:
	//{{AFX_DATA(SVToolAdjustmentDialogRotationPageClass)
	enum { IDD = IDD_TA_ROTATION_DIALOG };
	SVEnumerateComboClass	m_cbInterpolation;
	CString	m_strRotationAngleValue;
	CString	m_strRotationXValue;
	CString	m_strRotationYValue;
	BOOL	m_performRotation;
	//}}AFX_DATA
#pragma endregion Data Elements

protected:
	SVToolAdjustmentDialogSheetClass* m_pParentDialog;
	SVToolClass* m_pTool;

	SVEvaluateClass*			m_pEvaluateRotationX;
	SVDoubleValueObjectClass*	m_pRotationXResult;

	SVEvaluateClass*			m_pEvaluateRotationY;
	SVDoubleValueObjectClass*	m_pRotationYResult;

	SVEvaluateClass*			m_pEvaluateRotationAngle;
	SVDoubleValueObjectClass*	m_pRotationAngleResult;

	SVBoolValueObjectClass*		m_pPerformRotation;
	SVEnumerateValueObjectClass* m_pInterpolationMode;

	CFont angleFont;
#pragma endregion
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

