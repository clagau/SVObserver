//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSoftwareTriggerSetupDlg
//* .File Name       : $Workfile:   SVSoftwareTriggerSetupDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 12:50:40  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVSoftwareTriggerSetupDlg.h"
#pragma endregion Includes

BEGIN_MESSAGE_MAP(SVSoftwareTriggerSetupDlg, CDialog)
END_MESSAGE_MAP()

SVSoftwareTriggerSetupDlg::SVSoftwareTriggerSetupDlg(CWnd* pParent /*=nullptr*/) : CDialog(SVSoftwareTriggerSetupDlg::IDD, pParent)
{
}

void SVSoftwareTriggerSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	for (int i = 0; i < cTriggerChannelNr; ++i)
	{
		DDX_Text(pDX, IDC_PERIOD1 + i, m_period[i]);
	}
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
