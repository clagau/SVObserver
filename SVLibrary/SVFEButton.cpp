// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVFEButton
// * .File Name       : $Workfile:   SVFEButton.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:31:26  $
// ******************************************************************************

#include "stdafx.h"
#include "SVFEButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVFEButton

BEGIN_MESSAGE_MAP(SVFEButton, CButton)
	//{{AFX_MSG_MAP(SVFEButton)
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVFEButton message handlers

void SVFEButton::DrawItem(LPDRAWITEMSTRUCT lpDIS) 
{
	// Draws the button in its various states
	
	// create a CDC object for drawing
	CDC dc;
	dc.Attach(lpDIS->hDC);
	
	// create a CBrush for the background colour of the button
	CBrush theBrush(GetSysColor(COLOR_3DFACE));
	CBrush *poldBrush = dc.SelectObject(&theBrush);
	CPen *poldPen, thePen;
	if (lpDIS->itemState & ODS_SELECTED)	// Draw button as selected
	{
		// create the pen used to draw the border
		thePen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
		poldPen = dc.SelectObject(&thePen);
		
		// draw the sunken border
		dc.Rectangle(&lpDIS->rcItem);
		
		// draw the dots
		int x = lpDIS->rcItem.right / 2 - 2;
		int y = lpDIS->rcItem.bottom / 2;
		COLORREF dots = GetSysColor(COLOR_BTNTEXT);
		dc.SetPixel(x, y, dots);
		x += 3;
		dc.SetPixel(x, y, dots);
		x += 3;
		dc.SetPixel(x, y, dots);
	}
	else	// draw button as normal
	{
		// draw the outside border
		thePen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DFACE));
		poldPen = dc.SelectObject(&thePen);
		dc.Rectangle(&lpDIS->rcItem);
		dc.SelectObject(poldPen);
		thePen.DeleteObject();
		
		// draw the top and left sides
		thePen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DHILIGHT));
		poldPen = dc.SelectObject(&thePen);
		dc.MoveTo(lpDIS->rcItem.left + 1, lpDIS->rcItem.bottom - 1);
		dc.LineTo(lpDIS->rcItem.left + 1, lpDIS->rcItem.top + 1);
		dc.LineTo(lpDIS->rcItem.right - 2, lpDIS->rcItem.top + 1);
		dc.SelectObject(poldPen);
		thePen.DeleteObject();
		
		// draw the bottom and right sides (top edge)
		thePen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
		poldPen = dc.SelectObject(&thePen);
		dc.LineTo(lpDIS->rcItem.right - 2, lpDIS->rcItem.bottom - 2);
		dc.LineTo(lpDIS->rcItem.left + 1, lpDIS->rcItem.bottom - 2);
		dc.SelectObject(poldPen);
		thePen.DeleteObject();
		
		// draw the bottom and right sides (bottom edge)
		thePen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DDKSHADOW));
		poldPen = dc.SelectObject(&thePen);
		dc.MoveTo(lpDIS->rcItem.right - 1, lpDIS->rcItem.top);
		dc.LineTo(lpDIS->rcItem.right - 1, lpDIS->rcItem.bottom - 1);
		dc.LineTo(lpDIS->rcItem.left, lpDIS->rcItem.bottom - 1);
		
		// draw the dots
		int x = lpDIS->rcItem.right / 2 - 3;
		int y = lpDIS->rcItem.bottom / 2 - 1;
		COLORREF dots;
		if (lpDIS->itemState & ODS_DISABLED)
			dots = GetSysColor(COLOR_GRAYTEXT);
		else
			dots = GetSysColor(COLOR_BTNTEXT);
		dc.SetPixel(x, y, dots);
		x += 3;
		dc.SetPixel(x, y, dots);
		x += 3;
		dc.SetPixel(x, y, dots);
		if (lpDIS->itemState & ODS_DISABLED)
		{
			x = lpDIS->rcItem.right / 2 - 2;
			y = lpDIS->rcItem.bottom / 2;
			dots = GetSysColor(COLOR_3DHILIGHT);
			dc.SetPixel(x, y, dots);
			x += 3;
			dc.SetPixel(x, y, dots);
			x += 3;
			dc.SetPixel(x, y, dots);
		}
	}
	// reset the DC
	dc.SelectObject(poldBrush);
	dc.SelectObject(poldPen);
	thePen.DeleteObject();
	theBrush.DeleteObject();
	dc.Detach();
}

void SVFEButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// button should not keep the focus
	CButton::OnLButtonUp(nFlags, point);
	GetParent()->SetFocus();
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVLibrary\SVFEButton.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:31:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1.1.0   22 Oct 2010 08:10:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include problems and compilier efficiency.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Mar 2001 13:31:14   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  116
 * SCR Title:  Enhancements to SVListCtrl class and SVLibrary.Lib
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Correction due to change in SVFileEditCtrl.
 * SVFileEditCtrl is the only module using this class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   06 Dec 2000 11:54:54   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  75
 * SCR Title:  New Library Classes
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Split existing classes into their own file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
