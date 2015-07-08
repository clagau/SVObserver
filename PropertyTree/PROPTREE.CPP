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

#include "stdafx.h"
#include "PropTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
#define PROPTREEITEM_EXPANDCOLUMN		16			// width of the expand column
#define PROPTREEITEM_COLRNG				5			// width of splitter
//*/
//int SVRPropTree::PROPTREEITEM_EXPANDCOLUMN       = 16;            // width of the expand column
const int SVRPropTree::PROPTREEITEM_COLRNG        = 5;             // width of splitter
const int SVRPropTree::PROPTREEITEM_DEFHEIGHT     = 21;			// default heigt of an item
const int SVRPropTree::PROPTREEITEM_SPACE         = 5; 			// default horz spacing
const int SVRPropTree::PROPTREEITEM_EXPANDBOX     = 9; 			// size of the expand box
const int SVRPropTree::PROPTREEITEM_CHECKBOX      = 14;			// size of the check box
const int SVRPropTree::PNINDENT                   = 16;			// child level indent
const int SVRPropTree::PROPTREEITEM_EXPANDBOXHALF = (PROPTREEITEM_EXPANDBOX / 2);

static const CString strOfficeFontName	= _T("Tahoma");
static const CString strDefaultFontName = _T("MS Sans Serif");

static int CALLBACK FontFamilyProcFonts(const LOGFONT FAR* lplf, const TEXTMETRIC FAR*, ULONG, LPARAM)
{
	ASSERT(lplf != NULL);
	CString strFont = lplf->lfFaceName;
	return strFont.CollateNoCase (strOfficeFontName) == 0 ? 0 : 1;
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
	m_pVisbleList(NULL),
	m_Origin(100,0),
	m_nLastUID(1),
	m_pFocus(NULL),
	m_bDisableInput(false),
	m_bLockWindowUpdate(false),
	PROPTREEITEM_EXPANDCOLUMN(16),
	m_bShift(false)
{
	m_Root.Expand();

	// init global resources only once
	if (!s_nInstanceCount)
		InitGlobalResources();
	s_nInstanceCount++;
}


SVRPropTree::~SVRPropTree()
{
	DeleteAllItems();

	s_nInstanceCount--;

	// free global resource when ALL SVRPropTrees are destroyed
	if (!s_nInstanceCount)
		FreeGlobalResources();
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

	LPCTSTR pszCreateClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW, ::LoadCursor(NULL, IDC_ARROW));

	m_nID = nID;
	return pWnd->Create(pszCreateClass, _T(""), dwStyle, rect, pParentWnd, nID);
}


