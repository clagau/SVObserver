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

#include "Operators/SVEvaluate.h"
#include "Operators/SVImageTransform.h"
#include "SVIPDoc.h"
#include "Tools/SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVOGui/ValuesAccessor.h"
#include "SVOGui/DataController.h"
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

	m_pParentDialog	= Parent;
	m_pTool			= nullptr;

	m_pPerformTranslation		= nullptr;

	m_pEvaluateTranslationX	= nullptr;
	m_pTranslationXResult		= nullptr;

	m_pEvaluateTranslationY	= nullptr;
	m_pTranslationYResult		= nullptr;

	m_pPerformRotation		= nullptr;

	m_pEvaluateRotationX		= nullptr;
	m_pRotationXResult		= nullptr;

	m_pEvaluateRotationY		= nullptr;
	m_pRotationYResult		= nullptr;

	m_pEvaluateRotationAngle	= nullptr;
	m_pRotationAngleResult	= nullptr;

	m_pImageTransform			= nullptr;
	m_pLearnedTranslationX	= nullptr;
	m_pLearnedTranslationY	= nullptr;
	m_pLearnedRotationX		= nullptr;
	m_pLearnedRotationY		= nullptr;
	m_pLearnedRotationAngle	= nullptr;
}

SVToolAdjustmentDialogTransformationLearnPageClass::~SVToolAdjustmentDialogTransformationLearnPageClass()
{
}

HRESULT SVToolAdjustmentDialogTransformationLearnPageClass::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	if(nullptr != m_pImageTransform)
	{
		UpdateData( true ); // get data from dialog

		//@TODO[gra][8.00][15.01.2018]: The data controller should be used like the rest of SVOGui
		typedef SvOg::DataController<SvOg::ValuesAccessor, SvOg::ValuesAccessor::value_type> Controller;
		Controller Values{ SvOg::BoundValues{ m_pImageTransform->GetInspection()->GetUniqueObjectID(), m_pImageTransform->GetUniqueObjectID() } };
		Values.Init();

		Values.Set<double>(m_pLearnedTranslationX->GetEmbeddedID(), m_translationXValue);
		Values.Set<double>(m_pLearnedTranslationY->GetEmbeddedID(), m_translationYValue);
		Values.Set<double>(m_pLearnedRotationX->GetEmbeddedID(), m_rotationXValue);
		Values.Set<double>(m_pLearnedRotationY->GetEmbeddedID(), m_rotationYValue);
		Values.Set<double>(m_pLearnedRotationAngle->GetEmbeddedID(), m_rotationAngleValue);
		Values.Commit();
	}

	return l_hrOk;
}


