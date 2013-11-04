//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVActiveDisableButton
//* .File Name       : $Workfile:   SVActiveDisableButton.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 12:58:38  $
//******************************************************************************

// Embossing code taken from ToolBarEx.cpp by Joerg Koenig:
/////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1997 by Joerg Koenig
// All rights reserved
//
// Distribute freely, except: don't remove my name from the source or
// documentation (don't take credit for my work), mark your changes (don't
// get me blamed for your possible bugs), don't alter or remove this
// notice.
// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// Send bug reports, bug fixes, enhancements, requests, flames, etc., and
// I'll try to keep a version up to date.  I can be reached as follows:
//    J.Koenig@adg.de                 (company site)
//    Joerg.Koenig@rhein-neckar.de    (private site)
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SVActiveDisableButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVActiveDisableButton

SVActiveDisableButton::SVActiveDisableButton()
{
}

SVActiveDisableButton::~SVActiveDisableButton()
{
}

BEGIN_MESSAGE_MAP(SVActiveDisableButton, CButton)
	//{{AFX_MSG_MAP(SVActiveDisableButton)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_CHAR()
	ON_WM_ACTIVATE()
	ON_WM_SETFOCUS()
	ON_WM_MOUSEACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVActiveDisableButton message handlers

void SVActiveDisableButton::OnPaint() 
{
	CButton::OnPaint();
	if( m_bDisabled )
	{
		CDC* pDC = GetWindowDC();
		CRect rec;
		GetClientRect( &rec );
		rec.DeflateRect(3,3,3,3);
		Emboss( *pDC, rec );
	}
}

void SVActiveDisableButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if ( !m_bDisabled )
		CButton::OnLButtonDown(nFlags, point);
}

BOOL SVActiveDisableButton::EnableWindow( BOOL bEnable )
{
	bool bTmp = m_bDisabled;
	m_bDisabled = bEnable ? false : true;
	Invalidate();
	return bTmp ? FALSE : TRUE;
}

void SVActiveDisableButton::Emboss( CDC& dc, const CRect& rc ) const
{
	// create a monochrome memory DC
	CDC ddc;
	ddc.CreateCompatibleDC(0);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&ddc, rc.Width(), rc.Height());
	CBitmap * pOldBmp = ddc.SelectObject(&bmp);
	
	// build a mask
	ddc.PatBlt(0, 0, rc.Width(), rc.Height(), WHITENESS);

	dc.SetBkColor(::GetSysColor( COLOR_BTNFACE ));
	ddc.BitBlt(0, 0, rc.Width(), rc.Height(), &dc, rc.left, rc.top, SRCCOPY);
	dc.SetBkColor(::GetSysColor(COLOR_BTNHIGHLIGHT));
	ddc.BitBlt(0, 0, rc.Width(), rc.Height(), &dc, rc.left, rc.top, SRCPAINT);

	// Copy the image from the toolbar into the memory DC
	// and draw it (grayed) back into the toolbar.
	dc.FillSolidRect(rc.left, rc.top, rc.Width(), rc.Height(), ::GetSysColor( COLOR_BTNFACE ));
	dc.SetBkColor(RGB(0, 0, 0));
	dc.SetTextColor(RGB(255, 255, 255));
	CBrush brShadow, brHilight;
	brHilight.CreateSolidBrush(::GetSysColor(COLOR_BTNHIGHLIGHT));
	brShadow.CreateSolidBrush(::GetSysColor(COLOR_BTNSHADOW));
	CBrush * pOldBrush = dc.SelectObject(&brHilight);
	dc.BitBlt(rc.left+1, rc.top+1, rc.Width(), rc.Height(), &ddc, 0, 0, 0x00E20746L);
	dc.SelectObject(&brShadow);
	dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &ddc, 0, 0, 0x00E20746L);
	
	// reset DCs
	dc.SelectObject(pOldBrush);
	ddc.SelectObject(pOldBmp);
	ddc.DeleteDC();

	bmp.DeleteObject();
}

void SVActiveDisableButton::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if ( !m_bDisabled )
		CButton::OnLButtonDblClk(nFlags, point);
}

void SVActiveDisableButton::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if ( !m_bDisabled )
		CButton::OnKeyDown(nChar, nRepCnt, nFlags);
}

void SVActiveDisableButton::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if ( !m_bDisabled )
		CButton::OnKeyUp(nChar, nRepCnt, nFlags);
}

void SVActiveDisableButton::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if ( !m_bDisabled )
		CButton::OnChar(nChar, nRepCnt, nFlags);
}

void SVActiveDisableButton::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	if ( !m_bDisabled )
		CButton::OnActivate(nState, pWndOther, bMinimized);
}

void SVActiveDisableButton::OnSetFocus(CWnd* pOldWnd) 
{
	if ( !m_bDisabled )
		CButton::OnSetFocus(pOldWnd);
}

int SVActiveDisableButton::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
	if ( !m_bDisabled )
		return CButton::OnMouseActivate(pDesktopWnd, nHitTest, message);
	else
		return MA_NOACTIVATEANDEAT;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVActiveDisableButton.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 12:58:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Jul 2004 11:35:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/