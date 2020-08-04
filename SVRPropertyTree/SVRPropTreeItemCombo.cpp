// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : PropTreeItemCombo.cpp
// * .File Name       : $Workfile:   PropTreeItemCombo.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.4  $
// * .Check In Date   : $Date:   26 May 2014 10:16:02  $
// ******************************************************************************

// PropTreeItemCombo.cpp : implementation file
//
//  Copyright (C) 1998-2001 Scott Ramsay
//	sramsay@gonavi.com
//	http://www.gonavi.com
//
//  This material is provided "as is", with absolutely no warranty expressed
//  or implied. Any use is at your own risk.
// 
//  Permission to use or copy this software for any purpose is hereby granted 
//  without fee, provided the above notices are retained on all copies.
//  Permission to modify the code and to distribute modified code is granted,
//  provided the above notices are retained, and a notice that the code was
//  modified is included with the above copyright notice.
// 
//	If you use this code, drop me an email.  I'd like to know if you find the code
//	useful.

#include "stdafx.h"
#include "SVRPropTree.h"
#include "SVRPropTreeItemCombo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum { DROPDOWN_HEIGHT = 100};

/////////////////////////////////////////////////////////////////////////////
// SVRPropertyItemCombo

BEGIN_MESSAGE_MAP(SVRPropertyItemCombo, CComboBox)
	//{{AFX_MSG_MAP(SVRPropertyItemCombo)
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, OnKillfocusReflect)
	ON_CONTROL_REFLECT(CBN_SELENDOK, OnSelendok)
	ON_CONTROL_REFLECT(CBN_SELENDCANCEL, OnSelendcancel)
	ON_WM_ENABLE()
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVRPropertyItemCombo::SVRPropertyItemCombo() :
	m_nDropHeight(DROPDOWN_HEIGHT)
{
	Initialize();
}

SVRPropertyItemCombo::~SVRPropertyItemCombo()
{
}

void SVRPropertyItemCombo::Initialize()
{
	// Initialize all variables
	m_bShowButton      = false;
	m_bEnableButton    = true;
	m_ButtonText = _T("...");
}

/////////////////////////////////////////////////////////////////////////////
// SVRPropertyItemCombo message handlers

void SVRPropertyItemCombo::DrawAttribute(CDC* pDC, const RECT& rRect)
{
	assert(nullptr != m_pProp);

	CFont*	l_pOldFont;

	// verify the window has been created
	if (!IsWindow(m_hWnd))
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
		TRACE0("SVRPropertyItemCombo::DrawAttribute() - The window has not been created\n");
#endif
		return;
	}

	l_pOldFont = pDC->SelectObject(IsReadOnly() ? m_pProp->GetNormalFont() : m_pProp->GetBoldFont());

	pDC->SetTextColor( m_rgbForeColor );
	pDC->SetBkMode(TRANSPARENT);

	CRect DrawRect( rRect );
	CString Text;
	LONG idx;

	if( CB_ERR != (idx = GetCurSel()))
	{
		GetLBText(idx, Text);
	}

	pDC->DrawText(Text, DrawRect, DT_SINGLELINE|DT_VCENTER);

// JAB110708
	pDC->SelectObject (l_pOldFont);
}


