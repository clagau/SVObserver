// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : D:\...\PropTreeItemEdit.cpp
// * .File Name       : $Workfile:   PropTreeItemEdit.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.7  $
// * .Check In Date   : $Date:   26 May 2014 10:18:40  $
// ******************************************************************************

// PropTreeItemEdit.cpp : implementation file
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
#include "SVRPropTreeItemEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVRPropertyItemEdit

BEGIN_MESSAGE_MAP(SVRPropertyItemEdit, CEdit)
	//{{AFX_MSG_MAP(SVRPropertyItemEdit)
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocusReflect)
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	ON_WM_SIZE()
	ON_WM_ENABLE()
	//}}AFX_MSG_MAP
	ON_WM_KEYUP()
END_MESSAGE_MAP()


SVRPropertyItemEdit::SVRPropertyItemEdit() :
	m_pValue(nullptr)
{
	// Initialize all variables
	m_bShowButton      = false;
	m_ButtonText = _T("...");
	m_bKillFocusWorkaround = false;
}


SVRPropertyItemEdit::~SVRPropertyItemEdit()
{
}


void SVRPropertyItemEdit::DrawAttribute(CDC* pDC, const RECT& rRect)
{
	ASSERT(nullptr != m_pProp);

	pDC->SelectObject(IsReadOnly() ? m_pProp->GetNormalFont() : m_pProp->GetBoldFont());
	pDC->SetTextColor( m_rgbForeColor );
	//pDC->SetTextColor(RGB(0,0,0));
	pDC->SetBkMode(TRANSPARENT);

	CRect DrawRect( rRect );
	SVString Text = SvUl_SF::createSVString( m_vtData );
	pDC->DrawText(Text.c_str(), DrawRect, DT_SINGLELINE|DT_VCENTER);
}


void SVRPropertyItemEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar)
	{
		case VK_RETURN:
			break;
		default:
			CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
}


UINT SVRPropertyItemEdit::OnGetDlgCode() 
{
	return CEdit::OnGetDlgCode()|DLGC_WANTALLKEYS;
}


void SVRPropertyItemEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	SVRPropTree* pTree = m_pParent->GetPropTree() ;
	switch (nChar)
	{
		case VK_ESCAPE:
			OnRefresh();
			// fall through
		case VK_RETURN:
			{
				CommitChanges();

				//DisplayButton();
			}
			return;
		case VK_SHIFT:
			{
				pTree->m_bShift= true;
				break;
			}
		case VK_TAB:
			{
				CommitChanges();
				if( pTree )
				{
					if( pTree->m_bShift )
					{
						pTree->FocusPrev();
					}
					else
					{
						pTree->FocusNext();
					}
					pTree->RefreshItems();
				}
				break;
			}
		case VK_DOWN:
			{
				CommitChanges();
				if( pTree )
				{
					pTree->FocusNext();
					pTree->RefreshItems();
				}
				break;
			}
		case VK_UP:
			{
				CommitChanges();
				if( pTree )
				{
					pTree->FocusPrev();
					pTree->RefreshItems();
				}
				break;
			}
		default:
			{
				// check for CTRL + 'period' keystroke, if found, simulate a mouse click on the button
				if ((nChar == 0xBE || nChar == VK_DECIMAL) && GetKeyState(VK_CONTROL) < 0)
				{
					if ( m_bShowButton )
					{
						ButtonClicked();
					}
					return;
				}

				CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
			}
			break;
	}
}

void SVRPropertyItemEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( nChar == VK_SHIFT )
	{
		SVRPropTree* pTree = m_pParent->GetPropTree() ;
		pTree->m_bShift = false;
	}
	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}

void SVRPropertyItemEdit::OnKillfocusReflect() 
{
	CommitChanges();
	if ( GetFocus() != &m_btnDots )
	{
		ShowWindow( SW_HIDE );
		m_btnDots.ShowWindow( SW_HIDE );
	}
}

void SVRPropertyItemEdit::OnKillFocus(CWnd *pNewWnd) 
{
	m_bKillFocusWorkaround = true;	// prevent this edit from setting focus again on Activate during Commit
	CommitChanges();
	if ( pNewWnd != &m_btnDots )
	{
		ShowWindow( SW_HIDE );
		m_btnDots.ShowWindow( SW_HIDE );
		__super::OnKillFocus(pNewWnd);

		// Send message for custom user message to 
		// parent to handle loosing focus to a button belonging to a parent.
		TCHAR l_buff[256];
		int nChars = GetClassName( pNewWnd->GetSafeHwnd(), l_buff, sizeof(l_buff));
		if(_tcsicmp(l_buff, _T("Button")) ==0)
		{
			long l_ID = ::GetWindowLong( pNewWnd->GetSafeHwnd(), GWL_ID ) ;
			pNewWnd->GetParent()->SendMessage( MSG_TO_PARENT, MAKEWPARAM(l_ID, BN_CLICKED) , reinterpret_cast<LPARAM>(pNewWnd->GetSafeHwnd()));
		}
	}
	else
	{
		// The super class is not called so the button will still work.
		HideCaret();
	}
	m_bKillFocusWorkaround = false;
}

