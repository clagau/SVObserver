//*****************************************************************************
/// \copyright (c) 2015 by Seidenader Maschinenbau GmbH
/// \file TADialogTableParameterPage.h
/// All Rights Reserved 
//*****************************************************************************
/// Property page for the tool adjustment sheet for parameter in the Table-tool.
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TADialogTableParameterPage.h"
#include "SVObjectLibrary\SVClsids.h"
#include "SVFormulaEditorSheet.h"
#include "GuiCommands\GetAvailableObjects.h"
#include "ObjectInterfaces\TextDefineSvOi.h"
#include "FormulaController.h"
#include "GuiCommands\GetErrorMessageList.h"
#include "SVStatusLibrary\MessageManagerResource.h"
#include "SVMessage\SVMessage.h"
#include "ObjectInterfaces\GlobalConst.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace Seidenader { namespace SVOGui {
	static const std::string MaxRowTag("MaxRow");

	BEGIN_MESSAGE_MAP(TADialogTableParameterPage, CPropertyPage)
		//{{AFX_MSG_MAP(TADialogTableParameterPage)
		ON_BN_CLICKED(IDC_BUTTON_TABLE_CLEAR_FORMULA, OnBnClickedButtonFormula)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

#pragma region Constructor
	TADialogTableParameterPage::TADialogTableParameterPage( const GUID& rInspectionID, const GUID& rTaskObjectID ) 
		: CPropertyPage(TADialogTableParameterPage::IDD)
		, m_InspectionID(rInspectionID)
		, m_TaskObjectID(rTaskObjectID)
		, m_ClearEquationID(SV_GUID_NULL)
		, m_pFormulaController(nullptr)
		, m_Values(SvOg::BoundValues(rInspectionID, rTaskObjectID, boost::assign::map_list_of(MaxRowTag, TableTool_MaxRowGuid)))
	{
	}

	TADialogTableParameterPage::~TADialogTableParameterPage()
	{
	}
#pragma endregion Constructor

#pragma region Public Methods
	bool TADialogTableParameterPage::QueryAllowExit()
	{
		HRESULT hResult = SetPageData();
		return (S_OK == hResult);
	}
#pragma endregion Public Methods

#pragma region Protected Methods
	void TADialogTableParameterPage::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(TADialogTableParameterPage)
		DDX_Text(pDX, IDC_EDIT_TABLE_MAX_ROWS, m_maxRows);
		DDV_MinMaxLong(pDX, m_maxRows, SvOi::cTableMaxRowMin, SvOi::cTableMaxRowMax);
		DDX_Text(pDX, IDC_EDIT_TABLE_CLEAR_FORMULA, m_clearString);
		//}}AFX_DATA_MAP
	}

	BOOL TADialogTableParameterPage::OnInitDialog() 
	{
		CPropertyPage::OnInitDialog();

		m_Values.Init();

		typedef GuiCmd::GetAvailableObjects Command;
		typedef SVSharedPtr<Command> CommandPtr;
		SvUl::NameGuidList availableList;
		CommandPtr commandPtr = new Command(m_TaskObjectID, SVObjectTypeInfoStruct(SVEquationObjectType));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			availableList = commandPtr->AvailableObjects();
			for(int i=0; i<availableList.size(); i++)
			{
				if (SvOi::TableClearEquationName == availableList[i].first)
				{
					m_ClearEquationID = availableList[i].second;
					m_pFormulaController = new FormulaController(m_InspectionID, m_TaskObjectID, m_ClearEquationID);
					break;
				}
			}
		}
		setEquationText();

		m_maxRows = m_Values.Get<long>(MaxRowTag);

		UpdateData( FALSE );

		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
	}

	BOOL TADialogTableParameterPage::OnKillActive()
	{
		HRESULT hResult = SetPageData();
		if (S_OK == hResult)
		{
			return CPropertyPage::OnKillActive();
		}
		else
		{
			return FALSE;
		}
	}

	void TADialogTableParameterPage::OnBnClickedButtonFormula()
	{
		BOOL updateState = UpdateData(TRUE);
		if (updateState)
		{
			CString strCaption = _T("Clear Conditional Formula");

			SvOg::SVFormulaEditorSheetClass dlg(m_InspectionID, m_TaskObjectID, m_ClearEquationID, strCaption);
			dlg.DoModal();
			setEquationText();
		}
	}
#pragma endregion Protected Methods

#pragma region Private Methods
	HRESULT TADialogTableParameterPage::SetPageData()
	{
		HRESULT hResult = S_FALSE;
		BOOL updateState = UpdateData(TRUE);
		if (updateState)
		{
			m_Values.Set<long>(MaxRowTag, m_maxRows);
			hResult = m_Values.Commit(true);

			if (S_OK != hResult)
			{
				typedef GuiCmd::GetErrorMessageList Command;
				typedef SVSharedPtr<Command> CommandPtr;
				CommandPtr commandPtr = new Command(m_TaskObjectID);
				SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
				HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
				SvStl::MessageContainerVector messages;
				if (S_OK == hr)
				{
					messages = commandPtr->GetMessageList();
				}
				if (messages.size() > 0 && 0 != messages[0].getMessage().m_MessageCode)
				{
					SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
					Msg.setMessage(messages[0].getMessage());
				}
				else
				{
					SVStringArray msgList;
					msgList.push_back(SvUl_SF::Format(_T("%d"), hResult));
					SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
					Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_UnknownCommitError, msgList, SvStl::SourceFileParams(StdMessageParams));
				}
			}
		}
		return hResult;
	}

	void TADialogTableParameterPage::setEquationText()
	{
		// Get text from EquationStruct and place into Editor
		m_clearString = _T("");
		if (nullptr != m_pFormulaController)
		{
			m_clearString = m_pFormulaController->GetEquationText().c_str();
		}

		UpdateData(FALSE);
	}
#pragma endregion Private Mehods
}}