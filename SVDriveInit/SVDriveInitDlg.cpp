//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDriveInitDlg
//* .File Name       : $Workfile:   SVDriveInitDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   16 May 2014 08:53:52  $
//******************************************************************************

#include "stdafx.h"

#include "resource.h"
#include "SVDriveInitDlg.h"

#include "utilities.h"
#include "cpassdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const _TCHAR g_tszSVOUpdateFirmware[] = _T( "Software\\SVR Gesellschaft für Bildverarbeitung mbH\\SVObserver\\Utilities\\Update Firmware" );
static const _TCHAR g_tszSVOSettings[] = _T( "Software\\SVR Gesellschaft für Bildverarbeitung mbH\\SVObserver\\Settings" );
static const _TCHAR productIdLocation[] = _T( "Software\\Microsoft\\Windows NT\\CurrentVersion" );
static const _TCHAR oemSection[] = _T( "OEMSpecific" );
static const _TCHAR generalSection[] = _T( "general" );
static const _TCHAR UserDataSection[] = _T( "UserData" );
static const _TCHAR g_tsz1394Section[] = _T( "1394" );

static const _TCHAR g_tszWindowsRunOnce[] = _T( "Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce" );

static _TCHAR oeminfoFileName[_MAX_PATH];
static _TCHAR sysprepPath[_MAX_PATH];
static _TCHAR sysprepEditPath[_MAX_PATH];
static _TCHAR hardwareFileName[_MAX_PATH];
static _TCHAR sviminfoFileName[_MAX_PATH];
static _TCHAR windowsDriverPath[_MAX_PATH];

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
// SVDriveInitDlg dialog

SVDriveInitDlg::SVDriveInitDlg(CWnd* pParent /*=NULL*/)
: CDialog(SVDriveInitDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(SVDriveInitDlg)
    //}}AFX_DATA_INIT
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void SVDriveInitDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(SVDriveInitDlg)
    DDX_Control(pDX, IDC_COLOR_CHECK, m_ColorCheck);
	DDX_Control(pDX, IDC_SINGLECAMERA_CHECK, m_SingleCamera);
    DDX_Control(pDX, ID_MODEL_NUMBER, m_model_number);
    DDX_Control(pDX, IDC_TYPE, m_type);
    DDX_Control(pDX, ID_CDKEY5, m_cdkey5);
    DDX_Control(pDX, ID_CDKEY4, m_cdkey4);
    DDX_Control(pDX, ID_CDKEY3, m_cdkey3);
    DDX_Control(pDX, ID_CDKEY2, m_cdkey2);
    DDX_Control(pDX, ID_CDKEY1, m_cdkey1);
    DDX_Control(pDX, ID_DATE, m_date);
    DDX_Control(pDX, ID_SERVICED_BY, m_serviced_by);
    DDX_Control(pDX, ID_SERIAL_NUMBER, m_serial_number);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVDriveInitDlg, CDialog)
//{{AFX_MSG_MAP(SVDriveInitDlg)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_EN_CHANGE(ID_CDKEY1, OnChangeCdkey1)
ON_EN_CHANGE(ID_CDKEY2, OnChangeCdkey2)
ON_EN_CHANGE(ID_CDKEY3, OnChangeCdkey3)
ON_EN_CHANGE(ID_CDKEY4, OnChangeCdkey4)
ON_EN_CHANGE(ID_CDKEY5, OnChangeCdkey5)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVDriveInitDlg message handlers

BOOL SVDriveInitDlg::OnInitDialog()
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
    
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon
    
#ifndef _DEBUG
    CCPassDlg dlg;
   	BOOL blValidPassword = FALSE;
    
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK)
    {
    }
    else if (nResponse == IDCANCEL)
    {
        exit(1);
    }
#endif
    
    // Set Limits on Fields
    m_cdkey1.SetLimitText( PRODUCTID_PART_SIZE );
    m_cdkey2.SetLimitText( PRODUCTID_PART_SIZE );
    m_cdkey3.SetLimitText( PRODUCTID_PART_SIZE );
    m_cdkey4.SetLimitText( PRODUCTID_PART_SIZE );
    m_cdkey5.SetLimitText( PRODUCTID_PART_SIZE );
    
    
    m_serial_number.SetLimitText( SERIALNUMBER_SIZE );
    m_date.SetLimitText( 8 );
    m_serviced_by.SetLimitText( OEMINFO_SIZE );
    
    // Get the Windows Directory
    ::GetSystemDirectory(oeminfoFileName, sizeof( oeminfoFileName ) );
    strcat(oeminfoFileName, _T( "\\oeminfo.ini" ) );
    
    ::GetWindowsDirectory(windowsDriverPath, sizeof( windowsDriverPath ) );
    strcat(windowsDriverPath, _T( "\\System32\\drivers" ) );

    //set the path for the sysprep.inf edit directory
    strcpy(sysprepEditPath,_T("c:\\SVObserver\\sysprep\\" ) );
    
    //set the path for the hardware configuration file
    strcpy(hardwareFileName,_T("c:\\SVObserver\\bin\\hardware.ini" ) );

    //set the path for the svim configuration file
    strcpy(sviminfoFileName,_T("c:\\SVObserver\\bin\\svim.ini" ) );

    //get the file attributes
    DWORD dwAttr = GetFileAttributes(oeminfoFileName);
    
    //clear the read only flag
    dwAttr = dwAttr & ~FILE_ATTRIBUTE_READONLY;
    
    //set the file to read only
    SetFileAttributes(oeminfoFileName, dwAttr);
    
    GetSysPrepInfo();
    
    // Get Info from OEMINFO.ini
    GetOEMInfo();

    GetRegistryInfo();
    
    return TRUE;  // return TRUE  unless you set the focus to a control
}

void SVDriveInitDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void SVDriveInitDlg::OnPaint() 
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

