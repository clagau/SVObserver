//*****************************************************************************
/// \copyright (c) 2016,2016 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file TaTableSourcePage.h
/// All Rights Reserved 
//*****************************************************************************
/// Tool adjustment page to select a table as source.
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <afxctl.h>
#include "TADlgTableSourcePage.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "Definitions/TextDefinesSvDef.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
static LPCTSTR NoTableTag = _T("(No Table Available)");

BEGIN_MESSAGE_MAP(TADlgTableSourcePage, CPropertyPage)
	//{{AFX_MSG_MAP(TADlgTableSourcePage)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#pragma region Constructor
TADlgTableSourcePage::TADlgTableSourcePage(uint32_t inspectionId, uint32_t taskObjectId, int id)
	: CPropertyPage(id)
	, m_InspectionID(inspectionId)
	, m_TaskObjectID(taskObjectId)
{
}

TADlgTableSourcePage::~TADlgTableSourcePage()
{
}
#pragma endregion Constructor

#pragma region Protected Methods
void TADlgTableSourcePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TADlgTableSourcePage)
	DDX_Control(pDX, IDC_COMBO1, m_availableSourceTableCB);
	//}}AFX_DATA_MAP
}

BOOL TADlgTableSourcePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	RetrieveAvailableList();

	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getinputdatarequest();
	pRequest->set_objectid(m_TaskObjectID);
	pRequest->set_embeddedid(SvPb::SourceTableInputEId);
	pRequest->set_desired_first_object_type_for_connected_name(SvPb::SVToolSetObjectType);
	pRequest->set_exclude_first_object_name_in_conntected_name(true);

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (S_OK != hr || false == responseCmd.has_getinputdataresponse())
	{
		Log_Assert(false);
		return false;
	}

	m_availableSourceTableCB.Init(m_availableList, responseCmd.getinputdataresponse().data().connected_objectdottedname(), NoTableTag);

	UpdateData(false); // set data to dialog

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zur�ckgeben
}

void TADlgTableSourcePage::OnSelchangeCombo1()
{
	UpdateData(TRUE); // get data from dialog

	int index = m_availableSourceTableCB.GetCurSel();
	if (LB_ERR != index)
	{
		CString tableName;
		m_availableSourceTableCB.GetLBText(index, tableName);
		if (!tableName.IsEmpty() && tableName != NoTableTag)
		{
			HRESULT hr = ConnectToObject(SvPb::SourceTableInputEId, std::string(tableName));
			if (S_OK != hr)
			{
				SvDef::StringVector msgList;
				msgList.push_back(SvDef::cInputTag_SourceTable);
				msgList.push_back(std::string(tableName));
				SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
				Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ConnectTableSourceFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
			}
		}
	}
}
#pragma endregion Protected Methods

#pragma region Private Methods
HRESULT TADlgTableSourcePage::RetrieveAvailableList()
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getavailableobjectsrequest();
	auto* pTreeSearchParameter = pRequest->mutable_tree_search();
	pTreeSearchParameter->set_search_start_id(m_InspectionID);
	pTreeSearchParameter->mutable_type_info()->set_objecttype(SvPb::TableObjectType);
	pTreeSearchParameter->mutable_isbeforetoolmethod()->set_toolid(m_TaskObjectID);
	pTreeSearchParameter->set_pre_search_start_id(m_TaskObjectID);
	pRequest->set_desired_first_object_type_for_name(SvPb::SVToolSetObjectType);
	pRequest->set_exclude_first_object_name_in_name(true);


	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	SvUl::NameObjectIdList availableList;
	if (S_OK == hr && responseCmd.has_getavailableobjectsresponse())
	{
		m_availableList = SvCmd::convertNameObjectIdList(responseCmd.getavailableobjectsresponse().list());
	}
	return hr;
}

class ByName
{
	std::string m_name;
public:
	explicit ByName(const std::string& rName) : m_name(rName) {}
	bool operator()(const SvUl::NameObjectIdPair& rVal) const { return rVal.first == m_name; }
};

HRESULT TADlgTableSourcePage::ConnectToObject(SvPb::EmbeddedIdEnum id, const std::string& name)
{
	HRESULT hr = E_INVALIDARG;
	SvUl::NameObjectIdList::const_iterator it = std::find_if(m_availableList.begin(), m_availableList.end(), ByName(name));
	if (it != m_availableList.end())
	{
		SvPb::InspectionCmdRequest requestCmd;
		auto* pRequest = requestCmd.mutable_connecttoobjectrequest();
		pRequest->set_objectid(m_TaskObjectID);
		pRequest->set_embeddedid(id);
		pRequest->set_newconnectedid(it->second);
		pRequest->set_objecttype(SvPb::TableObjectType);

		hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, nullptr);
	}
	return hr;
}
#pragma endregion Private Methods
} //namespace SvOg

