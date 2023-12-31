//******************************************************************************
//* Copyright (c) 1998-2005
//* All Rights Reserved
//* Author           : Chris Maunder <chris@codeproject.com>
//* License          : The Code Project Open License (CPOL)
//* From             : http://www.codeproject.com/Articles/8/MFC-Grid-control
//******************************************************************************
//* .Module Name     : InPlaceEdit
//* .File Name       : $Workfile:   InPlaceEdit.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Oct 2014 10:32:34  $
//******************************************************************************

//////////////////////////////////////////////////////////////////////
// InPlaceEdit.h : header file
//
// MFC Grid Control - inplace editing class
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
// For use with GridCtrl v2.10+
//
//////////////////////////////////////////////////////////////////////

#pragma once

namespace SvGcl
{
	class InPlaceEdit : public CEdit
	{
	// Construction
	public:
		InPlaceEdit(CWnd* pParent, CRect& rect, DWORD dwStyle, UINT nID,
						int nRow, int nColumn, CString sInitText, UINT nFirstChar);

	// Attributes
	public:
 
	// Operations
	public:
			void EndEdit();
 
	// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(InPlaceEdit)
		public:
		virtual BOOL PreTranslateMessage(MSG* pMsg) override;
		protected:
		virtual void PostNcDestroy() override;
		//}}AFX_VIRTUAL
 
	// Implementation
	public:
			virtual ~InPlaceEdit();
 
	// Generated message map functions
	protected:
		//{{AFX_MSG(InPlaceEdit)
		afx_msg void OnKillFocus(CWnd* pNewWnd);
		afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg UINT OnGetDlgCode();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:
		int     m_nRow;
		int     m_nColumn;
		CString m_sInitText;
		UINT    m_nLastChar;
		BOOL    m_bExitOnArrows;
		CRect   m_Rect;
	};

} // namespace SvGcl


