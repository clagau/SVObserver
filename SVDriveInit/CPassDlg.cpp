//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CPassDlg
//* .File Name       : $Workfile:   CPassDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 08:33:08  $
//******************************************************************************

#include "stdafx.h"
#include "CPassDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPassDlg dialog


CPassDlg::CPassDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CPassDlg::IDD, pParent)
	, m_password{_T("")}
{
}


void CPassDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPassDlg)
	DDX_Text(pDX, IDC_EDIT1, m_password);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPassDlg, CDialog)
	//{{AFX_MSG_MAP(CPassDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPassDlg message handlers

void CPassDlg::OnOK() 
{
	UpdateData(TRUE);

	CString tmp, title;
	
	GetWindowText( title );

	if( (m_password.CompareNoCase("magic") != 0) && (m_password.CompareNoCase("merlin") != 0) )
		{
		tmp = "Invalid Password";
		MessageBox( tmp, title, MB_ICONERROR );
		}
	else
		{

	// TODO: Add extra validation here
	
	CDialog::OnOK();
		}
}

