//******************************************************************************
//* COPYRIGHT (c) 2014 by Seideander Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : MonitorListView
//* .File Name       : $Workfile:   MonitorListView.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Apr 2014 16:25:20  $
//******************************************************************************

#pragma once

#pragma region Includes
#include <map>
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

class SVIODoc;

enum MonitorListViewNodeType
{
	MonitorListNameNode = 1,
	ProductItemListNode = 2,
	ProductItemValuesNode = 3,
	ProductItemValuesItemNode = 4,
	ProductItemImagesNode = 5,
	ProductItemImagesItemNode = 6,
	RejectConditionListNode = 7,
	RejectConditionValuesNode = 8,
	RejectConditionValuesItemNode = 9,
	FailStatusListNode = 10,
	FailStatusValuesNode = 11,
	FailStatusValuesItemNode = 12
};

class MonitorListView : public CListView
{
	DECLARE_DYNCREATE(MonitorListView)

#pragma region Protected
protected:
	struct ExpandCollapseState
	{
		unsigned short bListCollapsed : 1;
		unsigned short bProductCollapsed : 1;
		unsigned short bProductValuesCollapsed : 1;
		unsigned short bProductImagesCollapsed : 1;
		unsigned short bRejectCollapsed : 1;
		unsigned short bRejectValuesCollapsed : 1;
		unsigned short bFailStatusCollapsed : 1;
		unsigned short bFailStatusValuesCollapsed : 1;
		
		ExpandCollapseState()
		: bListCollapsed(0)
		, bProductCollapsed(0)
		, bProductValuesCollapsed(0)
		, bProductImagesCollapsed(0)
		, bRejectCollapsed(0)
		, bRejectValuesCollapsed(0)
		, bFailStatusCollapsed(0)
		, bFailStatusValuesCollapsed(0)
		{
		}
	};

	typedef std::map<SVString, ExpandCollapseState> CollapseExpandMap;

	MonitorListView();           // protected constructor used by dynamic creation
	virtual ~MonitorListView();
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	
	bool EditMonitoredItem(int item);
	bool RemoveMonitoredItem(int item);

	int GetExpandCollapseState(MonitorListViewNodeType nodeType, const ExpandCollapseState& stateHolder);
	bool HandleExpandCollapse(MonitorListViewNodeType nodeType, ExpandCollapseState& stateHolder, int state);
	void CollapseItem(int item);
	void ExpandItem(int item);
	void ToggleExpandCollapse(int item);

	CImageList ImageList;
	CImageList StateImageList;
	CollapseExpandMap m_ExpandCollapseStates;
#pragma endregion Protected

#pragma region Public
public:
	CMenu m_ContextMenuNamedList;
	CMenu m_ContextMenuSubList;
	CMenu m_ContextMenuNode;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	SVIODoc* m_pDocument;
	SVIODoc* GetDocument();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnShowProperties();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	void AddItem();
	afx_msg void OnDeleteItem();
	afx_msg void OnEditItem();
	afx_msg void OnAddRemoveList();
	afx_msg void OnEditListProperties();

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

#pragma endregion Public
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\MonitorListView.h_v  $
 * 
 *    Rev 1.0   17 Apr 2014 16:25:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

