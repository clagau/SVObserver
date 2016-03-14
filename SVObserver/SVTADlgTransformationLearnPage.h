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
#include "SVTaskObjectInterfaceClass.h"
#pragma endregion Includes

class SVToolAdjustmentDialogSheetClass;
class SVImageTransformClass;
class SVToolClass;
class SVBoolValueObjectClass;
class SVEvaluateClass;
class SVDoubleValueObjectClass;

class SVToolAdjustmentDialogTransformationLearnPageClass : public CPropertyPage, public SVTaskObjectInterfaceClass
{
// Construction
public:
	SVToolAdjustmentDialogTransformationLearnPageClass( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* Parent );
	virtual ~SVToolAdjustmentDialogTransformationLearnPageClass();
	virtual HRESULT SetInspectionData() override;

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVToolAdjustmentDialogTransformationLearnPageClass)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
protected:
	
	void refresh();
	void refreshLearnedValues();

	// Generated message map functions
	//{{AFX_MSG(SVToolAdjustmentDialogTransformationLearnPageClass)
	virtual BOOL OnInitDialog();
	afx_msg void OnLearnButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//******************************************************************************
// Data Element(s):
//******************************************************************************
public:
	//{{AFX_DATA(SVToolAdjustmentDialogTransformationLearnPageClass)
	enum { IDD = IDD_TA_TRANSFORMATION_LEARN_DIALOG };
	CString	StrTranslationXValue;
	CString	StrTranslationYValue;
	CString	StrRotationXValue;
	CString	StrRotationYValue;
	CString	StrRotationAngleValue;
	CString	StrLearnedTranslationXValue;
	CString	StrLearnedTranslationYValue;
	CString	StrLearnedRotationXValue;
	CString	StrLearnedRotationYValue;
	CString	StrLearnedRotationAngleValue;
	//}}AFX_DATA

protected:
	SVToolAdjustmentDialogSheetClass* pParentDialog;
	SVToolClass* pTool;

	SVBoolValueObjectClass*		pPerformTranslation;

	SVEvaluateClass*			pEvaluateTranslationX;
	SVDoubleValueObjectClass*	pTranslationXResult;

	SVEvaluateClass*			pEvaluateTranslationY;
	SVDoubleValueObjectClass*	pTranslationYResult;

	SVBoolValueObjectClass*		pPerformRotation;

	SVEvaluateClass*			pEvaluateRotationX;
	SVDoubleValueObjectClass*	pRotationXResult;

	SVEvaluateClass*			pEvaluateRotationY;
	SVDoubleValueObjectClass*	pRotationYResult;

	SVEvaluateClass*			pEvaluateRotationAngle;
	SVDoubleValueObjectClass*	pRotationAngleResult;

	SVImageTransformClass*	    pImageTransform;

	SVDoubleValueObjectClass*   pLearnedTranslationX;
	SVDoubleValueObjectClass*   pLearnedTranslationY;
	SVDoubleValueObjectClass*   pLearnedRotationX;
	SVDoubleValueObjectClass*   pLearnedRotationY;
	SVDoubleValueObjectClass*   pLearnedRotationAngle;

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

