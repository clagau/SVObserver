//******************************************************************************
//* Copyright (c) 1998-2005
//* All Rights Reserved
//* Author           : Chris Maunder <chris@codeproject.com>
//* License          : The Code Project Open License (CPOL)
//* From             : http://www.codeproject.com/Articles/8/MFC-Grid-control
//******************************************************************************
//* .Module Name     : GridCell
//* .File Name       : $Workfile:   GridCell.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Oct 2014 10:32:30  $
//******************************************************************************

/////////////////////////////////////////////////////////////////////////////
// GridCell.h : header file
//
// MFC Grid Control - Grid cell class header file
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
// For use with GridCtrl v2.20+
//
//////////////////////////////////////////////////////////////////////

#pragma once

#pragma region Includes
#include "GridCellBase.h"
#pragma endregion Includes

namespace SvGcl
{
	class GridCtrl;

	class GridCell : public GridCellBase
	{
		friend class GridCtrl;
		DECLARE_DYNCREATE(GridCell)

	// Construction/Destruction
	public:
		GridCell();
		virtual ~GridCell();

	// Attributes
	public:
		GridCell& operator=(const GridCell& cell);

		virtual void  SetText(LPCTSTR szText) override        { m_strText = szText;  }                       
		virtual void  SetImage(int nImage) override           { m_nImage = nImage;   }                        
		virtual void  SetData(LPARAM lParam) override         { m_lParam = lParam;   }      
		virtual void  SetGrid(GridCtrl* pGrid) override      { m_pGrid = pGrid;     }                          
		// virtual void SetState(const DWORD nState);  -  use base class version   
		virtual void  SetFormat(DWORD nFormat) override       { m_nFormat = nFormat; }                      
		virtual void  SetTextClr(COLORREF clr) override       { m_crFgClr = clr;     }                          
		virtual void  SetBackClr(COLORREF clr) override       { m_crBkClr = clr;     }                          
		virtual void  SetFont(const LOGFONT* plf) override;
		virtual void  SetMargin(UINT nMargin) override        { m_nMargin = nMargin; }
		virtual CWnd* GetEditWnd() const override             { return m_pEditWnd;   }
		virtual void  SetCoords(int /*nRow*/, int /*nCol*/) override {}  // don't need to know the row and
																// column for base implementation

		virtual LPCTSTR     GetText() const override             { return (m_strText.IsEmpty())? _T("") : LPCTSTR(m_strText); }
		virtual int         GetImage() const override            { return m_nImage;  }
		virtual LPARAM      GetData() const override             { return m_lParam;  }
		virtual GridCtrl*  GetGrid() const override	            { return m_pGrid;   }
		// virtual DWORD    GetState() const - use base class
		virtual DWORD       GetFormat() const override;
		virtual COLORREF    GetTextClr() const override          { return m_crFgClr; } // TODO: change to use default cell
		virtual COLORREF    GetBackClr() const override          { return m_crBkClr; }
		virtual LOGFONT*    GetFont() const override;
		virtual CFont*      GetFontObject() const override;
		virtual UINT        GetMargin() const override;

		virtual BOOL        IsEditing() const override           { return m_bEditing; }
		virtual BOOL        IsDefaultFont() const override       { return (nullptr == m_plfFont); }
		virtual void        Reset() override;

	// editing cells
	public:
		virtual BOOL Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar) override;
		virtual void EndEdit() override;
	protected:
		virtual void OnEndEdit() override;

	protected:
		CString    m_strText;      // Cell text (or binary data if you wish...)
		LPARAM     m_lParam;       // 32-bit value to associate with item
		int        m_nImage;       // Index of the list view item’s icon
		DWORD      m_nFormat;
		COLORREF   m_crFgClr;
		COLORREF   m_crBkClr;
		LOGFONT*   m_plfFont;
		UINT       m_nMargin;

		BOOL       m_bEditing;     // Cell being edited?

		GridCtrl* m_pGrid;        // Parent grid control
		CWnd*      m_pEditWnd;
	};

	// This class is for storing grid default values. It's a little heavy weight, so
	// don't use it in bulk 
	class CGridDefaultCell : public GridCell
	{
		DECLARE_DYNCREATE(CGridDefaultCell)

	// Construction/Destruction
	public:
		CGridDefaultCell();
		virtual ~CGridDefaultCell();

	public:
		virtual DWORD GetStyle() const                      { return m_dwStyle;      }
		virtual void  SetStyle(DWORD dwStyle)               { m_dwStyle = dwStyle;   }
		virtual int   GetWidth() const                      { return m_Size.cx;      }
		virtual int   GetHeight() const                     { return m_Size.cy;      }
		virtual void  SetWidth(int nWidth)                  { m_Size.cx = nWidth;    }
		virtual void  SetHeight(int nHeight)                { m_Size.cy = nHeight;   }

		// Disable these properties
		virtual void     SetData(LPARAM /*lParam*/) override { ASSERT(FALSE);         }      
		virtual void     SetState(DWORD /*nState*/) override { ASSERT(FALSE);         }
		virtual DWORD    GetState() const override { return GridCell::GetState()|GVIS_READONLY; }
		virtual void     SetCoords( int /*row*/, int /*col*/) override { ASSERT(FALSE);         }
		virtual void     SetFont(const LOGFONT* /*plf*/) override;
		virtual LOGFONT* GetFont() const override;
		virtual CFont*   GetFontObject() const override;

	protected:
		CSize m_Size {30, 10};       // Default Size
		CFont m_Font;       // Cached font
		DWORD m_dwStyle {0};    // Cell Style - unused
	};

} // namespace SvGcl



