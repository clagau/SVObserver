//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolSetAdjustmentDialog
//* .File Name       : $Workfile:   SVToolSetAdjustmentDialogSheet.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   14 Jan 2014 12:32:42  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVToolSetAdjustmentDialogSheet.h"
#include "SVOGui/FormulaController.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVOGui/TextDefinesSvOg.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(SVToolSetAdjustmentDialogSheetClass, CPropertySheet)

//******************************************************************************
// Message Map Entries
//******************************************************************************
BEGIN_MESSAGE_MAP(SVToolSetAdjustmentDialogSheetClass, CPropertySheet)
	//{{AFX_MSG_MAP(SVToolSetAdjustmentDialogSheetClass)
	ON_WM_DESTROY()
	ON_COMMAND(IDOK,OnOK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVToolSetAdjustmentDialogSheetClass::SVToolSetAdjustmentDialogSheetClass( const GUID& rInspectionID, const GUID& rTaskObjectID, SVConditionalClass& rCondition, UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage )
: CPropertySheet( nIDCaption, pParentWnd, iSelectPage )
, m_InspectionID(rInspectionID)
, m_TaskObjectID(rTaskObjectID)
{
	init( rCondition );
}

SVToolSetAdjustmentDialogSheetClass::SVToolSetAdjustmentDialogSheetClass( const GUID& rInspectionID, const GUID& rTaskObjectID, SVConditionalClass& rCondition, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage )
: CPropertySheet( pszCaption, pParentWnd, iSelectPage )
, m_InspectionID(rInspectionID)
, m_TaskObjectID(rTaskObjectID)
{
	init( rCondition );
}

SVToolSetAdjustmentDialogSheetClass::~SVToolSetAdjustmentDialogSheetClass()
{
}

void SVToolSetAdjustmentDialogSheetClass::init( SVConditionalClass& rCondition )
{
	m_formulaPage = FormulaEditorPagePtr(new SvOg::SVFormulaEditorPageClass(m_InspectionID, m_TaskObjectID, 
		new SvOg::FormulaController(m_InspectionID, m_TaskObjectID, SVObjectTypeInfoStruct(SVEquationObjectType, SVConditionalObjectType), false),
								true, IDS_CONDITIONAL_STRING, IDS_CLASSNAME_SVTOOLSET));

	addPages();
}

void SVToolSetAdjustmentDialogSheetClass::addPages()
{
	AddPage( m_formulaPage.get() );
}

BOOL SVToolSetAdjustmentDialogSheetClass::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	// Disable and Hide Cancel Button
	HWND hWnd = ::GetDlgItem(m_hWnd, IDCANCEL);
	if (nullptr != hWnd)
	{
		::EnableWindow( hWnd, FALSE );
		::ShowWindow(hWnd, SW_HIDE );
	}

	// Remove Close Button
	ModifyStyle( WS_SYSMENU, 0, SWP_FRAMECHANGED );

	CString Text( _T( "ToolSet Adjustment: " ) );
	Text += m_formulaPage->GetOwnerName().c_str();

	SetWindowText( Text );
		
	return bResult;
}

void SVToolSetAdjustmentDialogSheetClass::OnDestroy() 
{
	CPropertySheet::OnDestroy();
}

void SVToolSetAdjustmentDialogSheetClass::OnOK()
{
	// Try to validate the Conditional
	if( m_formulaPage->validateAndSetEquation() )
	{
		m_formulaPage->SetDefaultInputs();
		EndDialog( IDOK );
	}
	else
	{
		// Equation must be valid or disabled
		SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_InvalidFormula, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10220 );
	}
}

void SVToolSetAdjustmentDialogSheetClass::OnCancel() 
{
	EndDialog( IDCANCEL );
}

