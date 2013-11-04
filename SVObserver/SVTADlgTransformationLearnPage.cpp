//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogTransformationLearnPage
//* .File Name       : $Workfile:   SVTADlgTransformationLearnPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:23:24  $
//******************************************************************************

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// General Include File(s)
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SVTADlgTransformationLearnPage.h"

#include "SVEvaluate.h"
#include "SVImageTransform.h"
#include "SVIPDoc.h"
#include "SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"

////////////////////////////////////////////////////////////////////////////////
// Prototyping
////////////////////////////////////////////////////////////////////////////////


//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVToolAdjustmentDialogTransformationLearnPageClass
//* Note(s)    : Property Page
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

SVToolAdjustmentDialogTransformationLearnPageClass::SVToolAdjustmentDialogTransformationLearnPageClass( SVToolAdjustmentDialogSheetClass* Parent )
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
	pTool			= NULL;

	pPerformTranslation		= NULL;

	pEvaluateTranslationX	= NULL;
	pTranslationXResult		= NULL;

	pEvaluateTranslationY	= NULL;
	pTranslationYResult		= NULL;

	pPerformRotation		= NULL;

	pEvaluateRotationX		= NULL;
	pRotationXResult		= NULL;

	pEvaluateRotationY		= NULL;
	pRotationYResult		= NULL;

	pEvaluateRotationAngle	= NULL;
	pRotationAngleResult	= NULL;

	pImageTransform			= NULL;
	pLearnedTranslationX	= NULL;
	pLearnedTranslationY	= NULL;
	pLearnedRotationX		= NULL;
	pLearnedRotationY		= NULL;
	pLearnedRotationAngle	= NULL;
}

