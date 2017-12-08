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
#include "InspectionEngine/SVEvaluate.h"
#include "SvOGui/SVFormulaEditorSheet.h"
#include "SVIPDoc.h"
#include "InspectionEngine/SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVShiftTool.h"
#include "SVShiftToolUtility.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"

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
, m_TranslationXValue(_T(""))
, m_TranslationYValue(_T(""))
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
	DDX_Text(pDX, IDC_TRANSLATIONX_EDIT, m_TranslationXValue);
	DDX_Text(pDX, IDC_TRANSLATIONY_EDIT, m_TranslationYValue);
	DDX_Control(pDX, IDC_COMBO_SHIFT_MODE, m_ctlShiftModeCombo);
	DDX_Control(pDX, IDC_TRANSLATION_X_FORMULA_BUTTON, m_btnFormulaX);
	DDX_Control(pDX, IDC_TRANSLATION_Y_FORMULA_BUTTON, m_btnFormulaY);
	DDX_Control(pDX, IDC_BTN_LEARN, m_btnSetReference);
	DDX_Control(pDX, IDC_BTN_NORMALIZE, m_btnNormalize);
	DDX_Control(pDX, IDC_SHIFT_TOOL_VALUES, m_ShiftValueStatic);
}

BEGIN_MESSAGE_MAP(SVTADlgTranslationShiftPageClass, CPropertyPage)
	ON_BN_CLICKED(IDC_BTN_LEARN, &SVTADlgTranslationShiftPageClass::OnBnClickedBtnLearn)
	ON_BN_CLICKED(IDC_BTN_NORMALIZE, &SVTADlgTranslationShiftPageClass::OnBnClickedBtnNormalize)
	ON_BN_CLICKED(IDC_PERFORM_TRANSLATION, &SVTADlgTranslationShiftPageClass::OnBnClickedPerformTranslation)
	ON_BN_CLICKED(IDC_TRANSLATION_X_FORMULA_BUTTON, &SVTADlgTranslationShiftPageClass::OnBnClickedTranslationXFormulaButton)
	ON_BN_CLICKED(IDC_TRANSLATION_Y_FORMULA_BUTTON, &SVTADlgTranslationShiftPageClass::OnBnClickedTranslationYFormulaButton)
	ON_CBN_SELCHANGE(IDC_COMBO_SHIFT_MODE, &SVTADlgTranslationShiftPageClass::OnCbnSelchangeComboShiftMode)
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

		SvDef::SVObjectTypeInfoStruct evaluateObjectInfo;
		evaluateObjectInfo.ObjectType = SvDef::SVMathContainerObjectType;

		// Get Evaluate Object for the X coordinate...
		evaluateObjectInfo.SubType = SvDef::SVEvaluateTranslationXObjectType;		
		pEvaluateTranslationX = dynamic_cast<SVEvaluateClass*>(pTool->getFirstObject(evaluateObjectInfo));
		
		// Get Evaluate Object for the Y coordinate...
		evaluateObjectInfo.SubType = SvDef::SVEvaluateTranslationYObjectType;
		pEvaluateTranslationY = dynamic_cast<SVEvaluateClass*>(pTool->getFirstObject(evaluateObjectInfo));

		SVShiftTool* l_pTool = nullptr;

		pParentDialog->GetToolByType( l_pTool );

		m_pvoShiftMode = &l_pTool->m_evoShiftMode;
		
		std::string EnumTypes;
		m_pvoShiftMode->GetEnumTypes( EnumTypes );
		m_ctlShiftModeCombo.SetEnumTypes( EnumTypes.c_str() );

		std::string ShiftType;
		m_pvoShiftMode->getValue( ShiftType );
		m_ctlShiftModeCombo.SelectString( -1, ShiftType.c_str() );

		m_lShiftType = m_ctlShiftModeCombo.GetCurSel();

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
		if( nullptr != pEvaluateTranslationX && nullptr != pEvaluateTranslationY )
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

		SVShiftTool* l_pTool = nullptr;

		pParentDialog->GetToolByType( l_pTool );

		int iCurSel = m_ctlShiftModeCombo.GetCurSel();
		if( iCurSel >= 0 )
		{
			long lValue = ( long ) m_ctlShiftModeCombo.GetItemData( iCurSel );

			if( S_OK == AddInputRequest( m_pvoShiftMode, lValue ) )
			{
				l_bRequestAdded = true;
				l_hrOk = S_OK;
			}
			else
			{
				l_hrOk = S_FALSE;
			}
		}

		if( S_OK == l_hrOk )
		{
			l_hrOk = AddInputRequestMarker();
		}

		if( S_OK == l_hrOk )
		{
			l_hrOk = RunOnce( pTool->GetUniqueObjectID() );
		}

		UpdateData( FALSE );
	}

	return l_hrOk;
}

void SVTADlgTranslationShiftPageClass::refresh()
{
	if (pTool)
	{
		SetInspectionData();

		FillShiftProperties();

		_variant_t Value;

		if( S_OK == GetValue( pTool->GetUniqueObjectID(), SVTranslationXObjectGuid, Value ) )
		{
			m_TranslationXValue = static_cast< LPCTSTR >( _bstr_t( Value ) );
		}
		else
		{
			m_TranslationXValue = _T("");
		}
		
		if( S_OK == GetValue( pTool->GetUniqueObjectID(), SVTranslationYObjectGuid, Value ) )
		{
			m_TranslationYValue = static_cast< LPCTSTR >( _bstr_t( Value ) );
		}
		else
		{
			m_TranslationYValue = _T("");
		}
		UpdateData(FALSE); // set data to dialog
	}
}
 
