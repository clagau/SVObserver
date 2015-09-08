//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgTranslationPage
//* .File Name       : $Workfile:   SVTADlgTranslationPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   15 May 2014 13:07:16  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTADlgTranslationPage.h"

#include "SVEvaluate.h"
#include "SVFormulaEditorSheet.h"
#include "SVIPDoc.h"
#include "SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVTransformationTool.h"
#pragma endregion

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
	ON_CBN_SELCHANGE(IDC_INTERPOLATION_MODE_COMBO, OnSelChangeInterpolationModeCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#pragma region Constructor
SVToolAdjustmentDialogTranslationPageClass::SVToolAdjustmentDialogTranslationPageClass(SVToolAdjustmentDialogSheetClass* Parent)
: CPropertyPage(SVToolAdjustmentDialogTranslationPageClass::IDD)
{
	//{{AFX_DATA_INIT(SVToolAdjustmentDialogTranslationPageClass)
	m_strTranslationXValue = _T("");
	m_strTranslationYValue = _T("");
	m_performTranslation = FALSE;
	//}}AFX_DATA_INIT
	
	m_pParentDialog	= Parent;
	m_pTool = nullptr;
	m_pEvaluateTranslationX	= nullptr;
	m_pEvaluateTranslationY	= nullptr;
	m_pInterpolationMode	= nullptr;
}
#pragma endregion

#pragma region Protected Methods
#pragma region MFC Methods
/////////////////////////////////////////////////////////////////////////////
// SVToolAdjustmentDialogTranslationPageClass message handlers
BOOL SVToolAdjustmentDialogTranslationPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	if (m_pParentDialog && (m_pTool = m_pParentDialog->GetTool()))
	{
		SetTaskObject( m_pTool );

		// Get Evaluate Object...
		SVObjectTypeInfoStruct evaluateObjectInfo;
		evaluateObjectInfo.ObjectType = SVMathContainerObjectType;
		
		// Get Evaluate Object for the X coordinate...
		evaluateObjectInfo.SubType = SVEvaluateTranslationXObjectType;		
		m_pEvaluateTranslationX = reinterpret_cast<SVEvaluateClass*>(::SVSendMessage(m_pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&evaluateObjectInfo)));
		
		// Get Evaluate Object for the Y coordinate...
		evaluateObjectInfo.SubType = SVEvaluateTranslationYObjectType;
		m_pEvaluateTranslationY = reinterpret_cast<SVEvaluateClass*>(::SVSendMessage(m_pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&evaluateObjectInfo)));

		// Interpolation Mode
		SVObjectTypeInfoStruct objectInfo;
		objectInfo.ObjectType = SVEnumValueObjectType;
		objectInfo.EmbeddedID = SVOutputInterpolationModeObjectGuid;
		m_pInterpolationMode = reinterpret_cast<SVEnumerateValueObjectClass*>(::SVSendMessage( m_pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&objectInfo)));
		if( m_pInterpolationMode )
		{
			CString l_strEnumList;

			m_pInterpolationMode->GetEnumTypes( l_strEnumList );
			m_cbInterpolation.SetEnumTypes( l_strEnumList );

			CString strEnum;
			m_pInterpolationMode->GetValue( strEnum );
			m_cbInterpolation.SelectString( -1, strEnum );
		}

		SVTransformationToolClass* l_pTool = NULL;

		m_pParentDialog->GetToolByType( l_pTool );
		// Check...
		if( m_pEvaluateTranslationX != NULL && m_pEvaluateTranslationY != NULL )
		{
			_variant_t l_Variant = 0;

			GetValue( m_pTool->GetUniqueObjectID(), SVPerformTranslationObjectGuid, l_Variant.GetVARIANT() );

			m_performTranslation = static_cast< bool >( l_Variant );

			UpdateData(FALSE);
			refresh();
			
			return TRUE;
		}
	}
	
	// Not valid call...
	if (GetParent())
	{
		GetParent()->SendMessage(WM_CLOSE);
	}
	else
	{
		SendMessage(WM_CLOSE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void SVToolAdjustmentDialogTranslationPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVToolAdjustmentDialogTranslationPageClass)
	DDX_Control(pDX, IDC_INTERPOLATION_MODE_COMBO, m_cbInterpolation);
	DDX_Text(pDX, IDC_TRANSLATION_X_EDIT, m_strTranslationXValue);
	DDX_Text(pDX, IDC_TRANSLATION_Y_EDIT, m_strTranslationYValue);
	DDX_Check(pDX, IDC_PERFORM_TRANSLATION, m_performTranslation);
	//}}AFX_DATA_MAP
}

