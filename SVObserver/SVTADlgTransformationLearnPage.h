//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogTransformationLearnPage
//* .File Name       : $Workfile:   SVTADlgTransformationLearnPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:23:36  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilityLibrary/SVGuid.h"
#pragma endregion Includes

class SVToolAdjustmentDialogSheetClass;
class SVImageTransformClass;
class SVToolClass;
class SVBoolValueObjectClass;
class SVEvaluateClass;
class SVDoubleValueObjectClass;

class SVToolAdjustmentDialogTransformationLearnPageClass : public CPropertyPage
{
// Construction
public:
	SVToolAdjustmentDialogTransformationLearnPageClass( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* Parent );
	virtual ~SVToolAdjustmentDialogTransformationLearnPageClass();
	HRESULT SetInspectionData();

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVToolAdjustmentDialogTransformationLearnPageClass)
	public:
	virtual BOOL OnSetActive() override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL
	
protected:
	
	void refresh();
	void refreshLearnedValues();

	// Generated message map functions
	//{{AFX_MSG(SVToolAdjustmentDialogTransformationLearnPageClass)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnLearnButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//******************************************************************************
// Data Element(s):
//******************************************************************************
public:
	//{{AFX_DATA(SVToolAdjustmentDialogTransformationLearnPageClass)
	enum { IDD = IDD_TA_TRANSFORMATION_LEARN_DIALOG };
	CString	m_TranslationXValue;
	CString	m_TranslationYValue;
	CString	m_RotationXValue;
	CString	m_RotationYValue;
	CString	m_RotationAngleValue;
	CString	m_LearnedTranslationXValue;
	CString	m_LearnedTranslationYValue;
	CString	m_LearnedRotationXValue;
	CString	m_LearnedRotationYValue;
	CString	m_LearnedRotationAngleValue;
	//}}AFX_DATA

protected:
	SVToolAdjustmentDialogSheetClass* m_pParentDialog;
	SVToolClass* m_pTool;

	SVBoolValueObjectClass*		m_pPerformTranslation;

	SVEvaluateClass*			m_pEvaluateTranslationX;
	SVDoubleValueObjectClass*	m_pTranslationXResult;

	SVEvaluateClass*			m_pEvaluateTranslationY;
	SVDoubleValueObjectClass*	m_pTranslationYResult;

	SVBoolValueObjectClass*		m_pPerformRotation;

	SVEvaluateClass*			m_pEvaluateRotationX;
	SVDoubleValueObjectClass*	m_pRotationXResult;

	SVEvaluateClass*			m_pEvaluateRotationY;
	SVDoubleValueObjectClass*	m_pRotationYResult;

	SVEvaluateClass*			m_pEvaluateRotationAngle;
	SVDoubleValueObjectClass*	m_pRotationAngleResult;

	SVImageTransformClass*	    m_pImageTransform;

	SVDoubleValueObjectClass*   m_pLearnedTranslationX;
	SVDoubleValueObjectClass*   m_pLearnedTranslationY;
	SVDoubleValueObjectClass*   m_pLearnedRotationX;
	SVDoubleValueObjectClass*   m_pLearnedRotationY;
	SVDoubleValueObjectClass*   m_pLearnedRotationAngle;

	CFont angleFont;

private:
	double m_translationXValue;
	double m_translationYValue;
	double m_rotationXValue;
	double m_rotationYValue;
	double m_rotationAngleValue;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

