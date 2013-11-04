//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAdjustToolSizePositionDlg
//* .File Name       : $Workfile:   SVUpDownButton.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:55:48  $
//******************************************************************************

#include "stdafx.h"
#include "SVUpDownButton.h"

BEGIN_MESSAGE_MAP(SVUpDownButton, CButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

SVUpDownButton::SVUpDownButton() : m_bLButtonDown(false)
{
}

void SVUpDownButton::OnLButtonDown( UINT nFlags, CPoint point )
{
	ASSERT( GetParent() );
	m_bLButtonDown = true;
	SetFocus();
	CButton::OnLButtonDown( nFlags, point );
	GetParent()->SendMessage( WM_SV_NOTIFY_LBUTTONDOWN, nFlags, (LPARAM) this );
}

void SVUpDownButton::OnLButtonUp( UINT nFlags, CPoint point )
{
	ASSERT( GetParent() );
	m_bLButtonDown = false;
	CButton::OnLButtonUp( nFlags, point );
	GetParent()->SendMessage( WM_SV_NOTIFY_LBUTTONUP, nFlags, (LPARAM) this );
}

void SVUpDownButton::OnKillFocus( CWnd* pNewWnd )
{
	if ( m_bLButtonDown )
	{
		ASSERT( GetParent() );
		CButton::OnKillFocus(pNewWnd);
		GetParent()->SendMessage( WM_SV_NOTIFY_LBUTTONUP, 0, (LPARAM) this );
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVUpDownButton.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:55:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Feb 2005 12:12:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Adding new files to implement new extent functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/