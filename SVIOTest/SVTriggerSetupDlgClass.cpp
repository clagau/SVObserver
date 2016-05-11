// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTriggerSetupDlgClass.cpp
// * .File Name       : $Workfile:   SVTriggerSetupDlgClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:50:48  $
// ******************************************************************************

#include "stdafx.h"
#include "sviotest.h"
#include "SVTriggerSetupDlgClass.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVIOLibrary/SVIOParameterEnum.h"
#include "SVIOTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVTriggerSetupDlgClass dialog

SVTriggerSetupDlgClass::SVTriggerSetupDlgClass(CWnd* pParent /*=nullptr*/)
	: CDialog(SVTriggerSetupDlgClass::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVTriggerSetupDlgClass)
	m_bTrig1Rising = FALSE;
	m_bTrig2Rising = FALSE;
	m_bTrig3Rising = FALSE;
	m_bTrig4Rising = FALSE;
	//}}AFX_DATA_INIT
}

SVTriggerSetupDlgClass::~SVTriggerSetupDlgClass()
{
}

void SVTriggerSetupDlgClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVTriggerSetupDlgClass)
	DDX_Check(pDX, IDC_TRIG1_RISING_CHECK, m_bTrig1Rising);
	DDX_Check(pDX, IDC_TRIG2_RISING_CHECK, m_bTrig2Rising);
	DDX_Check(pDX, IDC_TRIG3_RISING_CHECK, m_bTrig3Rising);
	DDX_Check(pDX, IDC_TRIG4_RISING_CHECK, m_bTrig4Rising);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVTriggerSetupDlgClass, CDialog)
	//{{AFX_MSG_MAP(SVTriggerSetupDlgClass)
	ON_BN_CLICKED(IDC_STROBE1_INV_BTN, OnStrobe1InvBtn)
	ON_BN_CLICKED(IDC_STROBE2_INV_BTN, OnStrobe2InvBtn)
	ON_BN_CLICKED(IDC_STROBE3_INV_BTN, OnStrobe3InvBtn)
	ON_BN_CLICKED(IDC_STROBE4_INV_BTN, OnStrobe4InvBtn)
	ON_BN_CLICKED(IDC_TRIG1_INV_BTN, OnTrig1InvBtn)
	ON_BN_CLICKED(IDC_TRIG2_INV_BTN, OnTrig2InvBtn)
	ON_BN_CLICKED(IDC_TRIG3_INV_BTN, OnTrig3InvBtn)
	ON_BN_CLICKED(IDC_TRIG4_INV_BTN, OnTrig4InvBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVTriggerSetupDlgClass message handlers

void SVTriggerSetupDlgClass::OnStrobe1InvBtn() 
{
	CButton* l_btn = (CButton*)GetDlgItem(IDC_STROBE1_INV_BTN);
	if( l_btn->GetCheck() )
	{
		SVIOConfigurationInterfaceClass::Instance().SetCameraStrobeValue(0, 0);
		m_lStrobeInverts |= 1;
	}
	else
	{
		SVIOConfigurationInterfaceClass::Instance().SetCameraStrobeValue(0, 1);
		m_lStrobeInverts &= ~1L;
	}
}

void SVTriggerSetupDlgClass::OnStrobe2InvBtn() 
{
	CButton* l_btn = (CButton*)GetDlgItem(IDC_STROBE2_INV_BTN);
	if( l_btn->GetCheck() )
	{
		SVIOConfigurationInterfaceClass::Instance().SetCameraStrobeValue(1, 0);
		m_lStrobeInverts |= 2;
	}
	else
	{
		SVIOConfigurationInterfaceClass::Instance().SetCameraStrobeValue(1, 1);
		m_lStrobeInverts &= ~2L;
	}
}

void SVTriggerSetupDlgClass::OnStrobe3InvBtn() 
{
	CButton* l_btn = (CButton*)GetDlgItem(IDC_STROBE3_INV_BTN);
	if( l_btn->GetCheck() )
	{
		SVIOConfigurationInterfaceClass::Instance().SetCameraStrobeValue(2, 0);
		m_lStrobeInverts |= 4;
	}
	else
	{
		SVIOConfigurationInterfaceClass::Instance().SetCameraStrobeValue(2, 1);
		m_lStrobeInverts &= ~4L;
	}
}

void SVTriggerSetupDlgClass::OnStrobe4InvBtn() 
{
	CButton* l_btn = (CButton*)GetDlgItem(IDC_STROBE4_INV_BTN);
	if( l_btn->GetCheck() )
	{
		SVIOConfigurationInterfaceClass::Instance().SetCameraStrobeValue(3, 0);
		m_lStrobeInverts |= 8;
	}
	else
	{
		SVIOConfigurationInterfaceClass::Instance().SetCameraStrobeValue(3, 1);
		m_lStrobeInverts &= ~8L;
	}
}


void SVTriggerSetupDlgClass::OnTrig1InvBtn() 
{
	CButton* l_btn = (CButton*)GetDlgItem(IDC_TRIG1_INV_BTN);
	if( l_btn->GetCheck() )
	{
		SVIOConfigurationInterfaceClass::Instance().SetCameraTriggerValue(0, 0);
		m_lTrigInverts |= 1;
	}
	else
	{
		SVIOConfigurationInterfaceClass::Instance().SetCameraTriggerValue(0, 1);
		m_lTrigInverts &= ~1L;
	}
}

void SVTriggerSetupDlgClass::OnTrig2InvBtn() 
{
	CButton* l_btn = (CButton*)GetDlgItem(IDC_TRIG2_INV_BTN);
	if( l_btn->GetCheck() )
	{
		SVIOConfigurationInterfaceClass::Instance().SetCameraTriggerValue(1, 0);
		m_lTrigInverts |= 2;
	}
	else
	{
		SVIOConfigurationInterfaceClass::Instance().SetCameraTriggerValue(1, 1);
		m_lTrigInverts &= ~2L;
	}
}

void SVTriggerSetupDlgClass::OnTrig3InvBtn() 
{
	CButton* l_btn = (CButton*)GetDlgItem(IDC_TRIG3_INV_BTN);
	if( l_btn->GetCheck() )
	{
		SVIOConfigurationInterfaceClass::Instance().SetCameraTriggerValue(2, 0);
		m_lTrigInverts |= 4;
	}
	else
	{
		SVIOConfigurationInterfaceClass::Instance().SetCameraTriggerValue(2, 1);
		m_lTrigInverts &= ~4L;
	}
}

void SVTriggerSetupDlgClass::OnTrig4InvBtn() 
{
	CButton* l_btn = (CButton*)GetDlgItem(IDC_TRIG4_INV_BTN);
	if( l_btn->GetCheck() )
	{
		SVIOConfigurationInterfaceClass::Instance().SetCameraTriggerValue(3, 0);
		m_lTrigInverts |= 8;
	}
	else
	{
		SVIOConfigurationInterfaceClass::Instance().SetCameraTriggerValue(3, 1);
		m_lTrigInverts &= ~8L;
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
	BOOL l_bEnable = TRUE;
	
	CButton* l_btn = (CButton*)GetDlgItem(IDC_STROBE1_INV_BTN);
	l_btn->SetCheck( m_lStrobeInverts & 1 ? TRUE : FALSE );

	switch( m_lSystemType )
	{
		case SVRABBIT_X1:
		case SVRABBIT_X3:
		{
			l_bEnable = FALSE;
			break;
		}
		default:
		{
			break;
		}
	}

	l_btn = (CButton*)GetDlgItem(IDC_STROBE2_INV_BTN);
	l_btn->SetCheck( m_lStrobeInverts & 2 ? TRUE : FALSE );
	l_btn->EnableWindow( l_bEnable );
	GetDlgItem(IDC_TRIG2_RISING_CHECK)->EnableWindow( l_bEnable );
	
	l_btn = (CButton*)GetDlgItem(IDC_STROBE3_INV_BTN);
	l_btn->SetCheck( m_lStrobeInverts & 4 ? TRUE : FALSE );
	l_btn->EnableWindow( l_bEnable );
	GetDlgItem(IDC_TRIG3_RISING_CHECK)->EnableWindow( l_bEnable );

	l_btn = (CButton*)GetDlgItem(IDC_STROBE4_INV_BTN);
	l_btn->SetCheck( m_lStrobeInverts & 8 ? TRUE : FALSE );
	l_btn->EnableWindow( l_bEnable );
	GetDlgItem(IDC_TRIG4_RISING_CHECK)->EnableWindow( l_bEnable );

	l_btn = (CButton*)GetDlgItem(IDC_TRIG1_INV_BTN);
	l_btn->SetCheck( m_lTrigInverts & 1 ? TRUE : FALSE );

	l_btn = (CButton*)GetDlgItem(IDC_TRIG2_INV_BTN);
	l_btn->SetCheck( m_lTrigInverts & 2 ? TRUE : FALSE );
	l_btn->EnableWindow( l_bEnable );

	l_btn = (CButton*)GetDlgItem(IDC_TRIG3_INV_BTN);
	l_btn->SetCheck( m_lTrigInverts & 4 ? TRUE : FALSE );
	l_btn->EnableWindow( l_bEnable );

	l_btn = (CButton*)GetDlgItem(IDC_TRIG4_INV_BTN);
	l_btn->SetCheck( m_lTrigInverts & 8 ? TRUE : FALSE );
	l_btn->EnableWindow( l_bEnable );


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

