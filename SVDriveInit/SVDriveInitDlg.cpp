//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVDriveInitDlg.h"
#include "SVLibrary/InitialInformation.h"
#include "SVLibrary/SVOINILoader.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVSystemLibrary/SVVersionInfo.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr UINT PRODUCTID_PART_SIZE = 5;
constexpr UINT SERIALNUMBER_SIZE = 20;
constexpr UINT MODELNUMBER_SIZE = 8;

constexpr char oemSection[] = _T("OEMSpecific");

constexpr char g_SerialNoTag[] = _T("SerialNo");
constexpr char g_ModelNoTag[] = _T("ModelNo");

constexpr char g_ProductIdTag[] = _T("ProductId");

constexpr char g_ModelNumberNotValidMsg[] = _T("Model Number is not Valid.");
constexpr char g_ModelNumberNotValidMsgPrompt[] = _T("Model Number is not Valid.\nWould you like to correct the Model Number ?");

constexpr char g_SysprepUpdatefailedMsg[] = _T("Update System Prep Info Failed");
constexpr char g_OEMUpdateFailedMsg[] = _T("Update OEM Info Failed");
constexpr char g_CreateProcessFailedMsg[] = _T("Create Process Failed");

static TCHAR oeminfoFileName[_MAX_PATH];

DWORD GetLastSystemErrorText(CString & szMsg);///< also in SVDriveInitDlg.cpp
HRESULT GetSystemErrorText(DWORD dwError, CString & szMsg);///< also in SVDriveInitDlg.cpp
void DisplayLastSystemError();

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };
	virtual BOOL OnInitDialog() override;

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
protected:
	DECLARE_MESSAGE_MAP()

	CString m_Version {_T("SVDriveInit Version: ")};
};

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_VERSION, m_Version);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_Version += SvSyl::getSvoVersionAsString().c_str();
	UpdateData(false);

	return true;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(SVDriveInitDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CONTROL_RANGE(EN_CHANGE, ID_CDKEY1, ID_CDKEY5, OnChangeCdkey)
END_MESSAGE_MAP()

SVDriveInitDlg::SVDriveInitDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(SVDriveInitDlg::IDD, pParent)
{
}

void SVDriveInitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, ID_MODEL_NUMBER, m_modelNumber);
	DDX_Control(pDX, ID_CDKEY1, m_cdKey[0]);
	DDX_Control(pDX, ID_CDKEY2, m_cdKey[1]);
	DDX_Control(pDX, ID_CDKEY3, m_cdKey[2]);
	DDX_Control(pDX, ID_CDKEY4, m_cdKey[3]);
	DDX_Control(pDX, ID_CDKEY5, m_cdKey[4]);
	DDX_Control(pDX, ID_SERIAL_NUMBER, m_serialNumber);
}

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

	// Set Limits on Fields
	for (int i = 0; i < cCdKeyBlockNr; ++i)
	{
		m_cdKey[i].SetLimitText(PRODUCTID_PART_SIZE);
	}
	m_serialNumber.SetLimitText(SERIALNUMBER_SIZE);

	// Get the Windows Directory
	::GetSystemDirectory(oeminfoFileName, sizeof(oeminfoFileName));
	_tcscat(oeminfoFileName, _T("\\oeminfo.ini"));

	//get the file attributes
	DWORD dwAttr = GetFileAttributes(oeminfoFileName);

	//clear the read only flag
	dwAttr = dwAttr & ~FILE_ATTRIBUTE_READONLY;

	//set the file to read only
	SetFileAttributes(oeminfoFileName, dwAttr);

	GetSysPrepInfo();

	// Get Info from OEMINFO.ini
	GetOEMInfo();

	bool rc = IsModelNumberValidAndAreBoardNamesPresent();
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

	bool rc = IsModelNumberValidAndAreBoardNamesPresent();
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

	CDialog::OnOK();
}

bool SVDriveInitDlg::IsModelNumberValidAndAreBoardNamesPresent() const
{
	SvLib::InitialInformation initialInfo;
	SvLib::SVOIniLoader iniLoader(initialInfo);

	CString Model;
	m_modelNumber.GetWindowText(Model);
	Model.MakeUpper();

	iniLoader.DecodeModelNumber(static_cast<LPCTSTR> (Model));

	if (iniLoader.isModelNumberDecodable())
	{
		if (S_OK == iniLoader.LoadHardwareIni(SvStl::GlobalPath::Inst().GetHardwareIniPath()))
		{
			return iniLoader.areBoardNamesPresent();
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////
// Get the OEM Specific info from the oeminfo.ini file
////////////////////////////////////////////////////////////////////////////////////////
bool SVDriveInitDlg::GetOEMInfo()
{
	bool rc = false;

	TCHAR tmp[SERIALNUMBER_SIZE + 1];
	std::string serialNumber;
	DWORD cnt = GetPrivateProfileString(oemSection, g_SerialNoTag, _T(""), tmp, sizeof(tmp), oeminfoFileName);
	if (SvLib::readSerialNumberFromSystem(serialNumber))
	{
		cnt = static_cast<DWORD>(serialNumber.size());
		if (0 < cnt && SERIALNUMBER_SIZE > cnt)
		{
			strcpy_s(tmp, serialNumber.c_str());
		}
		m_serialNumber.SetReadOnly(true);
	}
	if (cnt)
	{
		m_serialNumber.SetWindowText(tmp);
		m_serialNumber.SetModify(false);
		rc = true;
	}

	cnt = GetPrivateProfileString(oemSection, g_ModelNoTag, _T(""), tmp, sizeof(tmp), oeminfoFileName);
	if (cnt)
	{
		m_modelNumber.SetWindowText(tmp);
		m_modelNumber.SetModify(false);
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
		m_cdKey[i].SetWindowText(m_CDKey.GetSubElement(i));
		m_cdKey[i].SetModify(false);
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////
// Update the OEMSpecific Info in the oeminfo.ini file
////////////////////////////////////////////////////////////////////////////////////////
bool SVDriveInitDlg::UpdateOEMInfo()
{
	TCHAR tmp[81];
	CString szTemp, szModel, szSerial, szType;

	// Check if Serial number has been entered
	if (m_serialNumber.GetModify())
	{
		m_serialNumber.GetWindowText(tmp, sizeof(tmp));
		if (false == WritePrivateProfileString(oemSection, g_SerialNoTag, tmp, oeminfoFileName))
		{
			return false;
		}
	}

	// Check if model number has been entered
	if (m_modelNumber.GetModify())
	{
		m_modelNumber.GetWindowText(tmp, sizeof(tmp));
		if (false == WritePrivateProfileString(oemSection, g_ModelNoTag, tmp, oeminfoFileName))
		{
			return false;
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////
// Update sysprep.inf info
////////////////////////////////////////////////////////////////////////////////////////
bool SVDriveInitDlg::UpdateSysPrepInfo()
{
	for (int i = 0; i < cCdKeyBlockNr; ++i)
	{
		if (m_cdKey[i].GetModify())
		{
			CString CdkeyPart;
			m_cdKey[i].GetWindowText(CdkeyPart);
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
		CEdit& rEdit = m_cdKey[index];
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

