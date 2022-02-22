// ******************************************************************************
// * COPYRIGHT (c) 2000 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVRPropertyItem
// * .File Name       : $Workfile:   PropTreeItem.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   07 May 2013 07:48:26  $
// ******************************************************************************

// PropertyItem.cpp
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
// This code was modified by Koerber AG
#include "stdafx.h"
#include "SVRPropTree.h"

#include "SVRPropTreeItem.h"
#include "Definitions/Color.h"

/////////////////////////////////////////////////////////////////////////////
// drawing helper functions
//

// draw a dotted horizontal line
static void _DotHLine(HDC hdc, LONG x, LONG y, LONG w)
{
	for (; w>0; w-=2, x+=2)
		SetPixel(hdc, x, y, GetSysColor(COLOR_BTNSHADOW));
}


// draw the plus/minus button
static void _DrawExpand(HDC hdc, LONG x, LONG y, bool bExpand, bool bFill)
{
	HPEN hPen;
	HPEN oPen;
	HBRUSH oBrush;

	hPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
	oPen = (HPEN)SelectObject(hdc, hPen);
	oBrush = (HBRUSH)SelectObject(hdc, GetStockObject(bFill ? WHITE_BRUSH : NULL_BRUSH));

	Rectangle(hdc, x, y, x + SVRPropTree::PROPTREEITEM_EXPANDBOX, y + SVRPropTree::PROPTREEITEM_EXPANDBOX);
	SelectObject(hdc, GetStockObject(BLACK_PEN));

	if (!bExpand)
	{
		MoveToEx(hdc, x + SVRPropTree::PROPTREEITEM_EXPANDBOXHALF, y + 2, nullptr);
		LineTo(hdc, x + SVRPropTree::PROPTREEITEM_EXPANDBOXHALF, y + SVRPropTree::PROPTREEITEM_EXPANDBOX - 2);
	}

	MoveToEx(hdc, x + 2, y + SVRPropTree::PROPTREEITEM_EXPANDBOXHALF, nullptr);
	LineTo(hdc, x + SVRPropTree::PROPTREEITEM_EXPANDBOX - 2, y + SVRPropTree::PROPTREEITEM_EXPANDBOXHALF);

	SelectObject(hdc, oPen);
	SelectObject(hdc, oBrush);
	DeleteObject(hPen);
}

/////////////////////////////////////////////////////////////////////////////
// @doc EXTERNAL
// @mfunc Constructor

SVRPropertyItem::SVRPropertyItem() :
	m_pProp(nullptr),
	m_loc(0, 0),
	m_rc(0, 0, 0, 0),
	m_nCtrlID(0),
	m_dwState(0),
	m_bActivated(FALSE),
	m_bCommitOnce(FALSE),
	m_rcExpand(0, 0, 0, 0),
	m_rcCheckbox(0, 0, 0, 0),
	m_pParent(nullptr),
	m_pSibling(nullptr),
	m_pChild(nullptr),
	m_pVis(nullptr),
	m_bCanShrink(true),
	m_rgbForeColor(::GetSysColor(COLOR_BTNTEXT)),
	m_rgbBackColor(::GetSysColor(COLOR_WINDOW)),
	m_rgbBackColorReadOnly(::GetSysColor(COLOR_WINDOW)), //(SvDef::WhiteSmoke) 
	m_bCanHighlight(true),
	m_bBold(false),
	m_lHeight(SVRPropTree::PROPTREEITEM_DEFHEIGHT)

{
}


/////////////////////////////////////////////////////////////////////////////
// @doc EXTERNAL
// @mfunc Destructor

SVRPropertyItem::~SVRPropertyItem()
{
}


bool SVRPropertyItem::IsExpanded()
{
	return (m_dwState & ItemExpanded) ? true : false;
}


bool SVRPropertyItem::IsSelected()
{
	return (m_dwState & ItemSelected) ? true : false;
}


bool SVRPropertyItem::IsChecked()
{
	return (m_dwState & ItemChecked) ? true : false;
}


bool SVRPropertyItem::IsReadOnly()
{
	return (m_dwState & ItemReadOnly) ? true : false;
}

