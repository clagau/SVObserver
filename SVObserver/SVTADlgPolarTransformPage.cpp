//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgPolarTransformPage
//* .File Name       : $Workfile:   SVTADlgPolarTransformPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:50:32  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTADlgPolarTransformPage.h"
#include "SvOGui/SVFormulaEditorSheet.h"
#include "Operators/SVEvaluate.h"
#include "Operators/SVImagePolarTransform.h"
#include "SVIPDoc.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVOGui/ValuesAccessor.h"
#include "SVOGui/DataController.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVToolAdjustmentDialogPolarTransformPageClass::SVToolAdjustmentDialogPolarTransformPageClass( uint32_t inspectionId, uint32_t taskObjectId, SVToolAdjustmentDialogSheetClass* Parent )
: CPropertyPage(SVToolAdjustmentDialogPolarTransformPageClass::IDD)
{
	//{{AFX_DATA_INIT(SVToolAdjustmentDialogPolarTransformPageClass)
	m_CenterXValue = _T("");
	m_CenterYValue = _T("");
	m_StartRadiusValue = _T("");
	m_StrEndRadiusValue = _T("");
	m_StartAngleValue = _T("");
	m_EndAngleValue = _T("");
	bUseFormula = FALSE;
	//}}AFX_DATA_INIT

	m_pParentDialog	= Parent;
	m_pTaskObject			= nullptr;

	m_pEvaluateCenterX	 = nullptr;
	m_pCenterXResult		 = nullptr;
	m_pEvaluateCenterY	 = nullptr;
	m_pCenterYResult		 = nullptr;
	m_pEvaluateStartRadius = nullptr;
	m_pStartRadiusResult	 = nullptr;
	m_pEvaluateEndRadius	 = nullptr;
	m_pEndRadiusResult	 = nullptr;
	m_pEvaluateStartAngle	 = nullptr;
	m_pStartAngleResult	 = nullptr;
	m_pEvaluateEndAngle	 = nullptr;
	m_pEndAngleResult		 = nullptr;

	m_pUseFormula			 = nullptr;
	m_pInterpolationMode	 = nullptr;
}

SVToolAdjustmentDialogPolarTransformPageClass::~SVToolAdjustmentDialogPolarTransformPageClass()
{
}

void SVToolAdjustmentDialogPolarTransformPageClass::SetInspectionData()
{
	if( nullptr != m_pTaskObject && nullptr != m_pParentDialog )
	{
		UpdateData( true ); // get data from dialog

		//@TODO[gra][8.00][15.01.2018]: The data controller should be used like the rest of SVOGui
		typedef SvOg::DataController<SvOg::ValuesAccessor, SvOg::ValuesAccessor::value_type> Controller;
		Controller PolarTransformValues{ SvOg::BoundValues{m_pParentDialog->GetInspectionID(), m_ImagePolarTransformID} };
		PolarTransformValues.Init();

		PolarTransformValues.Set<bool>(m_pUseFormula->GetEmbeddedID(), bUseFormula ? true : false);

		int sel = interpolationComboCtrl.GetCurSel();
		if (0 <= sel)
		{
			long lValue = static_cast<long> (interpolationComboCtrl.GetItemData( sel ));
			PolarTransformValues.Set<long>(m_pInterpolationMode->GetEmbeddedID(), lValue);
		}
		PolarTransformValues.Commit();

		sel = m_AngularMethodCombo.GetCurSel();
		if (0 <= sel)
		{
			//@TODO[gra][8.00][15.01.2018]: The data controller should be used like the rest of SVOGui
			Controller Values {SvOg::BoundValues {m_pParentDialog->GetInspectionID(), m_pTaskObject->getObjectId()}};
			Values.Init();

			long lValue = static_cast<long>  (m_AngularMethodCombo.GetItemData(sel));
			Values.Set<long>(m_pAngleMethod->GetEmbeddedID(), lValue);
			Values.Commit();
		}
	}
}


