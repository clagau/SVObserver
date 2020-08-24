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
#pragma endregion Includes

namespace SvOi
{
	class IObjectClass;
}
namespace SvOp
{
class SVImageTransform;
class SVEvaluate;
}
namespace SvVol
{
class SVBoolValueObjectClass;
class SVDoubleValueObjectClass;
}

class SVToolAdjustmentDialogSheetClass;

class SVToolAdjustmentDialogTransformationLearnPageClass : public CPropertyPage
{
// Construction
public:
	SVToolAdjustmentDialogTransformationLearnPageClass( uint32_t inspectionId, uint32_t taskObjectId, SVToolAdjustmentDialogSheetClass* Parent );
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
	SvOi::IObjectClass* m_pTaskObject;

	SvVol::SVBoolValueObjectClass* m_pPerformTranslation;

	SvOp::SVEvaluate* m_pEvaluateTranslationX;
	SvVol::SVDoubleValueObjectClass*	m_pTranslationXResult;

	SvOp::SVEvaluate* m_pEvaluateTranslationY;
	SvVol::SVDoubleValueObjectClass* m_pTranslationYResult;

	SvVol::SVBoolValueObjectClass* m_pPerformRotation;

	SvOp::SVEvaluate* m_pEvaluateRotationX;
	SvVol::SVDoubleValueObjectClass* m_pRotationXResult;

	SvOp::SVEvaluate* m_pEvaluateRotationY;
	SvVol::SVDoubleValueObjectClass* m_pRotationYResult;

	SvOp::SVEvaluate* m_pEvaluateRotationAngle;
	SvVol::SVDoubleValueObjectClass* m_pRotationAngleResult;

	SvOp::SVImageTransform* m_pImageTransform;

	SvVol::SVDoubleValueObjectClass* m_pLearnedTranslationX;
	SvVol::SVDoubleValueObjectClass* m_pLearnedTranslationY;
	SvVol::SVDoubleValueObjectClass* m_pLearnedRotationX;
	SvVol::SVDoubleValueObjectClass* m_pLearnedRotationY;
	SvVol::SVDoubleValueObjectClass* m_pLearnedRotationAngle;

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

