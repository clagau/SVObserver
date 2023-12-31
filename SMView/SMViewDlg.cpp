//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file SMViewDlg.cpp 
/// All Rights Reserved 
//*****************************************************************************

/// These files contain your CShareViewDlg class.  This class defines
//the behavior of your application's main dialog.  The dialog's template is
//in ShareView.rc, which can be edited in Microsoft Visual C++. 
//******************************************************************************

#include "stdafx.h"
#include "SMViewDlg.h"
#include "SVSharedMemoryLibrary\ShareEvents.h"
#include "SVSharedMemoryLibrary\MonitorListCpy.h"
#include "MonitorLIstDlg.h"
#include "SVSharedMemoryLibrary\MLProduct.h"
#include "ProductDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_REFRESH (WM_USER + 100)

// CAboutDlg dialog used for App About
class CAboutDlg : public CDialogEx
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

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CShareViewDlg dialog




SMViewDlg::SMViewDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(SMViewDlg::IDD, pParent)
{
}

void SMViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_READY_VALUE, m_ReadyValueCtrl);
	DDX_Control(pDX, IDC_LIST_MONLISTS, m_MonListsCtrl);
	DDX_Control(pDX, IDC_EDIT_TRIGGER, m_EditTrigger);
}

BEGIN_MESSAGE_MAP(SMViewDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_REFRESH, OnRefresh)

	ON_BN_CLICKED(IDC_BUTTON_SHOW, &SMViewDlg::OnBnClickedButtonShow)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &SMViewDlg::OnBnClickedButtonExit)
	ON_BN_CLICKED(IDC_BUT_GETPRODUCT, &SMViewDlg::OnBnClickedButGetproduct)
	ON_BN_CLICKED(IDC_BUT_GETFAILSTATUS, &SMViewDlg::OnBnClickedButGetfailstatus)
	ON_BN_CLICKED(IDC_BUT_REJECT, &SMViewDlg::OnBnClickedButReject)
END_MESSAGE_MAP()

//enum ECOL { eName = 0, ePPQ, eIsActive, eRejectDepth, eProductDepth, eProductFilter, eImageCount, ePcount, };
LPCTSTR SMViewDlg::ColHeader[] = { _T("Name"),_T("PPQ"), _T("IsActive"), _T("RejectDepth"), _T("ProductDepth"), _T("Filter"), _T("ImCount")};
// CShareViewDlg message handlers

bool  SMViewDlg::PostRefresh(DWORD par)
{
	PostMessage(WM_REFRESH, par, 0);
	return true;

}

