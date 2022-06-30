//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDriveInitDlg
//* .File Name       : $Workfile:   SVDriveInitDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   17 Sep 2014 15:03:26  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "resource.h"
//Moved to precompiled header: #include <deque>
//Moved to precompiled header: #include <fstream>
//Moved to precompiled header: #include <io.h>
//Moved to precompiled header: #include <iostream>
//Moved to precompiled header: #include <string>

#include "SVLibrary/InitialInformation.h"
#include "SVLibrary/SVOINILoader.h"
#include "SVDriveInitDlg.h"
#include "cpassdlg.h"
#include "SVStatusLibrary\GlobalPath.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PRODUCTID_PART_SIZE	5
#define OEMKEYWORD_SIZE		3

#define REGISTRYPRODUCTID_SIZE 23  
#define CDPRODUCTID_SIZE 29  

#define OEMINFO_SIZE		80	// max size of entry in OEMSpecific section in oeminfo.ini
#define SERIALNUMBER_SIZE	20
#define MODELNUMBER_SIZE	8

constexpr char g_tszSVOUpdateFirmware[] = _T("Software\\SVR Gesellschaft f�r Bildverarbeitung mbH\\SVObserver\\Utilities\\Update Firmware");
constexpr char g_tszSVOSettings[] = _T("Software\\SVR Gesellschaft f�r Bildverarbeitung mbH\\SVObserver\\Settings");
constexpr char productIdLocation[] = _T("Software\\Microsoft\\Windows NT\\CurrentVersion");
constexpr char oemSection[] = _T("OEMSpecific");
constexpr char generalSection[] = _T("general");
constexpr char UserDataSection[] = _T("UserData");

constexpr char g_tszWindowsRunOnce[] = _T("Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce");

constexpr char g_bootIniFilepath[] = _T("C:\\boot.ini");
constexpr char g_bootIniBackupFilepath[] = _T("C:\\boot.%03d");

constexpr char g_mtxgigeSYS[] = _T("%s\\mtxgige.sys");
constexpr char g_mtxgigeSVR[] = _T("%s\\mtxgige.svr");
constexpr char g_mtxgigefilterSYS[] = _T("%s\\mtxgigefilter.sys");
constexpr char g_mtxgigefilterSVR[] = _T("%s\\mtxgigefilter.svr");

constexpr char g_MtxDmaParamRegKey[] = _T("SYSTEM\\ControlSet001\\Services\\MtxDma0\\Parameters");
constexpr char g_MatroxReserveMemorySizeTag[] = _T("MatroxReserveMemorySize");
constexpr char g_HighMemoryAreaReservedStartTag[] = _T("HighMemoryAreaReservedStart");
constexpr char g_DmaBufferSizeTag[] = _T("DmaBufferSize");
constexpr char g_HighMemoryAreaReservedSizeTag[] = _T("HighMemoryAreaReservedSize");
constexpr char g_MaxMemTag[] = _T("/MAXMEM=");
constexpr char g_MemoryNTTag[] = _T("(Memory: NT=");
constexpr char g_ReserveMemoryEntry[] = _T("%s(Memory: NT=%d MB, MIL=%d.0 MB)%c /MAXMEM=%d%s");

constexpr char g_SerialNoTag[] = _T("SerialNo");
constexpr char g_ModelNoTag[] = _T("ModelNo");
constexpr char g_DateTag[] = _T("Date");
constexpr char g_ServicedByTag[] = _T("ServicedBy");
constexpr char g_SingleCameraTag[] = _T("SingleCamera");
constexpr char g_ModelTag[] = _T("Model");

constexpr char g_ProductIdTag[] = _T("ProductId");
constexpr char g_SVIMTag[] = _T("SVIM");
constexpr char g_SVPCTag[] = _T("SVPC");

constexpr char g_FrameGrabberTag[] = _T("Frame Grabber");
constexpr char g_DigitizerDLLTag[] = _T("DigitizerDLL");
constexpr char g_SVMatroxGigeDLLTag[] = _T("SVMatroxGige.DLL");
constexpr char g_NetStartMatroxGigECmd[] = _T("net start \042Matrox GigE Vision Assistant Service\042");
constexpr char g_IOBoardTag[] = _T("IO Board");
constexpr char g_DigitalIODLLTag[] = _T("DigitalIODLL");

constexpr char g_bootLoaderTag[] = _T("boot loader");
constexpr char g_defaultTag[] = _T("default");
constexpr char g_OperatingSystemsTag[] = _T("operating systems");

