// ******************************************************************************
// * COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTriggerSetupDlgClass.cpp
// * .File Name       : $Workfile:   SVTriggerSetupDlgClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:50:48  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTriggerSetupDlgClass.h"
#include "SVIOTestDlg.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "Triggering/SVIOTriggerLoadLibraryClass.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void SVTriggerSetupDlgClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	for (int i = 0; i < cTriggerChannelNr; ++i)
	{
		DDX_Check(pDX, IDC_TRIG1_RISING_CHECK + i, m_bTrigRising[i]);
	}
}


BEGIN_MESSAGE_MAP(SVTriggerSetupDlgClass, CDialog)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_STROBE1_INV_BTN, IDC_STROBE4_INV_BTN, OnStrobeInvBtn)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_TRIG1_INV_BTN, IDC_TRIG4_INV_BTN, OnTrigInvBtn)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVTriggerSetupDlgClass message handlers

void SVTriggerSetupDlgClass::OnStrobeInvBtn(UINT nID)
{
	//Note GetDlgItem uses a temporary CWnd* and dynamic_cast cannot be used
	CButton* pButton = static_cast<CButton*> (GetDlgItem(nID));
	UINT index {nID - IDC_STROBE1_INV_BTN};
	long mask {1 << index};
	if(nullptr != pButton && pButton->GetCheck())
	{
		SVIOConfigurationInterfaceClass::Instance().SetCameraStrobeValue(index, 0);
		m_lStrobeInverts |= mask;
	}
	else
	{
		SVIOConfigurationInterfaceClass::Instance().SetCameraStrobeValue(index, 1);
		m_lStrobeInverts &= ~mask;
	}
}

void SVTriggerSetupDlgClass::OnTrigInvBtn(UINT nID)
{
	//Note GetDlgItem uses a temporary CWnd* and dynamic_cast cannot be used
	CButton* pButton = static_cast<CButton*> (GetDlgItem(nID));
	UINT index {nID - IDC_TRIG1_INV_BTN};
	long mask {1 << index};
	if (nullptr != pButton && pButton->GetCheck())
	{
		SVIOConfigurationInterfaceClass::Instance().SetCameraTriggerValue(index, 0);
		m_lTrigInverts |= mask;
	}
	else
	{
		SVIOConfigurationInterfaceClass::Instance().SetCameraTriggerValue(index, 1);
		m_lTrigInverts &= ~mask;
	}
}

void SVTriggerSetupDlgClass::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

BOOL SVTriggerSetupDlgClass::OnInitDialog() 
{
	CDialog::OnInitDialog();

	BOOL enableControl {TRUE};
	
	switch( m_lSystemType )
	{
		case SVRABBIT_X1:
		case SVRABBIT_X3:
		{
			enableControl = FALSE;
			break;
		}
		default:
		{
			break;
		}
	}

	for (int i = 0; i < cTriggerChannelNr; ++i)
	{
		//Note GetDlgItem uses a temporary CWnd* and dynamic_cast cannot be used
		CButton* pButton = static_cast<CButton*> (GetDlgItem(IDC_STROBE1_INV_BTN + i));
		long mask {1 << i};
		pButton->SetCheck((m_lStrobeInverts & mask) ? TRUE : FALSE);
		if (i > 1)
		{
			pButton->EnableWindow(enableControl);
			GetDlgItem(IDC_TRIG1_RISING_CHECK + i)->EnableWindow(enableControl);
		}

		//Note GetDlgItem uses a temporary CWnd* and dynamic_cast cannot be used
		pButton = static_cast<CButton*> (GetDlgItem(IDC_TRIG1_INV_BTN + i));
		pButton->SetCheck((m_lTrigInverts & mask) ? TRUE : FALSE);
		if (i > 1)
		{
			pButton->EnableWindow(enableControl);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

