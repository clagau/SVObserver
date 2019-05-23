// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVRPropTree
// * .File Name       : $Workfile:   PROPTREE.CPP  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   03 Jul 2013 16:26:16  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVRPropTree.h"
#include "SVRPropTreeState.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int SVRPropTree::PROPTREEITEM_COLRNG        = 5;             // width of splitter
const int SVRPropTree::PROPTREEITEM_DEFHEIGHT     = 21;			// default heigt of an item
const int SVRPropTree::PROPTREEITEM_SPACE         = 5; 			// default horz spacing
const int SVRPropTree::PROPTREEITEM_EXPANDBOX     = 9; 			// size of the expand box
const int SVRPropTree::PROPTREEITEM_CHECKBOX      = 14;			// size of the check box
const int SVRPropTree::PNINDENT                   = 16;			// child level indent
const int SVRPropTree::PROPTREEITEM_EXPANDBOXHALF = (PROPTREEITEM_EXPANDBOX / 2);

constexpr char* cOfficeFontName	= _T("Tahoma");
constexpr char* cDefaultFontName = _T("MS Sans Serif");

static int CALLBACK FontFamilyProcFonts(const LOGFONT FAR* lplf, const TEXTMETRIC FAR*, ULONG, LPARAM)
{
	ASSERT(nullptr != lplf);
	CString strFont = lplf->lfFaceName;
	return strFont.CollateNoCase (cOfficeFontName) == 0 ? 0 : 1;
}


/////////////////////////////////////////////////////////////////////////////
// SVRPropTree

UINT SVRPropTree::s_nInstanceCount;
CFont* SVRPropTree::s_pNormalFont;
CFont* SVRPropTree::s_pBoldFont;
SVRPropertyItem* SVRPropTree::s_pFound;

SVRPropTree::SVRPropTree() :
	m_bShowInfo(true),
	m_nInfoHeight(50),
	m_pVisbleList(nullptr),
	m_Origin(100,0),
	m_nLastUID(1),
	m_pFocus(nullptr),
	m_bDisableInput(false),
	m_bLockWindowUpdate(false),
	PROPTREEITEM_EXPANDCOLUMN(16),
	m_bShift(false)
{
	m_Root.Expand();

	// init global resources only once
	if (!s_nInstanceCount)
	{
		InitGlobalResources();
	}
	s_nInstanceCount++;
}


SVRPropTree::~SVRPropTree()
{
	DeleteAllItems();

	s_nInstanceCount--;

	// free global resource when ALL SVRPropTrees are destroyed
	if (!s_nInstanceCount)
	{
		FreeGlobalResources();
	}
}


BEGIN_MESSAGE_MAP(SVRPropTree, CWnd)
	//{{AFX_MSG_MAP(SVRPropTree)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ENABLE()
	ON_WM_SYSCOLORCHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// SVRPropTree message handlers

const POINT& SVRPropTree::GetOrigin()
{
	return m_Origin;
}


BOOL SVRPropTree::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CWnd* pWnd = this;

	LPCTSTR pszCreateClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW, ::LoadCursor(nullptr, IDC_ARROW));

	m_nID = nID;
	return pWnd->Create(pszCreateClass, _T(""), dwStyle, rect, pParentWnd, nID);
}


int SVRPropTree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	DWORD dwStyle;
	CRect rc;

	GetClientRect(rc);

	// create SVRPropTreeCtrl
	//

	dwStyle = WS_VISIBLE|WS_CHILD|WS_VSCROLL;

	if (!m_List.Create(dwStyle, rc, this, m_nID))
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
		TRACE0("Failed to create SVRPropTreeCtrl\n");
#endif
		return -1;
	}

	m_List.SetPropOwner(this);

	// create SVRPropertyInfo
	//

	dwStyle &= ~WS_VSCROLL;

	if (!m_Info.Create(_T(""), dwStyle, rc, this))
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
		TRACE0("Failed to create SVRPropertyInfo\n");
#endif
		return -1;
	}

	m_Info.SetPropOwner(this);

	return 0;
}


CWnd* SVRPropTree::GetCtrlParent()
{
	return &m_List;
}


void SVRPropTree::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	ResizeChildWindows(cx, cy);
}


