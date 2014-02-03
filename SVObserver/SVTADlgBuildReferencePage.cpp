//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogBuildReferencePage
//* .File Name       : $Workfile:   SVTADlgBuildReferencePage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Feb 2014 12:09:00  $
//******************************************************************************

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// General Include File(s)
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "SVTADlgBuildReferencePage.h"

////////////////////////////////////////////////////////////////////////////////
// Other Necessary Include File(s) - Module Link(s)
////////////////////////////////////////////////////////////////////////////////

#include "SVEvaluateX.h"
#include "SVEvaluateY.h"
#include "SVEvaluateBeta.h"
#include "SVFormulaEditorSheet.h"
#include "SVIPDoc.h"
#include "SVRotateReference.h"
#include "SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"

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
//* Class Name : SVToolAdjustmentDialogBuildReferencePageClass
//* Note(s)    : Property Page
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

SVToolAdjustmentDialogBuildReferencePageClass::SVToolAdjustmentDialogBuildReferencePageClass( SVToolAdjustmentDialogSheetClass* Parent )
	: CPropertyPage(SVToolAdjustmentDialogBuildReferencePageClass::IDD)
{
	//{{AFX_DATA_INIT(SVToolAdjustmentDialogBuildReferencePageClass)
	StrThetaValue = _T("");
	StrXValue = _T("");
	StrYValue = _T("");
	bThetaEnabled = FALSE;
	bXEnabled = FALSE;
	bYEnabled = FALSE;
	//}}AFX_DATA_INIT

	pParentDialog	= Parent;
	pTool			= NULL;

	pEvaluateX		= NULL;
	pXEnabled		= NULL;
	pXResult		= NULL;

	pEvaluateY		= NULL;
	pYEnabled		= NULL;
	pYResult		= NULL;

	pEvaluateBeta	= NULL;
	pYEnabled		= NULL;
	pYResult		= NULL;

	pRotate			= NULL;
	pLearnedX		= NULL;
	pLearnedY		= NULL;
	pLearnedBeta	= NULL;

	m_xResult = 0.0;
	m_yResult = 0.0;
	m_betaResult = 0.0;
}

HRESULT SVToolAdjustmentDialogBuildReferencePageClass::SetInspectionData()
{
	HRESULT l_hrOk = S_OK;

	if( pTool != NULL )
	{
		bool l_bUpdate = false;

		UpdateData( TRUE ); // get data from dialog

		if( pXEnabled )
		{
			l_bUpdate = true;

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( pXEnabled, bXEnabled );
			}

			if( bXEnabled && l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( pLearnedX, m_xResult );
			}
		}

		if( pYEnabled )
		{
			l_bUpdate = true;

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( pYEnabled, bYEnabled );
			}

			if( bYEnabled && l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( pLearnedY, m_yResult );
			}
		}

		if( pBetaEnabled )
		{
			l_bUpdate = true;

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( pBetaEnabled, bThetaEnabled );
			}

			if( bThetaEnabled && l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( pLearnedBeta, m_betaResult );
			}
		}

		if( l_bUpdate )
		{
			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequestMarker();
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = RunOnce( pTool );
			}
		}

		UpdateData( FALSE );
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}