bool SVRPropertyItem::IsButtonActiveIfReadOnly()
{
	return (m_dwState & ItemButtonEnableIfReadOnly) ? true : false;
}

bool SVRPropertyItem::IsActivated()
{
	return (m_dwState & ItemActivated) ? true : false;
}


bool SVRPropertyItem::IsHidden()
{
	return (m_dwState & ItemHidden) ? true : false;
}

bool SVRPropertyItem::CanShrink()
{
	return m_bCanShrink;
}

/////////////////////////////////////////////////////////////////////////////
// @doc EXTERNAL
// @mfunc Select item.
// @parm bool | bSelect | Default value is true.

void SVRPropertyItem::Select(bool bSelect)
{
	if (bSelect)
	{
		m_dwState |= ItemSelected;
	}
	else
	{
		m_dwState &= ~ItemSelected;
	}
}


void SVRPropertyItem::Expand(bool bExpand)
{
	if (bExpand)
	{
		m_dwState |= ItemExpanded;
	}
	else
	{
		if ( m_bCanShrink )
		{
			m_dwState &= ~ItemExpanded;
		}
	}

	if( nullptr != m_pProp )
	{
		m_pProp->RefreshItems(this);
	}
}


void SVRPropertyItem::Check(bool bCheck)
{
	if (bCheck)
	{
		m_dwState |= ItemChecked;
	}
	else
	{
		m_dwState &= ~ItemChecked;
	}
}

void SVRPropertyItem::SetCanShrink(bool bShrink)
{
	m_bCanShrink = bShrink;
}

/////////////////////////////////////////////////////////////////////////////
// @doc EXTERNAL
// @mfunc Set Read Only attribute.
// @parm bool | bReadOnly | Default value is true.

void SVRPropertyItem::ReadOnly(bool bReadOnly)
{
	if (bReadOnly)
	{
		m_dwState |= ItemReadOnly;
	}
	else
	{
		m_dwState &= ~ItemReadOnly;
	}
}

void SVRPropertyItem::ReadOnly(bool bReadOnly, bool bButtonEnabled)
{
	ReadOnly(bReadOnly);
	if (bButtonEnabled)
	{
		m_dwState |= ItemButtonEnableIfReadOnly;
	}
	else
	{
		m_dwState &= ~ItemButtonEnableIfReadOnly;
	}
}

void SVRPropertyItem::HideItem(bool bHide)
{
	if (bHide)
	{
		m_dwState |= ItemHidden;
	}
	else
	{
		m_dwState &= ~ItemHidden;
	}
}


bool SVRPropertyItem::IsCheckBox()
{
	return (m_dwState & ItemCheckbox) ? true : false;
}


void SVRPropertyItem::HasCheckBox(bool bCheckbox)
{
	if (bCheckbox)
	{
		m_dwState |= ItemCheckbox;
	}
	else
	{
		m_dwState &= ~ItemCheckbox;
	}
}


bool SVRPropertyItem::HitExpand(const POINT& pt)
{
	return m_rcExpand.PtInRect(pt) ? true : false;
}


bool SVRPropertyItem::HitCheckBox(const POINT& pt)
{
	return m_rcCheckbox.PtInRect(pt) ? true : false;
}


bool SVRPropertyItem::IsRootLevel()
{
	assert(nullptr != m_pProp);
	return GetParent() == m_pProp->GetRootItem();
}


LONG SVRPropertyItem::GetTotalHeight()
{
	SVRPropertyItem* pItem;
	LONG nHeight;

	nHeight = GetHeight();

	if (IsExpanded())
	{
		for (pItem = GetChild(); pItem; pItem = pItem->GetSibling())
			nHeight += pItem->GetTotalHeight();
	}

	return nHeight;
}


/////////////////////////////////////////////////////////////////////////////
// @doc EXTERNAL
// @mfunc Give the <c SVRPropertyItem> a name.
// @parm LPCTSTR | label | Points to a null-terminated character
// string that contains the <c SVRPropertyItem> name.

void SVRPropertyItem::SetLabelText(LPCTSTR sLabel)
{
	m_Label = sLabel;
}