void SVRPropTree::ResizeChildWindows(int cx, int cy)
{
	if (m_bShowInfo)
	{
		if (IsWindow(m_List.m_hWnd))
		{
			m_List.MoveWindow(0, 0, cx, cy - m_nInfoHeight);
		}

		if (IsWindow(m_Info.m_hWnd))
		{
			m_Info.MoveWindow(0, cy - m_nInfoHeight, cx, m_nInfoHeight);
		}
		m_Info.ShowWindow(SW_SHOW);
	}
	else
	{
		if (IsWindow(m_List.m_hWnd))
		{
			m_List.MoveWindow(0, 0, cx, cy);
		}
		m_Info.ShowWindow(SW_HIDE);
	}
}


void SVRPropTree::InitGlobalResources()
{
	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(info), &info, 0);

	LOGFONT lf;
	memset(&lf, 0, sizeof (LOGFONT));

	CWindowDC dc(nullptr);
	lf.lfCharSet = (BYTE)GetTextCharsetInfo(dc.GetSafeHdc(), nullptr, 0);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	// check if we should use system font
	_tcscpy(lf.lfFaceName, info.lfMenuFont.lfFaceName);

	bool fUseSystemFont = (info.lfMenuFont.lfCharSet > SYMBOL_CHARSET);
	if (!fUseSystemFont)
	{
		// check for "Tahoma" font existance:
		if (::EnumFontFamilies(dc.GetSafeHdc(), nullptr, FontFamilyProcFonts, 0)==0)
		{
			// Found! Use MS Office font!
			_tcscpy(lf.lfFaceName, cOfficeFontName);
		}
		else
		{
			// Not found. Use default font:
			_tcscpy(lf.lfFaceName, cDefaultFontName);
		}
	}

	FreeGlobalResources();

	s_pNormalFont = new CFont;
	s_pNormalFont->CreateFontIndirect(&lf);

	lf.lfWeight = FW_BOLD;
	s_pBoldFont = new CFont;
	s_pBoldFont->CreateFontIndirect(&lf);
}


void SVRPropTree::FreeGlobalResources()
{
	if (s_pNormalFont)
	{
		s_pNormalFont->DeleteObject();
		delete s_pNormalFont;
		s_pNormalFont = nullptr;
	}

	if (s_pBoldFont)
	{
		s_pBoldFont->DeleteObject();
		delete s_pBoldFont;
		s_pBoldFont = nullptr;
	}
}

void SVRPropTree::EnableListBoxStyle(bool bEnable)
{
	SetExpandColumnSize(1);
}

CFont* SVRPropTree::GetNormalFont()
{
	return s_pNormalFont;
}


CFont* SVRPropTree::GetBoldFont()
{
	return s_pBoldFont;
}


SVRPropertyItem* SVRPropTree::GetFocusedItem()
{
	return m_pFocus;
}


SVRPropertyItem* SVRPropTree::GetRootItem()
{
	return &m_Root;
}


void SVRPropTree::ClearVisibleList()
{
	m_pVisbleList = nullptr;
}

void SVRPropTree::SetExpandColumnSize(int iSize)
{
	PROPTREEITEM_EXPANDCOLUMN = iSize;
}

SVRPropertyItem* SVRPropTree::GetVisibleList()
{
	return m_pVisbleList;
}


void SVRPropTree::AddToVisibleList(SVRPropertyItem* pItem)
{
	if (nullptr == pItem)
	{
		return;
	}

	// check for an empty visible list
	if (!m_pVisbleList)
	{
		m_pVisbleList = pItem;
	}
	else
	{
		// Add the new item to the end of the list
		SVRPropertyItem* pNext;

		pNext = m_pVisbleList;
		while (pNext->GetNextVisible())
			pNext = pNext->GetNextVisible();

		pNext->SetNextVisible(pItem);
	}

	pItem->SetNextVisible(nullptr);
}


