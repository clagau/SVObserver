//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : InitializeIOSubsystemDlg.cpp
//* .File Name       : $Workfile:   InitializeIOSubsystemDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   18 Sep 2014 11:40:34  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "InitializeIOSubsystemDlg.h"
#include "SVIOLibrary\SVIOParameterEnum.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

long Shutdown();

#define ITTERATIONS 60
#define TIMEOUT_QUIT 180

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// InitializeIOSubsystemDlg dialog

InitializeIOSubsystemDlg::InitializeIOSubsystemDlg(CWnd* pParent /*=NULL*/)
	: CDialog(InitializeIOSubsystemDlg::IDD, pParent)
	, m_bLptIOInitialized(false)
	, m_lGoodCounter(0)
	, m_lVer(0)
	, m_TimeElapse(0)
	, m_FailedCounts(0)
	, m_bShutdown(false)
	, m_bToggle(false)
	, m_bOldBehavior(false)
	, m_lNewBehaviorCounter(0)
	, m_lProgressCounter(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void InitializeIOSubsystemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
	DDX_Text(pDX, IDC_SUCCESS_COUNT, m_lGoodCounter);
	DDX_Text(pDX, IDC_VERSION_EDIT, m_strVer);
	DDX_Text(pDX, IDC_TIME_EDIT, m_TimeElapse);
	DDX_Text(pDX, IDC_FAILED_COUNT, m_FailedCounts);
}

BEGIN_MESSAGE_MAP(InitializeIOSubsystemDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// InitializeIOSubsystemDlg message handlers

BOOL InitializeIOSubsystemDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	assert((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
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

	GetDlgItem( IDC_VERSION_EDIT)->ShowWindow( SW_HIDE );
	GetDlgItem( IDC_SUCCESS_COUNT_TEXT)->ShowWindow( SW_HIDE );
	GetDlgItem( IDC_SUCCESS_COUNT)->ShowWindow( SW_HIDE );
	GetDlgItem( IDC_ELAPSE_TEXT)->ShowWindow( SW_HIDE );
	GetDlgItem( IDC_TIME_EDIT)->ShowWindow( SW_HIDE );
	GetDlgItem( IDC_RETRY_TEXT)->ShowWindow( SW_HIDE );
	GetDlgItem( IDC_FAILED_COUNT)->ShowWindow( SW_HIDE );

	m_lTimer = SetTimer(1, 200, nullptr);
	m_Progress.SetRange(0, ITTERATIONS );
	SetWindowText( _T("Initialize IO Sub-System version 3.10") );
	time( &m_StartTime);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void InitializeIOSubsystemDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void InitializeIOSubsystemDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR InitializeIOSubsystemDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void InitializeIOSubsystemDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	if( m_bLptIOInitialized )
	{
		CDialog::OnTimer(nIDEvent);
		_variant_t value = m_IOSystem.GetParameterValue(SVBoardVersion);
		bool rabbitBoardPresent{ false };
		if(VT_EMPTY != value.vt)
		{
			rabbitBoardPresent = true;
			m_strVer = value.bstrVal;
			//! Note Version with 255.255 means the Rabbit board has not been found
			if( -1 != m_strVer.Find(_T("255.255")) )
			{
				rabbitBoardPresent = false;
			}
			else if( m_FanSpeed.IsEmpty() )
			{
				value = m_IOSystem.GetParameterValue(SVFanFreq);
				if (VT_EMPTY != value.vt)
				{
					long FanSpeed{ value.lVal };
					m_FanSpeed.Format(_T("Fan1: %ld, Fan2: %ld, Fan3: %ld, Fan4: %ld"), FanSpeed & 0xff, (FanSpeed >> 8) & 0xff, (FanSpeed >> 16) & 0xff, (FanSpeed >> 24) & 0xff);
					m_strLog.push_back(m_FanSpeed);
				}
				else
				{
					rabbitBoardPresent = false;
				}
			}
		}
		if(rabbitBoardPresent)
		{
			m_lGoodCounter++;
			if( m_bToggle )
			{
				bool State = (0 == (m_lGoodCounter % 2));
				m_IOSystem.SetOutputValue( 0 , State );
			}
		}
		else
		{
			m_FailedCounts++;
			m_lGoodCounter = 0;
		}
		time_t l_tmpTime;
		time( &l_tmpTime);

		m_lNewBehaviorCounter++ ;
		m_lNewBehaviorCounter %= ITTERATIONS;

		m_TimeElapse = static_cast<long>( l_tmpTime - m_StartTime ) ;
		if( true == m_bOldBehavior )
		{
			m_lProgressCounter = m_lGoodCounter;
		}
		else
		{
			m_lProgressCounter = m_lNewBehaviorCounter;
		}
		m_Progress.SetPos( m_lProgressCounter );
		
		if( m_lGoodCounter >= ITTERATIONS )
		{
			m_IOSystem.SetOutputPortValue( 0, 0xffff );
			KillTimer( m_lTimer );
			DestroyWindow();
			::exit(0);
		}
		if( m_TimeElapse > TIMEOUT_QUIT )
		{
			KillTimer( m_lTimer );
			int iRet = MessageBox( _T("Unable to initialize IO System Do you want to Re-boot"), NULL, MB_YESNO );
			if( IDYES == iRet )
			{
				Shutdown();
			}
			else
			{
				DestroyWindow();
			}
		}
	}
	else
	{
		HRESULT l_hr = m_IOSystem.Open("SVLptIO.dll");
		if( S_OK == l_hr )
		{
			m_bLptIOInitialized = true;
		}
		else
		{
			m_FailedCounts++;
			CString l_strTmp;
			l_strTmp.Format( "Init Error %ld\n", l_hr );
			m_strLog.push_back( l_strTmp );

			if( m_TimeElapse > TIMEOUT_QUIT )
			{
				KillTimer( m_lTimer );
				int iRet = MessageBox( _T("Unable to initialize IO System Do you want to Re-boot"), NULL, MB_YESNO );
				if( IDYES == iRet )
				{
					Shutdown();
				}
				else
				{
					DestroyWindow();
				}
			}
		}
	}
	if( m_bOldBehavior )
	{
		UpdateData(FALSE);
	}
}

InitializeIOSubsystemDlg::~InitializeIOSubsystemDlg()
{
}

void InitializeIOSubsystemDlg::OnDestroy()
{
	FILE* fh = nullptr;
	fopen_s(&fh, "c:\\SVObserver\\InitializeIOSubsystem Log.txt", "a" );

	if( nullptr != fh )
	{
		time_t Now;
		time(&Now);
		CString DateTime;
		struct tm NowInfo;
		localtime_s( &NowInfo, &Now );
		DateTime.Format(_T("Started InitializeIOSubsystem %02d.%02d.%04d %02d:%02d:%02d"), NowInfo.tm_mday, NowInfo.tm_mon + 1, NowInfo.tm_year + 1900, NowInfo.tm_hour, NowInfo.tm_min, NowInfo.tm_sec );
		fprintf( fh, "\n%s\n%s\nStartup Time %lld Seconds, Errors %ld\n", DateTime.GetString(), m_strVer.GetString(), m_TimeElapse, m_FailedCounts );
		for( size_t i = 0 ; i < m_strLog.size() ; i++ )
		{
			fprintf( fh, "%s\n", m_strLog[i].GetString() );
		}
		fclose( fh );
	}

	KillTimer( m_lTimer );

	m_IOSystem.Close();

	if( m_bShutdown )
	{
		Shutdown();
	}
	CDialog::OnDestroy();
}

long Shutdown()
{
	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp; 
 
	// Get a token for this process. 
 
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
	{
		printf("OpenProcessToken"); 
		return 0;
	}
 
	// Get the LUID for the shutdown privilege. 
 
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, 
			&tkp.Privileges[0].Luid); 
 
	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
 
	// Get the shutdown privilege for this process. 
 
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)nullptr, 0); 
 
	// Cannot test the return value of AdjustTokenPrivileges. 
 
	if (GetLastError() != ERROR_SUCCESS) 
	{
		printf("AdjustTokenPrivileges"); 
		return 0;
	}

	ExitWindowsEx(	EWX_REBOOT|EWX_FORCE, SHTDN_REASON_MINOR_MAINTENANCE | SHTDN_REASON_FLAG_PLANNED);

	return 0;
}

