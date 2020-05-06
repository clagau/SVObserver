//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteOutputsView
//* .File Name       : $Workfile:   SVRemoteOutputsView.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   23 Jul 2013 18:17:46  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <vector>


// SVRemoteOutputsView view
class SVIODoc;

class SVRemoteOutputsView : public CListView
{
	DECLARE_DYNCREATE(SVRemoteOutputsView)
	DECLARE_MESSAGE_MAP()

	SVRemoteOutputsView();
	virtual ~SVRemoteOutputsView();

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/) override;
	HRESULT RemoteOutputGroupNameAtItem( std::string& rGroupName, int p_iItem );
	bool EditOutput(int p_iWhere);
	bool AddOutput(int p_iWhere);
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRemoteOutputProperties();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRemoteOutputAdd();
	afx_msg void OnRemoteOutputDelete();
	afx_msg void OnRemoteOutputEdit();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

private:
	CListCtrl& m_rCtrl;
	SVIODoc* m_pDocument;
	CImageList m_ImageList;
	CImageList m_StateImageList;
	std::map<std::string, bool> m_GroupStates;
	std::vector<GUID> m_aPPQGUIDs;
	CMenu m_ContextMenuProp;
	CMenu m_ContextMenuItem;
	CMenu m_ContextMenuItemNoDelete;
	int m_CurrentItem{-1};
};



