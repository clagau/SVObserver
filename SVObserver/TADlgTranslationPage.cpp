//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgTranslationPage
//* .File Name       : $Workfile:   TADlgTranslationPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   15 May 2014 13:07:16  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TADlgTranslationPage.h"
#include "Operators/SVEvaluate.h"
#include "SvOGui/SVFormulaEditorSheet.h"
#pragma endregion

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(TADlgTranslationPage, CPropertyPage)
	//{{AFX_MSG_MAP(TADlgTranslationPage)
	ON_BN_CLICKED(IDC_TRANSLATION_X_FORMULA_BUTTON, OnXFormulaButton)
	ON_BN_CLICKED(IDC_TRANSLATION_Y_FORMULA_BUTTON, OnYFormulaButton)
	ON_BN_CLICKED(IDC_PERFORM_TRANSLATION, OnPerformTranslation)
	ON_CBN_SELCHANGE(IDC_INTERPOLATION_MODE_COMBO, OnSelChangeInterpolationModeCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#pragma region Constructor
TADlgTranslationPage::TADlgTranslationPage(uint32_t inspectionId, uint32_t taskObjectId)
: CPropertyPage(TADlgTranslationPage::IDD)
, m_InspectionID{ inspectionId }
, m_TaskObjectID{ taskObjectId }
//TaskID is set later
, m_Values{ SvOgu::BoundValues {inspectionId, SvDef::InvalidObjectId} }
{
}
#pragma endregion

TADlgTranslationPage::~TADlgTranslationPage()
{
}

#pragma region Protected Methods
#pragma region MFC Methods
/////////////////////////////////////////////////////////////////////////////
// TADlgTranslationPage message handlers
BOOL TADlgTranslationPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	SvOi::IObjectClass* pTool = SvOi::getObject(m_TaskObjectID);
	if (nullptr != pTool)
	{
		// Get Evaluate Object...
		SvDef::SVObjectTypeInfoStruct evaluateObjectInfo;
		evaluateObjectInfo.m_ObjectType = SvPb::SVMathContainerObjectType;

		// Get Evaluate Object for the X coordinate...
		evaluateObjectInfo.m_SubType = SvPb::SVEvaluateTranslationXObjectType;
		m_pEvaluateTranslationX = dynamic_cast<SvOp::SVEvaluate*> (pTool->getFirstObject(evaluateObjectInfo));

		// Get Evaluate Object for the Y coordinate...
		evaluateObjectInfo.m_SubType = SvPb::SVEvaluateTranslationYObjectType;
		m_pEvaluateTranslationY = dynamic_cast<SvOp::SVEvaluate*> (pTool->getFirstObject(evaluateObjectInfo));

		//Image Transformation
		SvDef::SVObjectTypeInfoStruct objectInfo;
		objectInfo.m_ObjectType = SvPb::SVTransformObjectType;
		objectInfo.m_SubType = SvPb::SVImageTransformObjectType;

		SvOi::IObjectClass* pImageTransform = pTool->getFirstObject(objectInfo);
		if (nullptr != pImageTransform)
		{
			m_Values.SetTaskID(pImageTransform->getObjectId());
			m_Values.Init();

			const SvDef::NameValueVector& rInterpolationModeList = m_Values.GetEnumTypes(SvPb::OutputInterpolationModeEId);
			m_cbInterpolation.SetEnumTypes(rInterpolationModeList);
			long CurrentSelection = m_Values.Get<long>(SvPb::OutputInterpolationModeEId);
			m_cbInterpolation.SetCurSelItemData(CurrentSelection);
		}
	}

	// Check...
	if( nullptr != m_pEvaluateTranslationX && nullptr != m_pEvaluateTranslationY)
	{
		m_performTranslation = m_Values.Get<bool>(SvPb::PerformTranslationEId);
		UpdateData(false);
		refresh();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zur�ckgeben
}

void TADlgTranslationPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TADlgTranslationPage)
	DDX_Control(pDX, IDC_INTERPOLATION_MODE_COMBO, m_cbInterpolation);
	DDX_Text(pDX, IDC_TRANSLATION_X_EDIT, m_TranslationXValue);
	DDX_Text(pDX, IDC_TRANSLATION_Y_EDIT, m_TranslationYValue);
	DDX_Check(pDX, IDC_PERFORM_TRANSLATION, m_performTranslation);
	//}}AFX_DATA_MAP
}

BOOL TADlgTranslationPage::OnSetActive()
{
	long CurrentSelection = m_Values.Get<long>(SvPb::OutputInterpolationModeEId);
	m_cbInterpolation.SetCurSelItemData(CurrentSelection);

	return CPropertyPage::OnSetActive();
}

void TADlgTranslationPage::OnXFormulaButton() 
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

void TADlgTranslationPage::OnYFormulaButton() 
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

void TADlgTranslationPage::OnPerformTranslation() 
{
	refresh();
}

void TADlgTranslationPage::OnSelChangeInterpolationModeCombo() 
{
	SetInspectionData();
}
#pragma endregion MFC Methods

void TADlgTranslationPage::SetInspectionData()
{
	UpdateData( true ); // get data from dialog

	m_Values.Set<bool>(SvPb::PerformTranslationEId, m_performTranslation ? true : false);

	int CurrentSelection = m_cbInterpolation.GetCurSel();
	if(0 <= CurrentSelection)
	{
		long lValue = static_cast<long> (m_cbInterpolation.GetItemData(CurrentSelection));
		m_Values.Set<long>(SvPb::OutputInterpolationModeEId, lValue);
	}
	 m_Values.Commit();
}

void TADlgTranslationPage::refresh()
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