int SVRPropTree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	DWORD dwStyle;
	CRect rc;

	GetClientRect(rc);

	// create SVRPropTreeCtrl
	//

	dwStyle = WS_VISIBLE|WS_CHILD|WS_VSCROLL;

	if (!m_List.Create(dwStyle, rc, this, m_nID))
	{
		TRACE0("Failed to create SVRPropTreeCtrl\n");
		return -1;
	}

	m_List.SetPropOwner(this);

	// create SVRPropertyInfo
	//

	dwStyle &= ~WS_VSCROLL;

	if (!m_Info.Create(_T(""), dwStyle, rc, this))
	{
		TRACE0("Failed to create SVRPropertyInfo\n");
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
			m_List.MoveWindow(0, 0, cx, cy - m_nInfoHeight);

		if (IsWindow(m_Info.m_hWnd))
			m_Info.MoveWindow(0, cy - m_nInfoHeight, cx, m_nInfoHeight);
		m_Info.ShowWindow(SW_SHOW);
	}
	else
	{
		if (IsWindow(m_List.m_hWnd))
			m_List.MoveWindow(0, 0, cx, cy);
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

	CWindowDC dc(NULL);
	lf.lfCharSet = (BYTE)GetTextCharsetInfo(dc.GetSafeHdc(), NULL, 0);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	// check if we should use system font
	_tcscpy(lf.lfFaceName, info.lfMenuFont.lfFaceName);

	bool fUseSystemFont = (info.lfMenuFont.lfCharSet > SYMBOL_CHARSET);
	if (!fUseSystemFont)
	{
		// check for "Tahoma" font existance:
		if (::EnumFontFamilies(dc.GetSafeHdc(), NULL, FontFamilyProcFonts, 0)==0)
		{
			// Found! Use MS Office font!
			_tcscpy(lf.lfFaceName, strOfficeFontName);
		}
		else
		{
			// Not found. Use default font:
			_tcscpy(lf.lfFaceName, strDefaultFontName);
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
		s_pNormalFont = NULL;
	}

	if (s_pBoldFont)
	{
		s_pBoldFont->DeleteObject();
		delete s_pBoldFont;
		s_pBoldFont = NULL;
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
	m_pVisbleList = NULL;
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
	if (!pItem)
		return;

	// check for an empty visible list
	if (!m_pVisbleList)
		m_pVisbleList = pItem;
	else
	{
		// Add the new item to the end of the list
		SVRPropertyItem* pNext;

		pNext = m_pVisbleList;
		while (pNext->GetNextVisible())
			pNext = pNext->GetNextVisible();

		pNext->SetNextVisible(pItem);
	}

	pItem->SetNextVisible(NULL);
}


bool SVRPropTree::EnumItems(SVRPropertyItem* pItem, ENUMPROPITEMPROC proc, LPARAM lParam)
{
	if (!pItem || !proc)
		return false;

	SVRPropertyItem* pNext;

	// don't count the root item in any enumerations
	if (pItem!=&m_Root && !proc(this, pItem, lParam))
		return false;

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
	if (!IsWindow(m_hWnd))
		return;

	if ( !m_bLockWindowUpdate )
	{
		m_List.UpdatePositions();

		Invalidate();

		m_List.UpdateResize();
		m_List.Invalidate();

		if ( m_pFocus != NULL && ! m_pFocus->IsReadOnly() )
		{
			m_pFocus->Activate();
		}
	}
}


void SVRPropTree::DeleteAllItems()
{
	ClearVisibleList();	// prevent potential crash
	Delete(NULL);
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
		nColumn = __max(PROPTREEITEM_EXPANDCOLUMN, nColumn);
	else
		nColumn = __min(__max(PROPTREEITEM_EXPANDCOLUMN, nColumn), rc.Width() - PROPTREEITEM_EXPANDCOLUMN);

	m_Origin.x = nColumn;

	Invalidate();
}


void SVRPropTree::Delete(SVRPropertyItem* pItem)
{
	if (pItem && pItem!=&m_Root && SendNotify(PTN_DELETEITEM, pItem))
		return;

	// passing in a NULL item is the same as calling DeleteAllItems
	if (!pItem)
		pItem = &m_Root;

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
			pItem->GetParent()->SetChild(pItem->GetSibling());
		else
		{
			pIter = pItem->GetParent()->GetChild();
			while (pIter->GetSibling() && pIter->GetSibling()!=pItem)
				pIter = pIter->GetSibling();

			if (pIter->GetSibling())
				pIter->SetSibling(pItem->GetSibling());
		}
	}

	if (pItem != &m_Root)
	{
		if (pItem==GetFocusedItem())
			SetFocusedItem(NULL);
		delete pItem;
	}
}


void SVRPropTree::SetFocusedItem(SVRPropertyItem* pItem)
{
	// If new focus item != nullptr, it will activated and for the old one automatically the OnKill will be fired.
	// But if the new focus item == nullptr, the focus-parameter will be set to nullptr and no new focus will be set,
	// and the old item do not unrecognized this. In this case it has to be called a method to tell them item, that he loss the focus. 
	if (nullptr == pItem && m_pFocus != nullptr)
	{
		m_pFocus->OnLossFocus();
	}
	
	m_pFocus = pItem;
	EnsureVisible(m_pFocus);

	if (!IsWindow(m_hWnd))
		return;

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
		return FALSE;

	for (SVRPropertyItem* pNext = m_pVisbleList; pNext; pNext = pNext->GetNextVisible())
	{
		if (pNext==pItem)
			return TRUE;
	}

	return FALSE;
}


void SVRPropTree::EnsureVisible(SVRPropertyItem* pItem)
{
	if (!pItem)
		return;

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

		m_List.OnVScroll(SB_THUMBTRACK, oy, NULL);
	}
}


