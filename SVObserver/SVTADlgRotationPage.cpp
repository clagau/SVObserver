//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "Operators/SVEvaluate.h"
#pragma endregion

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************

BEGIN_MESSAGE_MAP(SVTADlgRotationPage, CPropertyPage)
	//{{AFX_MSG_MAP(SVTADlgRotationPage)
	ON_BN_CLICKED(IDC_ROTATION_ANGLE_FORMULA_BUTTON, OnAngleFormulaButton)
	ON_BN_CLICKED(IDC_ROTATION_X_FORMULA_BUTTON, OnXFormulaButton)
	ON_BN_CLICKED(IDC_ROTATION_Y_FORMULA_BUTTON, OnYFormulaButton)
	ON_BN_CLICKED(IDC_PERFORM_ROTATION, OnPerformRotation)
	ON_CBN_SELCHANGE(IDC_INTERPOLATION_MODE_COMBO, OnSelChangeInterpolationModeCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#pragma region Constructor
SVTADlgRotationPage::SVTADlgRotationPage( uint32_t inspectionId, uint32_t taskObjectId)
: CPropertyPage(SVTADlgRotationPage::IDD)
, m_InspectionID{ inspectionId }
, m_TaskObjectID{ taskObjectId }
//TaskID is set later
, m_Values{ SvOgu::BoundValues{inspectionId, SvDef::InvalidObjectId} }
{
}
#pragma endregion

SVTADlgRotationPage::~SVTADlgRotationPage()
{
}

#pragma region Protected Methods
#pragma region MFC Methods
void SVTADlgRotationPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVTADlgRotationPage)
	DDX_Control(pDX, IDC_INTERPOLATION_MODE_COMBO, m_cbInterpolation);
	DDX_Text(pDX, IDC_ROTATION_ANGLE_EDIT, m_RotationAngleValue);
	DDX_Text(pDX, IDC_ROTATION_X_EDIT, m_RotationXValue);
	DDX_Text(pDX, IDC_ROTATION_Y_EDIT, m_RotationYValue);
	DDX_Check(pDX, IDC_PERFORM_ROTATION, m_performRotation);
	//}}AFX_DATA_MAP
}

BOOL SVTADlgRotationPage::OnSetActive()
{
	long CurrentSelection = m_Values.Get<long>(SvPb::OutputInterpolationModeEId);
	m_cbInterpolation.SetCurSelItemData(CurrentSelection);

	return CPropertyPage::OnSetActive();
}

/////////////////////////////////////////////////////////////////////////////
// SVTADlgRotationPage message handlers
BOOL SVTADlgRotationPage::OnInitDialog() 
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
		evaluateObjectInfo.m_ObjectType = SvPb::SVMathContainerObjectType;

		// Get Evaluate Object for the X coordinate...
		evaluateObjectInfo.m_SubType = SvPb::SVEvaluateRotationXObjectType;
		m_pEvaluateRotationX = dynamic_cast<SvOp::SVEvaluate*> (pTool->getFirstObject(evaluateObjectInfo));

		// Get Evaluate Object for the Y coordinate...
		evaluateObjectInfo.m_SubType = SvPb::SVEvaluateRotationYObjectType;
		m_pEvaluateRotationY = dynamic_cast<SvOp::SVEvaluate*> (pTool->getFirstObject(evaluateObjectInfo));

		evaluateObjectInfo.m_SubType = SvPb::SVEvaluateRotationAngleObjectType;
		m_pEvaluateRotationAngle = dynamic_cast<SvOp::SVEvaluate*>(pTool->getFirstObject(evaluateObjectInfo));

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

	if (nullptr != m_pEvaluateRotationX && nullptr != m_pEvaluateRotationY && nullptr != m_pEvaluateRotationAngle)
	{
		m_performRotation = m_Values.Get<bool>(SvPb::PerformRotationEId);
		UpdateData(false);
		refresh();
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void SVTADlgRotationPage::OnAngleFormulaButton() 
{
	if(nullptr != m_pEvaluateRotationAngle)
	{
		std::string Caption = m_pEvaluateRotationAngle->GetName();
		Caption += _T( " Formula" );

		SvDef::SVObjectTypeInfoStruct info(SvPb::SVMathContainerObjectType, SvPb::SVEvaluateRotationAngleObjectType);
		SvOg::SVFormulaEditorSheetClass dlg( m_InspectionID, m_TaskObjectID, info, Caption.c_str() );
		dlg.DoModal();

		refresh();
	}
}

void SVTADlgRotationPage::OnXFormulaButton() 
{
	if(nullptr !=  m_pEvaluateRotationX)
	{
		std::string Caption = m_pEvaluateRotationX->GetName();
		Caption += _T( " Formula" );

		SvDef::SVObjectTypeInfoStruct info(SvPb::SVMathContainerObjectType, SvPb::SVEvaluateRotationXObjectType);
		SvOg::SVFormulaEditorSheetClass dlg(m_InspectionID, m_TaskObjectID, info, Caption.c_str() );
		dlg.DoModal();

		refresh();
	}
}

void SVTADlgRotationPage::OnYFormulaButton() 
{
	if(nullptr != m_pEvaluateRotationY)
	{
		std::string Caption = m_pEvaluateRotationY->GetName();
		Caption += _T( " Formula" );

		SvDef::SVObjectTypeInfoStruct info(SvPb::SVMathContainerObjectType, SvPb::SVEvaluateRotationYObjectType);
		SvOg::SVFormulaEditorSheetClass dlg( m_InspectionID, m_TaskObjectID, info, Caption.c_str() );
		dlg.DoModal();

		refresh();
	}
}

void SVTADlgRotationPage::OnPerformRotation() 
{
	refresh();
}

void SVTADlgRotationPage::OnSelChangeInterpolationModeCombo() 
{
	SetInspectionData();
}
#pragma endregion MFC Methods

void SVTADlgRotationPage::SetInspectionData()
{
	UpdateData(true); // get data from dialog

	m_Values.Set<bool>(SvPb::PerformRotationEId, m_performRotation ? true : false);

	int CurrentSelection = m_cbInterpolation.GetCurSel();
	if(0 <= CurrentSelection)
	{
		long lValue = static_cast<long> (m_cbInterpolation.GetItemData(CurrentSelection));
		m_Values.Set<long>(SvPb::OutputInterpolationModeEId, lValue);
	}

	m_Values.Commit();
}

void SVTADlgRotationPage::refresh()
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