bool SVRPropTree::EnumItems(SVRPropertyItem* pItem, ENUMPROPITEMPROC proc, LPARAM lParam)
{
	if (nullptr == pItem || nullptr == proc)
	{
		return false;
	}

	SVRPropertyItem* pNext;

	// don't count the root item in any enumerations
	if (pItem!=&m_Root && !proc(this, pItem, lParam))
	{
		return false;
	}

	// recurse thru all child items
	pNext = pItem->GetChild();

	while (pNext)
	{
		if (!EnumItems(pNext, proc, lParam))
			return false;

		pNext = pNext->GetSibling();
	}

	return true;
}


void SVRPropTree::SetOriginOffset(LONG nOffset)
{
	m_Origin.y = nOffset;
}

	
void SVRPropTree::UpdatedItems()
{
	if( !IsWindow(m_hWnd) )
	{
		return;
	}

	if ( !m_bLockWindowUpdate )
	{
		m_List.UpdatePositions();

		Invalidate();

		m_List.UpdateResize();
		m_List.Invalidate();

		if ( nullptr != m_pFocus && ! m_pFocus->IsReadOnly() )
		{
			m_pFocus->Activate();
		}
	}
}


void SVRPropTree::DeleteAllItems()
{
	ClearVisibleList();	// prevent potential crash
	Delete(nullptr);
	UpdatedItems();
	m_nLastUID = 1; // reset uid counter
}


void SVRPropTree::DeleteItem(SVRPropertyItem* pItem)
{
	Delete(pItem);
	UpdatedItems();
}


LONG SVRPropTree::GetColumn()
{
	return m_Origin.x;
}


void SVRPropTree::SetColumn(LONG nColumn)
{
	CRect rc;

	GetClientRect(rc);
	
	if (rc.IsRectEmpty())
	{
		nColumn = __max(PROPTREEITEM_EXPANDCOLUMN, nColumn);
	}
	else
	{
		nColumn = __min(__max(PROPTREEITEM_EXPANDCOLUMN, nColumn), rc.Width() - PROPTREEITEM_EXPANDCOLUMN);
	}

	m_Origin.x = nColumn;

	Invalidate();
}


void SVRPropTree::Delete(SVRPropertyItem* pItem)
{
	if (pItem && pItem!=&m_Root && SendNotify(PTN_DELETEITEM, pItem))
	{
		return;
	}

	// passing in a NULL item is the same as calling DeleteAllItems
	if ( nullptr == pItem)
	{
		pItem = &m_Root;
	}

	// delete children

	SVRPropertyItem* pIter;
	SVRPropertyItem* pNext;

	pIter = pItem->GetChild();
	while (pIter)
	{
		pNext = pIter->GetSibling();
		DeleteItem(pIter);
		pIter = pNext;
	}

	// unlink from tree
	if (pItem->GetParent())
	{
		if (pItem->GetParent()->GetChild()==pItem)
		{
			pItem->GetParent()->SetChild(pItem->GetSibling());
		}
		else
		{
			pIter = pItem->GetParent()->GetChild();
			while (pIter->GetSibling() && pIter->GetSibling()!=pItem)
			{
				pIter = pIter->GetSibling();
			}

			if (pIter->GetSibling())
			{
				pIter->SetSibling(pItem->GetSibling());
			}
		}
	}

	if (pItem != &m_Root)
	{
		if (pItem==GetFocusedItem())
		{
			SetFocusedItem(nullptr);
		}
		delete pItem;
	}
}


void SVRPropTree::SetFocusedItem(SVRPropertyItem* pItem)
{
	// If new focus nullptr != item, it will activated and for the old one automatically the OnKill will be fired.
	// But if the new focus nullptr == item, the focus-parameter will be set to nullptr and no new focus will be set,
	// and the old item do not unrecognized this. In this case it has to be called a method to tell them item, that he loss the focus. 
	if (nullptr == pItem && m_pFocus != nullptr && IsWindow(m_hWnd))
	{
		m_pFocus->OnLossFocus();
	}
	
	m_pFocus = pItem;
	EnsureVisible(m_pFocus);

	if (!IsWindow(m_hWnd))
	{
		return;
	}

	Invalidate();
}


void SVRPropTree::ShowInfoText(bool bShow)
{
	m_bShowInfo = bShow;

	CRect rc;

	GetClientRect(rc);
	ResizeChildWindows(rc.Width(), rc.Height());
}


