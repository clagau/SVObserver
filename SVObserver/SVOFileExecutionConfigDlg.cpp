//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file SVOFileExecutionConfigDlg.cpp
/// This is the class for the General tab in the edit config dialog
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVObserver.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVMFCControls/SVFileDialog.h"
#include "SVOFileExecutionConfigDlg.h"
#include "SVOConfigAssistantDlg.h"
#include "SVOResource/ConstGlobalSvOr.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr static LPCTSTR ScriptFileFilters = _T("Commandline files/ Batch files (*.bat;*.cmd)|*.bat;*.cmd||");
constexpr static LPCTSTR ScriptFileExt = _T("bat;cmd");

SVOFileExecutionConfigDlg::SVOFileExecutionConfigDlg(CWnd* /*=nullptr*/)
	: CPropertyPage(SVOFileExecutionConfigDlg::IDD), m_pParent(nullptr)
{
}

SVOFileExecutionConfigDlg::~SVOFileExecutionConfigDlg()
{
}

void SVOFileExecutionConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, ID_PRE_FILE_EXEC, m_edtPrePath);
	DDX_Text(pDX, ID_POST_FILE_EXEC, m_edtPostPath);
	DDX_Text(pDX, IDC_FILE_EXEC_MESSAGE, m_messageField);
}

BEGIN_MESSAGE_MAP(SVOFileExecutionConfigDlg, CDialog)
	ON_BN_CLICKED(IDC_ADD_PRE_FILE_EXEC, OnBtnAddPre)
	ON_BN_CLICKED(IDC_ADD_POST_FILE_EXEC, OnBtnAddPost)
	ON_BN_CLICKED(IDC_REMOVE_PRE_FILE_EXEC, OnBtnRemovePre)
	ON_BN_CLICKED(IDC_REMOVE_POST_FILE_EXEC, OnBtnRemovePost)
	ON_EN_KILLFOCUS(ID_PRE_FILE_EXEC, OnEditPreTextfield)
	ON_EN_KILLFOCUS(ID_POST_FILE_EXEC, OnEditPostTextfield)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()


BOOL SVOFileExecutionConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pParent = (SVOConfigAssistantDlg*)GetParent()->GetParent();
	SetupList();

	SetWindowContextHelpId(IDD + SvOr::HELPFILE_DLG_IDD_OFFSET);

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	if (pConfig != nullptr)
	{
		if (!pConfig->getPreRunExecutionFilePath().empty())
		{
			m_edtPrePath = std::filesystem::path(pConfig->getPreRunExecutionFilePath()).filename().c_str();
		}

		if (!pConfig->getPostRunExecutionFilePath().empty())
		{
			m_edtPostPath = std::filesystem::path(pConfig->getPostRunExecutionFilePath()).filename().c_str();
		}
	}

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void SVOFileExecutionConfigDlg::SetupList()
{

}

void SVOFileExecutionConfigDlg::OnBtnAddPre()
{
	DWORD dwFlags = OFN_DONTADDTORECENT | OFN_ENABLESIZING | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
	bool bFullAccess = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
	SvMc::SVFileDialog dlg(true, bFullAccess, ScriptFileExt, nullptr, dwFlags, ScriptFileFilters, this);
	dlg.m_ofn.lpstrTitle = _T("Select File");

	if (dlg.DoModal() == IDOK)
	{
		std::string PathName = dlg.GetPathName().GetString();
		copyAndAddToConfig(PathName, SVOFileExecutionConfigDlg::ProcessPosition::PreRun);

	}
}

void SVOFileExecutionConfigDlg::OnBtnAddPost()
{
	DWORD dwFlags = OFN_DONTADDTORECENT | OFN_ENABLESIZING | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
	bool bFullAccess = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
	SvMc::SVFileDialog dlg(true, bFullAccess, ScriptFileExt, nullptr, dwFlags, ScriptFileFilters, this);
	dlg.m_ofn.lpstrTitle = _T("Select File");

	if (dlg.DoModal() == IDOK)
	{
		std::string PathName = dlg.GetPathName().GetString();
		copyAndAddToConfig(PathName, SVOFileExecutionConfigDlg::ProcessPosition::PostRun);

	}
}

void SVOFileExecutionConfigDlg::OnBtnRemovePre()
{
	clearTextfieldAndRemoveFromConfig(SVOFileExecutionConfigDlg::ProcessPosition::PreRun);
}

