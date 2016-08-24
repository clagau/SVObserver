//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMonitorListView
//* .File Name       : $Workfile:   MonitorListView.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   17 Oct 2014 13:34:14  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/bind.hpp>
#include "MonitorListView.h"
#include "SVObserver.h"
#include "SVIODoc.h"
#include "SVConfigurationObject.h"
#include "SVSVIMStateClass.h"
#include "MonitorListSheet.h"
#include "MonitorListPropertyDlg.h"
#include "RemoteMonitorListHelper.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVOResource/ConstGlobalSvOr.h"
#pragma endregion Includes

enum MonitorListViewUpdateHints
{
	None = 0,
	ExpandCollapseHint = 1,
	EditNodeHint = 2,
	RemoveNodeHint = 3
};

static const CString scPPQ = _T("PPQ");
static const CString scITEM = _T("Item");
static const CString scPRODUCTITEMLIST = _T("ProductItemList");
static const CString scREJECTCONDITIONLIST = _T("RejectConditionList");
static const CString scFAILSTATUSLIST = _T("FailStatusList");
static const CString scVALUES = _T("Values");
static const CString scIMAGES = _T("Images");


static SVString GetItemName(CListCtrl& rCtrl, int item)
{
	SVString name = rCtrl.GetItemText(item, 0);
	return  name;
}

static SVString GetListName(CListCtrl& rCtrl, int item)
{
	SVString name;
	int index = item;

	// check if we are at the MonitorListNameNode
	LVITEM lvItem;
	lvItem.mask = LVIF_PARAM;
	lvItem.iItem = index;
	lvItem.iSubItem = 0;
	if (rCtrl.GetItem(&lvItem))
	{
		if (lvItem.lParam == MonitorListNameNode)
		{
			name = rCtrl.GetItemText(item, 0);
		}
	}
	
	// find the row that with LPARAM that is MonitorListNameNode
	if (name.empty())
	{
		// get the previous row until the lparam is MonitorListNameNode
		bool done = false;
		do
		{
			index = rCtrl.GetNextItem(index, LVNI_ABOVE);
			if (index >= 0)
			{
				LVITEM lvItem;
				lvItem.mask = LVIF_PARAM;
				lvItem.iItem = index;
				lvItem.iSubItem = 0;
				if (rCtrl.GetItem(&lvItem))
				{
					if (lvItem.lParam == MonitorListNameNode)
					{
						name = rCtrl.GetItemText(index, 0);
						done = true;
					}
				}
			}
		} while (index >= 0 && !done);
	}
	return name;
}

static int AddMonitoredItems(CListCtrl& rCtrl, const MonitoredObjectList& items, bool bCollapse, int insertPos, int indent, int itemData)
{
	int pos = 0;
	LVITEM lvItem;

	// Insert ProductItemsList Tag
	lvItem.mask = LVIF_IMAGE | LVIF_STATE | LVIF_INDENT | LVIF_PARAM;
	lvItem.iIndent = indent;
	lvItem.state = INDEXTOSTATEIMAGEMASK(2); // IDI_REMOTE_OUTPUT_ICON
	lvItem.stateMask = LVIS_STATEIMAGEMASK;
	lvItem.iImage = 0; // IDI_IOITEM_ICON
	lvItem.iItem = insertPos + pos;
	lvItem.iSubItem = 0;
	lvItem.lParam = itemData;

	for (MonitoredObjectList::const_iterator it = items.begin(); it != items.end();++it)
	{
		const SVString& name = RemoteMonitorListHelper::GetNameFromMonitoredObject(*it);
		if (!name.empty())
		{
			lvItem.iItem = insertPos + pos++;
			int curPos = rCtrl.InsertItem(&lvItem);
			rCtrl.SetItemText(curPos, 0, name.c_str());
		}
	}
	return pos;
}

static int AddFailStatusList(CListCtrl& rCtrl, const MonitoredObjectList& values, bool bCollapse, bool bValuesCollapse, int insertPos)
{
	int pos = 0;
	LVITEM lvItem;

	// Insert FailStatusList
	lvItem.mask = LVIF_IMAGE | LVIF_STATE | LVIF_INDENT | LVIF_PARAM;
	lvItem.iIndent = 1;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iImage = 2 + bCollapse;
	lvItem.iItem = insertPos + pos++;
	lvItem.iSubItem = 0;
	lvItem.lParam = FailStatusListNode;

	// Insert FailStatusList tag
	int curPos = rCtrl.InsertItem(&lvItem);
	rCtrl.SetItemText(curPos, 0, scFAILSTATUSLIST);

	if (!bCollapse)
	{
		// Insert Values tag
		lvItem.iIndent = 2;
		lvItem.iItem = insertPos + pos++;
		lvItem.iImage = 2 + bValuesCollapse;
		lvItem.lParam = FailStatusValuesNode;

		curPos = rCtrl.InsertItem(&lvItem);
		rCtrl.SetItemText(curPos, 0, scVALUES);

		if (!bValuesCollapse)
		{
			// Insert FailStatusList Values
			pos += AddMonitoredItems(rCtrl, values, bCollapse, insertPos + pos, 3, FailStatusValuesItemNode);
		}
	}
	return pos;
}