HCURSOR SVDriveInitDlg::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}

void SVDriveInitDlg::OnOK() 
{
    bool rc;
    CString title;
    
    GetWindowText( title );
    
		rc = UpdateSysPrepInfo();
    if( !rc )
    {
        MessageBox( "Update System Prep Info Failed", title, MB_ICONINFORMATION );
        return;
    }
    
    rc = UpdateOEMInfo();
    if( !rc )
    {
        MessageBox( "Update OEM Info Failed", title, MB_ICONINFORMATION );
        return;
    }
    
    rc = UpdateSVIMInfo();
    if( !rc )
    {
        MessageBox( "Update SVIM Info Failed", title, MB_ICONINFORMATION );
        return;
    }
    
    rc = UpdateRegistryInfo();
    if( !rc )
    {
        MessageBox( "Update Registry Info Failed", title, MB_ICONINFORMATION );
        return;
    }
    
		//get the file attributes
    DWORD dwAttr = GetFileAttributes(oeminfoFileName);
    
    //set the read only flag
    dwAttr = dwAttr | FILE_ATTRIBUTE_READONLY;
    

		if( ! m_CDKey.UpdateProductId() )
		{
      DisplayLastSystemError();
      MessageBox( "Create Process Failed", title, MB_ICONINFORMATION );
      rc = false;
      return;
    }

	//set the file to read only
    SetFileAttributes(oeminfoFileName, dwAttr);

    if(rc)OnCancel();
}

////////////////////////////////////////////////////////////////////////////////////////
// Get the Windows NT productID (Certificate of Authenticity) from the registry
////////////////////////////////////////////////////////////////////////////////////////
bool SVDriveInitDlg::GetProductId()
{
    bool rc = true;
    // update the productID
    HKEY regKey;
    LONG retCode = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
        productIdLocation,
        0,
        KEY_QUERY_VALUE,
        &regKey );
    
    if( retCode == ERROR_SUCCESS )
    {
        _TCHAR oemProductID[REGISTRYPRODUCTID_SIZE+1];
        
        // Read the ProductId
        DWORD type;
        DWORD len = sizeof( oemProductID ) ;
        retCode = RegQueryValueEx( regKey, _T( "ProductId" ), 0, &type, (BYTE *)oemProductID, &len );
        
        if( retCode != ERROR_SUCCESS)
        {
            AfxMessageBox( IDS_GET_PRODUCTID_FAILED );
            rc = false;
        }
        RegFlushKey ( regKey );

        RegCloseKey( regKey );
    }
    return rc;
}

////////////////////////////////////////////////////////////////////////////////////////
// Get the SVObserver Registry Info
////////////////////////////////////////////////////////////////////////////////////////
bool SVDriveInitDlg::GetRegistryInfo()
{
    bool l_bOk = true;
    
    DWORD l_dwState = 0;
    
    HKEY l_hKey = NULL;
    
    int l_iErrorStatus = RegOpenKeyEx( HKEY_CURRENT_USER, g_tszSVOSettings, 0, KEY_QUERY_VALUE,	&l_hKey );
    
    if ( l_iErrorStatus == ERROR_SUCCESS )
    {
        // Read the ProductId
        DWORD l_dwType;
        DWORD l_dwSize = sizeof l_dwState;
        
        l_iErrorStatus = RegQueryValueEx( l_hKey, _T( "Color SVIM Mode Active" ), NULL, &l_dwType, (unsigned char *)&l_dwState, &l_dwSize );
        
        if( l_iErrorStatus != ERROR_SUCCESS)
        {
            AfxMessageBox( IDS_GET_CHECK_COLOR_FAILED );
            
            l_bOk = false;
        }
        RegFlushKey ( l_hKey );
        
        RegCloseKey( l_hKey );
    }
    
    if ( l_dwState )
    {
        m_ColorCheck.SetCheck( BST_CHECKED );
    }
    else
    {
        m_ColorCheck.SetCheck( BST_UNCHECKED );
    }
    
    return l_bOk;
}

////////////////////////////////////////////////////////////////////////////////////////
// Get the OEM Specific info from the oeminfo.ini file
////////////////////////////////////////////////////////////////////////////////////////
bool SVDriveInitDlg::GetOEMInfo()
{
    bool rc = false;
    
    _TCHAR tmp[OEMINFO_SIZE+1];
    DWORD cnt;
    
    cnt = GetPrivateProfileString(oemSection,_T("SerialNo"),_T(""),tmp,sizeof(tmp),oeminfoFileName);
    if( cnt )
    {
        m_serial_number.SetWindowText( tmp );
        m_serial_number.SetModify( FALSE );
        rc = true;
    }
    
    cnt = GetPrivateProfileString(oemSection,_T("ModelNo"),_T(""),tmp,sizeof(tmp),oeminfoFileName);
    if( cnt )
    {
        m_model_number.SetWindowText( tmp );
        m_model_number.SetModify( FALSE );
        rc = true;
    }
    
    cnt = GetPrivateProfileString(oemSection,_T("Date"),_T(""),tmp,sizeof(tmp),oeminfoFileName);
    if( cnt )
    {
        m_date.SetWindowText( tmp );
        m_date.SetModify( FALSE );
        rc = true;
    }

    cnt = GetPrivateProfileString(oemSection,_T("ServicedBy"),_T(""),tmp,sizeof(tmp),oeminfoFileName);
    if( cnt )
    {
        m_serviced_by.SetWindowText( tmp );
        m_serviced_by.SetModify( FALSE );
        rc = true;
    }

	cnt = GetPrivateProfileString( oemSection, _T( "SingleCamera" ), _T( "" ), tmp, sizeof( tmp ), oeminfoFileName );
	if ( cnt )
	{
		CString l_SingleCamera = tmp;
		bool l_Checked = l_SingleCamera.Left( 1 ) == "Y";
		m_SingleCamera.SetCheck( l_Checked ? BST_CHECKED : BST_UNCHECKED );
		rc = true;
	}

    cnt = GetPrivateProfileString(generalSection, _T("Model"),_T(""),tmp,sizeof(tmp),oeminfoFileName);
    if( cnt )
    {
        CString szTemp;
        szTemp = tmp;
        
        if(szTemp.Left(4) == "SVIM")
        {
            m_type.SetCurSel(0);
        }
        else if (szTemp.Left(4) == "SVPC")
        {
            m_type.SetCurSel(1);
        }
        else
        {
            m_type.SetCurSel(-1);
        }
        
        //		m_serviced_by.SetModify( FALSE );
        rc = true;
    }

    return rc;
}

