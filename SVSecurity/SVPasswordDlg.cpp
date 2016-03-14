//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPasswordDlg.cpp
//* .File Name       : $Workfile:   SVPasswordDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   30 May 2014 10:47:12  $
//******************************************************************************

#include "stdafx.h"
#include "resource.h"
#include "SVPasswordDlg.h"
#include "SVSecurity.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSVSecurityApp theApp;

SVPasswordDlg::SVPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SVPasswordDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVPasswordDlg)
	m_strPassword = _T("");
	m_strUser = _T("");
	m_strStatus = _T("");
	//}}AFX_DATA_INIT

}

SVPasswordDlg::~SVPasswordDlg()
{
}

void SVPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVPasswordDlg)
	DDX_Text(pDX, IDC_PASSWORD_EDIT, m_strPassword);
	DDX_Text(pDX, IDC_USER_NAME_EDIT, m_strUser);
	DDX_Text(pDX, IDC_STATUS, m_strStatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVPasswordDlg, CDialog)
	//{{AFX_MSG_MAP(SVPasswordDlg)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL SVPasswordDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	UpdateData(FALSE);
	GetDlgItem( IDC_USER_NAME_EDIT )->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVPasswordDlg::OnHelp() 
{
	theApp.HtmlHelp(HH_HELP_CONTEXT,IDD_PASSWORD_DLG + 0x60000);
}

BOOL SVPasswordDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// If the control ID comming thru is from static text or something invalid then
	// redirect it to the password dialog Id.
	if( ( pHelpInfo->iCtrlId & 0xffff ) == 0xffff || 
		pHelpInfo->iCtrlId < 500)
	{
		pHelpInfo->iCtrlId = IDD_PASSWORD_DLG + 0x60000;
	}
	else
	{
		pHelpInfo->iCtrlId += 0x70000;
	}	
	theApp.HtmlHelp(pHelpInfo->iCtrlId,HH_HELP_CONTEXT);
	return TRUE ; 
}

