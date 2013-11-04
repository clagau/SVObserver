//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgTranslationPage
//* .File Name       : $Workfile:   SVTADlgTranslationPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:24:14  $
//******************************************************************************

#include "stdafx.h"
#include "SVTADlgTranslationPage.h"

#include "SVEvaluate.h"
#include "SVFormulaEditorSheet.h"
#include "SVIPDoc.h"
#include "SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVTransformationTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogTranslationPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVToolAdjustmentDialogTranslationPageClass)
	ON_BN_CLICKED(IDC_TRANSLATION_X_FORMULA_BUTTON, OnXFormulaButton)
	ON_BN_CLICKED(IDC_TRANSLATION_Y_FORMULA_BUTTON, OnYFormulaButton)
	ON_BN_CLICKED(IDC_PERFORM_TRANSLATION, OnPerformTranslation)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVToolAdjustmentDialogTranslationPageClass::SVToolAdjustmentDialogTranslationPageClass(SVToolAdjustmentDialogSheetClass* Parent)
: CPropertyPage(SVToolAdjustmentDialogTranslationPageClass::IDD)
{
	//{{AFX_DATA_INIT(SVToolAdjustmentDialogTranslationPageClass)
	StrTranslationXValue = _T("");
	StrTranslationYValue = _T("");
	m_performTranslation = FALSE;
	//}}AFX_DATA_INIT
	
	pParentDialog	= Parent;
	pTool = NULL;
	
	pEvaluateTranslationX	= NULL;
	//pTranslationXResult = NULL;
	
	pEvaluateTranslationY	= NULL;
	//pTranslationYResult = NULL;
}


HRESULT SVToolAdjustmentDialogTranslationPageClass::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	if( pTool )
	{
		UpdateData( TRUE ); // get data from dialog

		SVTransformationToolClass* l_pTool = NULL;

		pParentDialog->GetToolByType( l_pTool );

		l_hrOk = AddInputRequest( pTool->GetUniqueObjectID(), SVPerformTranslationObjectGuid, m_performTranslation );


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

void SVToolAdjustmentDialogTranslationPageClass::refresh()
{
	if (pTool)
	{
		CWnd* pWnd = NULL;

		SetInspectionData();

		_variant_t l_Variant;

		if( GetValue( pTool->GetUniqueObjectID(), SVOutputEvaluateTranslationXResultObjectGuid, l_Variant.GetVARIANT() ) == S_OK )
		{
			StrTranslationXValue = static_cast< LPCTSTR >( _bstr_t( l_Variant ) );
		}
		else
		{
			StrTranslationXValue = _T("");
		}
		
		if( GetValue( pTool->GetUniqueObjectID(), SVOutputEvaluateTranslationYResultObjectGuid, l_Variant.GetVARIANT() ) == S_OK )
		{
			StrTranslationYValue = static_cast< LPCTSTR >( _bstr_t( l_Variant ) );
		}
		else
		{
			StrTranslationYValue = _T("");
		}
		
		UpdateData(FALSE); // set data to dialog
	}
}

void SVToolAdjustmentDialogTranslationPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVToolAdjustmentDialogTranslationPageClass)
	DDX_Text(pDX, IDC_TRANSLATION_X_EDIT, StrTranslationXValue);
	DDX_Text(pDX, IDC_TRANSLATION_Y_EDIT, StrTranslationYValue);
	DDX_Check(pDX, IDC_PERFORM_TRANSLATION, m_performTranslation);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// SVToolAdjustmentDialogTranslationPageClass message handlers

