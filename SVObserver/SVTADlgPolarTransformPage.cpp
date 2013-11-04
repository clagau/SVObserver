//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgPolarTransformPage
//* .File Name       : $Workfile:   SVTADlgPolarTransformPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:16:44  $
//******************************************************************************

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// General Include File(s)
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SVTADlgPolarTransformPage.h"

#include "SVFormulaEditorSheet.h"
#include "SVEvaluate.h"
#include "SVIPDoc.h"
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
//* Class Name : SVToolAdjustmentDialogPolarTransformPageClass
//* Note(s)    : Property Page
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

SVToolAdjustmentDialogPolarTransformPageClass::SVToolAdjustmentDialogPolarTransformPageClass( SVToolAdjustmentDialogSheetClass* Parent )
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
	pTool			= NULL;

	pEvaluateCenterX	 = NULL;
	pCenterXResult		 = NULL;
	pEvaluateCenterY	 = NULL;
	pCenterYResult		 = NULL;
	pEvaluateStartRadius = NULL;
	pStartRadiusResult	 = NULL;
	pEvaluateEndRadius	 = NULL;
	pEndRadiusResult	 = NULL;
	pEvaluateStartAngle	 = NULL;
	pStartAngleResult	 = NULL;
	pEvaluateEndAngle	 = NULL;
	pEndAngleResult		 = NULL;

	pUseFormula			 = NULL;
	pInterpolationMode	 = NULL;
}

