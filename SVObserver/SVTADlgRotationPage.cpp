//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgRotaionPage
//* .File Name       : $Workfile:   SVTADlgRotationPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   15 May 2014 13:07:12  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTADlgRotationPage.h"
#include "SvOGui/SVFormulaEditorSheet.h"
#include "InspectionEngine/SVEvaluate.h"
#include "InspectionEngine/SVImageTransform.h"

#pragma endregion

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************

BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogRotationPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVToolAdjustmentDialogRotationPageClass)
	ON_BN_CLICKED(IDC_ROTATION_ANGLE_FORMULA_BUTTON, OnAngleFormulaButton)
	ON_BN_CLICKED(IDC_ROTATION_X_FORMULA_BUTTON, OnXFormulaButton)
	ON_BN_CLICKED(IDC_ROTATION_Y_FORMULA_BUTTON, OnYFormulaButton)
	ON_BN_CLICKED(IDC_PERFORM_ROTATION, OnPerformRotation)
	ON_CBN_SELCHANGE(IDC_INTERPOLATION_MODE_COMBO, OnSelChangeInterpolationModeCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#pragma region Constructor
SVToolAdjustmentDialogRotationPageClass::SVToolAdjustmentDialogRotationPageClass( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID)
: CPropertyPage(SVToolAdjustmentDialogRotationPageClass::IDD)
, m_InspectionID{ rInspectionID }
, m_TaskObjectID{ rTaskObjectID }
{
}
#pragma endregion

SVToolAdjustmentDialogRotationPageClass::~SVToolAdjustmentDialogRotationPageClass()
{
}

#pragma region Protected Methods
#pragma region MFC Methods
void SVToolAdjustmentDialogRotationPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVToolAdjustmentDialogRotationPageClass)
	DDX_Control(pDX, IDC_INTERPOLATION_MODE_COMBO, m_cbInterpolation);
	DDX_Text(pDX, IDC_ROTATION_ANGLE_EDIT, m_RotationAngleValue);
	DDX_Text(pDX, IDC_ROTATION_X_EDIT, m_RotationXValue);
	DDX_Text(pDX, IDC_ROTATION_Y_EDIT, m_RotationYValue);
	DDX_Check(pDX, IDC_PERFORM_ROTATION, m_performRotation);
	//}}AFX_DATA_MAP
}

BOOL SVToolAdjustmentDialogRotationPageClass::OnSetActive()
{
	if (nullptr != m_pValues)
	{
		long CurrentSelection = m_pValues->Get<long>(SVOutputInterpolationModeObjectGuid);
		m_cbInterpolation.SetCurSelItemData(CurrentSelection);
	}

	return CPropertyPage::OnSetActive();
}