void SVRPropertyItemCombo::OnMove()
{
	if (IsWindow(m_hWnd) && IsWindowVisible())
	{
		SetWindowPos(nullptr, m_rc.left, m_rc.top, m_rc.Width(), m_rc.Height(), SWP_NOZORDER|SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}
}


void SVRPropertyItemCombo::OnRefresh()
{
	if ( nullptr != GetSafeHwnd() )
	{
		LONG idx = FindCBData(m_lComboData);

		if (idx != CB_ERR)
		{
			// check if the current selection has changed...
			long curSel = GetCurSel();
			if (curSel != idx)
				SetCurSel(idx);
		}
	}
}

void SVRPropertyItemCombo::OnCommit()
{
	ShowDropDown(FALSE);
	
	StoreItemData();
}

void SVRPropertyItemCombo::StoreItemData()
{
	LONG idx;
	// store combo box item data
	if ( nullptr != GetSafeHwnd() )
	{
		if( CB_ERR != (idx = GetCurSel()) )
		{
			m_lComboData = (LPARAM)GetItemData(idx);
		}
		else
		{
			m_lComboData = 0L;
		}
	}
}

void SVRPropertyItemCombo::OnActivate()
{
	int iButtonWidth = 0;
	if ( m_bEnableButton )
	{
		CDC* pDC = m_btnDots.GetDC();
		CSize size = pDC->GetTextExtent( m_ButtonText.c_str(), static_cast<int> (m_ButtonText.size()) );
		iButtonWidth = size.cx + g_buttonSpace;
	}	
	// activate the combo box
	SetWindowPos(nullptr, m_rc.left, m_rc.top, m_rc.Width()-iButtonWidth, m_rc.Height() + m_nDropHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

	DisplayButton();

	SetFocus();
}

void SVRPropertyItemCombo::OnLossFocus()
{
	OnKillFocus(nullptr);
}

BOOL SVRPropertyItemCombo::CreateComboBox(DWORD dwStyle)
{
	assert(nullptr != m_pProp);

	if (IsWindow(m_hWnd))
	{
		DestroyWindow();
	}

	dwStyle = (WS_CHILD|WS_VSCROLL|dwStyle|WS_VISIBLE);

	if (!Create(dwStyle, CRect(0,0,0,0), m_pProp->GetCtrlParent(), GetCtrlID()))
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
		TRACE0("SVRPropertyItemCombo::CreateComboBox() - failed to create combo box\n");
#endif
		return FALSE;
	}

	if ( m_bEnableButton )
	{
		m_btnDots.Create("", WS_CHILD, CRect(0,0,0,0), m_pProp->GetCtrlParent(), 32000-GetCtrlID());
		m_btnDots.SetWindowText( m_ButtonText.c_str() );
		m_btnDots.SetOwner( this );
		m_btnDots.SendMessage(WM_SETFONT, (WPARAM)m_pProp->GetNormalFont()->m_hObject);
	}
	SendMessage(WM_SETFONT, (WPARAM)m_pProp->GetNormalFont()->m_hObject);

	return TRUE;
}

void SVRPropertyItemCombo::EnableButton( bool bEnable )
{
	m_bEnableButton = bEnable;
}

void SVRPropertyItemCombo::DisplayButton()
{
	if ( m_bEnableButton )
	{
		CDC* pDC = m_btnDots.GetDC();
		CSize size = pDC->GetTextExtent( m_ButtonText.c_str(), static_cast<int> (m_ButtonText.size()) );
		int iButtonWidth = size.cx + g_buttonSpace;
		m_bShowButton = (m_pProp->SendNotify(PTN_QUERY_SHOW_BUTTON, this)) != FALSE;
		m_btnDots.SetWindowPos(nullptr, m_rc.right-iButtonWidth, m_rc.top, iButtonWidth, m_rc.Height(), SWP_NOZORDER | (m_bShowButton ? SWP_SHOWWINDOW : SWP_HIDEWINDOW));
	}
}

BOOL SVRPropertyItemCombo::CreateComboBoxBool()
{
	assert(nullptr != m_pProp);

	if (IsWindow(m_hWnd))
	{
		DestroyWindow();
	}

	// force as a non-visible child window
	DWORD dwStyle = WS_CHILD|WS_VSCROLL|CBS_SORT|CBS_DROPDOWNLIST;

	if (!Create(dwStyle, CRect(0,0,0,0), m_pProp->GetCtrlParent(), GetCtrlID()))
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
		TRACE0("SVRPropertyItemCombo::CreateComboBoxBool() - failed to create combo box\n");
#endif
		return FALSE;
	}

	if ( m_bEnableButton )
	{
		m_btnDots.Create("", WS_CHILD, CRect(0,0,0,0), m_pProp->GetCtrlParent(), 32000-GetCtrlID());
		m_btnDots.SetWindowText( m_ButtonText.c_str() );
		m_btnDots.SetOwner( this );
		m_btnDots.SendMessage(WM_SETFONT, (WPARAM)m_pProp->GetNormalFont()->m_hObject);
	}
	SendMessage(WM_SETFONT, (WPARAM)m_pProp->GetNormalFont()->m_hObject);

	// file the combo box
	LONG idx;
	idx = AddString(_T("True"));
	SetItemData(idx, TRUE);

	idx = AddString(_T("False"));
	SetItemData(idx, FALSE);

	return TRUE;
}


LONG SVRPropertyItemCombo::FindCBData(LPARAM lParam)
{
	LONG idx;
	
	for (idx = 0; idx < GetCount(); idx++)
	{
		if (GetItemData(idx) == static_cast<DWORD_PTR>(lParam))
		{
			return idx;
		}
	}

	return CB_ERR;
}


