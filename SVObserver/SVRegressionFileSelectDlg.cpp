// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVRegressionFileSelectDlg
// * .File Name       : $Workfile:   SVRegressionFileSelectDlg.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   08 Oct 2014 13:46:16  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVRegressionFileSelectDlg.h"
#include "SVRegressionFileSelectSheet.h"
#include "SVObserver.h"
#include "SVMFCControls/SVDlgFolder.h"
#include "SVMFCControls/SVFileDialog.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#pragma region Declarations
constexpr int PathBufferLen = MAX_PATH +2;
constexpr int MaxNumberCameraEntries = 4;
#pragma endregion Declarations 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVRegressionFileSelectDlg property page

IMPLEMENT_DYNCREATE(SVRegressionFileSelectDlg, CPropertyPage)

SVRegressionFileSelectDlg::SVRegressionFileSelectDlg(LPCTSTR lptstrDialogName, bool isCamera, uint32_t toolId)
	: CPropertyPage(SVRegressionFileSelectDlg::IDD)
	, m_isCamera(isCamera)
	, m_toolId(toolId)
	, m_RegTestFiles(_T(""))
	, m_iSelectFileRadio(RegressionFileEnum::RegNone)
	, m_pParent(nullptr)
{
	
	//{{AFX_DATA_INIT(SVRegressionFileSelectDlg)
	//}}AFX_DATA_INIT
	m_strCaption = lptstrDialogName;
	m_psp.pszTitle = m_strCaption;
	m_psp.dwFlags |= PSP_USETITLE;
}

SVRegressionFileSelectDlg::~SVRegressionFileSelectDlg()
{
}

void SVRegressionFileSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVRegressionFileSelectDlg)
	DDX_Control(pDX, IDC_BTN_REG_TEST_BROWSE_FILES, m_btnBrowseFiles);
	DDX_Radio(pDX, IDC_RADIO_REG_LIST, m_iSelectFileRadio);
	DDX_Text(pDX, IDC_EDIT_REG_SELECTED_FILES, m_RegTestFiles);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVRegressionFileSelectDlg, CPropertyPage)
	//{{AFX_MSG_MAP(SVRegressionFileSelectDlg)
	ON_BN_CLICKED(IDC_BTN_REG_TEST_BROWSE_FILES, OnBtnRegTestBrowseFiles)
	ON_BN_CLICKED(IDC_RADIO_REG_LIST, OnRadioRegUpdate)
	ON_BN_CLICKED(IDC_RADIO_REG_NONE, OnRadioRegUpdate)
	ON_BN_CLICKED(IDC_RADIO_REG_SINGLE, OnRadioRegUpdate)
	ON_BN_CLICKED(IDC_RADIO_REG_DIRECTORY, OnRadioRegUpdate)
	ON_BN_CLICKED(IDC_RADIO_REG_SUB_DIRECTORIES, OnRadioRegUpdate)
	ON_BN_CLICKED(IDC_BTN_REMOVE, OnRemovePage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVRegressionFileSelectDlg message handlers

void SVRegressionFileSelectDlg::SetDlgTitle( LPCTSTR lpszTitle )
{
	m_DialogName = lpszTitle;
}

void SVRegressionFileSelectDlg::OnBtnRegTestBrowseFiles() 
{
	UpdateData(true);
	int CameraNumber(0); 
	int  Pos = m_strCaption.ReverseFind(_T('_'));
	if(Pos != -1)
	{		
		CString sTemp = m_strCaption.Mid(Pos +1);
		CameraNumber = _ttoi(sTemp.GetString());
		CameraNumber = CameraNumber %  MaxNumberCameraEntries; 
	}
	//get last regression path for this camera from registry...
	std::string KeyName = SvUl::Format( _T("LastPath_%i"), CameraNumber);
	m_RegistryPath = AfxGetApp()->GetProfileString(_T("RegressionTest"), KeyName.c_str(), SvStl::GlobalPath::Inst().GetTempPath().c_str());
	bool bFullAccess = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);

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
	default:
		//nothing to do
		break;
	}
	
	int iPos = m_RegTestFiles.ReverseFind(_T('\\'));
	if (iPos != -1) //only write out registry entry if the path is not empty.
	{
		CString sTmpDirName = m_RegTestFiles;
		if (0 == m_RegTestFiles.Right(4).CompareNoCase(_T(".bmp")))
		{	//cut file name to save only path
			sTmpDirName = m_RegTestFiles.Left(iPos);
		}
		
		AfxGetApp()->WriteProfileString(_T("RegressionTest"), KeyName.c_str(), sTmpDirName);
	}

	UpdateData(FALSE);
}

BOOL SVRegressionFileSelectDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_pParent = dynamic_cast<SVRegressionFileSelectSheet*>(GetParent());
	m_pParent->m_psh.dwFlags |= PSH_NOAPPLYNOW;

	GetDlgItem(IDC_BTN_REMOVE)->ShowWindow(m_isCamera ? SW_HIDE : SW_SHOW );

	UpdateData(FALSE);
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


void SVRegressionFileSelectDlg::OnRadioRegUpdate() 
{
	UpdateData(TRUE);
}

void SVRegressionFileSelectDlg::OnRemovePage()
{
	m_pParent->removeFilePage(this);
}

void SVRegressionFileSelectDlg::SetRegressionData(RegressionTestStruct *pDataStruct)
{
	//set data from the struct...
	m_iSelectFileRadio = pDataStruct->iFileMethod;
	m_RegTestFiles = pDataStruct->FirstFile.c_str();

	UpdateData(FALSE);
}

void SVRegressionFileSelectDlg::ShowSelectFileDlg(bool bFullAccess)
{
	static TCHAR Filter[] = _T("BMP Files (*.bmp)|*.bmp|Image Files (*.bmp)|*.bmp||");
	SvMc::SVFileDialog dlg(true, bFullAccess, nullptr, nullptr, 0, Filter, nullptr);
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
			if (0 != m_RegTestFiles.Right(4).CompareNoCase(_T(".bmp")))
			{
				SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
				Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RegressionTest_NoBmpFileSelected, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10187);
				m_RegTestFiles = _T("");
			}
		}
	}
}

void SVRegressionFileSelectDlg::ShowSelectDirectoryDlg(bool bFullAccess)
{
//	CFolderDialog dlg(_T("Select Directories"), m_RegistryPath.c_str(), this);
	SvMc::SVDlgFolder dlg(bFullAccess, m_RegistryPath.c_str());
	dlg.InitDlgFolder(_T("OK"), _T("Select Folder"));

	INT_PTR rc = dlg.DoModal();
	if (IDOK == rc)
	{
		m_RegTestFiles = dlg.GetPathName();
	}
}

