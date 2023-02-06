//*****************************************************************************
/// \copyright (c) 2023,2023 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// This is the dialog of the Edit Modules.
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "EditModulesDialog.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "SVProtoBuf/ConverterHelper.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
SvOg::ModuleDataList getModuleData()
{
	SvOg::ModuleDataList dataList;
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	requestCmd.mutable_getmodulelistrequest();
	HRESULT hr = SvCmd::InspectionCommands(SvDef::InvalidObjectId, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getmodulelistresponse())
	{
		for (const auto& rEntry : responseCmd.getmodulelistresponse().list())
		{
			std::vector<std::pair<time_t, std::string>> m_historyList;
			std::transform(rEntry.historylist().begin(), rEntry.historylist().end(), std::back_inserter(m_historyList),
						   [](const auto& rHistory) -> std::pair<time_t, std::string> { return {rHistory.time(), rHistory.comment()}; });
			dataList.emplace_back(rEntry.name(), SVGUID {rEntry.guid()}, rEntry.number_of_instances(), rEntry.comment(), std::move(m_historyList));
		}
	}
	return dataList;
}
}

namespace SvOg
{
BEGIN_MESSAGE_MAP(EditModulesDialog, CDialog)
	ON_LBN_SELCHANGE(IDC_MODULE_LIST, OnSelchangeList)
	ON_BN_CLICKED(IDC_DELETE, OnDeleteModule)
END_MESSAGE_MAP()
#pragma endregion Declarations

#pragma region Constructor
EditModulesDialog::EditModulesDialog(CWnd* pParent /*nullptr*/)
	: CDialog(EditModulesDialog::IDD, pParent)
{

}

EditModulesDialog::~EditModulesDialog()
{}
#pragma endregion Constructor

#pragma region Protected Methods
void EditModulesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MODULE_LIST, m_moduleListBox);
	DDX_Text(pDX, IDC_HISTORY_TEXT, m_strHistory);
	DDX_Text(pDX, IDC_EDIT_COMMENT, m_strComment);
}

BOOL EditModulesDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	initModuleListControl();

	GetDlgItem(IDC_HISTORY_TEXT)->EnableWindow(false);
	GetDlgItem(IDC_EDIT_COMMENT)->EnableWindow(false);
	GetDlgItem(IDC_DELETE)->EnableWindow(false);
	GetDlgItem(IDC_EXPORT)->EnableWindow(false);
	GetDlgItem(IDC_IMPORT)->EnableWindow(false);

	UpdateData(FALSE);

	return TRUE;
}

void EditModulesDialog::OnSelchangeList()
{
	int index = m_moduleListBox.GetCurSel() - 1;
	if (0 <= index && m_moduleList.size() > index)
	{
		m_strComment = m_moduleList[index].m_comment.c_str();
		m_strHistory = "";
		for (const auto& rPair : m_moduleList[index].m_historyList)
		{
			char mbstr[100];
			std::strftime(mbstr, sizeof(mbstr), "%c", std::localtime(&rPair.first));
			m_strHistory += (std::string {mbstr} + ": " + rPair.second + "\n").c_str();
		}
		GetDlgItem(IDC_DELETE)->EnableWindow(0 == m_moduleList[index].m_numberOfUse);
	}
	else
	{
		m_strComment = "";
		m_strHistory = "";
		GetDlgItem(IDC_DELETE)->EnableWindow(false);
	}	
	UpdateData(FALSE);
}

void EditModulesDialog::OnDeleteModule()
{
	int index = m_moduleListBox.GetCurSel() - 1;
	if (0 <= index && m_moduleList.size() > index && 0 == m_moduleList[index].m_numberOfUse)
	{
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		requestCmd.mutable_deletemodulerequest()->set_guid(m_moduleList[index].m_guid.ToString());
		HRESULT hr = SvCmd::InspectionCommands(SvDef::InvalidObjectId, requestCmd, &responseCmd);
		if (S_OK != hr)
		{
			SvStl::MessageContainerVector errorMsgContainer = SvPb::convertProtobufToMessageVector(responseCmd.errormessage());
			if (errorMsgContainer.size() > 0)
			{
				SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
				Msg.setMessage(errorMsgContainer.at(0).getMessage());
			}
			else
			{
				Log_Assert(false);
			}
		}
		initModuleListControl();
	}
}

void EditModulesDialog::initModuleListControl()
{
	m_moduleList = getModuleData();
	SvUl::NameObjectIdList nameList;
	for (int i = 0; m_moduleList.size() > i; ++i)
	{
		nameList.emplace_back(m_moduleList[i].m_name + " (" + std::to_string(m_moduleList[i].m_numberOfUse) + ")", i);
	}
	m_moduleListBox.init(nameList, "<No Modules>");
	OnSelchangeList();
}
} //namespace SvOg