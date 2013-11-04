// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : PropertyList.cpp
// * .File Name       : $Workfile:   SVPropertyListBox.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:47:12  $
// ******************************************************************************

#include "stdafx.h"
#include "SVPropertyListBox.h"
#include "SVPropertyItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define BUTTON

/////////////////////////////////////////////////////////////////////////////
// SVPropertyListBox

SVPropertyListBox::SVPropertyListBox()
{
	m_nDivider  = 0;
	m_bTracking = FALSE;
	m_bAutoEdit = FALSE;
	SetBkColor(::GetSysColor(COLOR_WINDOW));
	SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
}

SVPropertyListBox::~SVPropertyListBox()
{
}


BEGIN_MESSAGE_MAP(SVPropertyListBox, CListBox)
	//{{AFX_MSG_MAP(SVPropertyListBox)
	ON_WM_CREATE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_CHAR()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_VSCROLL()
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_INPLACE_CONTROL, OnBeginInPlaceCtrl)
	ON_NOTIFY(LVN_ENDLABELEDIT,   IDC_INPLACE_CONTROL, OnEndInPlaceCtrl)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// SVPropertyListBox message handlers

BOOL SVPropertyListBox::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CListBox::PreCreateWindow(cs))
		return FALSE;
	
	cs.style = WS_VISIBLE | WS_CHILD | WS_TABSTOP | LBS_OWNERDRAWVARIABLE;
	
	return TRUE;
}

int SVPropertyListBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_hCursorSize = AfxGetApp()->LoadStandardCursor(IDC_SIZEWE);
	m_hCursorArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	
	return 0;
}

void SVPropertyListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if (lpMeasureItemStruct->CtlType != ODT_LISTBOX)
		return;

	TEXTMETRIC tm;
	CClientDC dc(this);	
	CFont* pFont = GetFont();
	CFont* pOldFont = dc.SelectObject(pFont);	
	dc.GetTextMetrics(&tm);
	int nItemHeight = tm.tmHeight + tm.tmExternalLeading;
	lpMeasureItemStruct->itemHeight = nItemHeight + 6;
}

int SVPropertyListBox::CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct)
{
	return 0;
}

#if defined(BUTTON)
void SVPropertyListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	BOOL bHasFocus   = FALSE;
	BOOL bIsSelected = FALSE;
	
	if (lpDrawItemStruct->CtlType != ODT_LISTBOX)
		return;
	
	SVPropertyItem *pItem = GetPropertyItem(lpDrawItemStruct->itemID);
	ASSERT(pItem != NULL);
	if (pItem->IsEditing())
		return;

	bHasFocus   = GetFocus() == this ? TRUE : FALSE;
	bIsSelected = lpDrawItemStruct->itemState & ODS_SELECTED ? TRUE : FALSE;
	// create a CDC object for drawing
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);
	
	CRect rect;
	GetClientRect(rect);
	
	CPen pen(PS_SOLID, 1, RGB(198, 198, 198));
	CPen* pOldPen = dc.SelectObject(&pen);

	int nLeftBorder = rect.left;
	dc.MoveTo(nLeftBorder, rect.top);
	dc.LineTo(nLeftBorder, rect.bottom);

	rect = lpDrawItemStruct->rcItem;

	int nBottom = rect.bottom - 1;

	dc.MoveTo(nLeftBorder, nBottom);
	dc.LineTo(rect.right, nBottom);

	if (m_nDivider == 0)
		m_nDivider = rect.Width() / 2;
	nLeftBorder += m_nDivider;

	dc.MoveTo(nLeftBorder, rect.top);
	dc.LineTo(nLeftBorder, nBottom);

	rect = lpDrawItemStruct->rcItem;
	rect.DeflateRect(1, 1);
	rect.right = nLeftBorder;

	COLORREF crOldBkColor, crOldTextColor;

	// Draws the button in its various states
	// create a CBrush for the background colour of the button
	CBrush theBrush(GetSysColor(COLOR_3DFACE));
	CBrush *poldBrush = dc.SelectObject(&theBrush);
	CPen *poldPen, thePen;
	if (bIsSelected && bHasFocus)	// Draw button as selected
	{
		// create the pen used to draw the border
		thePen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
		poldPen = dc.SelectObject(&thePen);
					
		// draw the sunken border
		dc.Rectangle(rect);
					
		crOldTextColor = dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));
		crOldBkColor   = dc.SetBkColor(GetSysColor(COLOR_3DFACE));
		rect.DeflateRect(3, 0);
		dc.DrawText(pItem->GetName(), &rect, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
	}
	else	// draw button as normal
	{
		// draw the outside border
		thePen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DFACE));
		poldPen = dc.SelectObject(&thePen);
		dc.Rectangle(&rect);
		dc.SelectObject(poldPen);
		thePen.DeleteObject();
					
		// draw the top and left sides
		thePen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DHILIGHT));
		poldPen = dc.SelectObject(&thePen);
		dc.MoveTo(rect.left + 1, rect.bottom - 1);
		dc.LineTo(rect.left + 1, rect.top + 1);
		dc.LineTo(rect.right - 2, rect.top + 1);
		dc.SelectObject(poldPen);
		thePen.DeleteObject();
					
		// draw the bottom and right sides (top edge)
		thePen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
		poldPen = dc.SelectObject(&thePen);
		dc.LineTo(rect.right - 2, rect.bottom - 2);
		dc.LineTo(rect.left + 1, rect.bottom - 2);
		dc.SelectObject(poldPen);
		thePen.DeleteObject();
					
		// draw the bottom and right sides (bottom edge)
		thePen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DDKSHADOW));
		poldPen = dc.SelectObject(&thePen);
		dc.MoveTo(rect.right - 1, rect.top);
		dc.LineTo(rect.right - 1, rect.bottom - 1);
		dc.LineTo(rect.left, rect.bottom - 1);
					
		rect.DeflateRect(3, 0);
		crOldTextColor = dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));
		crOldBkColor   = dc.SetBkColor(GetSysColor(COLOR_3DFACE));
		dc.DrawText(pItem->GetName(), &rect, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
	}

	rect = lpDrawItemStruct->rcItem;
	GetItemValueRect(rect);
	dc.SetBkColor(crOldBkColor);
	dc.SetTextColor(crOldTextColor);
	pItem->Draw(&dc, rect);
				
	// reset the DC
	dc.SelectObject(poldBrush);
	dc.SelectObject(poldPen);
	thePen.DeleteObject();
	theBrush.DeleteObject();
				
	dc.Detach();
}

#else

void SVPropertyListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	BOOL bHasFocus   = FALSE;
	BOOL bIsSelected = FALSE;
	
	if (lpDrawItemStruct->CtlType != ODT_LISTBOX)
		return;
	
	SVPropertyItem *pItem = GetPropertyItem(lpDrawItemStruct->itemID);
	ASSERT(pItem != NULL);
	if (pItem->IsEditing())
		return;
	
	bHasFocus   = GetFocus() == this ? TRUE : FALSE;
	bIsSelected = lpDrawItemStruct->itemState & ODS_SELECTED ? TRUE : FALSE;
				
	// create a CDC object for drawing
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);
				
	CRect rect;
	GetClientRect(rect);
				
	CPen pen(PS_SOLID, 1, RGB(198, 198, 198));
	CPen* pOldPen = dc.SelectObject(&pen);
				
	int nLeftBorder = rect.left;
	dc.MoveTo(nLeftBorder, rect.top);
	dc.LineTo(nLeftBorder, rect.bottom);
				
	rect = lpDrawItemStruct->rcItem;
				
	int nBottom = rect.bottom - 1;
				
	dc.MoveTo(nLeftBorder, nBottom);
	dc.LineTo(rect.right, nBottom);
				
	if (m_nDivider == 0)
		m_nDivider = rect.Width() / 2;
	nLeftBorder += m_nDivider;
				
	dc.MoveTo(nLeftBorder, rect.top);
	dc.LineTo(nLeftBorder, nBottom);
				
	rect.left   += 1;
	rect.bottom -= 1;
	rect.right   = nLeftBorder;
				
	// Calculate the colors to use
	COLORREF crBackground   = GetSysColor(bIsSelected && bHasFocus ? COLOR_HIGHLIGHT : COLOR_BTNFACE);
	COLORREF crText         = GetSysColor(bIsSelected && bHasFocus ? COLOR_HIGHLIGHTTEXT : COLOR_BTNTEXT);
	COLORREF crOldBkColor   = dc.SetBkColor(crBackground);
	COLORREF crOldTextColor = dc.SetTextColor(crText);
