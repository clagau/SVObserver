//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSoftwareTriggerSetupDlg
//* .File Name       : $Workfile:   SVSoftwareTriggerSetupDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 12:50:40  $
//******************************************************************************

#include "stdafx.h"
#include "SVIOTEST.h"
#include "SVSoftwareTriggerSetupDlg.h"

// SVSoftwareTriggerSetupDlg dialog

IMPLEMENT_DYNAMIC(SVSoftwareTriggerSetupDlg, CDialog)

SVSoftwareTriggerSetupDlg::SVSoftwareTriggerSetupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SVSoftwareTriggerSetupDlg::IDD, pParent)
	, m_period1(0)
	, m_period2(0)
	, m_period3(0)
	, m_period4(0)
{
}

SVSoftwareTriggerSetupDlg::~SVSoftwareTriggerSetupDlg()
{
}

void SVSoftwareTriggerSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PERIOD1, m_period1);
	DDX_Text(pDX, IDC_PERIOD2, m_period2);
	DDX_Text(pDX, IDC_PERIOD3, m_period3);
	DDX_Text(pDX, IDC_PERIOD4, m_period4);
}

BOOL SVSoftwareTriggerSetupDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
		
	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVSoftwareTriggerSetupDlg::OnOK() 
{
	UpdateData(true);
	
	CDialog::OnOK();
}

BEGIN_MESSAGE_MAP(SVSoftwareTriggerSetupDlg, CDialog)
END_MESSAGE_MAP()


// SVSoftwareTriggerSetupDlg message handlers