static int AddRejectConditionList(CListCtrl& rCtrl, const MonitoredObjectList& values, bool bCollapse, bool bValuesCollapse, int insertPos)
{
	int pos = 0;

	// Insert RejectConditionList
	LVITEM lvItem;

	lvItem.mask = LVIF_IMAGE | LVIF_STATE | LVIF_INDENT | LVIF_PARAM;
	lvItem.iIndent = 1;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iImage = 2 + bCollapse;
	lvItem.iItem = insertPos + pos++;
	lvItem.iSubItem = 0;
	lvItem.lParam = RejectConditionListNode;

	// Insert RejectConditionList tag
	int curPos = rCtrl.InsertItem(&lvItem);
	rCtrl.SetItemText(curPos, 0, scREJECTCONDITIONLIST);

	if (!bCollapse)
	{
		// Insert Values tag
		lvItem.iIndent = 2;
		lvItem.iItem = insertPos + pos++;
		lvItem.iImage = 2 + bValuesCollapse;
		lvItem.lParam = RejectConditionValuesNode;
	
		curPos = rCtrl.InsertItem(&lvItem);
		rCtrl.SetItemText(curPos, 0, scVALUES);

		if (!bValuesCollapse)
		{
			// Insert RejectConditionList Values
			pos += AddMonitoredItems(rCtrl, values, bCollapse, insertPos + pos, 3, RejectConditionValuesItemNode);
		}
	}
	return pos;
}

static int AddProductItemList(CListCtrl& rCtrl, const MonitoredObjectList& values, const MonitoredObjectList& images, bool bCollapse, bool bValuesCollapse, bool bImagesCollapse, int insertPos)
{
	int pos = 0;
	LVITEM lvItem;

	// Insert ProductItemsList Tag
	lvItem.mask = LVIF_IMAGE | LVIF_STATE | LVIF_INDENT | LVIF_PARAM;
	lvItem.iIndent = 1;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iImage = 2 + bCollapse;
	lvItem.iItem = insertPos + pos++;
	lvItem.iSubItem = 0;
	lvItem.lParam = ProductItemListNode;

	int curPos = rCtrl.InsertItem(&lvItem); 
	rCtrl.SetItemText(curPos, 0, scPRODUCTITEMLIST);

	if (!bCollapse)
	{
		// Insert Values Tag
		lvItem.iIndent = 2;
		lvItem.state = 0;
		lvItem.stateMask = 0;
		lvItem.iImage = 2 + bValuesCollapse;
		lvItem.iItem = insertPos + pos++;
		lvItem.iSubItem = 0;
		lvItem.lParam = ProductItemValuesNode;

		curPos = rCtrl.InsertItem(&lvItem);
		rCtrl.SetItemText(curPos, 0, scVALUES);

		if (!bValuesCollapse)
		{
			// Insert ProductItemList Values
			pos += AddMonitoredItems(rCtrl, values, bCollapse, insertPos + pos, 3, ProductItemValuesItemNode);
		}
				
		// Insert Images Tag
		lvItem.iIndent = 2;
		lvItem.state = 0;
		lvItem.stateMask = 0;
		lvItem.iImage = 2 + bImagesCollapse;
		lvItem.iItem = insertPos + pos++;
		lvItem.iSubItem = 0;
		lvItem.lParam = ProductItemImagesNode;

		curPos = rCtrl.InsertItem(&lvItem);
		rCtrl.SetItemText(curPos, 0, scIMAGES);

		if (!bImagesCollapse)
		{
			// Insert ProductItemList Images
			pos += AddMonitoredItems(rCtrl, images, bCollapse, insertPos + pos, 3, ProductItemImagesItemNode);
		}
	}
	return pos;
}
#pragma endregion Static Functions

IMPLEMENT_DYNCREATE(MonitorListView, CListView)

MonitorListView::MonitorListView() :
	m_rCtrl( GetListCtrl() )
{
	m_pDocument = dynamic_cast<SVIODoc *>(CListView::GetDocument());
	VERIFY(m_ContextMenuNamedList.LoadMenu(IDR_MONITOR_LIST_ADD_REMOVE));
	VERIFY(m_ContextMenuSubList.LoadMenu(IDR_MONITOR_SUB_LIST));
	VERIFY(m_ContextMenuNode.LoadMenu(IDR_MONITOR_LIST_ITEM));
}

MonitorListView::~MonitorListView()
{
	m_ContextMenuNamedList.DestroyMenu();
	m_ContextMenuSubList.DestroyMenu();
	m_ContextMenuNode.DestroyMenu();
}