void SVToolAdjustmentDialogPolarTransformPageClass::refresh()
{
	if( m_pTaskObject )
	{
		CWnd* pWnd = nullptr;

		SetInspectionData();

		if( nullptr != m_pUseFormula )
		{
			// refresh Use Formula settings...
			m_pUseFormula->GetValue( bUseFormula );

			// Update Formula Button activation states...
			if( pWnd = GetDlgItem( IDC_CENTER_X_FORMULA_BUTTON ) )
				pWnd->EnableWindow( bUseFormula );
			if( pWnd = GetDlgItem( IDC_CENTER_Y_FORMULA_BUTTON ) )
				pWnd->EnableWindow( bUseFormula );
			if( pWnd = GetDlgItem( IDC_START_RADIUS_FORMULA_BUTTON ) )
				pWnd->EnableWindow( bUseFormula );
			if( pWnd = GetDlgItem( IDC_END_RADIUS_FORMULA_BUTTON ) )
				pWnd->EnableWindow( bUseFormula );
			if( pWnd = GetDlgItem( IDC_START_ANGLE_FORMULA_BUTTON ) )
				pWnd->EnableWindow( bUseFormula );
			if( pWnd = GetDlgItem( IDC_END_ANGLE_FORMULA_BUTTON ) )
				pWnd->EnableWindow( bUseFormula );

		}
		if( nullptr != m_pAngleMethod )
		{
			std::string EnumString;
			m_pAngleMethod->getValue( EnumString );
			m_AngularMethodCombo.SelectString( -1, EnumString.c_str() );
		}

		// refresh interpolation mode combo settings...
		if( nullptr != m_pInterpolationMode )
		{
			std::string EnumString;
			if( S_OK == m_pInterpolationMode->getValue( EnumString ) )
			{
				// Set cur sel in combo box...
				interpolationComboCtrl.SelectString( -1, EnumString.c_str() );
			}
		}

		std::string Value;
		// refresh CenterX settings...
		if( nullptr != m_pCenterXResult )
		{
			m_pCenterXResult->getValue( Value );
			m_CenterXValue = Value.c_str();
		}
		else
		{
			m_CenterXValue = _T( "" );
		}

		// refresh CenterY settings...
		if( nullptr != m_pCenterYResult )
		{
			m_pCenterYResult->getValue( Value );
			m_CenterYValue = Value.c_str();
		}
		else
		{
			m_CenterYValue = _T( "" );
		}
		
		// refresh StartRadius settings...
		if( nullptr != m_pStartRadiusResult )
		{
			m_pStartRadiusResult->getValue( Value );
			m_StartRadiusValue = Value.c_str();
		}
		else
		{
			m_StartRadiusValue = _T( "" );
		}

		// refresh EndRadius settings...
		if( nullptr != m_pEndRadiusResult )
		{
			m_pEndRadiusResult->getValue( Value );
			m_StrEndRadiusValue = Value.c_str();
		}
		else
		{
			m_StrEndRadiusValue = _T( "" );
		}

		// refresh StartAngle settings...
		if( nullptr != m_pStartAngleResult )
		{
			m_pStartAngleResult->getValue( Value );
			m_StartAngleValue = Value.c_str();
		}
		else
		{
			m_StartAngleValue = _T( "" );
		}

		// refresh EndAngle settings...
		if( nullptr != m_pEndAngleResult )
		{
			m_pEndAngleResult->getValue( Value );
			m_EndAngleValue = Value.c_str();
		}
		else
		{
			m_EndAngleValue = _T( "" );
		}

		UpdateData( FALSE ); // set data to dialog
	}

}

void SVToolAdjustmentDialogPolarTransformPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVToolAdjustmentDialogPolarTransformPageClass)
	DDX_Control(pDX, IDC_ANGULAR_METHOD, m_AngularMethodCombo);
	DDX_Control(pDX, IDC_INTERPOLATION_MODE_COMBO, interpolationComboCtrl);
	DDX_Text(pDX, IDC_CENTER_X_EDIT, m_CenterXValue);
	DDX_Text(pDX, IDC_CENTER_Y_EDIT, m_CenterYValue);
	DDX_Text(pDX, IDC_START_RADIUS_EDIT, m_StartRadiusValue);
	DDX_Text(pDX, IDC_END_RADIUS_EDIT, m_StrEndRadiusValue);
	DDX_Text(pDX, IDC_START_ANGLE_EDIT, m_StartAngleValue);
	DDX_Text(pDX, IDC_END_ANGLE_EDIT, m_EndAngleValue);
	DDX_Check(pDX, IDC_USE_FORMULA_CHECK, bUseFormula);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogPolarTransformPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVToolAdjustmentDialogPolarTransformPageClass)
	ON_BN_CLICKED(IDC_CENTER_X_FORMULA_BUTTON, OnCenterXFormulaButton)
	ON_BN_CLICKED(IDC_CENTER_Y_FORMULA_BUTTON, OnCenterYFormulaButton)
	ON_BN_CLICKED(IDC_START_RADIUS_FORMULA_BUTTON, OnStartRadiusFormulaButton)
	ON_BN_CLICKED(IDC_END_RADIUS_FORMULA_BUTTON, OnEndRadiusFormulaButton)
	ON_BN_CLICKED(IDC_START_ANGLE_FORMULA_BUTTON, OnStartAngleFormulaButton)
	ON_BN_CLICKED(IDC_END_ANGLE_FORMULA_BUTTON, OnEndAngleFormulaButton)
	ON_BN_CLICKED(IDC_USE_FORMULA_CHECK, OnUseFormulaCheck)
	ON_CBN_SELCHANGE(IDC_INTERPOLATION_MODE_COMBO, OnSelchangeInterpolationModeCombo)
	ON_CBN_SELCHANGE(IDC_ANGULAR_METHOD, OnSelchangeAngularMethod)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVToolAdjustmentDialogPolarTransformPageClass message handlers

