//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgTranslationPage
//* .File Name       : $Workfile:   SVTADlgTranslationPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   15 May 2014 13:07:16  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTADlgTranslationPage.h"
#include "InspectionEngine/SVEvaluate.h"
#include "SvOGui/SVFormulaEditorSheet.h"
#include "InspectionEngine/SVTransformationTool.h"
#pragma endregion

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogTranslationPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVToolAdjustmentDialogTranslationPageClass)
	ON_BN_CLICKED(IDC_TRANSLATION_X_FORMULA_BUTTON, OnXFormulaButton)
	ON_BN_CLICKED(IDC_TRANSLATION_Y_FORMULA_BUTTON, OnYFormulaButton)
	ON_BN_CLICKED(IDC_PERFORM_TRANSLATION, OnPerformTranslation)
	ON_CBN_SELCHANGE(IDC_INTERPOLATION_MODE_COMBO, OnSelChangeInterpolationModeCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#pragma region Constructor
SVToolAdjustmentDialogTranslationPageClass::SVToolAdjustmentDialogTranslationPageClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID)
: CPropertyPage(SVToolAdjustmentDialogTranslationPageClass::IDD)
, m_InspectionID{ rInspectionID }
, m_TaskObjectID{ rTaskObjectID }
//TaskID is set later
, m_Values {SvOg::BoundValues {rInspectionID, GUID_NULL}}
{
}
#pragma endregion

SVToolAdjustmentDialogTranslationPageClass::~SVToolAdjustmentDialogTranslationPageClass()
{
}

#pragma region Protected Methods
#pragma region MFC Methods
/////////////////////////////////////////////////////////////////////////////
// SVToolAdjustmentDialogTranslationPageClass message handlers
BOOL SVToolAdjustmentDialogTranslationPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	SvOi::IObjectClass* pTool = SvOi::getObject(m_TaskObjectID);
	if (nullptr != pTool)
	{
		// Get Evaluate Object...
		SvDef::SVObjectTypeInfoStruct evaluateObjectInfo;
		evaluateObjectInfo.ObjectType = SvPb::SVMathContainerObjectType;

		// Get Evaluate Object for the X coordinate...
		evaluateObjectInfo.SubType = SvPb::SVEvaluateTranslationXObjectType;
		m_pEvaluateTranslationX = dynamic_cast<SVEvaluateClass*> (pTool->getFirstObject(evaluateObjectInfo));

		// Get Evaluate Object for the Y coordinate...
		evaluateObjectInfo.SubType = SvPb::SVEvaluateTranslationYObjectType;
		m_pEvaluateTranslationY = dynamic_cast<SVEvaluateClass*> (pTool->getFirstObject(evaluateObjectInfo));

		//Image Transformation
		SvDef::SVObjectTypeInfoStruct objectInfo;
		objectInfo.ObjectType = SvPb::SVTransformObjectType;
		objectInfo.SubType = SvPb::SVImageTransformObjectType;

		SvOi::IObjectClass* pImageTransform = pTool->getFirstObject(objectInfo);
		if (nullptr != pImageTransform)
		{
			m_Values.SetTaskID(pImageTransform->GetUniqueObjectID());
			m_Values.Init();

			const SvOi::NameValueVector& rInterpolationModeList = m_Values.GetEnumTypes(SVOutputInterpolationModeObjectGuid);
			m_cbInterpolation.SetEnumTypes(rInterpolationModeList);
			long CurrentSelection = m_Values.Get<long>(SVOutputInterpolationModeObjectGuid);
			m_cbInterpolation.SetCurSelItemData(CurrentSelection);
		}
	}

	// Check...
	if( nullptr != m_pEvaluateTranslationX && nullptr != m_pEvaluateTranslationY)
	{
		m_performTranslation = m_Values.Get<bool>(SVPerformTranslationObjectGuid);
		UpdateData(false);
		refresh();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void SVToolAdjustmentDialogTranslationPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVToolAdjustmentDialogTranslationPageClass)
	DDX_Control(pDX, IDC_INTERPOLATION_MODE_COMBO, m_cbInterpolation);
	DDX_Text(pDX, IDC_TRANSLATION_X_EDIT, m_TranslationXValue);
	DDX_Text(pDX, IDC_TRANSLATION_Y_EDIT, m_TranslationYValue);
	DDX_Check(pDX, IDC_PERFORM_TRANSLATION, m_performTranslation);
	//}}AFX_DATA_MAP
}

BOOL SVToolAdjustmentDialogTranslationPageClass::OnSetActive()
{
	long CurrentSelection = m_Values.Get<long>(SVOutputInterpolationModeObjectGuid);
	m_cbInterpolation.SetCurSelItemData(CurrentSelection);

	return CPropertyPage::OnSetActive();
}

void SVToolAdjustmentDialogTranslationPageClass::OnXFormulaButton() 
{
	if (nullptr != m_pEvaluateTranslationX)
	{
		std::string Caption = m_pEvaluateTranslationX->GetName();
		Caption += _T(" Formula");

		SvDef::SVObjectTypeInfoStruct info(SvPb::SVMathContainerObjectType, SvPb::SVEvaluateTranslationXObjectType);
		SvOg::SVFormulaEditorSheetClass dlg(m_InspectionID, m_TaskObjectID, info, Caption.c_str());
		dlg.DoModal();
		
		refresh();
	}
}

void SVToolAdjustmentDialogTranslationPageClass::OnYFormulaButton() 
{
	if (nullptr != m_pEvaluateTranslationY)
	{
		std::string Caption = m_pEvaluateTranslationY->GetName();
		Caption += _T(" Formula");

		SvDef::SVObjectTypeInfoStruct info(SvPb::SVMathContainerObjectType, SvPb::SVEvaluateTranslationYObjectType);
		SvOg::SVFormulaEditorSheetClass dlg(m_InspectionID, m_TaskObjectID, info, Caption.c_str());
		dlg.DoModal();
		
		refresh();
	}
}

void SVToolAdjustmentDialogTranslationPageClass::OnPerformTranslation() 
{
	refresh();
}

void SVToolAdjustmentDialogTranslationPageClass::OnSelChangeInterpolationModeCombo() 
{
	SetInspectionData();
}
#pragma endregion MFC Methods

void SVToolAdjustmentDialogTranslationPageClass::SetInspectionData()
{
	UpdateData( true ); // get data from dialog

	m_Values.Set<bool>(SVPerformTranslationObjectGuid, m_performTranslation ? true : false);

	int CurrentSelection = m_cbInterpolation.GetCurSel();
	if(0 <= CurrentSelection)
	{
		long lValue = static_cast<long> (m_cbInterpolation.GetItemData(CurrentSelection));
		m_Values.Set<long>(SVOutputInterpolationModeObjectGuid, lValue);
	}
	 m_Values.Commit();
}

void SVToolAdjustmentDialogTranslationPageClass::refresh()
{
	SetInspectionData();
	
	if (nullptr != m_pEvaluateTranslationX && nullptr != m_pEvaluateTranslationX->getOutputMathResult())
	{
		double Value{ 0.0 };
		m_pEvaluateTranslationX->getOutputMathResult()->GetValue(Value);
		m_TranslationXValue.Format(_T("%.0f"), Value);
	}
	if (nullptr != m_pEvaluateTranslationY && nullptr != m_pEvaluateTranslationY->getOutputMathResult())
	{
		double Value{ 0.0 };
		m_pEvaluateTranslationY->getOutputMathResult()->GetValue(Value);
		m_TranslationYValue.Format(_T("%.0f"), Value);
	}

	UpdateData(false); // set data to dialog
}
#pragma endregion
