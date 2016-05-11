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
#include "SVOMFCLibrary/SVDataItemManagerTemplate.h"
#include "SVIOEntryHostStruct.h"
#pragma endregion Includes

class SVIODoc;

class SVDiscreteOutputsView : public CListView
{
	DECLARE_DYNCREATE( SVDiscreteOutputsView )

	//{{AFX_MSG(SVDiscreteOutputsView)
	protected: 
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(SVDiscreteOutputsView)
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = nullptr);

protected:
	virtual void OnInitialUpdate(); // das erste mal nach der Konstruktion aufgerufen
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

public:
	#ifdef _DEBUG
		virtual void AssertValid() const;
		virtual void Dump(CDumpContext& dc) const;
	#endif

	virtual ~SVDiscreteOutputsView();

	SVIODoc* GetDocument();

protected:
	SVDiscreteOutputsView();

	CImageList ImageList;
	CImageList StateImageList;

private:
	typedef SVDataItemManagerTemplate< SVIOEntryHostStructPtr > SVDataItemManager;

	SVDataItemManager m_Items;

};

#ifndef _DEBUG
inline SVIODoc* SVDiscreteOutputsView::GetDocument()
   { return (SVIODoc*)m_pDocument; }
#endif
