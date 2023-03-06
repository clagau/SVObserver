// ******************************************************************************
// * COPYRIGHT (c) 2005 by K�rber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
/// \file SVRegressionFileSelectDlg.cpp
// SVRegressionFileSelectDlg property page

#pragma region Includes
#include "stdafx.h"
#include "SVRegressionFileSelectDlg.h"
#include "SVRegressionFileSelectSheet.h"
#include "Definitions/TextDefinesSvDef.h"
#include "SVSecurity/SVSecurityManager.h"
#include "SVMessage/SVMessage.h"
#include "SVMFCControls/SVDlgFolder.h"
#include "SVMFCControls/SVFileDialog.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/MessageManager.h"
#include "Definitions/TextDefinesSvDef.h"
#pragma endregion Includes

#pragma region Declarations
constexpr int PathBufferLen = MAX_PATH + 2;
constexpr int MaxNumberCameraEntries = 4;
#pragma endregion Declarations 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(SVRegressionFileSelectDlg, CPropertyPage)
strMatrix SVRegressionFileSelectDlg::m_PreviousFolderSettings(MaxNumberCameraEntries, std::vector<std::string>());
std::vector<int> SVRegressionFileSelectDlg::m_PreviousSelctedRadius(MaxNumberCameraEntries, RegressionFileEnum::RegSubDirectories);
SVRegressionFileSelectDlg::SVRegressionFileSelectDlg(LPCTSTR lptstrDialogName, bool isCamera, uint32_t toolId)
	: CPropertyPage(SVRegressionFileSelectDlg::IDD)
	, m_isCamera(isCamera)
	, m_toolId(toolId)
	, m_RegTestFiles(_T(""))
	, m_iSelectFileRadio(RegressionFileEnum::RegSubDirectories)
	, m_pParent(nullptr)
{

	//{{AFX_DATA_INIT(SVRegressionFileSelectDlg)
	//}}AFX_DATA_INIT
	m_strCaption = lptstrDialogName;
	m_psp.pszTitle = m_strCaption;
	m_psp.dwFlags |= PSP_USETITLE;
}

SVRegressionFileSelectDlg::~SVRegressionFileSelectDlg()
{}

void SVRegressionFileSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVRegressionFileSelectDlg)
	DDX_Control(pDX, IDC_BTN_REG_TEST_BROWSE_FILES, m_btnBrowseFiles);
	
	DDX_Control(pDX, IDC_LIST_REG_SELECTED_FOLDER, m_SelectFolderList);
	DDX_Control(pDX, IDC_EDIT_REG_SELECTED_FILES, m_EditRegTestFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVRegressionFileSelectDlg, CPropertyPage)
	//{{AFX_MSG_MAP(SVRegressionFileSelectDlg)
	ON_BN_CLICKED(IDC_BTN_REG_TEST_BROWSE_FILES, OnBtnRegTestBrowseFiles)
	ON_BN_CLICKED(IDC_RADIO_REG_LIST, OnClickRadioList)
	ON_BN_CLICKED(IDC_RADIO_REG_NONE, OnClickRadioNone)
	ON_BN_CLICKED(IDC_RADIO_REG_SINGLE, OnClickRadioSingle)
	ON_BN_CLICKED(IDC_RADIO_REG_DIRECTORY, OnClickRadioDirectory)
	ON_BN_CLICKED(IDC_RADIO_REG_SUB_DIRECTORIES, OnClickRadioSubDirectory)
	ON_BN_CLICKED(IDC_RADIO_REG_MULTDIR, OnClickRadioMultDir)
	ON_BN_CLICKED(IDC_BTN_REMOVE, OnRemovePage)
	ON_BN_CLICKED(IDC_BTN_REG_FOLDER_DEL, OnDeleteFolder)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVRegressionFileSelectDlg message handlers

void SVRegressionFileSelectDlg::SetDlgTitle(LPCTSTR lpszTitle)
{
	m_DialogName = lpszTitle;
}


