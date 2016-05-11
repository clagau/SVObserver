// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : CModelessMsgBox
// * .File Name       : $Workfile:   ModelessMessageBox.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 09:14:20  $
// ******************************************************************************

#include "stdafx.h"
#include "ModelessMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CModelessMsgBox, CDialog)
	//{{AFX_MSG_MAP(CModelessMsgBox)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CModelessMsgBox::CModelessMsgBox(const CString& s, HANDLE p_hEvent)
	: CDialog(CModelessMsgBox::IDD, nullptr)
{
	m_sMsg = s;

	DuplicateHandle( GetCurrentProcess(), p_hEvent,
		GetCurrentProcess(), &m_hEvent,
		0, FALSE, DUPLICATE_SAME_ACCESS );
}

CModelessMsgBox::~CModelessMsgBox()
{
	if( nullptr != m_hEvent )
	{
		::ResetEvent( m_hEvent );
		::CloseHandle( m_hEvent );

		m_hEvent = nullptr;
	}

	m_sMsg.Empty();
}

void CModelessMsgBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModelessMsgBox)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

void CModelessMsgBox::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
	try
	{
		delete this;
	}
	catch (...)
	{
		// we tried to delete something that was already deleted
		// or created on the stack.
		// or an invalid pointer
		ASSERT(FALSE);
	}
}

BOOL CModelessMsgBox::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetDlgItem(IDC_MODELESS_MESSAGE_BOX_TEXT)->SetWindowText(m_sMsg);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CModelessMsgBox::OnOK() 
{
	DestroyWindow();
}

void CModelessMsgBox::OnCancel()
{
	DestroyWindow();
}

void CModelessMsgBox::OnSize(UINT nType, int cx, int cy) 
{
	DialogUnits du;
	CWnd* pOK = GetDlgItem(IDOK);
	CWnd* pText = GetDlgItem(IDC_MODELESS_MESSAGE_BOX_TEXT);

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

		pText->SetWindowPos(nullptr, 0,0, mycx, mycy, SWP_NOMOVE | SWP_NOZORDER);
	}
	
	CDialog::OnSize(nType, cx, cy);
	Invalidate();
}