BEGIN_MESSAGE_MAP(MonitorListView, CListView)
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_MONITORLIST_ADD_REMOVE_LIST, &MonitorListView::OnAddRemoveList)
	ON_COMMAND(ID_MONITORLIST_EDIT_PROPERTIES, &MonitorListView::OnEditListProperties)
	ON_COMMAND(ID_MONITORLIST_ADD_EDIT_ITEMS, &MonitorListView::OnEditItem)
	ON_COMMAND(ID_MONITORLIST_REMOVE_ITEM, &MonitorListView::OnDeleteItem)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

#ifdef _DEBUG
void MonitorListView::AssertValid() const
{
	CListView::AssertValid();
}

void MonitorListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif

SVIODoc* MonitorListView::GetDocument()
{
	if( nullptr == m_pDocument )
	{
		m_pDocument = dynamic_cast<SVIODoc *>(CListView::GetDocument());
	}
	return m_pDocument;
}

int MonitorListView::GetExpandCollapseState(MonitorListViewNodeType nodeType, const ExpandCollapseState& stateHolder)
{
	int state = -1;
	switch (nodeType)
	{
		case MonitorListNameNode:
			state = stateHolder.bListCollapsed;
			break;

		case ProductItemListNode:
			state = stateHolder.bProductCollapsed;
			break;

		case ProductItemValuesNode:
			state = stateHolder.bProductValuesCollapsed;
			break;

		case ProductItemImagesNode:
			state = stateHolder.bProductImagesCollapsed;
			break;

		case RejectConditionListNode:
			state = stateHolder.bRejectCollapsed;
			break;

		case RejectConditionValuesNode:
			state = stateHolder.bRejectValuesCollapsed;
			break;

		case FailStatusListNode:
			state = stateHolder.bFailStatusCollapsed;
			break;

		case FailStatusValuesNode:
			state = stateHolder.bFailStatusValuesCollapsed;
			break;
	}
	return state;
}

bool MonitorListView::HandleExpandCollapse(MonitorListViewNodeType nodeType, ExpandCollapseState& stateHolder, int state)
{
	bool bRetVal = true;
	switch (nodeType)
	{
		case MonitorListNameNode:
			stateHolder.bListCollapsed = state;
			break;

		case ProductItemListNode:
			stateHolder.bProductCollapsed = state;
			break;

		case ProductItemValuesNode:
			stateHolder.bProductValuesCollapsed = state;
			break;

		case ProductItemImagesNode:
			stateHolder.bProductImagesCollapsed = state;
			break;

		case RejectConditionListNode:
			stateHolder.bRejectCollapsed = state;
			break;

		case RejectConditionValuesNode:
			stateHolder.bRejectValuesCollapsed = state;
			break;

		case FailStatusListNode:
			stateHolder.bFailStatusCollapsed = state;
			break;

		case FailStatusValuesNode:
			stateHolder.bFailStatusValuesCollapsed = state;
			break;

		case ProductItemValuesItemNode:
		case ProductItemImagesItemNode:
		case RejectConditionValuesItemNode:
		case FailStatusValuesItemNode:
			bRetVal = false;
			break;
	}
	return bRetVal;
}

void MonitorListView::CollapseItem(int item)
{
	LVITEM lvItem;
	lvItem.mask = LVIF_PARAM;
	lvItem.iItem = item;
	lvItem.iSubItem = 0;

	m_rCtrl.GetItem(&lvItem);
	MonitorListViewNodeType nodeType = static_cast<MonitorListViewNodeType>(lvItem.lParam);
	const SVString& name = GetListName(m_rCtrl, item);
	CollapseExpandMap::iterator it = m_ExpandCollapseStates.find(name);
	if (it != m_ExpandCollapseStates.end())
	{
		if (HandleExpandCollapse(nodeType, it->second, 1))
		{
			OnUpdate(this, ExpandCollapseHint, nullptr);
		}
	}
}

void MonitorListView::ExpandItem(int item)
{
	LVITEM lvItem;
	lvItem.mask = LVIF_PARAM;
	lvItem.iItem = item;
	lvItem.iSubItem = 0;

	m_rCtrl.GetItem(&lvItem);

	const SVString& name = GetListName(m_rCtrl, item);
	CollapseExpandMap::iterator it = m_ExpandCollapseStates.find(name);
	if (it != m_ExpandCollapseStates.end())
	{
		MonitorListViewNodeType nodeType = static_cast<MonitorListViewNodeType>(lvItem.lParam);
		if (HandleExpandCollapse(nodeType, it->second, 0))
		{
			OnUpdate(this, ExpandCollapseHint, nullptr);
		}
	}
}