HRESULT SVToolAdjustmentDialogPolarTransformPageClass::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	if( pTool )
	{
		UpdateData( TRUE ); // get data from dialog

		l_hrOk = AddInputRequest( pUseFormula, bUseFormula );

		if( l_hrOk == S_OK )
		{
			int sel = interpolationComboCtrl.GetCurSel();
			if( sel >= 0 )
			{
				long lValue = ( long ) interpolationComboCtrl.GetItemData( sel );

				l_hrOk = AddInputRequest( pInterpolationMode, lValue );
			}
		}

		if( l_hrOk == S_OK )
		{
			int sel = m_AngularMethodCombo.GetCurSel();
			if( sel >= 0 )
			{
				long lValue = ( long ) m_AngularMethodCombo.GetItemData( sel );

				l_hrOk = AddInputRequest( m_pAngleMethod, lValue );
			}
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


void SVToolAdjustmentDialogPolarTransformPageClass::refresh()
{
	if( pTool )
	{
		CWnd* pWnd = NULL;

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
			if( pInterpolationMode->GetValue( strEnum ) == S_OK )
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
		pEvaluateCenterX = ( SVEvaluateClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &evaluateObjectInfo );
		if( pEvaluateCenterX )
		{
			// Get Center X Result...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateCenterXResultObjectGuid;
			pCenterXResult = ( SVDoubleValueObjectClass* ) ::SVSendMessage( pEvaluateCenterX, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &resultObjectInfo );
		}

		// Get Evaluate Center Y...
		evaluateObjectInfo.SubType = SVEvaluateCenterYObjectType;		
		pEvaluateCenterY = ( SVEvaluateClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &evaluateObjectInfo );
		if( pEvaluateCenterY )
		{
			// Get Center Y Result...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateCenterYResultObjectGuid;
			pCenterYResult = ( SVDoubleValueObjectClass* ) ::SVSendMessage( pEvaluateCenterY, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &resultObjectInfo );
		}

		// Get Evaluate Start Radius...
		evaluateObjectInfo.SubType = SVEvaluateStartRadiusObjectType;		
		pEvaluateStartRadius = ( SVEvaluateClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &evaluateObjectInfo );
		if( pEvaluateStartRadius )
		{
			// Get Start Radius Result...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateStartRadiusResultObjectGuid;
			pStartRadiusResult = ( SVDoubleValueObjectClass* ) ::SVSendMessage( pEvaluateStartRadius, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &resultObjectInfo );
		}

		// Get Evaluate End Radius...
		evaluateObjectInfo.SubType = SVEvaluateEndRadiusObjectType;		
		pEvaluateEndRadius = ( SVEvaluateClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &evaluateObjectInfo );
		if( pEvaluateEndRadius )
		{
			// Get End Radius Result...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateEndRadiusResultObjectGuid;
			pEndRadiusResult = ( SVDoubleValueObjectClass* ) ::SVSendMessage( pEvaluateEndRadius, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &resultObjectInfo );
		}

		// Get Evaluate Start Angle...
		evaluateObjectInfo.SubType = SVEvaluateStartAngleObjectType;		
		pEvaluateStartAngle = ( SVEvaluateClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &evaluateObjectInfo );
		if( pEvaluateStartAngle )
		{
			// Get Start Angle Result...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateStartAngleResultObjectGuid;
			pStartAngleResult = ( SVDoubleValueObjectClass* ) ::SVSendMessage( pEvaluateStartAngle, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &resultObjectInfo );
		}

		// Get Evaluate End Angle...
		evaluateObjectInfo.SubType = SVEvaluateEndAngleObjectType;		
		pEvaluateEndAngle = ( SVEvaluateClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &evaluateObjectInfo );
		if( pEvaluateEndAngle )
		{
			// Get End Angle Result...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateEndAngleResultObjectGuid;
			pEndAngleResult = ( SVDoubleValueObjectClass* ) ::SVSendMessage( pEvaluateEndAngle, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &resultObjectInfo );
		}

		// Get Use Formula...
		SVObjectTypeInfoStruct objectInfo;
		objectInfo.EmbeddedID = SVOutputUseFormulaObjectGuid;
		pUseFormula = ( SVBoolValueObjectClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &objectInfo );
		if( pUseFormula )
		{
			pUseFormula->GetValue( bUseFormula );
		}

		// Get Use Angle Method ...
		objectInfo.EmbeddedID = SVOutputAngularMethodObjectGuid;
		m_pAngleMethod = ( SVEnumerateValueObjectClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &objectInfo );
		if( m_pAngleMethod )
		{
			CString l_strEnumList;
			m_pAngleMethod->GetEnumTypes( l_strEnumList );
			m_AngularMethodCombo.SetEnumTypes( l_strEnumList );
		}

		// Get Interpolation Mode...
		objectInfo.EmbeddedID = SVOutputInterpolationModeObjectGuid;
		pInterpolationMode = ( SVEnumerateValueObjectClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &objectInfo );
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

		SVFormulaEditorSheetClass dlg( strCaption );
		dlg.SetTaskObject( pEvaluateCenterX );

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

		SVFormulaEditorSheetClass dlg( strCaption );
		dlg.SetTaskObject( pEvaluateCenterY );

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

		SVFormulaEditorSheetClass dlg( strCaption );
		dlg.SetTaskObject( pEvaluateStartRadius );

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

		SVFormulaEditorSheetClass dlg( strCaption );
		dlg.SetTaskObject( pEvaluateEndRadius );

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

		SVFormulaEditorSheetClass dlg( strCaption );
		dlg.SetTaskObject( pEvaluateStartAngle );

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

		SVFormulaEditorSheetClass dlg( strCaption );
		dlg.SetTaskObject( pEvaluateEndAngle );

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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTADlgPolarTransformPage.cpp_v  $
 * 
 *    Rev 1.0   24 Apr 2013 11:16:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   02 Jul 2012 17:42:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   08 Dec 2010 13:39:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   24 Aug 2005 13:14:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed Use Formula Check Box.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   16 Aug 2005 09:45:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed un-necessary casting from AddInputRequest method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   11 Aug 2005 13:28:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added UpdateData( FALSE ) to OnInitDialog method to make sure the inspection data was pushed to the dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   09 Aug 2005 08:18:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   21 Jun 2005 08:32:10   ebeyeler
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
 *    Rev 1.6   24 Feb 2005 10:23:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  449
 * SCR Title:  Modify Polar Un-Wrap Tool
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Made changes to use a Angular Method combo instead of a use new method check box.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   18 Feb 2005 09:39:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  449
 * SCR Title:  Modify Polar Un-Wrap Tool
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modifications to Allow "Use New Angular Method" check box on the tool adjustment dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Feb 2005 16:04:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modifications to support new reset / create methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Apr 2003 16:21:32   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Nov 2002 13:47:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Add ResultDataIndex  and ResultImageIndex
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Apr 2001 19:28:52   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Apr 2001 15:43:48   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Mar 01 2000 12:15:32   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   NEW Module: Provides access to attributes of polar transform tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
