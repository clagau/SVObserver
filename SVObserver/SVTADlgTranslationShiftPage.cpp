//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgTranslationShiftPage
//* .File Name       : $Workfile:   SVTADlgTranslationShiftPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   15 May 2014 13:07:16  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVObserver.h"
#include "SVTADlgTranslationShiftPage.h"
#include "SVEvaluate.h"
#include "SvOGui/SVFormulaEditorSheet.h"
#include "SVIPDoc.h"
#include "SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVShiftTool.h"
#include "SVShiftToolUtility.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVStatusLibrary\MessageManagerResource.h"
#pragma endregion Includes

#pragma region Declarations
// SVTADlgTranslationShiftPage dialog

// Put in by the wizard but not needed.
//IMPLEMENT_DYNAMIC(SVTADlgTranslationShiftPageClass, CPropertyPage)

enum {IDC_SHIFT_VALUE_TRUE = 100};
#pragma endregion Declarations

#pragma region Constructor
SVTADlgTranslationShiftPageClass::SVTADlgTranslationShiftPageClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* Parent)
: CPropertyPage(SVTADlgTranslationShiftPageClass::IDD)
, StrTranslationXValue(_T(""))
, StrTranslationYValue(_T(""))
, pParentDialog( Parent )
, pTool( nullptr ) // This needs to change
, pEvaluateTranslationY( nullptr ) // This needs to change
, m_lShiftType( 0 )
{
}

SVTADlgTranslationShiftPageClass::~SVTADlgTranslationShiftPageClass()
{
}
#pragma endregion Constructor

void SVTADlgTranslationShiftPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TRANSLATIONX_EDIT, StrTranslationXValue);
	DDX_Text(pDX, IDC_TRANSLATIONY_EDIT, StrTranslationYValue);
	DDX_Control(pDX, IDC_COMBO_SHIFT_MODE, m_ctlShiftModeCombo);
	DDX_Control(pDX, IDC_TRANSLATION_X_FORMULA_BUTTON, m_btnFormulaX);
	DDX_Control(pDX, IDC_TRANSLATION_Y_FORMULA_BUTTON, m_btnFormulaY);
	DDX_Control(pDX, IDC_BTN_LEARN, m_btnSetReference);
	DDX_Control(pDX, IDC_BTN_NORMALIZE, m_btnNormalize);
	DDX_Control(pDX, IDC_SHIFT_TOOL_VALUES, m_ShiftValueStatic);
	DDX_Control(pDX, IDC_CHK_ENABLE_SOURCE_IMAGE_EXTENTS, m_ctlEnableSourceImageExtents);
}


BEGIN_MESSAGE_MAP(SVTADlgTranslationShiftPageClass, CPropertyPage)
	ON_BN_CLICKED(IDC_BTN_LEARN, &SVTADlgTranslationShiftPageClass::OnBnClickedBtnLearn)
	ON_BN_CLICKED(IDC_BTN_NORMALIZE, &SVTADlgTranslationShiftPageClass::OnBnClickedBtnNormalize)
	ON_BN_CLICKED(IDC_PERFORM_TRANSLATION, &SVTADlgTranslationShiftPageClass::OnBnClickedPerformTranslation)
	ON_BN_CLICKED(IDC_TRANSLATION_X_FORMULA_BUTTON, &SVTADlgTranslationShiftPageClass::OnBnClickedTranslationXFormulaButton)
	ON_BN_CLICKED(IDC_TRANSLATION_Y_FORMULA_BUTTON, &SVTADlgTranslationShiftPageClass::OnBnClickedTranslationYFormulaButton)
	ON_CBN_SELCHANGE(IDC_COMBO_SHIFT_MODE, &SVTADlgTranslationShiftPageClass::OnCbnSelchangeComboShiftMode)
	ON_BN_CLICKED(IDC_CHK_ENABLE_SOURCE_IMAGE_EXTENTS, &SVTADlgTranslationShiftPageClass::OnBnClickedChkEnableSourceImageExtents)
END_MESSAGE_MAP()


// SVTADlgTranslationShiftPage message handlers

BOOL SVTADlgTranslationShiftPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	if (pParentDialog && (pTool = pParentDialog->GetTool()))
	{
		// This sets the base class 
		//   SVTaskObjectValueInterface::m_psvTaskObject (Tool).  This is 
		//   necessary for performing a RunOnce later.
		SetTaskObject( pTool );

		SVObjectTypeInfoStruct evaluateObjectInfo;
		evaluateObjectInfo.ObjectType = SVMathContainerObjectType;

		// Get Evaluate Object for the X coordinate...
		evaluateObjectInfo.SubType = SVEvaluateTranslationXObjectType;		
		pEvaluateTranslationX = reinterpret_cast<SVEvaluateClass*>(::SVSendMessage(pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&evaluateObjectInfo)));
		
		// Get Evaluate Object for the Y coordinate...
		evaluateObjectInfo.SubType = SVEvaluateTranslationYObjectType;
		pEvaluateTranslationY = reinterpret_cast<SVEvaluateClass*>(::SVSendMessage(pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&evaluateObjectInfo)));

		SVShiftTool* l_pTool = NULL;

		pParentDialog->GetToolByType( l_pTool );

		m_pvoShiftMode = &l_pTool->m_evoShiftMode;
		CString svShiftEnumTypes;

		m_pvoShiftMode->GetEnumTypes(svShiftEnumTypes);
		m_ctlShiftModeCombo.SetEnumTypes(svShiftEnumTypes);

		CString svCurrentShiftType;
		m_pvoShiftMode->GetValue(svCurrentShiftType);
		m_ctlShiftModeCombo.SelectString(-1,svCurrentShiftType);

		m_lShiftType = m_ctlShiftModeCombo.GetCurSel();

		m_pvoEnableSourceImageExtents = &l_pTool->m_EnableSourceImageExtents;
		BOOL bEnableValue;
		m_pvoEnableSourceImageExtents->GetValue(bEnableValue);
		m_ctlEnableSourceImageExtents.SetCheck(bEnableValue);



		//put in the Property Tree for the properties for the Shift Tool Values
		CRect rc;
		
		// PTS_NOTIFY - SVRPropTree will send notification messages to the parent window
		DWORD dwStyle = WS_CHILD|WS_VISIBLE|PTS_NOTIFY;

		GetDlgItem(IDC_SHIFT_TOOL_VALUES)->GetWindowRect(rc);
		ScreenToClient(rc);
	
		// Create SVRPropTree control IDC_SHIFT_VALUE_TRUE
		m_Tree.Create(dwStyle, rc, this, IDC_SHIFT_VALUE_TRUE);
		m_Tree.ShowInfoText(false);
		long lVal = m_Tree.GetColumn();
		m_Tree.SetColumn(200);

		// Check...
		if( pEvaluateTranslationX != NULL && pEvaluateTranslationY != NULL )
		{
			_variant_t l_Variant = 0;


			UpdateData(FALSE);
			UpdateControls();
			SetupShiftPropertyTree();
			refresh();
			
			return TRUE;
		}

	}

	return TRUE;
}

