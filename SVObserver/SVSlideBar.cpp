//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSlideBarClass
//* .File Name       : $Workfile:   SVSlideBar.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:07:42  $
//******************************************************************************

#include "stdafx.h"
#include "SVSlideBar.h"
#include "SVObjectLibrary/SVLockableClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static SVLockableClass SVSlideBarLock;

SVSlideBarClass::SVSlideBarClass()
{
	bTracking		= FALSE;
	dwSlideBarType	= SV_SLIDEBAR_VERT;
	trackRect.SetRectEmpty();
}

SVSlideBarClass::~SVSlideBarClass()
{
}

BEGIN_MESSAGE_MAP(SVSlideBarClass, CWnd)
	//{{AFX_MSG_MAP(SVSlideBarClass)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_WINDOWPOSCHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVSlideBarClass message handlers

void SVSlideBarClass::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( SVSlideBarLock.Lock() )
	{
		if( bTracking )
		{
			invertTracker( trackRect );

			ClientToScreen( &point );
			if( dwSlideBarType == SV_SLIDEBAR_VERT )
			{
				trackRect.left  = point.x;
				trackRect.right = trackRect.left + SV_SLIDEBAR_WIDTH;
			}
			else
				if( dwSlideBarType == SV_SLIDEBAR_HORZ )
				{
					trackRect.top    = point.y;
					trackRect.bottom = trackRect.top + SV_SLIDEBAR_HEIGHT;
				}

			invertTracker( trackRect );
		}
		else
		{
			// Set cursor...
			if( dwSlideBarType == SV_SLIDEBAR_VERT )
				SetCursor( AfxGetApp()->LoadCursor( AFX_IDC_VSPLITBAR ) );
			else
				if( dwSlideBarType == SV_SLIDEBAR_HORZ )
					SetCursor( AfxGetApp()->LoadCursor( AFX_IDC_HSPLITBAR ) );
		
		}

		SVSlideBarLock.Unlock();

		CWnd::OnMouseMove(nFlags, point);
	}
}

void SVSlideBarClass::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( SVSlideBarLock.Lock() )
	{
		if( bTracking )
			return;
		
		bTracking = TRUE;

		SetCapture();

		GetWindowRect( &trackRect );

		invertTracker( trackRect );

		SVSlideBarLock.Unlock();

		CWnd::OnLButtonDown(nFlags, point);
	}
}

void SVSlideBarClass::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( SVSlideBarLock.Lock() )
	{
		if( ! bTracking )
			return;
		
		bTracking = FALSE;

		invertTracker( trackRect );

		ReleaseCapture();

		trackRect.SetRectEmpty();

		SVSlideBarLock.Unlock();

		CWnd::OnLButtonUp(nFlags, point);
	}
}

void SVSlideBarClass::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CWnd::OnWindowPosChanged(lpwndpos);

	CControlBar* pParent = ( CControlBar* ) GetParent();
	if( pParent )
	{
		DWORD dwParentStyle = pParent->GetBarStyle();
		BOOL bVisible = IsWindowVisible();
			
		if( dwParentStyle & CBRS_FLOATING )
		{
			// No drawing if parent control bar is floating ( not docked ! )
			if( bVisible )
				ShowWindow( SW_HIDE );
			return;
		}

		if( ! bVisible )
			ShowWindow( SW_SHOW );

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
			return;
		}
	}
}

void SVSlideBarClass::invertTracker( const CRect& rect )
{
	ASSERT_VALID(this);
	ASSERT(!rect.IsRectEmpty());
	ASSERT((GetStyle() & WS_CLIPCHILDREN) == 0);

	// pat-blt without clip children on
	CDC* pDC = GetDC();
	// invert the brush pattern (looks just like frame window sizing)
	CBrush* pBrush = CDC::GetHalftoneBrush();
	HBRUSH hOldBrush = NULL;
	if (pBrush != NULL)
		hOldBrush = (HBRUSH)SelectObject(pDC->m_hDC, pBrush->m_hObject);
	pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATINVERT);
	if (hOldBrush != NULL)
		SelectObject(pDC->m_hDC, hOldBrush);
	ReleaseDC(pDC);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVSlideBar.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:07:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   16 Dec 2009 12:54:22   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fix issues with includes and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   16 Feb 2005 15:16:26   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  440
   SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     updated header files
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   22 Apr 2003 15:34:12   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
 */
