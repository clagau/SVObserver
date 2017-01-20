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
#include "SVEvaluate.h"
#include "SVIPDoc.h"
#include "SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVImageTransform.h"
#include "SVToolSet.h"
#include "SVUtilityLibrary/SVString.h"
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
SVToolAdjustmentDialogRotationPageClass::SVToolAdjustmentDialogRotationPageClass( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* Parent )
	: CPropertyPage(SVToolAdjustmentDialogRotationPageClass::IDD)
{
	//{{AFX_DATA_INIT(SVToolAdjustmentDialogRotationPageClass)
	m_strRotationAngleValue = _T("");
	m_strRotationXValue = _T("");
	m_strRotationYValue = _T("");
	m_performRotation = FALSE;
	//}}AFX_DATA_INIT

	m_pParentDialog	= Parent;
	m_pTool			= nullptr;

	m_pPerformRotation		= nullptr;
	m_pEvaluateRotationX	= nullptr;
	m_pRotationXResult		= nullptr;
	m_pEvaluateRotationY	= nullptr;
	m_pRotationYResult		= nullptr;
	m_pEvaluateRotationAngle= nullptr;
	m_pRotationAngleResult	= nullptr;
	m_pInterpolationMode	= nullptr;
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
	DDX_Text(pDX, IDC_ROTATION_ANGLE_EDIT, m_strRotationAngleValue);
	DDX_Text(pDX, IDC_ROTATION_X_EDIT, m_strRotationXValue);
	DDX_Text(pDX, IDC_ROTATION_Y_EDIT, m_strRotationYValue);
	DDX_Check(pDX, IDC_PERFORM_ROTATION, m_performRotation);
	//}}AFX_DATA_MAP
}

