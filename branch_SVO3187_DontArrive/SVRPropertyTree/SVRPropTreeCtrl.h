// ******************************************************************************
// * COPYRIGHT (c) 2000 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : PropertyTreeCtrl.h
// * .File Name       : $Workfile:   PropTreeCtrl.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 16:42:02  $
// ******************************************************************************

// PropertyTreeCtrl.h : header file
//
//
//  Copyright (C) 1998-2001 Scott Ramsay
//	sramsay@gonavi.com
//	http://www.gonavi.com
//
//  This material is provided "as is", with absolutely no warranty expressed
//  or implied. Any use is at your own risk.
// 
//  Permission to use or copy this software for any purpose is hereby granted 
//  without fee, provided the above notices are retained on all copies.
//  Permission to modify the code and to distribute modified code is granted,
//  provided the above notices are retained, and a notice that the code was
//  modified is included with the above copyright notice.
// 
//	If you use this code, drop me an email.  I'd like to know if you find the code
//	useful.

#pragma once

class SVRPropTree;
class SVRPropertyItem;

/////////////////////////////////////////////////////////////////////////////
// SVRPropTreeCtrl window

class SVRPropTreeCtrl : public CWnd
{
// Construction
public:
	SVRPropTreeCtrl();
	virtual ~SVRPropTreeCtrl();

	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Operations
	void SetPropOwner(SVRPropTree* pProp);

	void UpdateResize();

	void UpdatePositions();
	void ClearLastReadonlyActivated();
protected:
	void RecreateBackBuffer(int cx, int cy);
	void CheckVisibleFocus();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVRPropTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:

// Attributes
protected:
	// SVRPropTree class that this class belongs
	SVRPropTree*    m_pProp;

	// bitmap back buffer for flicker free drawing
	CBitmap         m_BackBuffer;

	// current diminsions of the back buffer
	CSize           m_BackBufferSize;

	// splitter pevious position
	LONG            m_nPrevCol;

	// TRUE if we are dragging the splitter
	BOOL            m_bColDrag;

	SVRPropertyItem* m_pLastReadonlyActivated = nullptr;

	// Generated message map functions
protected:
	//{{AFX_MSG(SVRPropTreeCtrl)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG
public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	DECLARE_MESSAGE_MAP()
};

