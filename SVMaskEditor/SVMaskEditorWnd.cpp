//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMaskEditorWnd
//* .File Name       : $Workfile:   SVMaskEditorWnd.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   26 Apr 2013 14:08:36  $
//******************************************************************************

#include "stdafx.h"
#include "SVMaskEditor.h"

#include "SVMaskEditorCtl.h"
#include "SVMaskEditorDialog.h"
#include "SVMaskEditorWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVMaskEditorWndClass

SVMaskEditorWndClass::SVMaskEditorWndClass()
{
	EnableAutomation();

    PMother = NULL;
	bPerformDblClk = FALSE;
}

SVMaskEditorWndClass::~SVMaskEditorWndClass()
{
}

void SVMaskEditorWndClass::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CWnd::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(SVMaskEditorWndClass, CWnd)
	//{{AFX_MSG_MAP(SVMaskEditorWndClass)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(SVMaskEditorWndClass, CWnd)
	//{{AFX_DISPATCH_MAP(SVMaskEditorWndClass)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ISVMaskEditorWndClass to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {326BF441-EBC4-11D2-A75C-00106F000C7A}
static const IID IID_ISVMaskEditorWndClass =
{ 0x326bf441, 0xebc4, 0x11d2, { 0xa7, 0x5c, 0x0, 0x10, 0x6f, 0x0, 0xc, 0x7a } };

BEGIN_INTERFACE_MAP(SVMaskEditorWndClass, CWnd)
	INTERFACE_PART(SVMaskEditorWndClass, IID_ISVMaskEditorWndClass, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVMaskEditorWndClass message handlers

void SVMaskEditorWndClass::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    SCROLLINFO sInfo;
    sInfo.cbSize = sizeof( SCROLLINFO );
    sInfo.fMask  = SIF_ALL;
    // Get current 32 Bit values...
    GetScrollInfo( SB_HORZ, &sInfo );

    switch( nSBCode )
	{
		case SB_LINERIGHT:	++sInfo.nPos;
							break;

		case SB_LINELEFT:   --sInfo.nPos;
							break;

		case SB_PAGERIGHT:	sInfo.nPos += sInfo.nPage;
							break;

		case SB_PAGELEFT:	sInfo.nPos -= sInfo.nPage;
							break;

        case SB_THUMBPOSITION:
							sInfo.nPos = sInfo.nTrackPos;
							break;

		case SB_THUMBTRACK:
							sInfo.nPos = sInfo.nTrackPos;
							break;

		case SB_RIGHT:		sInfo.nPos = sInfo.nMax;
							break;

		case SB_LEFT:		sInfo.nPos = sInfo.nMin;
							break;

	};

    if( PMother )
        PMother->PanX( sInfo.nPos );

	
	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void SVMaskEditorWndClass::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    SCROLLINFO sInfo;
    sInfo.cbSize = sizeof( SCROLLINFO );
    sInfo.fMask  = SIF_ALL;
    // Get current 32 Bit values...
    GetScrollInfo( SB_VERT, &sInfo );

    switch( nSBCode )
	{
		case SB_LINEDOWN:	++sInfo.nPos;
							break;

		case SB_LINEUP:		--sInfo.nPos;
							break;

		case SB_PAGEDOWN:	sInfo.nPos += sInfo.nPage;
							break;

		case SB_PAGEUP:		sInfo.nPos -= sInfo.nPage;
							break;

        case SB_THUMBPOSITION:
							sInfo.nPos = sInfo.nTrackPos;
							break;

		case SB_THUMBTRACK:
							sInfo.nPos = sInfo.nTrackPos;
							break;

		case SB_BOTTOM:		sInfo.nPos = sInfo.nMax;
							break;

		case SB_TOP:		sInfo.nPos = sInfo.nMin;
							break;

	};

    if( PMother )
        PMother->PanY( sInfo.nPos );

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void SVMaskEditorWndClass::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// Do not call CWnd::OnPaint() for painting messages
}



void SVMaskEditorWndClass::OnLButtonDown( UINT nFlags, CPoint point )
{
	if( PMother && ! bPerformDblClk )
		PMother->CanvasLButtonDown( nFlags, point );
	
	CWnd::OnLButtonDown( nFlags, point );
}

void SVMaskEditorWndClass::OnLButtonDblClk( UINT nFlags, CPoint point ) 
{
	if( PMother )
	{
		// Set DblClick flag...
		bPerformDblClk = TRUE;
		PMother->CanvasLButtonDblClk( nFlags, point );
	}
	
	CWnd::OnLButtonDblClk( nFlags, point );
}

void SVMaskEditorWndClass::OnLButtonUp( UINT nFlags, CPoint point ) 
{
	if( PMother && ! bPerformDblClk )
		PMother->CanvasLButtonUp( nFlags, point );
	
	CWnd::OnLButtonUp( nFlags, point );
	
	// Last DblClick message resets this flag...
	bPerformDblClk = FALSE;
}

void SVMaskEditorWndClass::OnMouseMove( UINT nFlags, CPoint point ) 
{
	if( PMother && ! bPerformDblClk )
		PMother->CanvasMouseMove( nFlags, point );
	
	CWnd::OnMouseMove( nFlags, point );
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVMaskEditor\SVMaskEditorWnd.cpp_v  $
 * 
 *    Rev 1.0   26 Apr 2013 14:08:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