void MonitorListView::ToggleExpandCollapse(int item)
{
	LVITEM lvItem;
	lvItem.mask = LVIF_PARAM;
	lvItem.iItem = item;
	lvItem.iSubItem = 0;

	m_rCtrl.GetItem(&lvItem);

	const SVString& name = GetListName(m_rCtrl, item);
	CollapseExpandMap::iterator it = m_ExpandCollapseStates.find(name);
	if (it != m_ExpandCollapseStates.end())
	{
		MonitorListViewNodeType nodeType = static_cast<MonitorListViewNodeType>(lvItem.lParam);
		int state = GetExpandCollapseState(nodeType, it->second);
		if (state >= 0)
		{
			if (HandleExpandCollapse(nodeType, it->second, (state) ? 0 : 1))
			{
				OnUpdate(this, ExpandCollapseHint, nullptr);
			}
		}
	}
}

int MonitorListView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->style |= LVS_SINGLESEL;
	if (CListView::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	CWinApp* pApp = AfxGetApp();
	ImageList.Create( SvOr::IconSize, SvOr::IconSize, ILC_COLOR | ILC_MASK, 0, 1 );
	ImageList.Add(pApp->LoadIcon(IDI_IOITEM_ICON));		// 0
	ImageList.Add( pApp->LoadIcon(IDI_NOIOITEM_ICON));	// 1
	ImageList.Add( pApp->LoadIcon(IDI_COLLAPSE));		// 2
	ImageList.Add( pApp->LoadIcon(IDI_EXPAND));			// 3

	StateImageList.Create( SvOr::IconSize, SvOr::IconSize, ILC_COLOR | ILC_MASK, 0, 1 );
	StateImageList.Add(pApp->LoadIcon(IDI_PPQ_ICON));			// 0
	StateImageList.Add(pApp->LoadIcon(IDI_REMOTE_OUTPUT_ICON));	// 1

	m_rCtrl.SetImageList(&StateImageList, LVSIL_STATE);
	m_rCtrl.SetImageList(&ImageList, LVSIL_SMALL);

	// insert columns
	m_rCtrl.InsertColumn(0, scITEM, LVCFMT_LEFT, -1, -1);
	m_rCtrl.InsertColumn(1, scPPQ, LVCFMT_LEFT, -1, -1);
	
	m_rCtrl.SetColumnWidth(0, 500);
	m_rCtrl.SetColumnWidth(1, 125);

	m_rCtrl.SetExtendedStyle(m_rCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	return 0;
}

BOOL MonitorListView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |= (LVS_SHOWSELALWAYS | LVS_REPORT | LVS_NOSORTHEADER);
	cs.style &= ~LVS_SORTASCENDING;
	cs.style &= ~LVS_SORTDESCENDING;
	cs.dwExStyle |= LVS_EX_GRIDLINES;

	return CListView::PreCreateWindow(cs);
}

void MonitorListView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	SVIODoc* pIODoc = GetDocument();
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pIODoc && ::IsWindow(m_hWnd) && nullptr != pConfig )
	{
		pConfig->ValidateRemoteMonitorList(); // prune the list if necessary 

		m_rCtrl.SetRedraw(false);
		m_rCtrl.DeleteAllItems();

		if (lHint == None)
		{
			m_ExpandCollapseStates.clear();
		}
		int k = 0;

		const RemoteMonitorList& rList = pConfig->GetRemoteMonitorList();
		for (RemoteMonitorList::const_iterator it = rList.begin();it != rList.end();++it)
		{
			// Update State Map
			if (lHint == None)
			{
				m_ExpandCollapseStates[it->first] = ExpandCollapseState(); 
			}
			// Get The Expand/Collapse State
			ExpandCollapseState state = m_ExpandCollapseStates[it->first];

			// Insert List name
			LVITEM lvItem;
			lvItem.mask = LVIF_IMAGE | LVIF_STATE | LVIF_INDENT | LVIF_PARAM;
			lvItem.iIndent = 0;
			lvItem.state = INDEXTOSTATEIMAGEMASK(1);
			lvItem.stateMask = LVIS_STATEIMAGEMASK;
			lvItem.iImage = 2 + (state.bListCollapsed ? 1 : 0);

			lvItem.iItem = k++;
			lvItem.iSubItem = 0;
			lvItem.lParam = MonitorListNameNode;

			int ipos = m_rCtrl.InsertItem(&lvItem); 
			m_rCtrl.SetItemText(ipos, 0, it->first.c_str());

			// Set PPQ Name
			m_rCtrl.SetItemText(ipos, 1, it->second.GetPPQName().c_str());
		
			if (!state.bListCollapsed)
			{
				const RemoteMonitorNamedList& items = it->second;

				// Add ProductItemList
				const MonitoredObjectList& productValues = items.GetProductValuesList();
				const MonitoredObjectList& productImages = items.GetProductImagesList();
				ipos += AddProductItemList(m_rCtrl, productValues, productImages, state.bProductCollapsed, state.bProductValuesCollapsed, state.bProductImagesCollapsed, ++ipos);

				// Add RejectConditionList
				const MonitoredObjectList& rejectConditionList = items.GetRejectConditionList();
				ipos += AddRejectConditionList(m_rCtrl, rejectConditionList, state.bRejectCollapsed, state.bRejectValuesCollapsed, ++ipos);

				// Add FailStatusList
				const MonitoredObjectList& failStatusList = items.GetFailStatusList();
				ipos += AddFailStatusList(m_rCtrl, failStatusList, state.bFailStatusCollapsed, state.bFailStatusValuesCollapsed, ++ipos);
			}
			k = ipos;
		}
		m_rCtrl.SetRedraw(true);
	}
	// Do not call CListView::OnUpdate() - it will cause flicker
}

