// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVImageTestDlg.cpp
// * .File Name       : $Workfile:   SVImageTestDlg.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:10:26  $
// ******************************************************************************

#include "stdafx.h"
#include "SVImageTest.h"
#include "SVImageTestDlg.h"
#include "SVTestAcquisitionClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
// CSVImageTestDlg dialog

void CSVImageTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSVImageTestDlg)
	DDX_Control(pDX, IDC_LIST1, m_clbCameras);
	DDX_Text(pDX, IDC_MODELNOEDIT, m_modelNo);
	DDV_MaxChars(pDX, m_modelNo, 12);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSVImageTestDlg, CDialog)
	//{{AFX_MSG_MAP(CSVImageTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	ON_BN_CLICKED(IDC_UPDATEMODELNO, OnUpdateModelNumber)
	ON_BN_CLICKED(IDC_START_ALL_CAMERAS, OnStartAllCameras)
	ON_BN_CLICKED(IDC_STOP_ALL_CAMERAS, OnStopAllCameras)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSVImageTestDlg::OnInitDialog()
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

	for( int i = 0; i < 4; ++i )
	{
		CString l_WindowText;
		l_WindowText.Format( _T( "Camera %d" ), i );
		m_Camera[ i ].SetTabText( l_WindowText );
		m_CameraSheet.AddPage( m_Camera + i );
	}

	m_CameraSheet.Create(this, WS_CHILD | WS_VISIBLE, 0);
	m_CameraSheet.ModifyStyleEx (0, WS_EX_CONTROLPARENT);
	m_CameraSheet.ModifyStyle( 0, WS_TABSTOP );

	CRect rcSheet;
	GetDlgItem( IDC_CAMERA_TAB_AREA )->GetWindowRect( &rcSheet );
	ScreenToClient( &rcSheet );
	m_CameraSheet.SetWindowPos( NULL, rcSheet.left-7, rcSheet.top-7, 0, 0, 
			SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE );

	for( int j = 0; j < 4; ++j )
	{
		m_CameraSheet.SetActivePage( 3 - j );
	}

	CSVImageTestApp* pApp = (CSVImageTestApp *)AfxGetApp();
	m_modelNo = pApp->m_iniLoader.m_csModelNumber;

	UpdateData( FALSE );
	
	OnRefresh();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSVImageTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSVImageTestDlg::OnPaint() 
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
HCURSOR CSVImageTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSVImageTestDlg::OnRefresh() 
{
	ResetCameraList();

	if ( m_pSubsystem != NULL )
	{
		CSVImageTestApp* pApp = (CSVImageTestApp *)AfxGetApp();	

		unsigned long l_ulSize = 0;

		m_pSubsystem->m_svDigitizers.GetCount( &l_ulSize );

		for ( unsigned long i = 0; i < 4; i++ )
		{
			if( m_Camera[ i ].m_pAcquisition != NULL )
			{
				delete m_Camera[ i ].m_pAcquisition;

				m_Camera[ i ].m_pAcquisition = NULL;
				m_Camera[ i ].m_lSelectedCamera = -1;
			}

			if( i < l_ulSize )
			{
				m_Camera[ i ].m_pAcquisition = m_pSubsystem->GetAcquisitionDevice( i );

				if( m_Camera[ i ].m_pAcquisition != NULL )
				{
					unsigned long l_ulHandle = 0;

					BSTR l_bstrName = NULL;
					CString l_csName;

					m_Camera[ i ].m_pAcquisition->m_rSubsystem.m_svDigitizers.GetHandle( &l_ulHandle, i );
					m_Camera[ i ].m_pAcquisition->m_rSubsystem.m_svDigitizers.GetName( l_ulHandle, &l_bstrName );

					l_csName = l_bstrName;

					::SysFreeString( l_bstrName );

					m_clbCameras.AddString( l_csName );

					m_Camera[ i ].SelectCamera( i );
					m_Camera[ i ].EnableWindow( true );
				}
			}
			else
			{
				m_Camera[ i ].EnableWindow( false );
			}
		}


		UpdateData(FALSE);
	}
}

void CSVImageTestDlg::OnStartAllCameras() 
{
	StopAllCameras();

	OnRefresh();

	StartAllCameras();
}

void CSVImageTestDlg::OnStopAllCameras() 
{
	StopAllCameras();
}

void CSVImageTestDlg::OnUpdateModelNumber() 
{
	CSVImageTestApp* pApp = (CSVImageTestApp *)AfxGetApp();	
	CString l_modelNumber = m_modelNo;
	UpdateData();
	HRESULT hr = pApp->m_iniLoader.DecodeModelNumber(m_modelNo);
	if (hr == S_OK)
	{
		StopAllCameras();

		ResetCameraList();
		
		for ( unsigned long i = 0; i < 4; i++ )
		{
			if( m_Camera[ i ].m_pAcquisition != NULL )
			{
				delete m_Camera[ i ].m_pAcquisition;

				m_Camera[ i ].m_pAcquisition = NULL;
				m_Camera[ i ].m_lSelectedCamera = -1;
			}
		}

		// update settings from hardware.ini
		pApp->m_iniLoader.LoadHardwareIni(pApp->m_hardwareIniFile, pApp->m_iniLoader.m_csOptions);
		pApp->UnLoadDigitizer();
		m_pSubsystem = pApp->LoadDigitizer();

		OnRefresh();
	}
	else
	{
		AfxMessageBox("Invalid Model Number", MB_OK);
		m_modelNo = l_modelNumber;
	}
}

void CSVImageTestDlg::OnCancel() 
{
	for ( unsigned long i = 0; i < 4; i++ )
	{
		if( m_Camera[ i ].m_pAcquisition != NULL )
		{
			delete m_Camera[ i ].m_pAcquisition;

			m_Camera[ i ].m_pAcquisition = NULL;
			m_Camera[ i ].m_lSelectedCamera = -1;
		}
	}

	CDialog::OnCancel();
}

CSVImageTestDlg::CSVImageTestDlg(CWnd* pParent /*=NULL*/)
: CDialog(CSVImageTestDlg::IDD, pParent),
	m_pSubsystem( NULL )
{
	//{{AFX_DATA_INIT(CSVImageTestDlg)
	m_modelNo = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}

CSVImageTestDlg::~CSVImageTestDlg()
{
}

void CSVImageTestDlg::ResetCameraList()
{
	m_clbCameras.ResetContent();
}

void CSVImageTestDlg::StartAllCameras()
{
	for ( unsigned long i = 0; i < 4; i++ )
	{
		if( m_Camera[ i ].m_pAcquisition != NULL && ! m_Camera[ i ].m_bStarted )
		{
			m_Camera[ i ].StartAcquire();
		}
	}
}

void CSVImageTestDlg::StopAllCameras()
{
	for ( unsigned long i = 0; i < 4; i++ )
	{
		if( m_Camera[ i ].m_pAcquisition != NULL && m_Camera[ i ].m_bStarted )
		{
			m_Camera[ i ].StopAcquire();
		}
	}
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageTest\SVImageTestDlg.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 11:10:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3.1.0   29 Jul 2009 15:53:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   05 Mar 2008 09:41:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  622
 * SCR Title:  Update SVImageTest Application to Acquire on Multiple Cameras Simultaneously
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Added functionality for processing multiple camera acquisitions at the same time.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2.1.0   13 Jul 2009 15:40:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update image test program to use the new acquisition methodology for transferring the buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Nov 2006 15:11:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Revision header block
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
