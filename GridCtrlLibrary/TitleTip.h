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
// For use with CGridCtrl v2.10+
//
//////////////////////////////////////////////////////////////////////

#pragma once


namespace Seidenader
{
	namespace GridCtrlLibrary
	{
		#define TITLETIP_CLASSNAME _T("ZTitleTip")

		class CTitleTip : public CWnd
		{
		// Construction
		public:
			CTitleTip();
			virtual ~CTitleTip();
			virtual BOOL Create( CWnd *pParentWnd);

		// Attributes
		public:
			void SetParentWnd(CWnd* pParentWnd)  { m_pParentWnd = pParentWnd; }
			CWnd* GetParentWnd()                 { return m_pParentWnd;       }

		// Operations
		public:
			void Show(CRect rectTitle, LPCTSTR lpszTitleText, 
					  int xoffset = 0, LPRECT lpHoverRect = NULL, 
					  const LOGFONT* lpLogFont = NULL,
					  COLORREF crTextClr = CLR_DEFAULT, COLORREF crBackClr = CLR_DEFAULT);
			void Hide();

		// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CTitleTip)
			public:
			virtual BOOL PreTranslateMessage(MSG* pMsg);
			virtual BOOL DestroyWindow();
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
			//{{AFX_MSG(CTitleTip)
			afx_msg void OnMouseMove(UINT nFlags, CPoint point);
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()
		};
	} //namespace GridCtrlLibrary
} //namespace Seidenader

namespace SvGcl = Seidenader::GridCtrlLibrary;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\GridCtrlLibrary\TitleTip.h_v  $
 * 
 *    Rev 1.0   24 Oct 2014 10:32:28   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  942
 * SCR Title:  Create new Custom2 Filter SVO-324 SVO-67 SVO-74
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */