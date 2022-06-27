//******************************************************************************
//* Copyright (c) 1998-2005
//* All Rights Reserved
//* Author           : Chris Maunder <chris@codeproject.com>
//* License          : The Code Project Open License (CPOL)
//* From             : http://www.codeproject.com/Articles/8/MFC-Grid-control
//******************************************************************************
//* .Module Name     : GridCellCombo
//* .File Name       : $Workfile:   GridCellCombo.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Oct 2014 10:32:32  $
//******************************************************************************

/////////////////////////////////////////////////////////////////////////////
// GridCellCombo.h : header file
//
// MFC Grid Control - Grid combo cell class header file
//
// Written by Chris Maunder <chris@codeproject.com>
// Copyright (c) 1998-2005. All Rights Reserved.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name and all copyright 
// notices remains intact. 
//
// An email letting me know how you are using it would be nice as well. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// For use with GridCtrl v2.10
//
//////////////////////////////////////////////////////////////////////

#pragma once

#pragma region Includes
#include "GridCell.h"
#pragma endregion Includes

namespace SvGcl
{
	class GridCellCombo : public GridCell
	{
		friend class GridCtrl;
		DECLARE_DYNCREATE(GridCellCombo)

	public:
		GridCellCombo();

	// editing cells
	public:
		bool setTextIfInList(LPCTSTR szText);
		virtual BOOL  Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar) override;
		virtual void  EndEdit() override;

	// Operations
	public:
		virtual CSize GetCellExtent(CDC* pDC) override;

	// GridCellCombo specific calls
	public:
		void  SetOptions(const CStringArray& ar);
		void  SetStyle(DWORD dwStyle)           { m_dwStyle = dwStyle; }
		DWORD GetStyle()                        { return m_dwStyle;    }

	protected:
		virtual BOOL Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE) override;

		CStringArray m_Strings;
		DWORD        m_dwStyle;
	};

	/////////////////////////////////////////////////////////////////////////////
	// ComboEdit window

	#define IDC_COMBOEDIT 1001

	class ComboEdit : public CEdit
	{
		// Construction
	public:
		ComboEdit();

		// Attributes
	public:

		// Operations
	public:

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(ComboEdit)
		virtual BOOL PreTranslateMessage(MSG* pMsg) override;
		//}}AFX_VIRTUAL

		// Implementation
	public:
		virtual ~ComboEdit();

		// Generated message map functions
	protected:
		//{{AFX_MSG(ComboEdit)
		afx_msg void OnKillFocus(CWnd* pNewWnd);
		afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()
	};

	/////////////////////////////////////////////////////////////////////////////
	// InPlaceList window

	class InPlaceList : public CComboBox
	{
		friend class ComboEdit;

		// Construction
	public:
		InPlaceList(CWnd* pParent,         // parent
			CRect& rect,           // dimensions & location
			DWORD dwStyle,         // window/combobox style
			UINT nID,              // control ID
			int nRow, int nColumn, // row and column
			COLORREF crFore, COLORREF crBack,  // Foreground, background colour
			CStringArray& Items,   // Items in list
			CString sInitText,     // initial selection
			UINT nFirstChar);      // first character to pass to control

		// Attributes
	public:
		ComboEdit m_edit;  // subclassed edit control

		// Operations
	public:

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(InPlaceList)
	protected:
		virtual void PostNcDestroy() override;
		//}}AFX_VIRTUAL

		// Implementation
	public:
		virtual ~InPlaceList();
		void EndEdit();

	protected:
		int GetCorrectDropWidth();

		// Generated message map functions
	protected:
		//{{AFX_MSG(InPlaceList)
		afx_msg void OnKillFocus(CWnd* pNewWnd);
		afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg void OnDropdown();
		afx_msg void OnSelChange();
		afx_msg UINT OnGetDlgCode();
		afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
		//}}AFX_MSG
		//afx_msg void OnSelendOK();

		DECLARE_MESSAGE_MAP()

	private:
		int		 m_nNumLines;
		CString  m_sInitText;
		int		 m_nRow;
		int		 m_nCol;
		UINT     m_nLastChar; 
		BOOL	 m_bExitOnArrows; 
		COLORREF m_crForeClr, m_crBackClr;
	};

} // namespace SvGcl


