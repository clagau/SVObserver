//******************************************************************************
// * COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTestIODlg
//* .File Name       : $Workfile:   SVTestIODlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   16 Jul 2013 13:22:16  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTestIODlg.h"
#include "Triggering/SVTriggerInfoStruct.h"
#pragma endregion Includes

#define WM_SETOUTPUT (WM_USER + 101)

SVTestIODlg::SVTestIODlg(CWnd* pParent /*=nullptr*/) : CDialog(SVTestIODlg::IDD, pParent)
{
	
}

SVTestIODlg::~SVTestIODlg()
{
	
}

void SVTestIODlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	// IDC_INPUT1 to IDC_INPUT8 must be contiguous and sequential for this to work:
	for (int inputchannel = 1; inputchannel < c_upperBoundForInputChannel; inputchannel++)
	{
		DDX_Control(pDX, IDC_INPUT1+inputchannel-1, m_input[inputchannel]);
	}

	// IDC_OUTPUT1 to IDC_OUTPUT16 must be contiguous and sequential for this to work:
	for (int outputchannel = 1; outputchannel < c_upperBoundForOutputChannel; outputchannel++)
	{
		DDX_Control(pDX, IDC_OUTPUT1+outputchannel-1, m_Output[outputchannel]);
	}

	DDX_Control(pDX, IDC_LOG_LIST, m_LogList);
	DDX_Control(pDX, IDC_LOG_CHECK, m_LogBtn);
}

BEGIN_MESSAGE_MAP(SVTestIODlg, CDialog)

	// IDC_INPUT1 to IDC_INPUT8 must be contiguous and sequential for this to work:
	ON_CONTROL_RANGE(BN_CLICKED, IDC_INPUT1, IDC_INPUT8, &SVTestIODlg::OnInputButtonClicked)

	ON_BN_CLICKED(IDC_LOG_CHECK, &SVTestIODlg::OnBnClickedLogging)
	ON_BN_CLICKED(IDC_LOG_BUTTON, &SVTestIODlg::OnBnClickedResetTs)
	ON_BN_CLICKED(IDC_CLEAR_BTN, &SVTestIODlg::OnBnClickedClearButton)

	// IDC_TRIGGER_1 to IDC_TRIGGER_4 must be contiguous and sequential for this to work:
	ON_CONTROL_RANGE(BN_CLICKED, IDC_TRIGGER_1, IDC_TRIGGER_4, &SVTestIODlg::OnTriggerButtonClicked)

	ON_MESSAGE(WM_SETOUTPUT, OnSetOutput)

END_MESSAGE_MAP()


// SVTestIODlg message handlers

void SVTestIODlg::OnInputButtonClicked( UINT nID )
{
	int inputchannel = nID - IDC_INPUT1 + 1; //inputchannel is one-based

	ToggleInput( inputchannel );

}


void SVTestIODlg::ToggleInput(unsigned int inputchannel)
{
	if( inputchannel < 1 ||  inputchannel >= c_upperBoundForInputChannel)
	{
		return; //invalid inputchannel: do nothing!
	}

	unsigned long mask = 1 << (inputchannel-1);
	if( m_lInputs & mask )
	{
		m_lInputs &= ~mask ;
		m_input[inputchannel].SetIcon( AfxGetApp()->LoadIconA(IDI_ICON1));
	}
	else
	{
		m_lInputs |= mask ;
		m_input[inputchannel].SetIcon( AfxGetApp()->LoadIconA(IDI_ICON4));
	}
}

void SVTestIODlg::SetOutput( unsigned long zeroBasedOutputchannel, bool bState)
{
	PostMessage(WM_SETOUTPUT, static_cast<WPARAM>( zeroBasedOutputchannel), static_cast<LPARAM>(bState));
}

BOOL SVTestIODlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_isLogButtonChecked = (BST_CHECKED == m_LogBtn.GetCheck());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void SVTestIODlg::OnBnClickedResetTs()
{
	m_bResetStart = true;
	m_LogList.AddString("*** Reset TS ****");
}
void SVTestIODlg::OnBnClickedLogging()
{
	m_isLogButtonChecked = (BST_CHECKED == m_LogBtn.GetCheck());
}


void SVTestIODlg::OnBnClickedClearButton()
{
	m_LogList.ResetContent();

}

void SVTestIODlg::OnTriggerButtonClicked( UINT nID )
{
	unsigned long triggerIndex = nID - IDC_TRIGGER_1 + 1;

	auto iter = m_triggerCallbackMap.find(triggerIndex);
	if (m_triggerCallbackMap.end() != iter)
	{
		SvTrig::TriggerData triggerData;
		iter->second(std::move(triggerData));
	}

}


void SVTestIODlg::OnCancel()
{
	return;
}




LRESULT SVTestIODlg::OnSetOutput(WPARAM wParam, LPARAM lParam)
{

	HICON l_hState = nullptr;

	unsigned long zeroBasedOutputchannel = static_cast<unsigned long>(wParam);
	bool bState = lParam != 0;
	if (bState)
	{
		l_hState = AfxGetApp()->LoadIconA(IDI_ICON1);
	}
	else
	{
		l_hState = AfxGetApp()->LoadIconA(IDI_ICON4);
	}

	unsigned long oneBasedOutputchannel = zeroBasedOutputchannel + 1;

	if (oneBasedOutputchannel < c_upperBoundForOutputChannel)
	{
		m_Output[oneBasedOutputchannel].SetIcon(l_hState);
	}

	
	assert(m_isLogButtonChecked  == (m_LogBtn.GetCheck()== BST_CHECKED));
	if(m_isLogButtonChecked) 
	{
		CString l_strTmp;
		unsigned __int64 l_i64Check;
		unsigned __int64 l_iTime;

		if (m_bResetStart)
		{
			::QueryPerformanceCounter((LARGE_INTEGER*)&m_i64Start);
			::QueryPerformanceFrequency((LARGE_INTEGER*)&m_i64Frequency);
			m_i64Frequency /= 1000;
			m_bResetStart = false;
		}

		::QueryPerformanceCounter((LARGE_INTEGER*)&l_i64Check);
		l_iTime = (l_i64Check - m_i64Start) / m_i64Frequency;
		l_strTmp.Format("%010I64u Ch %02d %s", l_iTime, oneBasedOutputchannel, bState ? "Off" : "On");
		m_LogList.AddString(l_strTmp);
	}
	return 0;
}
