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
#include "SVEvaluate.h"
#include "SVIPDoc.h"
#include "SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVToolAdjustmentDialogPolarTransformPageClass::SVToolAdjustmentDialogPolarTransformPageClass( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* Parent )
: CPropertyPage(SVToolAdjustmentDialogPolarTransformPageClass::IDD)
{
	//{{AFX_DATA_INIT(SVToolAdjustmentDialogPolarTransformPageClass)
	StrCenterXValue = _T("");
	StrCenterYValue = _T("");
	StrStartRadiusValue = _T("");
	StrEndRadiusValue = _T("");
	StrStartAngleValue = _T("");
	StrEndAngleValue = _T("");
	bUseFormula = FALSE;
	//}}AFX_DATA_INIT

	pParentDialog	= Parent;
	pTool			= nullptr;

	pEvaluateCenterX	 = nullptr;
	pCenterXResult		 = nullptr;
	pEvaluateCenterY	 = nullptr;
	pCenterYResult		 = nullptr;
	pEvaluateStartRadius = nullptr;
	pStartRadiusResult	 = nullptr;
	pEvaluateEndRadius	 = nullptr;
	pEndRadiusResult	 = nullptr;
	pEvaluateStartAngle	 = nullptr;
	pStartAngleResult	 = nullptr;
	pEvaluateEndAngle	 = nullptr;
	pEndAngleResult		 = nullptr;

	pUseFormula			 = nullptr;
	pInterpolationMode	 = nullptr;
}

SVToolAdjustmentDialogPolarTransformPageClass::~SVToolAdjustmentDialogPolarTransformPageClass()
{
}

HRESULT SVToolAdjustmentDialogPolarTransformPageClass::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	if( pTool )
	{
		UpdateData( TRUE ); // get data from dialog

		l_hrOk = AddInputRequest( pUseFormula, bUseFormula );

		if( S_OK == l_hrOk )
		{
			int sel = interpolationComboCtrl.GetCurSel();
			if( sel >= 0 )
			{
				long lValue = ( long ) interpolationComboCtrl.GetItemData( sel );

				l_hrOk = AddInputRequest( pInterpolationMode, lValue );
			}
		}

		if( S_OK == l_hrOk )
		{
			int sel = m_AngularMethodCombo.GetCurSel();
			if( sel >= 0 )
			{
				long lValue = ( long ) m_AngularMethodCombo.GetItemData( sel );

				l_hrOk = AddInputRequest( m_pAngleMethod, lValue );
			}
		}

		if( S_OK == l_hrOk )
		{
			l_hrOk = AddInputRequestMarker();
		}

		if( S_OK == l_hrOk )
		{
			l_hrOk = RunOnce( pTool->GetUniqueObjectID() );
		}

		UpdateData( FALSE );
	}

	return l_hrOk;
}