HRESULT SVToolAdjustmentDialogTransformationLearnPageClass::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	if( pTool )
	{
		UpdateData( TRUE ); // get data from dialog

		l_hrOk = AddInputRequest( pLearnedTranslationX, m_translationXValue );

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( pLearnedTranslationY, m_translationYValue );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( pLearnedRotationX, m_rotationXValue );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( pLearnedRotationY, m_rotationYValue );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( pLearnedRotationAngle, m_rotationAngleValue );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequestMarker();
		}

		if( l_hrOk == S_OK )
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
		CWnd* pWnd = NULL;


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
		pEvaluateTranslationX = ( SVEvaluateClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &evaluateObjectInfo );
		if( pEvaluateTranslationX )
		{
			// Get Evaluate Result Object for the Translation X coordinate...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateTranslationXResultObjectGuid;
			pTranslationXResult = ( SVDoubleValueObjectClass* ) ::SVSendMessage( pEvaluateTranslationX, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &resultObjectInfo );
		}

		// Get Evaluate Object for the Translation Y coordinate...
		evaluateObjectInfo.SubType	   = SVEvaluateTranslationYObjectType;
		pEvaluateTranslationY = ( SVEvaluateClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &evaluateObjectInfo );
		if( pEvaluateTranslationY )
		{
			// Get Evaluate Result Object for the Translation Y coordinate...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateTranslationYResultObjectGuid;
			pTranslationYResult = ( SVDoubleValueObjectClass* ) ::SVSendMessage( pEvaluateTranslationY, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &resultObjectInfo );
		}

		// Get Evaluate Object for Translation X Coordinate...
		evaluateObjectInfo.SubType = SVEvaluateRotationXObjectType;
		pEvaluateRotationX = ( SVEvaluateClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &evaluateObjectInfo );
		if( pEvaluateRotationX )
		{
			// Get Evaluate Result Object for the Rotation X coordinate...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateRotationXResultObjectGuid;
			pRotationXResult = ( SVDoubleValueObjectClass* ) ::SVSendMessage( pEvaluateRotationX, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &resultObjectInfo );
		}

		// Get Evaluate Object for the Rotation Y coordinate...
		evaluateObjectInfo.SubType	   = SVEvaluateRotationYObjectType;
		pEvaluateRotationY = ( SVEvaluateClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &evaluateObjectInfo );
		if( pEvaluateRotationY )
		{
			// Get Evaluate Result Object for the Rotation Y coordinate...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateRotationYResultObjectGuid;
			pRotationYResult = ( SVDoubleValueObjectClass* ) ::SVSendMessage( pEvaluateRotationY, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &resultObjectInfo );
		}

		// Get Evaluate Object for the Rotation Angle...
		evaluateObjectInfo.SubType	   = SVEvaluateRotationAngleObjectType;
		pEvaluateRotationAngle = ( SVEvaluateClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &evaluateObjectInfo );
		if( pEvaluateRotationAngle )
		{
			// Get Evaluate Result Object for the Rotation Angle...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateRotationAngleResultObjectGuid;
			pRotationAngleResult = ( SVDoubleValueObjectClass* ) ::SVSendMessage( pEvaluateRotationAngle, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &resultObjectInfo );
		}

		// Get Rotation enabled...
		SVObjectTypeInfoStruct objectInfo;
		objectInfo.ObjectType = SVBoolValueObjectType;
		objectInfo.EmbeddedID = SVPerformRotationObjectGuid;
		pPerformRotation = ( SVBoolValueObjectClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &objectInfo );

		// get Translation Enabled
		objectInfo.EmbeddedID = SVPerformTranslationObjectGuid;
		pPerformTranslation = ( SVBoolValueObjectClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &objectInfo );

		// Get SVImageTransformClass Object...
		SVObjectTypeInfoStruct transformObjectInfo;
		transformObjectInfo.ObjectType = SVTransformObjectType;
		transformObjectInfo.SubType = SVImageTransformObjectType;

		pImageTransform = ( SVImageTransformClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &transformObjectInfo );
		if( pImageTransform )
		{
			// Get learned Objects...
			SVObjectTypeInfoStruct learnedObjectInfo;
			learnedObjectInfo.ObjectType = SVDoubleValueObjectType;

			// Get learned Translation X Object...
			learnedObjectInfo.EmbeddedID = SVLearnedTranslationXObjectGuid;
			pLearnedTranslationX = ( SVDoubleValueObjectClass* ) ::SVSendMessage( pImageTransform, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &learnedObjectInfo );

			// Get learned Translation Y Object...
			learnedObjectInfo.EmbeddedID = SVLearnedTranslationYObjectGuid;
			pLearnedTranslationY = ( SVDoubleValueObjectClass* ) ::SVSendMessage( pImageTransform, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &learnedObjectInfo );

			// Get learned Rotation X Object...
			learnedObjectInfo.EmbeddedID = SVLearnedRotationXObjectGuid;
			pLearnedRotationX = ( SVDoubleValueObjectClass* ) ::SVSendMessage( pImageTransform, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &learnedObjectInfo );

			// Get learned Translation Y Object...
			learnedObjectInfo.EmbeddedID = SVLearnedRotationYObjectGuid;
			pLearnedRotationY = ( SVDoubleValueObjectClass* ) ::SVSendMessage( pImageTransform, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &learnedObjectInfo );

			// Get learned Rotation Angle Object...
			learnedObjectInfo.EmbeddedID = SVLearnedRotationAngleObjectGuid;
			pLearnedRotationAngle = ( SVDoubleValueObjectClass* ) ::SVSendMessage( pImageTransform, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &learnedObjectInfo );
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTADlgTransformationLearnPage.cpp_v  $
 * 
 *    Rev 1.0   24 Apr 2013 11:23:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   02 Jul 2012 17:42:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   13 Oct 2005 15:43:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed SetInspectionData from Refresh().
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   11 Aug 2005 13:28:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added UpdateData( FALSE ) to OnInitDialog method to make sure the inspection data was pushed to the dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   09 Aug 2005 08:25:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Feb 2005 16:04:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modifications to support new reset / create methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Apr 2003 16:21:20   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Nov 2002 13:48:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Add ResultDataIndex  and ResultImageIndex
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Apr 2001 19:28:54   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Apr 2001 15:46:30   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   21 Feb 2000 09:33:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  104
 * SCR Title:  New Build Reference Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin.
 * New Dialog Class to support the Learn feature of the
 * Transformation Tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
