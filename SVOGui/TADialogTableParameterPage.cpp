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
TADialogTableParameterPage::TADialogTableParameterPage(const GUID& rInspectionID, const GUID& rTaskObjectID)
	: CPropertyPage(TADialogTableParameterPage::IDD)
	, m_InspectionID(rInspectionID)
	, m_TaskObjectID(rTaskObjectID)
	, m_ClearEquationID(GUID_NULL)
	, m_pFormulaController(nullptr)
	, m_Values {SvOg::BoundValues{ rInspectionID, rTaskObjectID }}
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

	m_Values.Init();

	SvPb::InspectionCmdMsgs request, response;
	SvPb::GetAvailableObjectsRequest* pGetAvailableObjectsRequest = request.mutable_getavailableobjectsrequest();

	SvPb::SetGuidInProtoBytes(pGetAvailableObjectsRequest->mutable_objectid(), m_TaskObjectID);
	pGetAvailableObjectsRequest->mutable_typeinfo()->set_objecttype(SvPb::SVEquationObjectType);
	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, request, &response);
	if (S_OK == hr && response.has_getavailableobjectsresponse())
	{
		auto availableList = SvCmd::convertNameGuidList(response.getavailableobjectsresponse().list());
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

	m_maxRows = m_Values.Get<long>(TableTool_MaxRowGuid);

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
		m_Values.Set<long>(TableTool_MaxRowGuid, m_maxRows);
		hResult = m_Values.Commit();

		if (S_OK != hResult)
		{
			SvStl::MessageContainerVector messages = m_Values.getFailedMessageList();
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
	SvPb::InspectionCmdMsgs Request, Response;
	SvPb::ResetObjectRequest* pResetObjectRequest = Request.mutable_resetobjectrequest();
	SvPb::SetGuidInProtoBytes(pResetObjectRequest->mutable_objectid(), m_TaskObjectID);
	HRESULT hres = SvCmd::InspectionCommands(m_InspectionID, Request, &Response);
	if (hres == S_OK && Response.has_resetobjectresponse())
	{
		SvStl::MessageContainerVector messages = SvCmd::setMessageContainerFromMessagePB(Response.resetobjectresponse().messages());
		if (messages.size() > 0 && 0 != messages[0].getMessage().m_MessageCode)
		{
			SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(messages[0].getMessage());
		}
	}
}
#pragma endregion Private Mehods
} //namespace SvOg
