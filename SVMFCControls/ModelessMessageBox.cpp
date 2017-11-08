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

class CModelessMsgBox;
#define ModelessMessageBox(s,e)	{CModelessMsgBox* pBox = new CModelessMsgBox((s),(e)); pBox->Create(CModelessMsgBox::IDD, nullptr);}

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

/////////////////////////////////////////////////////////////////////////////
// CModelessMsgBox dialog

class CModelessMsgBox : public CDialog
{
	// Construction
public:
	CModelessMsgBox(const std::string& rMsg, HANDLE p_hEvent);   // standard constructor
	virtual ~CModelessMsgBox();


protected:
	std::string m_Msg;
	HANDLE m_hEvent;

public:
	// Dialog Data
	//{{AFX_DATA(CModelessMsgBox)
	enum { IDD = IDD_MODELESS_MESSAGE_BOX };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
protected:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModelessMsgBox)
public:
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual void PostNcDestroy() override;
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModelessMsgBox)
	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;
	virtual void OnCancel() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CModelessMsgBox, CDialog)
	//{{AFX_MSG_MAP(CModelessMsgBox)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CModelessMsgBox::CModelessMsgBox(const std::string& rMsg, HANDLE p_hEvent)
	: CDialog(CModelessMsgBox::IDD, nullptr)
, m_Msg( rMsg )
{
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

	m_Msg.clear();
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
	
	GetDlgItem(IDC_MODELESS_MESSAGE_BOX_TEXT)->SetWindowText( m_Msg.c_str() );
	
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


void CallModelessMessageBox(std::string &Message, HANDLE hCheckEvent)
{
	ModelessMessageBox(Message, hCheckEvent);
}


