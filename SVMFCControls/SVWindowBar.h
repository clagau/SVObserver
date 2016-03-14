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
		////////////////////////////////////////////////////////////////////////////////
		// .Title       : SVWindowBarClass
		// -----------------------------------------------------------------------------
		// .Description : e.g. This base class capsules an Image and provides its ...
		//              :
		//              :
		// -----------------------------------------------------------------------------
		// .Export
		//	 Public Method				Description
		//  :
		//  :
		////////////////////////////////////////////////////////////////////////////////
		// .History
		//	 Date		Author		Comment                                       
		//  :dd.mm.yyyy				First Implementation
		//	:
		////////////////////////////////////////////////////////////////////////////////
		class SVWindowBarClass : public CControlBar
		{
			//******************************************************************************
			// Serialization Declaration(s):
			//******************************************************************************

			//******************************************************************************
			// MFC Declarations:
			//******************************************************************************
			DECLARE_DYNCREATE( SVWindowBarClass )

			//******************************************************************************
			// Manual Generated Message Map Entries
			//******************************************************************************

			//******************************************************************************
			// Class Wizard Generated Message Map Entries
			//******************************************************************************
		protected:
			//{{AFX_MSG(SVWindowBarClass)
			afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
			afx_msg void OnPaint();
			afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
			afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
			//}}AFX_MSG

			DECLARE_MESSAGE_MAP()

			//******************************************************************************
			// Friend Declaration(s):
			//******************************************************************************

			//******************************************************************************
			// Constructor(s):
			//******************************************************************************
		public:
			SVWindowBarClass();

			//******************************************************************************
			// Destructor(s):
			//******************************************************************************
		public:
			virtual ~SVWindowBarClass();


			//******************************************************************************
			// Operator(s):
			//******************************************************************************
			////////////////////////////////////////////////////////////////////////////////
			// Create Operator
			////////////////////////////////////////////////////////////////////////////////
		public:
			virtual BOOL Create( CWnd* pParentWnd, DWORD dwStyle, UINT nID );

			//******************************************************************************
			// Operation(s) Of Writing Access:
			//******************************************************************************
		public:
			virtual void OnUpdateCmdUI( CFrameWnd* pTarget, BOOL bDisableIfNoHndler );

			//******************************************************************************
			// Operation(s) Of Reading Access:
			//******************************************************************************

			//******************************************************************************
			// Operation(s) Of Process;
			//******************************************************************************

			//******************************************************************************
			// Operation(s) Of Visual Representation:
			//******************************************************************************
		public:
			virtual CSize CalcFixedLayout( BOOL bStretch, BOOL bHorz );
			virtual CSize CalcDynamicLayout( int nLength, DWORD dwMode );

			//******************************************************************************
			// Class Wizard Generated Virtual Function(s):
			//******************************************************************************
			// Überschreibungen
			// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
			//{{AFX_VIRTUAL(SVWindowBarClass)
		public:
			virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
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

