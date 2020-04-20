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

protected:
	typedef std::map<std::string, bool> SVGroupStateMap;

	SVRemoteOutputsView();           // protected constructor used by dynamic creation
	virtual ~SVRemoteOutputsView();
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/) override;
	HRESULT RemoteOutputGroupNameAtItem( std::string& rGroupName, int p_iItem );
	bool EditOutput(int p_iWhere);
	bool AddOutput(int p_iWhere);

	CImageList ImageList;
	CImageList StateImageList;
	SVGroupStateMap m_GroupStates;

public:
	CMenu m_ContextMenuProp;
	CMenu m_ContextMenuItem;
	CMenu m_ContextMenuItemNoDelete;
	int m_CurrentItem;

#ifdef _DEBUG
	virtual void AssertValid() const override;
	virtual void Dump(CDumpContext& dc) const override;
#endif

	SVIODoc* m_pDocument;
	SVIODoc* GetDocument();

	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRemoteOutputProperties();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRemoteOutputAdd();
	afx_msg void OnRemoteOutputDelete();
	afx_msg void OnRemoteOutputEdit();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnAddTransferBlock();

};