void SVOFileExecutionConfigDlg::OnBtnRemovePost()
{
	clearTextfieldAndRemoveFromConfig(SVOFileExecutionConfigDlg::ProcessPosition::PostRun);
}

void SVOFileExecutionConfigDlg::OnEditPreTextfield()
{
	UpdateData(TRUE);
	std::string PathName(m_edtPrePath);
	copyAndAddToConfig(PathName, SVOFileExecutionConfigDlg::ProcessPosition::PreRun);
	
}

void SVOFileExecutionConfigDlg::OnEditPostTextfield()
{
	UpdateData(TRUE);
	std::string PathName(m_edtPostPath);
	copyAndAddToConfig(PathName, SVOFileExecutionConfigDlg::ProcessPosition::PostRun);
}

void SVOFileExecutionConfigDlg::copyAndAddToConfig(std::string path, int preOrPost)
{
	m_messageField = "";

	if (std::filesystem::exists(path))
	{
		SVConfigurationObject* pConfig(nullptr);
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
		
		if (pConfig != nullptr)
		{
			CString runDirectory = SvStl::GlobalPath::Inst().GetRunPath().c_str();
			CString destinationPath = runDirectory + "\\";
			destinationPath += std::filesystem::path(path).filename().c_str();
		
			if (SVOFileExecutionConfigDlg::ProcessPosition::PreRun == preOrPost)
			{	
				pConfig->setPreRunExecutionFilePath(destinationPath);
				m_edtPrePath = std::filesystem::path(path).filename().c_str();
			}
			else if (SVOFileExecutionConfigDlg::ProcessPosition::PostRun == preOrPost)
			{
				pConfig->setPostRunExecutionFilePath(destinationPath);
				m_edtPostPath = std::filesystem::path(path).filename().c_str();
			}

			::CopyFile(path.c_str(), destinationPath, false);
			TheSVObserverApp.AddFileToConfig(destinationPath);
		}
	}

	createErrorMessage();


	UpdateData(FALSE);
}

void SVOFileExecutionConfigDlg::createErrorMessage()
{
	std::vector<std::string> missingFiles;
	
	CString runDirectory = SvStl::GlobalPath::Inst().GetRunPath().c_str();

	std::string prePathName;
	std::string postPathName;

	prePathName = m_edtPrePath.FindOneOf("\\") == -1 ?  runDirectory + "\\" + m_edtPrePath : m_edtPrePath;
	postPathName = m_edtPostPath.FindOneOf("\\") == -1 ? runDirectory + "\\" + m_edtPostPath : m_edtPostPath;


	if (!std::filesystem::exists(prePathName) && prePathName.length() != 0)
	{
		missingFiles.push_back("preRunExecution");
	}

	if (!std::filesystem::exists(postPathName) && postPathName.length() != 0)
	{
		missingFiles.push_back("postRunExecution");
	}
	
	CString message = "";
	
	std::string stringOfMissingFiles;
	if (missingFiles.size() >= 1)
	{
		stringOfMissingFiles = std::accumulate(missingFiles.begin(), missingFiles.end(), std::string(),
			[](const std::string& ss, const std::string& s)
			{
				return ss.empty() ? s : ss + ", " + s;
			});

		message.Format("File for %s does not exist !", stringOfMissingFiles.c_str());
	}
	m_messageField = message;
	UpdateData(FALSE);


	
}

void SVOFileExecutionConfigDlg::clearTextfieldAndRemoveFromConfig(int preOrPost)
{
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (pConfig != nullptr)
	{
		CString runDirectory = SvStl::GlobalPath::Inst().GetRunPath().c_str();
		CString filename = "";
	
		if (SVOFileExecutionConfigDlg::ProcessPosition::PreRun == preOrPost)
		{	
			filename = m_edtPrePath;
			m_edtPrePath = "";
			pConfig->setPreRunExecutionFilePath("");
		}
		else if (SVOFileExecutionConfigDlg::ProcessPosition::PostRun == preOrPost)
		{
			filename = m_edtPostPath;
			m_edtPostPath = "";
			pConfig->setPostRunExecutionFilePath("");
		}

		CString fullPath = runDirectory + "\\" + filename;
		std::remove(fullPath);
		TheSVObserverApp.RemoveFileFromConfig(fullPath);

		UpdateData(FALSE);
		createErrorMessage();
	}
}