//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileDialog
//* .File Name       : $Workfile:   SVFileDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Sep 2014 13:15:12  $
//******************************************************************************

#include "stdafx.h"
#include <Dlgs.h>
#include "SVFileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CMD_SHORTCUT	30993//30736	// Create Shortcut
#define CMD_DELETE		30994//30737	// Delete
#define CMD_RENAME		30995//30738	// Rename
#define CMD_PROPERTIES	30996//30739	// Properties
#define CMD_CUT			31001//30744	// Cut
#define CMD_COPY		31002			// Copy

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
	if (WM_INITMENUPOPUP == message)
	{
		HMENU hmenu = reinterpret_cast<HMENU>(wParam);
		if (::IsMenu(hmenu))
		{
/* for debug purposes, in case the IDs change again
			TCHAR szItemText[256] = {_T('\0')};
			int iItems = ::GetMenuItemCount(hmenu);
			for (int i = 0;i < iItems;i++)
			{
				UINT menuID = ::GetMenuItemID(hmenu, i);
				MENUITEMINFO itemInfo;
				ZeroMemory(&itemInfo, sizeof(itemInfo));
				ZeroMemory(&szItemText, sizeof(szItemText));
				itemInfo.cbSize = sizeof(itemInfo);
				itemInfo.fMask = MIIM_STRING; 
				itemInfo.dwTypeData = szItemText; 
				itemInfo.cch = (sizeof(szItemText) / sizeof(TCHAR));
				::GetMenuItemInfo(hmenu, menuID, false, &itemInfo);

				TRACE("Menu POS: %d ID: %d String: %s\n", i, menuID, szItemText);
			}
*/
			::RemoveMenu(hmenu, CMD_RENAME, MF_BYCOMMAND);
			::RemoveMenu(hmenu, CMD_DELETE, MF_BYCOMMAND);
			::RemoveMenu(hmenu, CMD_CUT, MF_BYCOMMAND);
		}
	}
	else if (WM_DRAWITEM == message || WM_MEASUREITEM == message)    // Short circuit popups since not MFC
	{
		return CWnd::DefWindowProc(message, wParam, lParam);
	}
	else if (message == WM_COMMAND)
	{
		if (28689 == LOWORD(wParam) ||  // DEL
			28696 == LOWORD(wParam) ||  // CTRL-X
			28690 == LOWORD(wParam))	// F2
			return 0;
	}
	return CWnd::WindowProc(message, wParam, lParam);
}

IMPLEMENT_DYNAMIC(SVFileDialog, CFileDialog)

SVFileDialog::SVFileDialog(BOOL bOpenFileDialog, bool bFullAccess, LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) 
: CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd, 0, false)
, m_bFullAccess(bFullAccess)
{
}

SVFileDialog::~SVFileDialog()
{
}

BEGIN_MESSAGE_MAP(SVFileDialog, CFileDialog)
	//{{AFX_MSG_MAP(SVFileDialog)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void SVFileDialog::OnFolderChange()
{
	CFileDialog::OnFolderChange();
	
	if (!m_bFullAccess && !::IsWindow(m_oWnd1.GetSafeHwnd()))
	{
		CWnd* psWnd = GetParent()->GetDlgItem(lst2);
		if (psWnd)
		{
			m_oWnd1.SubclassWindow(psWnd->GetSafeHwnd());

			CWnd *psCtrl = m_oWnd1.GetWindow(GW_CHILD);
			if (psCtrl)
			{
				DWORD dwStyle = ::GetWindowLong(psCtrl->GetSafeHwnd(), GWL_STYLE);
				dwStyle &= ~LVS_EDITLABELS;
				::SetWindowLong(psCtrl->GetSafeHwnd(), GWL_STYLE, dwStyle);
			}
		}
	}
	else
	{
		// Sometimes the List View Control doesn't get destroyed between invocations, so we reset the style to allow renaming.
		CWnd* psWnd = GetParent()->GetDlgItem(lst2);
		if (psWnd)
		{
			CWnd *psCtrl = psWnd->GetWindow(GW_CHILD);
			if (psCtrl)
			{
				DWORD dwStyle = ::GetWindowLong(psCtrl->GetSafeHwnd(), GWL_STYLE);
				if (LVS_EDITLABELS != (dwStyle & LVS_EDITLABELS))
				{
					dwStyle |= LVS_EDITLABELS;
					::SetWindowLong(psCtrl->GetSafeHwnd(), GWL_STYLE, dwStyle);
				}
			}
		}
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVFileDialog.cpp_v  $
 * 
 *    Rev 1.0   18 Sep 2014 13:15:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  944
 * SCR Title:  Fix Security for File and Folder Selection Dialog for 64 Bit
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin, moved from SVObserver
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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