BOOL SVToolAdjustmentDialogTranslationPageClass::OnSetActive()
{
	if( m_pInterpolationMode )
	{
		CString strEnum;
		m_pInterpolationMode->GetValue( strEnum );
		m_cbInterpolation.SelectString( -1, strEnum );
	}

	return CPropertyPage::OnSetActive();
}

void SVToolAdjustmentDialogTranslationPageClass::OnXFormulaButton() 
{
	if (m_pEvaluateTranslationX) //SEJ99 - This needs to change
	{
		CString strCaption = m_pEvaluateTranslationX->GetName(); //SEJ99 - This needs to change
		strCaption += _T(" Formula");

		const GUID& rInspectionID = m_pParentDialog->GetInspectionID();
		const GUID& rObjectID = m_pParentDialog->GetToolID();
		SVObjectTypeInfoStruct info(SVMathContainerObjectType, SVEvaluateTranslationXObjectType);
		SVFormulaEditorSheetClass dlg(rInspectionID, rObjectID, info, strCaption);
		dlg.DoModal();
		
		refresh();
	}
}

void SVToolAdjustmentDialogTranslationPageClass::OnYFormulaButton() 
{
	if (m_pEvaluateTranslationY) //SEJ99 - This needs to change
	{
		CString strCaption = m_pEvaluateTranslationY->GetName(); //SEJ99 - This needs to change
		strCaption += _T(" Formula");

		const GUID& rInspectionID = m_pParentDialog->GetInspectionID();
		const GUID& rObjectID = m_pParentDialog->GetToolID();
		SVObjectTypeInfoStruct info(SVMathContainerObjectType, SVEvaluateTranslationYObjectType);
		SVFormulaEditorSheetClass dlg(rInspectionID, rObjectID, info, strCaption);
		dlg.DoModal();
		
		refresh();
	}
}

void SVToolAdjustmentDialogTranslationPageClass::OnPerformTranslation() 
{
	refresh();
}

void SVToolAdjustmentDialogTranslationPageClass::OnSelChangeInterpolationModeCombo() 
{
	SetInspectionData();
}
#pragma endregion MFC Methods

HRESULT SVToolAdjustmentDialogTranslationPageClass::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	if( m_pTool )
	{
		UpdateData( TRUE ); // get data from dialog

		SVTransformationToolClass* l_pTool = NULL;

		m_pParentDialog->GetToolByType( l_pTool );

		l_hrOk = AddInputRequest( m_pTool->GetUniqueObjectID(), SVPerformTranslationObjectGuid, m_performTranslation );

		int sel = m_cbInterpolation.GetCurSel();
		if( sel >= 0 )
		{
			long lValue = ( long ) m_cbInterpolation.GetItemData( sel );
			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pInterpolationMode, lValue );
			}
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequestMarker();
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = RunOnce( m_pTool );
		}

		UpdateData( FALSE );
	}

	return l_hrOk;
}

void SVToolAdjustmentDialogTranslationPageClass::refresh()
{
	if (m_pTool)
	{
		CWnd* pWnd = NULL;

		SetInspectionData();

		_variant_t l_Variant;

		if( GetValue( m_pTool->GetUniqueObjectID(), SVOutputEvaluateTranslationXResultObjectGuid, l_Variant.GetVARIANT() ) == S_OK )
		{
			m_strTranslationXValue = static_cast< LPCTSTR >( _bstr_t( l_Variant ) );
		}
		else
		{
			m_strTranslationXValue = _T("");
		}
		
		if( GetValue( m_pTool->GetUniqueObjectID(), SVOutputEvaluateTranslationYResultObjectGuid, l_Variant.GetVARIANT() ) == S_OK )
		{
			m_strTranslationYValue = static_cast< LPCTSTR >( _bstr_t( l_Variant ) );
		}
		else
		{
			m_strTranslationYValue = _T("");
		}
		
		UpdateData(FALSE); // set data to dialog
	}
}
#pragma endregion

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTADlgTranslationPage.cpp_v  $
 * 
 *    Rev 1.3   15 May 2014 13:07:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SendMessage to use proper type cast of DWORD_PTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Feb 2014 12:14:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Jan 2014 16:49:24   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  879
 * SCR Title:  Add interpolation mode to transformation tool
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed code to conform with guidelines.
 * Changed OnInitDialog to initialize Interpolation Mode member variables.
 * Changed DoDataExchange method to include m_cbInterpolation.
 * Added protected MFC methods OnSetActive and OnSelChangeInterpolationModeCombo.
 * Changed SetInspectionData to check interpolation setting.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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