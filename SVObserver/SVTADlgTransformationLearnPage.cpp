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
#include "SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
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
	StrTranslationXValue = _T("");
	StrTranslationYValue = _T("");
	StrRotationXValue = _T("");
	StrRotationYValue = _T("");
	StrRotationAngleValue = _T("");
	StrLearnedTranslationXValue = _T("");
	StrLearnedTranslationYValue = _T("");
	StrLearnedRotationXValue = _T("");
	StrLearnedRotationYValue = _T("");
	StrLearnedRotationAngleValue = _T("");
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
			l_hrOk = RunOnce( pTool );
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

		StrTranslationXValue.Format( _T( "%lf" ), m_translationXValue );
		StrTranslationYValue.Format( _T( "%lf" ), m_translationYValue );
		StrRotationXValue.Format( _T( "%lf" ), m_rotationXValue );
		StrRotationYValue.Format( _T( "%lf" ), m_rotationYValue );
		StrRotationAngleValue.Format( _T( "%lf" ), m_rotationAngleValue );
		
		UpdateData( FALSE ); // set data to dialog
	}
	UpdateData( TRUE );	// get data from dialog
}

void SVToolAdjustmentDialogTransformationLearnPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVToolAdjustmentDialogTransformationLearnPageClass)
	DDX_Text(pDX, IDC_TRANSLATION_X_EDIT, StrTranslationXValue);
	DDX_Text(pDX, IDC_TRANSLATION_Y_EDIT, StrTranslationYValue);
	DDX_Text(pDX, IDC_ROTATION_X_EDIT, StrRotationXValue);
	DDX_Text(pDX, IDC_ROTATION_Y_EDIT, StrRotationYValue);
	DDX_Text(pDX, IDC_ROTATION_ANGLE_EDIT, StrRotationAngleValue);
	DDX_Text(pDX, IDC_LEARNED_TRANSLATION_X_EDIT, StrLearnedTranslationXValue);
	DDX_Text(pDX, IDC_LEARNED_TRANSLATION_Y_EDIT, StrLearnedTranslationYValue);
	DDX_Text(pDX, IDC_LEARNED_ROTATION_X_EDIT, StrLearnedRotationXValue);
	DDX_Text(pDX, IDC_LEARNED_ROTATION_Y_EDIT, StrLearnedRotationYValue);
	DDX_Text(pDX, IDC_LEARNED_ROTATION_ANGLE_EDIT, StrLearnedRotationAngleValue);
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
		pEvaluateTranslationX = reinterpret_cast<SVEvaluateClass*>(::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&evaluateObjectInfo) ));
		if( pEvaluateTranslationX )
		{
			// Get Evaluate Result Object for the Translation X coordinate...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateTranslationXResultObjectGuid;
			pTranslationXResult = reinterpret_cast<SVDoubleValueObjectClass*>(::SVSendMessage( pEvaluateTranslationX, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&resultObjectInfo) ));
		}

		// Get Evaluate Object for the Translation Y coordinate...
		evaluateObjectInfo.SubType	   = SVEvaluateTranslationYObjectType;
		pEvaluateTranslationY = reinterpret_cast<SVEvaluateClass*>(::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&evaluateObjectInfo) ));
		if( pEvaluateTranslationY )
		{
			// Get Evaluate Result Object for the Translation Y coordinate...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateTranslationYResultObjectGuid;
			pTranslationYResult = reinterpret_cast<SVDoubleValueObjectClass*>(::SVSendMessage( pEvaluateTranslationY, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&resultObjectInfo) ));
		}

		// Get Evaluate Object for Translation X Coordinate...
		evaluateObjectInfo.SubType = SVEvaluateRotationXObjectType;
		pEvaluateRotationX = reinterpret_cast<SVEvaluateClass*>(::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&evaluateObjectInfo) ));
		if( pEvaluateRotationX )
		{
			// Get Evaluate Result Object for the Rotation X coordinate...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateRotationXResultObjectGuid;
			pRotationXResult = reinterpret_cast<SVDoubleValueObjectClass*>(::SVSendMessage( pEvaluateRotationX, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&resultObjectInfo) ));
		}

		// Get Evaluate Object for the Rotation Y coordinate...
		evaluateObjectInfo.SubType	   = SVEvaluateRotationYObjectType;
		pEvaluateRotationY = reinterpret_cast<SVEvaluateClass*>(::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&evaluateObjectInfo) ));
		if( pEvaluateRotationY )
		{
			// Get Evaluate Result Object for the Rotation Y coordinate...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateRotationYResultObjectGuid;
			pRotationYResult = reinterpret_cast<SVDoubleValueObjectClass*>(::SVSendMessage( pEvaluateRotationY, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&resultObjectInfo) ));
		}

		// Get Evaluate Object for the Rotation Angle...
		evaluateObjectInfo.SubType	   = SVEvaluateRotationAngleObjectType;
		pEvaluateRotationAngle = reinterpret_cast<SVEvaluateClass*>(::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&evaluateObjectInfo) ));
		if( pEvaluateRotationAngle )
		{
			// Get Evaluate Result Object for the Rotation Angle...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateRotationAngleResultObjectGuid;
			pRotationAngleResult = reinterpret_cast<SVDoubleValueObjectClass*>(::SVSendMessage( pEvaluateRotationAngle, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&resultObjectInfo) ));
		}

		// Get Rotation enabled...
		SVObjectTypeInfoStruct objectInfo;
		objectInfo.ObjectType = SVBoolValueObjectType;
		objectInfo.EmbeddedID = SVPerformRotationObjectGuid;
		pPerformRotation = reinterpret_cast<SVBoolValueObjectClass*>(::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&objectInfo) ));

		// get Translation Enabled
		objectInfo.EmbeddedID = SVPerformTranslationObjectGuid;
		pPerformTranslation = reinterpret_cast<SVBoolValueObjectClass*>(::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&objectInfo) ));

		// Get SVImageTransformClass Object...
		SVObjectTypeInfoStruct transformObjectInfo;
		transformObjectInfo.ObjectType = SVTransformObjectType;
		transformObjectInfo.SubType = SVImageTransformObjectType;

		pImageTransform = reinterpret_cast<SVImageTransformClass*>(::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&transformObjectInfo) ));
		if( pImageTransform )
		{
			// Get learned Objects...
			SVObjectTypeInfoStruct learnedObjectInfo;
			learnedObjectInfo.ObjectType = SVDoubleValueObjectType;

			// Get learned Translation X Object...
			learnedObjectInfo.EmbeddedID = SVLearnedTranslationXObjectGuid;
			pLearnedTranslationX = reinterpret_cast<SVDoubleValueObjectClass*>(::SVSendMessage( pImageTransform, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&learnedObjectInfo) ));

			// Get learned Translation Y Object...
			learnedObjectInfo.EmbeddedID = SVLearnedTranslationYObjectGuid;
			pLearnedTranslationY = reinterpret_cast<SVDoubleValueObjectClass*>(::SVSendMessage( pImageTransform, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&learnedObjectInfo) ));

			// Get learned Rotation X Object...
			learnedObjectInfo.EmbeddedID = SVLearnedRotationXObjectGuid;
			pLearnedRotationX = reinterpret_cast<SVDoubleValueObjectClass*>(::SVSendMessage( pImageTransform, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&learnedObjectInfo) ));

			// Get learned Translation Y Object...
			learnedObjectInfo.EmbeddedID = SVLearnedRotationYObjectGuid;
			pLearnedRotationY = reinterpret_cast<SVDoubleValueObjectClass*>(::SVSendMessage( pImageTransform, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&learnedObjectInfo) ));

			// Get learned Rotation Angle Object...
			learnedObjectInfo.EmbeddedID = SVLearnedRotationAngleObjectGuid;
			pLearnedRotationAngle = reinterpret_cast<SVDoubleValueObjectClass*>(::SVSendMessage( pImageTransform, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&learnedObjectInfo) ));
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
	// refresh Learned Translation X settings...
	if( pLearnedTranslationX )
		pLearnedTranslationX->GetValue(	StrLearnedTranslationXValue );
	else
		StrLearnedTranslationXValue = _T( "" );
	
	// refresh Learned Translation Y settings...
	if( pLearnedTranslationY )
		pLearnedTranslationY->GetValue(	StrLearnedTranslationYValue );
	else
		StrLearnedTranslationYValue = _T( "" );

	// refresh Learned Rotation X settings...
	if( pLearnedRotationX )
		pLearnedRotationX->GetValue( StrLearnedRotationXValue );
	else
		StrLearnedRotationXValue = _T( "" );
	
	// refresh Learned Rotation Y settings...
	if( pLearnedRotationY )
		pLearnedRotationY->GetValue( StrLearnedRotationYValue );
	else
		StrLearnedRotationYValue = _T( "" );

	// refresh Learned Rotation Angle settings...
	if( pLearnedRotationAngle )
		pLearnedRotationAngle->GetValue( StrLearnedRotationAngleValue );
	else
		StrLearnedRotationAngleValue = _T( "" );

	UpdateData( FALSE );
}

BOOL SVToolAdjustmentDialogTransformationLearnPageClass::OnSetActive() 
{
	refresh();

	refreshLearnedValues();

	return CPropertyPage::OnSetActive();
}

