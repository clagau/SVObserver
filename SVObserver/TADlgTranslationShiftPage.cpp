//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgTranslationShiftPage
//* .File Name       : $Workfile:   TADlgTranslationShiftPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   15 May 2014 13:07:16  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TADlgTranslationShiftPage.h"
#include "SVIPDoc.h"
#include "SVShiftToolUtility.h"
#include "TADlgSheetClass.h"
#include "Operators/SVEvaluate.h"
#include "SvOGui/SVFormulaEditorSheet.h"
#include "SVRPropertyTree/SVRPropTreeItemStatic.h"
#include "Tools/SVShiftTool.h"
#pragma endregion Includes

#pragma region Declarations
// TADlgTranslationShiftPage dialog

// Put in by the wizard but not needed.
enum {IDC_SHIFT_VALUE_TRUE = 100};
#pragma endregion Declarations

#pragma region Constructor
TADlgTranslationShiftPage::TADlgTranslationShiftPage(uint32_t inspectionId, uint32_t, TADlgSheetClass* Parent)
: CPropertyPage(TADlgTranslationShiftPage::IDD)
, m_TranslationXValue(_T(""))
, m_TranslationYValue(_T(""))
, m_pParentDialog( Parent )
, m_pTool( nullptr ) // This needs to change
, pEvaluateTranslationY( nullptr ) // This needs to change
, m_lShiftType( 0 )
//TaskID is set later
, m_Values{ SvOgu::BoundValues{inspectionId, SvDef::InvalidObjectId} }
{
	if(nullptr != m_pParentDialog)
	{
		m_pTool = dynamic_cast<SvTo::SVShiftTool*> (m_pParentDialog->GetTaskObject());
	}
}

TADlgTranslationShiftPage::~TADlgTranslationShiftPage()
{
}
#pragma endregion Constructor

void TADlgTranslationShiftPage::DoDataExchange(CDataExchange* pDX)
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

BEGIN_MESSAGE_MAP(TADlgTranslationShiftPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BTN_LEARN, &TADlgTranslationShiftPage::OnBnClickedBtnLearn)
	ON_BN_CLICKED(IDC_BTN_NORMALIZE, &TADlgTranslationShiftPage::OnBnClickedBtnNormalize)
	ON_BN_CLICKED(IDC_PERFORM_TRANSLATION, &TADlgTranslationShiftPage::OnBnClickedPerformTranslation)
	ON_BN_CLICKED(IDC_TRANSLATION_X_FORMULA_BUTTON, &TADlgTranslationShiftPage::OnBnClickedTranslationXFormulaButton)
	ON_BN_CLICKED(IDC_TRANSLATION_Y_FORMULA_BUTTON, &TADlgTranslationShiftPage::OnBnClickedTranslationYFormulaButton)
	ON_CBN_SELCHANGE(IDC_COMBO_SHIFT_MODE, &TADlgTranslationShiftPage::OnCbnSelchangeComboShiftMode)
END_MESSAGE_MAP()

// TADlgTranslationShiftPage message handlers

BOOL TADlgTranslationShiftPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	if (nullptr != m_pTool)
	{
		m_Values.SetTaskID(m_pTool->getObjectId());
		m_Values.Init();

		SvDef::SVObjectTypeInfoStruct evaluateObjectInfo;
		evaluateObjectInfo.m_ObjectType = SvPb::SVMathContainerObjectType;

		// Get Evaluate Object for the X coordinate...
		evaluateObjectInfo.m_SubType = SvPb::SVEvaluateTranslationXObjectType;
		pEvaluateTranslationX = dynamic_cast<SvOp::SVEvaluate*>(m_pTool->getFirstObject(evaluateObjectInfo));
		
		// Get Evaluate Object for the Y coordinate...
		evaluateObjectInfo.m_SubType = SvPb::SVEvaluateTranslationYObjectType;
		pEvaluateTranslationY = dynamic_cast<SvOp::SVEvaluate*>(m_pTool->getFirstObject(evaluateObjectInfo));

		m_pvoShiftMode = &m_pTool->m_evoShiftMode;
		
		m_ctlShiftModeCombo.SetEnumTypes(m_pvoShiftMode->GetEnumVector());

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

HRESULT TADlgTranslationShiftPage::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	if (nullptr != m_pTool)
	{
		UpdateData( true ); // get data from dialog

		int iCurSel = m_ctlShiftModeCombo.GetCurSel();
		if( iCurSel >= 0 )
		{
			long lValue = static_cast<long> (m_ctlShiftModeCombo.GetItemData( iCurSel ));
			m_Values.Set<long>(m_pvoShiftMode->GetEmbeddedID(), lValue);
		}

		m_Values.Commit();
	}

	return l_hrOk;
}

