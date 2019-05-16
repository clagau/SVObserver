//******************************************************************************
//* Copyright (c) 1998-2005
//* All Rights Reserved
//* Author           : Chris Maunder <chris@codeproject.com> Podsypalnikov
//* License          : The Code Project Open License (CPOL)
//* From             : http://www.codeproject.com/Articles/8/MFC-Grid-control
//******************************************************************************
//* .Module Name     : GridCelldateTime
//* .File Name       : $Workfile:   GridCellDateTime.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Oct 2014 10:32:32  $
//******************************************************************************

// GridCellDateTime.h: interface for the CGridCellDateTime class.
//
// Provides the implementation for a datetime picker cell type of the
// grid control.
//
// For use with CGridCtrl v2.22+
//
//////////////////////////////////////////////////////////////////////

#pragma once

#pragma region Includes
#include "GridCell.h"
#pragma endregion Includes

namespace SvGcl
{
	class CGridCellDateTime : public CGridCell  
	{
		friend class CGridCtrl;
		DECLARE_DYNCREATE(CGridCellDateTime)

		CTime m_cTime;
		DWORD m_dwStyle;

	public:
		CGridCellDateTime();
		CGridCellDateTime(DWORD dwStyle);
		virtual ~CGridCellDateTime();
		virtual CSize GetCellExtent(CDC* pDC) override;

		// editing cells
	public:
		void Init(DWORD dwStyle);
		virtual BOOL  Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar) override;
		virtual CWnd* GetEditWnd() const override;
		virtual void  EndEdit() override;


		CTime* GetTime() {return &m_cTime;};
		void   SetTime(CTime time);
	};

	class CInPlaceDateTime : public CDateTimeCtrl
	{
	// Construction
	public:
		CInPlaceDateTime(CWnd* pParent,         // parent
						CRect& rect,           // dimensions & location
						DWORD dwStyle,         // window/combobox style
						UINT nID,              // control ID
						int nRow, int nColumn, // row and column
						COLORREF crFore, COLORREF crBack,  // Foreground, background colour
						CTime* pcTime,
          				UINT nFirstChar);      // first character to pass to control

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CInPlaceList)
		protected:
		virtual void PostNcDestroy() override;
		//}}AFX_VIRTUAL

	// Implementation
	public:
		virtual ~CInPlaceDateTime();
		void EndEdit();

	// Generated message map functions
	protected:
		//{{AFX_MSG(CInPlaceList)
		afx_msg void OnKillFocus(CWnd* pNewWnd);
		afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg UINT OnGetDlgCode();
		//}}AFX_MSG
		//afx_msg void OnSelendOK();

		DECLARE_MESSAGE_MAP()

	private:
		CTime*   m_pcTime;
		int		 m_nRow;
		int		 m_nCol;
 		UINT     m_nLastChar; 
		BOOL	 m_bExitOnArrows; 
		COLORREF m_crForeClr, m_crBackClr;
	};

} // namespace SvGcl