/*	
	LOGFONT	lf; GetFont()->GetLogFont(&lf);
	lf.lfWeight = FW_BOLD;
	CFont font; font.CreateFontIndirect(&lf);
	dc.SelectObject(font);
*/
	rect.DeflateRect(1, 1);
	dc.FillSolidRect(rect, crBackground);
	rect.DeflateRect(2, 2);
	dc.DrawText(pItem->GetName(), &rect, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
				
	dc.SelectObject(GetFont());
				
	rect = lpDrawItemStruct->rcItem;
	GetItemValueRect(rect);
	dc.SetBkColor(m_crBackground);
	dc.SetTextColor(m_crText);
	pItem->Draw(&dc, rect);
	dc.Detach();
}
#endif


BOOL SVPropertyListBox::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (GetFocus() == this)
		{
			switch (pMsg->wParam)
			{
				case VK_F8:
					if (GetCurSel() != LB_ERR)
						OnEditItem(GetCurSel(), CPoint(-1, -1), VK_RETURN);
					return TRUE;
					break;
				case VK_RIGHT:
				case VK_RETURN:	// edit itemdata
					if (GetCurSel() != LB_ERR)
						OnEditItem(GetCurSel(), CPoint(-1, -1), pMsg->wParam);
					return TRUE;
					break;
				default:
					return CListBox::PreTranslateMessage(pMsg);
					break;
			}
		}
	}

	return CListBox::PreTranslateMessage(pMsg);
}


void SVPropertyListBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (IsCharAlpha(nChar))
	{
		LookupPropertyItem(nChar);
	}
	else
		CListBox::OnChar(nChar, nRepCnt, nFlags);
}


void SVPropertyListBox::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	BOOL	bOutside;
	int iItem = CListBox::ItemFromPoint(point, bOutside);
	if (!bOutside)
	{
		OnEditItem(iItem, point, VK_LBUTTON);
	}
}


void SVPropertyListBox::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_bTracking)
	{
		// if columns were being resized then this indicates
		// that mouse is up so resizing is done.  Need to redraw
		// columns to reflect their new widths.
		
		m_bTracking = FALSE;
		// if mouse was captured then release it
		if (GetCapture() == this)
			::ReleaseCapture();
		
		::ClipCursor(NULL);
		
		CClientDC dc(this);
		InvertLine(&dc, CPoint(point.x, m_nDivTop), CPoint(point.x, m_nDivBtm));
		// set the divider position to the new value
		m_nDivider = point.x;
		Invalidate();
	}
	else
	{
		CRect	rect;
		BOOL	loc;
		
		int i = ItemFromPoint(point, loc);
		if (!loc)
		{
			SetCurSel(i);
		}
		
		CListBox::OnLButtonUp(nFlags, point);
	}
}


void SVPropertyListBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (GetFocus() != this)
		SetFocus();

	if ((point.x >= m_nDivider - 5) && (point.x <= m_nDivider + 5))
	{
		// if mouse clicked on divider line, then start resizing
		::SetCursor(m_hCursorSize);
		
		CRect windowRect;
		GetWindowRect(windowRect);
		windowRect.left += 10; windowRect.right -= 10;
		// do not let mouse leave the list box boundary
		::ClipCursor(windowRect);
		
		CRect clientRect;
		GetClientRect(clientRect);
		
		m_bTracking = TRUE;
		m_nDivTop = clientRect.top;
		m_nDivBtm = clientRect.bottom;
		m_nOldDivX = point.x;
		
		CClientDC dc(this);
		InvertLine(&dc, CPoint(m_nOldDivX, m_nDivTop), CPoint(m_nOldDivX, m_nDivBtm));
		
		// capture the mouse
		SetCapture();
	}
	else
	{
		m_bTracking = FALSE;
		CListBox::OnLButtonDown(nFlags, point);
	}
}


void SVPropertyListBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	CWnd* wnd = GetFocus();
	if (m_bTracking)
	{
		// move divider line to the mouse pos. if columns are
		// currently being resized
		CClientDC dc(this);
		// remove old divider line
		InvertLine(&dc, CPoint(m_nOldDivX, m_nDivTop), CPoint(m_nOldDivX, m_nDivBtm));
		// draw new divider line
		InvertLine(&dc, CPoint(point.x, m_nDivTop), CPoint(point.x, m_nDivBtm));
		m_nOldDivX = point.x;
	}
	else if ((point.x >= m_nDivider - 5) && (point.x <= m_nDivider + 5))
		// set the cursor to a sizing cursor if the cursor is over the row divider
		::SetCursor(m_hCursorSize);
	else
		CListBox::OnMouseMove(nFlags, point);
}


void SVPropertyListBox::InvertLine(CDC* pDC, CPoint ptFrom, CPoint ptTo)
{
	int nOldMode = pDC->SetROP2(R2_NOT);
	
	pDC->MoveTo(ptFrom);
	pDC->LineTo(ptTo);
	
	pDC->SetROP2(nOldMode);
}


void SVPropertyListBox::PreSubclassWindow() 
{
	m_nDivider = 0;
	m_bTracking = FALSE;
	
	m_hCursorSize = AfxGetApp()->LoadStandardCursor(IDC_SIZEWE);
	m_hCursorArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
}


SVPropertyItem* SVPropertyListBox::GetPropertyItem(int nItem) const
{
	if (nItem > -1 && nItem < GetCount())
		return reinterpret_cast < SVPropertyItem*>(GetItemDataPtr(nItem));
	return NULL;
}


void SVPropertyListBox::GetItemValueRect(CRect& rect)
{
	if (m_nDivider == 0)
		m_nDivider = rect.Width() / 2;
	rect.left += m_nDivider;
	rect.DeflateRect(3, 3);
}


void SVPropertyListBox::ResetContent(BOOL bRemoveItems/*=FALSE*/)
{
	if (bRemoveItems)
	{
		for (int i = 0; i < GetCount(); i++)
		{
			SVPropertyItem* pItem = GetPropertyItem(i);
			delete pItem;
		}
	}
	
	CListBox::ResetContent();
}


BOOL SVPropertyListBox::LookupPropertyItem(char nStartChar)
{
	int nCount = GetCount();

	if (nCount)
	{
		int nCurrItem = GetCurSel() + 1;

		if (nCurrItem == nCount)
			nCurrItem = 0;

		int nFindIndex = FindPropertyItem(nStartChar, nCurrItem, nCount - nCurrItem);

		if (nFindIndex == -1 && nCurrItem > 0)
			nFindIndex = FindPropertyItem(nStartChar, 0, nCurrItem);

		if (nFindIndex != -1)
		{
			SetCurSel(nFindIndex);
			return TRUE;
		}
	}

	return FALSE;
}


int SVPropertyListBox::FindPropertyItem(char nStartChar, int nFromIndex, int nCount)
{
	const CString strStartChar(nStartChar);

	while (nCount--)
	{
		SVPropertyItem* pItem = reinterpret_cast < SVPropertyItem*>(GetItemData(nFromIndex));
		ASSERT(pItem != NULL);
			
		if (pItem->GetName().Left(1).CompareNoCase(strStartChar) == 0)
			return nFromIndex;

		nFromIndex++;
	}

	return -1;
}


int SVPropertyListBox::AddPropertyItem(SVPropertyItem* pItem)
{
	int nIndex = AddString(_T(""));
	SetItemDataPtr(nIndex, pItem);
	return nIndex;
}


void SVPropertyListBox::OnBeginInPlaceCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO *plvDispInfo = (LV_DISPINFO*)pNMHDR;
	LVITEM			*plvItem     = &plvDispInfo->item;
	
	// In case OnBeginInPlaceCtrl called as window is being destroyed
	if (!IsWindow(GetSafeHwnd()))
		return;
	
	OnEditItem(plvItem->iItem, CPoint(-1, -1), (UINT)plvItem->lParam);
	
	*pResult = 0;
}


void SVPropertyListBox::OnEditItem(int nItem, CPoint point, UINT nChar)
{
	SVPropertyItem *pItem = GetPropertyItem(nItem);
	ASSERT(pItem != NULL);
	
	CRect rect;
	GetItemRect(nItem, rect);
	GetItemValueRect(rect);
	
	pItem->Edit(this, nItem, rect, point, IDC_INPLACE_CONTROL, nChar);
}


void SVPropertyListBox::OnEndInPlaceCtrl(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO *plvDispInfo = (LV_DISPINFO *)pNMHDR;
	LVITEM			*plvItem     = &plvDispInfo->item;

	// In case OnEndInPlaceCtrl called as window is being destroyed
	if (!IsWindow(GetSafeHwnd()))
		return;

	OnEndEditItem(plvItem);
	
	*pResult = 0;
}


void SVPropertyListBox::OnEndEditItem(LPLVITEM plvItem)
{
	SVPropertyItem *pItem = GetPropertyItem(plvItem->iItem);
	ASSERT(pItem != NULL);

	pItem->OnEndEdit(plvItem);
	
	int iItem = GetCurSel();
	switch (plvItem->lParam)
  {
		case VK_LEFT:
		case VK_UP:
			if (--iItem < 0)
				iItem = 0;
			break;
		case VK_TAB:
		case VK_RIGHT:
		case VK_DOWN:
			if (++iItem == GetCount())
				iItem--;
			break;
		default:
			break;
	}
	SetCurSel(iItem);
}


void SVPropertyListBox::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (GetFocus() != this)
		SetFocus();
	
	CListBox::OnVScroll(nSBCode, nPos, pScrollBar);
}

void SVPropertyListBox::OnSelchange() 
{
	if (m_bAutoEdit)
		OnEditItem(GetCurSel(), CPoint(-1, -1), VK_RIGHT);
}


COLORREF SVPropertyListBox::SetBkColor(COLORREF rgb)
{
	COLORREF crOld = m_crBackground;
	m_crBackground = rgb;

	return crOld;
}


COLORREF SVPropertyListBox::SetTextColor(COLORREF rgb)
{
	COLORREF crOld = m_crText;
	m_crText       = rgb;

	return crOld;
}


COLORREF SVPropertyListBox::GetBkColor(void)
{
	return m_crBackground;
}


COLORREF SVPropertyListBox::GetTextColor(void)
{
	return m_crText;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVPropertyListBox.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:47:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Mar 2001 14:11:08   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  116
 * SCR Title:  Enhancements to SVListCtrl class and SVLibrary.Lib
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Corrections made for portability and better utilization by SVFocus. This is also used by the SVReport project for SmithKline Beechem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Dec 2000 13:24:06   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  75
 * SCR Title:  New Library Classes
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Redefined SVPropertyItem class and Added methods to all inherited classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 Dec 2000 11:57:36   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  75
 * SCR Title:  New Library Classes
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Split out several classes into their own files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   05 Dec 2000 09:27:40   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  75
 * SCR Title:  New Library Classes
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   SVPropertyListBox and associated item classes.
 * SVPropertyItem
 * SVPropertyItemCombo
 * SVPropertyItemEdit
 * SVPropertyItemFile
 * SVPropertyItemFont
 * 
 * Initial Release
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/