void SVToolAdjustmentDialogBuildReferencePageClass::refresh()
{
	if( pTool )
	{
		CWnd* pWnd = NULL;

		SetInspectionData();

		// refresh X setiings...
		if( pXResult )
			pXResult->GetValue(	StrXValue );
		else
			StrXValue = _T( "" );

		if( pXEnabled )
			pXEnabled->GetValue( bXEnabled );
		else
		{
			if( pWnd = GetDlgItem( IDC_X_ENABLED_CHECK ) )
				pWnd->EnableWindow( FALSE );

			bXEnabled = FALSE;
		}

		// refresh Y setiings...
		if( pYResult )
			pYResult->GetValue(	StrYValue );
		else
			StrYValue = _T( "" );

		if( pYEnabled )
			pYEnabled->GetValue( bYEnabled );
		else
		{
			if( pWnd = GetDlgItem( IDC_Y_ENABLED_CHECK ) )
				pWnd->EnableWindow( FALSE );

			bYEnabled = FALSE;
		}

		// refresh Beta ( Theta ) setiings...
		if( pBetaResult )
			pBetaResult->GetValue( StrThetaValue );
		else
			StrThetaValue = _T( "" );

		if( pBetaEnabled )
			pBetaEnabled->GetValue( bThetaEnabled );
		else
		{
			if( pWnd = GetDlgItem( IDC_THETA_ENABLED_CHECK ) )
				pWnd->EnableWindow( FALSE );

			bThetaEnabled = FALSE;
		}

		UpdateData( FALSE ); // set data to dialog
	}

}