BOOL SVToolAdjustmentDialogPolarTransformPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	if( m_pParentDialog && ( m_pTaskObject = m_pParentDialog->GetTaskObject() ) )
	{
		// Get Evaluate Object...
		SvDef::SVObjectTypeInfoStruct evaluateObjectInfo;
		evaluateObjectInfo.m_ObjectType = SvPb::SVMathContainerObjectType;

		// Set up the requestor for the Evaluate result
		SvDef::SVObjectTypeInfoStruct resultObjectInfo;
		resultObjectInfo.m_ObjectType = SvPb::SVValueObjectType;
		resultObjectInfo.m_SubType = SvPb::SVDoubleValueObjectType;

		// Get Evaluate Center X...
		evaluateObjectInfo.m_SubType = SvPb::SVEvaluateCenterXObjectType;
		m_pEvaluateCenterX = dynamic_cast<SvOp::SVEvaluateClass*>(m_pTaskObject->getFirstObject(evaluateObjectInfo));
		if( m_pEvaluateCenterX )
		{
			// Get Center X Result...
			resultObjectInfo.m_EmbeddedID = SvPb::OutputEvaluateCenterXResultEId;
			m_pCenterXResult = dynamic_cast<SvVol::SVDoubleValueObjectClass*>(m_pEvaluateCenterX->getFirstObject(resultObjectInfo));
		}

		// Get Evaluate Center Y...
		evaluateObjectInfo.m_SubType = SvPb::SVEvaluateCenterYObjectType;
		m_pEvaluateCenterY = dynamic_cast<SvOp::SVEvaluateClass*>(m_pTaskObject->getFirstObject(evaluateObjectInfo));
		if( m_pEvaluateCenterY )
		{
			// Get Center Y Result...
			resultObjectInfo.m_EmbeddedID = SvPb::OutputEvaluateCenterYResultEId;
			m_pCenterYResult = dynamic_cast<SvVol::SVDoubleValueObjectClass*>(m_pEvaluateCenterY->getFirstObject(resultObjectInfo));
		}

		// Get Evaluate Start Radius...
		evaluateObjectInfo.m_SubType = SvPb::SVEvaluateStartRadiusObjectType;
		m_pEvaluateStartRadius = dynamic_cast<SvOp::SVEvaluateClass*>(m_pTaskObject->getFirstObject(evaluateObjectInfo));
		if( m_pEvaluateStartRadius )
		{
			// Get Start Radius Result...
			resultObjectInfo.m_EmbeddedID = SvPb::OutputEvaluateStartRadiusResultEId;
			m_pStartRadiusResult = dynamic_cast<SvVol::SVDoubleValueObjectClass*>(m_pEvaluateStartRadius->getFirstObject(resultObjectInfo));
		}

		// Get Evaluate End Radius...
		evaluateObjectInfo.m_SubType = SvPb::SVEvaluateEndRadiusObjectType;
		m_pEvaluateEndRadius = dynamic_cast<SvOp::SVEvaluateClass*>(m_pTaskObject->getFirstObject(evaluateObjectInfo));
		if( m_pEvaluateEndRadius )
		{
			// Get End Radius Result...
			resultObjectInfo.m_EmbeddedID = SvPb::OutputEvaluateEndRadiusResultEId;
			m_pEndRadiusResult = dynamic_cast<SvVol::SVDoubleValueObjectClass*>(m_pEvaluateEndRadius->getFirstObject(resultObjectInfo));
		}

		// Get Evaluate Start Angle...
		evaluateObjectInfo.m_SubType = SvPb::SVEvaluateStartAngleObjectType;
		m_pEvaluateStartAngle = dynamic_cast<SvOp::SVEvaluateClass*>(m_pTaskObject->getFirstObject(evaluateObjectInfo));
		if( m_pEvaluateStartAngle )
		{
			// Get Start Angle Result...
			resultObjectInfo.m_EmbeddedID = SvPb::OutputEvaluateStartAngleResultEId;
			m_pStartAngleResult = dynamic_cast<SvVol::SVDoubleValueObjectClass*>(m_pEvaluateStartAngle->getFirstObject(resultObjectInfo));
		}

		// Get Evaluate End Angle...
		evaluateObjectInfo.m_SubType = SvPb::SVEvaluateEndAngleObjectType;
		m_pEvaluateEndAngle = dynamic_cast<SvOp::SVEvaluateClass*>(m_pTaskObject->getFirstObject(evaluateObjectInfo));
		if( m_pEvaluateEndAngle )
		{
			// Get End Angle Result...
			resultObjectInfo.m_EmbeddedID = SvPb::OutputEvaluateEndAngleResultEId;
			m_pEndAngleResult = dynamic_cast<SvVol::SVDoubleValueObjectClass*>(m_pEvaluateEndAngle->getFirstObject(resultObjectInfo));
		}

		// Get Use Formula...
		SvDef::SVObjectTypeInfoStruct objectInfo;
		objectInfo.m_EmbeddedID = SvPb::OutputUseFormulaEId;
		m_pUseFormula = dynamic_cast<SvVol::SVBoolValueObjectClass*>(m_pTaskObject->getFirstObject(objectInfo));
		if( m_pUseFormula )
		{
			m_pUseFormula->GetValue( bUseFormula );
		}

		// Get Use Angle Method ...
		objectInfo.m_EmbeddedID = SvPb::OutputAngularMethodEId;
		m_pAngleMethod = dynamic_cast<SvVol::SVEnumerateValueObjectClass*>(m_pTaskObject->getFirstObject(objectInfo));
		if( m_pAngleMethod )
		{
			m_AngularMethodCombo.SetEnumTypes(m_pAngleMethod->GetEnumVector());
		}

		// Get Interpolation Mode...
		objectInfo.m_EmbeddedID = SvPb::OutputInterpolationModeEId;
		m_pInterpolationMode = dynamic_cast<SvVol::SVEnumerateValueObjectClass*>(m_pTaskObject->getFirstObject(objectInfo));
		if( m_pInterpolationMode )
		{
			// Populate Interpolation Mode combo...
			interpolationComboCtrl.SetEnumTypes(m_pInterpolationMode->GetEnumVector());
		}

		objectInfo.m_ObjectType = SvPb::SVPolarTransformObjectType;
		objectInfo.m_SubType = SvPb::SVImagePolarTransformObjectType;
		objectInfo.m_EmbeddedID = SvPb::NoEmbeddedId;
		SvOp::SVImagePolarTransformClass* pImagePolarTransform = dynamic_cast<SvOp::SVImagePolarTransformClass*> (m_pTaskObject->getFirstObject(objectInfo));
		if(nullptr != pImagePolarTransform)
		{
			m_ImagePolarTransformID = pImagePolarTransform->getObjectId();
		}


		UpdateData( FALSE );
	
		// Check...
		if( m_pEvaluateCenterX &&	m_pCenterXResult && m_pEvaluateCenterY && m_pCenterYResult &&
			m_pEvaluateStartRadius && m_pStartRadiusResult && m_pEvaluateEndRadius && m_pEndRadiusResult &&
			m_pEvaluateStartAngle && m_pStartAngleResult &&	m_pEvaluateEndAngle && m_pEndAngleResult &&
			m_pUseFormula && m_pInterpolationMode
		  )
		{
			refresh();

			return TRUE;
		}
	}

	// Not valid call...
	if( GetParent() )
		GetParent()->SendMessage( WM_CLOSE );
	else
		SendMessage( WM_CLOSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben

}