BOOL SVToolAdjustmentDialogRotationPageClass::OnSetActive()
{
	if( m_pInterpolationMode )
	{
		SVString EnumString;
		m_pInterpolationMode->GetValue( EnumString );
		m_cbInterpolation.SelectString( -1, EnumString.c_str() );
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

	if( m_pParentDialog && ( m_pTool = m_pParentDialog->GetTool() ) )
	{
		SetTaskObject( m_pTool );

		// Get Evaluate Object for the X coordinate...
		SVObjectTypeInfoStruct evaluateObjectInfo;
		evaluateObjectInfo.ObjectType = SVMathContainerObjectType;
	
		// Set up the requestor for the evaluate result object
		SVObjectTypeInfoStruct resultObjectInfo;
		resultObjectInfo.ObjectType = SVDoubleValueObjectType;
		
		// Get Evaluate Object for the X coordinate...
		evaluateObjectInfo.SubType = SVEvaluateRotationXObjectType;
		m_pEvaluateRotationX = dynamic_cast<SVEvaluateClass*>(m_pTool->getFirstObject(evaluateObjectInfo));
		if( m_pEvaluateRotationX )
		{
			// Get Evaluate Result Object for the X coordinate...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateRotationXResultObjectGuid;
			m_pRotationXResult = dynamic_cast<SVDoubleValueObjectClass*>(m_pEvaluateRotationX->getFirstObject(resultObjectInfo));
		}

		// Get Evaluate Object for the Y coordinate...
		evaluateObjectInfo.SubType = SVEvaluateRotationYObjectType;
		m_pEvaluateRotationY = dynamic_cast<SVEvaluateClass*>(m_pTool->getFirstObject(evaluateObjectInfo));
		if( m_pEvaluateRotationY )
		{
			// Get Evaluate Result Object for the Y coordinate...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateRotationYResultObjectGuid;
			m_pRotationYResult = dynamic_cast<SVDoubleValueObjectClass*>(m_pEvaluateRotationY->getFirstObject(resultObjectInfo));
		}

		// Get Evaluate Object for the Angle...
		evaluateObjectInfo.SubType	   = SVEvaluateRotationAngleObjectType;
		m_pEvaluateRotationAngle = dynamic_cast<SVEvaluateClass*>(m_pTool->getFirstObject(evaluateObjectInfo));
		if( m_pEvaluateRotationAngle )
		{
			// Get Evaluate Result Object for the Angle...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateRotationAngleResultObjectGuid;
			m_pRotationAngleResult = dynamic_cast<SVDoubleValueObjectClass*>(m_pEvaluateRotationAngle->getFirstObject(resultObjectInfo));
		}
		
		// Get Rotation enabled...
		SVObjectTypeInfoStruct objectInfo;
		objectInfo.ObjectType = SVBoolValueObjectType;
		objectInfo.EmbeddedID = SVPerformRotationObjectGuid;
		m_pPerformRotation = dynamic_cast<SVBoolValueObjectClass*>(m_pTool->getFirstObject(objectInfo));

		// Interpolation Mode
		objectInfo.ObjectType = SVEnumValueObjectType;
		objectInfo.EmbeddedID = SVOutputInterpolationModeObjectGuid;
		m_pInterpolationMode = dynamic_cast<SVEnumerateValueObjectClass*>(m_pTool->getFirstObject(objectInfo));
		if( m_pInterpolationMode )
		{
			SVString EnumList;

			m_pInterpolationMode->GetEnumTypes( EnumList );
			m_cbInterpolation.SetEnumTypes( EnumList.c_str() );

			SVString EnumString;
			m_pInterpolationMode->GetValue( EnumString );
			m_cbInterpolation.SelectString( -1, EnumString.c_str() );
		}

		UpdateData( FALSE );
	
		// Check...
		if( m_pEvaluateRotationX && m_pRotationXResult &&
			m_pEvaluateRotationY && m_pRotationYResult &&
			m_pEvaluateRotationAngle && m_pRotationAngleResult &&
			m_pPerformRotation)
		{
			// Get Inspection Data..
			m_pPerformRotation->GetValue( m_performRotation );
			UpdateData(FALSE);

			refresh();

			return TRUE;
		}

	}

	// Not valid call...
	if( GetParent() )
	{
		GetParent()->SendMessage( WM_CLOSE );
	}
	else
	{
		SendMessage( WM_CLOSE );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void SVToolAdjustmentDialogRotationPageClass::OnAngleFormulaButton() 
{
	if( m_pEvaluateRotationAngle )
	{
		SVString Caption = m_pEvaluateRotationAngle->GetName();
		Caption += _T( " Formula" );

		const GUID& rInspectionID = m_pParentDialog->GetInspectionID();
		const GUID& rObjectID = m_pParentDialog->GetToolID();
		SVObjectTypeInfoStruct info(SVMathContainerObjectType, SVEvaluateRotationAngleObjectType);
		SvOg::SVFormulaEditorSheetClass dlg( rInspectionID, rObjectID, info, Caption.c_str() );
		dlg.DoModal();

		refresh();
	}
}

void SVToolAdjustmentDialogRotationPageClass::OnXFormulaButton() 
{
	if( m_pEvaluateRotationX )
	{
		SVString Caption = m_pEvaluateRotationX->GetName();
		Caption += _T( " Formula" );

		const GUID& rInspectionID = m_pParentDialog->GetInspectionID();
		const GUID& rObjectID = m_pParentDialog->GetToolID();
		SVObjectTypeInfoStruct info(SVMathContainerObjectType, SVEvaluateRotationXObjectType);
		SvOg::SVFormulaEditorSheetClass dlg( rInspectionID, rObjectID, info, Caption.c_str() );
		dlg.DoModal();

		refresh();
	}
}

void SVToolAdjustmentDialogRotationPageClass::OnYFormulaButton() 
{
	if( m_pEvaluateRotationY )
	{
		SVString Caption = m_pEvaluateRotationY->GetName();
		Caption += _T( " Formula" );

		const GUID& rInspectionID = m_pParentDialog->GetInspectionID();
		const GUID& rObjectID = m_pParentDialog->GetToolID();
		SVObjectTypeInfoStruct info(SVMathContainerObjectType, SVEvaluateRotationYObjectType);
		SvOg::SVFormulaEditorSheetClass dlg( rInspectionID, rObjectID, info, Caption.c_str() );
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
	HRESULT l_hrOk = S_FALSE;

	if( m_pTool )
	{
		UpdateData( TRUE ); // get data from dialog
		bool bUpdate = false;

		l_hrOk = AddInputRequest( m_pPerformRotation, m_performRotation );

		if( S_OK == l_hrOk )
		{
			l_hrOk = AddInputRequestMarker();
		}

		if( S_OK == l_hrOk )
		{
			l_hrOk = RunOnce( m_pTool->GetUniqueObjectID() );
		}

		int sel = m_cbInterpolation.GetCurSel();
		if( sel >= 0 )
		{
			long lValue = ( long ) m_cbInterpolation.GetItemData( sel );
			bUpdate = true;
			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pInterpolationMode, lValue );
			}
		}

		if( bUpdate )
		{
			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequestMarker();
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = RunOnce( m_pTool->GetUniqueObjectID() );
			}
		}

		UpdateData( FALSE );
	}

	return l_hrOk;
}

void SVToolAdjustmentDialogRotationPageClass::refresh()
{
	if( m_pTool )
	{
		CWnd* pWnd = nullptr;

		SetInspectionData();

		SVString Value;
		// refresh X settings...
		if( m_pRotationXResult )
		{
			m_pRotationXResult->GetValue( Value );
			m_strRotationXValue = Value.c_str();
		}
		else
		{
			m_strRotationXValue = _T( "" );
		}

		// refresh Y settings...
		if( m_pRotationYResult )
		{
			m_pRotationYResult->GetValue( Value );
			m_strRotationYValue = Value.c_str();
		}
		else
		{
			m_strRotationYValue = _T( "" );
		}

		// refresh Angle settings...
		if( m_pRotationAngleResult )
		{
			m_pRotationAngleResult->GetValue( Value );
			m_strRotationAngleValue = Value.c_str();
		}
		else
		{
			m_strRotationAngleValue = _T( "" );
		}

		UpdateData( FALSE ); // set data to dialog
	}
}
#pragma endregion

