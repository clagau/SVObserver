// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : PropertyItemFile.cpp
// * .File Name       : $Workfile:   SVFileEditCtrl.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   01 Oct 2013 09:57:48  $
// ******************************************************************************

#include "stdafx.h"
#include "SVFileEditCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Helper function
BOOL IsWindow (CWnd *pWnd)
{
	if (!pWnd)
		return (FALSE);
	return ::IsWindow (pWnd->m_hWnd);
}

/////////////////////////////////////////////////////////////////////////////
// SVFileEditCtrl

SVFileEditCtrl::SVFileEditCtrl(	CWnd* pParent,
																CRect& rect,
																DWORD dwStyle,
																UINT nID,
																int nRow, int nCol,
																LPCTSTR sInitText, 
																UINT nFirstChar,
																BOOL bExitOnArrows/*=TRUE*/)
{
	// Initialize all variables
	m_buffer				= NULL;
	m_button				= new SVFEButton();
	m_edit					= new CEdit();
	m_bFindFolder   = FALSE;		
	m_pcfd          = NULL;
	
	m_sInitText     = sInitText;
	m_nRow          = nRow;
	m_nCol          = nCol;
	m_nLastChar     = 0; 
	m_bExitOnArrows = bExitOnArrows;
	
	DWORD dwEditStyle = WS_CHILD | WS_VISIBLE| dwStyle;
	if (!Create(NULL, NULL, dwEditStyle, rect, pParent, nID))
		return;
	
	SetFont(pParent->GetFont());
	SetWindowText(sInitText);
	SetFocus();
	
	switch (nFirstChar)
	{
		case VK_RETURN: 
			SetSel((int)_tcslen(m_sInitText), -1);
			return;
		case VK_BACK: 
			SetSel((int)_tcslen(m_sInitText), -1); 
			break;
		case VK_TAB:
		case VK_DOWN: 
		case VK_UP:   
		case VK_RIGHT:
		case VK_LEFT:  
		case VK_NEXT:  
		case VK_PRIOR: 
		case VK_HOME:
		case VK_SPACE:
		case VK_END: 
			SetSel(0, - 1);
			return;
		default:
			SetSel(0, - 1);
	}
	
	m_edit->SendMessage(WM_CHAR, nFirstChar);
}

SVFileEditCtrl::~SVFileEditCtrl()
{
	// clean up all variables
	if (m_buffer)
		delete m_buffer;
	if (m_button)
		delete m_button;
	if (m_edit)
		delete m_edit;
	if (m_pcfd)
		delete m_pcfd;
}


BEGIN_MESSAGE_MAP(SVFileEditCtrl, CWnd)
	//{{AFX_MSG_MAP(SVFileEditCtrl)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_ENABLE()
	//}}AFX_MSG_MAP
	ON_EN_KILLFOCUS (1, OnKillfocusEdit) 
	ON_BN_CLICKED (2, OnBrowse)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVFileEditCtrl message handlers

// OnCreate and OnSize are called if the control is created with Create() or CreateEx()
int SVFileEditCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Get the edit control styles and create the edit control
	DWORD style = lpCreateStruct->style & 0x1800089BL; // WS_VISIBLE | WS_DISABLED | permitted edit control styles
	if (!m_edit->Create(WS_CHILD | WS_CLIPSIBLINGS | style, CRect (0, 0, 1, 1), this, 1))
		return -1;
	
	// Get the button styles and create the button
	style = lpCreateStruct->style & 0x18000000L; // WS_VISIBLE | WS_DISABLED
	if (!m_button->Create(_T(""), WS_CHILD | WS_CLIPSIBLINGS | BS_OWNERDRAW | style, CRect (0, 0, 1, 1), this, 2))
		return -1;
	
	// determine if control will find folders or files
	m_bFindFolder = lpCreateStruct->style & FES_FOLDER;
	
	if (m_bFindFolder)
	{
		// set up the browseinfo structure used by SHBrowseForFolder()
		::ZeroMemory(&m_bi, sizeof(BROWSEINFO));
		m_bi.hwndOwner = GetSafeHwnd();
		m_bi.lpszTitle = _T("Browse for Folder");
		m_bi.ulFlags = BIF_RETURNONLYFSDIRS;
	}
	else
	{
		// create the CFileDialog
		m_pcfd = new CFileDialog(TRUE);
		m_pcfd->m_ofn.hwndOwner = GetSafeHwnd();
	}
	return 0;
}

void SVFileEditCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	// set the control sizes
	m_edit->MoveWindow(0, 0, cx - cy, cy);
	m_button->MoveWindow(cx - cy, 0, cy, cy);
}

// called when the user clicks the ellipses button
void SVFileEditCtrl::OnBrowse()
{
	if (m_bFindFolder)
		FECBrowseForFolder();
	else
		FECOpenFile();
}

// This is the default callback procedure for the SHBrowseForFolder function.
// It will set the current selection to the directory specified in the edit control
int CALLBACK FECCallBackProc(HWND hWnd, UINT Msg, LPARAM unused, LPARAM lpData)
{
	if (Msg == BFFM_INITIALIZED)
		::SendMessage(hWnd, BFFM_SETSELECTION, TRUE, lpData);
	return 0;
}

// FECBrowseForFolder() sets up and calls SHBrowseForFolder(). returns TRUE
// if user clicks OK, returns FALSE otherwise
BOOL SVFileEditCtrl::FECBrowseForFolder()
{
	BOOL Proc = TRUE;	// assume user of this class has set a callback procedure
	BOOL Display = TRUE;// assume user of this class has set a DisplayName
	AllocateBuffer(MAX_PATH);
	if (!m_bi.pszDisplayName)	// user has not set a display name
	{							// flag it, and use our own buffer
		Display = FALSE;
		m_bi.pszDisplayName = m_buffer;
	}
	CString Path;
	LPARAM oldLP = m_bi.lParam;
	if (!m_bi.lpfn)	// user has not set a callback procedure
	{					// flag it, and use the default procedure
		Proc = FALSE;
		GetWindowText(Path);	// get the path in the edit control
		m_bi.lParam = (LPARAM)(LPCTSTR)Path; // set lParam to point to path
		m_bi.lpfn = FECCallBackProc;	// set the callback procedure
	}
	LPITEMIDLIST idl = SHBrowseForFolder (&m_bi);
	if (!Display)		// reset m_bi to clear the DisplayName
		m_bi.pszDisplayName = NULL;
	if (!Proc)			// reset m_bi to clear the default callback proc.
	{
		m_bi.lpfn = NULL;
		m_bi.lParam = oldLP;
	}
	if (idl)
	{
		SHGetPathFromIDList (idl, m_buffer);	// get path string from ITEMIDLIST
		m_edit->SetWindowText(m_buffer);		// update edit control
		LPMALLOC lpm;
		if (SHGetMalloc (&lpm) == NOERROR)
			lpm->Free(idl);    // free memory returned by SHBrowseForFolder
		delete m_buffer;
		m_buffer = NULL;
		return TRUE;
	}
	delete m_buffer;
	m_buffer = NULL;
	return FALSE;
}

// FECOpenFile() sets up the CFileDialog and calls CFileDialog::DoModal().
// returns TRUE if the user clicked the OK button, returns FALSE otherwise
BOOL SVFileEditCtrl::FECOpenFile()
{
	BOOL dirset = TRUE; // assume user of this class has set the initial directory
	if (m_pcfd->m_ofn.lpstrInitialDir == NULL)	// user has not set the initial directory
	{											// flag it, set initial directory to
		dirset = FALSE;							// directory in edit control
		m_pcfd->m_ofn.lpstrInitialDir = (LPTSTR)GetStartPosition();
	}
	// Start the CFileDialog
	if (m_pcfd->DoModal() == IDOK)
	{	// user clicked OK, enter files selected into edit control
		CString Path = _T("");
		POSITION pos = m_pcfd->GetStartPosition();
		Path += m_pcfd->GetNextPathName(pos);
		while (pos)
			Path += _T(";") + m_pcfd->GetNextPathName(pos);
		m_edit->SetWindowText (Path);
		if (!dirset)	// reset initial directory
			m_pcfd->m_ofn.lpstrInitialDir = NULL;
		return TRUE;
	}
	if (!dirset)
		m_pcfd->m_ofn.lpstrInitialDir = NULL;
	return FALSE;
}

void SVFileEditCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	// make sure the edit control has the input focus
	CWnd::OnSetFocus(pOldWnd);

	m_edit->SetFocus();
}

void SVFileEditCtrl::OnEnable(BOOL bEnable) 
{
	CWnd::OnEnable(bEnable);
	
	// enable or disable the edit and button controls
	m_button->EnableWindow(bEnable);
	m_edit->EnableWindow(bEnable);
}

void SVFileEditCtrl::SetFont(CFont *pFont, BOOL bRedraw)
{
	m_edit->SetFont(pFont, bRedraw);
}

void SVFileEditCtrl::SetWindowText(LPCTSTR text)
{
 	m_edit->SetWindowText(text);
}

void SVFileEditCtrl::GetWindowText(CString &text) const
{
	m_edit->GetWindowText(text);
}

int SVFileEditCtrl::GetWindowText(LPTSTR buffer, int max) const
{
	return m_edit->GetWindowText(buffer, max);
}

int SVFileEditCtrl::GetWindowTextLength() const
{
	return m_edit->GetWindowTextLength();
}

BROWSEINFO* SVFileEditCtrl::GetBrowseInfo()
{
	// returns a pointer to the BROWSEINFO structure
	// so the user of this control can set their own
	// parameters for SHBrowseForFolder
	return &m_bi;
}

CEdit* SVFileEditCtrl::GetEditControl() const
{
	// returns a pointer to the edit control so the
	// user can modify it (background colour, transparency, ect.)
	return m_edit;
}

OPENFILENAME* SVFileEditCtrl::GetOpenFileName() const
{
	// returns a pointer to the OPENFILENAME structure so the
	// user can modify the Open File dialog
	if (m_pcfd)
		return (&m_pcfd->m_ofn);
	return NULL;
}

void SVFileEditCtrl::OnKillfocusEdit() 
{
	if (GetFocus() != m_button)
		EndEdit();
}

POSITION SVFileEditCtrl::GetStartPosition()
{
	// if the edit window is active, fills the buffer with the text in the window
	// returns a pointer to the buffer, type-casted as an MFC POSITION structure
	if (IsWindow(m_edit))
	{
		int len = GetWindowTextLength();
		if (!len)
			return NULL;
		AllocateBuffer(len + 1);
		GetWindowText(m_buffer, len + 1);
	}
	return (POSITION)m_buffer;
}

CString SVFileEditCtrl::GetNextPathName(POSITION &pos)
{
	// returns the file name specified position in the buffer
	// updates the position to point to the next file, to NULL if no more files
	if (!pos)
	{
		TRACE (_T("SVFileEditCtrl::GetNextPathName() called with a NULL position"));
		return (CString)"";
	}

	unsigned long x;
	TCHAR *str = (TCHAR *)pos;
	TCHAR *ptr = _tcschr(str, ';');
	if (ptr)
	{
		x = static_cast<unsigned long>(ptr - str);
		if (static_cast<unsigned long>((str - m_buffer)) + x + 1 >= _tcslen(m_buffer))
			pos = NULL;
		else
			pos = (POSITION)(pos + x + 1);
	}
	else
	{
		x = static_cast<unsigned long>(_tcslen(str));
		pos = NULL;
	}
	CString retval = str;
	
	return (retval.Left(x));
}

BOOL SVFileEditCtrl::FindFolder(BOOL bFolder)
{
	// if bFolder is TRUE, changes the control to find folders, not files
	if (bFolder)
	{
		if (m_bFindFolder)
			return TRUE;
		m_bFindFolder = TRUE;
		if (m_pcfd)
		{
			delete m_pcfd;
			m_pcfd = NULL;
		}
		// set up the browseinfo structure used by SHBrowseForFolder()
		AllocateBuffer(MAX_PATH);
		::ZeroMemory(&m_bi, sizeof(BROWSEINFO));
		m_bi.hwndOwner = GetSafeHwnd();
		m_bi.pszDisplayName = m_buffer;
		m_bi.lpszTitle = _T("Browse for Folder");
		m_bi.ulFlags = BIF_RETURNONLYFSDIRS;
		return TRUE;
	}
	else
	{
		// if bFolder is FALSE, changes the control to find files, not folders
		if (!m_bFindFolder)
			return TRUE;
		m_bFindFolder = FALSE;
		
		// create the CFileDialog
		m_pcfd = new CFileDialog(TRUE);
		m_pcfd->m_ofn.hwndOwner = GetSafeHwnd();
		return TRUE;
	}
	return FALSE;
}


