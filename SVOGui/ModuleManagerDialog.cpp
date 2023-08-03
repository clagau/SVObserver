//*****************************************************************************
/// \copyright (c) 2023,2023 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// This is the dialog of the Module Manager.
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ModuleManagerDialog.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVMFCControls/EnterStringDlg.h"
#include "SVSecurity/SVSecurityManager.h"
#include "SVMFCControls/SVFileDialog.h"
#include "SVMessage/SVMessage.h"
#include "FilesystemUtilities/FilepathUtilities.h"
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
BEGIN_MESSAGE_MAP(ModuleManagerDialog, CDialog)
	ON_LBN_SELCHANGE(IDC_MODULE_LIST, OnSelchangeList)
	ON_BN_CLICKED(IDC_DELETE, OnDeleteModule)
	ON_BN_CLICKED(IDC_IMPORT, OnImportModule)
	ON_BN_CLICKED(IDC_EXPORT, OnExportModule)
	ON_BN_CLICKED(IDC_RENAME, OnRenameModule)
	ON_BN_CLICKED(IDHELP, OnHelpButton)
END_MESSAGE_MAP()
#pragma endregion Declarations

#pragma region Constructor
ModuleManagerDialog::ModuleManagerDialog(CWnd* pParent /*nullptr*/)
	: CDialog(ModuleManagerDialog::IDD, pParent)
{

}

ModuleManagerDialog::~ModuleManagerDialog()
{}
#pragma endregion Constructor

#pragma region Protected Methods
void ModuleManagerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MODULE_LIST, m_moduleListBox);
	DDX_Text(pDX, IDC_HISTORY_TEXT, m_strHistory);
	DDX_Text(pDX, IDC_EDIT_COMMENT, m_strComment);
}

BOOL ModuleManagerDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	initModuleListControl();

	static_cast<CEdit*>(GetDlgItem(IDC_HISTORY_TEXT))->SetReadOnly(true);
	static_cast<CEdit*>(GetDlgItem(IDC_EDIT_COMMENT))->SetReadOnly(true);
	GetDlgItem(IDC_DELETE)->EnableWindow(false);
	GetDlgItem(IDC_EXPORT)->EnableWindow(false);
	GetDlgItem(IDC_IMPORT)->EnableWindow(true);

	UpdateData(FALSE);

	return TRUE;
}

void ModuleManagerDialog::OnSelchangeList()
{
	int index = getSelectedIndex();
	if (0 <= index && m_moduleList.size() > index)
	{
		m_strComment = m_moduleList[index].m_comment.c_str();
		m_strHistory = "";
		std::string guidForFirstLine {m_moduleList[index].m_guid.ToString()};
		for (const auto& rPair : m_moduleList[index].m_historyList)
		{
			char mbstr[100];
			std::strftime(mbstr, sizeof(mbstr), "%c", std::localtime(&rPair.first));
			
			m_strHistory = (std::string {mbstr} + ": " + rPair.second + guidForFirstLine + "\r\n").c_str() + m_strHistory;
			guidForFirstLine = "";
		}
		GetDlgItem(IDC_DELETE)->EnableWindow(0 == m_moduleList[index].m_numberOfUse);
		GetDlgItem(IDC_EXPORT)->EnableWindow(true);
		GetDlgItem(IDC_RENAME)->EnableWindow(true);
	}
	else
	{
		m_strComment = "";
		m_strHistory = "";
		GetDlgItem(IDC_DELETE)->EnableWindow(false);
		GetDlgItem(IDC_EXPORT)->EnableWindow(false);
		GetDlgItem(IDC_RENAME)->EnableWindow(false);
	}	
	UpdateData(FALSE);
}

void ModuleManagerDialog::OnDeleteModule()
{
	int index = getSelectedIndex();
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

std::string scanModuleName(const std::string& pathName)
{
	auto endPos = pathName.rfind(".svm");
	if (endPos == std::string::npos)
	{
		return {};
	}
	auto startPos = pathName.rfind("\\");
	if (startPos == std::string::npos)
	{
		startPos = 0;
	}
	else
	{
		++startPos;
	}
	return pathName.substr(startPos, endPos - startPos);
}

SvPb::InspectionCmdResponse importModule(const std::string& moduleName, const std::string& moduleContainerStr)
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_importmodulerequest();
	pRequest->set_name(moduleName);
	pRequest->set_datastring(moduleContainerStr);
	SvCmd::InspectionCommands(SvDef::InvalidObjectId, requestCmd, &responseCmd);
	return responseCmd;
}