bool SVRPropTree::IsItemVisible(SVRPropertyItem* pItem)
{
	if (!pItem)
	{
		return false;
	}

	for (SVRPropertyItem* pNext = m_pVisbleList; pNext; pNext = pNext->GetNextVisible())
	{
		if (pNext==pItem)
		{
			return true;
		}
	}

	return false;
}


void SVRPropTree::EnsureVisible(SVRPropertyItem* pItem)
{
	if( nullptr == pItem)
	{
		return;
	}

	// item is not scroll visible (expand all parents)
	if (!IsItemVisible(pItem))
	{
		SVRPropertyItem* pParent;

		pParent = pItem->GetParent();
		while (pParent)
		{
			pParent->Expand();
			pParent = pParent->GetParent();
		}

		UpdatedItems();
		UpdateWindow();
	}

	ASSERT(IsItemVisible(pItem));

	CRect rc;

	m_List.GetClientRect(rc);
	rc.OffsetRect(0, m_Origin.y);
	rc.bottom -= pItem->GetHeight();

	CPoint pt;

	pt = pItem->GetLocation();

	if (!rc.PtInRect(pt))
	{
		LONG oy;

		if (pt.y < rc.top)
			oy = pt.y;
		else
			oy = pt.y - rc.Height() + pItem->GetHeight();

		m_List.OnVScroll(SB_THUMBTRACK, oy, nullptr);
	}
}


SVRPropertyItem* SVRPropTree::InsertItem(SVRPropertyItem* pItem, SVRPropertyItem* pParent)
{
	if( nullptr == pItem)
	{
		return nullptr;
	}

	if (!pParent)
	{
		pParent = &m_Root;
	}

	if (!pParent->GetChild())
	{
		pParent->SetChild(pItem);
	}
	else
	{
		// add to end of the sibling list
		SVRPropertyItem* pNext;

		pNext = pParent->GetChild();
		while (pNext->GetSibling())
		{
			pNext = pNext->GetSibling();
		}

		pNext->SetSibling(pItem);
	}

	pItem->SetParent(pParent);
	pItem->SetPropOwner(this);

	// auto generate a default ID
	pItem->SetCtrlID(m_nLastUID++);

	SendNotify(PTN_INSERTITEM, pItem);

	UpdatedItems();

	return pItem;
}



LONG SVRPropTree::HitTest(const POINT& pt)
{
	POINT p = pt;

	SVRPropertyItem* pItem;

	// convert screen to tree coordinates
	p.y += m_Origin.y;

	if (nullptr != (pItem = FindItem(pt)))
	{
		if (!pItem->IsRootLevel() && pt.x >= m_Origin.x - PROPTREEITEM_COLRNG && pt.x <= m_Origin.x + PROPTREEITEM_COLRNG)
		{
			return HTCOLUMN;
		}

		if (pt.x > m_Origin.x + PROPTREEITEM_COLRNG)
		{
			return HTATTRIBUTE;
		}

		if (pItem->HitExpand(p))
		{
			return HTEXPAND;
		}

		if (pItem->HitCheckBox(p))
		{
			return HTCHECKBOX;
		}

		return HTLABEL;
	}

	return HTCLIENT;
}


SVRPropertyItem* SVRPropTree::FindItem(const POINT& pt)
{
	SVRPropertyItem* pItem;

	CPoint p = pt;

	// convert screen to tree coordinates
	p.y += m_Origin.y;

	// search the visible list for the item
	for (pItem = m_pVisbleList; pItem; pItem = pItem->GetNextVisible())
	{
		CPoint ipt = pItem->GetLocation();
		if (p.y>=ipt.y && p.y<ipt.y + pItem->GetHeight())
			return pItem;
	}

	return nullptr;
}


SVRPropertyItem* SVRPropTree::FindItem(UINT nCtrlID)
{
	s_pFound = nullptr;

	EnumItems(&m_Root, EnumFindItem, nCtrlID);

	return s_pFound;
}


bool CALLBACK SVRPropTree::EnumFindItem(SVRPropTree*, SVRPropertyItem* pItem, LPARAM lParam)
{
	ASSERT(nullptr != pItem);

	if (pItem->GetCtrlID()==(UINT)lParam)
	{
		s_pFound = pItem;
		return FALSE;
	}

	return TRUE;
}