SVRPropertyItem* SVRPropTree::InsertItem(SVRPropertyItem* pItem, SVRPropertyItem* pParent)
{
	if (!pItem)
		return NULL;

	if (!pParent)
		pParent = &m_Root;

	if (!pParent->GetChild())
		pParent->SetChild(pItem);
	else
	{
		// add to end of the sibling list
		SVRPropertyItem* pNext;

		pNext = pParent->GetChild();
		while (pNext->GetSibling())
			pNext = pNext->GetSibling();

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

	if ((pItem = FindItem(pt))!=NULL)
	{
		if (!pItem->IsRootLevel() && pt.x >= m_Origin.x - PROPTREEITEM_COLRNG && pt.x <= m_Origin.x + PROPTREEITEM_COLRNG)
			return HTCOLUMN;

		if (pt.x > m_Origin.x + PROPTREEITEM_COLRNG)
			return HTATTRIBUTE;

		if (pItem->HitExpand(p))
			return HTEXPAND;

		if (pItem->HitCheckBox(p))
			return HTCHECKBOX;

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

	return NULL;
}


SVRPropertyItem* SVRPropTree::FindItem(UINT nCtrlID)
{
	s_pFound = NULL;

	EnumItems(&m_Root, EnumFindItem, nCtrlID);

	return s_pFound;
}


bool CALLBACK SVRPropTree::EnumFindItem(SVRPropTree*, SVRPropertyItem* pItem, LPARAM lParam)
{
	ASSERT(pItem!=NULL);

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

	CWnd* pWnd;

	if ((pWnd = GetParent())!=NULL)
		pWnd->EnableWindow(!bDisable);
}


void SVRPropTree::SelectItems(SVRPropertyItem* pItem, bool bSelect)
{
	if (!pItem)
		pItem = &m_Root;

	EnumItems(pItem, EnumSelectAll, (LPARAM)bSelect);
}


SVRPropertyItem* SVRPropTree::FocusFirst()
{
	SVRPropertyItem *pold;

	pold = m_pFocus;

	SetFocusedItem(m_pVisbleList);

	if (m_pFocus)
	{
		SelectItems(NULL, false);
		m_pFocus->Select();
	}

	if (pold!=m_pFocus)
		SendNotify(PTN_SELCHANGE, m_pFocus);

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
			pNext = pNext->GetNextVisible();

		SetFocusedItem(pNext);

		if (m_pFocus)
		{
			SelectItems(NULL, false);
			m_pFocus->Select();
		}
	}

	if (pChange!=m_pFocus)
		SendNotify(PTN_SELCHANGE, m_pFocus);

	return pNext;
}


SVRPropertyItem* SVRPropTree::FocusPrev()
{
	SVRPropertyItem* pNext;
	SVRPropertyItem* pChange;

	pChange = m_pFocus;

	if (m_pFocus==NULL)
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
		SetFocusedItem(pNext);
	
	if (m_pFocus)
	{
		SelectItems(NULL, false);
		m_pFocus->Select();
	}

	if (pChange!=m_pFocus)
		SendNotify(PTN_SELCHANGE, m_pFocus);

	return pNext;
}


SVRPropertyItem* SVRPropTree::FocusNext()
{
	SVRPropertyItem* pNext;
	SVRPropertyItem* pChange;

	pChange = m_pFocus;

	if (m_pFocus==NULL)
		pNext = m_pVisbleList;
	else
	if (m_pFocus->GetNextVisible())
		pNext = m_pFocus->GetNextVisible();
	else
		pNext = NULL;

	if (pNext)
		SetFocusedItem(pNext);

	if (m_pFocus)
	{
		SelectItems(NULL, false);
		m_pFocus->Select();
	}

	if (pChange!=m_pFocus)
		SendNotify(PTN_SELCHANGE, m_pFocus);

	return pNext;
}


void SVRPropTree::UpdateMoveAllItems()
{
	EnumItems(&m_Root, EnumMoveAll);
}


void SVRPropTree::RefreshItems(SVRPropertyItem* pItem)
{
	if (!pItem)
		pItem = &m_Root;

	EnumItems(pItem, EnumRefreshAll);

	UpdatedItems();
}


bool CALLBACK SVRPropTree::EnumSelectAll(SVRPropTree*, SVRPropertyItem* pItem, LPARAM lParam)
{
	if (!pItem)
		return false;

	pItem->Select((lParam ? true : false));

	return true;
}


bool CALLBACK SVRPropTree::EnumRefreshAll(SVRPropTree*, SVRPropertyItem* pItem, LPARAM)
{
	if (!pItem)
		return false;

	pItem->OnRefresh();

	return true;
}


bool CALLBACK SVRPropTree::EnumMoveAll(SVRPropTree*, SVRPropertyItem* pItem, LPARAM)
{
	if (!pItem)
		return false;

	pItem->OnMove();

	return true;
}


LRESULT SVRPropTree::SendNotify(UINT nNotifyCode, SVRPropertyItem* pItem)
{
	if (!IsWindow(m_hWnd))
		return 0L;

	if (!(GetStyle() & PTS_NOTIFY))
		return 0L;

	NMPROPTREE nmmp;
	LPNMHDR lpnm;

	lpnm = NULL;

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
		UINT id = (UINT)::GetMenu(m_hWnd);
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

	CString sItemName;
	bOK = SaveState(sItemName, pItem, rState);
	return bOK;
}

bool SVRPropTree::SaveState(CString sItemName, SVRPropertyItem* pItem, SVRPropTreeState& rState)
{
	bool bOK = true;
	if( pItem )
	{
		sItemName += pItem->GetLabelText();
		sItemName += _T("¬");
		SVRPropTreeState::ItemType ItemState;
		ItemState.bExpanded = pItem->IsExpanded();
		rState.m_State[sItemName] = ItemState;

		SVRPropertyItem* pChild = pItem->GetChild();
		while( pChild )
		{
			bOK = SaveState( sItemName, pChild, rState ) && bOK;
			pChild = pChild->GetSibling();
		}
	}
	return bOK;

}

bool SVRPropTree::RestoreState(const SVRPropTreeState& rState)
{
	bool bOK = true;
	SVRPropertyItem* pItem = GetRootItem();

	CString sItemName;
	bOK = RestoreState(sItemName, pItem, rState);
	return bOK;
}

bool SVRPropTree::RestoreState(CString sItemName, SVRPropertyItem* pItem, const SVRPropTreeState& rState)
{
	bool bOK = true;
	if( pItem )
	{
		sItemName += pItem->GetLabelText();
		sItemName += _T("¬");

		SVRPropTreeState::MapType::const_iterator iter =  rState.m_State.find(sItemName);
		if( iter!= rState.m_State.end() )
		{
			pItem->Expand( iter->second.bExpanded );

			SVRPropertyItem* pChild = pItem->GetChild();
			while( pChild )
			{
				bOK = RestoreState( sItemName, pChild, rState ) && bOK;
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

/*
BOOL SVRPropTree::LockWindowUpdate()
{
	m_bLockWindowUpdate = true;
	return TRUE;
}

void SVRPropTree::UnlockWindowUpdate()
{
	m_bLockWindowUpdate = false;
	UpdatedItems();
}
*/

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\PropertyTree\PROPTREE.CPP_v  $
 * 
 *    Rev 1.1   03 Jul 2013 16:26:16   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 020.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   06 Jun 2013 08:02:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  827
 * SCR Title:  Add button to the adjust tool size/position to set size to parent image.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fix keyboard navigation using tab to move from field to field.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2013 16:41:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   10 Jul 2012 11:35:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Init method to free resources before allocating new ones.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   29 Oct 2007 14:57:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added DeleteObject(s) in FreeGlobalResources.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   19 May 2005 14:32:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   centralized constants
 * fixed potential crash
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   23 Jul 2004 07:43:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  435
 * SCR Title:  Add Bulb Mode Operation to the Digital SVIM for Point Gray Dragonfly Cameras
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated UpdatedItems method to re-caculate item positions and set the focused item active if not read only.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Jul 2004 16:16:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented Save / Restore state
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Jul 2003 08:40:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added PTN_QUERY_SHOW_BUTTON
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 15:41:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of the files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/