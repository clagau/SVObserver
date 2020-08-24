//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVWindowBar
//* .File Name       : $Workfile:   SVWindowBar.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 16:31:28  $
//******************************************************************************
#pragma once

#include "SVSlideBar.h"

namespace SvMc
{
	class SVWindowBar : public CControlBar
	{
		DECLARE_DYNCREATE( SVWindowBar )

	protected:
		//{{AFX_MSG(SVWindowBar)
		afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
		afx_msg void OnPaint();
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()

	public:
		SVWindowBar();
		virtual ~SVWindowBar();
		virtual BOOL Create( CWnd* pParentWnd, DWORD dwStyle, UINT nID );
		virtual void OnUpdateCmdUI( CFrameWnd* pTarget, BOOL bDisableIfNoHndler ) override;
		virtual CSize CalcFixedLayout( BOOL bStretch, BOOL bHorz ) override;
		virtual CSize CalcDynamicLayout( int nLength, DWORD dwMode ) override;

		//******************************************************************************
		// Class Wizard Generated Virtual Function(s):
		//******************************************************************************
		// Überschreibungen
		// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
		//{{AFX_VIRTUAL(SVWindowBar)
	public:
		virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = nullptr) override;
	protected:
		virtual void CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType = adjustBorder) override;
		//}}AFX_VIRTUAL


		//******************************************************************************
		// Debug Operation(s):
		//******************************************************************************

#ifdef _DEBUG
		void AssertValid() const;
		void Dump(CDumpContext& dc) const;
#endif

		//******************************************************************************
		// Data Element(s):
		//******************************************************************************
	protected:
		CString windowTitle;
		CSize windowSize;

		SVSlideBar	slidebar;
		BOOL			m_bMenuRemoved;
		HBRUSH			m_hWindowBackgroundColor;
	};
} //namespace SvMc
