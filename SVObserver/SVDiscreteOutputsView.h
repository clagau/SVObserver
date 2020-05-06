//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDiscreteOutputsView
//* .File Name       : $Workfile:   SVDiscreteOutputsView.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:02:22  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVIOLibrary/SVIOEntryHostStruct.h"
#pragma endregion Includes

class SVIODoc;

class SVDiscreteOutputsView : public CListView
{
	DECLARE_DYNCREATE( SVDiscreteOutputsView )
	DECLARE_MESSAGE_MAP()

	SVDiscreteOutputsView();
	virtual ~SVDiscreteOutputsView() = default;

protected:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = nullptr) override;
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) override;

	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
private:
	bool setListItem(int rowIndex, SVIOEntryHostStructPtr pIOEntry);
	CListCtrl& m_rCtrl;
	SVIODoc* m_pDocument{nullptr};
	CImageList m_ImageList;
	CImageList m_StateImageList;
	std::map<unsigned long, SVIOEntryHostStructPtr> m_Items;
};