void SVRPropertyItemCombo::OnSelendok() 
{
	m_bCommitOnce = false;	// always commit
	CommitChanges();	

	if ( m_bEnableButton )
	{
		m_bShowButton = (m_pProp->SendNotify(PTN_QUERY_SHOW_BUTTON, this)) != FALSE;
		m_btnDots.SetWindowPos(nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | (m_bShowButton ? SWP_SHOWWINDOW : SWP_HIDEWINDOW));
	}
}

void SVRPropertyItemCombo::OnSelendcancel() 
{
	CommitChanges();
	if ( m_bEnableButton )
	{
		m_bShowButton = (m_pProp->SendNotify(PTN_QUERY_SHOW_BUTTON, this)) != FALSE;
		m_btnDots.ShowWindow( m_bShowButton ? SW_SHOW : SW_HIDE );
	}
}


void SVRPropertyItemCombo::OnKillfocusReflect() 
{
	if ( GetFocus() != &m_btnDots )
	{
		ShowWindow( SW_HIDE );
		if ( m_bEnableButton )
		{
			m_btnDots.ShowWindow( SW_HIDE );
		}
	}
}

void SVRPropertyItemCombo::OnKillFocus( CWnd* pNewWnd ) 
{
	if ( pNewWnd != &m_btnDots )
	{
		ShowWindow( SW_HIDE );
		if ( m_bEnableButton )
		{
			m_btnDots.ShowWindow( SW_HIDE );
		}
	}
}


void SVRPropertyItemCombo::SetDropDownHeight(LONG nDropHeight)
{
	m_nDropHeight = nDropHeight;
}


LONG SVRPropertyItemCombo::GetDropDownHeight()
{
	return m_nDropHeight;
}


bool SVRPropertyItemCombo::GetItemValue( long& rValue )
{
	StoreItemData();
	rValue = static_cast< long >( m_lComboData );
	return true;
}

bool SVRPropertyItemCombo::GetItemValue( UINT& rValue )
{
	StoreItemData();
	rValue = static_cast< UINT >( m_lComboData );
	return true;
}


bool SVRPropertyItemCombo::GetItemValue(_variant_t& rValue)
{
	StoreItemData();
	rValue.Clear();

	rValue.ChangeType( VT_I8 );
	rValue.llVal = m_lComboData;

	return true;
}

bool SVRPropertyItemCombo::SetItemValue(const LPARAM lparam)
{
	m_lComboData = lparam;
	OnRefresh();

	return true;
}

void SVRPropertyItemCombo::ButtonClicked()
{
	if ( m_bEnableButton )
	{
		if ( m_bShowButton )
		{
			m_pProp->SendNotify(PTN_ITEMBUTTONCLICK, this);
		}
	}
}

void SVRPropertyItemCombo::OnEnable(BOOL bEnable) 
{
	// enables/disables the control
	CComboBox::OnEnable(bEnable);
	if ( m_bEnableButton )
	{
		if ( m_bShowButton )
		{
			m_btnDots.EnableWindow( bEnable );
		}
	}
}

void SVRPropertyItemCombo::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		if (nChar == VK_ESCAPE)
			OnRefresh();
		CommitChanges();
		return;
	}
	
	// check for CTRL + 'period' keystroke, if found, simulate a mouse click on the button
	if ((nChar == 0xBE || nChar == VK_DECIMAL) && GetKeyState(VK_CONTROL) < 0)
	{
		if ( m_bEnableButton )
		{
			if ( m_bShowButton )
			{
				ButtonClicked();
			}
		}
		return;
	}

	CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

void SVRPropertyItemCombo::OnSize(UINT nType, int cx, int cy) 
{
	CComboBox::OnSize(nType, cx, cy);
}

BOOL SVRPropertyItemCombo::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult )
{
	BOOL bResult;
	NMHDR* pnmhdr = (NMHDR*) lParam;

	if ( m_bEnableButton && pnmhdr->code == NM_CLICK && (UINT) m_btnDots.GetDlgCtrlID() == pnmhdr->idFrom )
	{
		ButtonClicked();
		bResult = TRUE;
	}
	else
	{
		bResult = CComboBox::OnNotify( wParam, lParam, pResult );
	}

	return bResult;
}

void SVRPropertyItemCombo::SetButtonText( LPCTSTR Text )
{
	m_ButtonText = Text;
}