void SVRegressionFileSelectDlg::OnBtnRegTestBrowseFiles()
{
	//get last regression path for this camera from registry...
	std::string KeyName = std::format(_T("LastPath_{}"), m_CameraNumber);
	m_RegistryPath = AfxGetApp()->GetProfileString(_T("RegressionTest"), KeyName.c_str(), SvStl::GlobalPath::Inst().GetTempPath().c_str());
	bool bFullAccess = TheSecurityManager().SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);

	switch (m_iSelectFileRadio)
	{
		case RegressionFileEnum::RegSingleFile:
		case RegressionFileEnum::RegFileList:
		case RegressionFileEnum::RegSingleDirectory:
			ShowSelectFileDlg(bFullAccess);
			break;
		case RegressionFileEnum::RegSubDirectories:
			ShowSelectDirectoryDlg(bFullAccess);
			break;
		case RegressionFileEnum::RegMultDirectories:
			ShowSelectMultDirectoryDlg(bFullAccess);
			break;

		default:
			//nothing to do
			break;
	}
	SetCurrentDirectory(SvStl::GlobalPath::Inst().GetRunPath().c_str());
	int iPos = m_RegTestFiles.ReverseFind(_T('\\'));
	if (iPos != -1) //only write out registry entry if the path is not empty.
	{
		CString sTmpDirName = m_RegTestFiles;

		if (filenameMatchesImageFormat(m_RegTestFiles.GetBuffer(), ImageFileFormat::any))
		{	//cut file name to save only path
			sTmpDirName = m_RegTestFiles.Left(iPos);
		}

		AfxGetApp()->WriteProfileString(_T("RegressionTest"), KeyName.c_str(), sTmpDirName);
	}
	ShowControls();

}

void SVRegressionFileSelectDlg::ShowControls()
{
	bool showFolder = (m_iSelectFileRadio == RegressionFileEnum::RegMultDirectories);
	if (m_iSelectFileRadio == RegressionFileEnum::RegNone)
	{
		m_EditRegTestFile.ShowWindow(SW_HIDE);
		m_SelectFolderList.ShowWindow(SW_HIDE);
	}
	else
	{
		m_EditRegTestFile.ShowWindow(showFolder ? SW_HIDE : SW_SHOW);
		m_SelectFolderList.ShowWindow(showFolder ? SW_SHOW : SW_HIDE);
		UpdateFolderList();
	}
	
	GetDlgItem(IDC_BTN_REG_TEST_BROWSE_FILES)->SetWindowText(showFolder ? "add ..." : "...");
	
	GetDlgItem(IDC_BTN_REG_FOLDER_DEL)->ShowWindow(showFolder ? SW_SHOW : SW_HIDE);
	UpdateTestFile();
	

}

void SVRegressionFileSelectDlg::UpdateFolderList()
{
	if (m_iSelectFileRadio == RegressionFileEnum::RegMultDirectories)
	{

		m_SelectFolderList.ResetContent();
		for (const auto& name : m_folders)
		{
			m_SelectFolderList.AddString(name.c_str());
		}
	}
}

void SVRegressionFileSelectDlg::UpdateRadioButton()
{
	UINT id = m_iSelectFileRadio + IDC_RADIO_REG_LIST;
	CheckRadioButton(IDC_RADIO_REG_LIST, IDC_RADIO_REG_NONE, id);
}


void SVRegressionFileSelectDlg::UpdateTestFile()
{
	m_EditRegTestFile.SetWindowText(m_RegTestFiles.GetString());
}
void SVRegressionFileSelectDlg::OnDeleteFolder()
{
	int  nIndex = m_SelectFolderList.GetCurSel();
	if (nIndex != LB_ERR)
	{
		CString cstext;
		m_SelectFolderList.GetText(nIndex, cstext);
		std::string folder = cstext.GetString();
		std::erase(m_folders, folder);
		m_SelectFolderList.DeleteString(nIndex);
	}
}

BOOL SVRegressionFileSelectDlg::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	int  Pos = m_strCaption.ReverseFind(_T('_'));
	if (Pos != -1)
	{
		CString sTemp = m_strCaption.Mid(Pos + 1);
		m_CameraNumber = _ttoi(sTemp.GetString());
		m_CameraNumber = m_CameraNumber % MaxNumberCameraEntries;
	}
	
	m_pParent = dynamic_cast<SVRegressionFileSelectSheet*>(GetParent());
	m_pParent->m_psh.dwFlags |= PSH_NOAPPLYNOW;

	GetDlgItem(IDC_BTN_REMOVE)->ShowWindow(m_isCamera ? SW_HIDE : SW_SHOW);
	m_iSelectFileRadio = m_PreviousSelctedRadius[m_CameraNumber];
	if (m_folders.size() == 0 && m_PreviousFolderSettings[m_CameraNumber].size() > 0)
	{
		auto inserter = std::back_inserter(m_folders);
		std::copy(m_PreviousFolderSettings[m_CameraNumber].begin(), m_PreviousFolderSettings[m_CameraNumber].end(), inserter);
	}
	
	ShowControls();
	UpdateRadioButton();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

RegressionFileEnum SVRegressionFileSelectDlg::GetFileSelectType()
{
	UpdateData(TRUE);
	return (RegressionFileEnum)m_iSelectFileRadio;
}

