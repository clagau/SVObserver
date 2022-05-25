// ******************************************************************************
// * COPYRIGHT (c) 2006 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVImageTestDlg.cpp
// * .File Name       : $Workfile:   SVImageTestDlg.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:10:26  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVImageTestApp.h"
#include "SVImageTestDlg.h"
#include "SVTestAcquisitionClass.h"
#include "SVTestAcquisitionSubsystem.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr long cGigeParameterIPAddress = 104;


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

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

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

void CSVImageTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_clbCameras);
	DDX_Text(pDX, IDC_MODELNOEDIT, m_modelNo);
	DDV_MaxChars(pDX, m_modelNo, 12);
}

BEGIN_MESSAGE_MAP(CSVImageTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	ON_BN_CLICKED(IDC_START_ALL_CAMERAS, OnStartAllCameras)
	ON_BN_CLICKED(IDC_STOP_ALL_CAMERAS, OnStopAllCameras)
END_MESSAGE_MAP()

BOOL CSVImageTestDlg::OnInitDialog()
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

	int cameraIndex {0};
	for(auto& rCamera : m_Cameras)
	{
		++cameraIndex;
		CString l_WindowText;
		l_WindowText.Format( _T( "Camera %d" ), cameraIndex);
		rCamera.SetTabText( l_WindowText );
		m_CameraSheet.AddPage(&rCamera);
	}

	m_CameraSheet.Create(this, WS_CHILD | WS_VISIBLE, 0);
	m_CameraSheet.ModifyStyleEx (0, WS_EX_CONTROLPARENT);
	m_CameraSheet.ModifyStyle( 0, WS_TABSTOP );

	CRect rcSheet;
	GetDlgItem( IDC_CAMERA_TAB_AREA )->GetWindowRect( &rcSheet );
	ScreenToClient( &rcSheet );
	m_CameraSheet.SetWindowPos( nullptr, rcSheet.left-7, rcSheet.top-7, 0, 0, 
			SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE );

	for( int j = 0; j < cCameraCount; ++j )
	{
		m_CameraSheet.SetActivePage( 3 - j );
	}

	m_pApp = dynamic_cast<SVImageTestApp*> (AfxGetApp());
	if (nullptr != m_pApp)
	{
		m_modelNo = m_pApp->m_iniLoader.ModelNumberString();
	}

	UpdateData( FALSE );

	if (nullptr != h_plcIODll)
	{
		SVCameraPage::m_pCreate = (SVCreatePtr)::GetProcAddress(h_plcIODll, "SVCreate");
		SVCameraPage::m_pDestroy = (SVDestroyPtr)::GetProcAddress(h_plcIODll, "SVDestroy");
		SVCameraPage::m_pStart = (SVTriggerStartPtr)::GetProcAddress(h_plcIODll, "SVTriggerStart");
		SVCameraPage::m_pStop = (SVTriggerStopPtr)::GetProcAddress(h_plcIODll, "SVTriggerStop");
	}

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

	if ( nullptr != m_pSubsystem )
	{
		ClearCameras();
		unsigned long count = m_pSubsystem->m_svDigitizers.GetCount();

		for (unsigned long i = 0; i < cCameraCount; i++)
		{
			if (i < count)
			{
				SVTestAcquisitionClass* pAcquisition = m_pSubsystem->GetAcquisitionDevice(i);

				if( nullptr != pAcquisition )
				{
					unsigned long digitizerHandle = pAcquisition->m_rSubsystem.m_svDigitizers.GetHandle(i);
					_variant_t name = pAcquisition->m_rSubsystem.m_svDigitizers.GetName(digitizerHandle);
					CString cameraName = name.bstrVal;
					m_clbCameras.AddString(cameraName);

					if (nullptr != m_pApp)
					{
						_variant_t value = pAcquisition->m_rSubsystem.m_svDigitizers.ParameterGetValue(digitizerHandle, cGigeParameterIPAddress);
						std::string ipAddress {_bstr_t(value.bstrVal)};
						for (int j = 0; j < 4; ++j)
						{
							if (m_pApp->m_CameraIpAddress[j] == ipAddress)
							{
								m_Cameras[j].m_pAcquisition = pAcquisition;
								m_Cameras[j].SelectCamera(i);
								m_Cameras[j].EnableWindow(true);
							}
						}
					}
				}
			}
			else
			{
				m_Cameras[i].EnableWindow( false );
			}
		}


		UpdateData(FALSE);
	}
}

void CSVImageTestDlg::OnStartAllCameras() 
{
	StopAllCameras();

	StartAllCameras();
}

void CSVImageTestDlg::OnStopAllCameras() 
{
	StopAllCameras();
}

void CSVImageTestDlg::OnCancel() 
{
	ClearCameras();

	CDialog::OnCancel();
}

CSVImageTestDlg::CSVImageTestDlg(CWnd* pParent /*=nullptr*/)
: CDialog(CSVImageTestDlg::IDD, pParent),
	m_pSubsystem( nullptr )
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

void CSVImageTestDlg::ClearCameras()
{
	for (auto& rCamera : m_Cameras)
	{
		if (nullptr != rCamera.m_pAcquisition)
		{
			delete rCamera.m_pAcquisition;

			rCamera.m_pAcquisition = nullptr;
			rCamera.m_lSelectedCamera = -1;
		}
	}
}

void CSVImageTestDlg::StartAllCameras()
{
	for ( unsigned long i = 0; i < cCameraCount; i++ )
	{
		if( nullptr != m_Cameras[i].m_pAcquisition && ! m_Cameras[i].m_bStarted )
		{
			m_Cameras[i].StartAcquire();
		}
	}
}

void CSVImageTestDlg::StopAllCameras()
{
	for ( unsigned long i = 0; i < cCameraCount; i++ )
	{
		if( nullptr != m_Cameras[i].m_pAcquisition && m_Cameras[i].m_bStarted )
		{
			m_Cameras[i].StopAcquire();
		}
	}
}


