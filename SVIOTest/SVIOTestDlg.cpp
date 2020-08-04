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
#include "SVIOTest.h"
#include "SVIOTestDlg.h"
#include "TriggerHandling/TriggerDispatcher.h"
#include "SVTriggerSetupDlgClass.h"
#include "SVSoftwareTriggerSetupDlg.h"
#include "SVLibrary/SVOIniClass.h"
#include "SVIOLibrary/SVIOParameterEnum.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVTimerLibrary/SVClock.h"
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
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
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
{
	//{{AFX_DATA_INIT(CSVIOTESTDlg)
	m_lStaticChannel = 0;
	m_BoardModel = -1;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	for (int triggerchannel = 0; triggerchannel < c_upperBoundForTriggerChannel; triggerchannel++)
	{
		m_FanFreq[triggerchannel] = 0;
		m_TriggerData[triggerchannel].OnTriggerStart();
	}
}

CSVIOTESTDlg::~CSVIOTESTDlg()
{
}

SVIOTriggerDataStruct* CSVIOTESTDlg::getTriggerData(unsigned long triggerChannel)
{
	SVIOTriggerDataStruct* pResult{nullptr};
	if(c_upperBoundForTriggerChannel > triggerChannel)
	{
		pResult = &m_TriggerData[triggerChannel];
	}
	return pResult;
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

	// IDC_FAN_FREQ1 to IDC_FAN_FREQ4,
	// IDC_FAN1_TXT to IDC_FAN4_TXT,
	// and IDC_FAN1 to IDC_FAN4 must be contiguous and sequential for this to work:
	for (int fanID = 0; fanID < c_upperBoundForFanId; ++fanID)
	{
		DDX_Text(pDX, IDC_FAN_FREQ1 + fanID, m_FanFreq[fanID]);
		DDX_Control(pDX, IDC_FAN1_TXT + fanID, m_FanTxt[fanID]);
		DDX_Control(pDX, IDC_FAN1 + fanID, m_Fan[fanID]);
	}
	// IDC_TRIGGER1_MINIMUM to IDC_TRIGGER4_MINIMUM,
	// IDC_TRIGGER1_AVERAGE to IDC_TRIGGER4_AVERAGE
	// IDC_TRIGGER1_MAXIMUM to IDC_TRIGGER4_MAXIMUM,
	// and IDC_TRIGGER1_COUNT to IDC_TRIGGER4_COUNT must be contiguous and sequential for this to work:
	for (int triggerchannel = 0; triggerchannel < c_upperBoundForTriggerChannel; ++triggerchannel)
	{
		DDX_Control(pDX, IDC_TRIGGER1_MINIMUM + triggerchannel, m_TriggerMinimum[triggerchannel]);
		DDX_Control(pDX, IDC_TRIGGER1_AVERAGE + triggerchannel, m_TriggerAverage[triggerchannel]);
		DDX_Control(pDX, IDC_TRIGGER1_MAXIMUM + triggerchannel, m_TriggerMaximum[triggerchannel]);
		DDX_Control(pDX, IDC_TRIGGER1_DISTANCE + triggerchannel, m_TriggerDistance[triggerchannel]);
		DDX_Control(pDX, IDC_TRIGGER1_COUNT + triggerchannel, m_TriggerCountWnd[triggerchannel]);
	}

	// IDC_INPUT1 to IDC_INPUT8 must be contiguous and sequential for this to work:
	for (int inputchannel = 1; inputchannel < c_upperBoundForInputChannel; inputchannel++)
	{
		DDX_Control(pDX, IDC_INPUT1 + inputchannel - 1, m_input[inputchannel]);
	}

	DDX_Control(pDX, IDC_RADIO1, m_cbtSlow);

	DDX_Text(pDX, IDC_STATIC_CHANNEL, m_lStaticChannel);
	//}}AFX_DATA_MAP

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
	ON_BN_CLICKED(IDC_ACCEPT_TRIGGERS, &CSVIOTESTDlg::OnBnClickedAcceptTriggers)
	ON_BN_CLICKED(IDC_TEST_OUTPUTS, OnTestOutputs)
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
	assert(IDM_ABOUTBOX == (IDM_ABOUTBOX & 0xFFF0));
	assert(IDM_ABOUTBOX < 0xF000);

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
	SvLib::SVOINIClass HardwareIni( SvStl::GlobalPath::Inst().GetHardwareIniPath() );
	std::string Value;

	Value = HardwareIni.GetValueString( _T("IO Board"), _T("10"), _T("Entech X1") );
	Value = _T("10 - ") + Value;
	m_BoardModelCombo.AddString( Value.c_str() );

	Value = HardwareIni.GetValueString( _T("IO Board"), _T("20"), _T("Entech X2") );
	Value = _T("20 - ") + Value;
	m_BoardModelCombo.AddString( Value.c_str() );

	Value = HardwareIni.GetValueString( _T("IO Board"), _T("12"), _T("Entech X3") );
	Value = _T("12 - ") + Value;
	m_BoardModelCombo.AddString( Value.c_str() );

	// Default board is an IO board model 10
	m_BoardModel = 0;

	if(0 == m_csDigital.CompareNoCase(SVLPTIODLL) )
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
	SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputCount(numOutputs);
	
	for( DWORD dwChan = 0 ; dwChan < numOutputs; dwChan++)
	{
		SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue( dwChan, false );
	}
}

