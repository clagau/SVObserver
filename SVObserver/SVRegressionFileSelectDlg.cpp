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
#include "SVStatusLibrary/MessageManagerResource.h"
#include "ObjectInterfaces/ErrorNumbers.h"
#include "TextDefinesSvO.h"
#pragma endregion Includes

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
	m_sRegTestFiles = _T("");
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
	DDX_Text(pDX, IDC_EDIT_REG_SELECTED_FILES, m_sRegTestFiles);
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
	m_sDialogName = lpszTitle;
}

void CSVRegressionFileSelectDlg::OnBtnRegTestBrowseFiles() 
{
	//get last regression path from registry...
	CString sFilePath;

	m_sRegistryPath = AfxGetApp()->GetProfileString(_T("RegressionTest"), _T("LastPath"), _T("C:\\Temp"));

	CString	csFileExtensionFilterList = _T("BMP's (*.bmp)|*.bmp||");
	static TCHAR szFilter[] = _T("BMP Files (*.bmp)|*.bmp|Image Files (*.bmp)|*.bmp||");
	bool bFullAccess = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
	SvMc::SVFileDialog dlg(true, bFullAccess, nullptr, nullptr, 0, szFilter, nullptr);
	dlg.m_ofn.lpstrTitle = _T("Select File");

	if ( m_sRegTestFiles.IsEmpty() )
	{
		//nothing has been set... use what is in the registry
		dlg.m_ofn.lpstrInitialDir = m_sRegistryPath.operator LPCTSTR();
	}
	else
	{
		dlg.m_ofn.lpstrInitialDir = m_sRegTestFiles.operator LPCTSTR();
	}

	if ( dlg.DoModal() == IDOK)
	{
		m_sRegTestFiles = dlg.GetPathName(); 
		if ( !m_sRegTestFiles.IsEmpty() )
		{
			if (0 != m_sRegTestFiles.Right(4).CompareNoCase(_T(".bmp")))
			{
				SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvO::RegressionTest_NoBmpFileSelected, StdMessageParams, SvOi::Err_10187 ); 
				m_sRegTestFiles = _T("");
			}

			int iPos = m_sRegTestFiles.ReverseFind(_T('\\'));
			if(iPos != -1)
			{
				//only write out registry entry if the path is not empty.
				CString sTmpDirName = m_sRegTestFiles.Left(iPos);
				AfxGetApp()->WriteProfileString(_T("RegressionTest"), _T("LastPath"), sTmpDirName);
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
	return m_sRegTestFiles;
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


void CSVRegressionFileSelectDlg::SetRegressionData(RegressionTestStruct *p_pDataStruct)
{
	//set data from the struct...
	m_iSelectFileRadio = p_pDataStruct->iFileMethod;
	m_sRegTestFiles = p_pDataStruct->csFirstFile;

	UpdateData(FALSE);
}

