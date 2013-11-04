//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgRotaionPage
//* .File Name       : $Workfile:   SVTADlgRotationPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:17:40  $
//******************************************************************************

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// General Include File(s)
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SVTADlgRotationPage.h"

#include "SVFormulaEditorSheet.h"
#include "SVEvaluate.h"
#include "SVIPDoc.h"
#include "SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVToolSet.h"

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
//* Class Name : SVToolAdjustmentDialogRotationPageClass
//* Note(s)    : Property Page
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

SVToolAdjustmentDialogRotationPageClass::SVToolAdjustmentDialogRotationPageClass( SVToolAdjustmentDialogSheetClass* Parent )
	: CPropertyPage(SVToolAdjustmentDialogRotationPageClass::IDD)
{
	//{{AFX_DATA_INIT(SVToolAdjustmentDialogRotationPageClass)
	StrRotationAngleValue = _T("");
	StrRotationXValue = _T("");
	StrRotationYValue = _T("");
	m_performRotation = FALSE;
	//}}AFX_DATA_INIT

	pParentDialog	= Parent;
	pTool			= NULL;

	pPerformRotation		= NULL;

	pEvaluateRotationX		= NULL;
	pRotationXResult		= NULL;

	pEvaluateRotationY		= NULL;
	pRotationYResult		= NULL;

	pEvaluateRotationAngle	= NULL;
	pRotationAngleResult	= NULL;
}