////////////////////////////////////////////////////////////////////////////////////////
// Get the sysprep.inf product ID info
////////////////////////////////////////////////////////////////////////////////////////
bool SVDriveInitDlg::GetSysPrepInfo()
{
	bool rc = true;

	m_cdkey1.SetWindowText( m_CDKey.GetSubElement( 0 ) );
	m_cdkey1.SetModify( FALSE );

	m_cdkey2.SetWindowText( m_CDKey.GetSubElement( 1 ) );
	m_cdkey2.SetModify( FALSE );

	m_cdkey3.SetWindowText( m_CDKey.GetSubElement( 2 ) );
	m_cdkey3.SetModify( FALSE );

	m_cdkey4.SetWindowText( m_CDKey.GetSubElement( 3 ) );
	m_cdkey4.SetModify( FALSE );

	m_cdkey5.SetWindowText( m_CDKey.GetSubElement( 4 ) );
	m_cdkey5.SetModify( FALSE );

	return rc;
}

////////////////////////////////////////////////////////////////////////////////////////
// Update the Registry
////////////////////////////////////////////////////////////////////////////////////////
bool SVDriveInitDlg::UpdateRegistryInfo()
{
	bool l_bOk = true;
	
	HKEY l_hKey = NULL;
	
	int l_iErrorStatus = RegOpenKeyEx( HKEY_CURRENT_USER, g_tszSVOSettings, 0, KEY_SET_VALUE,	&l_hKey );
	
	if ( l_iErrorStatus != ERROR_SUCCESS )
	{
		unsigned long l_ulValue = 0;
		
		l_iErrorStatus = RegCreateKeyEx( HKEY_CURRENT_USER, 
			g_tszSVOSettings,
			NULL,
			_T(""), 
			REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS,
			(SECURITY_ATTRIBUTES *) NULL,
			&l_hKey,
			&l_ulValue );
	}
	
	if ( l_iErrorStatus == ERROR_SUCCESS )
	{
		DWORD l_dwTemp = m_ColorCheck.GetCheck() == BST_CHECKED;
		
		l_iErrorStatus = RegSetValueEx( l_hKey, _T( "Color SVIM Mode Active" ), NULL, REG_DWORD, (unsigned char *)&l_dwTemp, sizeof l_dwTemp );
		
		l_bOk = l_iErrorStatus == ERROR_SUCCESS;
		
		RegFlushKey ( l_hKey );
		
		RegCloseKey( l_hKey );
		
		l_hKey = NULL;
	}
	
	// Check if model number has been entered
	if( m_model_number.GetModify() )
	{
		CString l_csModel;
		
		CString l_csFrameGrabber;
		CString l_csIOBoard;
		
		m_model_number.GetWindowText( l_csModel );
		
		l_csModel.MakeUpper();
		
		for ( int i = 0, j = 0; i < l_csModel.GetLength() && j < 7; i++ )
		{
			if( ::isalnum( l_csModel[i] ) )
			{
				switch ( ++j )
				{
					case 3:
					case 4:
					{
						l_csFrameGrabber += l_csModel[i];
						
						break;
					}
					case 5:
					case 6:
					{
						l_csIOBoard += l_csModel[i];
						
						break;
					}
				}
			}
		}

		CString l_csFrameGrabberName;
		TCHAR *l_pszName = l_csFrameGrabberName.GetBuffer( 1024 );
		GetPrivateProfileString( _T("Frame Grabber"), l_csFrameGrabber, _T(""), l_pszName, 1024, hardwareFileName);
		l_csFrameGrabberName.ReleaseBuffer();

		if( 0 < l_csFrameGrabberName.GetLength() )
		{
			CString l_csFrameGrabberDLL;
			TCHAR *l_pszDLL = l_csFrameGrabberDLL.GetBuffer( 1024 );
			GetPrivateProfileString( l_csFrameGrabberName, _T("DigitizerDLL"), _T(""), l_pszDLL, 1024, hardwareFileName);
			l_csFrameGrabberDLL.ReleaseBuffer();

			l_bOk = ( UpdateMatrox( l_csFrameGrabberName ) == S_OK );

			
			if( l_csFrameGrabberDLL.CompareNoCase( _T("SVMatroxGige.DLL")) >= 0 )
			{
				AddCommandBeforeSVObserver(_T("net start \042Matrox GigE Vision Assistant Service\042"), false);
			}
			else
			{
				AddCommandBeforeSVObserver(_T("net start \042Matrox GigE Vision Assistant Service\042"), true);
			}
		}

		CString l_csIOBoardName;
		TCHAR *l_pszBoardName = l_csIOBoardName.GetBuffer( 1024 );
		GetPrivateProfileString( _T("IO Board"), l_csIOBoard, _T(""), l_pszBoardName, 1024, hardwareFileName);
		l_csIOBoardName.ReleaseBuffer();

		if( 0 < l_csIOBoardName.GetLength() )
		{
			CString l_csIOBoardDLL;
			TCHAR *l_pszDLL = l_csIOBoardDLL.GetBuffer( 1024 );
			GetPrivateProfileString( l_csIOBoardName, _T("DigitalIODLL"), _T(""), l_pszDLL, 1024, hardwareFileName);
			l_csIOBoardDLL.ReleaseBuffer();

			l_bOk &= ( UpdateTVicLpt( l_csIOBoardDLL ) == S_OK );
		}
	}
	
	return l_bOk;
}

