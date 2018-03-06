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
#include "SVStatusLibrary/MessageManager.h"
#include "TextDefinesSvOg.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVMessage/SVMessage.h"
#include "InspectionCommands/GetInstanceIDByTypeInfo.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace SvOg
{
	SVFormulaEditorSheetClass::SVFormulaEditorSheetClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, const SvDef::SVObjectTypeInfoStruct& rInfo, LPCTSTR pCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPropertySheet(pCaption, pParentWnd, iSelectPage)
	{
		init(rInspectionID, rTaskObjectID, rInfo);
	}

	SVFormulaEditorSheetClass::SVFormulaEditorSheetClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, const SVGUID& rEquationID, LPCTSTR pCaption, CWnd* pParentWnd, UINT iSelectPage)
		: CPropertySheet(pCaption, pParentWnd, iSelectPage)
	{
		init(SvOi::IFormulaControllerPtr {new FormulaController(rInspectionID, rTaskObjectID, rEquationID)});
	}

	SVFormulaEditorSheetClass::SVFormulaEditorSheetClass(SvOi::IFormulaControllerPtr formulaController, LPCTSTR pCaption, CWnd* pParentWnd, UINT iSelectPage)
		: CPropertySheet(pCaption, pParentWnd, iSelectPage)
	{
		init(formulaController);
	}

	SVFormulaEditorSheetClass::~SVFormulaEditorSheetClass()
	{
	}

	void SVFormulaEditorSheetClass::init(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, const SvDef::SVObjectTypeInfoStruct& rInfo)
	{
		typedef SvCmd::GetInstanceIDByTypeInfo Command;
		typedef std::shared_ptr<Command> CommandPtr;
		SVGUID EquationID;

		// check for Math Container...
		if (SvDef::SVMathContainerObjectType == rInfo.ObjectType)
		{
			// Get the Math Container
			CommandPtr commandPtr(new Command(rTaskObjectID, rInfo));
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(rInspectionID, commandPtr);
			HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
			if (S_OK == hr)
			{
				// Get the Equation
				SvDef::SVObjectTypeInfoStruct info(SvDef::SVEquationObjectType, SvDef::SVMathEquationObjectType);
				GUID containerID = commandPtr->GetInstanceID();
				commandPtr = CommandPtr(new Command(containerID, info));
				SVObjectSynchronousCommandTemplate<CommandPtr> cmd(rInspectionID, commandPtr);
				HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
				if (S_OK == hr)
				{
					EquationID = commandPtr->GetInstanceID();
				}
				else
				{
					SvDef::StringVector msgList;
					msgList.push_back(SvUl::Format(_T("%d"), hr));
					SvStl::MessageMgrStd e(SvStl::LogOnly);
					e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnknownCommandError, SvStl::SourceFileParams(StdMessageParams));
					assert(false);
				}
			}
		}
		else
		{
			// Get the Equation
			CommandPtr commandPtr(new Command(rTaskObjectID, rInfo));
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(rInspectionID, commandPtr);
			HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
			if (S_OK == hr)
			{
				EquationID = commandPtr->GetInstanceID();
			}
			else
			{
				SvDef::StringVector msgList;
				msgList.push_back(SvUl::Format(_T("%d"), hr));
				SvStl::MessageMgrStd e(SvStl::LogOnly);
				e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnknownCommandError, SvStl::SourceFileParams(StdMessageParams));
				assert(false);
			}
		}
		m_isConditional = SvDef::SVConditionalObjectType == rInfo.SubType;

		init(SvOi::IFormulaControllerPtr{ new FormulaController(rInspectionID, rTaskObjectID, EquationID, m_isConditional) });
	}

	void SVFormulaEditorSheetClass::init(SvOi::IFormulaControllerPtr formulaController)
	{
		if(m_isConditional)
		{
			m_formulaPage = FormulaEditorPagePtr {new SvOg::SVFormulaEditorPageClass(formulaController, true, IDS_CONDITIONAL_STRING, IDS_CLASSNAME_SVTOOLSET)};
		}
		else
		{
			m_formulaPage = FormulaEditorPagePtr(new SVFormulaEditorPageClass(formulaController));
		}
		m_psh.dwFlags |= PSH_NOAPPLYNOW;
		AddPage(m_formulaPage.get());
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
						Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_InvalidFormula, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10224 );
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
} //namespace SvOg