void SVToolAdjustmentDialogTransformationLearnPageClass::refresh()
{
	if( m_pTool )
	{
		CWnd* pWnd = nullptr;


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

	if( m_pParentDialog && ( m_pTool = m_pParentDialog->GetTool() ) )
	{
		// Get EvaluateX Object...
		SvDef::SVObjectTypeInfoStruct evaluateObjectInfo;
		evaluateObjectInfo.ObjectType = SvPb::SVMathContainerObjectType;

		// Prep Requestor for Results
		SvDef::SVObjectTypeInfoStruct resultObjectInfo;
		resultObjectInfo.ObjectType = SvPb::SVValueObjectType;
		resultObjectInfo.SubType = SvPb::SVDoubleValueObjectType;

		// Get Evaluate Object for Translation X Coordinate...
		evaluateObjectInfo.SubType	   = SvPb::SVEvaluateTranslationXObjectType;
		m_pEvaluateTranslationX = dynamic_cast<SvOp::SVEvaluateClass*>(m_pTool->getFirstObject(evaluateObjectInfo));
		if( m_pEvaluateTranslationX )
		{
			// Get Evaluate Result Object for the Translation X coordinate...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateTranslationXResultObjectGuid;
			m_pTranslationXResult = dynamic_cast<SvVol::SVDoubleValueObjectClass*>(m_pEvaluateTranslationX->getFirstObject(resultObjectInfo));
		}

		// Get Evaluate Object for the Translation Y coordinate...
		evaluateObjectInfo.SubType	   = SvPb::SVEvaluateTranslationYObjectType;
		m_pEvaluateTranslationY = dynamic_cast<SvOp::SVEvaluateClass*>(m_pTool->getFirstObject(evaluateObjectInfo));
		if( m_pEvaluateTranslationY )
		{
			// Get Evaluate Result Object for the Translation Y coordinate...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateTranslationYResultObjectGuid;
			m_pTranslationYResult = dynamic_cast<SvVol::SVDoubleValueObjectClass*>(m_pEvaluateTranslationY->getFirstObject(resultObjectInfo));
		}

		// Get Evaluate Object for Translation X Coordinate...
		evaluateObjectInfo.SubType = SvPb::SVEvaluateRotationXObjectType;
		m_pEvaluateRotationX = dynamic_cast<SvOp::SVEvaluateClass*>(m_pTool->getFirstObject(evaluateObjectInfo));
		if( m_pEvaluateRotationX )
		{
			// Get Evaluate Result Object for the Rotation X coordinate...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateRotationXResultObjectGuid;
			m_pRotationXResult = dynamic_cast<SvVol::SVDoubleValueObjectClass*>(m_pEvaluateRotationX->getFirstObject(resultObjectInfo));
		}

		// Get Evaluate Object for the Rotation Y coordinate...
		evaluateObjectInfo.SubType	   = SvPb::SVEvaluateRotationYObjectType;
		m_pEvaluateRotationY = dynamic_cast<SvOp::SVEvaluateClass*>(m_pTool->getFirstObject(evaluateObjectInfo));
		if( m_pEvaluateRotationY )
		{
			// Get Evaluate Result Object for the Rotation Y coordinate...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateRotationYResultObjectGuid;
			m_pRotationYResult = dynamic_cast<SvVol::SVDoubleValueObjectClass*>(m_pEvaluateRotationY->getFirstObject(resultObjectInfo));
		}

		// Get Evaluate Object for the Rotation Angle...
		evaluateObjectInfo.SubType	   = SvPb::SVEvaluateRotationAngleObjectType;
		m_pEvaluateRotationAngle = dynamic_cast<SvOp::SVEvaluateClass*>(m_pTool->getFirstObject(evaluateObjectInfo));
		if( m_pEvaluateRotationAngle )
		{
			// Get Evaluate Result Object for the Rotation Angle...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateRotationAngleResultObjectGuid;
			m_pRotationAngleResult = dynamic_cast<SvVol::SVDoubleValueObjectClass*>(m_pEvaluateRotationAngle->getFirstObject(resultObjectInfo));
		}

		// Get Rotation enabled...
		SvDef::SVObjectTypeInfoStruct objectInfo;
		objectInfo.ObjectType = SvPb::SVValueObjectType;
		objectInfo.SubType = SvPb::SVBoolValueObjectType;
		objectInfo.EmbeddedID = SVPerformRotationObjectGuid;
		m_pPerformRotation = dynamic_cast<SvVol::SVBoolValueObjectClass*>(m_pTool->getFirstObject(objectInfo));

		// get Translation Enabled
		objectInfo.EmbeddedID = SVPerformTranslationObjectGuid;
		m_pPerformTranslation = dynamic_cast<SvVol::SVBoolValueObjectClass*>(m_pTool->getFirstObject(objectInfo));

		// Get SVImageTransformClass Object...
		SvDef::SVObjectTypeInfoStruct transformObjectInfo;
		transformObjectInfo.ObjectType = SvPb::SVTransformObjectType;
		transformObjectInfo.SubType = SvPb::SVImageTransformObjectType;

		m_pImageTransform = dynamic_cast<SvOp::SVImageTransformClass*>(m_pTool->getFirstObject(transformObjectInfo));
		if( m_pImageTransform )
		{
			// Get learned Objects...
			SvDef::SVObjectTypeInfoStruct learnedObjectInfo;
			learnedObjectInfo.ObjectType = SvPb::SVValueObjectType;
			learnedObjectInfo.SubType = SvPb::SVDoubleValueObjectType;

			// Get learned Translation X Object...
			learnedObjectInfo.EmbeddedID = SVLearnedTranslationXObjectGuid;
			m_pLearnedTranslationX = dynamic_cast<SvVol::SVDoubleValueObjectClass*>(m_pImageTransform->getFirstObject(learnedObjectInfo));

			// Get learned Translation Y Object...
			learnedObjectInfo.EmbeddedID = SVLearnedTranslationYObjectGuid;
			m_pLearnedTranslationY = dynamic_cast<SvVol::SVDoubleValueObjectClass*>(m_pImageTransform->getFirstObject(learnedObjectInfo));

			// Get learned Rotation X Object...
			learnedObjectInfo.EmbeddedID = SVLearnedRotationXObjectGuid;
			m_pLearnedRotationX = dynamic_cast<SvVol::SVDoubleValueObjectClass*>(m_pImageTransform->getFirstObject(learnedObjectInfo));

			// Get learned Translation Y Object...
			learnedObjectInfo.EmbeddedID = SVLearnedRotationYObjectGuid;
			m_pLearnedRotationY = dynamic_cast<SvVol::SVDoubleValueObjectClass*>(m_pImageTransform->getFirstObject(learnedObjectInfo));

			// Get learned Rotation Angle Object...
			learnedObjectInfo.EmbeddedID = SVLearnedRotationAngleObjectGuid;
			m_pLearnedRotationAngle = dynamic_cast<SvVol::SVDoubleValueObjectClass*>(m_pImageTransform->getFirstObject(learnedObjectInfo));
		}


		UpdateData( FALSE );
	
		// Check...
		if( m_pPerformTranslation && m_pPerformRotation &&
			m_pEvaluateTranslationX && m_pTranslationXResult &&
			m_pEvaluateTranslationY && m_pTranslationYResult &&
			m_pEvaluateRotationX && m_pRotationXResult &&
			m_pEvaluateRotationY && m_pRotationYResult &&
			m_pEvaluateRotationAngle && m_pRotationAngleResult &&
			m_pImageTransform && m_pLearnedTranslationX && m_pLearnedTranslationY && 
			m_pLearnedRotationX && m_pLearnedRotationY && m_pLearnedRotationAngle
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

	if( m_pTranslationXResult && m_pTranslationYResult &&
		m_pRotationXResult && m_pRotationYResult && 
		m_pRotationAngleResult &&
		m_pLearnedTranslationX && m_pLearnedTranslationY && 
		m_pLearnedRotationX && m_pLearnedRotationY && m_pLearnedRotationAngle
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
	std::string Value;
	// refresh Learned Translation X settings...
	if( nullptr != m_pLearnedTranslationX )
	{
		m_pLearnedTranslationX->getValue(	Value );
		m_LearnedTranslationXValue = Value.c_str();
	}
	else
	{
		m_LearnedTranslationXValue = _T("");
	}
	
	// refresh Learned Translation Y settings...
	if( nullptr != m_pLearnedTranslationY )
	{
		m_pLearnedTranslationY->getValue(	Value );
		m_LearnedTranslationYValue = Value.c_str();
	}
	else
	{
		m_LearnedTranslationYValue = _T("");
	}

	// refresh Learned Rotation X settings...
	if( nullptr != m_pLearnedRotationX )
	{
		m_pLearnedRotationX->getValue( Value );
		m_LearnedRotationXValue = Value.c_str();
	}
	else
	{
		m_LearnedRotationXValue = _T("");
	}
	
	// refresh Learned Rotation Y settings...
	if( nullptr != m_pLearnedRotationY )
	{
		m_pLearnedRotationY->getValue( Value );
		m_LearnedRotationYValue = Value.c_str();
	}
	else
	{
		m_LearnedRotationYValue = _T("");
	}

	// refresh Learned Rotation Angle settings...
	if( nullptr != m_pLearnedRotationAngle )
	{
		m_pLearnedRotationAngle->getValue( Value );

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

