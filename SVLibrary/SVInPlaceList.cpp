// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVInPlaceList
// * .File Name       : $Workfile:   SVInPlaceList.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   02 Oct 2013 11:49:06  $
// ******************************************************************************

#include "stdafx.h"
#include "SVInPlaceList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVInPlaceList::SVInPlaceList(CWnd* pParent,
														 CRect& rect,
														 DWORD dwStyle,
														 UINT nID,
														 int nRow, int nCol,
														 CStringArray& Items,
														 CString sInitText,
														 UINT nFirstChar,
														 BOOL bExitOnArrows/*=FALSE*/)
{
	m_bEscape       = FALSE;
	m_nNumLines     = 4;
	m_sInitText     = sInitText;
 	m_nRow		      = nRow;
 	m_nCol          = nCol;
 	m_nLastChar     = 0; 
	m_bExitOnArrows = bExitOnArrows;
	
	// Create the combobox
 	DWORD dwComboStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | dwStyle;
	int nHeight = rect.Height();
	rect.bottom = rect.bottom + m_nNumLines*nHeight + ::GetSystemMetrics(SM_CYHSCROLL);
	if (!Create(dwComboStyle, rect, pParent, nID))
		return;
	
	// Add the strings
	for (int i = 0; i < Items.GetSize(); i++) 
		AddString(Items[i]);
	
	SetFont(pParent->GetFont());
	SetItemHeight(-1, nHeight);
	
	int nMaxLength = GetCorrectDropWidth();
	
	SetDroppedWidth(nMaxLength);
	
	SetHorizontalExtent(0); // no horz scrolling
	
	// Set the initial text to m_sInitText
	if (::IsWindow(m_hWnd) && SelectString(-1, m_sInitText) == CB_ERR) 
		SetWindowText(m_sInitText);		// No text selected, so restore what was there before
	
	// Subclass the combobox edit control if style includes CBS_DROPDOWN
	if ((dwStyle & CBS_DROPDOWNLIST) != CBS_DROPDOWNLIST)
	{
		SetFocus();
		switch (nFirstChar)
		{
			case VK_RETURN: 
				SetEditSel((int)_tcslen(m_sInitText), -1);
				return;
			case VK_BACK: 
				SetEditSel((int)_tcslen(m_sInitText), -1); 
				break;
			case VK_DOWN: 
			case VK_UP:   
			case VK_RIGHT:
			case VK_LEFT:  
			case VK_NEXT:  
			case VK_PRIOR: 
			case VK_HOME:  
			case VK_END: 
				SetEditSel(0, - 1);
				return;
			default:
				SetEditSel(0, - 1);
				break;
		}
		SendMessage(WM_CHAR, nFirstChar);
	}
	else
	{
		SetFocus();
		ShowDropDown();
	}
}

SVInPlaceList::~SVInPlaceList()
{
}


BEGIN_MESSAGE_MAP(SVInPlaceList, CComboBox)
	//{{AFX_MSG_MAP(SVInPlaceList)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	ON_WM_CTLCOLOR_REFLECT()
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(CBN_SELENDCANCEL, OnSelendcancel)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void SVInPlaceList::EndEdit()
{
	CString str;

	if (m_bEscape)
		str = m_sInitText;
	else if (::IsWindow(m_hWnd))
		GetWindowText(str);
	
	// Send Notification to parent
	LV_DISPINFO dispinfo;
	
	dispinfo.hdr.hwndFrom    = GetSafeHwnd();
	dispinfo.hdr.idFrom      = GetDlgCtrlID();
	dispinfo.hdr.code        = LVN_ENDLABELEDIT;
	
	dispinfo.item.mask       = LVIF_TEXT | LVIF_PARAM;
	dispinfo.item.iItem      = m_nRow;
	dispinfo.item.iSubItem   = m_nCol;
	dispinfo.item.pszText    = LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();
	dispinfo.item.lParam     = (LPARAM) m_nLastChar; 
	
	CWnd* pOwner = GetOwner();
	if (IsWindow(pOwner->GetSafeHwnd()))
		pOwner->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&dispinfo);
	
	// Close this window (PostNcDestroy will delete this)
	if (::IsWindow(m_hWnd))
		PostMessage(WM_CLOSE, 0, 0);
}

