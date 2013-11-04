// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVNotifyButton
// * .File Name       : $Workfile:   SVNotifyButton.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 16:43:20  $
// ******************************************************************************

#include "stdafx.h"
#include "SVNotifyButton.h"

BEGIN_MESSAGE_MAP(SVNotifyButton, CButton)
	ON_WM_LBUTTONUP()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

void SVNotifyButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	CButton::OnLButtonUp(nFlags, point);

	CRect rect;
	GetWindowRect(&rect);
	CPoint ptScreen = point;
	ptScreen += rect.TopLeft();
	if ( rect.PtInRect( ptScreen ) )
	{
		NMHDR nmhdr;
		nmhdr.code = NM_CLICK;
		nmhdr.hwndFrom = GetSafeHwnd();
		nmhdr.idFrom = GetDlgCtrlID();
		GetOwner()->SendMessage(WM_NOTIFY, (WPARAM) GetDlgCtrlID(), (LPARAM) &nmhdr);
	}
}

void SVNotifyButton::OnKillFocus( CWnd* pNewWnd )
{
	CButton::OnKillFocus( pNewWnd );
	if ( GetOwner() && GetOwner() != pNewWnd )
	{
		GetOwner()->SendMessage(WM_KILLFOCUS, (WPARAM) pNewWnd->GetSafeHwnd(), NULL);
	}
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\PropertyTree\SVNotifyButton.cpp_v  $
 * 
 *    Rev 1.0   18 Apr 2013 16:43:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   13 Aug 2003 13:28:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration - moved from PropTreeItemCombo
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/