// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVListCtrl
// * .File Name       : $Workfile:   SVListCtrl.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   01 Oct 2013 09:57:46  $
// ******************************************************************************

#include "stdafx.h"
#include "SVListCtrl.h"
#include "SVInPlaceEdit.h"

#define OFFSET_FIRST	3
#define OFFSET_OTHER	6
#define IDC_EDITCTRL 0x1234

namespace SvMc
{
	SVListCtrl::SVListCtrl()
	{
		m_CurItem     = 0;
		m_CurSubItem  = 1;
		m_bHasFocus   = FALSE;
		m_bIsDragging = FALSE;
	}


	SVListCtrl::~SVListCtrl()
	{
	}


	BEGIN_MESSAGE_MAP(SVListCtrl, CListCtrl)
		//{{AFX_MSG_MAP(SVListCtrl)
		ON_WM_CREATE()
		ON_WM_HSCROLL()
		ON_WM_LBUTTONDOWN()
		ON_WM_MOUSEMOVE()
		ON_WM_TIMER()
		ON_WM_VSCROLL()
		ON_WM_LBUTTONUP()
		ON_WM_MEASUREITEM_REFLECT()
		ON_WM_DRAWITEM_REFLECT()
		ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBegindrag)
		ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
		ON_WM_CHAR()
		ON_WM_KILLFOCUS()
		ON_WM_SETFOCUS()
		ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginlabeledit)
		//}}AFX_MSG_MAP
		ON_MESSAGE(WM_SETFONT, OnSetFont)
		ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_EDITCTRL, OnBeginlabeledit)
		ON_NOTIFY(LVN_ENDLABELEDIT,   IDC_EDITCTRL, OnEndlabeledit)
	END_MESSAGE_MAP()


	/////////////////////////////////////////////////////////////////////////////
	// SVListCtrl message handlers

	BOOL SVListCtrl::PreCreateWindow(CREATESTRUCT& cs) 
	{
		// default is report view and full row selection
		cs.style &= ~LVS_TYPEMASK;
		cs.style &= ~LVS_SHOWSELALWAYS;
		cs.style |= LVS_REPORT | LVS_OWNERDRAWFIXED;
		cs.style |= LVS_EDITLABELS;

		return CListCtrl::PreCreateWindow(cs);
	}


	void SVListCtrl::PreSubclassWindow() 
	{
		DWORD	dwRemove = 0, dwAdd = 0;

#if defined(SVListCtrlEx)
		// Use the following line to generate grid lines within the SVListCtrl.
		ListView_SetExtendedListViewStyleEx(m_hWnd,LVS_EX_GRIDLINES,LVS_EX_GRIDLINES);	
#endif

		// Modify the window style
		dwRemove |= LVS_TYPEMASK;
		dwRemove |= LVS_SHOWSELALWAYS;
		dwAdd    |= LVS_REPORT;
		dwAdd    |= LVS_SINGLESEL;
		dwAdd    |= LVS_SHAREIMAGELISTS;
		dwAdd    |= LVS_OWNERDRAWFIXED;
		dwAdd    |= LVS_EDITLABELS;
		ModifyStyle(dwRemove, dwAdd);

		// The following statement will force the MeasureItem method to be called
		OnSetFont(0, 0);

		CListCtrl::PreSubclassWindow();
	}


	int SVListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
	{
		if (CListCtrl::OnCreate(lpCreateStruct) == -1)
			return -1;

		return 0;
	}


	BOOL SVListCtrl::PreTranslateMessage(MSG* pMsg) 
	{
		if (pMsg->message == WM_KEYDOWN)
		{
			if (GetFocus() == this)
			{
				switch (pMsg->wParam)
				{
				case VK_LEFT:
					{
						// Decrement the order number.
						m_CurSubItem--;
						if (m_CurSubItem < 1) 
							m_CurSubItem = 1;
						else
						{
							CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
							// Make the column visible.
							// We have to take into account that the header may be reordered.
							MakeColumnVisible(Header_OrderToIndex(pHeader->m_hWnd, m_CurSubItem));
							// Invalidate the item.
							m_CurItem = GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
							if (m_CurItem != -1)
							{
								CRect rcBounds;
								GetItemRect(m_CurItem, rcBounds, LVIR_BOUNDS);
								InvalidateRect(&rcBounds);
								UpdateWindow();
							}
						}
						return TRUE;
					}
				case VK_RIGHT:
					{
						// Increment the order number.
						m_CurSubItem++;
						CHeaderCtrl* pHeader = (CHeaderCtrl*) GetDlgItem(0);
						int nColumnCount = pHeader->GetItemCount();
						// Don't go beyond the last column.
						if (m_CurSubItem > nColumnCount -1) 
							m_CurSubItem = nColumnCount - 1;
						else
						{
							MakeColumnVisible(Header_OrderToIndex(pHeader->m_hWnd, m_CurSubItem));
							m_CurItem = GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
							// Invalidate the item.
							if (m_CurItem != -1)
							{
								CRect rcBounds;
								GetItemRect(m_CurItem, rcBounds, LVIR_BOUNDS);
								InvalidateRect(&rcBounds);
								UpdateWindow();
							}
						}
						return TRUE;
					}
				case VK_RETURN:	// edit itemdata
					{
						m_CurItem = GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
						if (m_CurSubItem != -1 && m_CurItem != -1)
						{
							CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
							int iSubItem = Header_OrderToIndex(pHeader->m_hWnd, m_CurSubItem);
							OnEditItem(m_CurItem, iSubItem, CPoint(-1, -1), VK_RETURN);
							return TRUE;
						}
						break;
					}
#if !defined(SVListCtrlEx)
				case VK_F8:
					{
						m_CurItem    = GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
						m_CurSubItem = IndexToOrder(1);

						if (m_CurSubItem != -1 && m_CurItem != -1)
						{
							CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
							int iSubItem = Header_OrderToIndex(pHeader->m_hWnd, m_CurSubItem);
							OnEditItem(m_CurItem, iSubItem, CPoint(-1, -1), VK_SPACE);
							return TRUE;
						}
						break;
					}
				case VK_F9:
					{
						m_CurItem    = GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
						m_CurSubItem = IndexToOrder(2);

						if (m_CurSubItem != -1 && m_CurItem != -1)
						{
							CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
							int iSubItem = Header_OrderToIndex(pHeader->m_hWnd, m_CurSubItem);
							OnEditItem(m_CurItem, iSubItem, CPoint(-1, -1), VK_SPACE);
							return TRUE;
						}
						break;
					}
#endif
				default:
					break;
				}
			}
		}

		return CListCtrl::PreTranslateMessage(pMsg);
	}


	void SVListCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
	{
		m_CurItem = GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

#if defined(SVListCtrlEx)	
		if (m_CurSubItem != -1 && m_CurItem != -1)
		{
			CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
			int iSubItem = Header_OrderToIndex(pHeader->m_hWnd, m_CurSubItem);
			OnEditItem(m_CurItem, iSubItem, CPoint(-1, -1), nChar);
			return;
		}
#endif

		CListCtrl::OnChar(nChar, nRepCnt, nFlags);
	}


	void SVListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
	{
		if (lpDrawItemStruct->CtlType != ODT_LISTVIEW)
			return;

		// Get item image and state info
		LV_ITEM lvi;
		lvi.mask      = LVIF_IMAGE | LVIF_STATE;
		lvi.iItem     = lpDrawItemStruct->itemID;
		lvi.iSubItem  = 0;
		lvi.stateMask = 0xFFFF;		// get all state flags
		GetItem(&lvi);

		// Should the item be highlighted
		BOOL bHighlight = ((lvi.state & LVIS_DROPHILITED) ||
			((lvi.state & LVIS_SELECTED)    &&
			((GetFocus() == this)           ||
			(GetStyle() & LVS_SHOWSELALWAYS))));

		CRect rcLabel;
		GetItemRect(lpDrawItemStruct->itemID, rcLabel, LVIR_LABEL);

		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		ASSERT(pDC);

		CRect rcClipBox;
		pDC->GetClipBox(rcClipBox);

		COLORREF crBackground;
		COLORREF crText;

		// Calculate the colors to use
		crBackground = ::GetSysColor(bHighlight ? COLOR_HIGHLIGHT     : COLOR_WINDOW);
		crText       = ::GetSysColor(bHighlight ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT);

		CRect rc;
		GetItemRect(lpDrawItemStruct->itemID, rc, LVIR_BOUNDS);
		pDC->SetBkColor(crBackground);

		// fill background color
		CRect rcClip(lpDrawItemStruct->rcItem);
		ExtTextOut(pDC->GetSafeHdc(), 0, 0, ETO_OPAQUE, rcClip, nullptr, 0, nullptr);

		// draw color in first col if any
		rcClip.right = rcLabel.right;
		// draw selection rect in 1.st col if selected
		if (bHighlight && (m_CurSubItem == 0))
		{
			rcClip.DeflateRect(1, 1);
			CBrush br(GetCellRGB());
			pDC->FillRect(rcClip, &br);
			pDC->DrawFocusRect(rcClip);
		}

		// if checkbox style
		UINT nStateImageMask = lvi.state & LVIS_STATEIMAGEMASK;

		// draw 1. item	
		CRect rcItem(lpDrawItemStruct->rcItem);
		GetItemRect(lpDrawItemStruct->itemID, rcItem, LVIR_LABEL);
		rcLabel = rcItem;
		rcLabel.DeflateRect(OFFSET_FIRST, 0);

		pDC->SetBkColor (crBackground);
		pDC->SetTextColor (crText);
		CString strText = GetItemText(lpDrawItemStruct->itemID, 0);
		pDC->DrawText(strText, - 1, rcLabel, DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_EXTERNALLEADING | DT_END_ELLIPSIS);

		// draw subitems..
		LV_COLUMN lvc;
		lvc.mask = LVCF_FMT | LVCF_WIDTH;
		for (int nColumn = 1; GetColumn(nColumn, &lvc); nColumn++)
		{
			rcItem.left = rcItem.right;
			rcItem.right += lvc.cx;
			if (rcItem.left < rcClipBox.right && rcItem.right > rcClipBox.left && rcItem.right > rcItem.left)
			{
				CString strText= GetItemText(lpDrawItemStruct->itemID, nColumn);
				if (strText.IsEmpty())
					continue;

				UINT nJustify = DT_LEFT;

				switch (lvc.fmt&LVCFMT_JUSTIFYMASK)
				{
				case LVCFMT_RIGHT:
					nJustify = DT_RIGHT;
					break;
				case LVCFMT_CENTER:
					nJustify = DT_CENTER;
					break;
				default:
					nJustify = DT_LEFT;
					break;
				}

				rcLabel = rcItem;
				rcLabel.DeflateRect(OFFSET_OTHER, 0);
				if (bHighlight)
					DrawFocusCell(pDC, lpDrawItemStruct->itemID, nColumn);
				if (!strText.IsEmpty())
					pDC->DrawText(strText, - 1, rcLabel, nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_EXTERNALLEADING | DT_END_ELLIPSIS);
			}
		}
	}


	void SVListCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
	{
		if (lpMeasureItemStruct->CtlType != ODT_LISTVIEW)
			return;

		TEXTMETRIC tm;
		CClientDC dc(this);	
		CFont* pFont = GetFont();
		CFont* pOldFont = dc.SelectObject(pFont);	
		dc.GetTextMetrics(&tm);
		int nItemHeight = tm.tmHeight + tm.tmExternalLeading;
		lpMeasureItemStruct->itemHeight = nItemHeight + 6;
	}


	void SVListCtrl::OnTimer(UINT_PTR nIDEvent) 
	{
		CListCtrl::OnTimer(nIDEvent);

		if (nIDEvent == 1)
		{
			if (CWnd::GetCapture() != this)
			{
				SetItemState(m_nDragTarget, 0, LVIS_DROPHILITED);
				m_bIsDragging = 0;
				CImageList::DragLeave(this);
				CImageList::EndDrag();
				ReleaseCapture();
				InvalidateRect(nullptr);
				UpdateWindow();
				KillTimer(1);
				return;
			}

			SetTimer(1, 300, nullptr);// reset timer
			DWORD dwPos = ::GetMessagePos();
			CPoint ptList(LOWORD(dwPos), HIWORD(dwPos));
			ScreenToClient(&ptList);

			CRect rect;
			GetClientRect(rect);
			int cy = rect.Height();
			//
			// perform autoscroll if the cursor is near the top or bottom.
			//

			// Hover test 
			CImageList::DragMove(ptList);
			UINT uHitTest = LVHT_ONITEM;
			m_nDragTarget = HitTest(ptList, &uHitTest);

			KillTimer(1);
		}
	}


	void SVListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
	{
		if (GetFocus() != this) 
			SetFocus();

		LVHITTESTINFO ht;
		ht.pt = point;

		SubItemHitTest(&ht);

		if (OnItemLButtonDown(ht))
		{
			if (ht.iItem!=-1 && ht.iSubItem > 0)
			{
				m_CurSubItem = IndexToOrder(ht.iSubItem);
				CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
				// Make the column fully visible.
				MakeColumnVisible(Header_OrderToIndex(pHeader->m_hWnd, m_CurSubItem));
				CListCtrl::OnLButtonDown(nFlags, point);
				OnControlLButtonDown(nFlags, point, ht);
				// update row anyway for selection bar
				CRect rc;
				GetItemRect(ht.iItem, rc, LVIR_BOUNDS);
				InvalidateRect(rc);
				UpdateWindow();
				return;
			}
		}

		CListCtrl::OnLButtonDown(nFlags, point);
	}


	void SVListCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
	{
		if (m_bIsDragging == TRUE)
		{
			KillTimer(1);
			ReleaseCapture();
			m_bIsDragging = FALSE;
			SetItemState(m_nDragTarget, 0, LVIS_DROPHILITED);
			if ((m_nDragTarget != -1) && (m_nDragTarget != m_nDragItem) && (m_nDragItem!=-1))// no drop on me self
			{
			}
		}
		else
			CListCtrl::OnLButtonUp(nFlags, point);
	}


	void SVListCtrl::OnMouseMove(UINT nFlags, CPoint point) 
	{
		if (m_bIsDragging)
		{
			KillTimer(1);
			if (CWnd::GetCapture() != this)
				m_bIsDragging = 0;

			if (nFlags == MK_RBUTTON || nFlags == MK_MBUTTON)
				m_bIsDragging = 0;

			if (GetKeyState(VK_ESCAPE) < 0)
				m_bIsDragging = 0;

			if (!m_bIsDragging)// why not put this in a funtion :)
			{
				SetItemState (m_nDragTarget, 0, LVIS_DROPHILITED);
				CImageList::DragLeave(this);
				CImageList::EndDrag();
				ReleaseCapture();
				InvalidateRect(nullptr);
				UpdateWindow();
			}	
			else
			{
				CPoint ptList(point);
				MapWindowPoints(this, &ptList, 1);
				CImageList::DragMove(ptList);
				UINT uHitTest = LVHT_ONITEM;
				m_nDragTarget = HitTest(ptList, &uHitTest);
				// try turn off hilight for previous DROPHILITED state
				int nPrev = GetNextItem(-1, LVNI_DROPHILITED);
				if (nPrev != m_nDragTarget)// prevents flicker 
					SetItemState(nPrev, 0, LVIS_DROPHILITED);

				CRect rect;
				GetClientRect (rect);
				int cy = rect.Height();
				if (m_nDragTarget != -1)
				{
					SetItemState(m_nDragTarget, LVIS_DROPHILITED, LVIS_DROPHILITED);
					SetTimer(1, 300, nullptr);
				}
			}
		}

		CListCtrl::OnMouseMove(nFlags, point);
	}


	BOOL SVListCtrl::OnItemLButtonDown(LVHITTESTINFO& ht)
	{
		return TRUE;
	}


	void SVListCtrl::OnControlLButtonDown(UINT nFlags, CPoint point, LVHITTESTINFO& ht)
	{
		OnEditItem(ht.iItem, ht.iSubItem, point, VK_LBUTTON);
	}


	void SVListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
	{
		if (GetFocus() != this) 
			SetFocus();

		CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
	}


	void SVListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
	{
		if (GetFocus() != this) 
			SetFocus();

		CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
	}


	void SVListCtrl::DrawFocusCell(CDC *pDC, int nItem, int iSubItem)
	{
		if (iSubItem == m_CurSubItem)
		{
			CRect rect;

			GetSubItemRect(nItem, iSubItem, LVIR_BOUNDS, rect);

			if (iSubItem == 0)
				GetItemRect(iSubItem, rect, LVIR_LABEL);
			rect.DeflateRect(1, 1);

#if defined(SVListCtrlEx)
			CBrush br(GetCellRGB());
			pDC->FillRect(rect, &br);
#endif

			pDC->DrawFocusRect(rect);
		}
	}


	COLORREF SVListCtrl::GetCellRGB()
	{
		return ::GetSysColor(CTLCOLOR_EDIT);
	}


	void SVListCtrl::MakeColumnVisible(int nCol)
	{
		if (nCol < 0)
			return;
		// Get the order array to total the column offset.
		CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
		int nColCount = pHeader->GetItemCount();
		ASSERT(nCol < nColCount);
		int *pOrderarray = new int[nColCount];
		Header_GetOrderArray(pHeader->m_hWnd, nColCount, pOrderarray);
		// Get the column offset
		int offset = 0;
		for (int i = 0; pOrderarray[i] != nCol; i++)
			offset += GetColumnWidth(pOrderarray[i]);

		int colwidth = GetColumnWidth(nCol);
		delete[] pOrderarray;

		CRect rect;
		GetItemRect(0, &rect, LVIR_BOUNDS);
		// Now scroll if we need to show the column
		CRect rcClient;
		GetClientRect(&rcClient);
		if (offset + rect.left < 0 || offset + colwidth + rect.left > rcClient.right)
		{
			CSize size(offset + rect.left, 0);
			Scroll(size);
			InvalidateRect(nullptr);
			UpdateWindow();
		}
	}


	int SVListCtrl::IndexToOrder(int iIndex)
	{
		// This translates a column index value to a column order value.
		CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
		int nColCount = pHeader->GetItemCount();
		int *pOrderarray = new int[nColCount];
		Header_GetOrderArray(pHeader->m_hWnd, nColCount, pOrderarray);
		for (int i = 0; i < nColCount; i++)
		{
			if (pOrderarray[i] == iIndex)
			{
				delete[] pOrderarray;
				return i;
			}
		}
		delete[] pOrderarray;
		return -1;
	}


	BOOL SVListCtrl::IsReadOnly(int nItem, int nSubItem/*=0*/)
	{
		if (nSubItem)
			return FALSE;
		return TRUE;
	}


	void SVListCtrl::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

		CPoint	cursor;
		GetCursorPos(&cursor);
		ScreenToClient(&cursor);

		LV_COLUMN	lvc;
		lvc.mask = LVCF_WIDTH;

		CRect	rect;
		GetItemRect(pDispInfo->item.iItem, &rect, LVIR_BOUNDS);

		if (rect.PtInRect(cursor))
		{
			CHeaderCtrl* pHeaderCtrl = GetHeaderCtrl();
			if( pHeaderCtrl )
			{
				int iNumCols = pHeaderCtrl->GetItemCount();

				for (int nCol = 1; nCol < iNumCols && GetSubItemRect(pDispInfo->item.iItem, nCol, LVIR_BOUNDS, rect) ; nCol++)
				{
					if (rect.PtInRect(cursor))
					{
						m_CurSubItem = nCol;
						OnEditItem(pDispInfo->item.iItem, nCol, cursor, VK_LBUTTON);
						break;
					}
				}
			}
		}
		*pResult = 1;
	}

	void SVListCtrl::OnEditItem(int iItem, int iSubItem, CPoint point, UINT nChar)
	{
		if (IsReadOnly(iItem, iSubItem))
			return;

		CRect rect;
		int offset = 0;
		if (!EnsureVisible(iItem, TRUE)) 
			return;

		GetSubItemRect(iItem, iSubItem, LVIR_BOUNDS, rect);

		// Now scroll if we need to expose the column
		CRect rcClient;
		GetClientRect(rcClient);
		if (offset + rect.left < 0 || offset + rect.left > rcClient.right)
		{
			CSize size(offset + rect.left, 0);		
			Scroll(size);
			rect.left -= size.cx;
		}
		rect.left += offset;	
		rect.right = rect.left + GetColumnWidth(iSubItem);
		if (rect.right > rcClient.right) 
			rect.right = rcClient.right;

		// Get Column alignment	
		LV_COLUMN lvcol;
		lvcol.mask = LVCF_FMT;
		GetColumn(iSubItem, &lvcol);

		DWORD dwStyle;
		if ((lvcol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT)
			dwStyle = ES_LEFT;
		else if ((lvcol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
			dwStyle = ES_RIGHT;
		else
			dwStyle = ES_CENTER;	

		dwStyle |= ES_AUTOHSCROLL;
		rect.DeflateRect(1, 1, 1, 1);
		CEdit *pEdit = new SVInPlaceEdit(this, rect, dwStyle, IDC_EDITCTRL, iItem, iSubItem, GetItemText(iItem, iSubItem), nChar);
	}	


	void SVListCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		LV_DISPINFO* plvDispInfo = (LV_DISPINFO*)pNMHDR;
		LV_ITEM* plvItem = &plvDispInfo->item;

		OnEndEditItem(plvItem);
		*pResult = 0;
	}


	void SVListCtrl::OnEndEditItem(LPLVITEM plvItem)
	{
		if (plvItem->mask & LVIF_TEXT)
		{
			CString strText = plvItem->pszText;
			SetItemText(plvItem->iItem, plvItem->iSubItem, strText);
		}

		switch (plvItem->lParam)
		{
		case VK_UP:
			{
				m_CurItem = GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
				int above = GetNextItem(m_CurItem, LVNI_ABOVE);

				if (above != -1)
				{
					SetItemState(above, LVIS_SELECTED | LVIS_FOCUSED, -1);
					m_CurItem = above;
				}
				break;
			}
		case VK_DOWN:
			{
				m_CurItem = GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
				int below = GetNextItem(m_CurItem, LVNI_BELOW);

				if (below != -1)
				{
					SetItemState(below, LVIS_SELECTED | LVIS_FOCUSED, -1);
					m_CurItem = below;
				}
				break;
			}
		case VK_LEFT:
			{
				// Decrement the order number.
				m_CurSubItem--;
				if (m_CurSubItem < 1) 
					m_CurSubItem = 1;
				else
				{
					CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
					// Make the column visible.
					// We have to take into account that the header may be reordered.
					MakeColumnVisible(Header_OrderToIndex(pHeader->m_hWnd, m_CurSubItem));
					// Invalidate the item.
					m_CurItem = GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
					if (m_CurItem != -1)
					{
						CRect rcBounds;
						GetItemRect(m_CurItem, rcBounds, LVIR_BOUNDS);
						InvalidateRect(&rcBounds);
						UpdateWindow();
					}
				}
				break;
			}
		case VK_TAB:
		case VK_RIGHT:
			{
				// Increment the order number.
				m_CurSubItem++;
				CHeaderCtrl* pHeader = (CHeaderCtrl*) GetDlgItem(0);
				int nColumnCount = pHeader->GetItemCount();
				// Don't go beyond the last column.
				if (m_CurSubItem > nColumnCount -1) 
					m_CurSubItem = nColumnCount - 1;
				else
				{
					MakeColumnVisible(Header_OrderToIndex(pHeader->m_hWnd, m_CurSubItem));
					m_CurItem = GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
					// Invalidate the item.
					if (m_CurItem != -1)
					{
						CRect rcBounds;
						GetItemRect(m_CurItem, rcBounds, LVIR_BOUNDS);
						InvalidateRect(&rcBounds);
						UpdateWindow();
					}
				}
				break;
			}
		default:
			break;
		}
	}


	void SVListCtrl::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
		// TODO: Add your control notification handler code here

		*pResult = 0;
	}


	void SVListCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
		// TODO: Add your control notification handler code here

		*pResult = 0;
	}


	void SVListCtrl::RepaintSelectedItems()
	{
		CRect rcBounds, rcLabel;

		// Invalidate focused item so it can repaint 
		int nItem = GetNextItem(-1, LVNI_FOCUSED);

		if(nItem != -1)
		{
			GetItemRect(nItem, rcBounds, LVIR_BOUNDS);
			GetItemRect(nItem, rcLabel, LVIR_LABEL);
			rcBounds.left = rcLabel.left;

			InvalidateRect(rcBounds, FALSE);
		}

		// Invalidate selected items depending on LVS_SHOWSELALWAYS
		if(!(GetStyle() & LVS_SHOWSELALWAYS))
		{
			for(nItem = GetNextItem(-1, LVNI_SELECTED);
				nItem != -1; nItem = GetNextItem(nItem, LVNI_SELECTED))
			{
				GetItemRect(nItem, rcBounds, LVIR_BOUNDS);
				GetItemRect(nItem, rcLabel, LVIR_LABEL);
				rcBounds.left = rcLabel.left;

				InvalidateRect(rcBounds, FALSE);
			}
		}

		UpdateWindow();
	}


	void SVListCtrl::OnSetFocus(CWnd* pOldWnd) 
	{
		CListCtrl::OnSetFocus(pOldWnd);

		// check if we are getting focus from label edit box
		if(nullptr != pOldWnd && ::IsWindow(pOldWnd->m_hWnd) && pOldWnd->GetParent() == this)
			return;

		// repaint items that should change appearance
		if((GetStyle() & LVS_TYPEMASK) == LVS_REPORT)
			RepaintSelectedItems();
	}


	void SVListCtrl::OnKillFocus(CWnd* pNewWnd) 
	{
		CListCtrl::OnKillFocus(pNewWnd);

		// check if we are losing focus to label edit box
		if(nullptr != pNewWnd && ::IsWindow(pNewWnd->m_hWnd) && pNewWnd->GetParent() == this)
			return;

		// repaint items that should change appearance
		if((GetStyle() & LVS_TYPEMASK) == LVS_REPORT)
			RepaintSelectedItems();
	}


	LRESULT SVListCtrl::OnSetFont(WPARAM wParam, LPARAM)
	{
		LRESULT res =  Default();

		CRect rc;
		GetWindowRect( &rc );

		WINDOWPOS wp;
		wp.hwnd = m_hWnd;
		wp.cx = rc.Width();
		wp.cy = rc.Height();
		wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
		SendMessage( WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp );

		return res;
	}
} //namespace SvMc