/////////////////////////////////////////////////////////////////////////////
// @doc EXTERNAL
// @mfunc Return the name of the <c SVRPropertyItem>.

LPCTSTR SVRPropertyItem::GetLabelText()
{
	return m_Label.c_str();
}


void SVRPropertyItem::SetInfoText(LPCTSTR sInfo)
{
	m_Info = sInfo;
}


LPCTSTR SVRPropertyItem::GetInfoText()
{
	return m_Info.c_str();
}


/////////////////////////////////////////////////////////////////////////////
// @doc EXTERNAL
// @mfunc Set the Control Id for the <c SVRPropertyItem>.
// @parm UINT | nCtrlID |

void SVRPropertyItem::SetCtrlID(UINT nCtrlID)
{
	m_nCtrlID = nCtrlID;
}


/////////////////////////////////////////////////////////////////////////////
// @doc EXTERNAL
// @mfunc Return the Control Id for the <c SVRPropertyItem>.

UINT SVRPropertyItem::GetCtrlID()
{
	return m_nCtrlID;
}


LONG SVRPropertyItem::GetHeight()
{
	return m_lHeight;
}

void SVRPropertyItem::SetHeight(long lHeight)
{
	m_lHeight = lHeight;
}

/////////////////////////////////////////////////////////////////////////////
// @doc EXTERNAL
// @mfunc OnMove

void SVRPropertyItem::OnMove()
{
	// no attributes, do nothing
}


/////////////////////////////////////////////////////////////////////////////
// @doc EXTERNAL
// @mfunc OnRefresh

void SVRPropertyItem::OnRefresh()
{
	// no attributes, do nothing
}


/////////////////////////////////////////////////////////////////////////////
// @doc EXTERNAL
// @mfunc OnCommit

void SVRPropertyItem::OnCommit()
{
	// no attributes, do nothing
}

void SVRPropertyItem::OnUndoCommit()
{
	// no attributes, do nothing
}


void SVRPropertyItem::Activate()
{
	m_bActivated  = true;
	m_bCommitOnce = false;

	OnActivate();
}


/////////////////////////////////////////////////////////////////////////////
// @doc EXTERNAL
// @mfunc CommitChanges

void SVRPropertyItem::CommitChanges()
{
	m_bActivated = false;

	if (m_bCommitOnce)
		return;

	m_bCommitOnce = true;

	assert(nullptr != m_pProp);

	OnCommit();

	LRESULT lr = m_pProp->SendNotify(PTN_ITEMCHANGED, this);

	if ( S_OK != lr )
	{
		OnUndoCommit();
	}
	m_pProp->RefreshItems(this);
}

void SVRPropertyItem::OnActivate()
{
	// no attributes, do nothing
}

void SVRPropertyItem::OnLossFocus()
{
	// no attributes, do nothing
}

void SVRPropertyItem::SetPropOwner(SVRPropTree* pProp)
{
	m_pProp = pProp;
}


const POINT& SVRPropertyItem::GetLocation()
{
	return m_loc;
}


SVRPropertyItem* SVRPropertyItem::GetParent()
{
	return m_pParent;
}


SVRPropertyItem* SVRPropertyItem::GetSibling()
{
	return m_pSibling;
}


SVRPropertyItem* SVRPropertyItem::GetChild()
{
	return m_pChild;
}


SVRPropertyItem* SVRPropertyItem::GetNextVisible()
{
	return m_pVis;
}


void SVRPropertyItem::SetParent(SVRPropertyItem* pParent)
{
	m_pParent = pParent;
}


void SVRPropertyItem::SetSibling(SVRPropertyItem* pSibling)
{
	m_pSibling = pSibling;
}


void SVRPropertyItem::SetChild(SVRPropertyItem* pChild)
{
	m_pChild = pChild;
}


void SVRPropertyItem::SetNextVisible(SVRPropertyItem* pVis)
{
	m_pVis = pVis;
}