HRESULT SVToolAdjustmentDialogRotationPageClass::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	if( pTool )
	{
		UpdateData( TRUE ); // get data from dialog

		l_hrOk = AddInputRequest( pPerformRotation, m_performRotation );

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

void SVToolAdjustmentDialogRotationPageClass::refresh()
{
	if( pTool )
	{
		CWnd* pWnd = NULL;

		SetInspectionData();

		// refresh X settings...
		if( pRotationXResult )
			pRotationXResult->GetValue(	StrRotationXValue );
		else
			StrRotationXValue = _T( "" );

		// refresh Y settings...
		if( pRotationYResult )
			pRotationYResult->GetValue(	StrRotationYValue );
		else
			StrRotationYValue = _T( "" );

		// refresh Angle setiings...
		if( pRotationAngleResult )
			pRotationAngleResult->GetValue( StrRotationAngleValue );
		else
			StrRotationAngleValue = _T( "" );

		UpdateData( FALSE ); // set data to dialog
	}

}

void SVToolAdjustmentDialogRotationPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVToolAdjustmentDialogRotationPageClass)
	DDX_Text(pDX, IDC_ROTATION_ANGLE_EDIT, StrRotationAngleValue);
	DDX_Text(pDX, IDC_ROTATION_X_EDIT, StrRotationXValue);
	DDX_Text(pDX, IDC_ROTATION_Y_EDIT, StrRotationYValue);
	DDX_Check(pDX, IDC_PERFORM_ROTATION, m_performRotation);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogRotationPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVToolAdjustmentDialogRotationPageClass)
	ON_BN_CLICKED(IDC_ROTATION_ANGLE_FORMULA_BUTTON, OnAngleFormulaButton)
	ON_BN_CLICKED(IDC_ROTATION_X_FORMULA_BUTTON, OnXFormulaButton)
	ON_BN_CLICKED(IDC_ROTATION_Y_FORMULA_BUTTON, OnYFormulaButton)
	ON_BN_CLICKED(IDC_PERFORM_ROTATION, OnPerformRotation)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

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

	if( pParentDialog && ( pTool = pParentDialog->GetTool() ) )
	{
		SetTaskObject( pTool );

		// Get Evaluate Object for the X coordinate...
		SVObjectTypeInfoStruct evaluateObjectInfo;
		evaluateObjectInfo.ObjectType = SVMathContainerObjectType;
	
		// Set up the requestor for the evaluate result object
		SVObjectTypeInfoStruct resultObjectInfo;
		resultObjectInfo.ObjectType = SVDoubleValueObjectType;
		
		// Get Evaluate Object for the X coordinate...
		evaluateObjectInfo.SubType = SVEvaluateRotationXObjectType;
		pEvaluateRotationX = ( SVEvaluateClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &evaluateObjectInfo );
		if( pEvaluateRotationX )
		{
			// Get Evaluate Result Object for the X coordinate...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateRotationXResultObjectGuid;
			pRotationXResult = ( SVDoubleValueObjectClass* ) ::SVSendMessage( pEvaluateRotationX, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &resultObjectInfo );
		}

		// Get Evaluate Object for the Y coordinate...
		evaluateObjectInfo.SubType = SVEvaluateRotationYObjectType;
		pEvaluateRotationY = ( SVEvaluateClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &evaluateObjectInfo );
		if( pEvaluateRotationY )
		{
			// Get Evaluate Result Object for the Y coordinate...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateRotationYResultObjectGuid;
			pRotationYResult = ( SVDoubleValueObjectClass* ) ::SVSendMessage( pEvaluateRotationY, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &resultObjectInfo );
		}

		// Get Evaluate Object for the Angle...
		evaluateObjectInfo.SubType	   = SVEvaluateRotationAngleObjectType;
		pEvaluateRotationAngle = ( SVEvaluateClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &evaluateObjectInfo );
		if( pEvaluateRotationAngle )
		{
			// Get Evaluate Result Object for the Angle...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateRotationAngleResultObjectGuid;
			pRotationAngleResult = ( SVDoubleValueObjectClass* ) ::SVSendMessage( pEvaluateRotationAngle, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &resultObjectInfo );
		}
		
		// Get Rotation enabled...
		SVObjectTypeInfoStruct objectInfo;
		objectInfo.ObjectType = SVBoolValueObjectType;
		objectInfo.EmbeddedID = SVPerformRotationObjectGuid;
		pPerformRotation = ( SVBoolValueObjectClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &objectInfo );

		UpdateData( FALSE );
	
		// Check...
		if( pEvaluateRotationX && pRotationXResult &&
			pEvaluateRotationY && pRotationYResult &&
			pEvaluateRotationAngle && pRotationAngleResult &&
			pPerformRotation)
		{
			// Get Inspection Data..
			pPerformRotation->GetValue( m_performRotation );
			UpdateData(FALSE);

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

void SVToolAdjustmentDialogRotationPageClass::OnAngleFormulaButton() 
{
	if( pEvaluateRotationAngle )
	{
		CString strCaption = pEvaluateRotationAngle->GetName();
		strCaption += _T( " Formula" );
		SVFormulaEditorSheetClass dlg( strCaption );
		dlg.SetTaskObject( pEvaluateRotationAngle );

		dlg.DoModal();

		refresh();
	}
}

void SVToolAdjustmentDialogRotationPageClass::OnXFormulaButton() 
{
	if( pEvaluateRotationX )
	{
		CString strCaption = pEvaluateRotationX->GetName();
		strCaption += _T( " Formula" );
		SVFormulaEditorSheetClass dlg( strCaption );
		dlg.SetTaskObject( pEvaluateRotationX );

		dlg.DoModal();

		refresh();
	}
}

void SVToolAdjustmentDialogRotationPageClass::OnYFormulaButton() 
{
	if( pEvaluateRotationY )
	{
		CString strCaption = pEvaluateRotationY->GetName();
		strCaption += _T( " Formula" );
		SVFormulaEditorSheetClass dlg( strCaption );
		dlg.SetTaskObject( pEvaluateRotationY );

		dlg.DoModal();

		refresh();
	}
}

void SVToolAdjustmentDialogRotationPageClass::OnPerformRotation() 
{
	refresh();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTADlgRotationPage.cpp_v  $
 * 
 *    Rev 1.0   24 Apr 2013 11:17:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   05 Dec 2012 11:59:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed items associated with SCR 198
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   01 Nov 2012 13:39:14   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  798
 * SCR Title:  Change Transformation Tool to have an option for translation only
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new method OnSetActive, changed method OnInitDialog for the new tanslation mode
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   02 Jul 2012 17:42:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   16 Aug 2005 09:45:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed un-necessary casting from AddInputRequest method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   11 Aug 2005 13:28:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added UpdateData( FALSE ) to OnInitDialog method to make sure the inspection data was pushed to the dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   10 Aug 2005 09:50:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  487
 * SCR Title:  Add new blob feature for source extents of blob box center
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Add Updatedata to OnInitDialog for performRotation
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   09 Aug 2005 08:25:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Feb 2005 16:04:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modifications to support new reset / create methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 16:21:02   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Nov 2002 13:47:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Add ResultDataIndex  and ResultImageIndex
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   21 Feb 2000 09:34:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  104
 * SCR Title:  New Build Reference Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin.
 * New Dialog Class to edit the Rotation parameters for the
 * Transformation Tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