/////////////////////////////////////////////////////////////////////////////
// SVToolAdjustmentDialogRotationPageClass message handlers
BOOL SVToolAdjustmentDialogRotationPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// Set the font of the angle static text
	CWnd* pWnd = GetDlgItem( IDC_STATIC_ANGLE );
	
	angleFont.CreatePointFont( 100,"Symbol");
		
	pWnd->SetFont( &angleFont );

	pWnd->SetWindowText("q:");

	CPropertyPage::OnInitDialog();

	SvOi::IObjectClass* pTool = SvOi::getObject(m_TaskObjectID);
	if (nullptr != pTool)
	{
		// Get Evaluate Object...
		SvDef::SVObjectTypeInfoStruct evaluateObjectInfo;
		evaluateObjectInfo.ObjectType = SvDef::SVMathContainerObjectType;

		// Get Evaluate Object for the X coordinate...
		evaluateObjectInfo.SubType = SvDef::SVEvaluateRotationXObjectType;
		m_pEvaluateRotationX = dynamic_cast<SVEvaluateClass*> (pTool->getFirstObject(evaluateObjectInfo));

		// Get Evaluate Object for the Y coordinate...
		evaluateObjectInfo.SubType = SvDef::SVEvaluateRotationYObjectType;
		m_pEvaluateRotationY = dynamic_cast<SVEvaluateClass*> (pTool->getFirstObject(evaluateObjectInfo));

		evaluateObjectInfo.SubType = SvDef::SVEvaluateRotationAngleObjectType;
		m_pEvaluateRotationAngle = dynamic_cast<SVEvaluateClass*>(pTool->getFirstObject(evaluateObjectInfo));

		//Image Transformation
		SvDef::SVObjectTypeInfoStruct objectInfo;
		objectInfo.ObjectType = SvDef::SVTransformObjectType;
		objectInfo.SubType = SvDef::SVImageTransformObjectType;

		SvOi::IObjectClass* pImageTransform = pTool->getFirstObject(objectInfo);
		if (nullptr != pImageTransform)
		{
			m_pValues = std::unique_ptr<Controller>(new Controller{ SvOg::BoundValues{ m_InspectionID, pImageTransform->GetUniqueObjectID() } });
			m_pValues->Init();

			const SvOi::NameValueVector& rInterpolationModeList = m_pValues->GetEnumTypes(SVOutputInterpolationModeObjectGuid);
			m_cbInterpolation.SetEnumTypes(rInterpolationModeList);
			long CurrentSelection = m_pValues->Get<long>(SVOutputInterpolationModeObjectGuid);
			m_cbInterpolation.SetCurSelItemData(CurrentSelection);
		}
	}

	if (nullptr != m_pEvaluateRotationX && nullptr != m_pEvaluateRotationY && nullptr != m_pEvaluateRotationAngle && nullptr != m_pValues)
	{
		m_performRotation = m_pValues->Get<bool>(SVPerformRotationObjectGuid);
		UpdateData(false);
		refresh();
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void SVToolAdjustmentDialogRotationPageClass::OnAngleFormulaButton() 
{
	if(nullptr != m_pEvaluateRotationAngle)
	{
		std::string Caption = m_pEvaluateRotationAngle->GetName();
		Caption += _T( " Formula" );

		SvDef::SVObjectTypeInfoStruct info(SvDef::SVMathContainerObjectType, SvDef::SVEvaluateRotationAngleObjectType);
		SvOg::SVFormulaEditorSheetClass dlg( m_InspectionID, m_TaskObjectID, info, Caption.c_str() );
		dlg.DoModal();

		refresh();
	}
}

void SVToolAdjustmentDialogRotationPageClass::OnXFormulaButton() 
{
	if(nullptr !=  m_pEvaluateRotationX)
	{
		std::string Caption = m_pEvaluateRotationX->GetName();
		Caption += _T( " Formula" );

		SvDef::SVObjectTypeInfoStruct info(SvDef::SVMathContainerObjectType, SvDef::SVEvaluateRotationXObjectType);
		SvOg::SVFormulaEditorSheetClass dlg(m_InspectionID, m_TaskObjectID, info, Caption.c_str() );
		dlg.DoModal();

		refresh();
	}
}

void SVToolAdjustmentDialogRotationPageClass::OnYFormulaButton() 
{
	if(nullptr != m_pEvaluateRotationY)
	{
		std::string Caption = m_pEvaluateRotationY->GetName();
		Caption += _T( " Formula" );

		SvDef::SVObjectTypeInfoStruct info(SvDef::SVMathContainerObjectType, SvDef::SVEvaluateRotationYObjectType);
		SvOg::SVFormulaEditorSheetClass dlg( m_InspectionID, m_TaskObjectID, info, Caption.c_str() );
		dlg.DoModal();

		refresh();
	}
}

void SVToolAdjustmentDialogRotationPageClass::OnPerformRotation() 
{
	refresh();
}

void SVToolAdjustmentDialogRotationPageClass::OnSelChangeInterpolationModeCombo() 
{
	SetInspectionData();
}
#pragma endregion MFC Methods

HRESULT SVToolAdjustmentDialogRotationPageClass::SetInspectionData()
{
	HRESULT Result{ E_FAIL };

	if(nullptr != m_pValues)
	{
		UpdateData(true); // get data from dialog

		m_pValues->Set<bool>(SVPerformRotationObjectGuid, m_performRotation ? true : false);

		int CurrentSelection = m_cbInterpolation.GetCurSel();
		if(0 <= CurrentSelection)
		{
			long lValue = static_cast<long> (m_cbInterpolation.GetItemData(CurrentSelection));
			m_pValues->Set<long>(SVOutputInterpolationModeObjectGuid, lValue);
		}

		Result = m_pValues->Commit();
	}

	return Result;
}

void SVToolAdjustmentDialogRotationPageClass::refresh()
{
	SetInspectionData();

	if (nullptr != m_pEvaluateRotationX && nullptr != m_pEvaluateRotationX->getOutputMathResult())
	{
		double Value{ 0.0 };
		m_pEvaluateRotationX->getOutputMathResult()->GetValue(Value);
		m_RotationXValue.Format(_T("%f"), Value);
	}
	if (nullptr != m_pEvaluateRotationY && nullptr != m_pEvaluateRotationY->getOutputMathResult())
	{
		double Value{ 0.0 };
		m_pEvaluateRotationY->getOutputMathResult()->GetValue(Value);
		m_RotationYValue.Format(_T("%f"), Value);
	}
	if (nullptr != m_pEvaluateRotationAngle && nullptr != m_pEvaluateRotationAngle->getOutputMathResult())
	{
		double Value{ 0.0 };
		m_pEvaluateRotationAngle->getOutputMathResult()->GetValue(Value);
		m_RotationAngleValue.Format(_T("%f"), Value);
	}

	UpdateData(false); // set data to dialog
}
#pragma endregion

