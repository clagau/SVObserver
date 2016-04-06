// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVRegressionExitDlg
// * .File Name       : $Workfile:   SVRegressionExitDlg.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   12 Dec 2014 09:52:14  $
// ******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "svobserver.h"
#include "SVRegressionExitDlg.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVRegressionExitDlg dialog
extern HRESULT GlobalRCSetMode(unsigned long lSVIMNewMode);


SVRegressionExitDlg::SVRegressionExitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SVRegressionExitDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVRegressionExitDlg)
	m_csExitModes = _T("");
	//}}AFX_DATA_INIT
}

SVRegressionExitDlg::~SVRegressionExitDlg()
{
}

void SVRegressionExitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVRegressionExitDlg)
	DDX_Control(pDX, IDC_COMBO_EXIT_MODE, m_ctlExitModes);
	DDX_CBString(pDX, IDC_COMBO_EXIT_MODE, m_csExitModes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVRegressionExitDlg, CDialog)
	//{{AFX_MSG_MAP(SVRegressionExitDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVRegressionExitDlg message handlers

BOOL SVRegressionExitDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ctlExitModes.AddString("Run");
	m_ctlExitModes.AddString("Stop");
	m_ctlExitModes.AddString("Test");
	m_ctlExitModes.AddString("Edit");

	m_ctlExitModes.SelectString(0,"Edit");
	
	UpdateData(TRUE);
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVRegressionExitDlg::OnOK() 
{
	UpdateData(TRUE);
	
	//check to see what m_csExitModes is set to

	if ( m_csExitModes.CompareNoCase("Run") == 0 )
	{
		::PostMessage (AfxGetMainWnd()->m_hWnd, WM_COMMAND, ID_GO_ONLINE, 0L);
	}

	else if ( m_csExitModes.CompareNoCase("Stop") == 0 )
	{
		::PostMessage (AfxGetMainWnd()->m_hWnd, WM_COMMAND, ID_GO_OFFLINE, 0L);
	}

	else if ( m_csExitModes.CompareNoCase("Test") == 0 )
	{
		::PostMessage (AfxGetMainWnd()->m_hWnd, WM_COMMAND, ID_MODE_TEST, 0L);
	}

	else if ( m_csExitModes.CompareNoCase("Edit") == 0 )
	{
		::PostMessage (AfxGetMainWnd()->m_hWnd, WM_COMMAND, ID_MODE_EDIT, 0L);
	}

	CDialog::OnOK();
}

