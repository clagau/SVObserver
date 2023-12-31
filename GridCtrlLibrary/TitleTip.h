//******************************************************************************
//* Copyright (c) 1998-2005
//* All Rights Reserved
//* Author           : Chris Maunder <chris@codeproject.com>, Zafir Anjum
//* License          : The Code Project Open License (CPOL)
//* From             : http://www.codeproject.com/Articles/8/MFC-Grid-control
//******************************************************************************
//* .Module Name     : Titletip
//* .File Name       : $Workfile:   TitleTip.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Oct 2014 10:32:28  $
//******************************************************************************

/////////////////////////////////////////////////////////////////////////////
// Titletip.h : header file
//
// MFC Grid Control - cell titletips
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
	#define TITLETIP_CLASSNAME _T("ZTitleTip")

	class TitleTip : public CWnd
	{
	// Construction
	public:
		TitleTip();
		virtual ~TitleTip();
		virtual BOOL Create( CWnd *pParentWnd) override;

	// Attributes
	public:
		void SetParentWnd(CWnd* pParentWnd)  { m_pParentWnd = pParentWnd; }
		CWnd* GetParentWnd()                 { return m_pParentWnd;       }

	// Operations
	public:
		void Show(CRect rectTitle, LPCTSTR lpszTitleText, 
					int xoffset = 0, LPRECT lpHoverRect = nullptr, 
					const LOGFONT* lpLogFont = nullptr,
					COLORREF crTextClr = CLR_DEFAULT, COLORREF crBackClr = CLR_DEFAULT);
		void Hide();

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(TitleTip)
		public:
		virtual BOOL PreTranslateMessage(MSG* pMsg) override;
		virtual BOOL DestroyWindow() override;
		//}}AFX_VIRTUAL

	// Implementation
	protected:
		CWnd  *m_pParentWnd;
		CRect  m_rectTitle;
		CRect  m_rectHover;
		DWORD  m_dwLastLButtonDown;
		DWORD  m_dwDblClickMsecs;
		BOOL   m_bCreated;

		// Generated message map functions
	protected:
		//{{AFX_MSG(TitleTip)
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	};

} // namespace SvGcl