BOOL SMViewDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	assert((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	assert(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		assert(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	m_EditTrigger.SetWindowText("-1");
	std::function<bool(DWORD)> functor = [this](DWORD param) { return PostRefresh(param); };
	SvSml::ShareEvents::GetInstance().SetCallbackFunction(functor);
	SvSml::ShareEvents::GetInstance().StartWatch();


	for (int c = 0; c <= ePcount; c++)
	{
		/*int ret = */m_MonListsCtrl.InsertColumn(c, ColHeader[c], LVCFMT_LEFT, 80);
	}


	PostRefresh(SvSml::ShareEvents::UKnown);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void SMViewDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void SMViewDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR SMViewDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void SMViewDlg::UpdateControls(bool isready)
{
	if (isready)
	{
		m_MonListsCtrl.DeleteAllItems();
		SvSml::MonitorListCpyMap::const_iterator it;
		int item(0);
		for (it = m_MemReader.m_MLContainer.m_MonitorListCpyMap.begin(); it != m_MemReader.m_MLContainer.m_MonitorListCpyMap.end(); ++it)
		{
			m_MonListsCtrl.InsertItem(item, it->second->m_MonitorListName.c_str());

			
			m_MonListsCtrl.SetItemText(item, ePPQ, it->second->m_ppqName.c_str());

			std::string text = it->second->GetIsActive() ? _T("true") : _T("false");
			m_MonListsCtrl.SetItemText(item, eIsActive, text.c_str());

			text = std::format(_T("{}"), it->second->m_rejectDepth);
			m_MonListsCtrl.SetItemText(item, eRejectDepth, text.c_str());

			text = std::format(_T("{}"), it->second->m_ProductDepth);
			m_MonListsCtrl.SetItemText(item, eProductDepth, text.c_str());

			text = std::format(_T("{}"), static_cast<int>(it->second->m_ProductFilter));
			m_MonListsCtrl.SetItemText(item, eProductFilter, text.c_str());


			//monitorListCpy.m_MonitorEntries[list]
			text = std::format(_T("{}"), it->second->m_MonitorEntries[SvSml::ListType::productItemsImage].size());
			m_MonListsCtrl.SetItemText(item, eImageCount, text.c_str());

			text = std::format(_T("{}"), it->second->m_MonitorEntries[SvSml::ListType::productItemsData].size());
			m_MonListsCtrl.SetItemText(item, ePcount, text.c_str());
			item++;
		}
	}
	CString csready = isready ? _T("true") : _T("false");
	m_ReadyValueCtrl.SetWindowText(csready);


}
LRESULT  SMViewDlg::OnRefresh(WPARAM wParam, LPARAM lParam)
{

	UNREFERENCED_PARAMETER(lParam);
	bool isready = (wParam == SvSml::ShareEvents::Ready);
	if (wParam == SvSml::ShareEvents::Ready)
	{
		try
		{
			DWORD version(0);
			m_MemReader.Reload(version);
		}
		catch (std::exception& rException)
		{
			UNREFERENCED_PARAMETER(rException);
			AfxMessageBox("Shared Mem Reader Reload failed");
		}
		
	}
	else if (wParam == SvSml::ShareEvents::Change)
	{
		m_MemReader.Clear();
	}
	UpdateControls(isready);
	return 0;
}


void SMViewDlg::OnBnClickedButtonShow()
{

	if (m_MonListsCtrl.GetItemCount() > 0)
	{
		MonitorListDlg dlg;
		dlg.DoModal();
	}

}

void SMViewDlg::OnBnClickedButtonExit()
{

	EndDialog(0);
}
void SMViewDlg::OnBnClickedButGetproduct()
{
	DisplayProduct(false);
}
void SMViewDlg::OnBnClickedButReject()
{
	DisplayProduct(true);
}


void SMViewDlg::DisplayProduct(bool isreject)
{
	int sel = m_MonListsCtrl.GetSelectionMark();
	if (sel < 0 || sel >= m_MonListsCtrl.GetItemCount())
	{
		return;
	}
	CString MonitorListName = m_MonListsCtrl.GetItemText(sel, 0);
	std::unique_ptr<SvSml::MLProduct> productPtr(new SvSml::MLProduct);
	CString text;
	m_EditTrigger.GetWindowTextA(text);
	int trigger = atoi(text.GetString());
	text.Format("%i", trigger);
	m_EditTrigger.SetWindowTextA(text.GetString());
	if (isreject)
	{
		m_MemReader.GetRejectData(MonitorListName.GetString(), trigger, productPtr.get(),true);
	}
	else
	{
		m_MemReader.GetProductData(MonitorListName.GetString(), trigger, productPtr.get(), true);
	}
	ProductDlg prod(productPtr.get());
	prod.DoModal();
}


void SMViewDlg::OnBnClickedButGetfailstatus()
{
	int sel = m_MonListsCtrl.GetSelectionMark();
	if (sel < 0 || sel >= m_MonListsCtrl.GetItemCount())
	{
		return;
	}
	CString MonitorListName = m_MonListsCtrl.GetItemText(sel, 0);
	SvSml::vecpProd Failstatus;
	m_MemReader.GetFailstatus(MonitorListName.GetString(), &Failstatus,nullptr );
	ProductDlg prod(&Failstatus);
	prod.DoModal();
}