void TADlgTranslationShiftPage::refresh()
{
	if (nullptr != m_pTool)
	{
		SetInspectionData();

		FillShiftProperties();

		variant_t Value = m_Values.Get<variant_t>(SvPb::TranslationXEId);
		m_TranslationXValue = static_cast< LPCTSTR >(_bstr_t(Value));

		Value = m_Values.Get<variant_t>(SvPb::TranslationYEId);
		m_TranslationYValue = static_cast< LPCTSTR >(_bstr_t(Value));

		UpdateData(false); // set data to dialog
	}
}
 
void TADlgTranslationShiftPage::OnBnClickedBtnLearn()
{
	SVShiftToolUtility::SetToolSetReference(m_pTool);

	refresh();
}

void TADlgTranslationShiftPage::OnBnClickedBtnNormalize()
{
	SVShiftToolUtility::SetToolNormalize(m_pTool);

	refresh();
}

void TADlgTranslationShiftPage::OnBnClickedPerformTranslation()
{
	refresh();
}

void TADlgTranslationShiftPage::OnBnClickedTranslationXFormulaButton()
{
	if (pEvaluateTranslationX)
	{
		std::string Caption = pEvaluateTranslationX->GetName();
		Caption += _T(" Formula");

		uint32_t inspectionID = m_pParentDialog->GetInspectionID();
		uint32_t objectID = m_pParentDialog->GetTaskObjectID();
		SvDef::SVObjectTypeInfoStruct info(SvPb::SVMathContainerObjectType, SvPb::SVEvaluateTranslationXObjectType);
		SvOg::SVFormulaEditorSheetClass dlg(inspectionID, objectID, info, Caption.c_str());
		dlg.DoModal();
		
		refresh();
	}
}

void TADlgTranslationShiftPage::OnBnClickedTranslationYFormulaButton()
{
	if (pEvaluateTranslationY)
	{
		std::string Caption = pEvaluateTranslationY->GetName();
		Caption += _T(" Formula");

		uint32_t inspectionID = m_pParentDialog->GetInspectionID();
		uint32_t objectID = m_pParentDialog->GetTaskObjectID();
		SvDef::SVObjectTypeInfoStruct info(SvPb::SVMathContainerObjectType, SvPb::SVEvaluateTranslationYObjectType);
		SvOg::SVFormulaEditorSheetClass dlg(inspectionID, objectID, info, Caption.c_str());
		dlg.DoModal();
		
		refresh();
	}
}

void TADlgTranslationShiftPage::OnCbnSelchangeComboShiftMode()
{
	UpdateControls();
	refresh();
}

void TADlgTranslationShiftPage::UpdateControls()
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

