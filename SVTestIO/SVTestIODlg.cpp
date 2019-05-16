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
//Moved to precompiled header: #include <functional>
//Moved to precompiled header: #include <algorithm>
#include "SVTestIODlg.h"
#pragma endregion Includes


IMPLEMENT_DYNAMIC(SVTestIODlg, CDialog)

SVTestIODlg::SVTestIODlg(CWnd* pParent /*=nullptr*/)
	: CDialog(SVTestIODlg::IDD, pParent)
	, m_lInputs(0)
	, m_bResetStart(true)
{
	::InitializeCriticalSectionAndSpinCount( &m_CriticalSection, 10000);
	m_pCallBacks.resize(4);
}

SVTestIODlg::~SVTestIODlg()
{
	::DeleteCriticalSection( &m_CriticalSection );
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

	ON_BN_CLICKED(IDC_LOG_BUTTON, &SVTestIODlg::OnBnClickedLogButton)
	ON_BN_CLICKED(IDC_CLEAR_BTN, &SVTestIODlg::OnBnClickedClearButton)

	// IDC_TRIGGER_1 to IDC_TRIGGER_4 must be contiguous and sequential for this to work:
	ON_CONTROL_RANGE(BN_CLICKED, IDC_TRIGGER_1, IDC_TRIGGER_4, &SVTestIODlg::OnTriggerButtonClicked)

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

	long mask = 1 << (inputchannel-1);
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

void SVTestIODlg::SetOutput( unsigned long zeroBasedOutputchannel, bool p_bState)
{

	HICON l_hState = nullptr;

	::EnterCriticalSection( &m_CriticalSection );
	if( p_bState )
	{
		l_hState = AfxGetApp()->LoadIconA( IDI_ICON1 );
	}
	else
	{
		l_hState = AfxGetApp()->LoadIconA( IDI_ICON4 );
	}

	unsigned long oneBasedOutputchannel = zeroBasedOutputchannel +1;

	if (oneBasedOutputchannel < c_upperBoundForOutputChannel)
	{
		m_Output[oneBasedOutputchannel].SetIcon( l_hState );
	}

	if( m_LogBtn.GetCheck() != 0 )
	{
		CString l_strTmp;
		unsigned __int64 l_i64Check;
		unsigned __int64 l_iTime;

		if( m_bResetStart )
		{
			::QueryPerformanceCounter( (LARGE_INTEGER*)&m_i64Start);
			::QueryPerformanceFrequency((LARGE_INTEGER*)&m_i64Frequency);
			m_i64Frequency/=1000;
			m_bResetStart = false;
		}

		::QueryPerformanceCounter( (LARGE_INTEGER*)&l_i64Check);
		l_iTime = (l_i64Check - m_i64Start)/m_i64Frequency;
		l_strTmp.Format( "%010I64u Ch %02d %s", l_iTime, oneBasedOutputchannel, p_bState? "Off" : "On");
		m_LogList.AddString(l_strTmp);
	}
	::LeaveCriticalSection(&m_CriticalSection);
}

BOOL SVTestIODlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void SVTestIODlg::OnBnClickedLogButton()
{
	m_bResetStart = true;
	m_LogList.AddString("*** Reset TS ****");
}


void SVTestIODlg::OnBnClickedClearButton()
{
	m_LogList.ResetContent();

}

void SVTestIODlg::OnTriggerButtonClicked( UINT nID )
{
	int triggerchannel = nID - IDC_TRIGGER_1 + 1; //triggerchannel is one-based

	m_TriggerDispatchers.DispatchIfPossible(triggerchannel);

}


void SVTestIODlg::OnCancel()
{
	return;

	CDialog::OnCancel();
}