void SVToolAdjustmentDialogPolarTransformPageClass::OnCenterXFormulaButton() 
{
	if( m_pEvaluateCenterX )
	{
		std::string Text = SvUl::LoadStdString( IDS_FORMULA_STRING );
		std::string Caption = m_pEvaluateCenterX->GetName();
		Caption += _T( " " ) +Text;

		uint32_t inspectionID = m_pParentDialog->GetInspectionID();
		uint32_t objectID = m_pParentDialog->GetTaskObjectID();
		SvDef::SVObjectTypeInfoStruct info(SvPb::SVMathContainerObjectType, SvPb::SVEvaluateCenterXObjectType);
		SvOg::SVFormulaEditorSheetClass dlg( inspectionID, objectID, info, Caption.c_str() );
		dlg.DoModal();

		refresh();
	}
}

void SVToolAdjustmentDialogPolarTransformPageClass::OnCenterYFormulaButton() 
{
	if( m_pEvaluateCenterY )
	{
		std::string Text = SvUl::LoadStdString( IDS_FORMULA_STRING );
		std::string Caption = m_pEvaluateCenterY->GetName();
		Caption += _T( " " ) +Text;

		uint32_t inspectionID = m_pParentDialog->GetInspectionID();
		uint32_t objectID = m_pParentDialog->GetTaskObjectID();
		SvDef::SVObjectTypeInfoStruct info(SvPb::SVMathContainerObjectType, SvPb::SVEvaluateCenterYObjectType);
		SvOg::SVFormulaEditorSheetClass dlg( inspectionID, objectID, info, Caption.c_str() );
		dlg.DoModal();

		refresh();
	}
}