LONG SVRPropertyItem::UpdatePosition( const RECT& rc, LONG x, LONG y )
{
	CPoint pt;
	LONG nTotal, nCol, ey;
	CRect drc, ir;

	assert(nullptr != m_pProp);


	if(!IsRootLevel() && IsHidden())
		return 0;

	// store the item's location
	m_loc = CPoint(x, y);

	// store the items rectangle position
	m_rc.SetRect(m_pProp->GetOrigin().x + m_pProp->PROPTREEITEM_SPACE, m_loc.y, rc.right, m_loc.y + GetHeight()-1);
	m_rc.OffsetRect(0, -m_pProp->GetOrigin().y);

	CRect rect(m_rc);
	rect.left -= 4;

	// init temp drawing variables
	nTotal = GetHeight();
	ey = (nTotal >> 1) - (m_pProp->PROPTREEITEM_EXPANDBOX >> 1) - 2;

	bool bCheck = false;

	// convert item coordinates to screen coordinates
	pt = m_loc;
	pt.y -= m_pProp->GetOrigin().y;
	nCol = m_pProp->GetOrigin().x;

	if (IsRootLevel())
	{
		drc.SetRect(pt.x + m_pProp->PROPTREEITEM_EXPANDCOLUMN, pt.y, rc.right, pt.y + nTotal);
	}
	else
	{
		drc.SetRect(pt.x + m_pProp->PROPTREEITEM_EXPANDCOLUMN, pt.y, nCol, pt.y + nTotal);
	}

	// calc/draw expand box position
	if (GetChild())
	{
		m_rcExpand.left = m_pProp->PROPTREEITEM_EXPANDCOLUMN/2 - m_pProp->PROPTREEITEM_EXPANDBOXHALF;
		m_rcExpand.top = m_loc.y + ey;
		m_rcExpand.right = m_rcExpand.left + m_pProp->PROPTREEITEM_EXPANDBOX - 1;
		m_rcExpand.bottom = m_rcExpand.top + m_pProp->PROPTREEITEM_EXPANDBOX - 1;

		ir = m_rcExpand;
		ir.OffsetRect(0, -m_pProp->GetOrigin().y);
	}
	else
	{
		m_rcExpand.SetRectEmpty();
	}

	// calc/draw check box position
	if (IsCheckBox())
	{
		bCheck = true;

		ir.left = drc.left + m_pProp->PROPTREEITEM_SPACE;
		ir.top = m_loc.y + ey;

		ir.right = ir.left + m_pProp->PROPTREEITEM_CHECKBOX;
		ir.bottom = ir.top + m_pProp->PROPTREEITEM_CHECKBOX;

		m_rcCheckbox = ir;
	}
	else
	{
		m_rcCheckbox.SetRectEmpty();
	}

	// calc label position
	ir = drc;
	ir.left += m_pProp->PROPTREEITEM_SPACE;

	// offset the label text if item has a check box
	if (bCheck)
		OffsetRect(&ir, m_pProp->PROPTREEITEM_CHECKBOX + m_pProp->PROPTREEITEM_SPACE * 2, 0);

	// draw label
	if (!m_Label.empty())
	{
		// draw the text highlighted if selected
		if (IsSelected() && CanHighlight())
		{
			CRect dr;
			dr = drc;
			dr.left = m_pProp->PROPTREEITEM_EXPANDCOLUMN;
		}
	}

	// draw check box frame
	if (IsCheckBox())
	{
		ir = m_rcCheckbox;
		ir.OffsetRect(0, -m_pProp->GetOrigin().y);
	}

	// draw children
	if (GetChild() && IsExpanded())
	{
		y += nTotal;

		SVRPropertyItem* pNext;

		for (pNext = GetChild(); pNext; pNext = pNext->GetSibling())
		{
			LONG nHeight = pNext->UpdatePosition( rc, x + (IsRootLevel() ? 0 : m_pProp->PNINDENT), y);
			nTotal += nHeight;
			y += nHeight;
		}
	}

	return nTotal;
}

/////////////////////////////////////////////////////////////////////////////
// @doc EXTERNAL
// @mfunc DrawItem
// @parm CDC* | pDC |
// @parm RECT& | rc |
// @parm long | x |
// @parm long | y |