////////////////////////////////////////////////////////////////////////////////////////
// Update the OEMSpecific Info in the oeminfo.ini file
////////////////////////////////////////////////////////////////////////////////////////
bool SVDriveInitDlg::UpdateOEMInfo()
{
    bool rc = true;
    _TCHAR tmp[81];
    BOOL retCode;
    CString szTemp, szModel, szSerial,szType;
    
    
    // Check if Serial number has been entered
    if( m_serial_number.GetModify() )
    {
        m_serial_number.GetWindowText( tmp, sizeof( tmp ) );
        retCode = WritePrivateProfileString(oemSection, _T("SerialNo"), tmp, oeminfoFileName);
        if( !retCode )
        {
            rc = false;
        }
    }
    
    // Check if Date Shipped has been entered
    if( m_date.GetModify() )
    {
        m_date.GetWindowText( tmp, sizeof( tmp ) );
        retCode = WritePrivateProfileString(oemSection, _T("Date"), tmp, oeminfoFileName);
        if( !retCode )
        {
            rc = false;
        }
    }
    
    // Check if model number has been entered
    if( m_model_number.GetModify() )
    {
        m_model_number.GetWindowText( tmp, sizeof( tmp ) );
        retCode = WritePrivateProfileString(oemSection, _T("ModelNo"), tmp, oeminfoFileName);
        if( !retCode )
        {
            retCode = false;
        }
    }
    
    // Check if Serviced By has been entered
    if( m_serviced_by.GetModify() )
    {
        m_serviced_by.GetWindowText( tmp, sizeof( tmp ) );
        retCode = WritePrivateProfileString(oemSection, _T("ServicedBy"), tmp, oeminfoFileName);
        if( !retCode )
        {
            rc = false;
        }
    }

    retCode = WritePrivateProfileString( oemSection, _T( "SingleCamera" ), ( m_SingleCamera.GetCheck() == BST_CHECKED ) ? _T( "Y" ) : _T( "N" ), oeminfoFileName );
    if( !retCode )
    {
        rc = false;
    }

    m_type.GetWindowText(szType);
    m_serial_number.GetWindowText(szSerial);
    m_model_number.GetWindowText(szModel);
    
    szTemp = szType + " " + szSerial + " " + szModel;
    retCode = WritePrivateProfileString(generalSection, _T("Model"), szTemp, oeminfoFileName);
    if( !retCode )
    {
        rc = false;
    }
    
    return rc;
}

bool SVDriveInitDlg::UpdateSVIMInfo()
{
//	_TCHAR tmp[SVIMINFO_SIZE + 1];
    bool rc = true;
    
    return rc;
}

////////////////////////////////////////////////////////////////////////////////////////
// Update sysprep.inf info
////////////////////////////////////////////////////////////////////////////////////////
bool SVDriveInitDlg::UpdateSysPrepInfo()
{
	bool rc = true;

	// Check if Serial number has been entered
	if( m_cdkey1.GetModify() || m_cdkey2.GetModify() || m_cdkey3.GetModify() || m_cdkey4.GetModify() || m_cdkey5.GetModify() )
	{
		CString szCdkeyPart;

		m_cdkey1.GetWindowText(szCdkeyPart);
		m_CDKey.SetSubElement( 0, szCdkeyPart );

		m_cdkey2.GetWindowText(szCdkeyPart);
		m_CDKey.SetSubElement( 1, szCdkeyPart );

		m_cdkey3.GetWindowText(szCdkeyPart);
		m_CDKey.SetSubElement( 2, szCdkeyPart );

		m_cdkey4.GetWindowText(szCdkeyPart);
		m_CDKey.SetSubElement( 3, szCdkeyPart );

		m_cdkey5.GetWindowText(szCdkeyPart);
		m_CDKey.SetSubElement( 4, szCdkeyPart );
	}

	return rc;
}

void SVDriveInitDlg::OnCancel() 
{
    CDialog::OnCancel();
}

void SVDriveInitDlg::OnChangeCdkey1() 
{
    int nStartChar,nEndChar;
    
    m_cdkey1.GetSel(nStartChar,nEndChar);
    
    CWnd* pWnd = GetNextDlgTabItem((CWnd*)&m_cdkey1,FALSE);
    if(pWnd && (nStartChar == 5) && (nEndChar == 5))
    {
        pWnd->SetFocus();
        ((CEdit*)pWnd)->SetSel(0,-1);
    }
}

void SVDriveInitDlg::OnChangeCdkey2() 
{
    int nStartChar,nEndChar;
    
    m_cdkey2.GetSel(nStartChar,nEndChar);
    
    CWnd* pWnd = GetNextDlgTabItem((CWnd*)&m_cdkey2,FALSE);
    if(pWnd && (nStartChar == 5) && (nEndChar == 5))
    {
        pWnd->SetFocus();
        ((CEdit*)pWnd)->SetSel(0,-1);
    }
}

void SVDriveInitDlg::OnChangeCdkey3() 
{
    int nStartChar,nEndChar;
    
    m_cdkey3.GetSel(nStartChar,nEndChar);
    
    CWnd* pWnd = GetNextDlgTabItem((CWnd*)&m_cdkey3,FALSE);
    if(pWnd && (nStartChar == 5) && (nEndChar == 5))
    {
        pWnd->SetFocus();
        ((CEdit*)pWnd)->SetSel(0,-1);
    }
}

