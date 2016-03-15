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

#pragma once

namespace Seidenader
{
	namespace SVMFCControls
	{
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
	} //SVMFCControls
} //Seidenader

namespace SvMc = Seidenader::SVMFCControls;