void MonitorListView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	UINT flags;
	int item = m_rCtrl.HitTest(point, &flags);

	if (item >= 0 && ((flags & LVHT_ONITEMLABEL) == LVHT_ONITEMLABEL))
	{
		if (TheSVObserverApp.OkToEdit())
		{
			SVSVIMStateClass::AddState(SV_STATE_EDITING);

			if (EditMonitoredItem(item))
			{
				SVSVIMStateClass::AddState(SV_STATE_MODIFIED);
				SVIODoc* pIODoc = GetDocument();
				if (pIODoc)
				{
					pIODoc->SetModifiedFlag();
				}
			}
			SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
			m_rCtrl.SetItemState(item, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		}
	}
}

BOOL MonitorListView::PreTranslateMessage(MSG* pMsg)
{
	BOOL bRet = false;
	if (pMsg->message == WM_KEYDOWN && TheSVObserverApp.OkToEdit())
	{
		POSITION Pos = m_rCtrl.GetFirstSelectedItemPosition();
		if (nullptr != Pos)
		{
			int item = m_rCtrl.GetNextSelectedItem(Pos);
			if (item >= 0)
			{
				if (pMsg->wParam == VK_DELETE)
				{
					OnDeleteItem();
					bRet = true;
				}
				else if (pMsg->wParam == VK_INSERT)
				{
					AddItem();
					bRet = true;
				}
				else if (pMsg->wParam == VK_RETURN)
				{
					OnEditItem();
					bRet = true;
				}
				else if (pMsg->wParam == VK_ADD)
				{
					ExpandItem(item);
					bRet = true;
				}
				else if (pMsg->wParam == VK_SUBTRACT)
				{
					// Collapse
					CollapseItem(item);
					bRet = true;
				}
			}
		}
	}
	if (!bRet)
	{
		bRet = CListView::PreTranslateMessage(pMsg);
	}
	return bRet;
}