void SVFileEditCtrl::PostNcDestroy() 
{
	delete this;	
}


BOOL SVFileEditCtrl::PreTranslateMessage(MSG* pMsg) 
{
	// Make sure that the keystrokes continue to the appropriate handlers
	if( pMsg->message == WM_KEYDOWN )
	{
		UINT nChar = static_cast<UINT>(pMsg->wParam);

		if ((nChar == VK_PRIOR || nChar == VK_NEXT ||
				nChar == VK_DOWN  || nChar == VK_UP   ||
				nChar == VK_RIGHT || nChar == VK_LEFT) &&
				(m_bExitOnArrows || GetKeyState(VK_CONTROL) < 0))
		{
			m_nLastChar = nChar;
			GetParent()->SetFocus();
			return TRUE;
		}

		if (nChar == VK_ESCAPE || nChar == VK_RETURN || nChar == VK_TAB)
		{
			if (nChar == VK_ESCAPE)
				SetWindowText(m_sInitText);
			m_nLastChar = nChar;
			GetParent()->SetFocus();
			return TRUE;
		}
	}
	
	// Catch the Alt key so we don't choke if focus is going to an owner drawn button
	if (pMsg->message == WM_SYSCHAR)
		return TRUE;
	
	return CWnd::PreTranslateMessage(pMsg);
}


void SVFileEditCtrl::SetSel(int nStartChar, int nEndChar, BOOL bNoScroll/*=FALSE*/)
{
	m_edit->SetSel(nStartChar, nEndChar, bNoScroll);
}


void SVFileEditCtrl::EndEdit()
{
	CString str;

	// EFW - BUG FIX - Clicking on a grid scroll bar in a derived class
	// that validates input can cause this to get called multiple times
	// causing assertions because the edit control goes away the first time.
	static BOOL bAlreadyEnding = FALSE;
	
	if (bAlreadyEnding)
		return;
		
	bAlreadyEnding = TRUE;
	GetWindowText(str);
			
	// Send Notification to parent
	LV_DISPINFO dispinfo;
				
	dispinfo.hdr.hwndFrom    = GetSafeHwnd();
	dispinfo.hdr.idFrom      = GetDlgCtrlID();
	dispinfo.hdr.code        = LVN_ENDLABELEDIT;
					
	dispinfo.item.mask       = LVIF_TEXT | LVIF_PARAM;
	dispinfo.item.iItem      = m_nRow;
	dispinfo.item.iSubItem   = m_nCol;
	dispinfo.item.pszText    = LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();
	dispinfo.item.lParam     = (LPARAM)m_nLastChar; 
						
	CWnd* pOwner = GetOwner();
	if (pOwner)
	{
		pOwner->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&dispinfo );
		pOwner->SetFocus();
	}
							
	// Close this window (PostNcDestroy will delete this)
	if (IsWindow(GetSafeHwnd()))
		SendMessage(WM_CLOSE, 0, 0);
	bAlreadyEnding = FALSE;
}


void SVFileEditCtrl::AllocateBuffer(int len)
{
	if (m_buffer)
		delete m_buffer;
	m_buffer = new TCHAR[len];
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVFileEditCtrl.cpp_v  $
 * 
 *    Rev 1.1   01 Oct 2013 09:57:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 13:36:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Oct 2010 08:10:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include problems and compilier efficiency.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Jun 2007 15:16:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Mar 2001 13:33:22   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  116
 * SCR Title:  Enhancements to SVListCtrl class and SVLibrary.Lib
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Program change: no longer requires SVFEEdit.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   06 Dec 2000 11:54:56   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  75
 * SCR Title:  New Library Classes
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Split existing classes into their own file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   05 Dec 2000 09:27:38   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  75
 * SCR Title:  New Library Classes
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   SVPropertyListBox and associated item classes.
 * SVPropertyItem
 * SVPropertyItemCombo
 * SVPropertyItemEdit
 * SVPropertyItemFile
 * SVPropertyItemFont
 * 
 * Initial Release
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
