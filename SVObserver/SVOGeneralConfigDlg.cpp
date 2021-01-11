//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file SVOGeneralConfigDlg.cpp
/// This is the class for the General tab in the edit config dialog
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVObserver.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVMFCControls/SVFileDialog.h"
#include "SVOGeneralConfigDlg.h"
#include "SVOConfigAssistantDlg.h"
#include "SVOResource/ConstGlobalSvOr.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr static LPCTSTR ScriptFileFilters = _T("All Inspection Export Files (*.bat;*.cmd)|*.bat;*.cmd||");
constexpr static LPCTSTR ScriptFileExt = _T("bat;cmd");

SVOGeneralConfigDlg::SVOGeneralConfigDlg(CWnd* /*=nullptr*/)
	: CPropertyPage(SVOGeneralConfigDlg::IDD), m_pParent(nullptr)
{
}

SVOGeneralConfigDlg::~SVOGeneralConfigDlg()
{
}

void SVOGeneralConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_PREBATCH, m_edtPrePath);
	DDX_Text(pDX, IDC_EDT_POSTBATCH, m_edtPostPath);
	DDX_Text(pDX, IDC_GENERAL_TAB_MESSAGE, m_messageField);
}

BEGIN_MESSAGE_MAP(SVOGeneralConfigDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_ADD_PRE, OnBtnAddPre)
	ON_BN_CLICKED(IDC_BTN_ADD_POST, OnBtnAddPost)
	ON_BN_CLICKED(IDC_BTN_REMOVE_PRE, OnBtnRemovePre)
	ON_BN_CLICKED(IDC_BTN_REMOVE_POST, OnBtnRemovePost)
	ON_EN_KILLFOCUS(IDC_EDT_PREBATCH, OnEditPreTextfield)
	ON_EN_KILLFOCUS(IDC_EDT_POSTBATCH, OnEditPostTextfield)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVOTriggerDeviceDlg message handlers

BOOL SVOGeneralConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pParent = (SVOConfigAssistantDlg*)GetParent()->GetParent();
	SetupList();

	SetWindowContextHelpId(IDD + SvOr::HELPFILE_DLG_IDD_OFFSET);

	if (std::filesystem::exists(SvStl::GlobalPath::Inst().GetRunPath() + "\\preBatch.bat"))
	{
		m_edtPrePath = "preBatch.bat";
	}

	if (std::filesystem::exists(SvStl::GlobalPath::Inst().GetRunPath() + "\\postBatch.bat"))
	{
		m_edtPostPath = "postBatch.bat";
	}

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void SVOGeneralConfigDlg::SetupList()
{

}

void SVOGeneralConfigDlg::OnBtnAddPre()
{
	DWORD dwFlags = OFN_DONTADDTORECENT | OFN_ENABLESIZING | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	bool bFullAccess = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
	SvMc::SVFileDialog dlg(true, bFullAccess, ScriptFileExt, nullptr, dwFlags, ScriptFileFilters, this);
	dlg.m_ofn.lpstrTitle = _T("Select File");

	if (dlg.DoModal() == IDOK)
	{
		std::string PathName = dlg.GetPathName();
		copyAndAddToConfig(PathName, SVOGeneralConfigDlg::ProcessPosition::PreBatch);

	}
}

void SVOGeneralConfigDlg::OnBtnAddPost()
{
	DWORD dwFlags = OFN_DONTADDTORECENT | OFN_ENABLESIZING | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	bool bFullAccess = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
	SvMc::SVFileDialog dlg(true, bFullAccess, ScriptFileExt, nullptr, dwFlags, ScriptFileFilters, this);
	dlg.m_ofn.lpstrTitle = _T("Select File");

	if (dlg.DoModal() == IDOK)
	{
		std::string PathName = dlg.GetPathName();
		copyAndAddToConfig(PathName, SVOGeneralConfigDlg::ProcessPosition::PostBatch);

	}
}

void SVOGeneralConfigDlg::OnBtnRemovePre()
{
	clearTextfieldAndRemoveFromConfig(SVOGeneralConfigDlg::ProcessPosition::PreBatch);
}

void SVOGeneralConfigDlg::OnBtnRemovePost()
{
	clearTextfieldAndRemoveFromConfig(SVOGeneralConfigDlg::ProcessPosition::PostBatch);
}

void SVOGeneralConfigDlg::OnEditPreTextfield()
{
	UpdateData(TRUE);
	std::string PathName(m_edtPrePath);
	copyAndAddToConfig(PathName, SVOGeneralConfigDlg::ProcessPosition::PreBatch);
	
}

void SVOGeneralConfigDlg::OnEditPostTextfield()
{
	UpdateData(TRUE);
	std::string PathName(m_edtPostPath);
	copyAndAddToConfig(PathName, SVOGeneralConfigDlg::ProcessPosition::PostBatch);
}

void SVOGeneralConfigDlg::copyAndAddToConfig(std::string path, int preOrPost)
{
	m_messageField = "";

	if (std::filesystem::exists(path))
	{
		// TODO: Should the name in the textfield be the original filename or the post/pre deafualt file name
		CString runDirectory = SvStl::GlobalPath::Inst().GetRunPath().c_str();
		CString destinationPath = "";

		if (SVOGeneralConfigDlg::ProcessPosition::PreBatch == preOrPost)
		{
			destinationPath = runDirectory + "\\preBatch.bat";
			m_edtPrePath = "preBatch.bat";
		}
		else if (SVOGeneralConfigDlg::ProcessPosition::PostBatch == preOrPost)
		{
			destinationPath = runDirectory + "\\postBatch.bat";
			m_edtPostPath = "postBatch.bat";
		}

		::CopyFile(path.c_str(), destinationPath, false);
		TheSVObserverApp.AddFileToConfig(destinationPath);
	}
	else
	{
		m_messageField = "File doesn't exist !!!";
	}

	UpdateData(FALSE);
}

void SVOGeneralConfigDlg::clearTextfieldAndRemoveFromConfig(int preOrPost)
{
	CString runDirectory = SvStl::GlobalPath::Inst().GetRunPath().c_str();
	CString filename = "";
	
	if (SVOGeneralConfigDlg::ProcessPosition::PreBatch == preOrPost)
	{	
		filename = m_edtPrePath;
		m_edtPrePath = "";
	}
	else if (SVOGeneralConfigDlg::ProcessPosition::PostBatch == preOrPost)
	{
		filename = m_edtPostPath;
		m_edtPostPath = "";
	}

	CString fullPath = runDirectory + "\\" + filename;
	std::remove(fullPath);
	TheSVObserverApp.RemoveFileFromConfig(fullPath);

	UpdateData(FALSE);
}