bool MonitorListView::RemoveMonitoredItem(int item)
{
	bool bRetVal = false;

	// Get Node Type
	LVITEM lvItem;
	lvItem.mask = LVIF_PARAM;
	lvItem.iItem = item;
	lvItem.iSubItem = 0;

	m_rCtrl.GetItem(&lvItem);
	MonitorListViewNodeType nodeType = static_cast<MonitorListViewNodeType>(lvItem.lParam);

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	if( nullptr != pConfig )
	{
		const SVString& name = GetListName(m_rCtrl, item);
		RemoteMonitorList list = pConfig->GetRemoteMonitorList();
		RemoteMonitorList::iterator it = list.find(name);
	
		switch (nodeType)
		{
			case MonitorListNameNode:
				{
					// Remove MonitorList Entry
					// Show Add/Remove List Dialog
					bRetVal = pConfig->SetupRemoteMonitorList();
				}
				break;

			case ProductItemListNode:
				{
					//Bring up ProductItemList to edit
					MonitorListSheet dlg(_T("Edit ProductItemList"), PRODUCT_OBJECT_LIST, true, it->second, this);
					UINT_PTR rc = dlg.DoModal();
					if (IDOK == rc)
					{
						// Ensure the name didn't change...
						it->second = dlg.GetMonitorList();
						pConfig->SetRemoteMonitorList(list);
						bRetVal = true;
					}
				}
				break;

			case ProductItemValuesNode:
				{
					//Bring up ProductItemList Value Tab to edit
					MonitorListSheet dlg(_T("Edit ProductItemList"), PRODUCT_OBJECT_LIST, true, it->second, this);
					UINT_PTR rc = dlg.DoModal();
					if (IDOK == rc)
					{
						// Ensure the name didn't change...
						it->second = dlg.GetMonitorList();
						pConfig->SetRemoteMonitorList(list);
						bRetVal = true;
					}				
				}
				break;

			case ProductItemImagesNode:
				{
					//Bring up ProductItemList Image Tab to edit
					MonitorListSheet dlg(_T("Edit ProductItemList"), PRODUCT_OBJECT_LIST, true, it->second, this, 1);
					UINT_PTR rc = dlg.DoModal();
					if (IDOK == rc)
					{
						// Ensure the name didn't change...
						it->second = dlg.GetMonitorList();
						pConfig->SetRemoteMonitorList(list);
						bRetVal = true;
					}
				}
				break;

			case ProductItemValuesItemNode:
				{
					// Remove ProductItemList Value Item
					if (it != list.end())
					{
						// Remove ProductItemList Values Item
						MonitoredObjectList valuesList = it->second.GetProductValuesList();
						const MonitoredObject& rMatch = RemoteMonitorListHelper::GetMonitoredObjectFromName(GetItemName(m_rCtrl, item));
						MonitoredObjectList::iterator valueIt = std::find(valuesList.begin(), valuesList.end(), rMatch);
						if (valueIt != valuesList.end())
						{
							valuesList.erase(valueIt);
							it->second.SetProductValuesList(valuesList);
							pConfig->SetRemoteMonitorList(list);
							bRetVal = true;
						}
					}
				}
				break;

			case ProductItemImagesItemNode:
				{
					if (it != list.end())
					{
						// Remove ProductItemList Images Item
						MonitoredObjectList imagesList = it->second.GetProductImagesList();
						const MonitoredObject& rMatch = RemoteMonitorListHelper::GetMonitoredObjectFromName(GetItemName(m_rCtrl, item));
						MonitoredObjectList::iterator imageIt = std::find(imagesList.begin(), imagesList.end(), rMatch);
						if (imageIt != imagesList.end())
						{
							imagesList.erase(imageIt);
							it->second.SetProductImagesList(imagesList);
							pConfig->SetRemoteMonitorList(list);
							bRetVal = true;
						}
					}
				}
				break;

			case RejectConditionListNode:
			case RejectConditionValuesNode:
				{
					//Bring up RejectConditionList to edit
					MonitorListSheet dlg(_T("Edit RejectconditionList"), REJECT_CONDITION_LIST, true, it->second, this);
					UINT_PTR rc = dlg.DoModal();
					if (IDOK == rc)
					{
						// Ensure the name didn't change...
						it->second = dlg.GetMonitorList();
						pConfig->SetRemoteMonitorList(list);
						bRetVal = true;
					}
				}
				break;

			case RejectConditionValuesItemNode:
				{
					// remove RejectConditionList Value Item
					if (it != list.end())
					{
						MonitoredObjectList valuesList = it->second.GetRejectConditionList();
						const MonitoredObject& rMatch = RemoteMonitorListHelper::GetMonitoredObjectFromName(GetItemName(m_rCtrl, item));
						MonitoredObjectList::iterator valueIt = std::find(valuesList.begin(), valuesList.end(), rMatch);
						if (valueIt != valuesList.end())
						{
							valuesList.erase(valueIt);
							it->second.SetRejectConditionList(valuesList);
							pConfig->SetRemoteMonitorList(list);
							bRetVal = true;
						}
					}
				}
				break;

			case FailStatusListNode:
			case FailStatusValuesNode:
				{
					// bring up FailStatusList to edit
					MonitorListSheet dlg(_T("Edit FailStatusList"), FAIL_STATUS_LIST, true, it->second, this);
					UINT_PTR rc = dlg.DoModal();
					if (IDOK == rc)
					{
						// Ensure the name didn't change...
						it->second = dlg.GetMonitorList();
						pConfig->SetRemoteMonitorList(list);
						bRetVal = true;
					}
				}
				break;

			case FailStatusValuesItemNode:
				{
					// remove FailStatusList Value Item
					if (it != list.end())
					{
						MonitoredObjectList valuesList = it->second.GetFailStatusList();
						const MonitoredObject& rMatch = RemoteMonitorListHelper::GetMonitoredObjectFromName(GetItemName(m_rCtrl, item));
						MonitoredObjectList::iterator valueIt = std::find(valuesList.begin(), valuesList.end(), rMatch);
						if (valueIt != valuesList.end())
						{
							valuesList.erase(valueIt);
							it->second.SetFailStatusList(valuesList);
							pConfig->SetRemoteMonitorList(list);
							bRetVal = true;
						}
					}
				}
				break;
		}
		if (bRetVal)
		{
			OnUpdate(this, RemoveNodeHint, nullptr);
		}
	}
	return bRetVal;
}