constexpr char g_svobserver1CmdFilepath[] = _T("C:\\SVObserver\\bin\\SVObserver1.cmd");
constexpr char g_InitializeIOSubsystemFilepath[] = _T("C:\\SVObserver\\bin\\InitializeIOSubsystem.exe");
constexpr char g_SVLptIODllFilename[] = _T("SVLptIO.dll");

constexpr char g_ModelNumberNotValidMsg[] = _T("Model Number is not Valid.");
constexpr char g_ModelNumberNotValidMsgPrompt[] = _T("Model Number is not Valid.\nWould you like to correct the Model Number ?");

constexpr char g_SysprepUpdatefailedMsg[] = _T("Update System Prep Info Failed");
constexpr char g_OEMUpdateFailedMsg[] = _T("Update OEM Info Failed");
constexpr char g_SvimInfoUpdateFailedMsg[] = _T("Update SVIM Info Failed");
constexpr char g_RegistryInfoUpdateFailedMsg[] = _T("Update Registry Info Failed");
constexpr char g_CreateProcessFailedMsg[] = _T("Create Process Failed");


constexpr char g_SVObserverExeTag[] = _T("SVObserver.exe");

static TCHAR oeminfoFileName[_MAX_PATH];
static TCHAR windowsDriverPath[_MAX_PATH];

constexpr int TmpBufSize = 1024;

DWORD GetLastSystemErrorText(CString & szMsg);///< also in SVDriveInitDlg.cpp
HRESULT GetSystemErrorText(DWORD dwError, CString & szMsg);///< also in SVDriveInitDlg.cpp
void DisplayLastSystemError();

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
// SVDriveInitDlg dialog

SVDriveInitDlg::SVDriveInitDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(SVDriveInitDlg::IDD, pParent)
{
}

void SVDriveInitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVDriveInitDlg)
	DDX_Control(pDX, IDC_SINGLECAMERA_CHECK, m_SingleCamera);
	DDX_Control(pDX, ID_MODEL_NUMBER, m_model_number);
	DDX_Control(pDX, IDC_TYPE, m_type);
	DDX_Control(pDX, ID_CDKEY1, m_cdkey[0]);
	DDX_Control(pDX, ID_CDKEY2, m_cdkey[1]);
	DDX_Control(pDX, ID_CDKEY3, m_cdkey[2]);
	DDX_Control(pDX, ID_CDKEY4, m_cdkey[3]);
	DDX_Control(pDX, ID_CDKEY5, m_cdkey[4]);
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
	ON_CONTROL_RANGE(EN_CHANGE, ID_CDKEY1, ID_CDKEY5, OnChangeCdkey)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVDriveInitDlg message handlers

