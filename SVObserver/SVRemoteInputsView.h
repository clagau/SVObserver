//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteInputsView
//* .File Name       : $Workfile:   SVRemoteInputsView.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:31:24  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOMFCLibrary/SVDataItemManagerTemplate.h"
#include "SVIOEntryHostStruct.h"
#pragma endregion Includes

class SVIODoc;

class SVRemoteInputsView : public CListView
{
	DECLARE_DYNCREATE(SVRemoteInputsView)

protected:
	SVRemoteInputsView();           // protected constructor used by dynamic creation
	virtual ~SVRemoteInputsView();

protected:
	CImageList ImageList;
	CImageList StateImageList;

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	SVIODoc* GetDocument();

protected:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()

private:
	typedef SVDataItemManagerTemplate< SVIOEntryHostStructPtr > SVDataItemManager;

	SVDataItemManager m_Items;
};

#ifndef _DEBUG  // Testversion in SVRemoteInputsView.cpp
inline SVIODoc* SVRemoteInputsView::GetDocument()
   { return (SVIODoc*)m_pDocument; }
#endif