void SVTADlgTranslationShiftPageClass::OnBnClickedBtnLearn()
{
	SVShiftTool* l_pTool = nullptr;

	pParentDialog->GetToolByType( l_pTool );
	SVShiftToolUtility::SetToolSetReference(l_pTool);

	refresh();
}

void SVTADlgTranslationShiftPageClass::OnBnClickedBtnNormalize()
{
	SVShiftTool* l_pTool = nullptr;

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
		std::string Caption = pEvaluateTranslationX->GetName();
		Caption += _T(" Formula");

		const GUID& rInspectionID = pParentDialog->GetInspectionID();
		const GUID& rObjectID = pParentDialog->GetToolID();
		SvDef::SVObjectTypeInfoStruct info(SvDef::SVMathContainerObjectType, SvDef::SVEvaluateTranslationXObjectType);
		SvOg::SVFormulaEditorSheetClass dlg(rInspectionID, rObjectID, info, Caption.c_str());
		dlg.DoModal();
		
		refresh();
	}
}

void SVTADlgTranslationShiftPageClass::OnBnClickedTranslationYFormulaButton()
{
	if (pEvaluateTranslationY)
	{
		std::string Caption = pEvaluateTranslationY->GetName();
		Caption += _T(" Formula");

		const GUID& rInspectionID = pParentDialog->GetInspectionID();
		const GUID& rObjectID = pParentDialog->GetToolID();
		SvDef::SVObjectTypeInfoStruct info(SvDef::SVMathContainerObjectType, SvDef::SVEvaluateTranslationYObjectType);
		SvOg::SVFormulaEditorSheetClass dlg(rInspectionID, rObjectID, info, Caption.c_str());
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

			GetDlgItem(IDC_SHIFT_TOOL_VALUES)->ShowWindow(SW_HIDE);
			m_Tree.ShowWindow(SW_HIDE);
			break;
		}

		case 1: // absolute
		{
			//enable formula buttons
			m_btnFormulaX.EnableWindow(TRUE);
			m_btnFormulaY.EnableWindow(TRUE);

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

			//show reference controls
			m_btnSetReference.ShowWindow(SW_SHOW);
			m_btnNormalize.ShowWindow(SW_SHOW);
			m_ShiftValueStatic.ShowWindow(SW_SHOW);
			GetDlgItem(IDC_SHIFT_TOOL_VALUES)->ShowWindow(SW_SHOW);
			m_Tree.ShowWindow(SW_SHOW);
			break;
		}
	}//end of switch iCurSel
}

void SVTADlgTranslationShiftPageClass::SetupShiftPropertyTree()
{
	SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
	SVRPropertyItemStatic* pPropItem;

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
	SVRPropertyItemStatic* pPropItem = nullptr;
	_variant_t Value;

	//get each value and put into property tree

	if ( S_OK == GetValue( pTool->GetUniqueObjectID(), SVShiftToolReferenceYObjectGuid, Value ) )
	{
		pPropItem = (SVRPropertyItemStatic*)m_Tree.FindItem(PROP_SHIFT_TRANS_Y);
		pPropItem->SetItemValue( static_cast< LPCTSTR >( _bstr_t(Value) ) );
	}

	if ( S_OK == GetValue( pTool->GetUniqueObjectID(), SVShiftToolReferenceXObjectGuid, Value ) )
	{
		pPropItem = dynamic_cast<SVRPropertyItemStatic*>(m_Tree.FindItem(PROP_SHIFT_TRANS_X));
		if ( pPropItem )
		{
			pPropItem->SetItemValue( static_cast< LPCTSTR >( _bstr_t(Value) ) );
		}
	}

	if ( S_OK == GetValue( pTool->GetUniqueObjectID(), SVImageTransformDisplacementXGuid, Value ) )
	{
		pPropItem = (SVRPropertyItemStatic*)m_Tree.FindItem(PROP_SHIFT_DISP_X);
		pPropItem->SetItemValue( static_cast< LPCTSTR >( _bstr_t(Value) ) );
	}
	if ( S_OK == GetValue( pTool->GetUniqueObjectID(), SVImageTransformDisplacementYGuid, Value ) )
	{
		pPropItem = (SVRPropertyItemStatic*)m_Tree.FindItem(PROP_SHIFT_DISP_Y);
		pPropItem->SetItemValue( static_cast< LPCTSTR >( _bstr_t(Value) ) );
	}
	if ( S_OK == GetValue( pTool->GetUniqueObjectID(), SVTopResultObjectGuid, Value ) )
	{
		pPropItem = (SVRPropertyItemStatic*)m_Tree.FindItem(PROP_SHIFT_RESULT_TOP);
		pPropItem->SetItemValue( static_cast< LPCTSTR >( _bstr_t(Value) ) );
	}
	if ( S_OK == GetValue( pTool->GetUniqueObjectID(), SVLeftResultObjectGuid, Value ) )
	{
		pPropItem = (SVRPropertyItemStatic*)m_Tree.FindItem(PROP_SHIFT_RESULT_LEFT);
		pPropItem->SetItemValue( static_cast< LPCTSTR >( _bstr_t(Value) ) );
	}
	
	m_Tree.RefreshItems();
}

BOOL SVTADlgTranslationShiftPageClass::OnSetActive()
{
	refresh();
	return CPropertyPage::OnSetActive();
}

