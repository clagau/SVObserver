// ******************************************************************************
// * COPYRIGHT (c) 2000 by Körber Pharma Inspection GmbH. All Rights Reserved
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

#pragma region Includes
#include "stdafx.h"
#include "SVRPropTree.h"
#include "SVRPropTreeItemEdit.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVStatusLibrary/MessageManagerHelper.h"
#pragma endregion Includes

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


void SVRPropertyItemEdit::DrawAttribute(CDC* pDC, const RECT& rRect)
{
	Log_Assert(nullptr != m_pProp);

	pDC->SelectObject(IsReadOnly() ? m_pProp->GetNormalFont() : m_pProp->GetBoldFont());
	pDC->SetTextColor( m_rgbForeColor );
	//pDC->SetTextColor(RGB(0,0,0));
	pDC->SetBkMode(TRANSPARENT);

	CRect DrawRect( rRect );
	pDC->DrawText(Data2String().c_str(), DrawRect, DT_SINGLELINE|DT_VCENTER);
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

	if ( pNewWnd != &m_btnDots )
	{
		//commit when the ... button is not pressed
		CommitChanges();
		ShowWindow( SW_HIDE );
		m_btnDots.ShowWindow( SW_HIDE );
		__super::OnKillFocus(pNewWnd);

		// Send message for custom user message to 
		// parent to handle loosing focus to a button belonging to a parent.
		TCHAR l_buff[256];
		GetClassName( pNewWnd->GetSafeHwnd(), l_buff, sizeof(l_buff));
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
		m_btnDots.EnableWindow( IsButtonActiveIfReadOnly() );
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
		SetWindowText( Data2String().c_str() );
	}
}


void SVRPropertyItemEdit::OnCommit()
{
	// hide edit control
	ShowWindow(SW_HIDE);

	CString	Text;
	GetWindowText( Text );
	m_Previous = Data2String();
	String2Data( Text );
	if( nullptr != m_pValue )
	{
		*m_pValue = std::string( Text );
	}
}

