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
#include "SVMFCControls/SVEnumerateCombo.h"
#include "SVOGui/DataController.h"
#pragma endregion

#pragma region Declarations
namespace SvOp
{
class SVEvaluate;
}
#pragma endregion;

class SVToolAdjustmentDialogRotationPageClass : public CPropertyPage
{
public:
	SVToolAdjustmentDialogRotationPageClass( uint32_t inspectionId, uint32_t taskObjectId);
	virtual ~SVToolAdjustmentDialogRotationPageClass();

#pragma region Protected Methods
protected:
#pragma region MFC Methods
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVToolAdjustmentDialogRotationPageClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual BOOL OnSetActive() override;
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(SVToolAdjustmentDialogRotationPageClass)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnAngleFormulaButton();
	afx_msg void OnXFormulaButton();
	afx_msg void OnYFormulaButton();
	afx_msg void OnPerformRotation();
	afx_msg void OnSelChangeInterpolationModeCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
#pragma endregion MFC Methods

	void SetInspectionData();
	void refresh();
#pragma endregion

#pragma region Member variables
#pragma region Data Elements
public:
	//{{AFX_DATA(SVToolAdjustmentDialogRotationPageClass)
	enum { IDD = IDD_TA_ROTATION_DIALOG };
	SvMc::SVEnumerateCombo	m_cbInterpolation;
	CString	m_RotationAngleValue;
	CString	m_RotationXValue;
	CString	m_RotationYValue;
	BOOL	m_performRotation{ false };
	//}}AFX_DATA
#pragma endregion Data Elements

protected:
	CFont angleFont;

	SvOp::SVEvaluate*	m_pEvaluateRotationX{ nullptr };
	SvOp::SVEvaluate*	m_pEvaluateRotationY{ nullptr };
	SvOp::SVEvaluate*	m_pEvaluateRotationAngle{ nullptr };

	const uint32_t m_InspectionID;
	const uint32_t m_TaskObjectID;
	SvOg::ValueController m_Values;
#pragma endregion
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

