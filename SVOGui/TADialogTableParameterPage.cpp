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
#include "SVFormulaEditorSheet.h"
#include "Definitions/TextDefineSVDef.h"
#include "FormulaController.h"
#include "SVStatusLibrary\MessageManager.h"
#include "Definitions/GlobalConst.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{

BEGIN_MESSAGE_MAP(TADialogTableParameterPage, CPropertyPage)
	//{{AFX_MSG_MAP(TADialogTableParameterPage)
	ON_BN_CLICKED(IDC_BUTTON_TABLE_CLEAR_FORMULA, OnBnClickedButtonFormula)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#pragma region Constructor
TADialogTableParameterPage::TADialogTableParameterPage(uint32_t inspectionID, uint32_t taskID)
	: CPropertyPage(TADialogTableParameterPage::IDD)
	, m_InspectionID(inspectionID)
	, m_TaskObjectID(taskID)
	, m_pFormulaController(nullptr)
	, m_values {SvOg::BoundValues{ inspectionID, taskID }}
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
	DDV_MinMaxLong(pDX, m_maxRows, SvDef::cTableMaxRowMin, SvDef::cTableMaxRowMax);
	DDX_Text(pDX, IDC_EDIT_TABLE_CLEAR_FORMULA, m_clearString);
	//}}AFX_DATA_MAP
}

BOOL TADialogTableParameterPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_values.Init();

	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getavailableobjectsrequest();
	pRequest->set_objectid(m_TaskObjectID);
	pRequest->mutable_typeinfo()->set_objecttype(SvPb::SVEquationObjectType);

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getavailableobjectsresponse())
	{
		auto availableList = SvCmd::convertNameObjectIdList(responseCmd.getavailableobjectsresponse().list());
		for (auto entry : availableList)
		{
			if (SvDef::TableClearEquationName == entry.first)
			{
				m_ClearEquationID = entry.second;
				m_pFormulaController = SvOi::IFormulaControllerPtr {new FormulaController(m_InspectionID, m_TaskObjectID, m_ClearEquationID)};
				break;
			}
		}
	}
	setEquationText();

	m_maxRows = m_values.Get<long>(SvPb::TableTool_MaxRowEId);

	UpdateData(FALSE);

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
	BOOL updateState = UpdateData(true);
	if (updateState)
	{
		CString strCaption = _T("Clear Conditional Formula");

		SvOg::SVFormulaEditorSheetClass dlg(m_pFormulaController, strCaption);
		dlg.DoModal();
		setEquationText();
	}
}
#pragma endregion Protected Methods

#pragma region Private Methods
HRESULT TADialogTableParameterPage::SetPageData()
{
	HRESULT hResult = S_FALSE;
	BOOL updateState = UpdateData(true);
	if (updateState)
	{
		m_values.Set<long>(SvPb::TableTool_MaxRowEId, m_maxRows);
		hResult = m_values.Commit();

		if (S_OK != hResult)
		{
			SvStl::MessageContainerVector messages = m_values.getFailedMessageList();
			if (messages.size() > 0 && 0 != messages[0].getMessage().m_MessageCode)
			{
				SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
				Msg.setMessage(messages[0].getMessage());
			}
			else
			{
				//this error is not coming directly from the commit and will be shown in another place.
				hResult = S_OK;
			}
		}
		else
		{
			resetInspection();
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

void TADialogTableParameterPage::resetInspection()
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_resetobjectrequest();
	pRequest->set_objectid(m_TaskObjectID);

	HRESULT hres = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (hres == S_OK && responseCmd.has_standardresponse())
	{
		SvStl::MessageContainerVector messages = SvPb::setMessageVectorFromMessagePB(responseCmd.standardresponse().errormessages());
		if (messages.size() > 0 && 0 != messages[0].getMessage().m_MessageCode)
		{
			SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(messages[0].getMessage());
		}
	}

	if (hres == S_OK)
	{
		requestCmd.Clear();
		auto* pSetDefaultRequest = requestCmd.mutable_setdefaultinputsrequest();
		pSetDefaultRequest->set_objectid(m_InspectionID);

		SvCmd::InspectionCommands(m_InspectionID, requestCmd, nullptr);
	}
	
}

#pragma endregion Private Mehods
} //namespace SvOg