int SVInPlaceList::GetCorrectDropWidth()
{
	const int nMaxWidth = 200;  // don't let the box be bigger than this
	
	// Reset the dropped width
	int nNumEntries = GetCount();
	int nWidth = 0;
	CString str;
	
	CClientDC dc(this);
	int nSave = dc.SaveDC();
	dc.SelectObject(GetFont());
	
	int nScrollWidth = ::GetSystemMetrics(SM_CXVSCROLL);
	for (int i = 0; i < nNumEntries; i++)
	{
		GetLBText(i, str);
		int nLength = dc.GetTextExtent(str).cx + nScrollWidth;
		nWidth = std::max(nWidth, nLength);
	}
	
	// Add margin space to the calculations
	nWidth += dc.GetTextExtent("0").cx;
	
	dc.RestoreDC(nSave);
	
	nWidth = std::min(nWidth, nMaxWidth);
	
	return nWidth;
}

void SVInPlaceList::PostNcDestroy() 
{
	CComboBox::PostNcDestroy();
	
	delete this;
}


void SVInPlaceList::OnDropdown() 
{
	SetDroppedWidth(GetCorrectDropWidth());
}


HBRUSH SVInPlaceList::CtlColor(CDC *pDC /*pDC*/, UINT nCtlColor /*nCtlColor*/) 
{
/*
	static CBrush brush(m_crBackClr);
	pDC->SetTextColor(m_crForeClr);
	pDC->SetBkMode(TRANSPARENT);
	return (HBRUSH) brush.GetSafeHandle();
*/
	return NULL;
}


BOOL SVInPlaceList::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
	{
		UINT	nChar = static_cast<UINT>(pMsg->wParam);

		if (nChar == VK_TAB || nChar == VK_RETURN || nChar == VK_ESCAPE)
		{
			if (nChar == VK_ESCAPE) 
				m_bEscape = TRUE;

			m_nLastChar = nChar;
			GetParent()->SetFocus();	// This will destroy this window
			return TRUE;
		}
	}
	
	return CComboBox::PreTranslateMessage(pMsg);
}


void SVInPlaceList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if ((nChar == VK_PRIOR || nChar == VK_NEXT ||
			nChar == VK_DOWN  || nChar == VK_UP   ||
			nChar == VK_RIGHT || nChar == VK_LEFT) &&
			(m_bExitOnArrows || GetKeyState(VK_CONTROL) < 0))
	{
		m_nLastChar = nChar;
		GetParent()->SetFocus();
		return;
	}
	
	CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
}


void SVInPlaceList::OnKillfocus() 
{
	EndEdit();
}


void SVInPlaceList::OnSelendcancel() 
{
	if (GetFocus() == this)
		GetParent()->SetFocus();
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVInPlaceList.cpp_v  $
 * 
 *    Rev 1.1   02 Oct 2013 11:49:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platforms.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 13:37:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2.1.0   22 Oct 2010 08:10:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include problems and compilier efficiency.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Mar 2001 13:40:52   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  116
 * SCR Title:  Enhancements to SVListCtrl class and SVLibrary.Lib
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Replacement classes for the following:
 * SVFEEdit h/cpp
 * SVLVEdit h/cpp
 * SVComboEdit h/cpp
 * Modifications make the preceeding files obsolete.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Dec 2000 13:24:08   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  75
 * SCR Title:  New Library Classes
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Redefined SVPropertyItem class and Added methods to all inherited classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   06 Dec 2000 11:55:00   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  75
 * SCR Title:  New Library Classes
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Split existing classes into their own file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/