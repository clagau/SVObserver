//*****************************************************************************
/// \copyright (c) 2015 by Seidenader Maschinenbau GmbH
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
	ON_BN_CLICKED(IDC_BUTTON_LINKED_BREAK_CONDITION, OnBnClickedButtonBreakCondition)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


TADialogLoopToolParameterPage::TADialogLoopToolParameterPage(uint32_t inspectionID, uint32_t taskObjectID)
	: CPropertyPage(TADialogLoopToolParameterPage::IDD)
	, m_InspectionID(inspectionID)
	, m_TaskObjectID(taskObjectID)
	, m_values {SvOg::BoundValues{ inspectionID, taskObjectID }}
	, m_objectSelector(inspectionID)
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
	DDX_Control(pDX, IDC_BUTTON_LINKED_LOOPS, m_ButtonLoopsLink);

	DDX_Control(pDX, IDC_EDIT_BREAK_CONDITION, m_EditBreakCondition);
	DDX_Control(pDX, IDC_BUTTON_LINKED_BREAK_CONDITION, m_ButtonBreakConditionLink);
	//}}AFX_DATA_MAP
}

BOOL TADialogLoopToolParameterPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	// Put the Down Arrow on the Button
	m_downArrowBitmap.LoadOEMBitmap(OBM_DNARROW);
	m_ButtonLoopsLink.SetBitmap(static_cast<HBITMAP> (m_downArrowBitmap));

	m_ButtonBreakConditionLink.SetBitmap(static_cast<HBITMAP> (m_downArrowBitmap));
	m_values.Init();
	m_MaxLoopCount = m_values.Get<long>(SvPb::MaxLoopsEId);
	CString valueString = m_values.Get<CString>(SvPb::LinkedLoops_LinkedEId);
	if (valueString.IsEmpty())
	{
		valueString = m_values.Get<CString>(SvPb::LinkedLoopsEId);
	}
	m_EditLoopsValue.SetWindowText(valueString.GetString());

	valueString = m_values.Get<CString>(SvPb::LoopBreak_LinkedEId);
	if (valueString.IsEmpty())
	{
		valueString = m_values.Get<CString>(SvPb::LoopBreakEId);
	}
	m_EditBreakCondition.SetWindowText(valueString.GetString());
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
		CString Value;
		m_EditLoopsValue.GetWindowText(Value);
		m_values.Set<CString>(SvPb::LinkedLoopsEId, Value);
		m_values.Set<long>(SvPb::MaxLoopsEId, m_MaxLoopCount);

		m_EditBreakCondition.GetWindowText(Value);
		m_values.Set<CString>(SvPb::LoopBreakEId, Value);

		hResult = m_values.Commit();
		if (S_OK == hResult)
		{
			Value = m_values.Get<CString>(SvPb::LinkedLoops_LinkedEId);
			if (Value.IsEmpty())
			{
				long MaxLoopCount = m_values.Get<long>(SvPb::MaxLoopsEId);
				long LoopCount = m_values.Get<long>(SvPb::LinkedLoopsEId);
				if (LoopCount > MaxLoopCount)
				{
					SvDef::StringVector messageList;
					std::string valueString = m_values.Get<CString>(SvPb::MaxLoopsEId);
					messageList.push_back(valueString);
					valueString = m_values.Get<CString>(SvPb::LinkedLoopsEId);
					messageList.push_back(valueString);
					
					SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
					Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_IsLessThan, messageList, SvStl::SourceFileParams(StdMessageParams), 0);
					
					
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
	SvStl::MessageContainerVector messages;
	if (responseCmd.has_standardresponse())
	{
		messages = SvPb::setMessageVectorFromMessagePB(responseCmd.standardresponse().errormessages());
	}

	if (messages.size() > 0 && 0 != messages[0].getMessage().m_MessageCode)
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(messages[0].getMessage());
	}
}

void TADialogLoopToolParameterPage::OnBnClickedButtonBreakCondition()
{
	CString Temp;
	m_EditBreakCondition.GetWindowText(Temp);
	std::string Value = Temp;
	if (m_objectSelector.Show(Value, "Loop break condition", this))
	{
		m_EditBreakCondition.SetWindowText(Value.c_str());
	}
	UpdateData(false);
}



void TADialogLoopToolParameterPage::OnBnClickedButtonLastLoopCount()
{
	CString Temp;
	m_EditLoopsValue.GetWindowText(Temp);
	std::string Value = Temp;

	if (m_objectSelector.Show(Value, "Loops Count", this))
	{
		m_EditLoopsValue.SetWindowText(Value.c_str());
	}
	UpdateData(false);
}


} //namespace SvOg
