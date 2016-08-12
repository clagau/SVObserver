// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIOTESTDlg
// * .File Name       : $Workfile:   SVIOTESTDlg.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   17 Sep 2014 15:18:02  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <string>
#include "SVIOTEST.h"
#include "SVIOTESTDlg.h"
#include "SVIOLibrary/SVIOParameterEnum.h"
#include "SVTriggerSetupDlgClass.h"
#include "SVSoftwareTriggerSetupDlg.h"
#include "SVOMFCLibrary/SVOINIClass.h"
#include "TriggerHandling/SVIOConfigurationInterfaceClass.h"
#include "SVStatusLibrary/GlobalPath.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT_PTR m_nTimer;
extern CSVIOTESTApp theApp;

static LPCTSTR SVLPTIODLL = _T("SVLptIO.dll");

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSVIOTESTDlg dialog

CSVIOTESTDlg::CSVIOTESTDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CSVIOTESTDlg::IDD, pParent)
	, m_lFanFreq1(0)
	, m_lFanFreq2(0)
	, m_lFanFreq3(0)
	, m_lFanFreq4(0)
{
	//{{AFX_DATA_INIT(CSVIOTESTDlg)
	m_lStaticChannel = 0;
	m_BoardModel = -1;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_svTrigger1Data.ulLastIndex = 0;
	m_svTrigger1Data.ulIndex = 0;
	m_svTrigger1Data.ulNextIndex = ( m_svTrigger1Data.ulIndex + 1 ) % 100;
	m_svTrigger1Data.lTriggerCount = 0;
	m_svTrigger1Data.m_LastTime = 0;
	m_svTrigger1Data.m_TotalTime = 0;
	m_svTrigger1Data.m_MaxTime = SVClock::GetMinTimeStamp();
	m_svTrigger1Data.m_MinTime = SVClock::GetMaxTimeStamp();

	m_svTrigger2Data.ulLastIndex = 0;
	m_svTrigger2Data.ulIndex = 0;
	m_svTrigger2Data.ulNextIndex = ( m_svTrigger2Data.ulIndex + 1 ) % 100;
	m_svTrigger2Data.lTriggerCount = 0;
	m_svTrigger2Data.m_LastTime = 0;
	m_svTrigger2Data.m_TotalTime = 0;
	m_svTrigger2Data.m_MaxTime = SVClock::GetMinTimeStamp();
	m_svTrigger2Data.m_MinTime = SVClock::GetMaxTimeStamp();

	m_svTrigger3Data.ulLastIndex = 0;
	m_svTrigger3Data.ulIndex = 0;
	m_svTrigger3Data.ulNextIndex = ( m_svTrigger3Data.ulIndex + 1 ) % 100;
	m_svTrigger3Data.lTriggerCount = 0;
	m_svTrigger3Data.m_LastTime = 0;
	m_svTrigger3Data.m_TotalTime = 0;
	m_svTrigger3Data.m_MaxTime = SVClock::GetMinTimeStamp();
	m_svTrigger3Data.m_MinTime = SVClock::GetMaxTimeStamp();

	m_svTrigger4Data.ulLastIndex = 0;
	m_svTrigger4Data.ulIndex = 0;
	m_svTrigger4Data.ulNextIndex = ( m_svTrigger4Data.ulIndex + 1 ) % 100;
	m_svTrigger4Data.lTriggerCount = 0;
	m_svTrigger4Data.m_LastTime = 0;
	m_svTrigger4Data.m_TotalTime = 0;
	m_svTrigger4Data.m_MaxTime = SVClock::GetMinTimeStamp();
	m_svTrigger4Data.m_MinTime = SVClock::GetMaxTimeStamp();
}

CSVIOTESTDlg::~CSVIOTESTDlg()
{
}

bool CSVIOTESTDlg::IsSoftwareTrigger() const
{
	return (m_csTrigger.CompareNoCase("SVSoftwareTriggerDevice.dll") == 0);
}

bool CSVIOTESTDlg::AllowTriggerParamEditing() const
{
	bool bRetVal = false;
	if( 0 == m_csDigital.CompareNoCase("SVNId.dll") ||
		0 == m_csDigital.CompareNoCase("SVNI.dll") ||
		IsSoftwareTrigger())
	{
		bRetVal = true;
	}
	return bRetVal;
}

void CSVIOTESTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSVIOTESTDlg)
	DDX_Control(pDX, IDC_BOARD_MODEL_COMBO, m_BoardModelCombo);
	DDX_Control(pDX, IDC_SYSTEM_TEXT, m_BoardTxt);
	DDX_Control(pDX, IDC_FAN4_TXT, m_Fan4Txt);
	DDX_Control(pDX, IDC_FAN3_TXT, m_Fan3Txt);
	DDX_Control(pDX, IDC_FAN2_TXT, m_Fan2Txt);
	DDX_Control(pDX, IDC_FAN1_TXT, m_Fan1Txt);
	DDX_Control(pDX, IDC_FAN4, m_Fan4);
	DDX_Control(pDX, IDC_FAN3, m_Fan3);
	DDX_Control(pDX, IDC_FAN2, m_Fan2);
	DDX_Control(pDX, IDC_FAN1, m_Fan1);
	DDX_Control(pDX, IDC_TRIGGER4_MINIMUM, m_Trigger4Minimum);
	DDX_Control(pDX, IDC_TRIGGER3_MINIMUM, m_Trigger3Minimum);
	DDX_Control(pDX, IDC_TRIGGER2_MINIMUM, m_Trigger2Minimum);
	DDX_Control(pDX, IDC_TRIGGER1_MINIMUM, m_Trigger1Minimum);
	DDX_Control(pDX, IDC_TRIGGER4_MAXIMUM, m_Trigger4Maximum);
	DDX_Control(pDX, IDC_TRIGGER3_MAXIMUM, m_Trigger3Maximum);
	DDX_Control(pDX, IDC_TRIGGER2_MAXIMUM, m_Trigger2Maximum);
	DDX_Control(pDX, IDC_TRIGGER1_MAXIMUM, m_Trigger1Maximum);
	DDX_Control(pDX, IDC_TRIGGER3_DISTANCE, m_Trigger3Distance);
	DDX_Control(pDX, IDC_TRIGGER4_DISTANCE, m_Trigger4Distance);
	DDX_Control(pDX, IDC_TRIGGER2_DISTANCE, m_Trigger2Distance);
	DDX_Control(pDX, IDC_TRIGGER4_COUNT, m_Trigger4Count);
	DDX_Control(pDX, IDC_TRIGGER3_COUNT, m_Trigger3Count);
	DDX_Control(pDX, IDC_TRIGGER2_COUNT, m_Trigger2Count);
	DDX_Control(pDX, IDC_TRIGGER4_AVERAGE, m_Trigger4Average);
	DDX_Control(pDX, IDC_TRIGGER3_AVERAGE, m_Trigger3Average);
	DDX_Control(pDX, IDC_TRIGGER2_AVERAGE, m_Trigger2Average);
	DDX_Control(pDX, IDC_TRIGGER1_DISTANCE, m_Trigger1Distance);
	DDX_Control(pDX, IDC_TRIGGER1_COUNT, m_Trigger1Count);
	DDX_Control(pDX, IDC_TRIGGER1_AVERAGE, m_Trigger1Average);
	DDX_Control(pDX, IDC_RADIO1, m_cbtSlow);
	DDX_Control(pDX, IDC_INPUT8, m_input8);
	DDX_Control(pDX, IDC_INPUT7, m_input7);
	DDX_Control(pDX, IDC_INPUT6, m_input6);
	DDX_Control(pDX, IDC_INPUT5, m_input5);
	DDX_Control(pDX, IDC_INPUT4, m_input4);
	DDX_Control(pDX, IDC_INPUT3, m_input3);
	DDX_Control(pDX, IDC_INPUT2, m_input2);
	DDX_Control(pDX, IDC_INPUT1, m_input1);
	DDX_Text(pDX, IDC_STATIC_CHANNEL, m_lStaticChannel);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_FAN_FREQ1, m_lFanFreq1);
	DDX_Text(pDX, IDC_FAN_FREQ2, m_lFanFreq2);
	DDX_Text(pDX, IDC_FAN_FREQ3, m_lFanFreq3);
	DDX_Text(pDX, IDC_FAN_FREQ4, m_lFanFreq4);
}

BEGIN_MESSAGE_MAP(CSVIOTESTDlg, CDialog)
	//{{AFX_MSG_MAP(CSVIOTESTDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON4, OnSequence)
	ON_BN_CLICKED(IDC_RADIO1, OnSlow)
	ON_BN_CLICKED(IDC_RADIO2, OnFast)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_TEST_OUTPUTS, OnTestOutputs)
	ON_BN_CLICKED(IDC_START_TRIGGERS, OnStartTriggers)
	ON_BN_CLICKED(IDC_STOP_TRIGGERS, OnStopTriggers)
	ON_BN_CLICKED(IDC_START_OUTPUTS, OnStartTest)
	ON_BN_CLICKED(IDC_RANDOM_BUTTON, OnRandBtn)
	ON_BN_CLICKED(IDC_FAST_BUTTON, OnFastSlow)
	ON_EN_CHANGE(IDC_STATIC_CHANNEL, OnChangeStaticChannel)
	ON_BN_CLICKED(IDC_TRIGGER_PARAM, OnTriggerParam)
	ON_CBN_SELCHANGE(IDC_BOARD_MODEL_COMBO, OnSelchangeBoardModelCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSVIOTESTDlg message handlers

BOOL CSVIOTESTDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT(IDM_ABOUTBOX == (IDM_ABOUTBOX & 0xFFF0));
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (nullptr != pSysMenu)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_nTimer = SetTimer(1, 10, 0);
	nSeq = 0;
	nSpeed = 0; //slow
	m_cbtSlow.SetCheck(~nSpeed);
	m_bInterruptEnabled = false;

	// Try to Get the board version if it exists..
	unsigned long l_lHandle;
	COleVariant l_vVersion;
	BSTR l_bstName = nullptr;
	CString l_strName;
	CString l_strWindowName;
	CString l_strNewName;
	CString l_strValue;
	m_psvTriggers->GetHandle( &l_lHandle, 0 );

	// Strobe and Trigger invert signals are write only.
	// therefore we need to remember how we set them.
	m_lStrobeInverts = 0;
	m_lTrigInverts = 0;

	// Read Information from hardware.ini about the board options...
	SVOINIClass l_INIHardware;
	l_INIHardware.SetFile( SvStl::GlobalPath::Inst().GetHardwareIniPath() );
	std::string l_strDesc;

	l_strDesc = l_INIHardware.GetValueString("IO Board","10","Entech X1");
	l_strDesc = "10 - " + l_strDesc;
	m_BoardModelCombo.AddString( l_strDesc.c_str() );

	l_strDesc = l_INIHardware.GetValueString("IO Board","20","Entech X2");
	l_strDesc = "20 - " + l_strDesc;
	m_BoardModelCombo.AddString( l_strDesc.c_str() );

	l_strDesc = l_INIHardware.GetValueString("IO Board","12","Entech X3");
	l_strDesc = "12 - " + l_strDesc;
	m_BoardModelCombo.AddString( l_strDesc.c_str() );

	// Default board is an IO board model 10
	m_BoardModel = 0;

	if(0 == m_csDigital.CompareNoCase(SVLPTIODLL))
	{
		VARIANT l_vValue;
		::VariantInit( &l_vValue );
		switch(m_lSystemType)
		{
			case SVRABBIT_X1:
			{
				ShowFans(true);
				m_BoardModel = 0;
				break;
			}
			case SVRABBIT_X2:
			{
				ShowFans(true);
				m_BoardModel = 1;
				break;
			}
			case SVRABBIT_X3:
			{
				ShowFans(true);
				m_BoardModel = 2;
				break;
			}
		}
		m_BoardModelCombo.SetCurSel( m_BoardModel );
	}
	else
	{
		ShowFans(false);

		m_BoardTxt.ShowWindow( SW_HIDE );

		GetDlgItem(IDC_BOARD_MODEL_COMBO)->ShowWindow( SW_HIDE );

		if (!AllowTriggerParamEditing())
		{
			GetDlgItem(IDC_TRIGGER_PARAM)->ShowWindow( SW_HIDE );
		}
	}
	// SVBoardVersion enum is used here to make the code more clear.
	// however at some time in the future the Dll parameters may be implemented
	// as an array and therefore this enum may not apply.
	HRESULT l_hr = m_psvTriggers->GetParameterName( l_lHandle, SVBoardVersion, &l_bstName );

	if( S_OK == l_hr )
	{
		l_strName = l_bstName;
		GetWindowText( l_strWindowName );
		::SysFreeString( l_bstName );
		l_hr = m_psvTriggers->GetParameterValue( l_lHandle, SVBoardVersion, &l_vVersion );
		// Display Information about the Board.
		l_strValue = l_vVersion.bstrVal;
		::VariantClear( &l_vVersion );
		l_strNewName.Format("%s - %s (%s - %s)",
			l_strWindowName,
			m_csDigital,
			l_strName, 
			l_strValue );
			
		SetWindowText( l_strNewName );
	}
	m_bThreadRunning = false;
	m_bTestRand = false;
	m_hWorkerThread = nullptr;
	m_bFast = false;
	m_lStaticChannel = 15;
	UpdateData(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSVIOTESTDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (IDM_ABOUTBOX == (nID & 0xFFF0))
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSVIOTESTDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSVIOTESTDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSVIOTESTDlg::OnButton1() 
{
	nSeq = 0; 

// TURN ON ALL OUPUTS
	unsigned long numOutputs;
	SvTh::SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputCount(numOutputs);
	
	for( DWORD dwChan = 0 ; dwChan < numOutputs; dwChan++)
	{
		SvTh::SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue( dwChan, false );
	}
}

void CSVIOTESTDlg::OnButton2() 
{
	nSeq = 0; 

// TURN OFF ALL OUPUTS
	unsigned long numOutputs;
	SvTh::SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputCount(numOutputs);
	for( DWORD dwChan = 0 ; dwChan < numOutputs ; dwChan++)
	{
		SvTh::SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue( dwChan, true );
	}
}

void CSVIOTESTDlg::OnTimer( UINT_PTR nIDEvent )
{
	DWORD Channel =0;
	bool bValue = 0;
	static int nCounter = 0;
	int nCount = 0;
	static int nSeqCount = 0;
	CStatic *pCStatic;

	// Update Digital Input status Indicators
	unsigned long numInputs;
	SvTh::SVIOConfigurationInterfaceClass::Instance().GetDigitalInputCount(numInputs);
	for (DWORD dwChan = 0 ; dwChan < numInputs ; dwChan++)
	{
		SvTh::SVIOConfigurationInterfaceClass::Instance().GetDigitalInputValue(dwChan, bValue);
		switch (dwChan)
		{
			case 0:
				pCStatic = &m_input1;
				break;
			case 1:
				pCStatic = &m_input2;
				break;
			case 2:
				pCStatic = &m_input3;
				break;
			case 3:
				pCStatic = &m_input4;
				break;
			case 4:
				pCStatic = &m_input5;
				break;
			case 5:
				pCStatic = &m_input6;
				break;
			case 6:
				pCStatic = &m_input7;
				break;
			case 7:
				pCStatic = &m_input8;
				break;
			default:
				pCStatic = &m_input1;
		}
		if( (SVRABBIT_X2 == m_lSystemType || SVRABBIT_X3 == m_lSystemType) && (6 == dwChan || 7 == dwChan) )
		{
			pCStatic->SetIcon(AfxGetApp()->LoadIcon( IDI_ICON5));
		}
		else if(bValue)
		{
			pCStatic->SetIcon(AfxGetApp()->LoadIcon(IDI_ICON1));
		}
		else
		{
			pCStatic->SetIcon(AfxGetApp()->LoadIcon(IDI_ICON3));
		}
	}

	// Read Fans
	if(0 == m_csDigital.CompareNoCase(SVLPTIODLL))
	{
		VARIANT l_vValue;
		::VariantInit( &l_vValue);
		if( S_OK == SvTh::SVIOConfigurationInterfaceClass::Instance().GetParameterValue(SVFanState, &l_vValue) )
		{
			m_Fan1.SetIcon( AfxGetApp()->LoadIcon( ( l_vValue.lVal & 1 ) ? IDI_ICON4 : IDI_ICON3 ));
			m_Fan2.SetIcon( AfxGetApp()->LoadIcon( ( l_vValue.lVal & 2 ) ? IDI_ICON4 : IDI_ICON3 ));
			m_Fan3.SetIcon( AfxGetApp()->LoadIcon( ( l_vValue.lVal & 4 ) ? IDI_ICON4 : IDI_ICON3 ));
			m_Fan4.SetIcon( AfxGetApp()->LoadIcon( ( l_vValue.lVal & 8 ) ? IDI_ICON4 : IDI_ICON3 ));
		}
		if( S_OK == SvTh::SVIOConfigurationInterfaceClass::Instance().GetParameterValue(SVFanFreq, &l_vValue) )
		{
			m_lFanFreq1 = l_vValue.lVal & 0xff;
			m_lFanFreq2 = (l_vValue.lVal >> 8) & 0xff;
			m_lFanFreq3 = (l_vValue.lVal >> 16) & 0xff;
			m_lFanFreq4 = (l_vValue.lVal >> 24) & 0xff;
			UpdateData(FALSE);
		}
	}

	// Sequence Outputs 0 - 7 and 8 - 15 
	unsigned long numOutputs;
	SvTh::SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputCount(numOutputs);

	if (nSeq != 0)
	{
		if (nSpeed != 0)
		{
			nCount = 10;
		}
		else
		{
			nCount = 80;
		}

		if (nCounter >= nCount)
		{
			// Outputs 0 - 7  
			SvTh::SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue((nSeqCount + 7) % 8 , true);
			SvTh::SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue(nSeqCount, false);
			
			// Outputs 8 - 15 (actually numOutputs now)
			SvTh::SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue((nSeqCount + 7) % 8 + 8, true);
			SvTh::SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue(nSeqCount + (numOutputs - 8) , false);

			if (7 == nSeqCount)
			{
				nSeqCount = 0;
			}
			else
			{
				nSeqCount++;
			}
			nCounter = 0;
		}
		nCounter++;
	}

	CString csText;

	unsigned long numTriggers = 0;
	m_psvTriggers->GetCount(&numTriggers);

	if (numTriggers > 0)
	{
		if ( 1 < m_svTrigger1Data.lTriggerCount && 
			 m_lTrigger1Count != m_svTrigger1Data.lTriggerCount )
		{
			m_lTrigger1Count = m_svTrigger1Data.lTriggerCount;

			long l_lCount = 100;

			if ( ( m_lTrigger1Count - 1 ) < l_lCount )
			{
				l_lCount = m_lTrigger1Count - 1;
			}

			long l_lLastValue = (long)( m_svTrigger1Data.m_LastTime * 1000 );

			long l_lAvgValue = (long)( ( m_svTrigger1Data.m_TotalTime * 1000 ) / l_lCount );

			long l_lMaxValue = (long)( m_svTrigger1Data.m_MaxTime * 1000 );

			long l_lMinValue = (long)( m_svTrigger1Data.m_MinTime * 1000 );

			csText.Format( "%ld", l_lMaxValue );
			m_Trigger1Maximum.SetWindowText( csText );

			csText.Format( "%ld", l_lMinValue );
			m_Trigger1Minimum.SetWindowText( csText );

			csText.Format( "%ld", m_lTrigger1Count );
			m_Trigger1Count.SetWindowText( csText );

			csText.Format( "%ld", l_lLastValue );
			m_Trigger1Distance.SetWindowText( csText );

			csText.Format( "%ld", l_lAvgValue );
			m_Trigger1Average.SetWindowText( csText );
		}
	}
	if (numTriggers > 1)
	{
		if ( 1 < m_svTrigger2Data.lTriggerCount && 
			 m_lTrigger2Count != m_svTrigger2Data.lTriggerCount )
		{
			m_lTrigger2Count = m_svTrigger2Data.lTriggerCount;

			long l_lCount = 100;

			if ( ( m_lTrigger2Count - 1 ) < l_lCount )
			{
				l_lCount = m_lTrigger2Count - 1;
			}

			long l_lLastValue = (long)( m_svTrigger2Data.m_LastTime * 1000 );

			long l_lAvgValue = (long)( ( m_svTrigger2Data.m_TotalTime * 1000 ) / l_lCount );

			long l_lMaxValue = (long)( m_svTrigger2Data.m_MaxTime * 1000 );

			long l_lMinValue = (long)( m_svTrigger2Data.m_MinTime * 1000 );

			csText.Format( "%ld", l_lMaxValue );
			m_Trigger2Maximum.SetWindowText( csText );

			csText.Format( "%ld", l_lMinValue );
			m_Trigger2Minimum.SetWindowText( csText );

			csText.Format( "%ld", m_lTrigger2Count );
			m_Trigger2Count.SetWindowText( csText );

			csText.Format( "%ld", l_lLastValue );
			m_Trigger2Distance.SetWindowText( csText );

			csText.Format( "%ld", l_lAvgValue );
			m_Trigger2Average.SetWindowText( csText );
		}
	}
	if (numTriggers > 2)
	{
		if ( 1 < m_svTrigger3Data.lTriggerCount && 
			 m_lTrigger3Count != m_svTrigger3Data.lTriggerCount )
		{
			m_lTrigger3Count = m_svTrigger3Data.lTriggerCount;

			long l_lCount = 100;

			if ( ( m_lTrigger3Count - 1 ) < l_lCount )
			{
				l_lCount = m_lTrigger3Count - 1;
			}

			long l_lLastValue = (long)( m_svTrigger3Data.m_LastTime * 1000 );

			long l_lAvgValue = (long)( ( m_svTrigger3Data.m_TotalTime * 1000 ) / l_lCount );

			long l_lMaxValue = (long)( m_svTrigger3Data.m_MaxTime * 1000 );

			long l_lMinValue = (long)( m_svTrigger3Data.m_MinTime * 1000 );

			csText.Format( "%ld", l_lMaxValue );
			m_Trigger3Maximum.SetWindowText( csText );

			csText.Format( "%ld", l_lMinValue );
			m_Trigger3Minimum.SetWindowText( csText );

			csText.Format( "%ld", m_lTrigger3Count );
			m_Trigger3Count.SetWindowText( csText );

			csText.Format( "%ld", l_lLastValue );
			m_Trigger3Distance.SetWindowText( csText );

			csText.Format( "%ld", l_lAvgValue );
			m_Trigger3Average.SetWindowText( csText );
		}
	}
	if (numTriggers > 3)
	{
		if ( 1 < m_svTrigger4Data.lTriggerCount && 
			 m_lTrigger4Count != m_svTrigger4Data.lTriggerCount )
		{
			m_lTrigger4Count = m_svTrigger4Data.lTriggerCount;

			long l_lCount = 100;

			if ( ( m_lTrigger4Count - 1 ) < l_lCount )
			{
				l_lCount = m_lTrigger4Count - 1;
			}

			long l_lLastValue = (long)( m_svTrigger4Data.m_LastTime * 1000 );

			long l_lAvgValue = (long)( ( m_svTrigger4Data.m_TotalTime * 1000 ) / l_lCount );

			long l_lMaxValue = (long)( m_svTrigger4Data.m_MaxTime * 1000 );

			long l_lMinValue = (long)( m_svTrigger4Data.m_MinTime * 1000 );

			csText.Format( "%ld", l_lMaxValue );
			m_Trigger4Maximum.SetWindowText( csText );

			csText.Format( "%ld", l_lMinValue );
			m_Trigger4Minimum.SetWindowText( csText );

			csText.Format( "%ld", m_lTrigger4Count );
			m_Trigger4Count.SetWindowText( csText );

			csText.Format( "%ld", l_lLastValue );
			m_Trigger4Distance.SetWindowText( csText );

			csText.Format( "%ld", l_lAvgValue );
			m_Trigger4Average.SetWindowText( csText );
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void CSVIOTESTDlg::OnClose() 
{
	KillTimer(m_nTimer);
	
	CDialog::OnClose();
}

void CSVIOTESTDlg::OnSequence() 
{
	nSeq = ~nSeq;
	// TURN OFF ALL OUPUTS
	unsigned long numOutputs;
	SvTh::SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputCount(numOutputs);
	for(DWORD dwChan = 0 ; dwChan < numOutputs ; dwChan++)
	{
		SvTh::SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue(dwChan, true);
	}
}

void CSVIOTESTDlg::OnSlow() 
{
	nSpeed = 0;
}

void CSVIOTESTDlg::OnFast() 
{
	nSpeed = 1;
}

void CSVIOTESTDlg::OnDestroy() 
{
	m_bThreadRunning = false;
	Sleep(100);

	if(m_bInterruptEnabled)
	{
		OnStopTriggers();
	}
	CDialog::OnDestroy();
}

void CSVIOTESTDlg::OnTestOutputs() 
{
	SvTh::SVIOConfigurationInterfaceClass::Instance().TestDigitalOutputs();
}

HRESULT CALLBACK SVCallback( SvTh::TriggerParameters triggerparams)
{
	SVClock::SVTimeStamp l_TimeStamp = SVClock::GetTimeStamp();

	if ( nullptr != triggerparams.m_pData )
	{
		SVIOTriggerDataStruct *l_pData = (SVIOTriggerDataStruct *)(triggerparams.m_pData);

		(l_pData->lTriggerCount)++;

		if ( 101 < l_pData->lTriggerCount )
		{
			l_pData->m_TotalTime -= ( l_pData->m_TimeStamp[ l_pData->ulNextIndex ] -
			                           l_pData->m_TimeStamp[ l_pData->ulIndex ] ) ;
		}

		l_pData->m_TimeStamp[ l_pData->ulIndex ] = l_TimeStamp;

		if ( 1 < l_pData->lTriggerCount )
		{
			l_pData->m_LastTime = ( l_pData->m_TimeStamp[ l_pData->ulIndex ] -
															 l_pData->m_TimeStamp[ l_pData->ulLastIndex ] ) ;

			l_pData->m_TotalTime += l_pData->m_LastTime;

			if ( 2 < l_pData->lTriggerCount )
			{
				if ( l_pData->m_MaxTime < l_pData->m_LastTime )
				{
					l_pData->m_MaxTime = l_pData->m_LastTime;
				}

				if ( l_pData->m_LastTime < l_pData->m_MinTime )
				{
					l_pData->m_MinTime = l_pData->m_LastTime;
				}
			}
		}

		l_pData->ulLastIndex = l_pData->ulIndex;
		l_pData->ulIndex = l_pData->ulNextIndex;
		l_pData->ulNextIndex = ( l_pData->ulNextIndex + 1 ) % 100;
	}

	return S_OK;
}

void CSVIOTESTDlg::OnStartTriggers() 
{
	CString csText("0");

	unsigned long numTriggers = 0;
	m_psvTriggers->GetCount(&numTriggers);

	SvTh::TriggerCallbackInformation TriggerCallbackInfo;

	TriggerCallbackInfo.m_pCallback = SVCallback;

	TriggerCallbackInfo.m_TriggerParameters = SvTh::TriggerParameters(this, &m_svTrigger1Data);

	m_svTrigger1Data.ulLastIndex = 0;
	m_svTrigger1Data.ulIndex = 0;
	m_svTrigger1Data.ulNextIndex = ( m_svTrigger1Data.ulIndex + 1 ) % 100;
	m_svTrigger1Data.lTriggerCount = 0;
	m_svTrigger1Data.m_LastTime = 0;
	m_svTrigger1Data.m_TotalTime = 0;
	m_svTrigger1Data.m_MaxTime = SVClock::GetMinTimeStamp();
	m_svTrigger1Data.m_MinTime = SVClock::GetMaxTimeStamp();

	m_Trigger1Count.SetWindowText( csText );
	m_Trigger1Distance.SetWindowText( csText );
	m_Trigger1Average.SetWindowText( csText );
	m_Trigger1Minimum.SetWindowText( csText );
	m_Trigger1Maximum.SetWindowText( csText );

	if (numTriggers > 0)
	{
		m_psvTriggers->Register( 1, TriggerCallbackInfo );
	}

	TriggerCallbackInfo.m_TriggerParameters.m_pData = &m_svTrigger2Data;

	m_svTrigger2Data.ulLastIndex = 0;
	m_svTrigger2Data.ulIndex = 0;
	m_svTrigger2Data.ulNextIndex = ( m_svTrigger2Data.ulIndex + 1 ) % 100;
	m_svTrigger2Data.lTriggerCount = 0;
	m_svTrigger2Data.m_LastTime = 0;
	m_svTrigger2Data.m_TotalTime = 0;
	m_svTrigger2Data.m_MaxTime = SVClock::GetMinTimeStamp();
	m_svTrigger2Data.m_MinTime = SVClock::GetMaxTimeStamp();

	m_Trigger2Count.SetWindowText( csText );
	m_Trigger2Distance.SetWindowText( csText );
	m_Trigger2Average.SetWindowText( csText );
	m_Trigger2Minimum.SetWindowText( csText );
	m_Trigger2Maximum.SetWindowText( csText );

	if (numTriggers > 1)
	{
		m_psvTriggers->Register( 2, TriggerCallbackInfo );
	}
	TriggerCallbackInfo.m_TriggerParameters.m_pData = &m_svTrigger3Data;

	m_svTrigger3Data.ulLastIndex = 0;
	m_svTrigger3Data.ulIndex = 0;
	m_svTrigger3Data.ulNextIndex = ( m_svTrigger3Data.ulIndex + 1 ) % 100;
	m_svTrigger3Data.lTriggerCount = 0;
	m_svTrigger3Data.m_LastTime = 0;
	m_svTrigger3Data.m_TotalTime = 0;
	m_svTrigger3Data.m_MaxTime = SVClock::GetMinTimeStamp();
	m_svTrigger3Data.m_MinTime = SVClock::GetMaxTimeStamp();

	m_Trigger3Count.SetWindowText( csText );
	m_Trigger3Distance.SetWindowText( csText );
	m_Trigger3Average.SetWindowText( csText );
	m_Trigger3Minimum.SetWindowText( csText );
	m_Trigger3Maximum.SetWindowText( csText );

	if (numTriggers > 2)
	{
		m_psvTriggers->Register( 3, TriggerCallbackInfo );
	}
	TriggerCallbackInfo.m_TriggerParameters.m_pData = &m_svTrigger4Data;

	m_svTrigger4Data.ulLastIndex = 0;
	m_svTrigger4Data.ulIndex = 0;
	m_svTrigger4Data.ulNextIndex = ( m_svTrigger4Data.ulIndex + 1 ) % 100;
	m_svTrigger4Data.lTriggerCount = 0;
	m_svTrigger4Data.m_LastTime = 0;
	m_svTrigger4Data.m_TotalTime = 0;
	m_svTrigger4Data.m_MaxTime = SVClock::GetMinTimeStamp();
	m_svTrigger4Data.m_MinTime = SVClock::GetMaxTimeStamp();

	m_Trigger4Count.SetWindowText( csText );
	m_Trigger4Distance.SetWindowText( csText );
	m_Trigger4Average.SetWindowText( csText );
	m_Trigger4Minimum.SetWindowText( csText );
	m_Trigger4Maximum.SetWindowText( csText );

	if (numTriggers > 3)
	{
		m_psvTriggers->Register( 4, TriggerCallbackInfo );
	}
	if (numTriggers > 0)
	{
		m_psvTriggers->Start( 1 );
	}
	if (numTriggers > 1)
	{
		m_psvTriggers->Start( 2 );
	}
	if (numTriggers > 2)
	{
		m_psvTriggers->Start( 3 );
	}
	if (numTriggers > 3)
	{
		m_psvTriggers->Start( 4 );
	}
	m_bInterruptEnabled = true;
	if (IsSoftwareTrigger())
	{
		GetDlgItem(IDC_TRIGGER_PARAM)->EnableWindow(false);
	}
}

void CSVIOTESTDlg::OnStopTriggers() 
{
	SvTh::TriggerCallbackInformation TriggerCallbackInfo;

	unsigned long numTriggers = 0;
	m_psvTriggers->GetCount(&numTriggers);

	if (numTriggers > 3)
	{
		m_psvTriggers->Stop( 4 );
	}

	if (numTriggers > 2)
	{
		m_psvTriggers->Stop( 3 );
	}

	if (numTriggers > 1)
	{
		m_psvTriggers->Stop( 2 );
	}

	if (numTriggers > 0)
	{
		m_psvTriggers->Stop( 1 );
	}

	TriggerCallbackInfo.m_pCallback = SVCallback;
	TriggerCallbackInfo.m_TriggerParameters.m_pOwner = this;

	TriggerCallbackInfo.m_TriggerParameters.m_pData = &m_svTrigger4Data;

	if (numTriggers > 3)
	{
		m_psvTriggers->Unregister( 4, TriggerCallbackInfo );
	}
	TriggerCallbackInfo.m_TriggerParameters.m_pData = &m_svTrigger3Data;

	if (numTriggers > 2)
	{
		m_psvTriggers->Unregister( 3, TriggerCallbackInfo );
	}

	TriggerCallbackInfo.m_TriggerParameters.m_pData = &m_svTrigger2Data;

	if (numTriggers > 1)
	{
		m_psvTriggers->Unregister( 2, TriggerCallbackInfo );
	}
	TriggerCallbackInfo.m_TriggerParameters.m_pData = &m_svTrigger1Data;

	if (numTriggers > 1)
	{
		m_psvTriggers->Unregister( 1, TriggerCallbackInfo );
	}
	m_bInterruptEnabled = false;

	if (IsSoftwareTrigger())
	{
		GetDlgItem(IDC_TRIGGER_PARAM)->EnableWindow(true);
	}
}

DWORD WINAPI SVWorkerThreadFunc( LPVOID lpParam )
{
	CSVIOTESTDlg* l_pOwner = (CSVIOTESTDlg*) lpParam;

	DWORD dwResult = 0;
	DWORD l_dwAccum = 0;
	do
	{
		bool l_bState = 0;
		if( l_pOwner->m_bTestRand )
		{
			l_dwAccum = (unsigned int )rand() + (( rand() << 1) & 0x8000 );

		}
		else
		{
			l_dwAccum++;
		}
		if( !l_pOwner->m_bFast )
		{
			Sleep( 1 );
		}
		for ( int i = 0 ; i < 16 ; i++ )
		{
			if( i != l_pOwner->m_lStaticChannel )
			{
				l_bState = ((l_dwAccum & (1 << i)) != 0);
				if( S_OK != SvTh::SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue( i, l_bState ) )
				{
					AfxMessageBox("Bad Return Code");
				}
			}
		}
	} while( l_pOwner->m_bThreadRunning );

	return 0L;
}

void CSVIOTESTDlg::OnStartTest() 
{
	DWORD dwThreadID;

	if( m_lStaticChannel >= 0 && m_lStaticChannel < 16 )
	{
		SvTh::SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue( m_lStaticChannel, false );
	}

	if( !m_bThreadRunning )
	{
		m_bThreadRunning = true;
		m_hWorkerThread = ::CreateThread(nullptr, 0, SVWorkerThreadFunc, (LPVOID)this, 0, &dwThreadID );
	}
	else
	{
		m_bThreadRunning = false;
	}
}

void CSVIOTESTDlg::OnRandBtn() 
{
	m_bTestRand = !m_bTestRand;
}

void CSVIOTESTDlg::OnFastSlow() 
{
	m_bFast = !m_bFast;
}

void CSVIOTESTDlg::OnChangeStaticChannel() 
{
	UpdateData();
	if( m_bThreadRunning )
	{
		if( m_lStaticChannel >= 0 && m_lStaticChannel < 16 )
		{
			SvTh::SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue( m_lStaticChannel, false );
		}
		else
		{
			AfxMessageBox( "Channel Out of Range");
		}
	}
}

void CSVIOTESTDlg::OnTriggerParam() 
{
	if (IsSoftwareTrigger())
	{
		SVSoftwareTriggerSetupDlg l_dlg;
		unsigned long l_ulHandle;
		unsigned long l_ulCount;
		VARIANT l_vValue;
		::VariantInit( &l_vValue);
		l_vValue.vt = VT_I4;

		m_psvTriggers->GetCount(&l_ulCount );
		for( unsigned long x = 0 ; x < l_ulCount ; x++ )
		{
			m_psvTriggers->GetHandle(&l_ulHandle, x );
			m_psvTriggers->GetParameterValue( l_ulHandle, 0, &l_vValue);

			switch (x)
			{
				case 0:
				l_dlg.m_period1 = l_vValue.lVal;
				break;

				case 1:
				l_dlg.m_period2 = l_vValue.lVal;
				break;

				case 2:
				l_dlg.m_period3 = l_vValue.lVal;
				break;

				case 3:
				l_dlg.m_period4 = l_vValue.lVal;
				break;
			}
		}
		if( IDOK == l_dlg.DoModal() )
		{
			for( unsigned long x = 0 ; x < l_ulCount ; x++ )
			{
				m_psvTriggers->GetHandle(&l_ulHandle, x );
			
				switch (x)
				{
					case 0:
					l_vValue.lVal = l_dlg.m_period1;
					break;

					case 1:
					l_vValue.lVal = l_dlg.m_period2;
					break;

					case 2:
					l_vValue.lVal = l_dlg.m_period3;
					break;

					case 3:
					l_vValue.lVal = l_dlg.m_period4;
					break;
				}
				m_psvTriggers->SetParameterValue( l_ulHandle, 0, &l_vValue);
			}
		}
	}
	else
	{
		SVTriggerSetupDlgClass l_dlg;
		unsigned long l_ulHandle;
		unsigned long l_ulCount;
		VARIANT l_vValue;
		::VariantInit( &l_vValue);
		l_dlg.m_psvTriggers = m_psvTriggers;

		l_dlg.m_lStrobeInverts = m_lStrobeInverts;
		l_dlg.m_lTrigInverts = m_lTrigInverts;
		l_dlg.m_lSystemType = m_lSystemType;

		m_psvTriggers->GetCount(&l_ulCount );
		for( unsigned long x = 0 ; x < l_ulCount ; x++ )
		{
			m_psvTriggers->GetHandle(&l_ulHandle, x );
			m_psvTriggers->GetParameterValue( l_ulHandle, SVSignalEdge, &l_vValue);
			switch( x )
			{
				case 0:
				{
					l_dlg.m_bTrig1Rising = l_vValue.lVal > 0 ? TRUE : FALSE;
					break;
				}
				case 1:
				{
					l_dlg.m_bTrig2Rising = l_vValue.lVal > 0 ? TRUE : FALSE;
					break;
				}
				case 2:
				{
					l_dlg.m_bTrig3Rising = l_vValue.lVal > 0 ? TRUE : FALSE;
					break;
				}
				case 3:
				{
					l_dlg.m_bTrig4Rising = l_vValue.lVal > 0 ? TRUE : FALSE;
					break;
				}
			}
		}
		if( IDOK == l_dlg.DoModal() )
		{
			for( unsigned long x = 0 ; x < l_ulCount ; x++ )
			{
				m_psvTriggers->GetHandle(&l_ulHandle, x );
				switch( x )
				{
					case 0:
					{
						l_vValue.lVal = l_dlg.m_bTrig1Rising ? 1 : -1;
						break;
					}
					case 1:
					{
						l_vValue.lVal = l_dlg.m_bTrig2Rising ? 1 : -1;
						break;
					}
					case 2:
					{
						l_vValue.lVal = l_dlg.m_bTrig3Rising ? 1 : -1;
						break;
					}
					case 3:
					{
						l_vValue.lVal = l_dlg.m_bTrig4Rising ? 1 : -1;
						break;
					}
				}
				m_psvTriggers->SetParameterValue( l_ulHandle, SVSignalEdge, &l_vValue);
			}
		}
		m_lStrobeInverts = l_dlg.m_lStrobeInverts;
		m_lTrigInverts = l_dlg.m_lTrigInverts;
	}
}

void CSVIOTESTDlg::ShowFans( bool p_bShow )
{
	UINT cmd = (p_bShow) ? SW_SHOW : SW_HIDE;
	
	m_Fan1.ShowWindow( cmd );
	m_Fan2.ShowWindow( cmd );
	m_Fan3.ShowWindow( cmd );
	m_Fan4.ShowWindow( cmd );
	m_Fan1Txt.ShowWindow( cmd );
	m_Fan2Txt.ShowWindow( cmd );
	m_Fan3Txt.ShowWindow( cmd );
	m_Fan4Txt.ShowWindow( cmd );
	GetDlgItem(IDC_FAN_FREQ1)->ShowWindow( cmd );
	GetDlgItem(IDC_FAN_FREQ2)->ShowWindow( cmd );
	GetDlgItem(IDC_FAN_FREQ3)->ShowWindow( cmd );
	GetDlgItem(IDC_FAN_FREQ4)->ShowWindow( cmd );
}

void CSVIOTESTDlg::OnSelchangeBoardModelCombo() 
{
	VARIANT l_vt;
	::VariantInit( &l_vt );
	l_vt.vt = VT_I4;

	int l_iModelSel = m_BoardModelCombo.GetCurSel();

	switch( l_iModelSel )
	{
		case 0:	// X1 IO Board
		{
			m_lSystemType = SVRABBIT_X1;
			l_vt.lVal = m_lSystemType;
			ShowFans(true);
			break;
		}
		case 1:	// X2 IO Board
		{
			m_lSystemType = SVRABBIT_X2;
			l_vt.lVal = m_lSystemType;
			ShowFans(true);
			break;
		}
		case 2: // X3 IO Board
		{
			m_lSystemType = SVRABBIT_X3;
			l_vt.lVal = m_lSystemType;
			ShowFans(true);
			break;
		}
		default:
		{
			m_lSystemType = SVRABBIT_X1;
			l_vt.lVal = m_lSystemType;
			break;
		}
	}
	if( S_OK != SvTh::SVIOConfigurationInterfaceClass::Instance().SetParameterValue( SVBoardType, &l_vt ) )
	{
		AfxMessageBox(" Error Setting System Type");
	}
}

