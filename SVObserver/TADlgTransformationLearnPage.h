//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogTransformationLearnPage
//* .File Name       : $Workfile:   TADlgTransformationLearnPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:23:36  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "Definitions/ObjectDefines.h"
#pragma endregion Includes

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
}

class TADlgSheetClass;

class TADlgTransformationLearnPage : public CPropertyPage
{
// Construction
public:
	TADlgTransformationLearnPage( uint32_t inspectionId, uint32_t taskObjectId, TADlgSheetClass* Parent );
	virtual ~TADlgTransformationLearnPage();
	HRESULT SetInspectionData();

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(TADlgTransformationLearnPage)
	public:
	virtual BOOL OnSetActive() override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL
	
protected:
	
	void refresh();
	void refreshLearnedValues();

	// Generated message map functions
	//{{AFX_MSG(TADlgTransformationLearnPage)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnLearnButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//******************************************************************************
// Data Element(s):
//******************************************************************************
public:
	//{{AFX_DATA(TADlgTransformationLearnPage)
	enum { IDD = IDD_TA_TRANSFORMATION_LEARN_DIALOG };
	CString	m_TranslationXValue = _T("");
	CString	m_TranslationYValue = _T("");
	CString	m_RotationXValue = _T("");
	CString	m_RotationYValue = _T("");
	CString	m_RotationAngleValue = _T("");
	CString	m_LearnedTranslationXValue = _T("");
	CString	m_LearnedTranslationYValue = _T("");
	CString	m_LearnedRotationXValue = _T("");
	CString	m_LearnedRotationYValue = _T("");
	CString	m_LearnedRotationAngleValue = _T("");
	//}}AFX_DATA

protected:
	SvOi::IObjectClass* m_pTaskObject = nullptr;

	SvVol::SVBoolValueObjectClass* m_pPerformTranslation = nullptr;

	SvOp::SVEvaluate* m_pEvaluateTranslationX = nullptr;
	SvVol::SVDoubleValueObjectClass*	m_pTranslationXResult = nullptr;

	SvOp::SVEvaluate* m_pEvaluateTranslationY = nullptr;
	SvVol::SVDoubleValueObjectClass* m_pTranslationYResult = nullptr;

	SvVol::SVBoolValueObjectClass* m_pPerformRotation = nullptr;

	SvOp::SVEvaluate* m_pEvaluateRotationX = nullptr;
	SvVol::SVDoubleValueObjectClass* m_pRotationXResult = nullptr;

	SvOp::SVEvaluate* m_pEvaluateRotationY = nullptr;
	SvVol::SVDoubleValueObjectClass* m_pRotationYResult = nullptr;

	SvOp::SVEvaluate* m_pEvaluateRotationAngle = nullptr;
	SvVol::SVDoubleValueObjectClass* m_pRotationAngleResult = nullptr;

	uint32_t m_inspectionId {SvDef::InvalidObjectId};
	uint32_t m_imageTransformId {SvDef::InvalidObjectId};

	CFont angleFont;

private:
	double m_translationXValue = 0.0;
	double m_translationYValue = 0.0;
	double m_rotationXValue = 0.0;
	double m_rotationYValue = 0.0;
	double m_rotationAngleValue = 0.0;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

