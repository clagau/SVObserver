//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogTransformationLearnPage
//* .File Name       : $Workfile:   SVTADlgTransformationLearnPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 13:07:14  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTADlgTransformationLearnPage.h"

#include "SVEvaluate.h"
#include "SVImageTransform.h"
#include "SVIPDoc.h"
#include "SVOCore/SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVToolAdjustmentDialogTransformationLearnPageClass::SVToolAdjustmentDialogTransformationLearnPageClass( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* Parent )
: CPropertyPage(SVToolAdjustmentDialogTransformationLearnPageClass::IDD)
{
	//{{AFX_DATA_INIT(SVToolAdjustmentDialogTransformationLearnPageClass)
	m_TranslationXValue = _T("");
	m_TranslationYValue = _T("");
	m_RotationXValue = _T("");
	m_RotationYValue = _T("");
	m_RotationAngleValue = _T("");
	m_LearnedTranslationXValue = _T("");
	m_LearnedTranslationYValue = _T("");
	m_LearnedRotationXValue = _T("");
	m_LearnedRotationYValue = _T("");
	m_LearnedRotationAngleValue = _T("");
	//}}AFX_DATA_INIT

	pParentDialog	= Parent;
	pTool			= nullptr;

	pPerformTranslation		= nullptr;

	pEvaluateTranslationX	= nullptr;
	pTranslationXResult		= nullptr;

	pEvaluateTranslationY	= nullptr;
	pTranslationYResult		= nullptr;

	pPerformRotation		= nullptr;

	pEvaluateRotationX		= nullptr;
	pRotationXResult		= nullptr;

	pEvaluateRotationY		= nullptr;
	pRotationYResult		= nullptr;

	pEvaluateRotationAngle	= nullptr;
	pRotationAngleResult	= nullptr;

	pImageTransform			= nullptr;
	pLearnedTranslationX	= nullptr;
	pLearnedTranslationY	= nullptr;
	pLearnedRotationX		= nullptr;
	pLearnedRotationY		= nullptr;
	pLearnedRotationAngle	= nullptr;
}

SVToolAdjustmentDialogTransformationLearnPageClass::~SVToolAdjustmentDialogTransformationLearnPageClass()
{
}

