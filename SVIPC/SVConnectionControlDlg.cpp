// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVConnectionControl
// * .File Name       : $Workfile:   SVConnectionControlDlg.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:09:38  $
// ******************************************************************************

#include "stdafx.h"
#include "SVConnectionControlDlg.h"
#include "SVConnectionControl.h"
#include "SVStatusLibrary/SVRegistry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVConnectionControlDlg::SVConnectionControlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SVConnectionControlDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVConnectionControlDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  mSelection = SVConnectionControl::NoProtocol;
}

void SVConnectionControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVConnectionControlDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVConnectionControlDlg, CDialog)
	//{{AFX_MSG_MAP(SVConnectionControlDlg)
	ON_BN_CLICKED(IDC_PROTOCOL_NAMED_PIPES, OnProtocolNamedPipes)
	ON_BN_CLICKED(IDC_PROTOCOL_TCPIP, OnProtocolTCPIP)
	ON_BN_CLICKED(IDC_PROTOCOL_TCPIP, OnProtocolTCPIP)
	ON_BN_CLICKED(IDC_PROTOCOL_NAMED_PIPES, OnProtocolNamedPipes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL SVConnectionControlDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

  CButton *pRadio;

  switch (mSelection)
  {
    case SVConnectionControl::NoProtocol : // no selection
      break;
  
    case SVConnectionControl::TCPIP: // TCP/IP
      pRadio = (CButton *) GetDlgItem (IDC_PROTOCOL_TCPIP);
      pRadio->SetCheck (1);
      break;

    case SVConnectionControl::NamedPipes : // Named Pipes
      pRadio = (CButton *) GetDlgItem (IDC_PROTOCOL_NAMED_PIPES);
      pRadio->SetCheck (1);
      break;

    default :
      break;
  }
	return TRUE;
}

void SVConnectionControlDlg::OnProtocolNamedPipes() 
{
	mSelection = SVConnectionControl::NamedPipes;
}

void SVConnectionControlDlg::OnProtocolTCPIP() 
{
	mSelection = SVConnectionControl::TCPIP;
}

void SVConnectionControlDlg::OnCancel() 
{
	mSelection = SVConnectionControl::NoProtocol;
	
	CDialog::OnCancel();
}

void SVConnectionControlDlg::OnOK() 
{
	UpdateData (TRUE);
	
	CDialog::OnOK();
}

