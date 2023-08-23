//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogTransformationLearnPage
//* .File Name       : $Workfile:   TADlgTransformationLearnPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 13:07:14  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TADlgTransformationLearnPage.h"
#include "TADlgSheetClass.h"
#include "Operators/SVEvaluate.h"
#include "Operators/SVImageTransform.h"
#include "SVOGuiUtility/DataController.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

TADlgTransformationLearnPage::TADlgTransformationLearnPage( uint32_t, uint32_t , TADlgSheetClass* Parent )
: CPropertyPage(TADlgTransformationLearnPage::IDD)
{
	m_pTaskObject = (nullptr != Parent) ? Parent->GetTaskObject() : nullptr;

}

TADlgTransformationLearnPage::~TADlgTransformationLearnPage()
{
}

HRESULT TADlgTransformationLearnPage::SetInspectionData()
{
	UpdateData(true); // get data from dialog

	//@TODO[gra][8.00][15.01.2018]: The data controller should be used like the rest of SVOGui
	SvOgu::ValueController Values {SvOgu::BoundValues{ m_inspectionId, m_imageTransformId }};
	Values.Init();

	Values.Set<double>(SvPb::LearnedTranslationXEId, m_translationXValue);
	Values.Set<double>(SvPb::LearnedTranslationYEId, m_translationYValue);
	Values.Set<double>(SvPb::LearnedRotationXEId, m_rotationXValue);
	Values.Set<double>(SvPb::LearnedRotationYEId, m_rotationYValue);
	Values.Set<double>(SvPb::LearnedRotationAngleEId, m_rotationAngleValue);
	return Values.Commit();
}


