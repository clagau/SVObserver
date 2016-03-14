//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
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
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"


// SVLicenseMgrModelessDlg dialog

BEGIN_MESSAGE_MAP(SVLicenseMgrModelessDlg, CDialog)
	//{{AFX_MSG_MAP(SVLicenseMgrModelessDlg)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVLicenseMgrModelessDlg& SVLicenseMgrModelessDlg::Instance()
{
	static SVLicenseMgrModelessDlg l_Object;

	return l_Object;
}

SVLicenseMgrModelessDlg::SVLicenseMgrModelessDlg(): CDialog(SVLicenseMgrModelessDlg::IDD, NULL),m_hEvent(NULL)
{
	
}

void SVLicenseMgrModelessDlg::Init(const CString& s,const SVLicenseErrorListSet& p_sList, HANDLE p_hEvent)
{
	m_sMsg = s;

	DuplicateHandle( GetCurrentProcess(), p_hEvent,
		GetCurrentProcess(), &m_hEvent,
		0, FALSE, DUPLICATE_SAME_ACCESS );

	m_sList = p_sList;
}

void SVLicenseMgrModelessDlg::Show(const CString& s,const SVLicenseErrorListSet& p_sList, HANDLE p_hEvent)
{
	SVLicenseMgrModelessDlg& l_rDlg = SVLicenseMgrModelessDlg::Instance();
	l_rDlg.Init(s,p_sList,p_hEvent);
	l_rDlg.Create(SVLicenseMgrModelessDlg::IDD, NULL);
}

void SVLicenseMgrModelessDlg::Destroy()
{
	SVLicenseMgrModelessDlg& l_rDlg = SVLicenseMgrModelessDlg::Instance();

	l_rDlg.DestroyWindow();
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
	if( m_hEvent != NULL )
	{
		::ResetEvent( m_hEvent );
		::CloseHandle( m_hEvent );

		m_hEvent = NULL;
	}

	m_sMsg.Empty();
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
		ASSERT(FALSE);
	}
}

BOOL SVLicenseMgrModelessDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetDlgItem(IDC_STATIC_TITLE)->SetWindowText(m_sMsg);

	//fill ListControl with items in m_dList
	SVLicenseErrorListSet::iterator l_Iter = m_sList.begin();

	while ( l_Iter != m_sList.end() )
	{
		SVObjectClass* pObj = SVObjectManagerClass::Instance().GetObject( *l_Iter );
		if ( pObj )
		{
			m_ctlErrorList.AddString(pObj->GetCompleteObjectName());
		}
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

		pText->SetWindowPos(NULL, 0,0, mycx, mycy, SWP_NOMOVE | SWP_NOZORDER);
	}
	
	CDialog::OnSize(nType, cx, cy);
	Invalidate();
}


