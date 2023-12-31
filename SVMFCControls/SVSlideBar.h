//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSlideBar
//* .File Name       : $Workfile:   SVSlideBar.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:07:54  $
//******************************************************************************

#pragma once

namespace SvMc
{
	#define SV_SLIDEBAR_WIDTH		5
	#define SV_SLIDEBAR_HEIGHT		5

	class SVSlideBar : public CWnd
	{
		// Construction
	public:
		SVSlideBar() = default;

		virtual ~SVSlideBar() = default;


		// Operations
	public:

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(SVSlideBar)
		//}}AFX_VIRTUAL

		// Implementation
	protected:
		void invertTracker( const CRect& rect );



		// Generated message map functions
	protected:
		//{{AFX_MSG(SVSlideBar)
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		//******************************************************************************
		// Data Element(s):
		//******************************************************************************
	protected:

		enum
		{
			SV_SLIDEBAR_HORZ	= 10,
			SV_SLIDEBAR_VERT
		};

		BOOL	bTracking {false};
		DWORD	dwSlideBarType {SV_SLIDEBAR_VERT};
		CRect   trackRect {0,0,0,0};
	};
} //namespace SvMc
