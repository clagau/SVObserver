//*****************************************************************************
/// \copyright (c) 2023,2023 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file ResultTabControl.cpp
/// All Rights Reserved 
//*****************************************************************************
/// A class that handle the creation off new ResultView tabs, closing and renaming them
//******************************************************************************
#include <afxtabctrl.h>
#include "stdafx.h"
#include "Definitions/TextDefinesSvDef.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "ResultTabControl.h"
#include "SVIPDoc.h"
#include "SVObjectLibrary/SVClassRegisterListClass.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVToolSet.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVXMLLibrary/SVNavigateTree.h"


#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(ResultTabControl, CMFCTabCtrl)

BEGIN_MESSAGE_MAP(ResultTabControl, CMFCTabCtrl)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

constexpr int cResultListDefaultPosition = 0;
constexpr int cResultTableDefaultposition = 1;
#pragma endregion Declarations

#pragma region Constructor
#pragma endregion Constructor

#pragma region Public Methods
BOOL ResultTabControl::Create(Style style, const RECT& rect, CWnd* pParentWnd, UINT nID, Location location, BOOL bCloseBtn)
{
	BOOL result = CMFCTabCtrl::Create(style, rect, pParentWnd, nID, location, bCloseBtn);

	CView* parentWindow = dynamic_cast<CView*>(pParentWnd);
	m_pIPDoc = dynamic_cast<SVIPDoc*> (parentWindow->GetDocument());
	if (nullptr == m_pIPDoc)
	{
		return -1;
	}

	EnableActiveTabCloseButton(true);
	EnableTabSwap(false);
	EnableInPlaceEdit(true);

	m_EditViewForNewTab.Create(WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, getNextViewIndex());
	m_EditViewForNewTab.SetFont(&afxGlobalData.fontRegular);
	AddTab(&m_EditViewForNewTab, _T("+"));

	return result;
}

void ResultTabControl::Save(SvOi::IObjectWriter& rWriter)
{
	_variant_t svVariant;

	rWriter.StartElement(SvXml::CTAG_RESULT_TABS);
	int numberOfTabWithoutAdd = GetVisibleTabsNum() - 2;
	for (int i = cResultTableDefaultposition; i <= numberOfTabWithoutAdd; i++)
	{
		CWnd* view = GetTabWndNoWrapper(i);
		ResultTableListCtrl* tableListCtrl = dynamic_cast<ResultTableListCtrl*> (view);

		if (nullptr != tableListCtrl)
		{
			rWriter.StartElement(SvXml::CTAG_RESULT_TABLE_TAB);

			CString tabLabel;
			GetTabLabel(i, tabLabel);
			svVariant = tabLabel;
			rWriter.WriteAttribute(SvXml::CTAG_NAME, svVariant);
			svVariant.Clear();

			//Ids can be converted to {#000000} format an then auto reassiging new ids should work
			//convertObjectIdToVariant
			svVariant = convertObjectIdToVariant(tableListCtrl->getTableIdForTab());
			rWriter.WriteAttribute(SvXml::CTAG_UNIQUE_REFERENCE_ID, svVariant);
			svVariant.Clear();

			rWriter.EndElement();
		}
	}

	rWriter.EndElement();
}

bool ResultTabControl::Load(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent)
{
	_variant_t svVariant;
	SVTreeType::SVBranchHandle htiResultTabs = nullptr;

	bool bOk = SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_RESULT_TABS, htiParent, htiResultTabs);
	if (bOk)
	{
		SVTreeType::SVBranchHandle htiTab(nullptr);

		htiTab = rTree.getFirstBranch(htiResultTabs);

		while (nullptr != htiTab)
		{
			if (false == SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_NAME, htiTab, svVariant))
			{
				return false;
			}
			CString tabLabel = svVariant;
			svVariant.Clear();
			
			if (false == SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_UNIQUE_REFERENCE_ID, htiTab, svVariant))
			{
				return false;
			}

			uint32_t tableId = calcObjectId(svVariant);

			if (tabLabel == SvDef::cTableResultView && m_pDefaultResultTableList != nullptr)
			{
				CWnd* view = GetTabWndNoWrapper(cResultTableDefaultposition);
				ResultTableListCtrl* tableListCtrl = dynamic_cast<ResultTableListCtrl*> (view);
				if (nullptr != tableListCtrl)
				{
					tableListCtrl->setTableIdForTab(tableId);
				}
				SetActiveTab(cResultTableDefaultposition);
			}
			else
			{
				addNewTab(tabLabel, tableId);
			}

			htiTab = rTree.getNextBranch(htiResultTabs, htiTab);
		}
	}
	return bOk;
}

