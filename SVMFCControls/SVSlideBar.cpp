//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSlideBar
//* .File Name       : $Workfile:   SVSlideBar.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:07:42  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVSlideBar.h"
#include "SVSystemLibrary/SVLockableClass.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvMc
{
	static SvSyl::SVLockableClass SVSlideBarLock;

	BEGIN_MESSAGE_MAP(SVSlideBar, CWnd)
		//{{AFX_MSG_MAP(SVSlideBar)
		ON_WM_MOUSEMOVE()
		ON_WM_LBUTTONDOWN()
		ON_WM_LBUTTONUP()
		ON_WM_WINDOWPOSCHANGED()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// SVSlideBar message handlers

	void SVSlideBar::OnMouseMove(UINT nFlags, CPoint point) 
	{
		if( SVSlideBarLock.Lock() )
		{
			if( bTracking )
			{
				invertTracker( trackRect );

				ClientToScreen( &point );
				if( SV_SLIDEBAR_VERT == dwSlideBarType )
				{
					trackRect.left  = point.x;
					trackRect.right = trackRect.left + SV_SLIDEBAR_WIDTH;
				}
				else if( SV_SLIDEBAR_HORZ == dwSlideBarType)
				{
					trackRect.top    = point.y;
					trackRect.bottom = trackRect.top + SV_SLIDEBAR_HEIGHT;
				}

				invertTracker( trackRect );
			}
			else
			{
				// Set cursor...
				if( SV_SLIDEBAR_VERT == dwSlideBarType)
				{
					SetCursor( AfxGetApp()->LoadCursor( AFX_IDC_VSPLITBAR ) );
				}
				else if( SV_SLIDEBAR_HORZ == dwSlideBarType )
				{
					SetCursor( AfxGetApp()->LoadCursor( AFX_IDC_HSPLITBAR ) );
				}
			}

			SVSlideBarLock.Unlock();

			CWnd::OnMouseMove(nFlags, point);
		}
	}

	void SVSlideBar::OnLButtonDown(UINT nFlags, CPoint point) 
	{
		if( SVSlideBarLock.Lock() )
		{
			if( !bTracking )
			{
				bTracking = true;

				SetCapture();

				GetWindowRect( &trackRect );

				invertTracker( trackRect );

				SVSlideBarLock.Unlock();

				CWnd::OnLButtonDown(nFlags, point);
			}
		}
	}

	void SVSlideBar::OnLButtonUp(UINT nFlags, CPoint point) 
	{
		if( SVSlideBarLock.Lock() )
		{
			if( bTracking )
			{
				bTracking = false;

				invertTracker( trackRect );

				ReleaseCapture();

				trackRect.SetRectEmpty();

				SVSlideBarLock.Unlock();

				CWnd::OnLButtonUp(nFlags, point);
			}
		}
	}

	void SVSlideBar::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
	{
		CWnd::OnWindowPosChanged(lpwndpos);

		CControlBar* pParent = dynamic_cast<CControlBar*> (GetParent());
		if( pParent )
		{
			DWORD dwParentStyle = pParent->GetBarStyle();
			BOOL bVisible = IsWindowVisible();

			if( dwParentStyle & CBRS_FLOATING )
			{
				// No drawing if parent control bar is floating ( not docked ! )
				if( bVisible )
				{
					ShowWindow( SW_HIDE );
				}
				return;
			}

			if( ! bVisible )
			{
				ShowWindow( SW_SHOW );
			}

			CRect oldRect;
			GetWindowRect( &oldRect );
			pParent->ScreenToClient( &oldRect );

			CRect rect;
			pParent->GetClientRect( &rect );

			if( dwParentStyle & CBRS_TOP )
			{
				// Horizontal slide bar...
				dwSlideBarType = SV_SLIDEBAR_HORZ;

				// Show slide bar on the bottom of parent control bar...
				rect.top = rect.bottom - SV_SLIDEBAR_HEIGHT;
				if( rect != oldRect || ! bVisible )
				{
					MoveWindow( &rect );
					BringWindowToTop();
				}
				return;
			}

			if( dwParentStyle & CBRS_BOTTOM )
			{
				// Horizontal slide bar...
				dwSlideBarType = SV_SLIDEBAR_HORZ;

				// Show slide bar on the top of parent control bar...
				rect.bottom = rect.top + SV_SLIDEBAR_HEIGHT;
				if( rect != oldRect || ! bVisible )
				{
					MoveWindow( &rect );
					BringWindowToTop();
				}
				return;
			}

			if( dwParentStyle & CBRS_LEFT )
			{
				// Vertical slide bar...
				dwSlideBarType = SV_SLIDEBAR_VERT;

				// Show slide bar on the right of parent control bar...
				rect.left = rect.right - SV_SLIDEBAR_WIDTH;
				if( rect != oldRect || ! bVisible )
				{
					MoveWindow( &rect );
					BringWindowToTop();
				}
				return;
			}

			if( dwParentStyle & CBRS_RIGHT )
			{
				// Vertical slide bar...
				dwSlideBarType = SV_SLIDEBAR_VERT;

				// Show slide bar on the left of parent control bar...
				rect.right = rect.left + SV_SLIDEBAR_WIDTH;
				if( rect != oldRect || ! bVisible )
				{
					MoveWindow( &rect );
					BringWindowToTop();
				}
			}
		}
	}

	void SVSlideBar::invertTracker( const CRect& rect )
	{
		ASSERT_VALID(this);
		assert(!rect.IsRectEmpty());
		assert((GetStyle() & WS_CLIPCHILDREN) == 0);

		// pat-blt without clip children on
		CDC* pDC = GetDC();
		// invert the brush pattern (looks just like frame window sizing)
		CBrush* pBrush = CDC::GetHalftoneBrush();
		HBRUSH hOldBrush = nullptr;
		if (nullptr != pBrush )
		{
			hOldBrush = (HBRUSH)SelectObject(pDC->m_hDC, pBrush->m_hObject);
		}
		pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATINVERT);
		if (nullptr != hOldBrush)
		{
			SelectObject(pDC->m_hDC, hOldBrush);
		}
		ReleaseDC(pDC);
	}
} //namespace SvMc