BOOL SVDriveInitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	assert((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	assert(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(false);
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

	SetIcon(m_hIcon, true);			// Set big icon
	SetIcon(m_hIcon, false);		// Set small icon

#ifndef _DEBUG
	CPassDlg dlg;

	INT_PTR nResponse = dlg.DoModal();
	if (IDOK == nResponse)
	{
	}
	else if (IDCANCEL == nResponse)
	{
		exit(1);
	}
#endif

	// Set Limits on Fields
	for (int i = 0; i < cCdKeyBlockNr; ++i)
	{
		m_cdkey[i].SetLimitText(PRODUCTID_PART_SIZE);
	}

	m_serial_number.SetLimitText(SERIALNUMBER_SIZE);
	m_date.SetLimitText(8);
	m_serviced_by.SetLimitText(OEMINFO_SIZE);

	// Get the Windows Directory
	::GetSystemDirectory(oeminfoFileName, sizeof(oeminfoFileName));
	_tcscat(oeminfoFileName, _T("\\oeminfo.ini"));

	::GetWindowsDirectory(windowsDriverPath, sizeof(windowsDriverPath));
	_tcscat(windowsDriverPath, _T("\\System32\\drivers"));

	//get the file attributes
	DWORD dwAttr = GetFileAttributes(oeminfoFileName);

	//clear the read only flag
	dwAttr = dwAttr & ~FILE_ATTRIBUTE_READONLY;

	//set the file to read only
	SetFileAttributes(oeminfoFileName, dwAttr);

	GetSysPrepInfo();

	// Get Info from OEMINFO.ini
	GetOEMInfo();

	bool rc = IsValidModelNumber();
	if (!rc)
	{
		CString title;
		GetWindowText(title);
		MessageBox(g_ModelNumberNotValidMsg, title, MB_ICONINFORMATION);
	}
	return true;  // return TRUE  unless you set the focus to a control
}

void SVDriveInitDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void SVDriveInitDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

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
	return (HCURSOR)m_hIcon;
}

void SVDriveInitDlg::OnOK()
{
	CString title;
	GetWindowText(title);

	bool rc = IsValidModelNumber();
	if (!rc)
	{
		int retCode = MessageBox(g_ModelNumberNotValidMsgPrompt, title, MB_ICONINFORMATION | MB_YESNO);
		if (IDYES == retCode)
		{
			return;
		}
	}
	rc = UpdateSysPrepInfo();
	if (!rc)
	{
		MessageBox(g_SysprepUpdatefailedMsg, title, MB_ICONINFORMATION);
		return;
	}

	rc = UpdateOEMInfo();
	if (!rc)
	{
		MessageBox(g_OEMUpdateFailedMsg, title, MB_ICONINFORMATION);
		return;
	}

	rc = UpdateSVIMInfo();
	if (!rc)
	{
		MessageBox(g_SvimInfoUpdateFailedMsg, title, MB_ICONINFORMATION);
		return;
	}

	rc = UpdateRegistryInfo();
	if (!rc)
	{
		MessageBox(g_RegistryInfoUpdateFailedMsg, title, MB_ICONINFORMATION);
		return;
	}

	//get the file attributes
	DWORD dwAttr = GetFileAttributes(oeminfoFileName);

	//set the read only flag
	dwAttr = dwAttr | FILE_ATTRIBUTE_READONLY;

	if (!m_CDKey.UpdateProductId())
	{
		DisplayLastSystemError();
		MessageBox(g_CreateProcessFailedMsg, title, MB_ICONINFORMATION);
		return;
	}

	//set the file to read only
	SetFileAttributes(oeminfoFileName, dwAttr);

	OnCancel();
}

bool SVDriveInitDlg::IsValidModelNumber() const
{
	SvLib::InitialInformation initialInfo;
	SvLib::SVOIniLoader iniLoader(initialInfo);
	bool bValid(true);

	CString Model;
	m_model_number.GetWindowText(Model);
	Model.MakeUpper();

	iniLoader.DecodeModelNumber(static_cast<LPCTSTR> (Model));

	if (iniLoader.isModelNumberDecodable())
	{
		if (S_OK == iniLoader.LoadHardwareIni(SvStl::GlobalPath::Inst().GetHardwareIniPath()))
		{
			if (iniLoader.GetInitialInfo().m_ProcessorBoardName.empty() || //@TODO [Arvid][8.00][16.01.2018] better create a new method here
				iniLoader.GetInitialInfo().m_TriggerBoardName.empty() ||
				iniLoader.GetInitialInfo().m_AcquisitionBoardName.empty() ||
				iniLoader.GetInitialInfo().m_DigitalBoardName.empty())
			{
				bValid = false;
			}
		}
	}
	else
	{
		bValid = false;
	}
	return bValid;
}

////////////////////////////////////////////////////////////////////////////////////////
// Get the OEM Specific info from the oeminfo.ini file
////////////////////////////////////////////////////////////////////////////////////////
bool SVDriveInitDlg::GetOEMInfo()
{
	bool rc = false;

	TCHAR tmp[OEMINFO_SIZE + 1];
	std::string serialNumber;
	DWORD cnt = GetPrivateProfileString(oemSection, g_SerialNoTag, _T(""), tmp, sizeof(tmp), oeminfoFileName);
	if (SvLib::readSerialNumberFromSystem(serialNumber))
	{
		cnt = static_cast<DWORD>(serialNumber.size());
		if (0 < cnt && OEMINFO_SIZE > cnt)
		{
			strcpy_s(tmp, serialNumber.c_str());
		}
		m_serial_number.SetReadOnly(true);
	}
	if (cnt)
	{
		m_serial_number.SetWindowText(tmp);
		m_serial_number.SetModify(false);
		rc = true;
	}

	cnt = GetPrivateProfileString(oemSection, g_ModelNoTag, _T(""), tmp, sizeof(tmp), oeminfoFileName);
	if (cnt)
	{
		m_model_number.SetWindowText(tmp);
		m_model_number.SetModify(false);
		rc = true;
	}

	cnt = GetPrivateProfileString(oemSection, g_DateTag, _T(""), tmp, sizeof(tmp), oeminfoFileName);
	if (cnt)
	{
		m_date.SetWindowText(tmp);
		m_date.SetModify(false);
		rc = true;
	}

	cnt = GetPrivateProfileString(oemSection, g_ServicedByTag, _T(""), tmp, sizeof(tmp), oeminfoFileName);
	if (cnt)
	{
		m_serviced_by.SetWindowText(tmp);
		m_serviced_by.SetModify(false);
		rc = true;
	}

	cnt = GetPrivateProfileString(oemSection, g_SingleCameraTag, _T(""), tmp, sizeof(tmp), oeminfoFileName);
	if (cnt)
	{
		CString l_SingleCamera = tmp;
		bool l_Checked = l_SingleCamera.Left(1) == _T("Y");
		m_SingleCamera.SetCheck(l_Checked ? BST_CHECKED : BST_UNCHECKED);
		rc = true;
	}

	cnt = GetPrivateProfileString(generalSection, g_ModelTag, _T(""), tmp, sizeof(tmp), oeminfoFileName);
	if (cnt)
	{
		CString szTemp;
		szTemp = tmp;

		if (szTemp.Left(4) == g_SVIMTag)
		{
			m_type.SetCurSel(0);
		}
		else if (szTemp.Left(4) == g_SVPCTag)
		{
			m_type.SetCurSel(1);
		}
		else
		{
			m_type.SetCurSel(-1);
		}

		//		m_serviced_by.SetModify(false);
		rc = true;
	}

	return rc;
}

////////////////////////////////////////////////////////////////////////////////////////
// Get the sysprep.inf product ID info
////////////////////////////////////////////////////////////////////////////////////////
bool SVDriveInitDlg::GetSysPrepInfo()
{
	for (int i = 0; i < cCdKeyBlockNr; ++i)
	{
		m_cdkey[i].SetWindowText(m_CDKey.GetSubElement(i));
		m_cdkey[i].SetModify(false);
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////
// Update the Registry
////////////////////////////////////////////////////////////////////////////////////////
bool SVDriveInitDlg::UpdateRegistryInfo()
{
	bool l_bOk = true;

	// Check if model number has been entered
	if (m_model_number.GetModify())
	{
		CString l_csModel;

		CString l_csFrameGrabber;
		CString l_csIOBoard;

		m_model_number.GetWindowText(l_csModel);

		l_csModel.MakeUpper();

		for (int i = 0, j = 0; i < l_csModel.GetLength() && j < 7; i++)
		{
			if (::isalnum(l_csModel[i]))
			{
				switch (++j)
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
		TCHAR *l_pszName = l_csFrameGrabberName.GetBuffer(TmpBufSize);
		GetPrivateProfileString(g_FrameGrabberTag, l_csFrameGrabber, _T(""), l_pszName, TmpBufSize, SvStl::GlobalPath::Inst().GetHardwareIniPath());
		l_csFrameGrabberName.ReleaseBuffer();

		if (0 < l_csFrameGrabberName.GetLength())
		{
			CString l_csFrameGrabberDLL;
			TCHAR *l_pszDLL = l_csFrameGrabberDLL.GetBuffer(TmpBufSize);
			GetPrivateProfileString(l_csFrameGrabberName, g_DigitizerDLLTag, _T(""), l_pszDLL, TmpBufSize, SvStl::GlobalPath::Inst().GetHardwareIniPath());
			l_csFrameGrabberDLL.ReleaseBuffer();

			l_bOk = (S_OK == UpdateMatrox(l_csFrameGrabberName));

			if (l_csFrameGrabberDLL.CompareNoCase(g_SVMatroxGigeDLLTag) >= 0)
			{
				AddCommandBeforeSVObserver(g_NetStartMatroxGigECmd, false);
			}
			else
			{
				AddCommandBeforeSVObserver(g_NetStartMatroxGigECmd, true);
			}
		}

		CString l_csIOBoardName;
		TCHAR *l_pszBoardName = l_csIOBoardName.GetBuffer(TmpBufSize);
		GetPrivateProfileString(g_IOBoardTag, l_csIOBoard, _T(""), l_pszBoardName, TmpBufSize, SvStl::GlobalPath::Inst().GetHardwareIniPath());
		l_csIOBoardName.ReleaseBuffer();

		if (0 < l_csIOBoardName.GetLength())
		{
			CString l_csIOBoardDLL;
			TCHAR *l_pszDLL = l_csIOBoardDLL.GetBuffer(TmpBufSize);
			GetPrivateProfileString(l_csIOBoardName, g_DigitalIODLLTag, _T(""), l_pszDLL, TmpBufSize, SvStl::GlobalPath::Inst().GetHardwareIniPath());
			l_csIOBoardDLL.ReleaseBuffer();

			l_bOk &= (S_OK == UpdateIOInterfaceDLL(l_csIOBoardDLL));
		}
	}
	return l_bOk;
}

////////////////////////////////////////////////////////////////////////////////////////
// Update the OEMSpecific Info in the oeminfo.ini file
////////////////////////////////////////////////////////////////////////////////////////
bool SVDriveInitDlg::UpdateOEMInfo()
{
	TCHAR tmp[81];
	CString szTemp, szModel, szSerial, szType;

	// Check if Serial number has been entered
	if (m_serial_number.GetModify())
	{
		m_serial_number.GetWindowText(tmp, sizeof(tmp));
		if (false == WritePrivateProfileString(oemSection, g_SerialNoTag, tmp, oeminfoFileName))
		{
			return false;
		}
	}

	// Check if Date Shipped has been entered
	if (m_date.GetModify())
	{
		m_date.GetWindowText(tmp, sizeof(tmp));
		if (false == WritePrivateProfileString(oemSection, g_DateTag, tmp, oeminfoFileName))
		{
			return false;
		}
	}

	// Check if model number has been entered
	if (m_model_number.GetModify())
	{
		m_model_number.GetWindowText(tmp, sizeof(tmp));
		if (false == WritePrivateProfileString(oemSection, g_ModelNoTag, tmp, oeminfoFileName))
		{
			return false;
		}
	}

	// Check if Serviced By has been entered
	if (m_serviced_by.GetModify())
	{
		m_serviced_by.GetWindowText(tmp, sizeof(tmp));
		if (false == WritePrivateProfileString(oemSection, g_ServicedByTag, tmp, oeminfoFileName))
		{
			return false;
		}
	}

	if (false == WritePrivateProfileString(oemSection, g_SingleCameraTag, (BST_CHECKED == m_SingleCamera.GetCheck()) ? _T("Y") : _T("N"), oeminfoFileName))
	{
		return false;
	}

	m_type.GetWindowText(szType);
	m_serial_number.GetWindowText(szSerial);
	m_model_number.GetWindowText(szModel);

	szTemp = szType + _T(" ") + szSerial + _T(" ") + szModel;
	if (false == WritePrivateProfileString(generalSection, g_ModelTag, szTemp, oeminfoFileName))
	{
		return false;
	}
	return true;
}

bool SVDriveInitDlg::UpdateSVIMInfo()
{
	//	TCHAR tmp[SVIMINFO_SIZE + 1];
	bool rc = true;

	return rc;
}

////////////////////////////////////////////////////////////////////////////////////////
// Update sysprep.inf info
////////////////////////////////////////////////////////////////////////////////////////
bool SVDriveInitDlg::UpdateSysPrepInfo()
{
	for (int i = 0; i < cCdKeyBlockNr; ++i)
	{
		if (m_cdkey[i].GetModify())
		{
			CString CdkeyPart;
			m_cdkey[i].GetWindowText(CdkeyPart);
			m_CDKey.SetSubElement(i, CdkeyPart);

		}
	}

	return true;
}

void SVDriveInitDlg::OnCancel()
{
	CDialog::OnCancel();
}

void SVDriveInitDlg::OnChangeCdkey(UINT nID)
{
	int index(nID - ID_CDKEY1);

	if (0 <= index && cCdKeyBlockNr > index)
	{
		CEdit& rEdit = m_cdkey[index];
		int nStartChar(0);
		int nEndChar(0);
		rEdit.GetSel(nStartChar, nEndChar);

		CWnd* pWnd = GetNextDlgTabItem(reinterpret_cast<CWnd*>(&rEdit), false);
		if (pWnd && (PRODUCTID_PART_SIZE == nStartChar) && (PRODUCTID_PART_SIZE == nEndChar))
		{
			pWnd->SetFocus();
			reinterpret_cast<CEdit*>(pWnd)->SetSel(0, -1);
		}
	}
}

HRESULT SVDriveInitDlg::BackupBootIni()
{
	HRESULT l_Status = S_OK;
	CString l_NewBootName;

	for (int i = 0; i < 1000; ++i)
	{
		l_NewBootName.Format(g_bootIniBackupFilepath, i);

		if (0 == ::_access(l_NewBootName, 0))
		{
			//get the file attributes
			DWORD l_Attr = GetFileAttributes(l_NewBootName);

			//remove the read only flag
			l_Attr = l_Attr & ~FILE_ATTRIBUTE_READONLY;

			//set the file to read only
			SetFileAttributes(l_NewBootName, l_Attr);

			if (i < 1 || 9 < i)
			{
				::remove(l_NewBootName);
			}
			else
			{
				CString l_ToBootName;
				l_ToBootName.Format(g_bootIniBackupFilepath, i - 1);
				::rename(l_NewBootName, l_ToBootName);
			}
		}
		else
		{
			if (9 < i)
			{
				l_NewBootName.Format(g_bootIniBackupFilepath, 9);
			}
			break;
		}
	}

	//copy boot.ini -> boot.xxx
	CopyFile(g_bootIniFilepath, l_NewBootName, false);

	DWORD l_Attr = GetFileAttributes(l_NewBootName);

	//remove the read only flag
	l_Attr = l_Attr & ~FILE_ATTRIBUTE_READONLY;

	//set the file to read only
	SetFileAttributes(l_NewBootName, l_Attr);

	return l_Status;
}

HRESULT SVDriveInitDlg::UpdateIOInterfaceDLL(LPCTSTR p_szIOBoard)
{
	HRESULT l_Status = S_OK;

	CString l_IOBoardDLL = p_szIOBoard;

	std::deque< std::string > l_FileData;

	std::ifstream l_CmdFile;

	l_CmdFile.open(g_svobserver1CmdFilepath);

	if (l_CmdFile.is_open())
	{
		while (!l_CmdFile.eof())
		{
			std::string l_Temp;

			std::getline(l_CmdFile, l_Temp);

			if (!l_Temp.empty())
			{
				l_FileData.push_back(l_Temp);
			}
		}
		l_CmdFile.close();
	}

	std::deque<std::string>::iterator l_Iter = l_FileData.begin();

	while (l_Iter != l_FileData.end())
	{
		CString l_Temp = l_Iter->c_str();

		if (0 == l_Temp.CompareNoCase(g_InitializeIOSubsystemFilepath))
		{
			break;
		}
		++l_Iter;
	}

	if (0 == l_IOBoardDLL.Compare(g_SVLptIODllFilename))
	{
		if (l_Iter == l_FileData.end())
		{
			l_FileData.insert(l_FileData.begin(), g_InitializeIOSubsystemFilepath);
		}
	}
	else
	{
		if (l_Iter != l_FileData.end())
		{
			l_FileData.erase(l_Iter);
		}
	}

	std::ofstream l_WriteCmdFile;

	l_WriteCmdFile.open(g_svobserver1CmdFilepath, std::ios_base::out | std::ios_base::trunc);

	if (l_WriteCmdFile.is_open())
	{
		l_Iter = l_FileData.begin();

		while (l_Iter != l_FileData.end())
		{
			l_WriteCmdFile << *l_Iter << std::endl;
			++l_Iter;
		}
		l_WriteCmdFile.close();
	}
	return l_Status;
}

HRESULT SVDriveInitDlg::UpdateMatrox(LPCTSTR p_szDigitizer)
{
	HRESULT l_Status = S_OK;

	int l_Size = GetPrivateProfileInt(p_szDigitizer, g_MatroxReserveMemorySizeTag, 0, SvStl::GlobalPath::Inst().GetHardwareIniPath());

	if (0 < l_Size)
	{
		CString l_From;
		CString l_To;

		l_From.Format(g_mtxgigeSVR, windowsDriverPath);
		l_To.Format(g_mtxgigeSYS, windowsDriverPath);

		if (-1 == ::_access(l_To, 0))
		{
			::rename(l_From, l_To);
		}

		l_From.Format(g_mtxgigefilterSVR, windowsDriverPath);
		l_To.Format(g_mtxgigefilterSYS, windowsDriverPath);

		if (-1 == ::_access(l_To, 0))
		{
			::rename(l_From, l_To);
		}
	}
	else
	{
		CString l_From;
		CString l_To;

		l_Status = RemoveMatroxBootIniMaxMem();

		l_From.Format(g_mtxgigeSYS, windowsDriverPath);
		l_To.Format(g_mtxgigeSVR, windowsDriverPath);

		if (0 == ::_access(l_From, 0))
		{
			::rename(l_From, l_To);
		}

		l_From.Format(g_mtxgigefilterSYS, windowsDriverPath);
		l_To.Format(g_mtxgigefilterSVR, windowsDriverPath);

		if (0 == ::_access(l_From, 0))
		{
			::rename(l_From, l_To);
		}
	}
	return l_Status;
}

HRESULT SVDriveInitDlg::UpdateMatroxRegistryMaxMem(size_t& p_rMaxSize, size_t p_MILSize)
{
	HRESULT l_Status = S_OK;

	HKEY l_hKey = nullptr;
	size_t l_MaxSize = 0;

	l_Status = RegOpenKeyEx(HKEY_LOCAL_MACHINE, g_MtxDmaParamRegKey, 0, KEY_ALL_ACCESS, &l_hKey);

	if (ERROR_SUCCESS == l_Status)
	{
		DWORD l_Size = 0;
		DWORD l_Type = 0;

		l_Status = RegQueryValueEx(l_hKey, g_HighMemoryAreaReservedStartTag, nullptr, &l_Type, nullptr, &l_Size);

		if (ERROR_SUCCESS == l_Status)
		{
			if (REG_DWORD == l_Type)
			{
				l_Size = sizeof(l_MaxSize);

				l_Status = RegQueryValueEx(l_hKey, g_HighMemoryAreaReservedStartTag, nullptr, &l_Type, reinterpret_cast<LPBYTE>(&l_MaxSize), &l_Size);
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
	}

	if (ERROR_SUCCESS == l_Status)
	{
		size_t l_SizeInMB = p_MILSize << 20;

		HRESULT l_Temp = RegSetValueEx(l_hKey, g_DmaBufferSizeTag, 0, REG_DWORD, reinterpret_cast<unsigned char *>(&p_MILSize), sizeof(p_MILSize));

		l_Status = RegSetValueEx(l_hKey, g_HighMemoryAreaReservedSizeTag, 0, REG_DWORD, reinterpret_cast<unsigned char *>(&l_SizeInMB), sizeof(l_SizeInMB));

		if (ERROR_SUCCESS != l_Temp)
		{
			l_Status = l_Temp;
		}

		RegFlushKey(l_hKey);
		RegCloseKey(l_hKey);

		l_hKey = nullptr;

		l_MaxSize -= l_SizeInMB;
	}

	if (ERROR_SUCCESS == l_Status)
	{
		p_rMaxSize = l_MaxSize >> 20;
	}
	else
	{
		p_rMaxSize = 0;
	}

	return l_Status;
}

HRESULT SVDriveInitDlg::RemoveMatroxBootIniMaxMem()
{
	HRESULT l_Status = BackupBootIni();

	if (S_OK == l_Status)
	{
		DWORD l_Attr = GetFileAttributes(g_bootIniFilepath);

		//remove the read only flag
		l_Attr = l_Attr & ~FILE_ATTRIBUTE_READONLY;

		//set the file to read only
		SetFileAttributes(g_bootIniFilepath, l_Attr);

		CString l_Default;

		TCHAR* l_pszDefault = l_Default.GetBuffer(TmpBufSize);
		GetPrivateProfileString(g_bootLoaderTag, g_defaultTag, _T(""), l_pszDefault, TmpBufSize, g_bootIniFilepath);
		l_Default.ReleaseBuffer();

		if (0 < l_Default.GetLength())
		{
			CString l_Option;

			TCHAR* l_pszOption = l_Option.GetBuffer(TmpBufSize);
			GetPrivateProfileString(g_OperatingSystemsTag, l_Default, _T(""), l_pszOption, TmpBufSize, g_bootIniFilepath);
			l_Option.ReleaseBuffer();

			if (0 < l_Option.GetLength())
			{
				if (-1 != l_Option.Find(g_MaxMemTag))
				{
					int l_MemoryPos = l_Option.Find(g_MemoryNTTag);
					int l_MaxMemPos = l_Option.Find(g_MaxMemTag);

					if (l_MemoryPos == -1)
					{
						l_MemoryPos = l_Option.Find(_T('\"'), 1);
					}

					if (l_MemoryPos != -1)
					{
						while (0 <= l_MemoryPos && l_Option[l_MemoryPos - 1] == _T(' '))
						{
							--l_MemoryPos;
						}
					}

					if (l_MemoryPos != -1 && l_MaxMemPos != -1)
					{
						CString l_NewOption;
						size_t l_MaxSize = 0;

						int l_MaxMemEndSpacePos = l_Option.Find(' ', l_MaxMemPos + 8);
						int l_MaxMemEndSlashPos = l_Option.Find('/', l_MaxMemPos + 8);

						if (l_MaxMemEndSpacePos == -1 && l_MaxMemEndSlashPos == -1)
						{
							l_NewOption.Format(_T("%s\""), l_Option.Left(l_MemoryPos).GetString());
						}
						else if (l_MaxMemEndSpacePos == -1)
						{
							l_NewOption.Format(_T("%s\" %s"), l_Option.Left(l_MemoryPos).GetString(), l_Option.Mid(l_MaxMemEndSlashPos).GetString());
						}
						else if (l_MaxMemEndSlashPos == -1)
						{
							l_NewOption.Format(_T("%s\"%s"), l_Option.Left(l_MemoryPos).GetString(), l_Option.Mid(l_MaxMemEndSpacePos).GetString());
						}
						else
						{
							if (l_MaxMemEndSpacePos < l_MaxMemEndSlashPos)
							{
								l_NewOption.Format(_T("%s\"%s"), l_Option.Left(l_MemoryPos).GetString(), l_Option.Mid(l_MaxMemEndSpacePos).GetString());
							}
							else
							{
								l_NewOption.Format(_T("%s\" %s"), l_Option.Left(l_MemoryPos).GetString(), l_Option.Mid(l_MaxMemEndSlashPos).GetString());
							}
						}

						WritePrivateProfileString(g_OperatingSystemsTag, l_Default, l_NewOption, g_bootIniFilepath);

						l_Status = UpdateMatroxRegistryMaxMem(l_MaxSize, 0);
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

		l_Attr = GetFileAttributes(g_bootIniFilepath);

		//remove the read only flag
		l_Attr = l_Attr | FILE_ATTRIBUTE_READONLY;

		//set the file to read only
		SetFileAttributes(g_bootIniFilepath, l_Attr);
	}
	return l_Status;
}

HRESULT SVDriveInitDlg::AddCommandBeforeSVObserver(LPCTSTR p_strNewCommand, bool p_bRemove = false)
{
	HRESULT l_Status = S_OK;

	std::deque<std::string> l_FileData;

	std::ifstream l_CmdFile;

	l_CmdFile.open(g_svobserver1CmdFilepath);

	// Read in the file.
	if (l_CmdFile.is_open())
	{
		while (!l_CmdFile.eof())
		{
			std::string l_Temp;

			std::getline(l_CmdFile, l_Temp);

			if (!l_Temp.empty())
			{
				l_FileData.push_back(l_Temp);
			}
		}
		l_CmdFile.close();
	}

	std::deque<std::string>::iterator l_Iter = l_FileData.begin();

	// Find the line containing the new line or SVObserver.exe.
	while (l_Iter != l_FileData.end())
	{
		CString l_Temp = l_Iter->c_str();
		if (l_Temp.Find(p_strNewCommand) >= 0)
		{
			if (p_bRemove)
			{
				l_FileData.erase(l_Iter);
				break;
			}
			else
			{	// No need to do anything else. It is already here.
				return S_OK;
			}
		}

		if (l_Temp.Find(g_SVObserverExeTag) >= 0)
		{
			if (!p_bRemove)
			{
				l_FileData.insert(l_Iter, p_strNewCommand);
			}
			break;
		}

		++l_Iter;
	}

	std::ofstream l_WriteCmdFile;

	l_WriteCmdFile.open(g_svobserver1CmdFilepath, std::ios_base::out | std::ios_base::trunc);

	if (l_WriteCmdFile.is_open())
	{
		l_Iter = l_FileData.begin();

		while (l_Iter != l_FileData.end())
		{
			l_WriteCmdFile << *l_Iter << std::endl;
			++l_Iter;
		}
		l_WriteCmdFile.close();
	}
	return l_Status;
}

void DisplayLastSystemError()
{
	CString szTemp, szText;
	DWORD dwError = GetLastSystemErrorText(szText);
	szTemp.Format(_T("(%0X) %s"), dwError, szText.GetString());
	MessageBox(nullptr, szTemp, _T("ERROR"), MB_OK);
}

DWORD GetLastSystemErrorText(CString & szMsg)
{
	DWORD dwError = GetLastError();
	GetSystemErrorText(dwError, szMsg);
	return dwError;
}

HRESULT GetSystemErrorText(DWORD dwError, CString & szMsg)
{
	LPVOID lpMsgBuf;
	if (0 == FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		reinterpret_cast<LPTSTR>(&lpMsgBuf),
		0,
		nullptr))
		return S_FALSE;

	szMsg.Format(_T("%s"), lpMsgBuf);
	LocalFree(lpMsgBuf);
	return S_OK;
}

