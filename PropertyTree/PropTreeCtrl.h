// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
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

#if !defined(AFX_PROPERTYTREECTRL_H__2E09E831_09F5_44AA_B41D_9C4BF495873C__INCLUDED_)
#define AFX_PROPERTYTREECtrl_H__2E09E831_09F5_44AA_B41D_9C4BF495873C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SVRPropTree;

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

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYTREECTRL_H__2E09E831_09F5_44AA_B41D_9C4BF495873C__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\PropertyTree\PropTreeCtrl.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 16:42:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Feb 2005 08:16:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  473
 * SCR Title:  Fix Property Tree bugs
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   rearranged class definition
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   23 Jul 2004 07:53:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  435
 * SCR Title:  Add Bulb Mode Operation to the Digital SVIM for Point Gray Dragonfly Cameras
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a new method UpdatePositions to caculate the new item positions so that the sub-controls get placed in the correct places.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 15:42:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of the file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/