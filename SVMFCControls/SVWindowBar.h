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

namespace Seidenader
{
	namespace SVMFCControls
	{
		class SVWindowBarClass : public CControlBar
		{
			DECLARE_DYNCREATE( SVWindowBarClass )

		protected:
			//{{AFX_MSG(SVWindowBarClass)
			afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
			afx_msg void OnPaint();
			afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
			afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
			//}}AFX_MSG

			DECLARE_MESSAGE_MAP()

		public:
			SVWindowBarClass();
			virtual ~SVWindowBarClass();
			virtual BOOL Create( CWnd* pParentWnd, DWORD dwStyle, UINT nID );
			virtual void OnUpdateCmdUI( CFrameWnd* pTarget, BOOL bDisableIfNoHndler );
			virtual CSize CalcFixedLayout( BOOL bStretch, BOOL bHorz );
			virtual CSize CalcDynamicLayout( int nLength, DWORD dwMode );

			//******************************************************************************
			// Class Wizard Generated Virtual Function(s):
			//******************************************************************************
			// Überschreibungen
			// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
			//{{AFX_VIRTUAL(SVWindowBarClass)
		public:
			virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = nullptr);
		protected:
			virtual void CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType = adjustBorder);
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

			SVSlideBarClass	slidebar;
			BOOL			m_bMenuRemoved;
			HBRUSH			m_hWindowBackgroundColor;
		};
	} //SVMFCControls
} //Seidenader

namespace SvMc = Seidenader::SVMFCControls;

