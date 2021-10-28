//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file SVOFileExecutionConfigDlg.cpp
/// This is the class for the General tab in the edit config dialog
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVSecurity/SVSecurityManager.h"
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

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void SVOFileExecutionConfigDlg::SetupList()
{
	SetWindowContextHelpId(IDD + SvOr::HELPFILE_DLG_IDD_OFFSET);
	if (m_pParent->GetPreExecutionFilePath().empty() == false)
	{
		m_edtPrePath = std::filesystem::path(m_pParent->GetPreExecutionFilePath()).filename().c_str();
	}

	if (m_pParent->GetPostExecutionFilePath().empty() == false)
	{
		m_edtPostPath = std::filesystem::path(m_pParent->GetPostExecutionFilePath()).filename().c_str();
	}

	UpdateData(FALSE);
}

void SVOFileExecutionConfigDlg::OnBtnAddPre()
{
	DWORD dwFlags = OFN_DONTADDTORECENT | OFN_ENABLESIZING | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
	bool bFullAccess = TheSecurityManager().SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
	SvMc::SVFileDialog dlg(true, bFullAccess, ScriptFileExt, nullptr, dwFlags, ScriptFileFilters, this);
	dlg.m_ofn.lpstrTitle = _T("Select File");

	if (dlg.DoModal() == IDOK)
	{
		std::string PathName = dlg.GetPathName().GetString();
		addExecutionFileToConfig(PathName, SVOFileExecutionConfigDlg::ProcessPosition::PreRun);

	}
}

void SVOFileExecutionConfigDlg::OnBtnAddPost()
{
	DWORD dwFlags = OFN_DONTADDTORECENT | OFN_ENABLESIZING | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
	bool bFullAccess = TheSecurityManager().SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
	SvMc::SVFileDialog dlg(true, bFullAccess, ScriptFileExt, nullptr, dwFlags, ScriptFileFilters, this);
	dlg.m_ofn.lpstrTitle = _T("Select File");

	if (dlg.DoModal() == IDOK)
	{
		std::string PathName = dlg.GetPathName().GetString();
		addExecutionFileToConfig(PathName, SVOFileExecutionConfigDlg::ProcessPosition::PostRun);

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
	addExecutionFileToConfig(PathName, SVOFileExecutionConfigDlg::ProcessPosition::PreRun);
	
}

void SVOFileExecutionConfigDlg::OnEditPostTextfield()
{
	UpdateData(TRUE);
	std::string PathName(m_edtPostPath);
	addExecutionFileToConfig(PathName, SVOFileExecutionConfigDlg::ProcessPosition::PostRun);
}

void SVOFileExecutionConfigDlg::addExecutionFileToConfig(std::string path, ProcessPosition preOrPost)
{
	CString runDirectory = SvStl::GlobalPath::Inst().GetRunPath().c_str();

	if (path.find("\\") == std::string::npos)
	{
		runDirectory += "\\";
		path = std::string(runDirectory) + path;
	}

	if (ProcessPosition::PreRun == preOrPost)
	{	
		m_pParent->SetPreExecutionFilePath(path.c_str());
		m_edtPrePath = std::filesystem::path(path).filename().c_str();
	}
	else if (ProcessPosition::PostRun == preOrPost)
	{
		m_pParent->SetPostExecutionFilePath(path.c_str());
		m_edtPostPath = std::filesystem::path(path).filename().c_str();
	}

	m_pParent->ItemChanged(EXECUTION_DLG, "", ITEM_ACTION_REFRESH);
	UpdateData(FALSE);
}

void SVOFileExecutionConfigDlg::clearTextfieldAndRemoveFromConfig(ProcessPosition preOrPost)
{
		if (SVOFileExecutionConfigDlg::ProcessPosition::PreRun == preOrPost)
		{	
			m_pParent->SetPreExecutionFilePath("");
			m_edtPrePath = "";
		}
		else if (SVOFileExecutionConfigDlg::ProcessPosition::PostRun == preOrPost)
		{
			m_pParent->SetPostExecutionFilePath("");
			m_edtPostPath = "";
		}
		
		UpdateData(FALSE);
		m_pParent->ItemChanged(EXECUTION_DLG, "", ITEM_ACTION_REFRESH);
}