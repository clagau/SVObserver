//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SubSystemTestMFCDlg
//* .File Name       : $Workfile:   SubSystemTestMFCDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 17:16:00  $
//******************************************************************************

#include "stdafx.h"
#include "SubSystemTestMFC.h"
#include "SubSystemTestMFCDlg.h"
#include "SVIOLibrary\SVIOConfigurationInterfaceClass.h"
#include "SVImageLibrary\SVDigitizerLoadLibraryClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
char* Month[13]={"Invalid","January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

void CSubSystemTestMFCDlg::CountResult(bool bPass)
{
	FILE* fh = NULL;
	fopen_s( &fh, "c:\\SVObserver\\bin\\CorecoFaults.txt","a");
	if( fh != NULL )
	{
		SYSTEMTIME stime;
		memset( &stime, 0, sizeof( SYSTEMTIME));
		::GetSystemTime( &stime );
		
		fprintf( fh, "%2d of %s %04d %02d:%02d:%02d %s\n",stime.wDay, Month[stime.wMonth], stime.wYear,
			stime.wHour, stime.wMinute, stime.wSecond,
			bPass ? "Passed" : "Failed");
		fflush( fh );
		fclose( fh );
	}
}


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

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


// CSubSystemTestMFCDlg dialog




CSubSystemTestMFCDlg::CSubSystemTestMFCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSubSystemTestMFCDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSubSystemTestMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
}

BEGIN_MESSAGE_MAP(CSubSystemTestMFCDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CSubSystemTestMFCDlg message handlers

BOOL CSubSystemTestMFCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	CheckDigitizer();

	//OnOK();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSubSystemTestMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
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

void CSubSystemTestMFCDlg::OnPaint()
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
HCURSOR CSubSystemTestMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSubSystemTestMFCDlg::CheckDigitizer()
{
	bool l_bDigitizerOK = false;
	SVDigitizerLoadLibraryClass l_svDLLDigitizers;
	SVIODigitalLoadLibraryClass l_svIOConfig;

	while( true )
	{
		if( l_svDLLDigitizers.Open( _T("SVCI.dll") ) != S_OK )
		{
			m_ListBox.AddString(_T("Error Opening SVCI.Dll"));
			break;
		}

		unsigned long l_ulSize = 0;

		if( l_svDLLDigitizers.GetCount( &l_ulSize ) != S_OK )
		{
			CString l_strInfo;
			l_strInfo.Format( _T("Error Connecting to digitizer Subsystem %d found"), l_ulSize);
			m_ListBox.AddString( l_strInfo );
			break;
		}

		if( l_ulSize == 0 )
		{
			m_ListBox.AddString( _T("Could not find Digitizer"));
		}
		else
		{
			l_bDigitizerOK = true;
		}
		break;
	}

	CountResult(l_bDigitizerOK);

	//////////////////////////////// Digital Section
	if ( l_svIOConfig.Open( _T("c:\\SVObserver\\bin\\SVTVicLPT.dll") ) != S_OK )
	{
		m_ListBox.AddString( _T("Error 0 Opening The SVTVicLPT dll"));
	}
	else
	{	
		while( true )
		{
			
			m_ListBox.AddString( _T("Sending Data to IO"));
			if( l_svIOConfig.SetOutputValue( 0, l_bDigitizerOK ) != S_OK) // Output Bit.
			{
				m_ListBox.AddString( _T("Error 1 Opening The SVTVicLPT dll"));
				break;
			}
			Sleep(150);
			if( l_svIOConfig.SetOutputValue( 1, 0 )) // Trigger Bit on.
			{
				m_ListBox.AddString( _T("Error 2 Opening The SVTVicLPT dll"));
				break;
			}
			Sleep(150);
			if( l_svIOConfig.SetOutputValue( 1, 1 )) // Trigger Bit off.
			{
				m_ListBox.AddString(_T("Error 3 Opening The SVTVicLPT dll"));
				break;
			}
			Sleep(150);
			if( l_svIOConfig.SetOutputValue( 0, 1 )) // Pass / Fail bit off.
			{
				m_ListBox.AddString( _T("Error 4 Opening The SVTVicLPT dll"));
				break;
			}
			Sleep(15000);
			if( l_svIOConfig.SetOutputValue( 2, 0 )) // Energize Output 3 to restart system.
			{
				m_ListBox.AddString( _T("Error 5 Opening The SVTVicLPT dll"));
				break;
			}
			Sleep(1000);
			break;
		}
	}
	UpdateData(FALSE);

	l_svDLLDigitizers.Close();
	l_svIOConfig.Close();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SubSystemTestMFC\SubSystemTestMFCDlg.cpp_v  $
 * 
 *    Rev 1.0   18 Apr 2013 17:16:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/