CString SVRegressionFileSelectDlg::GetSelectedFile()
{
	return m_RegTestFiles;
}

CString SVRegressionFileSelectDlg::GetPageName()
{
	return m_strCaption;
}

void SVRegressionFileSelectDlg::OnClickRadio(int RegEnum)
{
	m_iSelectFileRadio = RegEnum;
	m_PreviousSelctedRadius[m_CameraNumber] = m_iSelectFileRadio;
	ShowControls();
}


void SVRegressionFileSelectDlg::OnClickRadioList()
{
	OnClickRadio(RegFileList);
}

void SVRegressionFileSelectDlg::OnClickRadioSingle()
{
	OnClickRadio(RegSingleFile);
}
void SVRegressionFileSelectDlg::OnClickRadioDirectory()
{
	OnClickRadio(RegSingleDirectory);
}
void SVRegressionFileSelectDlg::OnClickRadioSubDirectory()
{
	OnClickRadio(RegSubDirectories);
}
void SVRegressionFileSelectDlg::OnClickRadioMultDir()
{
	OnClickRadio(RegMultDirectories);
}
void SVRegressionFileSelectDlg::OnClickRadioNone()
{
	OnClickRadio(RegNone);
}



void SVRegressionFileSelectDlg::OnRemovePage()
{
	m_pParent->removeFilePage(this);
}


void SVRegressionFileSelectDlg::SetRegressionData(RegressionTestStruct* pDataStruct)
{
	//set data from the struct...
	m_iSelectFileRadio = pDataStruct->iFileMethod;
	m_RegTestFiles = pDataStruct->firstFilepath.c_str();

	UpdateData(FALSE);
}

void SVRegressionFileSelectDlg::ShowSelectFileDlg(bool bFullAccess)
{
	SvMc::SVFileDialog dlg(true, bFullAccess, nullptr, nullptr, 0, SvDef::fileDlgFilterMilSupportedImageFilesTypes, nullptr);
	dlg.m_ofn.lpstrTitle = _T("Select File");

	TCHAR FileName[PathBufferLen];
	_tcscpy_s(FileName, PathBufferLen, m_RegTestFiles.GetString());
	if (m_RegTestFiles.IsEmpty())
	{
		//nothing has been set... use what is in the registry
		dlg.m_ofn.lpstrInitialDir = m_RegistryPath.c_str();
	}
	else
	{
		///setting lpstrFile instead of lpstrInitialDir avoids strange Windows7 behavior of CFileDialog
		dlg.m_ofn.lpstrFile = FileName;
		dlg.m_ofn.nMaxFile = PathBufferLen;
		dlg.m_ofn.lpstrInitialDir = nullptr;
	}

	if (dlg.DoModal() == IDOK)
	{
		m_RegTestFiles = dlg.GetPathName();
		if (!m_RegTestFiles.IsEmpty())
		{
			if (ImageFileFormat::invalid == inferMilImageFileFormat(m_RegTestFiles.GetString()))
			{
				SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
				Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_InvalidImageFileType, SvStl::SourceFileParams(StdMessageParams));
				m_RegTestFiles = _T("");
			}
		}
	}
}

void SVRegressionFileSelectDlg::ShowSelectDirectoryDlg(bool bFullAccess)
{
	
	SvMc::SVDlgFolder dlg(bFullAccess, m_RegistryPath.c_str());
	dlg.InitDlgFolder(_T("OK"), _T("Select Folder"));

	INT_PTR rc = dlg.DoModal();
	if (IDOK == rc)
	{
		m_RegTestFiles = dlg.GetPathName();
	}

}
void SVRegressionFileSelectDlg::ShowSelectMultDirectoryDlg(bool bFullAccess)
{
	SvMc::SVDlgFolder dlg(bFullAccess, m_RegistryPath.c_str(), this);
	dlg.InitDlgFolder(_T("OK"), _T("Select Folder"));
	if (dlg.DoModal() == IDOK)
	{
		m_RegTestFiles = dlg.GetPathName();
		std::string folder = m_RegTestFiles.GetString();
		if (std::ranges::find(m_folders, folder) == m_folders.end())
			m_folders.push_back(folder);

		m_PreviousFolderSettings[m_CameraNumber].clear();
		auto inserter = std::back_inserter(m_PreviousFolderSettings[m_CameraNumber]);
		std::copy(m_folders.begin(), m_folders.end(), inserter);

		if (m_folders.size())
		{
			m_RegTestFiles = m_folders[0].c_str();
		}

	}

}