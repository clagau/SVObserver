//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVFileDialog
//* .File Name       : $Workfile:   SVFileDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:36:02  $
//******************************************************************************

#include "stdafx.h"
#include "SVObserver.h"
#include "SVFileDialog.h"
#include "SVMessage/SVMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CMD_SHORTCUT	30736	// Create Shortcut
#define CMD_DELETE		30737	// Delete
#define CMD_RENAME		30738	// Rename
#define CMD_PROPERTIES	30739	// Properties
#define CMD_CUT			30744	// Cut

/////////////////////////////////////////////////////////////////////////////
// CNewWnd
CNewWnd::CNewWnd()
{
}

CNewWnd::~CNewWnd()
{
}

BEGIN_MESSAGE_MAP(CNewWnd, CWnd)
	//{{AFX_MSG_MAP(CNewWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewWnd message handlers

LRESULT CNewWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( message == WM_INITMENUPOPUP )
	{
		HMENU hmenu = (HMENU)(wParam);
		if( ::IsMenu( hmenu ) )
		{
			int iItems = ::GetMenuItemCount(hmenu);

			if( CMD_RENAME == ::GetMenuItemID( hmenu, (iItems - 2) - 1 ) )
				::RemoveMenu( hmenu, CMD_RENAME, MF_BYCOMMAND );
				//::EnableMenuItem( hmenu, CMD_RENAME, MF_BYCOMMAND | MF_GRAYED );
			if( CMD_DELETE == ::GetMenuItemID( hmenu, (iItems - 3) - 1 ) )
				::RemoveMenu( hmenu, CMD_DELETE, MF_BYCOMMAND );
				//::EnableMenuItem( hmenu, CMD_DELETE, MF_BYCOMMAND | MF_GRAYED );
			if( CMD_CUT == ::GetMenuItemID( hmenu, (iItems - 7) - 1 ) )
				::RemoveMenu( hmenu, CMD_CUT, MF_BYCOMMAND );
				//::EnableMenuItem( hmenu, CMD_CUT, MF_BYCOMMAND | MF_GRAYED );
		}// end if

		return CWnd::WindowProc(message, wParam, lParam);
	}// end if
	else if( message == WM_DRAWITEM )    // Short circuit popups since not MFC
		return CWnd::DefWindowProc(message, wParam, lParam);
	else if( message == WM_MEASUREITEM ) // Short circuit popups since not MFC
		return CWnd::DefWindowProc(message, wParam, lParam);
	else if( message == WM_COMMAND && wParam == 94225 ) // DEL
		return 0;
	else if( message == WM_COMMAND && wParam == 94232 ) // CTRL-X
		return 0;
	else if( message == WM_COMMAND && wParam == 94226 ) // F2
		return 0;
	else
		return CWnd::WindowProc(message, wParam, lParam);
}// end WindowProc

/////////////////////////////////////////////////////////////////////////////
// CSVFileDialog

IMPLEMENT_DYNAMIC(CSVFileDialog, CFileDialog)

CSVFileDialog::CSVFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
	m_bFullAccess = false;
	m_bFullAccess = (TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_UNRESTRICTED_FILE_ACCESS )) ;  
}


BEGIN_MESSAGE_MAP(CSVFileDialog, CFileDialog)
	//{{AFX_MSG_MAP(CSVFileDialog)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSVFileDialog::OnFileNameChange( )
{
	if( !m_bFullAccess && !::IsWindow( m_oWnd1.GetSafeHwnd() ) )
	{
		CWnd *psWnd = GetParent()->GetDlgItem(lst1);
		psWnd = psWnd->GetNextWindow();
		m_oWnd1.SubclassWindow( psWnd->GetSafeHwnd() );

		DWORD dwStyle;
		CWnd *psCtrl = m_oWnd1.GetWindow( GW_CHILD );
		dwStyle = ::GetWindowLong( psCtrl->GetSafeHwnd(), GWL_STYLE );
		dwStyle &= ~LVS_EDITLABELS;
		::SetWindowLong( psCtrl->GetSafeHwnd(), GWL_STYLE, dwStyle );
	}// end if

}// end OnFileNameChange

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVFileDialog.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:36:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   27 Jan 2011 11:06:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   16 Dec 2009 10:24:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Aug 2005 11:16:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed name of function SVIsAllowed to SVIsDisplayable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   21 Jun 2005 13:07:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial changes to add SVSecurity
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Apr 2003 09:33:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   05 Mar 2003 13:55:34   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  331
 * SCR Title:  Makes changes to the Security system to prohibit users from renaming files
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed security and menus as requested by Germany.
 * Some users won't be able to rename files on the File Open/Save Dialogs. Other users lost or gained certain privileges.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   04 Oct 2001 14:49:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  229
 * SCR Title:  Upgrade SVObserver to Version 3.34 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified source code to remove compiler warning message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
