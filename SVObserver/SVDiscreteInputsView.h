//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDiscreteInputsView
//* .File Name       : $Workfile:   SVDiscreteInputsView.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:02:16  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOMFCLibrary/SVDataItemManagerTemplate.h"
#include "SVIOEntryHostStruct.h"
#pragma endregion Includes

class SVIODoc;

class SVDiscreteInputsView : public CListView
{
	DECLARE_DYNCREATE( SVDiscreteInputsView )

	//{{AFX_MSG(SVDiscreteInputsView)
protected:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(SVDiscreteInputsView)
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = nullptr) override;
protected:
	virtual void OnInitialUpdate() override; // das erste mal nach der Konstruktion aufgerufen
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) override;
	//}}AFX_VIRTUAL

#ifdef _DEBUG
public:
	virtual void AssertValid() const override;
	virtual void Dump(CDumpContext& dc) const override;
#endif

public:
	virtual ~SVDiscreteInputsView();

	SVIODoc* GetDocument();

protected:
	SVDiscreteInputsView();

	CImageList ImageList;
	CImageList StateImageList;

private:
	typedef SVDataItemManagerTemplate< SVIOEntryHostStructPtr > SVDataItemManager;

	SVDataItemManager m_Items;
};

#ifndef _DEBUG  // Testversion in SVDiscreteInputsView.cpp
inline SVIODoc* SVDiscreteInputsView::GetDocument()
   { return (SVIODoc*)m_pDocument; }
#endif