void ResultTabControl::updateTab()
{
	CWnd* view = GetTabWndNoWrapper(GetActiveTab());

	ResultTableListCtrl* tableListCtrl = dynamic_cast<ResultTableListCtrl*> (view);
	if (nullptr != tableListCtrl)
	{
		tableListCtrl->updateList();
	}

	ResultListCtrl* listCtrl = dynamic_cast<ResultListCtrl*> (view);
	if (nullptr != listCtrl)
	{
		listCtrl->updateList();
	}
}

void ResultTabControl::tableIdChanged()
{
	CWnd* view = GetTabWndNoWrapper(GetActiveTab());
	ResultTableListCtrl* tableListCtrl = dynamic_cast<ResultTableListCtrl*> (view);

	SVToolSet* toolset = m_pIPDoc->GetToolSet();
	tableListCtrl->setTableIdForTab(toolset->GetResultList()->getTableId());
	updateTab();

	SVSVIMStateClass::AddState(SV_STATE_MODIFIED);
}
#pragma endregion Public Methods

#pragma region Private Methods
void ResultTabControl::OnDraw(CDC* pDC)
{
	if (m_isInitialized)
	{
		m_defaultResultList.RedrawWindow();
		m_pDefaultResultTableList->RedrawWindow();
	}
	__super::OnDraw(pDC);
};

BOOL ResultTabControl::StartRenameTab(int iTab)
{
	if (isDefaultTab(iTab) || SVSVIMStateClass::CheckState(SV_STATE_RUNNING))
	{
		return false;
	}

	return CMFCTabCtrl::StartRenameTab(iTab);
}

BOOL ResultTabControl::RenameTab()
{
	bool renameStatus = false;
	CString newTabName, oldTabName;

	GetTabLabel(m_iEditedTab, oldTabName);
	renameStatus = __super::RenameTab();
	GetTabLabel(m_iEditedTab, newTabName);

	if (newTabName != oldTabName)
	{
		SVSVIMStateClass::AddState(SV_STATE_MODIFIED);
	}

	return renameStatus;
}

BOOL ResultTabControl::SetActiveTab(int iTab)
{
	BOOL setActive = false;
	CString tabLabel;

	iTab = iTab == -1 ? GetActiveTab() : iTab;
	GetTabLabel(iTab, tabLabel);
	CWnd* view = GetTabWndNoWrapper(iTab);

	if (tabLabel == "+" && m_isInitialized)
	{
		addNewTab();
		setActive = true;
	}
	else if (view == &m_defaultResultList || view == m_pDefaultResultTableList.get())
	{
		EnableActiveTabCloseButton(false);
		setActive = CMFCTabCtrl::SetActiveTab(iTab);
	}
	else
	{
		EnableActiveTabCloseButton(true);
		setActive = CMFCTabCtrl::SetActiveTab(iTab);
	}

	updateTab();

	return setActive;
}

void ResultTabControl::OnLButtonUp(UINT nFlags, CPoint point)
{
	int tabIndex = GetTabFromPoint(point);
	if (IsTabCloseButtonPressed())
	{
		CWnd* view = GetTabWndNoWrapper(GetActiveTab());
		SetActiveTab(tabIndex - 1);
		CMFCTabCtrl::OnLButtonUp(nFlags, point);
		
		RemoveTab(tabIndex);
		m_additionalTabs.erase(std::remove_if(m_additionalTabs.begin(), m_additionalTabs.end(), 
			[view](std::shared_ptr<CListCtrl> tab){return tab.get() == view; }),
			m_additionalTabs.end());
		SVSVIMStateClass::AddState(SV_STATE_MODIFIED);
	}
	else
	{
		CMFCTabCtrl::OnLButtonUp(nFlags, point);
	}
}

