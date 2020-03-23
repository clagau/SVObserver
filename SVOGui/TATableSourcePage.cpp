//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file TaTableSourcePage.h
/// All Rights Reserved 
//*****************************************************************************
/// Tool adjustment page to select a table as source.
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <afxctl.h>
#include "TATableSourcePage.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage\SVMessage.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "Definitions/TextDefineSVDef.h"
#include "Definitions/StringTypeDef.h"
#include "SVProtoBuf/ConverterHelper.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
static LPCTSTR NoTableTag = _T("(No Table Available)");

BEGIN_MESSAGE_MAP(TATableSourcePage, CPropertyPage)
	//{{AFX_MSG_MAP(TATableSourcePage)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#pragma region Constructor
TATableSourcePage::TATableSourcePage(const SVGUID & rInspectionID, const SVGUID& rTaskObjectID, int id)
	: CPropertyPage(id)
	, m_InspectionID(rInspectionID)
	, m_TaskObjectID(rTaskObjectID)
{
}

TATableSourcePage::~TATableSourcePage()
{
}
#pragma endregion Constructor

#pragma region Protected Methods
void TATableSourcePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TATableSourcePage)
	DDX_Control(pDX, IDC_COMBO1, m_availableSourceTableCB);
	//}}AFX_DATA_MAP
}

BOOL TATableSourcePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	RetrieveAvailableList();

	std::string selectedTableName;
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getinputsrequest();
	SvPb::SetGuidInProtoBytes(pRequest->mutable_objectid(), m_TaskObjectID);
	pRequest->mutable_typeinfo()->set_objecttype(SvPb::TableObjectType);
	pRequest->set_objecttypetoinclude(SvPb::SVToolSetObjectType);
	pRequest->set_shouldexcludefirstobjectname(true);

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	SvUl::InputNameGuidPairList connectedList;
	if (S_OK == hr && responseCmd.has_getinputsresponse() && 0 < responseCmd.getinputsresponse().list_size())
	{
		m_inputName = responseCmd.getinputsresponse().list(0).inputname();
		selectedTableName = responseCmd.getinputsresponse().list(0).objectname();
	}
	else
	{	//this block should not reached, but if no input found, use default inputName and try with them to solve the problem. 
		m_inputName = SvDef::cInputTag_SourceTable;
	}

	m_availableSourceTableCB.Init(m_availableList, selectedTableName, NoTableTag);

	UpdateData(false); // set data to dialog

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void TATableSourcePage::OnSelchangeCombo1()
{
	UpdateData(TRUE); // get data from dialog

	int index = m_availableSourceTableCB.GetCurSel();
	if (LB_ERR != index)
	{
		CString tableName;
		m_availableSourceTableCB.GetLBText(index, tableName);
		if (!tableName.IsEmpty() && tableName != NoTableTag)
		{
			HRESULT hr = ConnectToObject(m_inputName, std::string(tableName));
			if (S_OK != hr)
			{
				SvDef::StringVector msgList;
				msgList.push_back(m_inputName);
				msgList.push_back(std::string(tableName));
				SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
				Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ConnectTableSourceFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
			}
		}
	}
}
#pragma endregion Protected Methods

#pragma region Private Methods
HRESULT TATableSourcePage::RetrieveAvailableList()
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getavailableobjectsrequest();
	SvPb::SetGuidInProtoBytes(pRequest->mutable_objectid(), m_InspectionID);
	pRequest->mutable_typeinfo()->set_objecttype(SvPb::TableObjectType);
	pRequest->set_objecttypetoinclude(SvPb::SVToolSetObjectType);
	pRequest->set_shouldexcludefirstobjectname(true);
	SvPb::SetGuidInProtoBytes(pRequest->mutable_isbeforetoolmethod()->mutable_toolid(), m_TaskObjectID);

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	SvUl::NameGuidList availableList;
	if (S_OK == hr && responseCmd.has_getavailableobjectsresponse())
	{
		m_availableList = SvCmd::convertNameGuidList(responseCmd.getavailableobjectsresponse().list());
	}
	return hr;
}

class ByName
{
	std::string m_name;
public:
	explicit ByName(const std::string& rName) : m_name(rName) {}
	bool operator()(const SvUl::NameGuidPair& rVal) const { return rVal.first == m_name; }
};

HRESULT TATableSourcePage::ConnectToObject(const std::string& inputName, const std::string& name)
{
	HRESULT hr = E_INVALIDARG;
	SvUl::NameGuidList::const_iterator it = std::find_if(m_availableList.begin(), m_availableList.end(), ByName(name));
	if (it != m_availableList.end())
	{
		SvPb::InspectionCmdRequest requestCmd;
		auto* pRequest = requestCmd.mutable_connecttoobjectrequest();
		SvPb::SetGuidInProtoBytes(pRequest->mutable_objectid(), m_TaskObjectID);
		pRequest->set_inputname(inputName);
		SvPb::SetGuidInProtoBytes(pRequest->mutable_newconnectedid(), it->second);
		pRequest->set_objecttype(SvPb::TableObjectType);

		hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, nullptr);
	}
	return hr;
}
#pragma endregion Private Methods
} //namespace SvOg