void ModuleManagerDialog::OnImportModule()
{
	std::string data;
	std::string moduleName;

	static LPCTSTR ModuleFileFilters = _T("Module Import Files (*.svm)|*.svm||");
	static LPCTSTR ModuleFileExt = _T("svm");
	DWORD dwFlags = OFN_DONTADDTORECENT | OFN_ENABLESIZING | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	bool bFullAccess = TheSecurityManager().SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
	SvMc::SVFileDialog dlg(true, bFullAccess, ModuleFileExt, nullptr, dwFlags, ModuleFileFilters, this);
	dlg.m_ofn.lpstrTitle = _T("Select File");

	if (dlg.DoModal() == IDOK)
	{
		std::string PathName = dlg.GetPathName().GetString();
		moduleName = scanModuleName(PathName);

		try
		{
			data = SvFs::readContentFromFile(PathName, false);
		}
		catch (const SvStl::MessageContainer& rMsg)
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(rMsg.getMessage());
			assert(false);
			return;
		}
	}
	else
	{
		return;
	}

	auto response = importModule(moduleName, data);
	while (S_OK != response.hresult())
	{
		SvStl::MessageContainerVector errorMsgContainer = SvPb::convertProtobufToMessageVector(response.errormessage());
		if (errorMsgContainer.size() > 0)
		{
			if (SvStl::Tid_ModuleNameExistAlready == errorMsgContainer[0].getMessage().m_AdditionalTextId || SvStl::Tid_NameContainsInvalidChars == errorMsgContainer[0].getMessage().m_AdditionalTextId)
			{
				EnterStringDlg nameDlg {moduleName,  SvCmd::checkNewModuleName, "Name invalid or exists already, choose new Module Name", this};
				if (IDOK != nameDlg.DoModal())
				{
					return;
				}
				moduleName = nameDlg.getString();
				response = importModule(moduleName, data);
			}
			else
			{
				SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
				Msg.setMessage(errorMsgContainer.at(0).getMessage());
				return;
			}
		}
		else
		{
			Log_Assert(false);
			return;
		}
	}
	initModuleListControl();
}

void ModuleManagerDialog::OnExportModule()
{
	auto index = getSelectedIndex();
	if (0 <= index && m_moduleList.size() > index)
	{
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_exportmodulerequest();
		pRequest->set_guid(m_moduleList[index].m_guid.ToString());
		HRESULT hr = SvCmd::InspectionCommands(SvDef::InvalidObjectId, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_exportmoduleresponse())
		{
			bool bFullAccess = TheSecurityManager().SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
			constexpr const TCHAR* filter = _T("Module Files (*.svm)|*.svm||");
			SvMc::SVFileDialog dlg(false, bFullAccess, _T("svm"), nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter);
			dlg.m_ofn.lpstrTitle = _T("Set filename to save");

			INT_PTR rc = dlg.DoModal();
			if (IDOK == rc)
			{
				try
				{
					SvFs::writeStringToFile(std::string {dlg.GetPathName()}, responseCmd.exportmoduleresponse().datastring(), false);
				}
				catch (const SvStl::MessageContainer& rExp)
				{
					SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
					Msg.setMessage(rExp.getMessage());
				}
			}
		}
		else
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
	}
}

void ModuleManagerDialog::OnRenameModule()
{
	auto index = getSelectedIndex();
	if (0 <= index && m_moduleList.size() > index)
	{
		EnterStringDlg nameDlg {m_moduleList[index].m_name,  SvCmd::checkNewModuleName, "Enter new Module Name", this};
		if (IDOK != nameDlg.DoModal())
		{
			return;
		}
		auto newName = nameDlg.getString();
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_renamemodulerequest();
		pRequest->set_guid(m_moduleList[index].m_guid.ToString());
		pRequest->set_name(newName);
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

void ModuleManagerDialog::OnHelpButton()
{
	OnCommandHelp(0, GetWindowContextHelpId());
}

void ModuleManagerDialog::initModuleListControl()
{
	m_moduleListBox.ResetContent();
	m_moduleList = getModuleData();
	if (m_moduleList.size())
	{
		for (int i = 0; m_moduleList.size() > i; ++i)
		{
			m_moduleListBox.SetItemData(m_moduleListBox.AddString((m_moduleList[i].m_name + " (" + std::to_string(m_moduleList[i].m_numberOfUse) + ")").c_str()), i);
		}
	}
	else
	{
		m_moduleListBox.SetItemData(m_moduleListBox.AddString("<No Modules>"), 0);
	}
	
	OnSelchangeList();
}

int ModuleManagerDialog::getSelectedIndex()
{
	return static_cast<int>(m_moduleListBox.GetItemData(m_moduleListBox.GetCurSel()));
}
} //namespace SvOg