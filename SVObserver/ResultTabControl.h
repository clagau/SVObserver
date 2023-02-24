//*****************************************************************************
/// \copyright (c) 2023,2023 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file ResultTabControl.h
/// All Rights Reserved 
//*****************************************************************************
/// A class that handle the creation off new ResultView tabs, closing and renaming them
//******************************************************************************
#pragma once

#pragma region Includes
#include <afxtabctrl.h>
#include "ResultTableListCtrl.h"
#include "ResultListCtrl.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#pragma endregion Inlcudes

namespace SvOi
{
class IObjectWriter;
}

class ResultTabControl : public CMFCTabCtrl
{
	DECLARE_DYNCREATE(ResultTabControl)
	DECLARE_MESSAGE_MAP()

public:
	ResultTabControl() = default;
	virtual ~ResultTabControl() = default;
	virtual BOOL Create(Style style, const RECT& rect, CWnd* pParentWnd, UINT nID, Location location = LOCATION_BOTTOM, BOOL bCloseBtn = FALSE);

	typedef SvXml::SVXMLMaterialsTree SVTreeType;
	void Save(SvOi::IObjectWriter& rWriter);
	bool Load(SVTreeType& rTree, SVTreeType::SVBranchHandle hParent);
	
	void updateTab();
	void tableIdChanged();
	void initializeResultTabControl() { addDefaultTabs(); };

private:
	virtual void OnDraw(CDC* pDC) override;
	virtual BOOL StartRenameTab(int iTab) override;
	virtual BOOL RenameTab() override;
	virtual BOOL SetActiveTab(int iTab) override;
	virtual void OnLButtonUp(UINT nFlags, CPoint point);

	void addNewTab();
	void addNewTab(const CString tabName, const uint32_t tableId);
	void addDefaultTabs();
	
	const bool isDefaultTab(const int tabIndex);
	const std::vector<std::string> getAllTabNames();
	int getNextViewIndex() { return ++viewIndex; };
	

#pragma region Member Variables
	ResultListCtrl m_defaultResultList;
	std::shared_ptr<ResultTableListCtrl> m_pDefaultResultTableList;
	CEdit m_EditViewForNewTab;
	std::vector<std::shared_ptr<CListCtrl>> m_additionalTabs;
	SVIPDoc* m_pIPDoc = nullptr;
	int viewIndex {0};
	bool m_isInitialized {false};
#pragma endregion Member Variables
};

