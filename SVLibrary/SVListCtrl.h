// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVListCtrl
// * .File Name       : $Workfile:   SVListCtrl.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   01 Oct 2013 10:02:44  $
// ******************************************************************************

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_SVLISTCTRL_38D91BE601A5_INCLUDED
#define _INC_SVLISTCTRL_38D91BE601A5_INCLUDED


class SVListCtrl : public CListCtrl
{
public:
	SVListCtrl();

	virtual ~SVListCtrl();
	//override this to set the color for current cell
	virtual COLORREF GetCellRGB(void);
	//called before from within OnlButtonDown and OnDblclk, but before anything happens, return TRUE to continue, FALSE to say not selecting the item
	virtual BOOL OnItemLButtonDown(LVHITTESTINFO& ht);
	//override this to activate any control when lButtonDown in a cell, called from within OnLButtonDown
	virtual void OnControlLButtonDown(UINT nFlags, CPoint point, LVHITTESTINFO& ht);

	//{{AFX_VIRTUAL(SVListCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(SVListCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);	
	afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM);
	DECLARE_MESSAGE_MAP()

protected:
	//translates column index value to a column order value.
	int IndexToOrder(int iIndex);

	// Editing
	virtual void  OnEditItem(int iItem, int iSubItem, CPoint point, UINT nChar);
	virtual void  OnEndEditItem(LPLVITEM plvItem);

private:
	void RepaintSelectedItems();
	void MakeColumnVisible(int nCol);
	void DrawFocusCell(CDC *pDC, int nItem, int iSubItem);
	BOOL IsReadOnly(int nItem, int nSubItem=0);

private:
	int		m_CurItem;
	int		m_CurSubItem;
	int		m_nDragItem, m_nDragTarget;
	BOOL	m_bIsDragging;
	BOOL	m_bHasFocus;
};

#endif /* _INC_SVLISTCTRL_38D91BE601A5_INCLUDED */

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVListCtrl.h_v  $
 * 
 *    Rev 1.1   01 Oct 2013 10:02:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 13:37:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Mar 2001 13:18:38   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  116
 * SCR Title:  Enhancements to SVListCtrl class and SVLibrary.Lib
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Bug Fix: Modified SVListCtrl to recover the original item value if the escape key is pressed. In the past pressing the escape key while editing would return a blank string.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   05 Apr 2000 11:53:32   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  4
 * SCR Title:  Baseline Development: SVIM Communications
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Updated ModelIds
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   04 Apr 2000 17:10:08   Joe
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  1
 * SCR Title:  Baseline Development: Run Mode
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to add required functionality to SVFocus and other projects associated with SVocus.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Mar 22 2000 17:28:16   mike
 * Initial revision.
*/