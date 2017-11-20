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
#include "SVOCore/SVEvaluate.h"
#include "SvOGui/SVFormulaEditorSheet.h"
#include "SVIPDoc.h"
#include "SVOCore/SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVOCore/SVTransformationTool.h"

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
SVToolAdjustmentDialogTranslationPageClass::SVToolAdjustmentDialogTranslationPageClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* Parent)
: CPropertyPage(SVToolAdjustmentDialogTranslationPageClass::IDD)
{
	//{{AFX_DATA_INIT(SVToolAdjustmentDialogTranslationPageClass)
	m_TranslationXValue = _T("");
	m_TranslationYValue = _T("");
	m_performTranslation = FALSE;
	//}}AFX_DATA_INIT
	
	m_pParentDialog	= Parent;
	m_pTool = nullptr;
	m_pEvaluateTranslationX	= nullptr;
	m_pEvaluateTranslationY	= nullptr;
	m_pInterpolationMode	= nullptr;
}
#pragma endregion

SVToolAdjustmentDialogTranslationPageClass::~SVToolAdjustmentDialogTranslationPageClass()
{
}

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
		SvDef::SVObjectTypeInfoStruct evaluateObjectInfo;
		evaluateObjectInfo.ObjectType = SvDef::SVMathContainerObjectType;
		
		// Get Evaluate Object for the X coordinate...
		evaluateObjectInfo.SubType = SvDef::SVEvaluateTranslationXObjectType;		
		m_pEvaluateTranslationX = dynamic_cast<SVEvaluateClass*>(m_pTool->getFirstObject(evaluateObjectInfo));
		
		// Get Evaluate Object for the Y coordinate...
		evaluateObjectInfo.SubType = SvDef::SVEvaluateTranslationYObjectType;
		m_pEvaluateTranslationY = dynamic_cast<SVEvaluateClass*>(m_pTool->getFirstObject(evaluateObjectInfo));

		// Interpolation Mode
		SvDef::SVObjectTypeInfoStruct objectInfo;
		objectInfo.ObjectType = SvDef::SVValueObjectType;
		objectInfo.SubType = SvDef::SVEnumValueObjectType;
		objectInfo.EmbeddedID = SVOutputInterpolationModeObjectGuid;
		m_pInterpolationMode = dynamic_cast<SVEnumerateValueObjectClass*>(m_pTool->getFirstObject(objectInfo));
		if( m_pInterpolationMode )
		{
			std::string EnumList;

			m_pInterpolationMode->GetEnumTypes( EnumList );
			m_cbInterpolation.SetEnumTypes( EnumList.c_str() );

			std::string EnumString;
			m_pInterpolationMode->getValue( EnumString );

			m_cbInterpolation.SelectString( -1, EnumString.c_str() );
		}

		SVTransformationToolClass* l_pTool = nullptr;

		m_pParentDialog->GetToolByType( l_pTool );
		// Check...
		if( nullptr != m_pEvaluateTranslationX && nullptr != m_pEvaluateTranslationY )
		{
			_variant_t Value = 0;

			GetValue( m_pTool->GetUniqueObjectID(), SVPerformTranslationObjectGuid, Value );

			m_performTranslation = static_cast<bool> (Value);

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
	DDX_Text(pDX, IDC_TRANSLATION_X_EDIT, m_TranslationXValue);
	DDX_Text(pDX, IDC_TRANSLATION_Y_EDIT, m_TranslationYValue);
	DDX_Check(pDX, IDC_PERFORM_TRANSLATION, m_performTranslation);
	//}}AFX_DATA_MAP
}

BOOL SVToolAdjustmentDialogTranslationPageClass::OnSetActive()
{
	if( m_pInterpolationMode )
	{
		std::string EnumString;
		m_pInterpolationMode->getValue( EnumString );
		m_cbInterpolation.SelectString( -1, EnumString.c_str() );
	}

	return CPropertyPage::OnSetActive();
}

void SVToolAdjustmentDialogTranslationPageClass::OnXFormulaButton() 
{
	if (m_pEvaluateTranslationX)
	{
		std::string Caption = m_pEvaluateTranslationX->GetName();
		Caption += _T(" Formula");

		const GUID& rInspectionID = m_pParentDialog->GetInspectionID();
		const GUID& rObjectID = m_pParentDialog->GetToolID();
		SvDef::SVObjectTypeInfoStruct info(SvDef::SVMathContainerObjectType, SvDef::SVEvaluateTranslationXObjectType);
		SvOg::SVFormulaEditorSheetClass dlg(rInspectionID, rObjectID, info, Caption.c_str());
		dlg.DoModal();
		
		refresh();
	}
}

void SVToolAdjustmentDialogTranslationPageClass::OnYFormulaButton() 
{
	if (m_pEvaluateTranslationY)
	{
		std::string Caption = m_pEvaluateTranslationY->GetName();
		Caption += _T(" Formula");

		const GUID& rInspectionID = m_pParentDialog->GetInspectionID();
		const GUID& rObjectID = m_pParentDialog->GetToolID();
		SvDef::SVObjectTypeInfoStruct info(SvDef::SVMathContainerObjectType, SvDef::SVEvaluateTranslationYObjectType);
		SvOg::SVFormulaEditorSheetClass dlg(rInspectionID, rObjectID, info, Caption.c_str());
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

		SVTransformationToolClass* l_pTool = nullptr;

		m_pParentDialog->GetToolByType( l_pTool );

		l_hrOk = AddInputRequest( m_pTool->GetUniqueObjectID(), SVPerformTranslationObjectGuid, m_performTranslation );

		int sel = m_cbInterpolation.GetCurSel();
		if( sel >= 0 )
		{
			long lValue = ( long ) m_cbInterpolation.GetItemData( sel );
			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pInterpolationMode, lValue );
			}
		}

		if( S_OK == l_hrOk )
		{
			l_hrOk = AddInputRequestMarker();
		}

		if( S_OK == l_hrOk )
		{
			l_hrOk = RunOnce( m_pTool->GetUniqueObjectID() );
		}

		UpdateData( FALSE );
	}

	return l_hrOk;
}

void SVToolAdjustmentDialogTranslationPageClass::refresh()
{
	if (m_pTool)
	{
		CWnd* pWnd = nullptr;

		SetInspectionData();

		_variant_t Value;

		if( S_OK == GetValue( m_pTool->GetUniqueObjectID(), SVOutputEvaluateTranslationXResultObjectGuid, Value ) )
		{
			m_TranslationXValue = static_cast< LPCTSTR >( _bstr_t( Value ) );
		}
		else
		{
			m_TranslationXValue = _T("");
		}
		
		if( S_OK == GetValue( m_pTool->GetUniqueObjectID(), SVOutputEvaluateTranslationYResultObjectGuid, Value ) )
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
#pragma endregion

