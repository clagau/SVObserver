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
#include "SVObjectLibrary\SVClsids.h"
#include "InspectionCommands\GetAvailableObjects.h"
#include "Definitions/TextDefineSVDef.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage\SVMessage.h"
#include "Definitions/GlobalConst.h"
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


TADialogLoopToolParameterPage::TADialogLoopToolParameterPage(const GUID& rInspectionID, const GUID& rTaskObjectID)
	: CPropertyPage(TADialogLoopToolParameterPage::IDD)
	, m_InspectionID(rInspectionID)
	, m_TaskObjectID(rTaskObjectID)
	, m_Values {SvOg::BoundValues{ rInspectionID, rTaskObjectID }}
	, m_objectSelector(rInspectionID)
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
	m_Values.Init();
	m_MaxLoopCount = m_Values.Get<long>(MaxLoopsGuid);
	CString valueString = m_Values.Get<CString>(LinkedLoops_LinkedGuid);
	if (valueString.IsEmpty())
	{
		valueString = m_Values.Get<CString>(LinkedLoopsGuid);
	}
	m_EditLoopsValue.SetWindowText(valueString.GetString());

	valueString = m_Values.Get<CString>(LoopBreakGuid);
	if (valueString.IsEmpty())
	{
		valueString = m_Values.Get<CString>(LoopBreak_LinkedGuid);
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
		m_Values.Set<CString>(LinkedLoopsGuid, Value);
		m_Values.Set<long>(MaxLoopsGuid, m_MaxLoopCount);

		m_EditBreakCondition.GetWindowText(Value);
		m_Values.Set<CString>(LoopBreakGuid, Value);

		hResult = m_Values.Commit();
		if (S_OK != hResult)
		{
			SvStl::MessageContainerVector messages = m_Values.getFailedMessageList();
			if (messages.size() > 0 && 0 != messages[0].getMessage().m_MessageCode)
			{
				SvStl::MessageMgrStd Msg(SvStl::LogAndDisplay);
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
	SvPb::InspectionCmdMsgs Request, Response;
	SvPb::ResetObjectRequest* pRequestMessage = Request.mutable_resetobjectrequest();

	SvPb::SetGuidInProtoBytes(pRequestMessage->mutable_objectid(), m_TaskObjectID);
	SvCmd::InspectionCommandsSynchronous(m_InspectionID, &Request, &Response);
	SvStl::MessageContainerVector messages;
	if (Response.has_resetobjectresponse())
	{
		messages = SvCmd::setMessageContainerFromMessagePB(Response.resetobjectresponse().messages());
	}

	if (messages.size() > 0 && 0 != messages[0].getMessage().m_MessageCode)
	{
		SvStl::MessageMgrStd Msg(SvStl::LogAndDisplay);
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