void InitializeIOSubsystemDlg::OnCancel()
{
	return;
}

void InitializeIOSubsystemDlg::OnOK()
{
	return;
}

void InitializeIOSubsystemDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_bShutdown = false;
	CDialog::OnLButtonDown(nFlags, point);
}

void InitializeIOSubsystemDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL InitializeIOSubsystemDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if( WM_KEYDOWN == pMsg->message )
	{
		if( VK_F8 == pMsg->wParam )
		{
			if( m_bOldBehavior )
			{
				m_bOldBehavior = false;
				GetDlgItem( IDC_VERSION_EDIT)->ShowWindow( SW_HIDE );
				GetDlgItem( IDC_SUCCESS_COUNT_TEXT)->ShowWindow( SW_HIDE );
				GetDlgItem( IDC_SUCCESS_COUNT)->ShowWindow( SW_HIDE );
				GetDlgItem( IDC_ELAPSE_TEXT)->ShowWindow( SW_HIDE );
				GetDlgItem( IDC_TIME_EDIT)->ShowWindow( SW_HIDE );
				GetDlgItem( IDC_RETRY_TEXT)->ShowWindow( SW_HIDE );
				GetDlgItem( IDC_FAILED_COUNT)->ShowWindow( SW_HIDE );
			}
			else
			{
				m_bOldBehavior = true;
				GetDlgItem( IDC_VERSION_EDIT)->ShowWindow( SW_SHOW );
				GetDlgItem( IDC_SUCCESS_COUNT_TEXT)->ShowWindow( SW_SHOW );
				GetDlgItem( IDC_SUCCESS_COUNT)->ShowWindow( SW_SHOW );
				GetDlgItem( IDC_ELAPSE_TEXT)->ShowWindow( SW_SHOW );
				GetDlgItem( IDC_TIME_EDIT)->ShowWindow( SW_SHOW );
				GetDlgItem( IDC_RETRY_TEXT)->ShowWindow( SW_SHOW );
				GetDlgItem( IDC_FAILED_COUNT)->ShowWindow( SW_SHOW );
			}
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}