void ResultTabControl::addNewTab()
{
	CRect Rect;
	GetClientRect(Rect);
	int newInsertPosition = GetVisibleTabsNum() - 1;

	std::shared_ptr<ResultTableListCtrl> newTableListControl = std::make_shared<ResultTableListCtrl>();
	newTableListControl->setIPDoc(m_pIPDoc);
	
	newTableListControl->Create(WS_CHILD | WS_VISIBLE | WS_BORDER, Rect, this, getNextViewIndex());
	InsertTab(newTableListControl.get(), std::format("{} {}", SvDef::cTableResultView, newInsertPosition).c_str(), newInsertPosition);

	m_additionalTabs.push_back(newTableListControl);
	SetActiveTab(newInsertPosition);
	m_pIPDoc->OnResultsTablePicker();

	SVToolSet* toolset = m_pIPDoc->GetToolSet();
	std::string toolname = SvCmd::getDottedName(m_pIPDoc->GetInspectionID(), toolset->GetResultList()->getTableId());
	toolname = toolname.substr(0, toolname.find_first_of('.'));

	if (toolname.empty() == false)
	{
		SetTabLabel(newInsertPosition, toolname.c_str());
	}
	SVSVIMStateClass::AddState(SV_STATE_MODIFIED);
}

void ResultTabControl::addNewTab(const CString tabName, const uint32_t tableId)
{
	CRect Rect;
	GetClientRect(Rect);
	int newInsertPosition = GetVisibleTabsNum() - 1;

	std::shared_ptr<ResultTableListCtrl> newTableListControl = std::make_shared<ResultTableListCtrl>();
	newTableListControl->setIPDoc(m_pIPDoc);
	newTableListControl->setTableIdForTab(tableId);
	newTableListControl->Create(WS_CHILD | WS_VISIBLE | WS_BORDER, Rect, this, getNextViewIndex());
	InsertTab(newTableListControl.get(), tabName, newInsertPosition);
	
	if (tabName == SvDef::cTableResultView)
	{
		m_pDefaultResultTableList = newTableListControl;
	}
	else
	{
		m_additionalTabs.push_back(newTableListControl);
	}

	SetActiveTab(newInsertPosition);
}

void ResultTabControl::addDefaultTabs()
{
	CRect Rect;
	GetClientRect(Rect);

	m_defaultResultList.setIPDoc(m_pIPDoc);
	m_defaultResultList.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, Rect, this, getNextViewIndex());
	InsertTab(&m_defaultResultList, SvDef::cResultView, cResultListDefaultPosition);

	std::vector<std::string> tabNames = getAllTabNames();
	if (std::find(tabNames.begin(), tabNames.end(), SvDef::cTableResultView) == tabNames.end())
	{
		m_pDefaultResultTableList = std::make_shared<ResultTableListCtrl>();
		m_pDefaultResultTableList->setIPDoc(m_pIPDoc);
		SVToolSet* toolset = m_pIPDoc->GetToolSet();
		m_pDefaultResultTableList->setTableIdForTab(toolset->GetResultList()->getTableId());
		m_pDefaultResultTableList->Create(WS_CHILD | WS_VISIBLE | WS_BORDER, Rect, this, getNextViewIndex());
		InsertTab(m_pDefaultResultTableList.get(), SvDef::cTableResultView, cResultTableDefaultposition);
	}

	AutoDestroyWindow(true);
	m_isInitialized = true;
	SetActiveTab(cResultListDefaultPosition);
	UpdateWindow();
}

const bool ResultTabControl::isDefaultTab(const int tabIndex)
{
	bool isDefault = false;
	CWnd* view = GetTabWndNoWrapper(tabIndex);

	if (view == &m_defaultResultList || view == m_pDefaultResultTableList.get() || view == &m_EditViewForNewTab)
	{
		EnableActiveTabCloseButton(false);
		isDefault = true;
	}
	return isDefault;
}

const std::vector<std::string> ResultTabControl::getAllTabNames()
{
	std::vector<std::string> tabNames;
	CString tabLabel;

	for (int i = 0; i < GetVisibleTabsNum(); i++)
	{
		GetTabLabel(i, tabLabel);
		tabNames.push_back(std::string(tabLabel));
	}

	return tabNames;
}
#pragma endregion Private Methods
