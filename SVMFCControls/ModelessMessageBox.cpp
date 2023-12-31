// ******************************************************************************
// * COPYRIGHT (c) 2005 by K�rber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : ModelessMessageBox
// * .File Name       : $Workfile:   ModelessMessageBox.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 09:14:20  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVOResource/resource.h"

#include "ModelessMessageBox.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(ModelessMessageBox, CDialog)
	//{{AFX_MSG_MAP(ModelessMessageBox)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// the following is from Windows Developer's Journal Feb 1998 p 54
// code by James M. Curran; JamesCurram@CompuServe.com
class DialogUnits
{
public:
	DialogUnits()
	{
		DWORD dw = GetDialogBaseUnits();
		m_duXx4 = LOWORD(dw);
		m_duYx8 = HIWORD(dw);
	}

	~DialogUnits() {};

	int X(int x) { return ((x * m_duXx4) / 4); }
	int Y(int y) { return ((y * m_duYx8) / 8); }

private:
	WORD m_duXx4;	// dialog units on the X axis times 4
	WORD m_duYx8;	// dialog units on the Y axis times 8
};

ModelessMessageBox::ModelessMessageBox(const std::string& rMsg, HANDLE p_hEvent)
	: CDialog(ModelessMessageBox::IDD, nullptr)
, m_Msg( rMsg )
{
	DuplicateHandle( GetCurrentProcess(), p_hEvent,
		GetCurrentProcess(), &m_hEvent,
		0, FALSE, DUPLICATE_SAME_ACCESS );
}

ModelessMessageBox::~ModelessMessageBox()
{
	if( nullptr != m_hEvent )
	{
		::ResetEvent( m_hEvent );
		::CloseHandle( m_hEvent );

		m_hEvent = nullptr;
	}

	m_Msg.clear();
}

void ModelessMessageBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ModelessMessageBox)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

void ModelessMessageBox::PostNcDestroy() 
{
	CDialog::PostNcDestroy();

	delete this;
}

BOOL ModelessMessageBox::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetDlgItem(IDC_MODELESS_MESSAGE_BOX_TEXT)->SetWindowText( m_Msg.c_str() );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void ModelessMessageBox::OnOK() 
{
	DestroyWindow();
}

void ModelessMessageBox::OnCancel()
{
	DestroyWindow();
}

void ModelessMessageBox::OnSize(UINT nType, int cx, int cy) 
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