void SVRPropertyItemEdit::OnEnable(BOOL bEnable) 
{
	// enables/disables the control
	CEdit::OnEnable(bEnable);
	if ( m_bShowButton )
	{
		m_btnDots.EnableWindow( bEnable );
	}
}


void SVRPropertyItemEdit::OnMove()
{
	if (IsWindow(m_hWnd))
	{
		SetWindowPos(nullptr, m_rc.left, m_rc.top, m_rc.Width(), m_rc.Height(), SWP_NOZORDER|SWP_NOACTIVATE);
	}
}


void SVRPropertyItemEdit::OnRefresh()
{
	if (IsWindow(m_hWnd))
	{
		SVString Text = SvUl_SF::createSVString( m_vtData );
		SetWindowText( Text.c_str() );
	}
}


void SVRPropertyItemEdit::OnCommit()
{
	// hide edit control
	ShowWindow(SW_HIDE);

	CString	Text;
	GetWindowText( Text );
	m_Previous = SvUl_SF::createSVString( m_vtData );
	m_vtData.SetString( Text );
	if( nullptr != m_pValue )
	{
		*m_pValue = SVString( Text );
	}
}

void SVRPropertyItemEdit::OnUndoCommit()
{
	m_vtData.SetString(m_Previous.c_str());
	if(nullptr != m_pValue)
	{
		*m_pValue = m_Previous;
	}
}


void SVRPropertyItemEdit::OnActivate()
{
	// Check if the edit control needs creation
	if (!IsWindow(m_hWnd))
	{
		DWORD dwStyle = WS_CHILD|ES_AUTOHSCROLL|WS_TABSTOP;
		CreateEdit(dwStyle);
	}

	if ( !m_bKillFocusWorkaround )
	{
		OnRefresh();

		DisplayButton();
		int iButtonWidth = 0;
		if (m_bShowButton)
		{
			CDC* pDC = m_btnDots.GetDC();
			CSize size = pDC->GetTextExtent( m_ButtonText.c_str(), static_cast<int> (m_ButtonText.size()) );
			iButtonWidth = size.cx + g_buttonSpace;
			m_btnDots.ReleaseDC(pDC);
		}

		SetWindowPos(nullptr, m_rc.left, m_rc.top, m_rc.Width()-iButtonWidth, m_rc.Height(), SWP_NOZORDER | SWP_SHOWWINDOW);
		SetSel(0,-1);
		SetFocus();
	}

}

void SVRPropertyItemEdit::OnLossFocus()
{
	OnKillFocus(nullptr);
}

BOOL SVRPropertyItemEdit::CreateEdit(DWORD dwStyle)
{
	BOOL bRet = Create(dwStyle, m_rc, m_pProp->GetCtrlParent(), GetCtrlID());

	if ( bRet )
	{
		m_btnDots.Create("", WS_CHILD, CRect(0,0,0,0), m_pProp->GetCtrlParent(), 32000-GetCtrlID());
		m_btnDots.SetWindowText(m_ButtonText.c_str());
		m_btnDots.SetOwner( this );

		SendMessage(WM_SETFONT, (WPARAM)m_pProp->GetNormalFont()->m_hObject);
		m_btnDots.SendMessage(WM_SETFONT, (WPARAM)m_pProp->GetNormalFont()->m_hObject);
	}

	return bRet;
}

void SVRPropertyItemEdit::DisplayButton()
{
	CDC* pDC = m_btnDots.GetDC();
	CSize size = pDC->GetTextExtent( m_ButtonText.c_str(), static_cast<int> (m_ButtonText.size()) );
	m_btnDots.ReleaseDC(pDC);

	int iButtonWidth = size.cx + g_buttonSpace;
	m_bShowButton = (m_pProp->SendNotify(PTN_QUERY_SHOW_BUTTON, this)) != FALSE;
	m_btnDots.SetWindowPos(nullptr, m_rc.right-iButtonWidth, m_rc.top, iButtonWidth, m_rc.Height(), SWP_NOZORDER | (m_bShowButton ? SWP_SHOWWINDOW : SWP_HIDEWINDOW));
}


void SVRPropertyItemEdit::ButtonClicked()
{
	if ( m_bShowButton )
	{
		bool bResult = false;
		LRESULT lr = m_pProp->SendNotify(PTN_ITEMBUTTONCLICK, this);
		m_bCommitOnce = false;	// always commit
		CommitChanges();
	}
}

void SVRPropertyItemEdit::OnSize(UINT nType, int cx, int cy) 
{
	CEdit::OnSize(nType, cx, cy);
}

BOOL SVRPropertyItemEdit::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult )
{
	BOOL bResult=FALSE;
	NMHDR* pnmhdr = (NMHDR*) lParam;

	if ( pnmhdr->code == NM_CLICK && (UINT) m_btnDots.GetDlgCtrlID() == pnmhdr->idFrom )
	{
		ButtonClicked();
		bResult = TRUE;
	}
	else
	{
		bResult = __super::OnNotify( wParam, lParam, pResult );
	}

	return bResult;
}