void SVDriveInitDlg::OnChangeCdkey4() 
{
    int nStartChar,nEndChar;
    
    m_cdkey4.GetSel(nStartChar,nEndChar);
    
    CWnd* pWnd = GetNextDlgTabItem((CWnd*)&m_cdkey4,FALSE);
    if(pWnd && (nStartChar == 5) && (nEndChar == 5))
    {
        pWnd->SetFocus();
        ((CEdit*)pWnd)->SetSel(0,-1);
    }
}

void SVDriveInitDlg::OnChangeCdkey5() 
{
    int nStartChar,nEndChar;
    
    m_cdkey5.GetSel(nStartChar,nEndChar);
    
    CWnd* pWnd = GetNextDlgTabItem((CWnd*)&m_cdkey5,FALSE);
    if(pWnd && (nStartChar == 5) && (nEndChar == 5))
    {
        pWnd->SetFocus();
        ((CEdit*)pWnd)->SetSel(0,-1);
    }
}

HRESULT SVDriveInitDlg::BackupBootIni()
{
	HRESULT l_Status = S_OK;
	CString l_NewBootName;

	for( int i = 0; i < 1000; ++i )
	{
		l_NewBootName.Format( _T( "C:\\boot.%03d" ), i );

		if( ::_access( l_NewBootName, 0 ) == 0 )
		{
			//get the file attributes
			DWORD l_Attr = GetFileAttributes( l_NewBootName );
	    
			//remove the read only flag
			l_Attr = l_Attr & ~FILE_ATTRIBUTE_READONLY;
	    
			//set the file to read only
			SetFileAttributes( l_NewBootName, l_Attr );

			if( i < 1 || 9 < i)
			{
				::remove( l_NewBootName );
			}
			else
			{
				CString l_ToBootName;
				
				l_ToBootName.Format( _T( "C:\\boot.%03d" ), i - 1 );

				::rename( l_NewBootName, l_ToBootName );
			}
		}
		else
		{
			if( 9 < i )
			{
				l_NewBootName.Format( _T( "C:\\boot.%03d" ), 9 );
			}

			break;
		}
	}

  //copy boot.ini -> boot.xxx
  CopyFile( _T( "C:\\boot.ini" ), l_NewBootName, FALSE );

	DWORD l_Attr = GetFileAttributes( l_NewBootName );
  
	//remove the read only flag
	l_Attr = l_Attr & ~FILE_ATTRIBUTE_READONLY;
  
	//set the file to read only
	SetFileAttributes( l_NewBootName, l_Attr );

	return l_Status;
}

HRESULT SVDriveInitDlg::UpdateTVicLpt( LPCTSTR p_szIOBoard )
{
	HRESULT l_Status = S_OK;

	CString l_IOBoardDLL = p_szIOBoard;

	std::deque< std::string > l_FileData;

	std::ifstream l_CmdFile;

	l_CmdFile.open( _T( "C:\\SVObserver\\bin\\SVObserver1.cmd" ) );

	if( l_CmdFile.is_open() )
	{
		while( ! l_CmdFile.eof() )
		{
			std::string l_Temp;

			std::getline( l_CmdFile, l_Temp );

			if( ! l_Temp.empty() )
			{
				l_FileData.push_back( l_Temp );
			}
		}

		l_CmdFile.close();
	}

	std::deque< std::string >::iterator l_Iter = l_FileData.begin();

	while( l_Iter != l_FileData.end() )
	{
		CString l_Temp = l_Iter->c_str();

		if( l_Temp.CompareNoCase( _T( "C:\\SVObserver\\bin\\InitializeIOSubsystem.exe" ) ) == 0 )
		{
			break;
		}

		++l_Iter;
	}

	if( l_IOBoardDLL.Compare( _T( "SVTVicLpt.dll" ) ) == 0 )
	{
		if( l_Iter == l_FileData.end() )
		{
			l_FileData.insert( l_FileData.begin(), _T( "C:\\SVObserver\\bin\\InitializeIOSubsystem.exe" ) );
		}
	}
	else
	{
		if( l_Iter != l_FileData.end() )
		{
			l_FileData.erase( l_Iter );
		}
	}

	std::ofstream l_WriteCmdFile;

	l_WriteCmdFile.open( _T( "C:\\SVObserver\\bin\\SVObserver1.cmd" ), std::ios_base::out | std::ios_base::trunc );

	if( l_WriteCmdFile.is_open() )
	{
		l_Iter = l_FileData.begin();

		while( l_Iter != l_FileData.end() )
		{
			l_WriteCmdFile << *l_Iter << std::endl;

			++l_Iter;
		}

		l_WriteCmdFile.close();
	}

	return l_Status;
}

HRESULT SVDriveInitDlg::UpdateMatrox( LPCTSTR p_szDigitizer )
{
	HRESULT l_Status = S_OK;

	int l_Size = GetPrivateProfileInt( p_szDigitizer, _T("MatroxReserveMemorySize"), 0, hardwareFileName );

	if( 0 < l_Size )
	{
		CString l_From;
		CString l_To;

		// l_Status = AddMatroxBootIniMaxMem( l_Size );   // This causes the system to crash on boot.  Use MILConfig to set memory.

		l_From.Format( _T( "%s\\mtxgige.svr" ), windowsDriverPath );
		l_To.Format( _T( "%s\\mtxgige.sys" ), windowsDriverPath );

		if( ::_access( l_To, 0 ) == -1 )
		{
			::rename( l_From, l_To ); 
		}

		l_From.Format( _T( "%s\\mtxgigefilter.svr" ), windowsDriverPath );
		l_To.Format( _T( "%s\\mtxgigefilter.sys" ), windowsDriverPath );

		if( ::_access( l_To, 0 ) == -1 )
		{
			::rename( l_From, l_To ); 
		}
	}
	else
	{
		CString l_From;
		CString l_To;

		l_Status = RemoveMatroxBootIniMaxMem();

		l_From.Format( _T( "%s\\mtxgige.sys" ), windowsDriverPath );
		l_To.Format( _T( "%s\\mtxgige.svr" ), windowsDriverPath );

		if( ::_access( l_From, 0 ) == 0 )
		{
			::rename( l_From, l_To ); 
		}

		l_From.Format( _T( "%s\\mtxgigefilter.sys" ), windowsDriverPath );
		l_To.Format( _T( "%s\\mtxgigefilter.svr" ), windowsDriverPath );

		if( ::_access( l_From, 0 ) == 0 )
		{
			::rename( l_From, l_To ); 
		}
	}

	return l_Status;
}