void TADlgTranslationShiftPage::SetupShiftPropertyTree()
{
	SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());

	if (pRoot)
	{
		pRoot->SetCanShrink(false);
		pRoot->SetLabelText(_T("Shift Tool Results"));
		
		auto* pPropItem = dynamic_cast< SVRPropertyItemStatic*> (m_Tree.InsertItem(new SVRPropertyItemStatic(),pRoot));
		if ( pPropItem )
		{
			pPropItem->SetLabelText(_T("Reference Translation X"));
			pPropItem->SetCtrlID(PROP_SHIFT_TRANS_X);
		}
		
		pPropItem = dynamic_cast<SVRPropertyItemStatic*>(m_Tree.InsertItem(new SVRPropertyItemStatic(),pRoot));
		if ( pPropItem )
		{
			pPropItem->SetLabelText(_T("Reference Translation Y"));
			pPropItem->SetCtrlID(PROP_SHIFT_TRANS_Y);
		}

		pPropItem = dynamic_cast<SVRPropertyItemStatic*>(m_Tree.InsertItem(new SVRPropertyItemStatic(),pRoot));
		if ( pPropItem )
		{
			pPropItem->SetLabelText(_T("Displacement X"));
			pPropItem->SetCtrlID(PROP_SHIFT_DISP_X);
		}
		
		pPropItem = dynamic_cast<SVRPropertyItemStatic*>(m_Tree.InsertItem(new SVRPropertyItemStatic(),pRoot));
		if ( pPropItem )
		{
			pPropItem->SetLabelText(_T("Displacement Y"));
			pPropItem->SetCtrlID(PROP_SHIFT_DISP_Y);
		}
		
		pPropItem = dynamic_cast<SVRPropertyItemStatic*>(m_Tree.InsertItem(new SVRPropertyItemStatic(),pRoot));
		if ( pPropItem )
		{
			pPropItem->SetLabelText(_T("Top Result"));
			pPropItem->SetCtrlID(PROP_SHIFT_RESULT_TOP);
		}
		
		pPropItem = dynamic_cast<SVRPropertyItemStatic*>(m_Tree.InsertItem(new SVRPropertyItemStatic(),pRoot));
		if ( pPropItem )
		{
			pPropItem->SetLabelText(_T("Left Result"));
			pPropItem->SetCtrlID(PROP_SHIFT_RESULT_LEFT);
		}

		pRoot->Select(true);
		pRoot->Expand();
	}
}

void TADlgTranslationShiftPage::FillShiftProperties()
{
	//get each value and put into property tree
	variant_t Value = m_Values.Get<variant_t>(SvPb::ShiftToolReferenceYEId);
	SVRPropertyItemStatic* pPropItem = dynamic_cast<SVRPropertyItemStatic*> (m_Tree.FindItem(PROP_SHIFT_TRANS_Y));
	if (nullptr != pPropItem)
	{
		pPropItem->SetItemValue(static_cast<LPCTSTR> (_bstr_t(Value)));
	}

	Value = m_Values.Get<variant_t>(SvPb::ShiftToolReferenceXEId);
	pPropItem = dynamic_cast<SVRPropertyItemStatic*> (m_Tree.FindItem(PROP_SHIFT_TRANS_X));
	if ( nullptr != pPropItem )
	{
		pPropItem->SetItemValue(static_cast<LPCTSTR> (_bstr_t(Value)));
	}

	Value = m_Values.Get<variant_t>(SvPb::ImageTransformDisplacementXEId);
	pPropItem = dynamic_cast<SVRPropertyItemStatic*> (m_Tree.FindItem(PROP_SHIFT_DISP_X));
	if (nullptr != pPropItem)
	{
		pPropItem->SetItemValue(static_cast<LPCTSTR> (_bstr_t(Value)));
	}

	Value = m_Values.Get<variant_t>(SvPb::ImageTransformDisplacementYEId);
	pPropItem = dynamic_cast<SVRPropertyItemStatic*> (m_Tree.FindItem(PROP_SHIFT_DISP_Y));
	if (nullptr != pPropItem)
	{
		pPropItem->SetItemValue(static_cast<LPCTSTR> (_bstr_t(Value)));
	}

	Value = m_Values.Get<variant_t>(SvPb::TopResultEId);
	pPropItem = dynamic_cast<SVRPropertyItemStatic*> (m_Tree.FindItem(PROP_SHIFT_RESULT_TOP));
	if (nullptr != pPropItem)
	{
		pPropItem->SetItemValue(static_cast<LPCTSTR> (_bstr_t(Value)));
	}

	Value = m_Values.Get<variant_t>(SvPb::LeftResultEId);
	pPropItem = dynamic_cast<SVRPropertyItemStatic*> (m_Tree.FindItem(PROP_SHIFT_RESULT_LEFT));
	if (nullptr != pPropItem)
	{
		pPropItem->SetItemValue( static_cast<LPCTSTR> (_bstr_t(Value)));
	}
	
	m_Tree.RefreshItems();
}

BOOL TADlgTranslationShiftPage::OnSetActive()
{
	refresh();
	return CPropertyPage::OnSetActive();
}

