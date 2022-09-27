//*****************************************************************************
/// \copyright (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file TADialogTableParameterPage.h
/// All Rights Reserved 
//*****************************************************************************
/// Property page for the tool adjustment sheet for parameter in the Table-tool.
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TADialogLoopToolParameterPage.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{

BEGIN_MESSAGE_MAP(TADialogLoopToolParameterPage, CPropertyPage)
	//{{AFX_MSG_MAP(TADialogLoopToolParameterPage)
	ON_BN_CLICKED(IDC_BUTTON_LINKED_LOOPS, OnBnClickedButtonLastLoopCount)
	ON_EN_KILLFOCUS(IDC_EDIT_LINKED_LOOPS, OnKillFocusLastLoopCount)
	ON_BN_CLICKED(IDC_BUTTON_LINKED_BREAK_CONDITION, OnBnClickedButtonBreakCondition)
	ON_EN_KILLFOCUS(IDC_EDIT_BREAK_CONDITION, OnKillFocusBreakCondition)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


TADialogLoopToolParameterPage::TADialogLoopToolParameterPage(uint32_t inspectionID, uint32_t taskObjectID)
	: CPropertyPage(TADialogLoopToolParameterPage::IDD)
	, m_InspectionID(inspectionID)
	, m_TaskObjectID(taskObjectID)
	, m_values {SvOg::BoundValues{ inspectionID, taskObjectID }}
{
}

TADialogLoopToolParameterPage::~TADialogLoopToolParameterPage()
{
}



bool TADialogLoopToolParameterPage::QueryAllowExit()
{
	HRESULT hResult = SetPageData();
	return (S_OK == hResult);
}



void TADialogLoopToolParameterPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TADialogTableParameterPage)
	DDX_Text(pDX, IDC_EDIT_MAX_LOOPS, m_MaxLoopCount);
	DDV_MinMaxLong(pDX, m_MaxLoopCount, 1, 10000);
	DDX_Control(pDX, IDC_EDIT_LINKED_LOOPS, m_EditLoopsValue);
	DDX_Control(pDX, IDC_BUTTON_LINKED_LOOPS, m_ButtonLoopsValue);

	DDX_Control(pDX, IDC_EDIT_BREAK_CONDITION, m_EditBreakCondition);
	DDX_Control(pDX, IDC_BUTTON_LINKED_BREAK_CONDITION, m_ButtonBreakCondition);
	//}}AFX_DATA_MAP
}

BOOL TADialogLoopToolParameterPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	m_values.Init();
	m_MaxLoopCount = m_values.Get<long>(SvPb::MaxLoopsEId);
	m_LoopsValueWidget = std::make_unique<LinkedValueWidgetHelper>(m_EditLoopsValue, m_ButtonLoopsValue, m_InspectionID, m_TaskObjectID, SvPb::LinkedLoopsEId, &m_values);
	m_BreakConditionWidget = std::make_unique<LinkedValueWidgetHelper>(m_EditBreakCondition, m_ButtonBreakCondition, m_InspectionID, m_TaskObjectID, SvPb::LoopBreakEId, &m_values);
	UpdateData(FALSE);
	return TRUE;
}

BOOL TADialogLoopToolParameterPage::OnKillActive()
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

HRESULT TADialogLoopToolParameterPage::SetPageData()
{
	HRESULT hResult = S_FALSE;
	BOOL updateState = UpdateData(true);
	if (updateState)
	{
		m_values.Set<long>(SvPb::MaxLoopsEId, m_MaxLoopCount);
		hResult = m_values.Commit();
		if (S_OK == hResult)
		{
			auto data = m_values.Get<LinkedValueData>(SvPb::LinkedLoopsEId);
			if (SvPb::LinkedSelectedOption::DirectValue == data.m_selectedOption)
			{
				long MaxLoopCount = m_values.Get<long>(SvPb::MaxLoopsEId);
				long LoopCount = data.m_Value;
				if (LoopCount > MaxLoopCount)
				{
					SvDef::StringVector messageList;
					std::string valueString = m_values.Get<CString>(SvPb::MaxLoopsEId).GetString();
					messageList.push_back(valueString);
					messageList.push_back(std::to_string(LoopCount));
					
					SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
					Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_IsLessThan, messageList, SvStl::SourceFileParams(StdMessageParams));
					
					
					return E_FAIL;
				}
			
			}
		}

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


void TADialogLoopToolParameterPage::resetInspection()
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_resetobjectrequest();
	pRequest->set_objectid(m_TaskObjectID);

	SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	SvStl::MessageContainerVector messages {SvPb::convertProtobufToMessageVector(responseCmd.errormessage())};
	if (messages.size() > 0 && 0 != messages[0].getMessage().m_MessageCode)
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(messages[0].getMessage());
	}
}

void TADialogLoopToolParameterPage::OnBnClickedButtonBreakCondition()
{
	m_BreakConditionWidget->OnButton();
}

void TADialogLoopToolParameterPage::OnBnClickedButtonLastLoopCount()
{
	m_LoopsValueWidget->OnButton();
}

void TADialogLoopToolParameterPage::OnKillFocusBreakCondition()
{
	m_BreakConditionWidget->EditboxToValue();
}

void TADialogLoopToolParameterPage::OnKillFocusLastLoopCount()
{
	m_LoopsValueWidget->EditboxToValue();
}

} //namespace SvOg