void SVRPropertyItemEdit::OnUndoCommit()
{
	String2Data( m_Previous.c_str() );
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
		SetReadOnly(true == IsReadOnly());
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

void SVRPropertyItemEdit::ReadOnlyActivate()
{
	Activate();
	SetReadOnly(TRUE);
}

void SVRPropertyItemEdit::HideEditItemCtrls()
{
	ShowWindow(SW_HIDE);
	m_btnDots.ShowWindow(SW_HIDE);
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
		CreateBtn();
		SendMessage(WM_SETFONT, (WPARAM)m_pProp->GetNormalFont()->m_hObject);		
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

void SVRPropertyItemEdit::CreateBtn()
{
	if (nullptr == m_btnDots.m_hWnd)
	{
		m_btnDots.Create("", WS_CHILD, CRect(0, 0, 0, 0), m_pProp->GetCtrlParent(), 32000 - GetCtrlID());
	}
	m_btnDots.SetWindowText(m_ButtonText.c_str());
	m_btnDots.SetOwner(this);
	m_btnDots.SendMessage(WM_SETFONT, (WPARAM)m_pProp->GetNormalFont()->m_hObject);
}

void SVRPropertyItemEdit::ButtonClicked()
{
	if ( m_bShowButton )
	{
		m_pProp->SendNotify(PTN_ITEMBUTTONCLICK, this);
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
	NMHDR* pnmhdr = reinterpret_cast<NMHDR*> (lParam);

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
	
	return m_vtData.vt;
}


bool SVRPropertyItemEdit::GetItemValue(BYTE& rValue)
{
	rValue = static_cast<BYTE> (m_vtData);

	return true;
}


bool SVRPropertyItemEdit::GetItemValue(bool& rValue)
{
	rValue = static_cast<bool> (m_vtData);
	
	return true;
}


bool SVRPropertyItemEdit::GetItemValue(short& rValue)
{
	rValue = static_cast<short> (m_vtData);
	
	return true;
}


bool SVRPropertyItemEdit::GetItemValue(USHORT& rValue)
{
	rValue = static_cast<USHORT> (m_vtData);

	return true;
}


bool SVRPropertyItemEdit::GetItemValue(long& rValue)
{
	rValue = static_cast<long> (m_vtData);

	return true;
}


bool SVRPropertyItemEdit::GetItemValue(ULONG& rValue)
{
	rValue = static_cast<long> (m_vtData);

	return true;
}


bool SVRPropertyItemEdit::GetItemValue(int& rValue)
{
	rValue = static_cast<long> (m_vtData);

	return true;
}


bool SVRPropertyItemEdit::GetItemValue(UINT& rValue)
{
	rValue = static_cast<long> (m_vtData);

	return true;
}


bool SVRPropertyItemEdit::GetItemValue(float& rValue)
{
	rValue = static_cast<float> (m_vtData);

	return true;
}


bool SVRPropertyItemEdit::GetItemValue(double& rValue)
{
	rValue = static_cast<double> (m_vtData);

	return true;
}


bool SVRPropertyItemEdit::GetItemValue(std::string& rValue)
{
	rValue = Data2String();

	return true;
}


bool SVRPropertyItemEdit::GetItemValue(_variant_t& rValue)
{
	rValue = m_vtData;
	return true;
}


bool SVRPropertyItemEdit::SetItemValue(const BYTE bVal)
{
	try
	{
		m_vtData = bVal;
	}
	catch(const _com_error&)
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
	catch (const _com_error&)
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
	catch(const _com_error&)
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
	catch(const _com_error&)
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
	catch(const _com_error&)
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
	catch(const _com_error&)
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
	catch(const _com_error&)
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
	catch(const _com_error&)
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
	catch(const _com_error&)
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
	catch(const _com_error&)
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
	catch(const _com_error&)
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
	catch(const _com_error&)
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
	catch(const _com_error&)
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
	catch(const _com_error&)
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
	catch(const _com_error&)
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
	catch(const _com_error&)
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
	catch(const _com_error&)
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
	catch(const _com_error&)
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
	catch(const _com_error&)
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
	catch(const _com_error&)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::SetItemValuePtr(std::string& rVal)
{
	try
	{
		m_vtData.SetString( rVal.c_str() );
		m_pValue = &rVal;
	}
	catch(const _com_error&)
	{
		return false;
	}

	return true;
}

std::string SVRPropertyItemEdit::Data2String()
{
	std::string Result;

	switch( m_vtData.vt )
	{
	case VT_BOOL:
		Result = m_vtData.boolVal ? _T("True") : _T("False");
		break;
	case VT_BOOL | VT_BYREF:
		Result = *m_vtData.pboolVal ? _T("True") : _T("False");
		break;
	case VT_I1:
		Result = std::format( _T("{:c}"), m_vtData.cVal );
		break;
	case VT_I1 | VT_BYREF:
		Result = std::format( _T("{:c}"), *m_vtData.pcVal );
		break;
	case VT_UI1:
		Result = std::format( _T("{:#01x}"), m_vtData.bVal);
		break;
	case VT_UI1 | VT_BYREF:
		Result = std::format( _T("{:#01x}"), *m_vtData.pbVal);
		break;
	case VT_I2:
		Result = std::format( _T("{:d}"), m_vtData.iVal );
		break;
	case VT_I2 | VT_BYREF:
		Result = std::format( _T("{:d}"), *m_vtData.piVal );
		break;
	case VT_UI2:
		Result = std::format( _T("{:#04x}"), m_vtData.uiVal);
		break;
	case VT_UI2 | VT_BYREF:
		Result = std::format( _T("{:#04x}"), *m_vtData.puiVal);
		break;
	case VT_I4:
		Result = std::format( _T("{}"), m_vtData.lVal );
		break;
	case VT_I4 | VT_BYREF:
		Result = std::format( _T("{}"), *m_vtData.plVal );
		break;
	case VT_UI4:
		Result = std::format( _T("{:#06x}"), m_vtData.ulVal);
		break;
	case VT_UI4 | VT_BYREF:
		Result = std::format( _T("{:#06x}"), *m_vtData.pulVal);
		break;
	case VT_INT:
		Result = std::format( _T("{}"), m_vtData.intVal );
		break;
	case VT_INT | VT_BYREF:
		Result = std::format( _T("{}"), *m_vtData.pintVal );
		break;
	case VT_UINT:
		Result = std::format( _T("{:#06x}"), m_vtData.uintVal);
		break;
	case VT_UINT | VT_BYREF:
		Result = std::format( _T("{:#06x}"), *m_vtData.puintVal);
		break;
	case VT_R4:
		Result = std::format( _T("{:g}"), m_vtData.fltVal );
		break;
	case VT_R4 | VT_BYREF:
		Result = std::format( _T("{:g}"), *m_vtData.pfltVal );
		break;
	case VT_R8:
		Result = std::format( _T("{:g}"), m_vtData.dblVal );
		break;
	case VT_R8 | VT_BYREF:
		Result = std::format( _T("{:g}"), *m_vtData.pdblVal );
		break;
	case VT_BSTR:
		Result = SvUl::createStdString( m_vtData.bstrVal );
		break;
	case VT_BSTR | VT_BYREF:
		Result = SvUl::createStdString( *m_vtData.pbstrVal );
		break;
	default:
		break;
	}

	return Result;
}

void SVRPropertyItemEdit::String2Data( LPCTSTR Text )
{
	TCHAR* pStopCharacter;

	switch( m_vtData.vt )
	{

	case VT_BOOL:
	case VT_BOOL | VT_BYREF:
		{
			bool Value(false);
			if( 0 == _tcsncmp( Text, _T("1"), 1 ) || 0 == _tcsnicmp( Text, _T("True"), 4 ) )
			{
				Value = true;
			}

			(VT_BYREF == (VT_BYREF & m_vtData.vt)) ? *m_vtData.pboolVal = Value : m_vtData.boolVal = Value;
		}
		break;

	case VT_UI1:
	case VT_UI1 | VT_BYREF:
		{
			BYTE Value = static_cast<BYTE> (_tcstoul(Text, &pStopCharacter, 0));
			if( 0 == *pStopCharacter )
			{
				(VT_BYREF == (VT_BYREF & m_vtData.vt)) ? *m_vtData.pbVal = Value : m_vtData.bVal = Value;
			}
			break;
		}
	case VT_UI2:
	case VT_UI2 | VT_BYREF:
		{
			USHORT Value = static_cast<USHORT> (_tcstoul(Text, &pStopCharacter, 0));
			if( 0 == *pStopCharacter )
			{
				( VT_BYREF == (VT_BYREF & m_vtData.vt) ) ? *m_vtData.puiVal = Value : m_vtData.uiVal = Value;
			}
			break;
		}
	case VT_UI4:
	case VT_UI4 | VT_BYREF:
		{
			ULONG Value = static_cast<ULONG> (_tcstoul(Text, &pStopCharacter, 0));
			if( 0 == *pStopCharacter )
			{
				( VT_BYREF == (VT_BYREF & m_vtData.vt) ) ? *m_vtData.pulVal = Value : m_vtData.ulVal = Value;
			}
			break;
		}
	case VT_UINT:
	case VT_UINT | VT_BYREF:
		{
			ULONG Value = static_cast<ULONG> (_tcstoul(Text, &pStopCharacter, 0));
			if( 0 == *pStopCharacter )
			{
				( VT_BYREF == (VT_BYREF & m_vtData.vt) ) ? *m_vtData.puintVal = Value : m_vtData.uintVal = Value;
			}
			break;
		}
	case VT_INT:
	case VT_INT | VT_BYREF:
		{
			long Value = _tcstol(Text, &pStopCharacter, 0);
			if( 0 == *pStopCharacter )
			{
				( VT_BYREF == (VT_BYREF & m_vtData.vt) ) ? *m_vtData.pintVal = Value : m_vtData.intVal = Value;
			}
			break;
		}
	case VT_I1:
	case VT_I1 | VT_BYREF:
		{
			if (_tcslen(Text) > 0)
			{
				( VT_BYREF == (VT_BYREF & m_vtData.vt) ) ? *m_vtData.pcVal = Text[0] : m_vtData.cVal = Text[0];
			}
			break;
		}
	case VT_I2:
	case VT_I2 | VT_BYREF:
		{
			short Value = static_cast<short> (_tcstol(Text, &pStopCharacter, 0));
			if( 0 == *pStopCharacter )
			{
				( VT_BYREF == (VT_BYREF & m_vtData.vt) ) ? *m_vtData.piVal = Value : m_vtData.iVal = Value;
			}
			break;
		}
	case VT_I4:
	case VT_I4 | VT_BYREF:
		{
			long Value = _tcstol(Text, &pStopCharacter, 0);
			if( 0 == *pStopCharacter )
			{
				( VT_BYREF == (VT_BYREF & m_vtData.vt) ) ? *m_vtData.plVal = Value : m_vtData.lVal = Value;
			}
			break;
		}
	case VT_R4:
	case VT_R4 | VT_BYREF:
		{
			float Value = static_cast<float> (_tcstod(Text, &pStopCharacter));
			if( 0 == *pStopCharacter )
			{
				( VT_BYREF == (VT_BYREF & m_vtData.vt) ) ? *m_vtData.pfltVal = Value : m_vtData.fltVal = Value;
			}
			break;
		}
	case VT_R8:
	case VT_R8 | VT_BYREF:
		{
			double Value = _tcstod(Text, &pStopCharacter);
			if( 0 == *pStopCharacter )
			{
				( VT_BYREF == (VT_BYREF & m_vtData.vt) ) ? *m_vtData.pdblVal = Value : m_vtData.dblVal = Value;
			}
			break;
		}
	case VT_BSTR:
	case VT_BSTR | VT_BYREF:
		m_vtData.SetString(Text);
		break;
	}
}