void TADlgTransformationLearnPage::refresh()
{
	if( m_pTaskObject )
	{
		m_translationXValue = 0.0;
		m_translationYValue = 0.0;

		BOOL bPerformTranslation = false;
		if( m_pPerformTranslation )
			m_pPerformTranslation->GetValue( bPerformTranslation );

		if( bPerformTranslation )
		{
			// refresh Translation X settings...
			if( m_pTranslationXResult )
				m_pTranslationXResult->GetValue(	m_translationXValue );
			
			// refresh Translation Y settings...
			if( m_pTranslationYResult )
				m_pTranslationYResult->GetValue(	m_translationYValue );
		}
		

		m_rotationXValue = 0.0;
		m_rotationYValue = 0.0;
		m_rotationAngleValue = 0.0;

		BOOL bPerformRotation = false;
		if( m_pPerformRotation )
			m_pPerformRotation->GetValue( bPerformRotation );

		if( bPerformRotation )
		{
			// refresh Rotation X settings...
			if( m_pRotationXResult )
				m_pRotationXResult->GetValue(	m_rotationXValue );
			
			// refresh Rotation Y settings...
			if( m_pRotationYResult )
				m_pRotationYResult->GetValue(	m_rotationYValue );
			
			// refresh Rotation Angle settings...
			if( m_pRotationAngleResult )
				m_pRotationAngleResult->GetValue( m_rotationAngleValue );
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

void TADlgTransformationLearnPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TADlgTransformationLearnPage)
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


BEGIN_MESSAGE_MAP(TADlgTransformationLearnPage, CPropertyPage)
	//{{AFX_MSG_MAP(TADlgTransformationLearnPage)
	ON_BN_CLICKED(IDC_LEARN_BUTTON, OnLearnButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TADlgTransformationLearnPage message handlers

BOOL TADlgTransformationLearnPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// Set the font of the angle static text
	CWnd* pWnd = GetDlgItem( IDC_STATIC_ANGLE );
	
	angleFont.CreatePointFont( 100,"Symbol");
		
	pWnd->SetFont( &angleFont );

	pWnd->SetWindowText("q:");

	if (nullptr != m_pTaskObject)
	{
		// Get EvaluateX Object...
		SvDef::SVObjectTypeInfoStruct evaluateObjectInfo;
		evaluateObjectInfo.m_ObjectType = SvPb::SVMathContainerObjectType;

		// Prep Requestor for Results
		SvDef::SVObjectTypeInfoStruct resultObjectInfo;
		resultObjectInfo.m_ObjectType = SvPb::SVValueObjectType;
		resultObjectInfo.m_SubType = SvPb::SVDoubleValueObjectType;

		// Get Evaluate Object for Translation X Coordinate...
		evaluateObjectInfo.m_SubType	   = SvPb::SVEvaluateTranslationXObjectType;
		m_pEvaluateTranslationX = dynamic_cast<SvOp::SVEvaluate*>(m_pTaskObject->getFirstObject(evaluateObjectInfo));
		if( m_pEvaluateTranslationX )
		{
			// Get Evaluate Result Object for the Translation X coordinate...
			resultObjectInfo.m_EmbeddedID = SvPb::OutputEvaluateTranslationXResultEId;
			m_pTranslationXResult = dynamic_cast<SvVol::SVDoubleValueObjectClass*>(m_pEvaluateTranslationX->getFirstObject(resultObjectInfo));
		}

		// Get Evaluate Object for the Translation Y coordinate...
		evaluateObjectInfo.m_SubType	   = SvPb::SVEvaluateTranslationYObjectType;
		m_pEvaluateTranslationY = dynamic_cast<SvOp::SVEvaluate*>(m_pTaskObject->getFirstObject(evaluateObjectInfo));
		if( m_pEvaluateTranslationY )
		{
			// Get Evaluate Result Object for the Translation Y coordinate...
			resultObjectInfo.m_EmbeddedID = SvPb::OutputEvaluateTranslationYResultEId;
			m_pTranslationYResult = dynamic_cast<SvVol::SVDoubleValueObjectClass*>(m_pEvaluateTranslationY->getFirstObject(resultObjectInfo));
		}

		// Get Evaluate Object for Translation X Coordinate...
		evaluateObjectInfo.m_SubType = SvPb::SVEvaluateRotationXObjectType;
		m_pEvaluateRotationX = dynamic_cast<SvOp::SVEvaluate*>(m_pTaskObject->getFirstObject(evaluateObjectInfo));
		if( m_pEvaluateRotationX )
		{
			// Get Evaluate Result Object for the Rotation X coordinate...
			resultObjectInfo.m_EmbeddedID = SvPb::OutputEvaluateRotationXResultEId;
			m_pRotationXResult = dynamic_cast<SvVol::SVDoubleValueObjectClass*>(m_pEvaluateRotationX->getFirstObject(resultObjectInfo));
		}

		// Get Evaluate Object for the Rotation Y coordinate...
		evaluateObjectInfo.m_SubType	   = SvPb::SVEvaluateRotationYObjectType;
		m_pEvaluateRotationY = dynamic_cast<SvOp::SVEvaluate*>(m_pTaskObject->getFirstObject(evaluateObjectInfo));
		if( m_pEvaluateRotationY )
		{
			// Get Evaluate Result Object for the Rotation Y coordinate...
			resultObjectInfo.m_EmbeddedID = SvPb::OutputEvaluateRotationYResultEId;
			m_pRotationYResult = dynamic_cast<SvVol::SVDoubleValueObjectClass*>(m_pEvaluateRotationY->getFirstObject(resultObjectInfo));
		}

		// Get Evaluate Object for the Rotation Angle...
		evaluateObjectInfo.m_SubType	   = SvPb::SVEvaluateRotationAngleObjectType;
		m_pEvaluateRotationAngle = dynamic_cast<SvOp::SVEvaluate*>(m_pTaskObject->getFirstObject(evaluateObjectInfo));
		if( m_pEvaluateRotationAngle )
		{
			// Get Evaluate Result Object for the Rotation Angle...
			resultObjectInfo.m_EmbeddedID = SvPb::OutputEvaluateRotationAngleResultEId;
			m_pRotationAngleResult = dynamic_cast<SvVol::SVDoubleValueObjectClass*>(m_pEvaluateRotationAngle->getFirstObject(resultObjectInfo));
		}

		// Get Rotation enabled...
		SvDef::SVObjectTypeInfoStruct objectInfo;
		objectInfo.m_ObjectType = SvPb::SVValueObjectType;
		objectInfo.m_SubType = SvPb::SVBoolValueObjectType;
		objectInfo.m_EmbeddedID = SvPb::PerformRotationEId;
		m_pPerformRotation = dynamic_cast<SvVol::SVBoolValueObjectClass*>(m_pTaskObject->getFirstObject(objectInfo));

		// get Translation Enabled
		objectInfo.m_EmbeddedID = SvPb::PerformTranslationEId;
		m_pPerformTranslation = dynamic_cast<SvVol::SVBoolValueObjectClass*>(m_pTaskObject->getFirstObject(objectInfo));

		// Get SVImageTransform Object...
		SvDef::SVObjectTypeInfoStruct transformObjectInfo;
		transformObjectInfo.m_ObjectType = SvPb::SVTransformObjectType;
		transformObjectInfo.m_SubType = SvPb::SVImageTransformObjectType;

		auto* pImageTransform = dynamic_cast<SvOp::SVImageTransform*>(m_pTaskObject->getFirstObject(transformObjectInfo));

		UpdateData( FALSE );
	
		// Check...
		if( m_pPerformTranslation && m_pPerformRotation &&
			m_pEvaluateTranslationX && m_pTranslationXResult &&
			m_pEvaluateTranslationY && m_pTranslationYResult &&
			m_pEvaluateRotationX && m_pRotationXResult &&
			m_pEvaluateRotationY && m_pRotationYResult &&
			m_pEvaluateRotationAngle && m_pRotationAngleResult &&
			pImageTransform )
		{
			m_inspectionId = pImageTransform->GetInspection()->getObjectId();
			m_imageTransformId = pImageTransform->getObjectId();

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

void TADlgTransformationLearnPage::OnLearnButton() 
{
	UpdateData( TRUE );

	if( m_pTranslationXResult && m_pTranslationYResult &&
		m_pRotationXResult && m_pRotationYResult && 
		m_pRotationAngleResult )
	{
		// Run Tool - Get Updated Results
		refresh();

		SetInspectionData();

		refreshLearnedValues();
	}
}

void TADlgTransformationLearnPage::refreshLearnedValues()
{
	SvOgu::ValueController Values {SvOgu::BoundValues{ m_inspectionId, m_imageTransformId }};
	Values.Init();

	m_LearnedTranslationXValue = Values.Get<CString>(SvPb::LearnedTranslationXEId);
	m_LearnedTranslationYValue = Values.Get<CString>(SvPb::LearnedTranslationYEId);
	m_LearnedRotationXValue = Values.Get<CString>(SvPb::LearnedRotationXEId);
	m_LearnedRotationYValue = Values.Get<CString>(SvPb::LearnedRotationYEId);
	m_LearnedRotationAngleValue = Values.Get<CString>(SvPb::LearnedRotationAngleEId);

	UpdateData( FALSE );
}

BOOL TADlgTransformationLearnPage::OnSetActive() 
{
	refresh();

	refreshLearnedValues();

	return CPropertyPage::OnSetActive();
}