HRESULT SVTADlgTranslationShiftPageClass::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;
	bool l_bRequestAdded = false;

	if( pTool )
	{
		UpdateData( TRUE ); // get data from dialog

		SVShiftTool* l_pTool = NULL;

		pParentDialog->GetToolByType( l_pTool );

		int iCurSel = m_ctlShiftModeCombo.GetCurSel();
		if( iCurSel >= 0 )
		{
			long lValue = ( long ) m_ctlShiftModeCombo.GetItemData( iCurSel );

			if( AddInputRequest( m_pvoShiftMode, lValue ) == S_OK )
			{
				l_bRequestAdded = true;
				l_hrOk = S_OK;
			}
			else
			{
				l_hrOk = S_FALSE;
			}

			BOOL bValue = FALSE;
			if ( m_ctlEnableSourceImageExtents.GetCheck() )
			{
				bValue = TRUE;
			}

			if ( AddInputRequest(m_pvoEnableSourceImageExtents, bValue) == S_OK )
			{
				l_bRequestAdded = true;
				l_hrOk = S_OK;
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

void SVTADlgTranslationShiftPageClass::refresh()
{
	if (pTool)
	{
		CWnd* pWnd = NULL;

		SetInspectionData();

		FillShiftProperties();

		_variant_t l_Variant;

		if( GetValue( pTool->GetUniqueObjectID(), SVTranslationXObjectGuid, l_Variant.GetVARIANT() ) == S_OK )
		{
			StrTranslationXValue = static_cast< LPCTSTR >( _bstr_t( l_Variant ) );
		}
		else
		{
			StrTranslationXValue = _T("");
		}
		
		if( GetValue( pTool->GetUniqueObjectID(), SVTranslationYObjectGuid, l_Variant.GetVARIANT() ) == S_OK )
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
 
void SVTADlgTranslationShiftPageClass::OnBnClickedBtnLearn()
{
	SVShiftTool* l_pTool = NULL;

	pParentDialog->GetToolByType( l_pTool );
	SVShiftToolUtility::SetToolSetReference(l_pTool);

	refresh();
}

void SVTADlgTranslationShiftPageClass::OnBnClickedBtnNormalize()
{
	SVShiftTool* l_pTool = NULL;

	pParentDialog->GetToolByType( l_pTool );
	SVShiftToolUtility::SetToolNormalize(l_pTool);

	refresh();
}

void SVTADlgTranslationShiftPageClass::OnBnClickedPerformTranslation()
{
	refresh();
}

void SVTADlgTranslationShiftPageClass::OnBnClickedTranslationXFormulaButton()
{
	if (pEvaluateTranslationX)
	{
		CString strCaption = pEvaluateTranslationX->GetName();
		strCaption += _T(" Formula");

		const GUID& rInspectionID = pParentDialog->GetInspectionID();
		const GUID& rObjectID = pParentDialog->GetToolID();
		SVObjectTypeInfoStruct info(SVMathContainerObjectType, SVEvaluateTranslationXObjectType);
		SvOg::SVFormulaEditorSheetClass dlg(rInspectionID, rObjectID, info, strCaption);
		dlg.DoModal();
		
		refresh();
	}
}

void SVTADlgTranslationShiftPageClass::OnBnClickedTranslationYFormulaButton()
{
	if (pEvaluateTranslationY)
	{
		CString strCaption = pEvaluateTranslationY->GetName();
		strCaption += _T(" Formula");

		const GUID& rInspectionID = pParentDialog->GetInspectionID();
		const GUID& rObjectID = pParentDialog->GetToolID();
		SVObjectTypeInfoStruct info(SVMathContainerObjectType, SVEvaluateTranslationYObjectType);
		SvOg::SVFormulaEditorSheetClass dlg(rInspectionID, rObjectID, info, strCaption);
		dlg.DoModal();
		
		refresh();
	}
}

void SVTADlgTranslationShiftPageClass::OnCbnSelchangeComboShiftMode()
{
	UpdateControls();
	refresh();
}

void SVTADlgTranslationShiftPageClass::UpdateControls()
{
	int iCurSel = m_ctlShiftModeCombo.GetCurSel();

	switch (iCurSel)
	{
		case 0: //none
		{
			//disable Formula Buttons
			m_btnFormulaX.EnableWindow(FALSE);
			m_btnFormulaY.EnableWindow(FALSE);

			//hide reference controls
			m_btnSetReference.ShowWindow(SW_HIDE);
			m_btnNormalize.ShowWindow(SW_HIDE);
			m_ShiftValueStatic.ShowWindow(SW_HIDE);
			m_ctlEnableSourceImageExtents.EnableWindow(FALSE);
			GetDlgItem(IDC_SHIFT_TOOL_VALUES)->ShowWindow(SW_HIDE);
			m_Tree.ShowWindow(SW_HIDE);
			break;
		}

		case 1: // absolue
		{
			//enable formula buttons
			m_btnFormulaX.EnableWindow(TRUE);
			m_btnFormulaY.EnableWindow(TRUE);
			m_ctlEnableSourceImageExtents.EnableWindow(TRUE);

			//hide reference controls
			m_btnSetReference.ShowWindow(SW_HIDE);
			m_btnNormalize.ShowWindow(SW_HIDE);
			m_ShiftValueStatic.ShowWindow(SW_HIDE);
			GetDlgItem(IDC_SHIFT_TOOL_VALUES)->ShowWindow(SW_HIDE);
			m_Tree.ShowWindow(SW_HIDE);
			break;
		}

		case 2: // reference
		{

			//enable formula buttons
			m_btnFormulaX.EnableWindow(TRUE);
			m_btnFormulaY.EnableWindow(TRUE);
			m_ctlEnableSourceImageExtents.EnableWindow(TRUE);

			//show reference controls
			m_btnSetReference.ShowWindow(SW_SHOW);
			m_btnNormalize.ShowWindow(SW_SHOW);
			m_ShiftValueStatic.ShowWindow(SW_SHOW);
			GetDlgItem(IDC_SHIFT_TOOL_VALUES)->ShowWindow(SW_SHOW);
			m_Tree.ShowWindow(SW_SHOW);
			break;
		}
	}//end of swithc iCurSel

}

void SVTADlgTranslationShiftPageClass::SetupShiftPropertyTree()
{
	SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
	SVRPropertyItemStatic* pPropItem;

	CString sValue;

	if (pRoot)
	{
		pRoot->SetCanShrink(false);
		pRoot->SetLabelText(_T("Shift Tool Results"));
		
		pPropItem = dynamic_cast< SVRPropertyItemStatic*> (m_Tree.InsertItem(new SVRPropertyItemStatic(),pRoot));
		if ( pPropItem )
		{
			pPropItem->SetLabelText(_T("Reference Translation X"));
			pPropItem->SetCtrlID(PROP_SHIFT_TRANS_X);
		}
		
		pPropItem = (SVRPropertyItemStatic*)m_Tree.InsertItem(new SVRPropertyItemStatic(),pRoot);
		if ( pPropItem )
		{
			pPropItem->SetLabelText(_T("Reference Translation Y"));
			pPropItem->SetCtrlID(PROP_SHIFT_TRANS_Y);
		}

		pPropItem = (SVRPropertyItemStatic*)m_Tree.InsertItem(new SVRPropertyItemStatic(),pRoot);
		if ( pPropItem )
		{
			pPropItem->SetLabelText(_T("Displacement X"));
			pPropItem->SetCtrlID(PROP_SHIFT_DISP_X);
		}
		
		pPropItem = (SVRPropertyItemStatic*)m_Tree.InsertItem(new SVRPropertyItemStatic(),pRoot);
		if ( pPropItem )
		{
			pPropItem->SetLabelText(_T("Displacement Y"));
			pPropItem->SetCtrlID(PROP_SHIFT_DISP_Y);
		}
		
		pPropItem = (SVRPropertyItemStatic*)m_Tree.InsertItem(new SVRPropertyItemStatic(),pRoot);
		if ( pPropItem )
		{
			pPropItem->SetLabelText(_T("Top Result"));
			pPropItem->SetCtrlID(PROP_SHIFT_RESULT_TOP);
		}
		
		pPropItem = (SVRPropertyItemStatic*)m_Tree.InsertItem(new SVRPropertyItemStatic(),pRoot);
		if ( pPropItem )
		{
			pPropItem->SetLabelText(_T("Left Result"));
			pPropItem->SetCtrlID(PROP_SHIFT_RESULT_LEFT);
		}


		pRoot->Select(true);
		pRoot->Expand();
	}
}

void SVTADlgTranslationShiftPageClass::FillShiftProperties()
{
	SVRPropertyItemStatic* pPropItem = NULL;
	_variant_t l_Variant;
	CString sVal;

	//get each value and put into property tree

	if ( GetValue( pTool->GetUniqueObjectID(), SVShiftToolReferenceYObjectGuid, l_Variant.GetVARIANT() ) == S_OK )
	{
		pPropItem = (SVRPropertyItemStatic*)m_Tree.FindItem(PROP_SHIFT_TRANS_Y);
		pPropItem->SetItemValue( static_cast< LPCTSTR >( _bstr_t(l_Variant) ) );
	}

	if ( GetValue( pTool->GetUniqueObjectID(), SVShiftToolReferenceXObjectGuid, l_Variant.GetVARIANT() ) == S_OK )
	{
		pPropItem = dynamic_cast<SVRPropertyItemStatic*>(m_Tree.FindItem(PROP_SHIFT_TRANS_X));
		if ( pPropItem )
		{
			pPropItem->SetItemValue( static_cast< LPCTSTR >( _bstr_t(l_Variant) ) );
		}
	}

	if ( GetValue( pTool->GetUniqueObjectID(), SVImageTransformDisplacementXGuid, l_Variant.GetVARIANT() ) == S_OK )
	{
		pPropItem = (SVRPropertyItemStatic*)m_Tree.FindItem(PROP_SHIFT_DISP_X);
		pPropItem->SetItemValue( static_cast< LPCTSTR >( _bstr_t(l_Variant) ) );
	}
	if ( GetValue( pTool->GetUniqueObjectID(), SVImageTransformDisplacementYGuid, l_Variant.GetVARIANT() ) == S_OK )
	{
		pPropItem = (SVRPropertyItemStatic*)m_Tree.FindItem(PROP_SHIFT_DISP_Y);
		pPropItem->SetItemValue( static_cast< LPCTSTR >( _bstr_t(l_Variant) ) );
	}
	if ( GetValue( pTool->GetUniqueObjectID(), SVTopResultObjectGuid, l_Variant.GetVARIANT() ) == S_OK )
	{
		pPropItem = (SVRPropertyItemStatic*)m_Tree.FindItem(PROP_SHIFT_RESULT_TOP);
		pPropItem->SetItemValue( static_cast< LPCTSTR >( _bstr_t(l_Variant) ) );
	}
	if ( GetValue( pTool->GetUniqueObjectID(), SVLeftResultObjectGuid, l_Variant.GetVARIANT() ) == S_OK )
	{
		pPropItem = (SVRPropertyItemStatic*)m_Tree.FindItem(PROP_SHIFT_RESULT_LEFT);
		pPropItem->SetItemValue( static_cast< LPCTSTR >( _bstr_t(l_Variant) ) );
	}
	
	m_Tree.RefreshItems();
}

void SVTADlgTranslationShiftPageClass::OnBnClickedChkEnableSourceImageExtents()
{
	refresh();
	if( m_ctlEnableSourceImageExtents.GetCheck()==0)
	{
		SvStl::MessageMgrDisplayAndNotify e( SvStl::LogAndDisplay );
		e.setMessage( SVMSG_SVO_60_SHIFT_TOOL_SOURCE_IMAGE_EXTENTS_DISABLED, nullptr, StdMessageParams, SvOi::Err_17053_Shift_Tool_Source_Image_Extents_Disabled );
	}
}
BOOL SVTADlgTranslationShiftPageClass::OnSetActive()
{
	refresh();
	return CPropertyPage::OnSetActive();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTADlgTranslationShiftPage.cpp_v  $
 * 
 *    Rev 1.4   15 May 2014 13:07:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SendMessage to use proper type cast of DWORD_PTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   07 Apr 2014 14:05:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  896
 * SCR Title:  Add a Warning Message to the Shift Tool Adjustment Dialog
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified line breaks in the long message so it looks better.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Apr 2014 06:13:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  896
 * SCR Title:  Add a Warning Message to the Shift Tool Adjustment Dialog
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Warning message - displayed when checkbox is unchecked for "Enable Source Image Extents".
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 12:14:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Apr 2013 11:24:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   11 Jan 2013 15:04:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  807
 * SCR Title:  Allow Shift Tool to use Source Image Extents
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the new extent functionality that does not rebuild the image buffers on a positional change.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Jan 2013 12:47:00   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the SetReference & SetNormalize to use the SVShiftToolUtility.  Hid the Enable Source Image checkbox
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Jan 2013 16:43:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new checkbox for Enable source image Extents
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Jan 2013 15:13:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added property tree for the values and added the combo box for the different modes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/