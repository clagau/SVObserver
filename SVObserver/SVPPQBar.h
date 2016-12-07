//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPPQBar
//* .File Name       : $Workfile:   SVPPQBar.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:30:48  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVMFCControls\SVWindowBar.h"
#pragma endregion Includes

class SVPPQObject;

class SVButtonList : public CArray< CButton*, CButton* >
{
public:
	virtual ~SVButtonList()
	{
		RemoveAll();
		FreeExtra();
	}
};


class SVPPQWindowClass : public CWnd
{
protected:
	//{{AFX_MSG(SVPPQWindowClass)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	SVPPQWindowClass();

	virtual ~SVPPQWindowClass();

	BOOL BuildButtons( SVPPQObject* PSelectedPPQ );

	BOOL DestroyButtons();

	//{{AFX_VIRTUAL(SVPPQWindowClass)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = nullptr) override;
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
	//}}AFX_VIRTUAL

protected:
	SVButtonList buttonList;
	SVPPQObject* m_pPPQ;
};

class SVPPQBarClass : public SvMc::SVWindowBarClass
{
	DECLARE_DYNCREATE( SVPPQBarClass )

protected:
	//{{AFX_MSG(SVPPQBarClass)
	afx_msg void OnSelChangePQTab( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnSizeParent(WPARAM, LPARAM lParam);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	SVPPQBarClass();

	virtual ~SVPPQBarClass();

	BOOL Create( CWnd* pParentWnd, DWORD dwStyle = WS_CHILD | WS_VISIBLE, UINT nID = AFX_IDW_CONTROLBAR_FIRST + 16 );
	BOOL BuildButtons();

	BOOL DestroyButtons();
	
	CSize CalcFixedLayout( BOOL bStretch, BOOL bHorz );

	//{{AFX_VIRTUAL(SVPPQBarClass)
	//}}AFX_VIRTUAL

protected:
	SVPPQWindowClass PPQWindow;

	CTabCtrl	pqTabCtrl;
};