void SVToolAdjustmentDialogBuildReferencePageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVToolAdjustmentDialogBuildReferencePageClass)
	DDX_Text(pDX, IDC_THETA_EDIT, StrThetaValue);
	DDX_Text(pDX, IDC_X_EDIT, StrXValue);
	DDX_Text(pDX, IDC_Y_EDIT, StrYValue);
	DDX_Check(pDX, IDC_THETA_ENABLED_CHECK, bThetaEnabled);
	DDX_Check(pDX, IDC_X_ENABLED_CHECK, bXEnabled);
	DDX_Check(pDX, IDC_Y_ENABLED_CHECK, bYEnabled);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogBuildReferencePageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVToolAdjustmentDialogBuildReferencePageClass)
	ON_BN_CLICKED(IDC_LEARN_BUTTON, OnLearnButton)
	ON_BN_CLICKED(IDC_THETA_FORMULA_BUTTON, OnThetaFormulaButton)
	ON_BN_CLICKED(IDC_X_FORMULA_BUTTON, OnXFormulaButton)
	ON_BN_CLICKED(IDC_Y_FORMULA_BUTTON, OnYFormulaButton)
	ON_BN_CLICKED(IDC_THETA_ENABLED_CHECK, OnThetaEnabledCheck)
	ON_BN_CLICKED(IDC_X_ENABLED_CHECK, OnXEnabledCheck)
	ON_BN_CLICKED(IDC_Y_ENABLED_CHECK, OnYEnabledCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVToolAdjustmentDialogBuildReferencePageClass message handlers

BOOL SVToolAdjustmentDialogBuildReferencePageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// Set the font of the angle static text
	CWnd* pWnd = GetDlgItem( IDC_STATIC2 );
	
	thetaFont.CreatePointFont( 100,"Symbol");
		
	pWnd->SetFont( &thetaFont );

	pWnd->SetWindowText("q:");

	if( pParentDialog && ( pTool = pParentDialog->GetTool() ) )
	{
		SetTaskObject( pTool );

		// Get EvaluateX Object...
		SVObjectTypeInfoStruct evaluateXObjectInfo;
		evaluateXObjectInfo.ObjectType = SVMathContainerObjectType;
		evaluateXObjectInfo.SubType	   = SVEvaluateXObjectType;
		pEvaluateX = ( SVEvaluateXClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&evaluateXObjectInfo) );
		if( pEvaluateX )
		{
			// Get EvaluateX Enabled Object...
			SVObjectTypeInfoStruct enableObjectInfo;
			enableObjectInfo.ObjectType = SVBoolValueObjectType;
			enableObjectInfo.EmbeddedID = SVOutputEvaluateXEnabledObjectGuid;
			pXEnabled = ( SVBoolValueObjectClass* ) ::SVSendMessage( pEvaluateX, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&enableObjectInfo) );

			// Get EvaluateX Result Object...
			SVObjectTypeInfoStruct resultObjectInfo;
			resultObjectInfo.ObjectType = SVDoubleValueObjectType;
			resultObjectInfo.EmbeddedID = SVOutputEvaluateXResultObjectGuid;
			pXResult = ( SVDoubleValueObjectClass* ) ::SVSendMessage( pEvaluateX, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&resultObjectInfo) );
		}

		// Get EvaluateY Object...
		SVObjectTypeInfoStruct evaluateYObjectInfo;
		evaluateYObjectInfo.ObjectType = SVMathContainerObjectType;
		evaluateYObjectInfo.SubType	   = SVEvaluateYObjectType;
		pEvaluateY = ( SVEvaluateYClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&evaluateYObjectInfo) );
		if( pEvaluateY )
		{
			// Get EvaluateY Enabled Object...
			SVObjectTypeInfoStruct enableObjectInfo;
			enableObjectInfo.ObjectType = SVBoolValueObjectType;
			enableObjectInfo.EmbeddedID = SVOutputEvaluateYEnabledObjectGuid;
			pYEnabled = ( SVBoolValueObjectClass* ) ::SVSendMessage( pEvaluateY, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&enableObjectInfo) );

			// Get EvaluateY Result Object...
			SVObjectTypeInfoStruct resultObjectInfo;
			resultObjectInfo.ObjectType = SVDoubleValueObjectType;
			resultObjectInfo.EmbeddedID = SVOutputEvaluateYResultObjectGuid;
			pYResult = ( SVDoubleValueObjectClass* ) ::SVSendMessage( pEvaluateY, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&resultObjectInfo) );
		}

		// Get EvaluateBeta Object...
		SVObjectTypeInfoStruct evaluateBetaObjectInfo;
		evaluateBetaObjectInfo.ObjectType = SVMathContainerObjectType;
		evaluateBetaObjectInfo.SubType	   = SVEvaluateBetaObjectType;
		pEvaluateBeta = ( SVEvaluateBetaClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&evaluateBetaObjectInfo) );
		if( pEvaluateBeta )
		{
			// Get EvaluateBeta Enabled Object...
			SVObjectTypeInfoStruct enableObjectInfo;
			enableObjectInfo.ObjectType = SVBoolValueObjectType;
			enableObjectInfo.EmbeddedID = SVOutputEvaluateBetaEnabledObjectGuid;
			pBetaEnabled = ( SVBoolValueObjectClass* ) ::SVSendMessage( pEvaluateBeta, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&enableObjectInfo) );

			// Get EvaluateBeta Result Object...
			SVObjectTypeInfoStruct resultObjectInfo;
			resultObjectInfo.ObjectType = SVDoubleValueObjectType;
			resultObjectInfo.EmbeddedID = SVOutputEvaluateBetaResultObjectGuid;
			pBetaResult = ( SVDoubleValueObjectClass* ) ::SVSendMessage( pEvaluateBeta, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&resultObjectInfo) );
		}

		// Get RotateReference Object...
		SVObjectTypeInfoStruct rotateObjectInfo;
		rotateObjectInfo.ObjectType = SVRotateReferenceObjectType;
		pRotate = ( SVRotateReferenceClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&rotateObjectInfo) );
		if( pRotate )
		{
			// Get learned X Object...
			SVObjectTypeInfoStruct learnedXObjectInfo;
			learnedXObjectInfo.ObjectType = SVDoubleValueObjectType;
			learnedXObjectInfo.EmbeddedID = SVOutputLearnedXObjectGuid;
			pLearnedX = ( SVDoubleValueObjectClass* ) ::SVSendMessage( pRotate, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&learnedXObjectInfo) );

			// Get learned Y Object...
			SVObjectTypeInfoStruct learnedYObjectInfo;
			learnedYObjectInfo.ObjectType = SVDoubleValueObjectType;
			learnedYObjectInfo.EmbeddedID = SVOutputLearnedYObjectGuid;
			pLearnedY = ( SVDoubleValueObjectClass* ) ::SVSendMessage( pRotate, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&learnedYObjectInfo) );

			// Get learned Beta Object...
			SVObjectTypeInfoStruct learnedBetaObjectInfo;
			learnedBetaObjectInfo.ObjectType = SVDoubleValueObjectType;
			learnedBetaObjectInfo.EmbeddedID = SVOutputLearnedBetaObjectGuid;
			pLearnedBeta = ( SVDoubleValueObjectClass* ) ::SVSendMessage( pRotate, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&learnedBetaObjectInfo) );
		}


		UpdateData( FALSE );
	
		// Check...
		if( pEvaluateX && pXEnabled && pXResult &&
			pEvaluateY && pYEnabled && pYResult &&
			pEvaluateBeta && pBetaEnabled && pBetaResult &&
			pRotate && pLearnedX && pLearnedY && pLearnedBeta
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

void SVToolAdjustmentDialogBuildReferencePageClass::OnLearnButton() 
{
	UpdateData( TRUE );

	if( pXResult && pYResult && pBetaResult )
	{
		if( S_OK != pXResult->GetValue( m_xResult ) )
			return;

		if( S_OK != pYResult->GetValue( m_yResult ) )
			return;

		if( S_OK != pBetaResult->GetValue( m_betaResult ) )
			return;

		refresh();
	}
}

void SVToolAdjustmentDialogBuildReferencePageClass::OnThetaFormulaButton() 
{
	if( pEvaluateBeta )
	{
		CString strCaption = pEvaluateBeta->GetName();
		strCaption += _T( " Formula" );
		SVFormulaEditorSheetClass dlg( strCaption );
		dlg.SetTaskObject( pEvaluateBeta );

		dlg.DoModal();

		refresh();
	}
}

void SVToolAdjustmentDialogBuildReferencePageClass::OnXFormulaButton() 
{
	if( pEvaluateX )
	{
		CString strCaption = pEvaluateX->GetName();
		strCaption += _T( " Formula" );
		SVFormulaEditorSheetClass dlg( strCaption );
		dlg.SetTaskObject( pEvaluateX );

		dlg.DoModal();

		refresh();
	}
}

void SVToolAdjustmentDialogBuildReferencePageClass::OnYFormulaButton() 
{
	if( pEvaluateY )
	{
		CString strCaption = pEvaluateY->GetName();
		strCaption += _T( " Formula" );
		SVFormulaEditorSheetClass dlg( strCaption );
		dlg.SetTaskObject( pEvaluateY );

		dlg.DoModal();

		refresh();
	}
}

void SVToolAdjustmentDialogBuildReferencePageClass::OnThetaEnabledCheck() 
{
	refresh();
}

void SVToolAdjustmentDialogBuildReferencePageClass::OnXEnabledCheck() 
{
	refresh();
}

void SVToolAdjustmentDialogBuildReferencePageClass::OnYEnabledCheck() 
{
	refresh();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTADlgBuildReferencePage.cpp_v  $
 * 
 *    Rev 1.1   01 Feb 2014 12:09:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:13:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   02 Jul 2012 17:40:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   16 Aug 2005 09:45:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed un-necessary casting from AddInputRequest method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   11 Aug 2005 13:28:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added UpdateData( FALSE ) to OnInitDialog method to make sure the inspection data was pushed to the dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   09 Aug 2005 08:15:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   21 Jun 2005 08:32:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * value object functions now use HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   17 Feb 2005 15:49:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modifications to support new extents.  Also use reset objects rather than recreating them.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   22 Apr 2003 15:43:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   20 Nov 2002 13:34:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Add ResultDataIndex and ResultImageIndex
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   19 Apr 2001 19:23:40   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   19 Apr 2001 15:33:40   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:40:22   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Oct 1999 14:00:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added logic in InitDialog to set the symbol font and text for the theta static text control.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 14 1999 15:13:04   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Provide Build Reference Dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
