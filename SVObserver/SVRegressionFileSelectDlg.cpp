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
#include "svobserver.h"
#include "SVRegressionFileSelectDlg.h"
#include "SVMFCControls/SVFileDialog.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVMessage/ErrorNumbers.h"
#include "TextDefinesSvO.h"
#include "SVStatusLibrary/GlobalPath.h"
#pragma endregion Includes

#pragma region Declarations
static const int PathBufferLen = MAX_PATH +2;
static const int MaxNumberCameraEntries = 4;
#pragma endregion Declarations 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSVRegressionFileSelectDlg property page

IMPLEMENT_DYNCREATE(CSVRegressionFileSelectDlg, CPropertyPage)

CSVRegressionFileSelectDlg::CSVRegressionFileSelectDlg(LPCTSTR lptstrDialogName) : CPropertyPage(CSVRegressionFileSelectDlg::IDD)
{
	
	//{{AFX_DATA_INIT(CSVRegressionFileSelectDlg)
	m_iSelectFileRadio = 2;
	m_RegTestFiles = _T("");
	//}}AFX_DATA_INIT
	m_strCaption = lptstrDialogName;
	m_psp.pszTitle = m_strCaption;
	m_psp.dwFlags |= PSP_USETITLE;
}

CSVRegressionFileSelectDlg::~CSVRegressionFileSelectDlg()
{
}

void CSVRegressionFileSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSVRegressionFileSelectDlg)
	DDX_Control(pDX, IDC_BTN_REG_TEST_BROWSE_FILES, m_btnBrowseFiles);
	DDX_Radio(pDX, IDC_RADIO_REG_LIST, m_iSelectFileRadio);
	DDX_Text(pDX, IDC_EDIT_REG_SELECTED_FILES, m_RegTestFiles);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSVRegressionFileSelectDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CSVRegressionFileSelectDlg)
	ON_BN_CLICKED(IDC_BTN_REG_TEST_BROWSE_FILES, OnBtnRegTestBrowseFiles)
	ON_BN_CLICKED(IDC_RADIO_REG_LIST, OnRadioRegList)
	ON_BN_CLICKED(IDC_RADIO_REG_NONE, OnRadioRegNone)
	ON_BN_CLICKED(IDC_RADIO_REG_SINGLE, OnRadioRegSingle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSVRegressionFileSelectDlg message handlers

void CSVRegressionFileSelectDlg::SetDlgTitle( LPCTSTR lpszTitle )
{
	m_DialogName = lpszTitle;
}

void CSVRegressionFileSelectDlg::OnBtnRegTestBrowseFiles() 
{
	
	int CameraNumber(0); 
	int  Pos = m_strCaption.ReverseFind(_T('_'));
	if(Pos != -1)
	{		
		CString sTemp = m_strCaption.Mid(Pos +1);
		CameraNumber = _ttoi(sTemp.GetString());
		CameraNumber = CameraNumber %  MaxNumberCameraEntries; 
	}
	//get last regression path for this camera from registry...
	SVString KeyName = SvUl_SF::Format( _T("LastPath_%i"), CameraNumber);
	m_RegistryPath = AfxGetApp()->GetProfileString(_T("RegressionTest"), KeyName.c_str(), SvStl::GlobalPath::Inst().GetTempPath().c_str());

	static TCHAR Filter[] = _T("BMP Files (*.bmp)|*.bmp|Image Files (*.bmp)|*.bmp||");
	bool bFullAccess = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
	SvMc::SVFileDialog dlg(true, bFullAccess, nullptr, nullptr, 0, Filter, nullptr);
	dlg.m_ofn.lpstrTitle = _T("Select File");

	TCHAR FileName[PathBufferLen];
	_tcscpy_s(FileName,PathBufferLen,m_RegTestFiles.GetString()); 
	if ( m_RegTestFiles.IsEmpty() )
	{
		//nothing has been set... use what is in the registry
		dlg.m_ofn.lpstrInitialDir = m_RegistryPath.c_str();
	}
	else
	{
		///seting lpstrFile instead of lpstrInitialDir avoids strange Windows7 behaviour of CFileDialog
		dlg.m_ofn.lpstrFile = 	FileName;
		dlg.m_ofn.nMaxFile = PathBufferLen;
		dlg.m_ofn.lpstrInitialDir =nullptr;
	}

	if ( dlg.DoModal() == IDOK)
	{
		m_RegTestFiles = dlg.GetPathName(); 
		if ( !m_RegTestFiles.IsEmpty() )
		{
			if (0 != m_RegTestFiles.Right(4).CompareNoCase(_T(".bmp")))
			{
				SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_RegressionTest_NoBmpFileSelected, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10187 ); 
				m_RegTestFiles = _T("");
			}

			int iPos = m_RegTestFiles.ReverseFind(_T('\\'));
			if(iPos != -1)
			{
				//only write out registry entry if the path is not empty.
				CString sTmpDirName = m_RegTestFiles.Left(iPos);
				AfxGetApp()->WriteProfileString(_T("RegressionTest"),  KeyName.c_str(), sTmpDirName);
			}
		}
	}
	UpdateData(FALSE);
}

BOOL CSVRegressionFileSelectDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_pParent = dynamic_cast<CSVRegressionFileSelectSheet*>(GetParent());
	m_pParent->m_psh.dwFlags |= PSH_NOAPPLYNOW;

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

RegressionFileEnum CSVRegressionFileSelectDlg::GetFileSelectType()
{
	UpdateData(TRUE);
	return (RegressionFileEnum)m_iSelectFileRadio;
}

CString CSVRegressionFileSelectDlg::GetSelectedFile()
{
	return m_RegTestFiles;
}

CString CSVRegressionFileSelectDlg::GetPageName()
{
	return m_strCaption;
}


void CSVRegressionFileSelectDlg::OnRadioRegList() 
{
	UpdateData(TRUE);
}


void CSVRegressionFileSelectDlg::OnRadioRegNone() 
{
	UpdateData(TRUE);

}

void CSVRegressionFileSelectDlg::OnRadioRegSingle() 
{
	UpdateData(TRUE);
}


void CSVRegressionFileSelectDlg::SetRegressionData(RegressionTestStruct *pDataStruct)
{
	//set data from the struct...
	m_iSelectFileRadio = pDataStruct->iFileMethod;
	m_RegTestFiles = pDataStruct->FirstFile.c_str();

	UpdateData(FALSE);
}

