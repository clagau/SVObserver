// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : D:\...\PropTreeItemEdit.cpp
// * .File Name       : $Workfile:   PropTreeItemEdit.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.6  $
// * .Check In Date   : $Date:   26 May 2014 08:45:26  $
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
#include "proptree.h"
#include "PropTreeItemEdit.h"

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
	m_pstr(NULL)
{
	// Initialize all variables
	m_bShowButton      = false;
	m_strButtonText = _T("...");
	m_bKillFocusWorkaround = false;
}


SVRPropertyItemEdit::~SVRPropertyItemEdit()
{
}


void SVRPropertyItemEdit::DrawAttribute(CDC* pDC, const RECT& rc)
{
	ASSERT(m_pProp!=NULL);

	pDC->SelectObject(IsReadOnly() ? m_pProp->GetNormalFont() : m_pProp->GetBoldFont());
	pDC->SetTextColor( m_rgbForeColor );
	//pDC->SetTextColor(RGB(0,0,0));
	pDC->SetBkMode(TRANSPARENT);

	CRect r = rc;
	CString str;
	m_vtData.toString(str);
	pDC->DrawText(str, r, DT_SINGLELINE|DT_VCENTER);
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
		SetWindowPos(NULL, m_rc.left, m_rc.top, m_rc.Width(), m_rc.Height(), SWP_NOZORDER|SWP_NOACTIVATE);
//		SetWindowPos(NULL, m_rc.left, m_rc.top, m_rc.Width(), m_rc.Height(), SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}


void SVRPropertyItemEdit::OnRefresh()
{
	if (IsWindow(m_hWnd))
	{
		CString str;
		m_vtData.toString(str);
		SetWindowText(str);
	}
}


void SVRPropertyItemEdit::OnCommit()
{
	// hide edit control
	ShowWindow(SW_HIDE);

	CString	str;
	GetWindowText(str);
	m_vtData.toString(m_strPrevious);
	m_vtData.fromString(str);
	if (m_pstr)
		*m_pstr = str;
}

void SVRPropertyItemEdit::OnUndoCommit()
{
	m_vtData.fromString(m_strPrevious);
	if (m_pstr)
	{
		*m_pstr = m_strPrevious;
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

		CDC* pDC = m_btnDots.GetDC();
		CSize size = pDC->GetTextExtent(m_strButtonText);
		m_btnDots.ReleaseDC(pDC);

		int iButtonWidth = size.cx + 4;
		SetWindowPos(NULL, m_rc.left, m_rc.top, m_rc.Width()-iButtonWidth, m_rc.Height(), SWP_NOZORDER | SWP_SHOWWINDOW);
		SetSel(0,-1);
		DisplayButton();

		SetFocus();
	}

}

BOOL SVRPropertyItemEdit::CreateEdit(DWORD dwStyle)
{
	BOOL bRet = Create(dwStyle, m_rc, m_pProp->GetCtrlParent(), GetCtrlID());

	if ( bRet )
	{
		m_btnDots.Create("", WS_CHILD, CRect(0,0,0,0), m_pProp->GetCtrlParent(), 32000-GetCtrlID());
		m_btnDots.SetWindowText(m_strButtonText);
		m_btnDots.SetOwner( this );

		SendMessage(WM_SETFONT, (WPARAM)m_pProp->GetNormalFont()->m_hObject);
		m_btnDots.SendMessage(WM_SETFONT, (WPARAM)m_pProp->GetNormalFont()->m_hObject);
	}

	return bRet;
}

void SVRPropertyItemEdit::DisplayButton()
{
	CDC* pDC = m_btnDots.GetDC();
	CSize size = pDC->GetTextExtent(m_strButtonText);
	m_btnDots.ReleaseDC(pDC);

	int iButtonWidth = size.cx + 4;
	m_bShowButton = (m_pProp->SendNotify(PTN_QUERY_SHOW_BUTTON, this)) != FALSE;
	m_btnDots.SetWindowPos(NULL, m_rc.right-iButtonWidth, m_rc.top, iButtonWidth, m_rc.Height(), SWP_NOZORDER | (m_bShowButton ? SWP_SHOWWINDOW : SWP_HIDEWINDOW));
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

void SVRPropertyItemEdit::SetButtonText( const CString& sText )
{
	m_strButtonText = sText;
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


bool SVRPropertyItemEdit::GetItemValue(CString& str)
{
	try
	{
		m_vtData.toString(str);
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


bool SVRPropertyItemEdit::SetItemValue(LPCTSTR strSrc)
{
	try
	{
		m_vtData = strSrc;
		m_pstr   = NULL;
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
		m_vtData.AttachRef(&bVal);
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
		m_vtData.AttachRef(&iVal);
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
		m_vtData.AttachRef(&uiVal);
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
		m_vtData.AttachRef(&lVal);
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
		m_vtData.AttachRef(&ulVal);
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
		m_vtData.AttachRef(&intVal);
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
		m_vtData.AttachRef(&uintVal);
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
		m_vtData.AttachRef(&fltVal);
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
		m_vtData.AttachRef(&dblVal);
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}


bool SVRPropertyItemEdit::SetItemValuePtr(CString& strVal)
{
	try
	{
		m_vtData = strVal;
		m_pstr   = &strVal;
	}
	catch(_com_error e)
	{
		return false;
	}

	return true;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\PropertyTree\PropTreeItemEdit.cpp_v  $
 * 
 *    Rev 1.6   26 May 2014 08:45:26   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  897
 * SCR Title:  Fix SVObserver GDI Resource Leaks
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added ReleaseDC to clean up after GetDC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   29 Jan 2014 11:09:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  873
 * SCR Title:  Fix inconsistant GUI labels and functionality on IO pages
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed code that positions the cursor on the decimal of floating point numbers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Jan 2014 09:22:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   modified on kill focus. Position of workaround flag.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   05 Jul 2013 14:35:48   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 021.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   14 Jun 2013 10:22:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  827
 * SCR Title:  Add button to the adjust tool size/position to set size to parent image.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Enabled up and down Arrow keys to make navigation easier.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   14 Jun 2013 06:32:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  827
 * SCR Title:  Add button to the adjust tool size/position to set size to parent image.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added a user message for lossing focus to a parent windows buttons.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Jul 2013 16:26:22   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 020.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   06 Jun 2013 08:02:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  827
 * SCR Title:  Add button to the adjust tool size/position to set size to parent image.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fix keyboard navigation using tab to move from field to field.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 May 2013 07:48:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 011.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   19 Apr 2013 10:03:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  827
 * SCR Title:  Add button to the adjust tool size/position to set size to parent image.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added support for SVAdjustToolSizePositionDialog so the tab key will work.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2013 16:42:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   10 Sep 2008 10:56:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Corrected Focus and Change Notification logic.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   16 Feb 2005 08:18:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  473
 * SCR Title:  Fix Property Tree bugs
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed bug where the first click outside the edit was eaten
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   11 Aug 2004 15:24:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  435
 * SCR Title:  Add Bulb Mode Operation to the Digital SVIM for Point Gray Dragonfly Cameras
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problem with button focus
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Jul 2004 16:20:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added a NotifyButton
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   12 Jan 2004 09:34:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed beep when pressing enter
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 Oct 2003 09:09:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for denial (undo) of commitchanges
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 15:41:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of the files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/