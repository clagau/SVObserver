//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEditHelper
//* .File Name       : $Workfile:   SVEditHelper.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:24:32  $
//******************************************************************************

#include "stdafx.h"
#include "SVEditHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVEditHelper

SVEditHelper::SVEditHelper()
{
}

SVEditHelper::~SVEditHelper()
{
}


BEGIN_MESSAGE_MAP(SVEditHelper, CEdit)
	//{{AFX_MSG_MAP(SVEditHelper)
	ON_WM_KEYUP()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVEditHelper message handlers

void SVEditHelper::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch( nChar )
	{
	case VK_RETURN:
		GetParent()->SendMessage( WM_APP_ED_EDIT_FINISHED, (WPARAM)TRUE );	// Commit changes
	break;

	case VK_ESCAPE:
		GetParent()->SendMessage( WM_APP_ED_EDIT_FINISHED, (WPARAM)FALSE );	// Disregard changes
	break;
	}
	
	CEdit::OnKeyUp(nChar, nRepCnt, nFlags);
}

void SVEditHelper::OnKillFocus(CWnd* pNewWnd) 
{
	GetParent()->SendMessage( WM_APP_ED_EDIT_FINISHED, (WPARAM)TRUE );	// Commit changes

	CEdit::OnKillFocus(pNewWnd);
}


BOOL SVEditHelper::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN )
	{
		UINT nChar = pMsg->wParam;
		if (nChar == VK_ESCAPE || nChar == VK_RETURN || nChar == VK_TAB)
		{
			GetParent()->SetFocus();
			return TRUE;
		}
	}

	return CEdit::PreTranslateMessage(pMsg);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVEditHelper.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:24:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Aug 2012 12:46:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in for editable list box used in remote output groups.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */
