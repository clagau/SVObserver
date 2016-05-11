// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : PropTreeCtrl.cpp
// * .File Name       : $Workfile:   PropTreeCtrl.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 16:42:00  $
// ******************************************************************************

#include "stdafx.h"
#include "PropTree.h"
//#include "Resource.h"
#include "PropTreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace	// file scope
{
	void YieldPaintMessages(HWND hWnd = nullptr )
	{// Let other PAINT messages through
		
		MSG msg;
		
		while (PeekMessage(&msg, hWnd, WM_PAINT, WM_PAINT, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

extern HINSTANCE ghInst;

/////////////////////////////////////////////////////////////////////////////
// SVRPropTreeCtrl

SVRPropTreeCtrl::SVRPropTreeCtrl() :
	m_pProp(nullptr),
	m_BackBufferSize(0,0),
	m_bColDrag(FALSE),
	m_nPrevCol(0)
{
}

SVRPropTreeCtrl::~SVRPropTreeCtrl()
{
}

BEGIN_MESSAGE_MAP(SVRPropTreeCtrl, CWnd)
	//{{AFX_MSG_MAP(SVRPropTreeCtrl)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// SVRPropTreeCtrl message handlers

void SVRPropTreeCtrl::SetPropOwner(SVRPropTree* pProp)
{
	m_pProp = pProp;
}


BOOL SVRPropTreeCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CWnd* pWnd = this;

	LPCTSTR pszCreateClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, ::LoadCursor(nullptr, IDC_ARROW));

	return pWnd->Create(pszCreateClass, _T(""), dwStyle, rect, pParentWnd, nID);
}


void SVRPropTreeCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	RecreateBackBuffer(cx, cy);

	if (m_pProp)
	{
		UpdateResize();
		Invalidate();
		UpdateWindow();

		// inform all items that a resize has been made
		m_pProp->UpdateMoveAllItems();
	}
}

void SVRPropTreeCtrl::RecreateBackBuffer(int cx, int cy)
{
	if (m_BackBufferSize.cx<cx || m_BackBufferSize.cy<cy)
	{
		m_BackBufferSize = CSize(cx, cy);

		CWindowDC dc(nullptr);

		int nPlanes = dc.GetDeviceCaps(PLANES);
		int nBitCount = dc.GetDeviceCaps(BITSPIXEL);

		m_BackBuffer.DeleteObject();
		m_BackBuffer.CreateBitmap(cx, cy, nPlanes, nBitCount, nullptr);
	}
}

void SVRPropTreeCtrl::UpdateResize()
{
	SCROLLINFO si;
	LONG nHeight;
	CRect rc;

	ASSERT(nullptr != m_pProp);

	GetClientRect(rc);
	nHeight = rc.Height() + 1;

	ZeroMemory(&si, sizeof(SCROLLINFO));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_RANGE|SIF_PAGE;
	si.nMin = 0;
	si.nMax = m_pProp->GetRootItem()->GetTotalHeight();
	si.nPage = nHeight;

	if ((int)si.nPage>si.nMax)
		m_pProp->SetOriginOffset(0);

	SetScrollInfo(SB_VERT, &si, TRUE);

	// force set column for clipping
	m_pProp->SetColumn(m_pProp->GetColumn());
}


void SVRPropTreeCtrl::UpdatePositions() 
{
	CRect rc;
	GetClientRect(rc);

	SVRPropertyItem* pItem;
	LONG nTotal = 0;

	rc.DeflateRect(2,2);

	// draw all items
	for (pItem = m_pProp->GetRootItem()->GetChild(); pItem; pItem = pItem->GetSibling())
	{
		LONG nHeight = pItem->UpdatePosition( rc, 0, nTotal);
		nTotal += nHeight;
	}
}

void SVRPropTreeCtrl::OnPaint() 
{
	CPaintDC dc(this);
	CDC memdc;
	CBitmap* pOldBitmap;
	HBRUSH		l_hOldBrush;

	ASSERT(nullptr != m_pProp);

	m_pProp->ClearVisibleList();

	memdc.CreateCompatibleDC(&dc);
	pOldBitmap = memdc.SelectObject(&m_BackBuffer);

	CRect rc;
	GetClientRect(rc);

	// draw control background
	l_hOldBrush = (HBRUSH) memdc.SelectObject(GetSysColorBrush(COLOR_BTNFACE));
	memdc.PatBlt(rc.left, rc.top, rc.Width(), rc.Height(), PATCOPY);

	// draw control inside fill color
	rc.DeflateRect(2,2);
	memdc.PatBlt(rc.left, rc.top, rc.Width(), rc.Height(), m_pProp->IsWindowEnabled() ? WHITENESS : PATCOPY);
	rc.InflateRect(2,2);

	// draw expand column
	memdc.SelectObject(GetSysColorBrush(COLOR_BTNFACE));
	memdc.PatBlt(0, 0, m_pProp->PROPTREEITEM_EXPANDCOLUMN, rc.Height(), PATCOPY);

	// draw edge
	memdc.DrawEdge(&rc, BDR_SUNKENOUTER, BF_RECT);

	SVRPropertyItem* pItem;
	LONG nTotal = 0;

	ASSERT(nullptr != m_pProp->GetRootItem());

	rc.DeflateRect(2,2);

	// create clip region
	HRGN hRgn = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
	SelectClipRgn(memdc.m_hDC, hRgn);

	// draw all items
	for (pItem = m_pProp->GetRootItem()->GetChild(); pItem; pItem = pItem->GetSibling())
	{
		LONG nHeight = pItem->DrawItem(&memdc, rc, 0, nTotal);
		nTotal += nHeight;
	}

	// remove clip region
	SelectClipRgn(memdc.m_hDC, nullptr);
	DeleteObject(hRgn);

	// copy back buffer to the display
	dc.GetClipBox(&rc);
	dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &memdc, rc.left, rc.top, SRCCOPY);

//JAB110508 
	memdc.SelectObject(pOldBitmap);
	memdc.SelectObject(l_hOldBrush);
	memdc.DeleteDC();
}


BOOL SVRPropTreeCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (nHitTest==HTCLIENT)
	{
		CPoint pt;

		ASSERT(nullptr != m_pProp);

		GetCursorPos(&pt);
		ScreenToClient(&pt);

		switch (m_pProp->HitTest(pt))
		{
			case HTCOLUMN:
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
				return TRUE;

			case HTCHECKBOX:
			case HTEXPAND:
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
				return TRUE;
		}
	}

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}


void SVRPropTreeCtrl::OnLButtonDown(UINT, CPoint point) 
{
	ASSERT(nullptr != m_pProp);

	if (m_pProp->IsDisableInput())
		return;

	m_pProp->SendNotify(NM_CLICK);

	if (!m_pProp->IsWindowEnabled())
		return;

	SetFocus();

	LONG nHit = m_pProp->HitTest(point);

	SVRPropertyItem* pItem;
	CRect rc;
	CDC* pDC;

	switch (nHit)
	{
		case HTCOLUMN:
			if (m_pProp->SendNotify(PTN_COLUMNCLICK))
				break;

			m_bColDrag = TRUE;
			SetCapture();

			m_nPrevCol = m_pProp->GetOrigin().x;

			// paint drag line
			pDC = GetDC();
			GetClientRect(rc);
			pDC->PatBlt(m_nPrevCol - m_pProp->PROPTREEITEM_COLRNG/2, 0, m_pProp->PROPTREEITEM_COLRNG, rc.bottom, PATINVERT);
			ReleaseDC(pDC);
			break;

		case HTCHECKBOX:
			if (nullptr != (pItem = m_pProp->FindItem(point)))
			{
				pItem->Check(!pItem->IsChecked());
				m_pProp->SendNotify(PTN_CHECKCLICK, pItem);
				Invalidate();
			}
			break;

		case HTEXPAND:
			if (nullptr != (pItem = m_pProp->FindItem(point)))
			{
				if ( pItem->CanShrink() || !pItem->IsExpanded() )
				{
					if (pItem->GetChild() && !m_pProp->SendNotify(PTN_ITEMEXPANDING, pItem))
					{
						pItem->Expand(!pItem->IsExpanded());

						UpdateResize();
						Invalidate();
						UpdateWindow();
						CheckVisibleFocus();
					}
				}
			}
			break;

		default:
			if (nullptr != (pItem = m_pProp->FindItem(point)))
			{
				if ( pItem->CanHighlight() )
				{
					SVRPropertyItem* pOldFocus = m_pProp->GetFocusedItem();

					m_pProp->SelectItems(nullptr, FALSE);
					m_pProp->SetFocusedItem(pItem);

					YieldPaintMessages();

					pItem->Select();

					Invalidate();

					if (pItem!=pOldFocus)
						m_pProp->SendNotify(PTN_SELCHANGE, pItem);

					if (nHit==HTATTRIBUTE && !pItem->IsRootLevel())
					{
						if (!m_pProp->SendNotify(PTN_PROPCLICK, pItem) && !pItem->IsReadOnly())
							pItem->Activate();
					}
				}
			}
			else
			{
				m_pProp->SelectItems(nullptr, FALSE);
				m_pProp->SetFocusedItem(nullptr);
				m_pProp->SendNotify(PTN_SELCHANGE);
				Invalidate();
			}
			break;
	}
}


void SVRPropTreeCtrl::OnLButtonUp(UINT, CPoint point) 
{
	if (m_bColDrag)
	{
		CDC* pDC = GetDC();
		CRect rc;

		GetClientRect(rc);
		pDC->PatBlt(m_nPrevCol - m_pProp->PROPTREEITEM_COLRNG/2, 0, m_pProp->PROPTREEITEM_COLRNG, rc.bottom, PATINVERT);
		ReleaseDC(pDC);

		m_bColDrag = FALSE;
		ReleaseCapture();

		m_pProp->SetColumn(point.x);
		// m_pProp->UpdateMoveAllItems(); T.B. SCR 401 Bug Fix...
		Invalidate();
	}
}


void SVRPropTreeCtrl::OnLButtonDblClk(UINT, CPoint point)
{
	ASSERT(nullptr != m_pProp);

	m_pProp->SendNotify(NM_DBLCLK);

	SVRPropertyItem* pItem;
	SVRPropertyItem* pOldFocus;

	if (nullptr != (pItem = m_pProp->FindItem(point)) && pItem->GetChild())
	{
		switch (m_pProp->HitTest(point))
		{
			case HTCOLUMN:
				break;

			case HTCHECKBOX:
				pItem->Check(!pItem->IsChecked());
				m_pProp->SendNotify(PTN_CHECKCLICK, pItem);
				Invalidate();
				break;

			case HTATTRIBUTE:
				if (!pItem->IsRootLevel())
					break;

				// pass thru to default

			default:
				pOldFocus = m_pProp->GetFocusedItem();
				m_pProp->SelectItems(nullptr, FALSE);
				m_pProp->SetFocusedItem(pItem);
				pItem->Select();

				if (pItem!=pOldFocus)
					m_pProp->SendNotify(PTN_SELCHANGE, pItem);

				// pass thru to HTEXPAND

			case HTEXPAND:
				if (!m_pProp->SendNotify(PTN_ITEMEXPANDING, pItem))
				{
					if ( pItem->CanShrink() || !pItem->IsExpanded() )
					{
						pItem->Expand(!pItem->IsExpanded());

						UpdateResize();
						Invalidate();
						UpdateWindow();
						CheckVisibleFocus();
					}
				}
				break;
		}
	}
}


void SVRPropTreeCtrl::OnMouseMove(UINT, CPoint point)
{
	if (m_bColDrag)
	{
		CDC* pDC = GetDC();
		CRect rc;

		GetClientRect(rc);
		pDC->PatBlt(m_nPrevCol - m_pProp->PROPTREEITEM_COLRNG/2, 0, m_pProp->PROPTREEITEM_COLRNG, rc.bottom, PATINVERT);
		pDC->PatBlt(point.x - m_pProp->PROPTREEITEM_COLRNG/2, 0, m_pProp->PROPTREEITEM_COLRNG, rc.bottom, PATINVERT);
		m_nPrevCol = point.x;
		ReleaseDC(pDC);
	}
}


BOOL SVRPropTreeCtrl::OnMouseWheel(UINT, short zDelta, CPoint) 
{
	SCROLLINFO si;

	ZeroMemory(&si, sizeof(SCROLLINFO));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_RANGE;

	GetScrollInfo(SB_VERT, &si);

	CRect rc;
	GetClientRect(rc);

	if (si.nMax - si.nMin < rc.Height())
		return TRUE;

	SetFocus();
	OnVScroll(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0, nullptr);

	return TRUE;
}

void SVRPropTreeCtrl::OnKeyDown(UINT nChar, UINT, UINT) 
{
	SVRPropertyItem* pItem;

	ASSERT(nullptr != m_pProp);

	if (m_pProp->IsDisableInput() || !m_pProp->IsWindowEnabled())
		return;

	switch (nChar)
	{
		case VK_RETURN:
			if (nullptr != (pItem = m_pProp->GetFocusedItem()) && !pItem->IsRootLevel() && !pItem->IsReadOnly())
			{
				pItem->Activate();
			}
			break;

		case VK_HOME:
			if (m_pProp->FocusFirst())
				Invalidate();
			break;

		case VK_END:
			if (m_pProp->FocusLast())
				Invalidate();
			break;

		case VK_LEFT:
			if (nullptr != (pItem = m_pProp->GetFocusedItem()))
			{
				if (!m_pProp->SendNotify(PTN_ITEMEXPANDING, pItem))
				{
					if ( pItem->CanShrink() )
					{
						if (pItem->GetChild() && pItem->IsExpanded())
						{
							pItem->Expand(FALSE);
							UpdateResize();
							Invalidate();
							UpdateWindow();
							CheckVisibleFocus();
							break;
						}
					}
				}
			}
			else
				break;
			// pass thru to next case VK_UP
		case VK_UP:
			if (m_pProp->FocusPrev())
				Invalidate();
			break;

		case VK_RIGHT:
			if (nullptr != (pItem = m_pProp->GetFocusedItem()))
			{
				if (!m_pProp->SendNotify(PTN_ITEMEXPANDING, pItem))
				{
					if (pItem->GetChild() && !pItem->IsExpanded())
					{
						pItem->Expand();
						UpdateResize();
						Invalidate();
						UpdateWindow();
						CheckVisibleFocus();
						break;
					}
				}
			}
			else
				break;
			// pass thru to next case VK_DOWN
		case VK_DOWN:
			if (m_pProp->FocusNext())
				Invalidate();
			break;
	}
}


UINT SVRPropTreeCtrl::OnGetDlgCode() 
{
	return DLGC_WANTARROWS|DLGC_WANTCHARS|DLGC_WANTALLKEYS;
}


void SVRPropTreeCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar*) 
{
	SCROLLINFO si;
	CRect rc;
	LONG nHeight;

	SetFocus();

	GetClientRect(rc);
	nHeight = rc.Height() + 1;

	ZeroMemory(&si, sizeof(SCROLLINFO));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_RANGE;

	GetScrollInfo(SB_VERT, &si);

	LONG ny = m_pProp->GetOrigin().y;

	switch (nSBCode)
	{
		case SB_LINEDOWN:
			ny += m_pProp->PROPTREEITEM_DEFHEIGHT;
			break;

		case SB_LINEUP:
			ny -= m_pProp->PROPTREEITEM_DEFHEIGHT;
			break;

		case SB_PAGEDOWN:
			ny += nHeight;
			break;

		case SB_PAGEUP:
			ny -= nHeight;
			break;

		case SB_THUMBTRACK:
			ny = nPos;
			break;
	}

	ny = __min(__max(ny, si.nMin), si.nMax - nHeight);

	m_pProp->SetOriginOffset(ny);
	si.fMask = SIF_POS;
	si.nPos = ny;

	SetScrollInfo(SB_VERT, &si, TRUE);
	Invalidate();
}


void SVRPropTreeCtrl::CheckVisibleFocus()
{
	ASSERT(nullptr != m_pProp);

	SVRPropertyItem* pItem;
	
	if (nullptr == (pItem = m_pProp->GetFocusedItem()))
		return;

	if (!m_pProp->IsItemVisible(pItem))
	{
		if (m_pProp->IsSingleSelection())
			pItem->Select(FALSE);

		m_pProp->SetFocusedItem(nullptr);
		m_pProp->SendNotify(PTN_SELCHANGE, nullptr);

		Invalidate();
	}
}