bool MonitorListView::EditMonitoredItem(int item)
{
	bool bRetVal = false;

	// Get Node Type
	LVITEM lvItem;
	lvItem.mask = LVIF_PARAM;
	lvItem.iItem = item;
	lvItem.iSubItem = 0;

	m_rCtrl.GetItem(&lvItem);
	MonitorListViewNodeType nodeType = static_cast<MonitorListViewNodeType>(lvItem.lParam);

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	if( nullptr != pConfig )
	{
		const SVString& listName = GetListName(m_rCtrl, item);
		RemoteMonitorList rList = pConfig->GetRemoteMonitorList();
		RemoteMonitorList::iterator it = rList.find(listName);

		if (it != rList.end())
		{
			switch (nodeType)
			{
				case MonitorListNameNode:
					{
						// Show Add/Remove List Dialog
						bRetVal = pConfig->SetupRemoteMonitorList();
						if (bRetVal)
						{
							OnUpdate(this, 0, nullptr);
							return true;
						}
					}
					break;

				case ProductItemListNode:
				case ProductItemValuesNode:
				case ProductItemValuesItemNode:
					{
						MonitorListSheet dlg(_T("Edit ProductItemList"), PRODUCT_OBJECT_LIST, true, it->second, this);
						UINT_PTR rc = dlg.DoModal();
						if (IDOK == rc)
						{
							// Ensure the name didn't change...
							it->second = dlg.GetMonitorList();
							pConfig->SetRemoteMonitorList(rList);
							bRetVal = true;
						}
					}
					break;

				case ProductItemImagesNode:
				case ProductItemImagesItemNode:
					{
						MonitorListSheet dlg(_T("Edit ProductItemList"), PRODUCT_OBJECT_LIST, true, it->second, this, 1);
						UINT_PTR rc = dlg.DoModal();
						if (IDOK == rc)
						{
							// Ensure the name didn't change...
							it->second = dlg.GetMonitorList();
							pConfig->SetRemoteMonitorList(rList);
							bRetVal = true;
						}
					}
					break;

				case RejectConditionListNode:
				case RejectConditionValuesNode:
				case RejectConditionValuesItemNode:
					{
						MonitorListSheet dlg(_T("Edit RejectconditionList"), REJECT_CONDITION_LIST, true, it->second, this);
						UINT_PTR rc = dlg.DoModal();
						if (IDOK == rc)
						{
							// Ensure the name didn't change...
							it->second = dlg.GetMonitorList();
							pConfig->SetRemoteMonitorList(rList);
							bRetVal = true;
						}
					}
					break;

				case FailStatusListNode:
				case FailStatusValuesNode:
				case FailStatusValuesItemNode:
					{
						MonitorListSheet dlg(_T("Edit FailStatusList"), FAIL_STATUS_LIST, true, it->second, this);
						UINT_PTR rc = dlg.DoModal();
						if (IDOK == rc)
						{
							// Ensure the name didn't change...
							it->second = dlg.GetMonitorList();
							pConfig->SetRemoteMonitorList(rList);
							bRetVal = true;
						}
					}
					break;
			}
			if (bRetVal)
			{
				OnUpdate(this, EditNodeHint, nullptr);
			}
		}
	}
	return bRetVal;
}

void MonitorListView::AddItem()
{
	if (TheSVObserverApp.OkToEdit())
	{
		POSITION Pos = m_rCtrl.GetFirstSelectedItemPosition();
		int item = -1;
		if (Pos)
		{
			item = m_rCtrl.GetNextSelectedItem(Pos);
		}
		SVSVIMStateClass::AddState(SV_STATE_EDITING);

		if (EditMonitoredItem(item))
		{
			SVSVIMStateClass::AddState(SV_STATE_MODIFIED);
			SVIODoc* pIODoc = GetDocument();
			if (pIODoc)
			{
				pIODoc->SetModifiedFlag();
			}
		}
		SVSVIMStateClass::RemoveState(SV_STATE_EDITING);

		if (item > -1)
		{
			m_rCtrl.SetItemState(item, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		}
	}
}


void MonitorListView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	UINT nFlags = 0;
	CPoint ListPoint = point;

	if (TheSVObserverApp.OkToEdit())
	{
		m_rCtrl.ScreenToClient( &ListPoint);
			
		int item = m_rCtrl.HitTest(ListPoint, &nFlags);

		if (item >= 0)
		{
			//Get Node Type
			LVITEM lvItem;
			lvItem.mask = LVIF_PARAM;
			lvItem.iItem = item;
			lvItem.iSubItem = 0;

			m_rCtrl.GetItem(&lvItem);
			MonitorListViewNodeType nodeType = static_cast<MonitorListViewNodeType>(lvItem.lParam);

			switch (nodeType)
			{
			//Top List Node
			case MonitorListNameNode: 
				{
					CMenu* PupupMenu = m_ContextMenuNamedList.GetSubMenu(0);
					ASSERT(nullptr != PupupMenu);
					CWnd* PopupOwner = this;
					while (PopupOwner->GetStyle() & WS_CHILD)
					{
						PopupOwner = PopupOwner->GetParent();
					}
					PupupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
												point.x, point.y, PopupOwner);
					break;
				}
			//sub lists
			case ProductItemListNode:
			case ProductItemValuesNode:
			case ProductItemImagesNode:
			case RejectConditionListNode:
			case RejectConditionValuesNode:
			case FailStatusListNode:
			case FailStatusValuesNode:
				{
					CMenu* PupupMenu = m_ContextMenuSubList.GetSubMenu(0);
					ASSERT(nullptr != PupupMenu);
					CWnd* PopupOwner = this;
					while (PopupOwner->GetStyle() & WS_CHILD)
					{
						PopupOwner = PopupOwner->GetParent();
					}
					PupupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
												point.x, point.y, PopupOwner);
					break;
				}
			//leaf nodes
			case ProductItemValuesItemNode:
			case ProductItemImagesItemNode:
			case RejectConditionValuesItemNode:
			case FailStatusValuesItemNode:
				{
					CMenu* PupupMenu = m_ContextMenuNode.GetSubMenu(0);
					ASSERT(nullptr != PupupMenu);
					CWnd* PopupOwner = this;
					while (PopupOwner->GetStyle() & WS_CHILD)
					{
						PopupOwner = PopupOwner->GetParent();
					}
					PupupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
												point.x, point.y, PopupOwner);
					break;
				}		
			default:
				{
					break;
				}
			} //end switch
		}//end if item >= 0
	}// Ok to Edit
}