LONG SVRPropertyItem::DrawItem(CDC* pDC, const RECT& rc, LONG x, LONG y)
{
	CPoint pt;
	LONG nTotal, nCol, ey;
	CRect drc, ir;

	CFont*	l_pOldFont;

	assert(nullptr != m_pProp);


	if(!IsRootLevel() && IsHidden())
	{
		return 0;
	}


	COLORREF rgbOldFore = pDC->SetTextColor( m_rgbForeColor );
	COLORREF rgbOldBack {0};
	if (IsReadOnly())
	{
		rgbOldBack = pDC->SetBkColor(m_rgbBackColorReadOnly);
	}
	else
	{
		rgbOldBack = pDC->SetBkColor(m_rgbBackColor);
	}


	// Add Item to the list of visble items
	m_pProp->AddToVisibleList(this);

	// store the item's location
	m_loc = CPoint(x, y);

	// store the items rectangle position
	m_rc.SetRect(m_pProp->GetOrigin().x + m_pProp->PROPTREEITEM_SPACE, m_loc.y, rc.right, m_loc.y + GetHeight()-1);
	m_rc.OffsetRect(0, -m_pProp->GetOrigin().y);

	CRect rect(m_rc);
	rect.left -= 4;
	if ( IsReadOnly())
	{
		pDC->FillSolidRect(&rect, m_rgbBackColorReadOnly);
	}
	else
	{
		pDC->FillSolidRect(&rect, m_rgbBackColor);
	}

	// init temp drawing variables
	nTotal = GetHeight();
	ey = (nTotal >> 1) - (m_pProp->PROPTREEITEM_EXPANDBOX >> 1) - 2;

	bool bCheck = false;

	// convert item coordinates to screen coordinates
	pt = m_loc;
	pt.y -= m_pProp->GetOrigin().y;
	nCol = m_pProp->GetOrigin().x;

	if (IsRootLevel())
	{
		drc.SetRect(pt.x + m_pProp->PROPTREEITEM_EXPANDCOLUMN, pt.y, rc.right, pt.y + nTotal);
	}
	else
	{
		drc.SetRect(pt.x + m_pProp->PROPTREEITEM_EXPANDCOLUMN, pt.y, nCol, pt.y + nTotal);
	}

	pDC->FillSolidRect(&drc, m_rgbBackColor);


	// root level items are shaded
	if (IsRootLevel())
	{
		HGDIOBJ hOld = pDC->SelectObject(GetSysColorBrush(COLOR_BTNFACE));
		pDC->PatBlt(rc.left, drc.top, rc.right - rc.left + 1, drc.Height(), PATCOPY);
		pDC->SelectObject(hOld);
	}

	// calc/draw expand box position
	if (GetChild())
	{
		m_rcExpand.left = m_pProp->PROPTREEITEM_EXPANDCOLUMN/2 - m_pProp->PROPTREEITEM_EXPANDBOXHALF;
		m_rcExpand.top = m_loc.y + ey;
		m_rcExpand.right = m_rcExpand.left + m_pProp->PROPTREEITEM_EXPANDBOX - 1;
		m_rcExpand.bottom = m_rcExpand.top + m_pProp->PROPTREEITEM_EXPANDBOX - 1;

		ir = m_rcExpand;
		ir.OffsetRect(0, -m_pProp->GetOrigin().y);
		if ( CanShrink() )
			_DrawExpand(pDC->m_hDC, ir.left, ir.top, IsExpanded(), !IsRootLevel());
	}
	else
	{
		m_rcExpand.SetRectEmpty();
	}

	// calc/draw check box position
	if (IsCheckBox())
	{
		bCheck = true;

		ir.left = drc.left + m_pProp->PROPTREEITEM_SPACE;
		ir.top = m_loc.y + ey;

		ir.right = ir.left + m_pProp->PROPTREEITEM_CHECKBOX;
		ir.bottom = ir.top + m_pProp->PROPTREEITEM_CHECKBOX;

		m_rcCheckbox = ir;
	}
	else
	{
		m_rcCheckbox.SetRectEmpty();
	}

	HRGN hRgn = nullptr;

	// create a clipping region for the label
	if (!IsRootLevel())
	{
		hRgn = CreateRectRgn(drc.left, drc.top, drc.right, drc.bottom);
		SelectClipRgn(pDC->m_hDC, hRgn);
	}

	// calc label position
	ir = drc;
	ir.left += m_pProp->PROPTREEITEM_SPACE;

	// offset the label text if item has a check box
	if (bCheck)
	{
		OffsetRect(&ir, m_pProp->PROPTREEITEM_CHECKBOX + m_pProp->PROPTREEITEM_SPACE * 2, 0);
	}

	// draw label
	if (!m_Label.empty())
	{
		if (IsRootLevel())
		{
			l_pOldFont = pDC->SelectObject(SVRPropTree::GetBoldFont());
		}
		else
		{
			l_pOldFont = pDC->SelectObject(IsBold() ? SVRPropTree::GetBoldFont() : SVRPropTree::GetNormalFont());
		}

		pDC->SetTextColor( m_rgbForeColor );
		//pDC->SetTextColor(GetSysColor(COLOR_BTNTEXT));
		pDC->SetBkMode(TRANSPARENT);
		pDC->DrawText(m_Label.c_str(), &ir, DT_SINGLELINE|DT_VCENTER|DT_CALCRECT);

		// draw the text highlighted if selected
		if (IsSelected() && CanHighlight())
		{
			HGDIOBJ oPen = pDC->SelectObject(GetStockObject(NULL_PEN));
			HGDIOBJ oBrush = pDC->SelectObject(GetSysColorBrush(COLOR_HIGHLIGHT));
			
			CRect dr;
			dr = drc;
			dr.left = m_pProp->PROPTREEITEM_EXPANDCOLUMN;
			
			pDC->Rectangle(&dr);
			
			pDC->SelectObject(oPen);
			pDC->SelectObject(oBrush);

			pDC->SetTextColor(GetSysColor(COLOR_BTNHIGHLIGHT));
		}

		// check if we need to draw the text as disabled
		if (!m_pProp->IsWindowEnabled())
		{
			pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
		}

		pDC->DrawText(m_Label.c_str(), &ir, DT_SINGLELINE|DT_VCENTER);

//JAB110508
		pDC->SelectObject(l_pOldFont);
	}

	// draw check box frame
	if (IsCheckBox())
	{
		ir = m_rcCheckbox;
		ir.OffsetRect(0, -m_pProp->GetOrigin().y);
		pDC->DrawFrameControl(&ir, DFC_BUTTON, DFCS_BUTTONCHECK | (IsChecked() ? DFCS_CHECKED : 0));
	}

	// remove clip region
	if (hRgn)
	{
		SelectClipRgn(pDC->m_hDC, nullptr);
		DeleteObject(hRgn);
	}

	// draw horzontal sep
	_DotHLine(pDC->m_hDC, m_pProp->PROPTREEITEM_EXPANDCOLUMN, pt.y + nTotal - 1, rc.right - m_pProp->PROPTREEITEM_EXPANDCOLUMN + 1);

	// draw separators
	if (!IsRootLevel())
	{
		// column sep
		CPen pn1(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
		CPen* pOld;

		pOld = pDC->SelectObject(&pn1);
		pDC->MoveTo(nCol, drc.top);
		pDC->LineTo(nCol, drc.bottom);

		CPen pn2(PS_SOLID, 1, GetSysColor(COLOR_BTNHIGHLIGHT));
		pDC->SelectObject(&pn2);
		pDC->MoveTo(nCol + 1, drc.top);
		pDC->LineTo(nCol + 1, drc.bottom);

		pDC->SelectObject(pOld);
	}

	// draw attribute
	if (!IsRootLevel())
	{
		// create clip region
		hRgn = CreateRectRgn(m_rc.left, m_rc.top, m_rc.right, m_rc.bottom);
		SelectClipRgn(pDC->m_hDC, hRgn);
		
		DrawAttribute(pDC, m_rc);

		SelectClipRgn(pDC->m_hDC, nullptr);
		DeleteObject(hRgn);
	}

	// draw children
	if (GetChild() && IsExpanded())
	{
		y += nTotal;

		SVRPropertyItem* pNext;

		for (pNext = GetChild(); pNext; pNext = pNext->GetSibling())
		{
			LONG nHeight = pNext->DrawItem(pDC, rc, x + (IsRootLevel() ? 0 : m_pProp->PNINDENT), y);
			nTotal += nHeight;
			y += nHeight;
		}
	}

	pDC->SetTextColor( rgbOldFore );
	pDC->SetBkColor( rgbOldBack );

	return nTotal;
}


/////////////////////////////////////////////////////////////////////////////
// @doc EXTERNAL
// @mfunc DrawAttribute
// @parm CDC* | pDC |
// @parm RECT& | rc |

void SVRPropertyItem::DrawAttribute(CDC*, const RECT&)
{
	// no attributes are assocatied with this type
}


int SVRPropertyItem::GetItemType(void)
{
	assert(0);
	return 0;
}


bool SVRPropertyItem::GetItemValue(bool&)
{
	// no items are assocatied with this type
	return false;
}


bool SVRPropertyItem::GetItemValue(BYTE& /*bVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::GetItemValue(short& /*iVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::GetItemValue(USHORT& /*uiVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::GetItemValue(long& /*lVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::GetItemValue(ULONG& /*ulVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::GetItemValue(int& /*intVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::GetItemValue(UINT& /*uintVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::GetItemValue(float& /*fltVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::GetItemValue(double& /*dblVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::GetItemValue(std::string& /*strVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::GetItemValue(_variant_t& /*vtVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::SetItemValue(const bool /*boolVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::SetItemValue(const BYTE /*bVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::SetItemValue(const short /*iVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::SetItemValue(const USHORT /*uiVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::SetItemValue(const long /*lVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::SetItemValue(const ULONG /*ulVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::SetItemValue(const int /*intVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::SetItemValue(const UINT /*uintVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::SetItemValue(const float /*fltVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::SetItemValue(const double /*dblVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::SetItemValue(LPCTSTR /*lpszVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::SetItemValuePtr(BYTE& /*bVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::SetItemValuePtr(short& /*iVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::SetItemValuePtr(USHORT& /*uiVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::SetItemValuePtr(long& /*lVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::SetItemValuePtr(ULONG& /*ulVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::SetItemValuePtr(int& /*intVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::SetItemValuePtr(UINT& /*uintVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::SetItemValuePtr(float& /*fltVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::SetItemValuePtr(double& /*dblVal*/)
{
	assert(0);
	return false;
}


bool SVRPropertyItem::SetItemValuePtr(std::string& /*strSrc*/)
{
	assert(0);
	return false;
}

COLORREF SVRPropertyItem::GetForeColor()
{
	return m_rgbForeColor;
}

COLORREF SVRPropertyItem::GetBackColor()
{
	return m_rgbBackColor;
}

COLORREF SVRPropertyItem::SetForeColor(COLORREF rgb)
{
	COLORREF rgbPrev = m_rgbForeColor;
	m_rgbForeColor = rgb;
	m_pProp->RefreshItems(this);
	return rgbPrev;

}

COLORREF SVRPropertyItem::SetBackColor(COLORREF rgb)
{
	COLORREF rgbPrev = m_rgbBackColor;
	m_rgbBackColor = rgb;
	m_pProp->RefreshItems(this);
	return rgbPrev;
}

COLORREF SVRPropertyItem::SetBackColorReadOnly(bool refresh,COLORREF rgb)
{
	COLORREF rgbPrev = m_rgbBackColorReadOnly;
	m_rgbBackColorReadOnly = rgb;
	if(refresh)
	{
		m_pProp->RefreshItems(this);
	}
	return rgbPrev;
}

bool SVRPropertyItem::CanHighlight()
{
	return m_bCanHighlight;
}

void SVRPropertyItem::SetCanHighlight(bool bCanHighlight)
{
	m_bCanHighlight = bCanHighlight;
}

bool SVRPropertyItem::IsBold()
{
	return m_bBold;
}

void SVRPropertyItem::SetBold(bool bBold)
{
	m_bBold = bBold;
	m_pProp->RefreshItems(this);
}

SVRPropTree* SVRPropertyItem::GetPropTree()
{
	return m_pProp;
}

