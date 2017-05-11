//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFormulaEditorSheet
//* .File Name       : $Workfile:   SVFormulaEditorSheet.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   17 Jul 2014 19:11:04  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVOResource/resource.h"
#include "SVFormulaEditorSheet.h"
#include "FormulaController.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvOg.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVMessage\SVMessage.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace Seidenader { namespace SVOGui
{
	SVFormulaEditorSheetClass::SVFormulaEditorSheetClass(const GUID& rInspectionID, const GUID& rTaskObjectID, const SVObjectTypeInfoStruct& rInfo, LPCTSTR pCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPropertySheet(pCaption, pParentWnd, iSelectPage)
	, m_InspectionID(rInspectionID)
	, m_TaskObjectID(rTaskObjectID)
	{
		init(rInfo);
	}

	SVFormulaEditorSheetClass::SVFormulaEditorSheetClass(const GUID& rInspectionID, const GUID& rTaskObjectID, const GUID& rEquationId, LPCTSTR pCaption, CWnd* pParentWnd, UINT iSelectPage)
		: CPropertySheet(pCaption, pParentWnd, iSelectPage)
		, m_InspectionID(rInspectionID)
		, m_TaskObjectID(rTaskObjectID)
	{
		init(rEquationId);
	}

	SVFormulaEditorSheetClass::~SVFormulaEditorSheetClass()
	{
	}

	void SVFormulaEditorSheetClass::init(const SVObjectTypeInfoStruct& rInfo)
	{
		m_formulaPage = FormulaEditorPagePtr(new SVFormulaEditorPageClass(m_InspectionID, m_TaskObjectID, new FormulaController(m_InspectionID, m_TaskObjectID, rInfo)));
		m_psh.dwFlags |= PSH_NOAPPLYNOW;
		AddPage( m_formulaPage.get() );
	}

	void SVFormulaEditorSheetClass::init(const GUID& rEquationId)
	{
		m_formulaPage = FormulaEditorPagePtr(new SVFormulaEditorPageClass(m_InspectionID, m_TaskObjectID, new FormulaController(m_InspectionID, m_TaskObjectID, rEquationId)));
		m_psh.dwFlags |= PSH_NOAPPLYNOW;
		AddPage( m_formulaPage.get() );
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////////
	BOOL SVFormulaEditorSheetClass::OnInitDialog()
	{
		BOOL bResult = CPropertySheet::OnInitDialog();

		// Disable and Hide Cancel Button
		HWND hWnd = ::GetDlgItem(m_hWnd, IDCANCEL);
		if (nullptr != hWnd )
		{
			::EnableWindow( hWnd, false );
			::ShowWindow(hWnd, SW_HIDE );
		}

		// Remove Close Button
		ModifyStyle( WS_SYSMENU, 0, SWP_FRAMECHANGED );

		return bResult;
	}

	BEGIN_MESSAGE_MAP(SVFormulaEditorSheetClass, CPropertySheet)
		//{{AFX_MSG_MAP(SVFormulaEditorSheetClass)
			ON_COMMAND(IDOK,OnOK)
		ON_WM_SYSCOMMAND()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// SVFormulaEditorSheetClass message handlers

	void SVFormulaEditorSheetClass::OnOK()
	{
		// Try to validate the Equations
		int cnt = GetPageCount();

		for( int i = 0;i < cnt; i++ )
		{
			CPropertyPage* pPage = GetPage(i);
			if( pPage && pPage->GetSafeHwnd() ) 
			{
				if( SVFormulaEditorPageClass* pFormularPage = dynamic_cast<SVFormulaEditorPageClass*>( pPage ) )
				{
					if( !pFormularPage->validateAndSetEquation() )
					{
						// Equation must be valid or disabled
						SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
						Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_InvalidFormula, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10224 );
						return;
					}
				}
			}
		}

		EndDialog( IDOK );
	}

	void SVFormulaEditorSheetClass::OnSysCommand(UINT nID, LPARAM lParam)
	{
		switch (nID & 0xFFF0)
		{
			// Ignore the close command
			case SC_CLOSE:
				// Do nothing.
				break;
			default:
				CPropertySheet::OnSysCommand(nID, lParam);
				break;
		}
	}
} /* namespace SVOGui */ } /* namespace Seidenader */