void CSVIOTESTDlg::OnButton2() 
{
	nSeq = 0; 

// TURN OFF ALL OUPUTS
	unsigned long numOutputs;
	SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputCount(numOutputs);
	for( DWORD dwChan = 0 ; dwChan < numOutputs ; dwChan++)
	{
		SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue( dwChan, true );
	}
}

void CSVIOTESTDlg::OnTimer( UINT_PTR nIDEvent )
{
	bool bValue = 0;
	CStatic *pCStatic;

	// Update Digital Input status Indicators
	unsigned long numInputs;
	SVIOConfigurationInterfaceClass::Instance().GetDigitalInputCount(numInputs);
	for (DWORD dwChan = 0 ; dwChan < numInputs ; dwChan++)
	{
		SVIOConfigurationInterfaceClass::Instance().GetDigitalInputValue(dwChan, bValue);
		switch (dwChan)
		{
			case 0:
				pCStatic = &m_input[1];
				break;
			case 1:
				pCStatic = &m_input[2];
				break;
			case 2:
				pCStatic = &m_input[3];
				break;
			case 3:
				pCStatic = &m_input[4];
				break;
			case 4:
				pCStatic = &m_input[5];
				break;
			case 5:
				pCStatic = &m_input[6];
				break;
			case 6:
				pCStatic = &m_input[7];
				break;
			case 7:
				pCStatic = &m_input[8];
				break;
			default:
				pCStatic = &m_input[1];
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
		if( S_OK == SVIOConfigurationInterfaceClass::Instance().GetParameterValue(SVFanState, &l_vValue) )
		{
			for (unsigned int triggerchannel = 0; triggerchannel < c_upperBoundForTriggerChannel; ++triggerchannel)
			{
				m_Fan[triggerchannel].SetIcon( AfxGetApp()->LoadIcon( ( l_vValue.lVal & (1 << triggerchannel)) ? IDI_ICON4 : IDI_ICON3 ));
			}
		}
		if( S_OK == SVIOConfigurationInterfaceClass::Instance().GetParameterValue(SVFanFreq, &l_vValue) )
		{
			for (unsigned int triggerchannel = 0; triggerchannel < c_upperBoundForTriggerChannel; ++triggerchannel)
			{
				m_FanFreq[triggerchannel] = l_vValue.lVal >> (8 * triggerchannel) & 0xff;
			}
			UpdateData(FALSE);
		}
	}

	// Sequence Outputs 0 - 7 and 8 - 15 
	unsigned long numOutputs;
	SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputCount(numOutputs);

	if (nSeq != 0)
	{
		int nCount = 0;
		if (nSpeed != 0)
		{
			nCount = 10;
		}
		else
		{
			nCount = 80;
		}

		static int nCounter = 0;
		if (nCounter >= nCount)
		{
			static int nSeqCount = 0;
			// Outputs 0 - 7  
			SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue((nSeqCount + 7) % 8 , true);
			SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue(nSeqCount, false);
			
			// Outputs 8 - 15 (actually numOutputs now)
			SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue((nSeqCount + 7) % 8 + 8, true);
			SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue(nSeqCount + (numOutputs - 8) , false);

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

	unsigned long numTriggers = 0;
	m_psvTriggers->GetCount(&numTriggers);

	for(unsigned long i = 0; i < numTriggers; ++i)
	{
		updateValues(i);
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
	SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputCount(numOutputs);
	for(DWORD dwChan = 0 ; dwChan < numOutputs ; dwChan++)
	{
		SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue(dwChan, true);
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
	SVIOConfigurationInterfaceClass::Instance().TestDigitalOutputs();
}

HRESULT CALLBACK SVCallback(const SvTh::TriggerParameters& rTriggerData)
{
	double timestamp{0.0};
	SvTh::IntVariantMap::const_iterator Iter(rTriggerData.m_Data.end());
	Iter = rTriggerData.m_Data.find(SvTh::TriggerDataEnum::TimeStamp);
	if (rTriggerData.m_Data.end() != Iter)
	{
		timestamp = Iter->second;
	}

	CSVIOTESTDlg* pDlg = reinterpret_cast<CSVIOTESTDlg*> (rTriggerData.m_pOwner);
	Iter = rTriggerData.m_Data.find(SvTh::TriggerDataEnum::TriggerChannel);
	if(nullptr != pDlg && rTriggerData.m_Data.end() != Iter)
	{
		unsigned long triggerChannel = Iter->second;
		SVIOTriggerDataStruct* pData = pDlg->getTriggerData(triggerChannel);

		if (nullptr != pData)
		{
			(pData->lTriggerCount)++;

			if ( 101 < pData->lTriggerCount )
			{
				pData->m_TotalTime -= ( pData->m_TimeStamp[ pData->ulNextIndex ] -
										   pData->m_TimeStamp[ pData->ulIndex ] ) ;
			}

			pData->m_TimeStamp[ pData->ulIndex ] = timestamp;

			if ( 1 < pData->lTriggerCount )
			{
				pData->m_LastTime = ( pData->m_TimeStamp[ pData->ulIndex ] -
																 pData->m_TimeStamp[ pData->ulLastIndex ] ) ;

				pData->m_TotalTime += pData->m_LastTime;

				if ( 2 < pData->lTriggerCount )
				{
					if ( pData->m_MaxTime < pData->m_LastTime )
					{
						pData->m_MaxTime = pData->m_LastTime;
					}

					if ( pData->m_LastTime < pData->m_MinTime )
					{
						pData->m_MinTime = pData->m_LastTime;
					}
				}
			}

			pData->ulLastIndex = pData->ulIndex;
			pData->ulIndex = pData->ulNextIndex;
			pData->ulNextIndex = ( pData->ulNextIndex + 1 ) % 100;
		}
	}

	return S_OK;
}



void SVIOTriggerDataStruct::OnTriggerStart()
{
	ulLastIndex = 0;
	ulIndex = 0;
	ulNextIndex = 1;
	lTriggerCount = 0;
	m_LastTime = 0;
	m_TotalTime = 0;
	m_MaxTime = SvTl::GetMinTimeStamp();
	m_MinTime = SvTl::GetMaxTimeStamp();
}

void CSVIOTESTDlg::StartTrigger(int triggerchannel) 
{
	m_TriggerData[triggerchannel].OnTriggerStart();

	m_TriggerCountWnd[triggerchannel].SetWindowText( "0" );
	m_TriggerDistance[triggerchannel].SetWindowText( "0" );
	m_TriggerAverage[triggerchannel].SetWindowText( "0" );
	m_TriggerMinimum[triggerchannel].SetWindowText( "0" );
	m_TriggerMaximum[triggerchannel].SetWindowText( "0" );

}


void CSVIOTESTDlg::OnStartTriggers() 
{
	CString csText("0");

	unsigned long numTriggers = 0;
	m_psvTriggers->GetCount(&numTriggers);

	SvTh::TriggerDispatcher dispatcher(SVCallback, SvTh::TriggerParameters{this});

	for(unsigned int triggerchannel = 1; triggerchannel < 5; triggerchannel++)
	{
		StartTrigger(triggerchannel);
		if (numTriggers >= triggerchannel)
		{
			m_psvTriggers->Register( triggerchannel, dispatcher );
		}
	}

	for(unsigned triggerchannel = 1; triggerchannel < 5; triggerchannel++)
	{
		if (numTriggers >= triggerchannel)
		{
			m_psvTriggers->Start( triggerchannel );
		}
	}

	m_bInterruptEnabled = true;
	_variant_t moduleReady = m_bInterruptEnabled;
	SVIOConfigurationInterfaceClass::Instance().SetParameterValue(SVModuleReady, &moduleReady.GetVARIANT());

	if (IsSoftwareTrigger())
	{
		GetDlgItem(IDC_TRIGGER_PARAM)->EnableWindow(false);
	}
}

void CSVIOTESTDlg::OnStopTriggers() 
{
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

	SvTh::TriggerDispatcher dispatcher(SVCallback, SvTh::TriggerParameters{this});

	if (numTriggers > 3)
	{
		m_psvTriggers->Unregister( 4, dispatcher );
	}

	if (numTriggers > 2)
	{
		m_psvTriggers->Unregister( 3, dispatcher );
	}

	if (numTriggers > 1)
	{
		m_psvTriggers->Unregister( 2, dispatcher );
	}

	if (numTriggers > 1)
	{
		m_psvTriggers->Unregister( 1, dispatcher );
	}
	m_bInterruptEnabled = false;
	_variant_t moduleReady = m_bInterruptEnabled;
	SVIOConfigurationInterfaceClass::Instance().SetParameterValue(SVModuleReady, &moduleReady.GetVARIANT());

	if (IsSoftwareTrigger())
	{
		GetDlgItem(IDC_TRIGGER_PARAM)->EnableWindow(true);
	}
}

DWORD WINAPI SVWorkerThreadFunc( LPVOID lpParam )
{
	CSVIOTESTDlg* l_pOwner = (CSVIOTESTDlg*) lpParam;

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
				if( S_OK != SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue( i, l_bState ) )
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

	if( !m_bThreadRunning )
	{
		if (m_lStaticChannel >= 0 && m_lStaticChannel < 16)
		{
			SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue(m_lStaticChannel, false);
		}

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
			SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue( m_lStaticChannel, false );
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
		unsigned long triggerchannel;
		unsigned long l_ulCount;
		VARIANT l_vValue;
		::VariantInit( &l_vValue);
		l_vValue.vt = VT_I4;

		m_psvTriggers->GetCount(&l_ulCount );
		for( unsigned long x = 0 ; x < l_ulCount ; x++ )
		{
			m_psvTriggers->GetHandle(&triggerchannel, x );
			m_psvTriggers->GetParameterValue( triggerchannel, 0, &l_vValue);

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
				m_psvTriggers->GetHandle(&triggerchannel, x );
			
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
				m_psvTriggers->SetParameterValue( triggerchannel, 0, &l_vValue);
			}
		}
	}
	else
	{
		SVTriggerSetupDlgClass l_dlg;
		unsigned long triggerchannel;
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
			m_psvTriggers->GetHandle(&triggerchannel, x );
			m_psvTriggers->GetParameterValue( triggerchannel, SVSignalEdge, &l_vValue);
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
				m_psvTriggers->GetHandle(&triggerchannel, x );
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
				m_psvTriggers->SetParameterValue( triggerchannel, SVSignalEdge, &l_vValue);
			}
		}
		m_lStrobeInverts = l_dlg.m_lStrobeInverts;
		m_lTrigInverts = l_dlg.m_lTrigInverts;
	}
}

void CSVIOTESTDlg::ShowFans( bool p_bShow )
{
	UINT cmd = (p_bShow) ? SW_SHOW : SW_HIDE;
	
	for (unsigned int triggerchannel = 0; triggerchannel < c_upperBoundForTriggerChannel; ++triggerchannel)
	{
		m_Fan[ triggerchannel ].ShowWindow( cmd );
		m_FanTxt[ triggerchannel ].ShowWindow( cmd );
	}
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
	if( S_OK != SVIOConfigurationInterfaceClass::Instance().SetParameterValue( SVBoardType, &l_vt ) )
	{
		AfxMessageBox(" Error Setting System Type");
	}
}

void CSVIOTESTDlg::updateValues(int triggerchannel)
{
	CString csText;

	if ( 1 < m_TriggerData[triggerchannel].lTriggerCount && 
		m_TriggerCount[triggerchannel] != m_TriggerData[triggerchannel].lTriggerCount )
	{
		m_TriggerCount[triggerchannel] = m_TriggerData[triggerchannel].lTriggerCount;

		long l_lCount = 100;

		if ( ( m_TriggerCount[triggerchannel] - 1 ) < l_lCount )
		{
			l_lCount = m_TriggerCount[triggerchannel] - 1;
		}

		long l_lLastValue = (long)( m_TriggerData[triggerchannel].m_LastTime * 1000 );

		long l_lAvgValue = (long)( ( m_TriggerData[triggerchannel].m_TotalTime * 1000 ) / l_lCount );

		long l_lMaxValue = (long)( m_TriggerData[triggerchannel].m_MaxTime * 1000 );

		long l_lMinValue = (long)( m_TriggerData[triggerchannel].m_MinTime * 1000 );

		csText.Format( "%ld", l_lMaxValue );
		m_TriggerMaximum[triggerchannel].SetWindowText( csText );

		csText.Format( "%ld", l_lMinValue );
		m_TriggerMinimum[triggerchannel].SetWindowText( csText );

		csText.Format( "%ld", m_TriggerCount[triggerchannel] );
		m_TriggerCountWnd[triggerchannel].SetWindowText( csText );

		csText.Format( "%ld", l_lLastValue );
		m_TriggerDistance[triggerchannel].SetWindowText( csText );

		csText.Format( "%ld", l_lAvgValue );
		m_TriggerAverage[triggerchannel].SetWindowText( csText );
	}

}


void CSVIOTESTDlg::OnBnClickedAcceptTriggers()
{
	if (m_bCurrentlyAcceptingTriggers)
	{
		OnStopTriggers();
		m_bCurrentlyAcceptingTriggers = false;
	}
	else
	{
		OnStartTriggers();
		m_bCurrentlyAcceptingTriggers = true;
	}
}
