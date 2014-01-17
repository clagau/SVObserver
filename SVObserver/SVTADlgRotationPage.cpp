//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgRotaionPage
//* .File Name       : $Workfile:   SVTADlgRotationPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   15 Jan 2014 16:46:52  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTADlgRotationPage.h"

#include "SVFormulaEditorSheet.h"
#include "SVEvaluate.h"
#include "SVIPDoc.h"
#include "SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVImageTransform.h"
#include "SVToolSet.h"
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

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVToolAdjustmentDialogRotationPageClass
//* Note(s)    : Property Page
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
#pragma region Constructor
SVToolAdjustmentDialogRotationPageClass::SVToolAdjustmentDialogRotationPageClass( SVToolAdjustmentDialogSheetClass* Parent )
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
		CString strEnum;
		m_pInterpolationMode->GetValue( strEnum );
		m_cbInterpolation.SelectString( -1, strEnum );
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
		m_pEvaluateRotationX = ( SVEvaluateClass* ) ::SVSendMessage( m_pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &evaluateObjectInfo );
		if( m_pEvaluateRotationX )
		{
			// Get Evaluate Result Object for the X coordinate...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateRotationXResultObjectGuid;
			m_pRotationXResult = ( SVDoubleValueObjectClass* ) ::SVSendMessage( m_pEvaluateRotationX, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &resultObjectInfo );
		}

		// Get Evaluate Object for the Y coordinate...
		evaluateObjectInfo.SubType = SVEvaluateRotationYObjectType;
		m_pEvaluateRotationY = ( SVEvaluateClass* ) ::SVSendMessage( m_pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &evaluateObjectInfo );
		if( m_pEvaluateRotationY )
		{
			// Get Evaluate Result Object for the Y coordinate...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateRotationYResultObjectGuid;
			m_pRotationYResult = ( SVDoubleValueObjectClass* ) ::SVSendMessage( m_pEvaluateRotationY, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &resultObjectInfo );
		}

		// Get Evaluate Object for the Angle...
		evaluateObjectInfo.SubType	   = SVEvaluateRotationAngleObjectType;
		m_pEvaluateRotationAngle = ( SVEvaluateClass* ) ::SVSendMessage( m_pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &evaluateObjectInfo );
		if( m_pEvaluateRotationAngle )
		{
			// Get Evaluate Result Object for the Angle...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateRotationAngleResultObjectGuid;
			m_pRotationAngleResult = ( SVDoubleValueObjectClass* ) ::SVSendMessage( m_pEvaluateRotationAngle, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &resultObjectInfo );
		}
		
		// Get Rotation enabled...
		SVObjectTypeInfoStruct objectInfo;
		objectInfo.ObjectType = SVBoolValueObjectType;
		objectInfo.EmbeddedID = SVPerformRotationObjectGuid;
		m_pPerformRotation = ( SVBoolValueObjectClass* ) ::SVSendMessage( m_pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &objectInfo );

		// Interpolation Mode
		objectInfo.ObjectType = SVEnumValueObjectType;
		objectInfo.EmbeddedID = SVOutputInterpolationModeObjectGuid;
		m_pInterpolationMode = ( SVEnumerateValueObjectClass* ) ::SVSendMessage( m_pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD ) &objectInfo );
		if( m_pInterpolationMode )
		{
			CString l_strEnumList;

			m_pInterpolationMode->GetEnumTypes( l_strEnumList );
			m_cbInterpolation.SetEnumTypes( l_strEnumList );

			CString strEnum;
			m_pInterpolationMode->GetValue( strEnum );
			m_cbInterpolation.SelectString( -1, strEnum );
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
		CString strCaption = m_pEvaluateRotationAngle->GetName();
		strCaption += _T( " Formula" );
		SVFormulaEditorSheetClass dlg( strCaption );
		dlg.SetTaskObject( m_pEvaluateRotationAngle );

		dlg.DoModal();

		refresh();
	}
}

void SVToolAdjustmentDialogRotationPageClass::OnXFormulaButton() 
{
	if( m_pEvaluateRotationX )
	{
		CString strCaption = m_pEvaluateRotationX->GetName();
		strCaption += _T( " Formula" );
		SVFormulaEditorSheetClass dlg( strCaption );
		dlg.SetTaskObject( m_pEvaluateRotationX );

		dlg.DoModal();

		refresh();
	}
}

void SVToolAdjustmentDialogRotationPageClass::OnYFormulaButton() 
{
	if( m_pEvaluateRotationY )
	{
		CString strCaption = m_pEvaluateRotationY->GetName();
		strCaption += _T( " Formula" );
		SVFormulaEditorSheetClass dlg( strCaption );
		dlg.SetTaskObject( m_pEvaluateRotationY );

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

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequestMarker();
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = RunOnce( m_pTool );
		}

		int sel = m_cbInterpolation.GetCurSel();
		if( sel >= 0 )
		{
			long lValue = ( long ) m_cbInterpolation.GetItemData( sel );
			bUpdate = true;
			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pInterpolationMode, lValue );
			}
		}

		if( bUpdate )
		{
			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequestMarker();
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = RunOnce( m_pTool );
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
		CWnd* pWnd = NULL;

		SetInspectionData();

		// refresh X settings...
		if( m_pRotationXResult )
			m_pRotationXResult->GetValue( m_strRotationXValue );
		else
			m_strRotationXValue = _T( "" );

		// refresh Y settings...
		if( m_pRotationYResult )
			m_pRotationYResult->GetValue( m_strRotationYValue );
		else
			m_strRotationYValue = _T( "" );

		// refresh Angle setiings...
		if( m_pRotationAngleResult )
			m_pRotationAngleResult->GetValue( m_strRotationAngleValue );
		else
			m_strRotationAngleValue = _T( "" );

		UpdateData( FALSE ); // set data to dialog
	}
}
#pragma endregion

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTADlgRotationPage.cpp_v  $
 * 
 *    Rev 1.1   15 Jan 2014 16:46:52   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  879
 * SCR Title:  Add interpolation mode to transformation tool
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed code to conform with guidelines.
 * Added SVImageTransform.h.
 * Changed DoDataExchange method to include m_cbInterpolation.
 * Added protected method OnSetActive.
 * Changed OnInitDialog to initialize Interpolation Mode member variables.
 * Added protected MFC method OnSelChangeInterpolationModeCombo.
 * Changed SetInspectionData to check interpolation setting.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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