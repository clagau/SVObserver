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
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "MonitorListPropertyDlg.h"
#include "RemoteMonitorListHelper.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVOResource/ConstGlobalSvOr.h"
#include "TextDefinesSvO.h"
#include "MonitorListSelector.h"

#pragma endregion Includes

enum MonitorListViewUpdateHints
{
	None = 0,
	ExpandCollapseHint = 1,
	EditNodeHint = 2,
	RemoveNodeHint = 3
};

constexpr char* cPPQ = _T("PPQ");
constexpr char* cItem = _T("Item");
constexpr char* cIsActivated = _T("Is Activated");
constexpr char* cProductITemList = _T("ProductItemList");
constexpr char* cRejectConditionList = _T("RejectConditionList");
constexpr char* cFailStatusList = _T("FailStatusList");
constexpr char* cValues = _T("Values");
constexpr char* cImages = _T("Images");

static std::string GetItemName(CListCtrl& rCtrl, int item)
{
	std::string name = rCtrl.GetItemText(item, 0);
	return  name;
}

static std::string GetListName(CListCtrl& rCtrl, int item)
{
	std::string name;
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

static int AddMonitoredItems(CListCtrl& rCtrl, const MonitoredObjectList& items, bool bCollapse, int NextLine, int indent, int itemData)
{

	LVITEM lvItem;
	int next(NextLine);
	// Insert ProductItemsList Tag
	lvItem.mask = LVIF_IMAGE | LVIF_STATE | LVIF_INDENT | LVIF_PARAM;
	lvItem.iIndent = indent;
	lvItem.state = INDEXTOSTATEIMAGEMASK(2); // IDI_REMOTE_OUTPUT_ICON
	lvItem.stateMask = LVIS_STATEIMAGEMASK;
	lvItem.iImage = 0; // IDI_IOITEM_ICON
	lvItem.iSubItem = 0;
	lvItem.lParam = itemData;

	for (MonitoredObjectList::const_iterator it = items.begin(); it != items.end(); ++it)
	{
		const std::string& name = RemoteMonitorListHelper::GetNameFromMonitoredObject(*it);
		if (!name.empty())
		{
			lvItem.iItem = next++;
			int curPos = rCtrl.InsertItem(&lvItem);
			rCtrl.SetItemText(curPos, 0, name.c_str());
		}
	}
	return next;
}

static int AddFailStatusList(CListCtrl& rCtrl, const MonitoredObjectList& values, bool bCollapse, bool bValuesCollapse, int NextLine)
{

	LVITEM lvItem;
	int next(NextLine);
	// Insert FailStatusList
	lvItem.mask = LVIF_IMAGE | LVIF_STATE | LVIF_INDENT | LVIF_PARAM;
	lvItem.iIndent = 1;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iImage = 2 + bCollapse;
	lvItem.iItem = next++;
	lvItem.iSubItem = 0;
	lvItem.lParam = FailStatusListNode;

	// Insert FailStatusList tag
	int curPos = rCtrl.InsertItem(&lvItem);

	rCtrl.SetItemText(curPos, 0, cFailStatusList);

	if (!bCollapse)
	{
		// Insert Values tag
		lvItem.iIndent = 2;
		lvItem.iItem = next++;
		lvItem.iImage = 2 + bValuesCollapse;
		lvItem.lParam = FailStatusValuesNode;

		curPos = rCtrl.InsertItem(&lvItem);
		rCtrl.SetItemText(curPos, 0, cValues);

		if (!bValuesCollapse)
		{
			// Insert FailStatusList Values
			next = AddMonitoredItems(rCtrl, values, bCollapse, next, 3, FailStatusValuesItemNode);
		}
	}
	return next;
}

static int AddRejectConditionList(CListCtrl& rCtrl, const MonitoredObjectList& values, bool bCollapse, bool bValuesCollapse, int NextLine)
{
	int next(NextLine);
	// Insert RejectConditionList
	LVITEM lvItem;

	lvItem.mask = LVIF_IMAGE | LVIF_STATE | LVIF_INDENT | LVIF_PARAM;
	lvItem.iIndent = 1;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iImage = 2 + bCollapse;
	lvItem.iItem = next++;
	lvItem.iSubItem = 0;
	lvItem.lParam = RejectConditionListNode;

	// Insert RejectConditionList tag
	int curPos = rCtrl.InsertItem(&lvItem);

	rCtrl.SetItemText(curPos, 0, cRejectConditionList);

	if (!bCollapse)
	{
		// Insert Values tag
		lvItem.iIndent = 2;
		lvItem.iItem = next++;
		lvItem.iImage = 2 + bValuesCollapse;
		lvItem.lParam = RejectConditionValuesNode;
		curPos = rCtrl.InsertItem(&lvItem);
		rCtrl.SetItemText(curPos, 0, cValues);

		if (!bValuesCollapse)
		{
			// Insert RejectConditionList Values
			next = AddMonitoredItems(rCtrl, values, bCollapse, next, 3, RejectConditionValuesItemNode);
		}
	}
	return next;
}

static int AddProductItemList(CListCtrl& rCtrl, const MonitoredObjectList& values, const MonitoredObjectList& images, bool bCollapse, bool bValuesCollapse, bool bImagesCollapse, int NextLine)
{

	int next(NextLine);
	LVITEM lvItem;

	// Insert ProductItemsList Tag
	lvItem.mask = LVIF_IMAGE | LVIF_STATE | LVIF_INDENT | LVIF_PARAM;
	lvItem.iIndent = 1;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iImage = 2 + bCollapse;
	lvItem.iItem = next++;
	lvItem.iSubItem = 0;
	lvItem.lParam = ProductItemListNode;

	int curPos = rCtrl.InsertItem(&lvItem);
	rCtrl.SetItemText(curPos, 0, cProductITemList);

	if (!bCollapse)
	{
		// Insert Values Tag
		lvItem.iIndent = 2;
		lvItem.state = 0;
		lvItem.stateMask = 0;
		lvItem.iImage = 2 + bValuesCollapse;
		lvItem.iItem = next++;
		lvItem.iSubItem = 0;
		lvItem.lParam = ProductItemValuesNode;

		curPos = rCtrl.InsertItem(&lvItem);
		rCtrl.SetItemText(curPos, 0, cValues);

		if (!bValuesCollapse)
		{
			// Insert ProductItemList Values
			next = AddMonitoredItems(rCtrl, values, bCollapse, next, 3, ProductItemValuesItemNode);
		}

		// Insert Images Tag
		lvItem.iIndent = 2;
		lvItem.state = 0;
		lvItem.stateMask = 0;
		lvItem.iImage = 2 + bImagesCollapse;
		lvItem.iItem = next++;
		lvItem.iSubItem = 0;
		lvItem.lParam = ProductItemImagesNode;

		curPos = rCtrl.InsertItem(&lvItem);

		rCtrl.SetItemText(curPos, 0, cImages);

		if (!bImagesCollapse)
		{
			// Insert ProductItemList Images
			next = AddMonitoredItems(rCtrl, images, bCollapse, next, 3, ProductItemImagesItemNode);
		}
	}
	return next;
}
#pragma endregion Static Functions

IMPLEMENT_DYNCREATE(MonitorListView, CListView)

MonitorListView::MonitorListView() :
	m_rCtrl(GetListCtrl())
{
	m_pDocument = dynamic_cast<SVIODoc *>(CListView::GetDocument());
	VERIFY(m_ContextMenuNamedList.LoadMenu(IDR_MONITOR_LIST_ADD_REMOVE));
	VERIFY(m_ContextMenuSubList.LoadMenu(IDR_MONITOR_SUB_LIST));
	VERIFY(m_ContextMenuNode.LoadMenu(IDR_MONITOR_LIST_ITEM));
	VERIFY(m_ContextMenuProductList.LoadMenu(IDR_MONITOR_PRODUCT_LIST));
}

MonitorListView::~MonitorListView()
{
	m_ContextMenuNamedList.DestroyMenu();
	m_ContextMenuSubList.DestroyMenu();
	m_ContextMenuNode.DestroyMenu();
	m_ContextMenuProductList.DestroyMenu();
}

BEGIN_MESSAGE_MAP(MonitorListView, CListView)
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_MONITORLIST_ADD_REMOVE_LIST, &MonitorListView::OnAddRemoveList)
	ON_COMMAND(ID_MONITORLIST_EDIT_PROPERTIES, &MonitorListView::OnEditListProperties)
	ON_COMMAND(ID_MONITORLIST_ADD_EDIT_ITEMS, &MonitorListView::OnEditItem)
	ON_COMMAND(ID_MONITORLIST_ADD_EDIT_IMAGE_ITEMS, &MonitorListView::OnEditImageItem)
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
	if (nullptr == m_pDocument)
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
	const std::string& name = GetListName(m_rCtrl, item);
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

	const std::string& name = GetListName(m_rCtrl, item);
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

	const std::string& name = GetListName(m_rCtrl, item);
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
	ImageList.Create(SvOr::IconSize, SvOr::IconSize, ILC_COLOR | ILC_MASK, 0, 1);
	ImageList.Add(pApp->LoadIcon(IDI_IOITEM_ICON));		// 0
	ImageList.Add(pApp->LoadIcon(IDI_NOIOITEM_ICON));	// 1
	ImageList.Add(pApp->LoadIcon(IDI_COLLAPSE));		// 2
	ImageList.Add(pApp->LoadIcon(IDI_EXPAND));			// 3

	StateImageList.Create(SvOr::IconSize, SvOr::IconSize, ILC_COLOR | ILC_MASK, 0, 1);
	StateImageList.Add(pApp->LoadIcon(IDI_PPQ_ICON));			// 0
	StateImageList.Add(pApp->LoadIcon(IDI_REMOTE_OUTPUT_ICON));	// 1

	m_rCtrl.SetImageList(&StateImageList, LVSIL_STATE);
	m_rCtrl.SetImageList(&ImageList, LVSIL_SMALL);

	// insert columns
	m_rCtrl.InsertColumn(0, cItem, LVCFMT_LEFT, -1, -1);
	m_rCtrl.InsertColumn(1, cPPQ, LVCFMT_LEFT, -1, -1);
	m_rCtrl.InsertColumn(2, cIsActivated, LVCFMT_LEFT, -1, -1);

	m_rCtrl.SetColumnWidth(0, 500);
	m_rCtrl.SetColumnWidth(1, 125);
	m_rCtrl.SetColumnWidth(2, 125);

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
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pIODoc && ::IsWindow(m_hWnd) && nullptr != pConfig)
	{
		pConfig->ValidateRemoteMonitorList(); // prune the list if necessary 

		m_rCtrl.SetRedraw(false);
		m_rCtrl.DeleteAllItems();

		if (lHint == None)
		{
			m_ExpandCollapseStates.clear();
		}
		int NextLine(0);
		const RemoteMonitorListMap& rList = pConfig->GetRemoteMonitorList();
		for (RemoteMonitorListMap::const_iterator it = rList.begin(); it != rList.end(); ++it)
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

			lvItem.iItem = NextLine++;
			lvItem.iSubItem = 0;
			lvItem.lParam = MonitorListNameNode;

			int ipos = m_rCtrl.InsertItem(&lvItem);
			m_rCtrl.SetItemText(ipos, 0, it->first.c_str());

			// Set PPQ Name
			m_rCtrl.SetItemText(ipos, 1, it->second.GetPPQName().c_str());

			// Set IsActivated 
			m_rCtrl.SetItemText(ipos, 2, it->second.IsActive() ? SvO::cTrue : SvO::cFalse);

			if (!state.bListCollapsed)
			{
				const RemoteMonitorNamedList& items = it->second;

				// Add ProductItemList
				const MonitoredObjectList& productValues = items.GetProductValuesList();
				const MonitoredObjectList& productImages = items.GetProductImagesList();
				NextLine = AddProductItemList(m_rCtrl, productValues, productImages, state.bProductCollapsed, state.bProductValuesCollapsed, state.bProductImagesCollapsed, NextLine);

				// Add RejectConditionList
				const MonitoredObjectList& rejectConditionList = items.GetRejectConditionList();
				NextLine = AddRejectConditionList(m_rCtrl, rejectConditionList, state.bRejectCollapsed, state.bRejectValuesCollapsed, NextLine);

				// Add FailStatusList
				const MonitoredObjectList& failStatusList = items.GetFailStatusList();
				NextLine = AddFailStatusList(m_rCtrl, failStatusList, state.bFailStatusCollapsed, state.bFailStatusValuesCollapsed, NextLine);
			}

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

			if (EditMonitoredItem(item,false))
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

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	if (nullptr != pConfig)
	{
		const std::string& name = GetListName(m_rCtrl, item);
		RemoteMonitorListMap list = pConfig->GetRemoteMonitorList();
		RemoteMonitorListMap::iterator it = list.find(name);

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
			
			MonitorlistSelector MLSelector(PRODUCT_OBJECT_LIST, false, it->second, NULL);
			int res = MLSelector.DisplayDialog();
			if (IDOK == res)
			{
				// Ensure the name didn't change...
				it->second = MLSelector.GetMonitorList();
				pConfig->SetRemoteMonitorList(list);
				bRetVal = true;
			}
		}
		break;

		case ProductItemValuesNode:
		{
			MonitorlistSelector MLSelector(PRODUCT_OBJECT_LIST, false, it->second, NULL);
			int res = MLSelector.DisplayDialog();
			if (res == 1)
			{
				// Ensure the name didn't change...
				it->second = MLSelector.GetMonitorList();
				pConfig->SetRemoteMonitorList(list);
				bRetVal = true;
			}
		}
		break;

		case ProductItemImagesNode:
		{
			MonitorlistSelector MLSelector(PRODUCT_OBJECT_LIST, true, it->second, NULL);
			int res = MLSelector.DisplayDialog();
			if (res == IDOK)
			{
				// Ensure the name didn't change...
				it->second = MLSelector.GetMonitorList();
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
			MonitorlistSelector MLSelector(REJECT_CONDITION_LIST, false, it->second, NULL);
			int res = MLSelector.DisplayDialog();
			//Bring up RejectConditionList to edit
			if (IDOK == res)
			{
				// Ensure the name didn't change...
				it->second = MLSelector.GetMonitorList();
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
			MonitorlistSelector MLSelector(REJECT_CONDITION_LIST, false, it->second, NULL);
			int res = MLSelector.DisplayDialog();

			if (IDOK == res)
			{
				// Ensure the name didn't change...
				it->second = MLSelector.GetMonitorList();
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

bool MonitorListView::EditMonitoredItem(int item, bool bEditImage)
{
	bool bRetVal = false;
	
	// Get Node Type
	LVITEM lvItem;
	lvItem.mask = LVIF_PARAM;
	lvItem.iItem = item;
	lvItem.iSubItem = 0;

	m_rCtrl.GetItem(&lvItem);
	MonitorListViewNodeType nodeType = static_cast<MonitorListViewNodeType>(lvItem.lParam);
	bool ImageTab(false);
	
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	if (nullptr != pConfig)
	{
		const std::string& listName = GetListName(m_rCtrl, item);
		RemoteMonitorListMap rList = pConfig->GetRemoteMonitorList();
		RemoteMonitorListMap::iterator it = rList.find(listName);

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
				ImageTab = bEditImage; ///fall thru
			case ProductItemValuesNode:
			case ProductItemValuesItemNode:
			{
				MonitorlistSelector MLSelector(PRODUCT_OBJECT_LIST, ImageTab, it->second, NULL);
				int res = MLSelector.DisplayDialog();
				if (res == 1)
				{
					// Ensure the name didn't change...
					it->second = MLSelector.GetMonitorList();
					pConfig->SetRemoteMonitorList(rList);
					bRetVal = true;
				}

			}
			break;

			case ProductItemImagesNode:
			case ProductItemImagesItemNode:
			{

				MonitorlistSelector MLSelector(PRODUCT_OBJECT_LIST, true, it->second, NULL);
				int res = MLSelector.DisplayDialog();
				if (res == 1)
				{
					// Ensure the name didn't change...
					it->second = MLSelector.GetMonitorList();
					pConfig->SetRemoteMonitorList(rList);
					bRetVal = true;
				}

			}
			break;

			case RejectConditionListNode:
			case RejectConditionValuesNode:
			case RejectConditionValuesItemNode:
			{
				MonitorlistSelector MLSelector(REJECT_CONDITION_LIST, false, it->second, NULL);
				int res = MLSelector.DisplayDialog();
				if (IDOK == res)
				{
					// Ensure the name didn't change...
					it->second = MLSelector.GetMonitorList();
					pConfig->SetRemoteMonitorList(rList);
					bRetVal = true;
				}
			}
			break;

			case FailStatusListNode:
			case FailStatusValuesNode:
			case FailStatusValuesItemNode:
			{
				MonitorlistSelector MLSelector(FAIL_STATUS_LIST, false, it->second, NULL);
				int res = MLSelector.DisplayDialog();
				if (IDOK == res)
				{
					// Ensure the name didn't change...
					it->second = MLSelector.GetMonitorList();
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

		if (EditMonitoredItem(item,false))
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
		m_rCtrl.ScreenToClient(&ListPoint);

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
			{
				CMenu* PupupMenu = m_ContextMenuProductList.GetSubMenu(0);
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
		SVConfigurationObject* pConfig(nullptr);
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

		if (nullptr != pConfig)
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

			SVConfigurationObject* pConfig(nullptr);
			SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
			if (nullptr != pConfig)
			{
				const std::string& listName = GetListName(m_rCtrl, item);
				RemoteMonitorListMap rList = pConfig->GetRemoteMonitorList();
				RemoteMonitorListMap::iterator it = rList.find(listName);
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

void  MonitorListView::OnEditImageItem()
{
	OnEdit(true);
}

void MonitorListView::OnEditItem()
{
	OnEdit(false);
}
void MonitorListView::OnEdit(bool bImageItem)
{
	if (TheSVObserverApp.OkToEdit())
	{
		SVSVIMStateClass::AddState(SV_STATE_EDITING);

		POSITION Pos = m_rCtrl.GetFirstSelectedItemPosition();
		if (nullptr != Pos)
		{
			int item = m_rCtrl.GetNextSelectedItem(Pos);
			if (EditMonitoredItem(item, bImageItem))
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