HRESULT SVDriveInitDlg::UpdateMatroxRegistryMaxMem( size_t& p_rMaxSize, size_t p_MILSize )
{
	HRESULT l_Status = S_OK;

	HKEY l_hKey = NULL;
	size_t l_MaxSize = 0;
	
	l_Status = RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T( "SYSTEM\\ControlSet001\\Services\\MtxDma0\\Parameters" ), 0, KEY_ALL_ACCESS, &l_hKey );
	
	if ( l_Status == ERROR_SUCCESS )
	{
		DWORD l_Size = 0;
		DWORD l_Type = 0;

		l_Status = RegQueryValueEx( l_hKey, _T( "HighMemoryAreaReservedStart" ), NULL, &l_Type, NULL, &l_Size);

		if( l_Status == ERROR_SUCCESS )
		{
			if( l_Type == REG_DWORD )
			{
				l_Size = sizeof( l_MaxSize );

				l_Status = RegQueryValueEx( l_hKey, _T( "HighMemoryAreaReservedStart" ), NULL, &l_Type, (LPBYTE) &l_MaxSize, &l_Size );
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
	}

	if ( l_Status == ERROR_SUCCESS )
	{
		size_t l_SizeInMB = p_MILSize << 20;

		HRESULT l_Temp = RegSetValueEx( l_hKey, _T( "DmaBufferSize" ), NULL, REG_DWORD, (unsigned char *)&p_MILSize, sizeof p_MILSize );
		
		l_Status = RegSetValueEx( l_hKey, _T( "HighMemoryAreaReservedSize" ), NULL, REG_DWORD, (unsigned char *)&l_SizeInMB, sizeof l_SizeInMB );

		if( l_Temp != ERROR_SUCCESS )
		{
			l_Status = l_Temp;
		}
		
		RegFlushKey ( l_hKey );
		
		RegCloseKey( l_hKey );
		
		l_hKey = NULL;

		l_MaxSize -= l_SizeInMB;
	}
	
	if( l_Status == ERROR_SUCCESS )
	{
		p_rMaxSize = l_MaxSize >> 20;
	}
	else
	{
		p_rMaxSize = 0;
	}

	return l_Status;
}

HRESULT SVDriveInitDlg::AddMatroxBootIniMaxMem( size_t p_MILSize )
{
	HRESULT l_Status = BackupBootIni();

	if( l_Status == S_OK )
	{
		DWORD l_Attr = GetFileAttributes( _T( "C:\\boot.ini" ) );
	  
		//remove the read only flag
		l_Attr = l_Attr & ~FILE_ATTRIBUTE_READONLY;
	  
		//set the file to read only
		SetFileAttributes( _T( "C:\\boot.ini" ), l_Attr );

		CString l_Default;

		TCHAR *l_pszDefault = l_Default.GetBuffer( 1024 );
		GetPrivateProfileString( _T("boot loader"), _T("default"), _T(""), l_pszDefault, 1024, _T( "C:\\boot.ini" ) );
		l_Default.ReleaseBuffer();

		if( 0 < l_Default.GetLength() )
		{
			CString l_Option;

			TCHAR *l_pszOption = l_Option.GetBuffer( 1024 );
			GetPrivateProfileString( _T("operating systems"), l_Default, _T(""), l_pszOption, 1024, _T( "C:\\boot.ini" ) );
			l_Option.ReleaseBuffer();

			if( 0 < l_Option.GetLength() )
			{
				if( l_Option.Find( _T( "/MAXMEM=" ) ) == -1 )
				{
					int l_Pos = l_Option.ReverseFind( '\"' );

					if( l_Pos != -1 )
					{
						size_t l_MaxSize = 0;

						l_Status = UpdateMatroxRegistryMaxMem( l_MaxSize, p_MILSize );

						if( l_Status == S_OK )
						{
							CString l_NewOption;

							l_NewOption.Format( _T( "%s(Memory: NT=%d MB, MIL=%d.0 MB)%c /MAXMEM=%d%s" ), 
								l_Option.Left( l_Pos ), static_cast<int>(l_MaxSize), static_cast<int>(p_MILSize), l_Option[ l_Pos ], static_cast<int>(p_MILSize), l_Option.Mid( l_Pos + 1 ) );

							WritePrivateProfileString( _T("operating systems"), l_Default, l_NewOption, _T( "C:\\boot.ini" ) );
						}
					}
					else
					{
						l_Status = E_FAIL;
					}
				}
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}

		l_Attr = GetFileAttributes( _T( "C:\\boot.ini" ) );
	  
		//remove the read only flag
		l_Attr = l_Attr | FILE_ATTRIBUTE_READONLY;
	  
		//set the file to read only
		SetFileAttributes( _T( "C:\\boot.ini" ), l_Attr );
	}

	return l_Status;
}

HRESULT SVDriveInitDlg::RemoveMatroxBootIniMaxMem()
{
	HRESULT l_Status = BackupBootIni();

	if( l_Status == S_OK )
	{
		DWORD l_Attr = GetFileAttributes( _T( "C:\\boot.ini" ) );
	  
		//remove the read only flag
		l_Attr = l_Attr & ~FILE_ATTRIBUTE_READONLY;
	  
		//set the file to read only
		SetFileAttributes( _T( "C:\\boot.ini" ), l_Attr );

		CString l_Default;

		TCHAR *l_pszDefault = l_Default.GetBuffer( 1024 );
		GetPrivateProfileString( _T("boot loader"), _T("default"), _T(""), l_pszDefault, 1024, _T( "C:\\boot.ini" ) );
		l_Default.ReleaseBuffer();

		if( 0 < l_Default.GetLength() )
		{
			CString l_Option;

			TCHAR *l_pszOption = l_Option.GetBuffer( 1024 );
			GetPrivateProfileString( _T("operating systems"), l_Default, _T(""), l_pszOption, 1024, _T( "C:\\boot.ini" ) );
			l_Option.ReleaseBuffer();

			if( 0 < l_Option.GetLength() )
			{
				if( l_Option.Find( _T( "/MAXMEM=" ) ) != -1 )
				{
					int l_MemoryPos = l_Option.Find( "(Memory: NT=" );
					int l_MaxMemPos = l_Option.Find( "/MAXMEM=" );

					if( l_MemoryPos == -1 )
					{
						l_MemoryPos = l_Option.Find( _T( '\"' ), 1 );
					}

					if( l_MemoryPos != -1 )
					{
						while( 0 <= l_MemoryPos && l_Option[ l_MemoryPos - 1 ] == _T( ' ' ) )
						{
							--l_MemoryPos;
						}
					}

					if( l_MemoryPos != -1 && l_MaxMemPos != -1 )
					{
						CString l_NewOption;
						size_t l_MaxSize = 0;

						int l_MaxMemEndSpacePos = l_Option.Find( ' ', l_MaxMemPos + 8 );
						int l_MaxMemEndSlashPos = l_Option.Find( '/', l_MaxMemPos + 8 );

						if( l_MaxMemEndSpacePos == -1 && l_MaxMemEndSlashPos == -1 )
						{
							l_NewOption.Format( _T( "%s\"" ), l_Option.Left( l_MemoryPos ) );
						}
						else if( l_MaxMemEndSpacePos == -1 )
						{
							l_NewOption.Format( _T( "%s\" %s" ), l_Option.Left( l_MemoryPos ), l_Option.Mid( l_MaxMemEndSlashPos ) );
						}
						else if( l_MaxMemEndSlashPos == -1 )
						{
							l_NewOption.Format( _T( "%s\"%s" ), l_Option.Left( l_MemoryPos ), l_Option.Mid( l_MaxMemEndSpacePos ) );
						}
						else
						{
							if( l_MaxMemEndSpacePos < l_MaxMemEndSlashPos )
							{
								l_NewOption.Format( _T( "%s\"%s" ), l_Option.Left( l_MemoryPos ), l_Option.Mid( l_MaxMemEndSpacePos ) );
							}
							else
							{
								l_NewOption.Format( _T( "%s\" %s" ), l_Option.Left( l_MemoryPos ), l_Option.Mid( l_MaxMemEndSlashPos ) );
							}
						}

						WritePrivateProfileString( _T("operating systems"), l_Default, l_NewOption, _T( "C:\\boot.ini" ) );

						l_Status = UpdateMatroxRegistryMaxMem( l_MaxSize, 0 );
					}
					else
					{
						l_Status = E_FAIL;
					}
				}
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}

		l_Attr = GetFileAttributes( _T( "C:\\boot.ini" ) );
	  
		//remove the read only flag
		l_Attr = l_Attr | FILE_ATTRIBUTE_READONLY;
	  
		//set the file to read only
		SetFileAttributes( _T( "C:\\boot.ini" ), l_Attr );
	}

	return l_Status;
}

HRESULT SVDriveInitDlg::AddCommandBeforeSVObserver( LPCTSTR p_strNewCommand, bool p_bRemove=false )
{
	HRESULT l_Status = S_OK;

	std::deque< std::string > l_FileData;

	std::ifstream l_CmdFile;

	l_CmdFile.open( _T( "C:\\SVObserver\\bin\\SVObserver1.cmd" ) );

	// Read in the file.
	if( l_CmdFile.is_open() )
	{
		while( ! l_CmdFile.eof() )
		{
			std::string l_Temp;

			std::getline( l_CmdFile, l_Temp );

			if( ! l_Temp.empty() )
			{
				l_FileData.push_back( l_Temp );
			}
		}

		l_CmdFile.close();
	}

	std::deque< std::string >::iterator l_Iter = l_FileData.begin();

	// Find the line containing the new line or SVObserver.exe.
	while( l_Iter != l_FileData.end() )
	{
		CString l_Temp = l_Iter->c_str();
		if( l_Temp.Find( p_strNewCommand ) >= 0 )
		{
			if( p_bRemove )
			{
				l_FileData.erase( l_Iter );
				break;
			}
			else
			{	// No need to do anything else. It is already here.
				return S_OK;
			}
		}

		if( l_Temp.Find( _T( "SVObserver.exe" ) ) >= 0 )
		{
			if( !p_bRemove )
			{
				l_FileData.insert( l_Iter, p_strNewCommand );
			}
			break;
		}

		++l_Iter;
	}

	std::ofstream l_WriteCmdFile;

	l_WriteCmdFile.open( _T( "C:\\SVObserver\\bin\\SVObserver1.cmd" ), std::ios_base::out | std::ios_base::trunc );

	if( l_WriteCmdFile.is_open() )
	{
		l_Iter = l_FileData.begin();

		while( l_Iter != l_FileData.end() )
		{
			l_WriteCmdFile << *l_Iter << std::endl;

			++l_Iter;
		}

		l_WriteCmdFile.close();
	}

	return l_Status;
}


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVDriveInit\SVDriveInitDlg.cpp_v  $
 * 
 *    Rev 1.3   16 May 2014 08:53:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised BackupBootIni to correct type mismatch.
 * Revised AddMatroxBootIniMaxMem to correct type mismatch.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   30 Sep 2013 14:58:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platforms.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 14:06:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 009.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   12 Apr 2013 14:28:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Updated issues  with PVCS log entries.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 08:33:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   11 Dec 2012 18:21:34   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   (DoDataExchange) Added IDC_SINGLECAMERA_CHECK.
 *   (GetOEMInfo) Added read of new key "SingleCamera".
 *   (UpdateOEMInfo) Added write of new key "SingleCamera".
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   10 Sep 2012 09:47:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  788
 * SCR Title:  Modify SVDriveInit so it adds net start to GigE images
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added a function to add/remove a command before SVObserver.exe.  Use the new function to add a net start command before SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25.1.1   11 Dec 2012 17:18:20   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   (DoDataExchange) Added IDC_SINGLECAMERA_CHECK.
 *   (GetOEMInfo) Added read of new key "SingleCamera".
 *   (UpdateOEMInfo) Added write of new key "SingleCamera".
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25.1.0   08 Nov 2012 15:08:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  788
 * SCR Title:  Modify SVDriveInit so it adds net start to GigE images
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added a function to add/remove a command before SVObserver.exe.  Use the new function to add a net start command before SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   28 Jun 2012 15:00:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removing Shared Memory for the webserver from the SVObserver application.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   23 Jan 2012 11:16:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  739
 * SCR Title:  Upgrade SVObserver for Devario Release
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Fixed issue with Type defaulting to SVPC, now will come up blank.  Also fixed issue with OEM number not wrting to the oeminfo.ini
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   19 Dec 2011 14:05:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Drive Init program to enable and configure the shared memory functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   12 Nov 2009 13:09:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  668
 * SCR Title:  Upgrade SVObserver version for Barracuda release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to fix GigE issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   09 Nov 2009 09:41:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  668
 * SCR Title:  Upgrade SVObserver version for Barracuda release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated software to fix issues with FBReseal.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   05 Nov 2009 09:20:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  668
 * SCR Title:  Upgrade SVObserver version for Barracuda release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed batch file creation issue.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   04 Nov 2009 18:46:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  668
 * SCR Title:  Upgrade SVObserver version for Barracuda release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated drive init program to fix issues associated with new functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   12 Feb 2009 14:09:34   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  638
 * SCR Title:  Upgrade SVObserver to 5.20
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated to use fbreseal instead of sysprep
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   20 Apr 2007 10:06:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  595
 * SCR Title:  Fix issues with Utility Menu when switching products
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed section where it was writing the UpdateFirmware to the registry. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   13 Apr 2007 07:20:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  586
 * SCR Title:  Update integration programs to remove matrox memory reservation from boot.ini
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   took out section that wrote out the boot.ini.  This was no longer needed
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   29 Mar 2007 15:15:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  593
 * SCR Title:  Remove HUB product types from SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed more HUB Information that was missed in UpdateSVIMInfo.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   23 Mar 2007 07:35:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  593
 * SCR Title:  Remove HUB product types from SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed checkbox for HUB / Shared bandwidth.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   06 Apr 2006 14:33:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  540
 * SCR Title:  Integrate Coreco X64 Analog card in SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed the string length for the X64. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   31 Mar 2006 14:57:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  540
 * SCR Title:  Integrate Coreco X64 Analog card in SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added support for the X64-AN board
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   20 Jan 2006 07:04:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  538
 * SCR Title:  Remove instances when displaying RAID error message on event log read errors
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to add start-up support for 1394 Shared Bandwidth/Hub configuration.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   08 Nov 2005 15:13:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Dialog to use new HARDWARE.INI file for configuring SVIM.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   17 Oct 2005 10:37:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated model number
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   22 Feb 2005 13:53:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  452
 * SCR Title:  Upgrade SVObserver to version 4.50
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated to add new model numbers for matrox boards.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   28 May 2004 08:30:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  415
 * SCR Title:  Upgrade SVObserver to 4.31
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   made changes so that the color flag will be saved.  a 4783 system will now get the correct boot.ini if the model number is entered correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.4   13 Jan 2004 13:31:12   Joe
* Project:  SVObserver
* Change Request (SCR) nbr:  352
* SCR Title:  Update SVObserver to Version 4.30 Release
* Checked in by:  rYoho;  Rob Yoho
* Change Description:  
*   Update information to prevent sysprep from running when the OEM number does not change.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.3   12 Jan 2004 11:34:12   Joe
* Project:  SVObserver
* Change Request (SCR) nbr:  352
* SCR Title:  Update SVObserver to Version 4.30 Release
* Checked in by:  rYoho;  Rob Yoho
* Change Description:  
*   Updated dialog to not run sysprep program unless the OEM number change.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.2   16 Dec 2003 14:26:58   Joe
* Project:  SVObserver
* Change Request (SCR) nbr:  352
* SCR Title:  Update SVObserver to Version 4.30 Release
* Checked in by:  rYoho;  Rob Yoho
* Change Description:  
*   Added new SVIM Color flag to the dialog to remove the need for adjusting color flag with registry files.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.1   10 Sep 2003 10:09:06   Joe
* Project:  SVObserver
* Change Request (SCR) nbr:  322
* SCR Title:  Add Additional Digital IO Resources to SVObserver
* Checked in by:  eBeyeler;  Eric Beyeler
* Change Description:  
*   Update OnOK method to add different error messages.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.0   28 Aug 2003 10:58:30   Joe
* Project:  SVObserver
* Change Request (SCR) nbr:  322
* SCR Title:  Add Additional Digital IO Resources to SVObserver
* Checked in by:  rYoho;  Rob Yoho
* Change Description:  
*   Initial Check-in.
* 
* /////////////////////////////////////////////////////////////////////////////////////
*/
