// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVRegressionFileSelectDlg
// * .File Name       : $Workfile:   SVRegressionFileSelectDlg.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 14:41:20  $
// ******************************************************************************

#include "stdafx.h"
#include "svobserver.h"
#include "SVRegressionFileSelectDlg.h"

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

	m_sRegistryPath = AfxGetApp()->GetProfileString(_T("RegressionTest"), _T("LastPath"),_T("C:\\Temp"));

	static char BASED_CODE szFilter[] = "BMP Files (*.bmp)|*.bmp|Image Files (*.bmp)|*.bmp|All Files (*.*)|*.*||";

	CString	csFileExtensionFilterList = "BMP's (*.bmp)|*.bmp||";
	// TODO: Add your control notification handler code here
	//CFileDialog dlg(TRUE,"*.bmp",szFilter);
	CFileDialog dlg(TRUE,NULL,NULL,0,szFilter,NULL);


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
		int iPos = m_sRegTestFiles.ReverseFind('\\');

		CString sTmpDirName = m_sRegTestFiles.Left(iPos);
		AfxGetApp()->WriteProfileString(_T("RegressionTest"),_T("LastPath"),sTmpDirName);
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

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVRegressionFileSelectDlg.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:41:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Apr 2009 14:58:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  657
 * SCR Title:  Remove Apply button from the Tool Adjust Dialogs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed the apply button
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Sep 2005 14:38:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed the flag on the open file dialog for Multi Select.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Jul 2005 13:59:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added code to save the current settings and will also save the regression file select path in the registry
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Jul 2005 11:34:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/