bool SVRPropTree::IsDisableInput()
{
	return m_bDisableInput;
}


void SVRPropTree::DisableInput(bool bDisable)
{
	m_bDisableInput = bDisable;

	CWnd* pWnd( GetParent() );

	if( nullptr != pWnd )
	{
		pWnd->EnableWindow(!bDisable);
	}
}


void SVRPropTree::SelectItems(SVRPropertyItem* pItem, bool bSelect)
{
	if( nullptr == pItem )
	{
		pItem = &m_Root;
	}

	EnumItems(pItem, EnumSelectAll, (LPARAM)bSelect);
}


SVRPropertyItem* SVRPropTree::FocusFirst()
{
	SVRPropertyItem *pold;

	pold = m_pFocus;

	SetFocusedItem(m_pVisbleList);

	if (m_pFocus)
	{
		SelectItems(nullptr, false);
		m_pFocus->Select();
	}

	if (pold!=m_pFocus)
	{
		SendNotify(PTN_SELCHANGE, m_pFocus);
	}

	return m_pFocus;
}


SVRPropertyItem* SVRPropTree::FocusLast()
{
	SVRPropertyItem* pNext;
	SVRPropertyItem* pChange;

	pChange = m_pFocus;

	pNext = m_pVisbleList;

	if (pNext)
	{
		while (pNext->GetNextVisible())
		{
			pNext = pNext->GetNextVisible();
		}

		SetFocusedItem(pNext);

		if (m_pFocus)
		{
			SelectItems(nullptr, false);
			m_pFocus->Select();
		}
	}

	if (pChange!=m_pFocus)
	{
		SendNotify(PTN_SELCHANGE, m_pFocus);
	}

	return pNext;
}


SVRPropertyItem* SVRPropTree::FocusPrev()
{
	SVRPropertyItem* pNext;
	SVRPropertyItem* pChange;

	pChange = m_pFocus;

	if (nullptr == m_pFocus)
	{
		// get the last visible item
		pNext = m_pVisbleList;
		while (pNext && pNext->GetNextVisible())
			pNext = pNext->GetNextVisible();
	}
	else
	{
		pNext = m_pVisbleList;
		while (pNext && pNext->GetNextVisible()!=m_pFocus)
			pNext = pNext->GetNextVisible();
	}

	if (pNext)
	{
		SetFocusedItem(pNext);
	}
	
	if (m_pFocus)
	{
		SelectItems(nullptr, false);
		m_pFocus->Select();
	}

	if (pChange!=m_pFocus)
	{
		SendNotify(PTN_SELCHANGE, m_pFocus);
	}

	return pNext;
}


SVRPropertyItem* SVRPropTree::FocusNext()
{
	SVRPropertyItem* pNext;
	SVRPropertyItem* pChange;

	pChange = m_pFocus;

	if (nullptr == m_pFocus)
	{
		pNext = m_pVisbleList;
	}
	else if (m_pFocus->GetNextVisible())
	{
		pNext = m_pFocus->GetNextVisible();
	}
	else
	{
		pNext = nullptr;
	}

	if( nullptr != pNext )
	{
		SetFocusedItem( pNext );
	}

	if (m_pFocus)
	{
		SelectItems(nullptr, false);
		m_pFocus->Select();
	}

	if (pChange!=m_pFocus)
	{
		SendNotify(PTN_SELCHANGE, m_pFocus);
	}

	return pNext;
}


void SVRPropTree::UpdateMoveAllItems()
{
	EnumItems(&m_Root, EnumMoveAll);
}


void SVRPropTree::RefreshItems(SVRPropertyItem* pItem)
{
	if( nullptr == pItem )
	{
		pItem = &m_Root;
	}

	EnumItems(pItem, EnumRefreshAll);

	UpdatedItems();
}


bool CALLBACK SVRPropTree::EnumSelectAll(SVRPropTree*, SVRPropertyItem* pItem, LPARAM lParam)
{
	if( nullptr == pItem )
	{
		return false;
	}

	pItem->Select((lParam ? true : false));

	return true;
}