void SVToolAdjustmentDialogPolarTransformPageClass::OnStartRadiusFormulaButton() 
{
	if( m_pEvaluateStartRadius )
	{
		std::string Text = SvUl::LoadStdString( IDS_FORMULA_STRING );
		std::string Caption = m_pEvaluateStartRadius->GetName();
		Caption += _T( " " ) +Text;

		uint32_t inspectionID = m_pParentDialog->GetInspectionID();
		uint32_t objectID = m_pParentDialog->GetTaskObjectID();
		SvDef::SVObjectTypeInfoStruct info(SvPb::SVMathContainerObjectType, SvPb::SVEvaluateStartRadiusObjectType);
		SvOg::SVFormulaEditorSheetClass dlg( inspectionID, objectID, info, Caption.c_str() );
		dlg.DoModal();

		refresh();
	}
}

void SVToolAdjustmentDialogPolarTransformPageClass::OnEndRadiusFormulaButton() 
{
	if( m_pEvaluateEndRadius )
	{
		std::string Text = SvUl::LoadStdString( IDS_FORMULA_STRING );
		std::string Caption = m_pEvaluateEndRadius->GetName();
		Caption += _T( " " ) +Text;

		uint32_t inspectionID = m_pParentDialog->GetInspectionID();
		uint32_t objectID = m_pParentDialog->GetTaskObjectID();
		SvDef::SVObjectTypeInfoStruct info(SvPb::SVMathContainerObjectType, SvPb::SVEvaluateEndRadiusObjectType);
		SvOg::SVFormulaEditorSheetClass dlg( inspectionID, objectID, info, Caption.c_str() );
		dlg.DoModal();

		refresh();
	}
}

void SVToolAdjustmentDialogPolarTransformPageClass::OnStartAngleFormulaButton() 
{
	if( m_pEvaluateStartAngle )
	{
		std::string Text = SvUl::LoadStdString( IDS_FORMULA_STRING );
		std::string Caption = m_pEvaluateStartAngle->GetName();
		Caption += _T( " " ) +Text;

		uint32_t inspectionID = m_pParentDialog->GetInspectionID();
		uint32_t objectID = m_pParentDialog->GetTaskObjectID();
		SvDef::SVObjectTypeInfoStruct info(SvPb::SVMathContainerObjectType, SvPb::SVEvaluateStartAngleObjectType);
		SvOg::SVFormulaEditorSheetClass dlg( inspectionID, objectID, info, Caption.c_str() );
		dlg.DoModal();

		refresh();
	}
}

void SVToolAdjustmentDialogPolarTransformPageClass::OnEndAngleFormulaButton() 
{
	if( m_pEvaluateEndAngle )
	{
		std::string Text = SvUl::LoadStdString( IDS_FORMULA_STRING );
		std::string Caption = m_pEvaluateEndAngle->GetName();
		Caption += _T( " " ) +Text;

		uint32_t inspectionID = m_pParentDialog->GetInspectionID();
		uint32_t objectID = m_pParentDialog->GetTaskObjectID();
		SvDef::SVObjectTypeInfoStruct info(SvPb::SVMathContainerObjectType, SvPb::SVEvaluateEndAngleObjectType);
		SvOg::SVFormulaEditorSheetClass dlg( inspectionID, objectID, info, Caption.c_str() );
		dlg.DoModal();

		refresh();
	}
}

void SVToolAdjustmentDialogPolarTransformPageClass::OnUseFormulaCheck() 
{
	refresh();
}

void SVToolAdjustmentDialogPolarTransformPageClass::OnSelchangeInterpolationModeCombo() 
{
	refresh();
}


void SVToolAdjustmentDialogPolarTransformPageClass::OnSelchangeAngularMethod() 
{
	refresh();
}