void MonitorListView::OnAddRemoveList()
{
	if (TheSVObserverApp.OkToEdit())
	{
		bool bRet = true;
		SVSVIMStateClass::AddState(SV_STATE_EDITING);
		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		if( nullptr != pConfig )
		{
			bRet = pConfig->SetupRemoteMonitorList();
			if (bRet)
			{
				SVSVIMStateClass::AddState(SV_STATE_MODIFIED);
				SVIODoc* pIODoc = GetDocument();
				if (pIODoc)
				{
					pIODoc->SetModifiedFlag();
				}
				OnUpdate(this, 0, nullptr);
			}
		}
		SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
	}
}

void MonitorListView::OnEditListProperties()
{
	if (TheSVObserverApp.OkToEdit())
	{
		SVSVIMStateClass::AddState(SV_STATE_EDITING);
		POSITION Pos = m_rCtrl.GetFirstSelectedItemPosition();
		if (Pos)
		{
			int item = m_rCtrl.GetNextSelectedItem(Pos);

			SVConfigurationObject* pConfig( nullptr );
			SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
			if( nullptr != pConfig )
			{
				const SVString& listName = GetListName(m_rCtrl, item);
				RemoteMonitorList rList = pConfig->GetRemoteMonitorList();
				RemoteMonitorList::iterator it = rList.find(listName);
				if (it != rList.end())
				{
					MonitorListPropertyDlg dlg(rList, listName.c_str());

					if (IDOK == dlg.DoModal())
					{
						SVSVIMStateClass::AddState(SV_STATE_MODIFIED);
						SVIODoc* pIODoc = GetDocument();
						if (pIODoc)
						{
							pIODoc->SetModifiedFlag();
						}
						pConfig->SetRemoteMonitorList(rList);
						OnUpdate(this, 0, nullptr);
					}
				}
			}
		}
		SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
	}
}

void MonitorListView::OnDeleteItem()
{
	if (TheSVObserverApp.OkToEdit())
	{
		SVSVIMStateClass::AddState(SV_STATE_EDITING);
		POSITION Pos = m_rCtrl.GetFirstSelectedItemPosition();
		if (nullptr != Pos)
		{
			int item = m_rCtrl.GetNextSelectedItem(Pos);
			int Nextitem = m_rCtrl.GetNextItem(item, LVNI_ABOVE);
			if (RemoveMonitoredItem(item))
			{
				SVSVIMStateClass::AddState(SV_STATE_MODIFIED);
				SVIODoc* pIODoc = GetDocument();
				if (pIODoc)
				{
					pIODoc->SetModifiedFlag();
				}

				if (m_rCtrl.GetItemCount())
				{
					if (item < 0)
					{
						item = 0;
					}
					m_rCtrl.SetItemState(Nextitem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
				}
			}
		}
		SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
	}
}

void MonitorListView::OnEditItem()
{
	if (TheSVObserverApp.OkToEdit())
	{
		SVSVIMStateClass::AddState(SV_STATE_EDITING);

		POSITION Pos = m_rCtrl.GetFirstSelectedItemPosition();
		if (nullptr != Pos)
		{
			int item = m_rCtrl.GetNextSelectedItem(Pos);
			if (EditMonitoredItem(item))
			{
				SVSVIMStateClass::AddState(SV_STATE_MODIFIED);
				SVIODoc* pIODoc = GetDocument();
				if (pIODoc)
				{
					pIODoc->SetModifiedFlag();
				}
			}
		}
		SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
	}
}

// Collapse and Expand logic if the user clicks on an expandable item in the list.
void MonitorListView::OnLButtonDown(UINT nFlags, CPoint point)
{
	UINT uFlags;
	int item = m_rCtrl.HitTest(point, &uFlags);
	if (!(uFlags & LVHT_ONITEMLABEL) && (uFlags & LVHT_ONITEMSTATEICON | LVHT_ONITEMICON))
	{
		// Toggle Collapse.
		if (item > -1)
		{
			ToggleExpandCollapse(item);
		}
	}
	CListView::OnLButtonDown(nFlags, point);
}