BOOL SVToolAdjustmentDialogTranslationPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	if (pParentDialog && (pTool = pParentDialog->GetTool()))
	{
		SetTaskObject( pTool );

		// Get Evaluate Object...
		SVObjectTypeInfoStruct evaluateObjectInfo;
		evaluateObjectInfo.ObjectType = SVMathContainerObjectType;
		
		/*
		// Set up the requestor for the Evaluate result
		SVObjectTypeInfoStruct resultObjectInfo;
		resultObjectInfo.ObjectType = SVDoubleValueObjectType;
		*/
		
		// Get Evaluate Object for the X coordinate...
		evaluateObjectInfo.SubType = SVEvaluateTranslationXObjectType;		
		pEvaluateTranslationX = (SVEvaluateClass*) ::SVSendMessage(pTool, SVM_GETFIRST_OBJECT, NULL, (DWORD) &evaluateObjectInfo);

		/*
		if (pEvaluateTranslationX)
		{
			// Get Evaluate Result Object for the X coordinate...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateTranslationXResultObjectGuid;
			pTranslationXResult = (SVDoubleValueObjectClass*) ::SVSendMessage(pEvaluateTranslationX, SVM_GETFIRST_OBJECT, NULL, (DWORD) &resultObjectInfo);
		}
		*/
		
		// Get Evaluate Object for the Y coordinate...
		evaluateObjectInfo.SubType = SVEvaluateTranslationYObjectType;
		pEvaluateTranslationY = (SVEvaluateClass*) ::SVSendMessage(pTool, SVM_GETFIRST_OBJECT, NULL, (DWORD) &evaluateObjectInfo);

		/*
		if (pEvaluateTranslationY)
		{
			// Get Evaluate Result Object for the Y coordinate...
			resultObjectInfo.EmbeddedID = SVOutputEvaluateTranslationYResultObjectGuid;
			pTranslationYResult = (SVDoubleValueObjectClass*) ::SVSendMessage(pEvaluateTranslationY, SVM_GETFIRST_OBJECT, NULL, (DWORD) &resultObjectInfo);
		}
		*/
		
		/*
		// Get Translation enabled...
		SVObjectTypeInfoStruct objectInfo;
		objectInfo.ObjectType = SVBoolValueObjectType;
		objectInfo.EmbeddedID = SVPerformTranslationObjectGuid;
		pPerformTranslation = (SVBoolValueObjectClass*) ::SVSendMessage(pTool, SVM_GETFIRST_OBJECT, NULL, (DWORD) &objectInfo);
		*/

		/*
		SVObjectTypeInfoStruct objectInfoMode;
		objectInfoMode.ObjectType = SVLongValueObjectType;
		objectInfoMode.EmbeddedID = SVTranslationModeGuid;
		pTranslationMode = (SVLongValueObjectClass*) ::SVSendMessage(pTool, SVM_GETFIRST_OBJECT, NULL, (DWORD) &objectInfoMode);

		long lModeValue = 0;
		if ( pTranslationMode )
		{
			pTranslationMode->GetValue(lModeValue);
		}
		*/

		SVTransformationToolClass* l_pTool = NULL;

		pParentDialog->GetToolByType( l_pTool );
		// Check...
		if( pEvaluateTranslationX != NULL && pEvaluateTranslationY != NULL )
		{
			_variant_t l_Variant = 0;

			GetValue( pTool->GetUniqueObjectID(), SVPerformTranslationObjectGuid, l_Variant.GetVARIANT() );

			m_performTranslation = static_cast< bool >( l_Variant );

			UpdateData(FALSE);
			refresh();
			
			return TRUE;
		}
	}
	
	// Not valid call...
	if (GetParent())
		GetParent()->SendMessage(WM_CLOSE);
	else
		SendMessage(WM_CLOSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zur�ckgeben
}


void SVToolAdjustmentDialogTranslationPageClass::OnXFormulaButton() 
{
	if (pEvaluateTranslationX)
	{
		CString strCaption = pEvaluateTranslationX->GetName();
		strCaption += _T(" Formula");
		SVFormulaEditorSheetClass dlg(strCaption);
		dlg.SetTaskObject(pEvaluateTranslationX);
		
		dlg.DoModal();
		
		refresh();
	}
}

void SVToolAdjustmentDialogTranslationPageClass::OnYFormulaButton() 
{
	if (pEvaluateTranslationY)
	{
		CString strCaption = pEvaluateTranslationY->GetName();
		strCaption += _T(" Formula");
		SVFormulaEditorSheetClass dlg(strCaption);
		dlg.SetTaskObject(pEvaluateTranslationY);
		
		dlg.DoModal();
		
		refresh();
	}
}

void SVToolAdjustmentDialogTranslationPageClass::OnPerformTranslation() 
{
	refresh();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTADlgTranslationPage.cpp_v  $
 * 
 *    Rev 1.0   24 Apr 2013 11:24:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   05 Dec 2012 12:10:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changes made for adding the new Shift Tool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   01 Nov 2012 13:39:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  798
 * SCR Title:  Change Transformation Tool to have an option for translation only
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added Tanslation Mode combo and static text to dialog - new method OnCbnSelCHangeComoboTransMode
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   02 Jul 2012 18:03:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   16 Aug 2005 09:45:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed un-necessary casting from AddInputRequest method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   10 Aug 2005 10:06:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added UpdateData(FALSE) to onInitDialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   09 Aug 2005 08:25:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Feb 2005 16:04:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modifications to support new reset / create methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Apr 2003 16:21:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Nov 2002 13:49:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Indexes for Value Object
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Apr 2001 19:28:56   Steve
 * Project:  SVObserver
 * Change Request(SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Apr 2001 15:48:08   Steve
 * Project:  SVObserver
 * Change Request(SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   21 Feb 2000 09:31:02   sjones
 * Project:  SVObserver
 * Change Request(SCR) nbr:  104
 * SCR Title:  New Build Reference Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin.
 * New Dialog Class to edit the translation parameters for the
 * Transformation Tool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