void SVToolAdjustmentDialogPolarTransformPageClass::refresh()
{
	if( pTool )
	{
		CWnd* pWnd = nullptr;

		SetInspectionData();

		if( pUseFormula )
		{
			// refresh Use Formula settings...
			pUseFormula->GetValue( bUseFormula );

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
		if( m_pAngleMethod )
		{
			CString strEnum;
			m_pAngleMethod->GetValue( strEnum );
			m_AngularMethodCombo.SelectString( -1, strEnum );
		}

		// refresh interpolation mode combo settings...
		if( pInterpolationMode )
		{
			CString strEnum;
			if( S_OK == pInterpolationMode->GetValue( strEnum ) )
			{
				// Set cur sel in combo box...
				interpolationComboCtrl.SelectString( -1, strEnum );
			}
		}

		// refresh CenterX settings...
		if( pCenterXResult )
		{
			pCenterXResult->GetValue( StrCenterXValue );
		}
		else
		{
			StrCenterXValue = _T( "" );
		}

		// refresh CenterY settings...
		if( pCenterYResult )
		{
			pCenterYResult->GetValue( StrCenterYValue );
		}
		else
		{
			StrCenterYValue = _T( "" );
		}
		
		// refresh StartRadius settings...
		if( pStartRadiusResult )
		{
			pStartRadiusResult->GetValue( StrStartRadiusValue );
		}
		else
		{
			StrStartRadiusValue = _T( "" );
		}

		// refresh EndRadius settings...
		if( pEndRadiusResult )
		{
			pEndRadiusResult->GetValue( StrEndRadiusValue );
		}
		else
		{
			StrEndRadiusValue = _T( "" );
		}

		// refresh StartAngle settings...
		if( pStartAngleResult )
		{
			pStartAngleResult->GetValue( StrStartAngleValue );
		}
		else
		{
			StrStartAngleValue = _T( "" );
		}

		// refresh EndAngle settings...
		if( pEndAngleResult )
		{
			pEndAngleResult->GetValue( StrEndAngleValue );
		}
		else
		{
			StrEndAngleValue = _T( "" );
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
	DDX_Text(pDX, IDC_CENTER_X_EDIT, StrCenterXValue);
	DDX_Text(pDX, IDC_CENTER_Y_EDIT, StrCenterYValue);
	DDX_Text(pDX, IDC_START_RADIUS_EDIT, StrStartRadiusValue);
	DDX_Text(pDX, IDC_END_RADIUS_EDIT, StrEndRadiusValue);
	DDX_Text(pDX, IDC_START_ANGLE_EDIT, StrStartAngleValue);
	DDX_Text(pDX, IDC_END_ANGLE_EDIT, StrEndAngleValue);
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

	if( pParentDialog && ( pTool = pParentDialog->GetTool() ) )
	{
		SetTaskObject( pTool );

		// Get Evaluate Object...
		SVObjectTypeInfoStruct evaluateObjectInfo;
		evaluateObjectInfo.ObjectType = SVMathContainerObjectType;

		// Set up the requestor for the Evaluate result
		SVObjectTypeInfoStruct resultObjectInfo;
		resultObjectInfo.ObjectType = SVDoubleValueObjectType;

		// Get Evaluate Center X...
		evaluateObjectInfo.SubType = SVEvaluateCenterXObjectType;		
		pEvaluateCenterX = dynamic_cast<SVEvaluateClass*>(pTool->getFirstObject(evaluateObjectInfo));
		if( pEvaluateCenterX )
		{
			// Get Center X Result...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateCenterXResultObjectGuid;
			pCenterXResult = dynamic_cast<SVDoubleValueObjectClass*>(pEvaluateCenterX->getFirstObject(resultObjectInfo));
		}

		// Get Evaluate Center Y...
		evaluateObjectInfo.SubType = SVEvaluateCenterYObjectType;		
		pEvaluateCenterY = dynamic_cast<SVEvaluateClass*>(pTool->getFirstObject(evaluateObjectInfo));
		if( pEvaluateCenterY )
		{
			// Get Center Y Result...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateCenterYResultObjectGuid;
			pCenterYResult = dynamic_cast<SVDoubleValueObjectClass*>(pEvaluateCenterY->getFirstObject(resultObjectInfo));
		}

		// Get Evaluate Start Radius...
		evaluateObjectInfo.SubType = SVEvaluateStartRadiusObjectType;		
		pEvaluateStartRadius = dynamic_cast<SVEvaluateClass*>(pTool->getFirstObject(evaluateObjectInfo));
		if( pEvaluateStartRadius )
		{
			// Get Start Radius Result...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateStartRadiusResultObjectGuid;
			pStartRadiusResult = dynamic_cast<SVDoubleValueObjectClass*>(pEvaluateStartRadius->getFirstObject(resultObjectInfo));
		}

		// Get Evaluate End Radius...
		evaluateObjectInfo.SubType = SVEvaluateEndRadiusObjectType;		
		pEvaluateEndRadius = dynamic_cast<SVEvaluateClass*>(pTool->getFirstObject(evaluateObjectInfo));
		if( pEvaluateEndRadius )
		{
			// Get End Radius Result...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateEndRadiusResultObjectGuid;
			pEndRadiusResult = dynamic_cast<SVDoubleValueObjectClass*>(pEvaluateEndRadius->getFirstObject(resultObjectInfo));
		}

		// Get Evaluate Start Angle...
		evaluateObjectInfo.SubType = SVEvaluateStartAngleObjectType;		
		pEvaluateStartAngle = dynamic_cast<SVEvaluateClass*>(pTool->getFirstObject(evaluateObjectInfo));
		if( pEvaluateStartAngle )
		{
			// Get Start Angle Result...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateStartAngleResultObjectGuid;
			pStartAngleResult = dynamic_cast<SVDoubleValueObjectClass*>(pEvaluateStartAngle->getFirstObject(resultObjectInfo));
		}

		// Get Evaluate End Angle...
		evaluateObjectInfo.SubType = SVEvaluateEndAngleObjectType;		
		pEvaluateEndAngle = dynamic_cast<SVEvaluateClass*>(pTool->getFirstObject(evaluateObjectInfo));
		if( pEvaluateEndAngle )
		{
			// Get End Angle Result...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateEndAngleResultObjectGuid;
			pEndAngleResult = dynamic_cast<SVDoubleValueObjectClass*>(pEvaluateEndAngle->getFirstObject(resultObjectInfo));
		}

		// Get Use Formula...
		SVObjectTypeInfoStruct objectInfo;
		objectInfo.EmbeddedID = SVOutputUseFormulaObjectGuid;
		pUseFormula = dynamic_cast<SVBoolValueObjectClass*>(pTool->getFirstObject(objectInfo));
		if( pUseFormula )
		{
			pUseFormula->GetValue( bUseFormula );
		}

		// Get Use Angle Method ...
		objectInfo.EmbeddedID = SVOutputAngularMethodObjectGuid;
		m_pAngleMethod = dynamic_cast<SVEnumerateValueObjectClass*>(pTool->getFirstObject(objectInfo));
		if( m_pAngleMethod )
		{
			CString l_strEnumList;
			m_pAngleMethod->GetEnumTypes( l_strEnumList );
			m_AngularMethodCombo.SetEnumTypes( l_strEnumList );
		}

		// Get Interpolation Mode...
		objectInfo.EmbeddedID = SVOutputInterpolationModeObjectGuid;
		pInterpolationMode = dynamic_cast<SVEnumerateValueObjectClass*>(pTool->getFirstObject(objectInfo));
		if( pInterpolationMode )
		{
			// Populate Interpolation Mode combo...
			CString strEnumList;
			pInterpolationMode->GetEnumTypes( strEnumList );
			interpolationComboCtrl.SetEnumTypes( strEnumList );
		}
		

		UpdateData( FALSE );
	
		// Check...
		if( pEvaluateCenterX &&	pCenterXResult && pEvaluateCenterY && pCenterYResult &&
			pEvaluateStartRadius && pStartRadiusResult && pEvaluateEndRadius && pEndRadiusResult &&
			pEvaluateStartAngle && pStartAngleResult &&	pEvaluateEndAngle && pEndAngleResult &&
			pUseFormula && pInterpolationMode
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
	if( pEvaluateCenterX )
	{
		CString l_Temp;
		l_Temp.LoadString( IDS_FORMULA_STRING );

		CString strCaption;
		strCaption = pEvaluateCenterX->GetName();
		strCaption += _T( " " );
		strCaption += l_Temp;

		const GUID& rInspectionID = pParentDialog->GetInspectionID();
		const GUID& rObjectID = pParentDialog->GetToolID();
		SVObjectTypeInfoStruct info(SVMathContainerObjectType, SVEvaluateCenterXObjectType);
		SvOg::SVFormulaEditorSheetClass dlg( rInspectionID, rObjectID, info, strCaption );
		dlg.DoModal();

		refresh();
	}
}

void SVToolAdjustmentDialogPolarTransformPageClass::OnCenterYFormulaButton() 
{
	if( pEvaluateCenterY )
	{
		CString l_Temp;
		l_Temp.LoadString( IDS_FORMULA_STRING );

		CString strCaption;
		strCaption = pEvaluateCenterY->GetName();
		strCaption += _T( " " );
		strCaption += l_Temp;

		const GUID& rInspectionID = pParentDialog->GetInspectionID();
		const GUID& rObjectID = pParentDialog->GetToolID();
		SVObjectTypeInfoStruct info(SVMathContainerObjectType, SVEvaluateCenterYObjectType);
		SvOg::SVFormulaEditorSheetClass dlg( rInspectionID, rObjectID, info, strCaption );
		dlg.DoModal();

		refresh();
	}
}

void SVToolAdjustmentDialogPolarTransformPageClass::OnStartRadiusFormulaButton() 
{
	if( pEvaluateStartRadius )
	{
		CString l_Temp;
		l_Temp.LoadString( IDS_FORMULA_STRING );

		CString strCaption;
		strCaption = pEvaluateStartRadius->GetName();
		strCaption += _T( " " );
		strCaption += l_Temp;

		const GUID& rInspectionID = pParentDialog->GetInspectionID();
		const GUID& rObjectID = pParentDialog->GetToolID();
		SVObjectTypeInfoStruct info(SVMathContainerObjectType, SVEvaluateStartRadiusObjectType);
		SvOg::SVFormulaEditorSheetClass dlg( rInspectionID, rObjectID, info, strCaption );
		dlg.DoModal();

		refresh();
	}
}

void SVToolAdjustmentDialogPolarTransformPageClass::OnEndRadiusFormulaButton() 
{
	if( pEvaluateEndRadius )
	{
		CString l_Temp;
		l_Temp.LoadString( IDS_FORMULA_STRING );

		CString strCaption;
		strCaption = pEvaluateEndRadius->GetName();
		strCaption += _T( " " );
		strCaption += l_Temp;

		const GUID& rInspectionID = pParentDialog->GetInspectionID();
		const GUID& rObjectID = pParentDialog->GetToolID();
		SVObjectTypeInfoStruct info(SVMathContainerObjectType, SVEvaluateEndRadiusObjectType);
		SvOg::SVFormulaEditorSheetClass dlg( rInspectionID, rObjectID, info, strCaption );
		dlg.DoModal();

		refresh();
	}
}

void SVToolAdjustmentDialogPolarTransformPageClass::OnStartAngleFormulaButton() 
{
	if( pEvaluateStartAngle )
	{
		CString l_Temp;
		l_Temp.LoadString( IDS_FORMULA_STRING );

		CString strCaption;
		strCaption = pEvaluateStartAngle->GetName();
		strCaption += _T( " " );
		strCaption += l_Temp;

		const GUID& rInspectionID = pParentDialog->GetInspectionID();
		const GUID& rObjectID = pParentDialog->GetToolID();
		SVObjectTypeInfoStruct info(SVMathContainerObjectType, SVEvaluateStartAngleObjectType);
		SvOg::SVFormulaEditorSheetClass dlg( rInspectionID, rObjectID, info, strCaption );
		dlg.DoModal();

		refresh();
	}
}

void SVToolAdjustmentDialogPolarTransformPageClass::OnEndAngleFormulaButton() 
{
	if( pEvaluateEndAngle )
	{
		CString l_Temp;
		l_Temp.LoadString( IDS_FORMULA_STRING );

		CString strCaption;
		strCaption = pEvaluateEndAngle->GetName();
		strCaption += _T( " " );
		strCaption += l_Temp;

		const GUID& rInspectionID = pParentDialog->GetInspectionID();
		const GUID& rObjectID = pParentDialog->GetToolID();
		SVObjectTypeInfoStruct info(SVMathContainerObjectType, SVEvaluateEndAngleObjectType);
		SvOg::SVFormulaEditorSheetClass dlg( rInspectionID, rObjectID, info, strCaption );
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

