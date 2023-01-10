//******************************************************************************
//* COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOLicenseMgrModelessDlg
//* .File Name       : $Workfile:   SVLicenseMgrModelessDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   06 May 2013 14:38:18  $
//******************************************************************************

#include "stdafx.h"
#include "SVLicenseMgrModelessDlg.h"


// SVLicenseMgrModelessDlg dialog

BEGIN_MESSAGE_MAP(SVLicenseMgrModelessDlg, CDialog)
	//{{AFX_MSG_MAP(SVLicenseMgrModelessDlg)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVLicenseMgrModelessDlg& SVLicenseMgrModelessDlg::Instance()
{
	static SVLicenseMgrModelessDlg Object;

	return Object;
}

SVLicenseMgrModelessDlg::SVLicenseMgrModelessDlg(): CDialog(SVLicenseMgrModelessDlg::IDD, nullptr), m_hEvent(nullptr)
{
}


void SVLicenseMgrModelessDlg::Init(const std::string& rMsg, const std::set<std::string>& rList, HANDLE hEvent)
{
	m_Msg = rMsg;
	m_sNameList = rList;

	DuplicateHandle(GetCurrentProcess(), hEvent, GetCurrentProcess(), &m_hEvent, 0, FALSE, DUPLICATE_SAME_ACCESS);
}


void SVLicenseMgrModelessDlg::Show(const std::string& rMsg, const std::set<std::string>& rList, HANDLE hEvent)
{
	SVLicenseMgrModelessDlg& rDlg = SVLicenseMgrModelessDlg::Instance();
	rDlg.Init(rMsg, rList, hEvent);
	rDlg.Create(SVLicenseMgrModelessDlg::IDD, nullptr);
}

void SVLicenseMgrModelessDlg::Destroy()
{
	SVLicenseMgrModelessDlg& rDlg = SVLicenseMgrModelessDlg::Instance();

	rDlg.DestroyWindow();
}



SVLicenseMgrModelessDlg::~SVLicenseMgrModelessDlg()
{
	CleanUp();
}

void SVLicenseMgrModelessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVLicenseMgrModelessDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Text(pDX, IDC_STATIC_TITLE, m_sMessageTitle);
	DDX_Control(pDX, IDC_LST_TOOL_ERRORS, m_ctlErrorList);
	//}}AFX_DATA_MAP
}

void SVLicenseMgrModelessDlg::CleanUp()
{
	if( nullptr != m_hEvent )
	{
		::ResetEvent( m_hEvent );
		::CloseHandle( m_hEvent );

		m_hEvent = nullptr;
	}

	m_Msg.clear();
}

// SVLicenseMgrModelessDlg message handlers
void SVLicenseMgrModelessDlg::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
	try
	{
		CleanUp();
	}
	catch (...)
	{
		// we tried to delete something that was already deleted
		// or created on the stack.
		// or an invalid pointer
		Log_Assert(FALSE);
	}
}

BOOL SVLicenseMgrModelessDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetDlgItem(IDC_STATIC_TITLE)->SetWindowText(m_Msg.c_str());

	//fill ListControl with items in m_dList
	auto l_Iter = m_sNameList.begin();
	
	while (l_Iter != m_sNameList.end())
	{
		m_ctlErrorList.AddString(l_Iter->c_str());
		++l_Iter;
	}

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVLicenseMgrModelessDlg::OnOK() 
{
	DestroyWindow();
}

void SVLicenseMgrModelessDlg::OnSize(UINT nType, int cx, int cy) 
{
	DialogUnits du;
	CWnd* pOK = GetDlgItem(IDOK);
	CWnd* pText = GetDlgItem(IDC_STATIC_TITLE);

	if (pOK && pOK->m_hWnd && pText && pText->m_hWnd)
	{

		CRect rectText;

		pText->GetWindowRect(&rectText);

		ScreenToClient(&rectText);


		int x,y, mycx,mycy;
		// Position the OK button
		y = cy - du.Y(15);
		x = (cx - du.Y(40)) / 2;
		pOK->SetWindowPos(&wndTop, x,y, 0,0, SWP_NOSIZE);

		// resize the text box
		mycx = cx - 20;
		mycy = y/*OK button Y*/ -10 - rectText.top;

		pText->SetWindowPos(nullptr, 0, 0, mycx, mycy, SWP_NOMOVE | SWP_NOZORDER);
	}
	
	CDialog::OnSize(nType, cx, cy);
	Invalidate();
}