bool CALLBACK SVRPropTree::EnumRefreshAll(SVRPropTree*, SVRPropertyItem* pItem, LPARAM)
{
	if( nullptr == pItem )
	{
		return false;
	}

	pItem->OnRefresh();

	return true;
}


bool CALLBACK SVRPropTree::EnumMoveAll(SVRPropTree*, SVRPropertyItem* pItem, LPARAM)
{
	if( nullptr == pItem )
	{
		return false;
	}

	pItem->OnMove();

	return true;
}


LRESULT SVRPropTree::SendNotify(UINT nNotifyCode, SVRPropertyItem* pItem)
{
	if (!IsWindow(m_hWnd))
	{
		return 0L;
	}

	if (!(GetStyle() & PTS_NOTIFY))
	{
		return 0L;
	}

	NMPROPTREE nmmp;
	LPNMHDR lpnm;

	lpnm = nullptr;

	switch (nNotifyCode)
	{
		case PTN_INSERTITEM:
		case PTN_DELETEITEM:
		case PTN_DELETEALLITEMS:
		case PTN_ITEMCHANGED:
		case PTN_ITEMBUTTONCLICK:
		case PTN_SELCHANGE:
		case PTN_ITEMEXPANDING:
		case PTN_COLUMNCLICK:
		case PTN_PROPCLICK:
		case PTN_CHECKCLICK:
		case PTN_QUERY_SHOW_BUTTON:
			lpnm = (LPNMHDR)&nmmp;
			nmmp.pItem = pItem;
			break;
	}

	if (lpnm)
	{
		UINT_PTR id = reinterpret_cast<UINT_PTR> (::GetMenu(m_hWnd));
		lpnm->code = nNotifyCode;
		lpnm->hwndFrom = m_hWnd;
		lpnm->idFrom = id;
	
		return GetParent()->SendMessage(WM_NOTIFY, (WPARAM)id, (LPARAM)lpnm);
	}

	return 0L;
}


void SVRPropTree::OnEnable(BOOL bEnable) 
{
	CWnd::OnEnable(bEnable);
	Invalidate();
}


void SVRPropTree::OnSysColorChange() 
{
	CWnd::OnSysColorChange();
	
	Invalidate();	
}


bool SVRPropTree::IsSingleSelection()
{
	// right now only support single selection
	return true;
}

bool SVRPropTree::SaveState(SVRPropTreeState& rState)
{
	bool bOK = true;
	SVRPropertyItem* pItem = GetRootItem();

	bOK = SaveState( _T(""), pItem, rState);
	return bOK;
}

bool SVRPropTree::SaveState(LPCTSTR ItemName, SVRPropertyItem* pItem, SVRPropTreeState& rState)
{
	bool bOK = true;
	if( pItem )
	{
		std::string Value( ItemName );
		Value += pItem->GetLabelText();
		Value += _T("¬");
		rState.m_State[Value.c_str()] = pItem->IsExpanded();

		SVRPropertyItem* pChild = pItem->GetChild();
		while( pChild )
		{
			bOK = SaveState( Value.c_str(), pChild, rState ) && bOK;
			pChild = pChild->GetSibling();
		}
	}
	return bOK;

}

bool SVRPropTree::RestoreState(const SVRPropTreeState& rState)
{
	bool bOK = true;
	SVRPropertyItem* pItem = GetRootItem();

	bOK = RestoreState( _T(""), pItem, rState);
	return bOK;
}

bool SVRPropTree::RestoreState(LPCTSTR ItemName, SVRPropertyItem* pItem, const SVRPropTreeState& rState)
{
	bool bOK = true;
	if( nullptr != pItem )
	{
		std::string Value( ItemName );
		Value += pItem->GetLabelText();
		Value += _T("¬");

		SVRPropTreeState::MapStringTreeState::const_iterator iter =  rState.m_State.find( Value.c_str() );
		if( iter!= rState.m_State.end() )
		{
			pItem->Expand( iter->second );

			SVRPropertyItem* pChild = pItem->GetChild();
			while( pChild )
			{
				bOK = RestoreState( Value.c_str(), pChild, rState ) && bOK;
				pChild = pChild->GetSibling();
			}
		}
		else
		{
			bOK = false;
		}

	}
	return bOK;

}
