//*****************************************************************************
/// \copyright (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file TADlgTableParameterPage.h
/// All Rights Reserved 
//*****************************************************************************
/// Property page for the tool adjustment sheet for parameter in the Table-tool.
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TADlgTableParameterPage.h"
#include "SVFormulaEditorSheet.h"
#include "Definitions/TextDefinesSvDef.h"
#include "SVOGuiUtility/FormulaController.h"
#include "SVStatusLibrary/MessageManager.h"
#include "Definitions/GlobalConst.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{

BEGIN_MESSAGE_MAP(TADlgTableParameterPage, CPropertyPage)
	//{{AFX_MSG_MAP(TADlgTableParameterPage)
	ON_BN_CLICKED(IDC_BUTTON_TABLE_CLEAR_FORMULA, OnBnClickedButtonFormula)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#pragma region Constructor
TADlgTableParameterPage::TADlgTableParameterPage(uint32_t inspectionID, uint32_t taskID)
	: CPropertyPage(TADlgTableParameterPage::IDD)
	, m_InspectionID(inspectionID)
	, m_TaskObjectID(taskID)
	, m_pFormulaController(nullptr)
	, m_values {SvOgu::BoundValues{ inspectionID, taskID }}
{
}

TADlgTableParameterPage::~TADlgTableParameterPage()
{
}
#pragma endregion Constructor

#pragma region Public Methods
bool TADlgTableParameterPage::QueryAllowExit()
{
	HRESULT hResult = SetPageData();
	return (S_OK == hResult);
}
#pragma endregion Public Methods

#pragma region Protected Methods
void TADlgTableParameterPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TADlgTableParameterPage)
	DDX_Text(pDX, IDC_EDIT_TABLE_MAX_ROWS, m_maxRows);
	DDV_MinMaxLong(pDX, m_maxRows, SvDef::cTableMaxRowMin, SvDef::cTableMaxRowMax);
	DDX_Text(pDX, IDC_EDIT_TABLE_CLEAR_FORMULA, m_clearString);
	//}}AFX_DATA_MAP
}

BOOL TADlgTableParameterPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_values.Init();

	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getavailableobjectsrequest();
	auto* pTreeSearchParameter = pRequest->mutable_tree_search();
	pTreeSearchParameter->set_search_start_id(m_TaskObjectID);
	pTreeSearchParameter->mutable_type_info()->set_objecttype(SvPb::SVEquationObjectType);

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getavailableobjectsresponse())
	{
		auto availableList = SvCmd::convertNameObjectIdList(responseCmd.getavailableobjectsresponse().list());
		
		auto iter = std::find_if(availableList.begin(), availableList.end(),
		[](const auto& entry) -> bool { return SvDef::TableClearEquationName == entry.first; });
		
		if (iter != availableList.end())
		{
			m_ClearEquationID = iter->second;
			m_pFormulaController = SvOi::IFormulaControllerPtr {new SvOgu::FormulaController(m_InspectionID, m_TaskObjectID, m_ClearEquationID)};
		}
	}
	setEquationText();

	m_maxRows = m_values.Get<long>(SvPb::TableTool_MaxRowEId);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

BOOL TADlgTableParameterPage::OnKillActive()
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

void TADlgTableParameterPage::OnBnClickedButtonFormula()
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
HRESULT TADlgTableParameterPage::SetPageData()
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
				SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
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

void TADlgTableParameterPage::setEquationText()
{
	// Get text from EquationStruct and place into Editor
	m_clearString = _T("");
	if (nullptr != m_pFormulaController)
	{
		m_clearString = m_pFormulaController->GetEquationText().c_str();
	}

	UpdateData(FALSE);
}

void TADlgTableParameterPage::resetInspection()
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_resetobjectrequest();
	pRequest->set_objectid(m_TaskObjectID);

	HRESULT hres = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	SvStl::MessageContainerVector messages = SvPb::convertProtobufToMessageVector(responseCmd.errormessage());
	if (messages.size() > 0 && 0 != messages[0].getMessage().m_MessageCode)
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(messages[0].getMessage());
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
