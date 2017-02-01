//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file RRSViewDlg.cpp 
/// All Rights Reserved 
//*****************************************************************************

/// These files contain your CShareViewDlg class.  This class defines
//the behavior of your application's main dialog.  The dialog's template is
//in ShareView.rc, which can be edited in Microsoft Visual C++. 
//******************************************************************************

#include "stdafx.h"
#include "RRSView.h"
#include "RRSViewDlg.h"
#include "SVSharedMemoryLibrary\ShareEvents.h"
#include "SVSharedMemoryLibrary\MonitorListCpy.h"
#include "MonitorLIstDlg.h"
//Moved to precompiled header: #include <boost/function.hpp> 
//Moved to precompiled header:  #include <boost/bind.hpp>

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




CRRSViewDlg::CRRSViewDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRRSViewDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRRSViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_READY_VALUE, m_ReadyValueCtrl);
	DDX_Control(pDX, IDC_LIST_MONLISTS, m_MonListsCtrl);
}

BEGIN_MESSAGE_MAP(CRRSViewDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_REFRESH, OnRefresh)

	ON_BN_CLICKED(IDC_BUTTON_SHOW, &CRRSViewDlg::OnBnClickedButtonShow)
END_MESSAGE_MAP()


LPCTSTR CRRSViewDlg::ColHeader[] = {_T("Name"),_T("PPQ"), _T("IsActive"), _T("RejectDepth"), _T("Filter"), _T("PCount")};
// CShareViewDlg message handlers

bool  CRRSViewDlg::PostRefresh(DWORD par)
{
	PostMessage(WM_REFRESH,par,0);
	return true;

}

BOOL CRRSViewDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}


	boost::function<bool (DWORD)> f =  boost::bind(&CRRSViewDlg::PostRefresh,this,_1);
	SvSml::ShareEvents::GetInstance().SetCallbackFunction(f);
	SvSml::ShareEvents::GetInstance().StartWatch();

	
	for(int c =0;c<= ePcount; c++)
	{
		int ret =	m_MonListsCtrl.InsertColumn(c, ColHeader[c],LVCFMT_LEFT,80);
		int debug = ret;
	}
	
	
	PostRefresh(SvSml::ShareEvents::UKnown);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRRSViewDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CRRSViewDlg::OnPaint()
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
 HCURSOR CRRSViewDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT  CRRSViewDlg::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	bool isready = SvSml::ShareEvents::GetInstance().GetIsReady() ;
	if(isready)
	{
		SvSml::MonitorListCpyHelper::ReloadMonitorMap(m_mlReader,  m_monitorMap);
		m_MonListsCtrl.DeleteAllItems();
		SvSml::MonitorListCpyMap::const_iterator it;  
		int item(0);
		for(it = m_monitorMap.begin(); it !=  m_monitorMap.end(); ++it)
		{
			m_MonListsCtrl.InsertItem(item, it->second.m_name.c_str());
		
			//enum ECOL {eName=0, ePPQ, eIsActive, eRejectDepth, eProductFilter,ePcount };
			m_MonListsCtrl.SetItemText(item,ePPQ,it->second.m_ppq.c_str() );
			
			SVString text = it->second.m_IsActive? _T("true") : _T("false");
			m_MonListsCtrl.SetItemText(item,eIsActive,text.c_str() );

			text = SvUl_SF::Format(_T("%i"),it->second.m_rejectDepth );
			m_MonListsCtrl.SetItemText(item,eRejectDepth,text.c_str());

			text = SvUl_SF::Format(_T("%i"),it->second.m_ProductFilter );
			m_MonListsCtrl.SetItemText(item,eProductFilter,text.c_str() );
			
			text = SvUl_SF::Format(_T("%i"),it->second.prodItems.size() );
			m_MonListsCtrl.SetItemText(item,ePcount,text.c_str() );
			item++;
		}
	
	}
	

	CString csready = isready ? _T("true") : _T("false");
	m_ReadyValueCtrl.SetWindowText(csready);
	
	

	return 0;
}


void CRRSViewDlg::OnBnClickedButtonShow()
{
	
	if(m_MonListsCtrl.GetItemCount() >0 )
	{
		MonitorListDlg dlg;
		dlg.DoModal();
	}
	
}