HRESULT SVToolAdjustmentDialogTransformationLearnPageClass::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	if( pTool )
	{
		UpdateData( TRUE ); // get data from dialog

		l_hrOk = AddInputRequest( pLearnedTranslationX, m_translationXValue );

		if( S_OK == l_hrOk )
		{
			l_hrOk = AddInputRequest( pLearnedTranslationY, m_translationYValue );
		}

		if( S_OK == l_hrOk )
		{
			l_hrOk = AddInputRequest( pLearnedRotationX, m_rotationXValue );
		}

		if( S_OK == l_hrOk )
		{
			l_hrOk = AddInputRequest( pLearnedRotationY, m_rotationYValue );
		}

		if( S_OK == l_hrOk )
		{
			l_hrOk = AddInputRequest( pLearnedRotationAngle, m_rotationAngleValue );
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


void SVToolAdjustmentDialogTransformationLearnPageClass::refresh()
{
	if( pTool )
	{
		CWnd* pWnd = nullptr;


		m_translationXValue = 0.0;
		m_translationYValue = 0.0;

		BOOL bPerformTranslation = FALSE;
		if( pPerformTranslation )
			pPerformTranslation->GetValue( bPerformTranslation );

		if( bPerformTranslation )
		{
			// refresh Translation X settings...
			if( pTranslationXResult )
				pTranslationXResult->GetValue(	m_translationXValue );
			
			// refresh Translation Y settings...
			if( pTranslationYResult )
				pTranslationYResult->GetValue(	m_translationYValue );
		}
		

		m_rotationXValue = 0.0;
		m_rotationYValue = 0.0;
		m_rotationAngleValue = 0.0;

		BOOL bPerformRotation = FALSE;
		if( pPerformRotation )
			pPerformRotation->GetValue( bPerformRotation );

		if( bPerformRotation )
		{
			// refresh Rotation X settings...
			if( pRotationXResult )
				pRotationXResult->GetValue(	m_rotationXValue );
			
			// refresh Rotation Y settings...
			if( pRotationYResult )
				pRotationYResult->GetValue(	m_rotationYValue );
			
			// refresh Rotation Angle settings...
			if( pRotationAngleResult )
				pRotationAngleResult->GetValue( m_rotationAngleValue );
		}

		m_TranslationXValue.Format( _T( "%lf" ), m_translationXValue );
		m_TranslationYValue.Format( _T( "%lf" ), m_translationYValue );
		m_RotationXValue.Format( _T( "%lf" ), m_rotationXValue );
		m_RotationYValue.Format( _T( "%lf" ), m_rotationYValue );
		m_RotationAngleValue.Format( _T( "%lf" ), m_rotationAngleValue );
		
		UpdateData( FALSE ); // set data to dialog
	}
	UpdateData( TRUE );	// get data from dialog
}

void SVToolAdjustmentDialogTransformationLearnPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVToolAdjustmentDialogTransformationLearnPageClass)
	DDX_Text(pDX, IDC_TRANSLATION_X_EDIT, m_TranslationXValue);
	DDX_Text(pDX, IDC_TRANSLATION_Y_EDIT, m_TranslationYValue);
	DDX_Text(pDX, IDC_ROTATION_X_EDIT, m_RotationXValue);
	DDX_Text(pDX, IDC_ROTATION_Y_EDIT, m_RotationYValue);
	DDX_Text(pDX, IDC_ROTATION_ANGLE_EDIT, m_RotationAngleValue);
	DDX_Text(pDX, IDC_LEARNED_TRANSLATION_X_EDIT, m_LearnedTranslationXValue);
	DDX_Text(pDX, IDC_LEARNED_TRANSLATION_Y_EDIT, m_LearnedTranslationYValue);
	DDX_Text(pDX, IDC_LEARNED_ROTATION_X_EDIT, m_LearnedRotationXValue);
	DDX_Text(pDX, IDC_LEARNED_ROTATION_Y_EDIT, m_LearnedRotationYValue);
	DDX_Text(pDX, IDC_LEARNED_ROTATION_ANGLE_EDIT, m_LearnedRotationAngleValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogTransformationLearnPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVToolAdjustmentDialogTransformationLearnPageClass)
	ON_BN_CLICKED(IDC_LEARN_BUTTON, OnLearnButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVToolAdjustmentDialogTransformationLearnPageClass message handlers

BOOL SVToolAdjustmentDialogTransformationLearnPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// Set the font of the angle static text
	CWnd* pWnd = GetDlgItem( IDC_STATIC_ANGLE );
	
	angleFont.CreatePointFont( 100,"Symbol");
		
	pWnd->SetFont( &angleFont );

	pWnd->SetWindowText("q:");

	if( pParentDialog && ( pTool = pParentDialog->GetTool() ) )
	{
		SetTaskObject( pTool );

		// Get EvaluateX Object...
		SVObjectTypeInfoStruct evaluateObjectInfo;
		evaluateObjectInfo.ObjectType = SVMathContainerObjectType;

		// Prep Requestor for Results
		SVObjectTypeInfoStruct resultObjectInfo;
		resultObjectInfo.ObjectType = SVDoubleValueObjectType;

		// Get Evaluate Object for Translation X Coordinate...
		evaluateObjectInfo.SubType	   = SVEvaluateTranslationXObjectType;
		pEvaluateTranslationX = dynamic_cast<SVEvaluateClass*>(pTool->getFirstObject(evaluateObjectInfo));
		if( pEvaluateTranslationX )
		{
			// Get Evaluate Result Object for the Translation X coordinate...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateTranslationXResultObjectGuid;
			pTranslationXResult = dynamic_cast<SVDoubleValueObjectClass*>(pEvaluateTranslationX->getFirstObject(resultObjectInfo));
		}

		// Get Evaluate Object for the Translation Y coordinate...
		evaluateObjectInfo.SubType	   = SVEvaluateTranslationYObjectType;
		pEvaluateTranslationY = dynamic_cast<SVEvaluateClass*>(pTool->getFirstObject(evaluateObjectInfo));
		if( pEvaluateTranslationY )
		{
			// Get Evaluate Result Object for the Translation Y coordinate...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateTranslationYResultObjectGuid;
			pTranslationYResult = dynamic_cast<SVDoubleValueObjectClass*>(pEvaluateTranslationY->getFirstObject(resultObjectInfo));
		}

		// Get Evaluate Object for Translation X Coordinate...
		evaluateObjectInfo.SubType = SVEvaluateRotationXObjectType;
		pEvaluateRotationX = dynamic_cast<SVEvaluateClass*>(pTool->getFirstObject(evaluateObjectInfo));
		if( pEvaluateRotationX )
		{
			// Get Evaluate Result Object for the Rotation X coordinate...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateRotationXResultObjectGuid;
			pRotationXResult = dynamic_cast<SVDoubleValueObjectClass*>(pEvaluateRotationX->getFirstObject(resultObjectInfo));
		}

		// Get Evaluate Object for the Rotation Y coordinate...
		evaluateObjectInfo.SubType	   = SVEvaluateRotationYObjectType;
		pEvaluateRotationY = dynamic_cast<SVEvaluateClass*>(pTool->getFirstObject(evaluateObjectInfo));
		if( pEvaluateRotationY )
		{
			// Get Evaluate Result Object for the Rotation Y coordinate...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateRotationYResultObjectGuid;
			pRotationYResult = dynamic_cast<SVDoubleValueObjectClass*>(pEvaluateRotationY->getFirstObject(resultObjectInfo));
		}

		// Get Evaluate Object for the Rotation Angle...
		evaluateObjectInfo.SubType	   = SVEvaluateRotationAngleObjectType;
		pEvaluateRotationAngle = dynamic_cast<SVEvaluateClass*>(pTool->getFirstObject(evaluateObjectInfo));
		if( pEvaluateRotationAngle )
		{
			// Get Evaluate Result Object for the Rotation Angle...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateRotationAngleResultObjectGuid;
			pRotationAngleResult = dynamic_cast<SVDoubleValueObjectClass*>(pEvaluateRotationAngle->getFirstObject(resultObjectInfo));
		}

		// Get Rotation enabled...
		SVObjectTypeInfoStruct objectInfo;
		objectInfo.ObjectType = SVBoolValueObjectType;
		objectInfo.EmbeddedID = SVPerformRotationObjectGuid;
		pPerformRotation = dynamic_cast<SVBoolValueObjectClass*>(pTool->getFirstObject(objectInfo));

		// get Translation Enabled
		objectInfo.EmbeddedID = SVPerformTranslationObjectGuid;
		pPerformTranslation = dynamic_cast<SVBoolValueObjectClass*>(pTool->getFirstObject(objectInfo));

		// Get SVImageTransformClass Object...
		SVObjectTypeInfoStruct transformObjectInfo;
		transformObjectInfo.ObjectType = SVTransformObjectType;
		transformObjectInfo.SubType = SVImageTransformObjectType;

		pImageTransform = dynamic_cast<SVImageTransformClass*>(pTool->getFirstObject(transformObjectInfo));
		if( pImageTransform )
		{
			// Get learned Objects...
			SVObjectTypeInfoStruct learnedObjectInfo;
			learnedObjectInfo.ObjectType = SVDoubleValueObjectType;

			// Get learned Translation X Object...
			learnedObjectInfo.EmbeddedID = SVLearnedTranslationXObjectGuid;
			pLearnedTranslationX = dynamic_cast<SVDoubleValueObjectClass*>(pImageTransform->getFirstObject(learnedObjectInfo));

			// Get learned Translation Y Object...
			learnedObjectInfo.EmbeddedID = SVLearnedTranslationYObjectGuid;
			pLearnedTranslationY = dynamic_cast<SVDoubleValueObjectClass*>(pImageTransform->getFirstObject(learnedObjectInfo));

			// Get learned Rotation X Object...
			learnedObjectInfo.EmbeddedID = SVLearnedRotationXObjectGuid;
			pLearnedRotationX = dynamic_cast<SVDoubleValueObjectClass*>(pImageTransform->getFirstObject(learnedObjectInfo));

			// Get learned Translation Y Object...
			learnedObjectInfo.EmbeddedID = SVLearnedRotationYObjectGuid;
			pLearnedRotationY = dynamic_cast<SVDoubleValueObjectClass*>(pImageTransform->getFirstObject(learnedObjectInfo));

			// Get learned Rotation Angle Object...
			learnedObjectInfo.EmbeddedID = SVLearnedRotationAngleObjectGuid;
			pLearnedRotationAngle = dynamic_cast<SVDoubleValueObjectClass*>(pImageTransform->getFirstObject(learnedObjectInfo));
		}


		UpdateData( FALSE );
	
		// Check...
		if( pPerformTranslation && pPerformRotation &&
			pEvaluateTranslationX && pTranslationXResult &&
			pEvaluateTranslationY && pTranslationYResult &&
			pEvaluateRotationX && pRotationXResult &&
			pEvaluateRotationY && pRotationYResult &&
			pEvaluateRotationAngle && pRotationAngleResult &&
			pImageTransform && pLearnedTranslationX && pLearnedTranslationY && 
			pLearnedRotationX && pLearnedRotationY && pLearnedRotationAngle
		  )
		{
			// Run the Tool - Get Updated Results
			refresh();

			// Refresh the learned values on the dialog
			refreshLearnedValues();

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

void SVToolAdjustmentDialogTransformationLearnPageClass::OnLearnButton() 
{
	UpdateData( TRUE );

	if( pTranslationXResult && pTranslationYResult &&
		pRotationXResult && pRotationYResult && 
		pRotationAngleResult &&
		pLearnedTranslationX && pLearnedTranslationY && 
		pLearnedRotationX && pLearnedRotationY && pLearnedRotationAngle
	  )
	{
		// Run Tool - Get Updated Results
		refresh();

		SetInspectionData();

		refreshLearnedValues();
	}
}

void SVToolAdjustmentDialogTransformationLearnPageClass::refreshLearnedValues()
{
	SVString Value;
	// refresh Learned Translation X settings...
	if( nullptr != pLearnedTranslationX )
	{
		pLearnedTranslationX->getValue(	Value );
		m_LearnedTranslationXValue = Value.c_str();
	}
	else
	{
		m_LearnedTranslationXValue = _T("");
	}
	
	// refresh Learned Translation Y settings...
	if( nullptr != pLearnedTranslationY )
	{
		pLearnedTranslationY->getValue(	Value );
		m_LearnedTranslationYValue = Value.c_str();
	}
	else
	{
		m_LearnedTranslationYValue = _T("");
	}

	// refresh Learned Rotation X settings...
	if( nullptr != pLearnedRotationX )
	{
		pLearnedRotationX->getValue( Value );
		m_LearnedRotationXValue = Value.c_str();
	}
	else
	{
		m_LearnedRotationXValue = _T("");
	}
	
	// refresh Learned Rotation Y settings...
	if( nullptr != pLearnedRotationY )
	{
		pLearnedRotationY->getValue( Value );
		m_LearnedRotationYValue = Value.c_str();
	}
	else
	{
		m_LearnedRotationYValue = _T("");
	}

	// refresh Learned Rotation Angle settings...
	if( nullptr != pLearnedRotationAngle )
	{
		pLearnedRotationAngle->getValue( Value );

		m_LearnedRotationAngleValue = Value.c_str();
	}
	else
	{
		m_LearnedRotationAngleValue = _T("");
	}

	UpdateData( FALSE );
}

BOOL SVToolAdjustmentDialogTransformationLearnPageClass::OnSetActive() 
{
	refresh();

	refreshLearnedValues();

	return CPropertyPage::OnSetActive();
}