void SVRPropertyItemEdit::SetButtonText( LPCTSTR Text )
{
	m_ButtonText = Text;
}


int SVRPropertyItemEdit::GetItemType(void)
{
	
	return V_VT(&m_vtData);
}


bool SVRPropertyItemEdit::GetItemValue(BYTE& bVal)
{
	try
	{
		bVal = (BYTE)m_vtData;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::GetItemValue(bool& bVal)
{
	try
	{
		bVal = (bool)m_vtData;
	}
	catch(_com_error e)
	{
		return false;
	}
	
	return true;
}


bool SVRPropertyItemEdit::GetItemValue(short& iVal)
{
	try
	{
		iVal = (short)m_vtData;
	}
	catch(_com_error e)
	{
		return false;
	}
	
	return true;
}


bool SVRPropertyItemEdit::GetItemValue(USHORT& uiVal)
{
	try
	{
		uiVal = (USHORT)m_vtData;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::GetItemValue(long& lVal)
{
	try
	{
		lVal = (long)m_vtData;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::GetItemValue(ULONG& ulVal)
{
	try
	{
		ulVal = (ULONG)m_vtData;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::GetItemValue(int& intVal)
{
	try
	{
		intVal = (int)m_vtData;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::GetItemValue(UINT& uintVal)
{
	try
	{
		uintVal = (UINT)m_vtData;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::GetItemValue(float& fltVal)
{
	try
	{
		fltVal = (float)m_vtData;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::GetItemValue(double& dblVal)
{
	try
	{
		dblVal = (double)m_vtData;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::GetItemValue(SVString& rVal)
{
	try
	{
		rVal = SvUl_SF::createSVString( m_vtData );
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::GetItemValue(VARIANT& vtVal)
{
	vtVal = m_vtData;
	return true;
}


bool SVRPropertyItemEdit::SetItemValue(const BYTE bVal)
{
	try
	{
		m_vtData = bVal;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::SetItemValue(const short iVal)
{
	try
	{
		m_vtData = iVal;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::SetItemValue(const USHORT uiVal)
{
	try
	{
		m_vtData = uiVal;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::SetItemValue(const long lVal)
{
	try
	{
		m_vtData = lVal;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::SetItemValue(const ULONG ulVal)
{
	try
	{
		m_vtData = ulVal;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::SetItemValue(const int intVal)
{
	try
	{
		m_vtData = intVal;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::SetItemValue(const UINT uintVal)
{
	try
	{
		m_vtData = uintVal;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::SetItemValue(const float fltVal)
{
	try
	{
		m_vtData = fltVal;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::SetItemValue(const double dblVal)
{
	try
	{
		m_vtData = dblVal;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::SetItemValue(LPCTSTR Val)
{
	try
	{
		m_vtData.SetString( Val );
		m_pValue   = nullptr;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}



bool SVRPropertyItemEdit::SetItemValue(const bool  boolVal)
{
	try
	{
		m_vtData = boolVal;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::SetItemValuePtr(BYTE& bVal)
{
	try
	{
		m_vtData.Clear();
		m_vtData.ChangeType( VT_UI1 | VT_BYREF );
		m_vtData.pbVal = &bVal;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::SetItemValuePtr(short& iVal)
{
	try
	{
		m_vtData.Clear();
		m_vtData.ChangeType( VT_I2 | VT_BYREF );
		m_vtData.piVal = &iVal;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::SetItemValuePtr(USHORT& uiVal)
{
	try
	{
		m_vtData.Clear();
		m_vtData.ChangeType( VT_UI2 | VT_BYREF );
		m_vtData.puiVal = &uiVal;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::SetItemValuePtr(long& lVal)
{
	try
	{
		m_vtData.Clear();
		m_vtData.ChangeType( VT_I4 | VT_BYREF );
		m_vtData.plVal = &lVal;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::SetItemValuePtr(ULONG& ulVal)
{
	try
	{
		m_vtData.Clear();
		m_vtData.ChangeType( VT_UI4 | VT_BYREF );
		m_vtData.pulVal = &ulVal;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::SetItemValuePtr(int& intVal)
{
	try
	{
		m_vtData.Clear();
		m_vtData.ChangeType( VT_INT | VT_BYREF );
		m_vtData.pintVal = &intVal;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::SetItemValuePtr(UINT& uintVal)
{
	try
	{
		m_vtData.Clear();
		m_vtData.ChangeType( VT_UINT | VT_BYREF );
		m_vtData.puintVal = &uintVal;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::SetItemValuePtr(float& fltVal)
{
	try
	{
		m_vtData.Clear();
		m_vtData.ChangeType( VT_R4 | VT_BYREF );
		m_vtData.pfltVal = &fltVal;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::SetItemValuePtr(double& dblVal)
{
	try
	{
		m_vtData.Clear();
		m_vtData.ChangeType( VT_R8 | VT_BYREF );
		m_vtData.pdblVal = &dblVal;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::SetItemValuePtr(SVString& rVal)
{
	try
	{
		m_vtData.SetString( rVal.c_str() );
		m_pValue = &rVal;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}

