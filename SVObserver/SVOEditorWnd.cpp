//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVOEditorWnd
//* .File Name       : $Workfile:   SVOEditorWnd.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   21 May 2014 17:51:24  $
//******************************************************************************

//@doc

#include "stdafx.h"
#include "SVOEditorWnd.h"
#include "SVObserver.h"	// for SVEXCLUDECHARS_INSPECTION_NAME

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
 *		@mfunc Constructor
 */
CSVOEditorWnd::CSVOEditorWnd( 
		CListBox* pLB	// @parm Pointer to list box containing item to edit
	)
{
	m_pLB = pLB ;
	m_edit_ended = false ;
}

/*
 *		Destructor -- protected to prevent allocation on the stack
 *			since object will be deleted when window is destroyed
 */
CSVOEditorWnd::~CSVOEditorWnd()
{
}

enum { WM_WAIT_FOR_DONE_EDITING = WM_APP + 1};


BEGIN_MESSAGE_MAP(CSVOEditorWnd, CEdit)
	//{{AFX_MSG_MAP(CSVOEditorWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_NCDESTROY()
    ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
    ON_MESSAGE(WM_WAIT_FOR_DONE_EDITING, OnWaitForDoneEditing)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*
 *		@mfunc Starts editing item
 *
 *		@rdesc True if editing was started. False if n is not a valid index for the
 *			list box being edited
 */
bool CSVOEditorWnd::Edit(
		int n		// @parm Index of item in list box to edit
	)
{
	// 
	//	Invalid index, return false, deletes object
	//
	if ( n < 0 || n >= m_pLB->GetCount() )
	{
		delete this ;
		return false ;
	}

    m_bDoneEditing = false;
	m_edit_index = n ;

	CString s ;
	m_pLB->GetText( n, s ) ;

	CRect r ;
	m_pLB->GetItemRect( n, &r ) ;
	r.InflateRect( 1, 1 ) ;

	Create( WS_BORDER | WS_CHILD | WS_VISIBLE | ES_WANTRETURN, r, m_pLB, 1 ) ;
	SetFont( m_pLB->GetFont()) ;

	SetWindowText( s ) ;
	SetCapture() ;
	SetFocus() ;

	return true ;
}

/*
 *		Handler for mouse click, end editing if outside this window
 */
void CSVOEditorWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	ClientToScreen( &point ) ;
	if ( WindowFromPoint( point ) != this )
		EndEditing() ;
	else
		CEdit::OnLButtonDown(nFlags, point);
}

/*
 *		Delete C++ object when window is destroyed
 */
void CSVOEditorWnd::OnNcDestroy() 
{
	CEdit::OnNcDestroy();
	
	delete this ;
}

/*
 *		Handler for keyboard, ends editing on tab, return, or escape key
 *		leaving listbox entry unchanged for escape key only
 */
void CSVOEditorWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    CString sExcludedChars = SVEXCLUDECHARS_INSPECTION_NAME;
	switch (nChar )
	{
	    case VK_ESCAPE:
		    EndEditing( false ) ;
	    case VK_TAB:
	    case VK_RETURN:
		    EndEditing() ;
		    break ;
	    default:
        {
		    CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
		    break ;
        }
	}
}


void CSVOEditorWnd::WaitForDoneEditing()
{

    SendMessage(WM_WAIT_FOR_DONE_EDITING, 0,0);
}

LRESULT CSVOEditorWnd::OnWaitForDoneEditing(WPARAM, LPARAM)
{
    while (!m_bDoneEditing)
    {
        ::Sleep(0);
        YieldMessages();
    }
    return 0;
}

void CSVOEditorWnd::YieldMessages()
{// Let other messages through
	MSG msg;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


/*
 *		@mfunc Ends editing sequence
 */
void CSVOEditorWnd::EndEditing(
		bool b		// @parmopt bool | b | true | If true, the list box item is updated
	)
{
	// Prevent multiple calls.  
	//
	//		EndEditing() may cause loss of focus, but so may other actions in which case
	//		we need to call EndEditing().  Even though we try to clear the message queue below
	//		we can't completely eliminate the possibility of recursion, so we use a flag
	//		to stop it.
	//
	if ( m_edit_ended )
		return ;

	m_edit_ended = true ;
	ReleaseCapture() ;
	if (b )
	{
		//
		//		Update the list box
		//
		CString s ;
		GetWindowText( s ) ;
        if ( m_pLB->FindStringExact(-1,s) == LB_ERR )
        {
            DWORD_PTR pointer = m_pLB->GetItemData(m_edit_index);
		    m_pLB->DeleteString( m_edit_index ) ;
		    m_pLB->InsertString( m_edit_index, s ) ;
            m_pLB->SetItemData(m_edit_index, pointer);
		    m_pLB->SetCurSel( m_edit_index ) ;
        }
	}

	MSG msg ;
	while ( ::PeekMessage( &msg, m_hWnd, 0, 0, PM_REMOVE ) )
		;

    m_bDoneEditing = true;
	DestroyWindow() ;
}

/*
 *		Tell Windows that we need to get all WM_KEYDOWN messages
 */
UINT CSVOEditorWnd::OnGetDlgCode() 
{
	return DLGC_WANTALLKEYS ;
}

/*
 *		Handler for kill focus reflection, ends edit sequence.  Things like Alt-tabbing to 
 *			another program will cause this to happen in a way which can't be trapped by any
 *			other method
 */
void CSVOEditorWnd::OnKillfocus() 
{
	EndEditing() ;
}

void CSVOEditorWnd::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CString sExcludedChars = _T("\\/:*\"<>|.");
	switch (nChar)
    {
        case VK_BACK:
        {
            break;
        }
        default:
        {
            if (::_tcschr(sExcludedChars,nChar) )
            {
                return;
            }
        }
    }
	
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void CSVOEditorWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
    EndEditing() ;	
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVOEditorWnd.cpp_v  $
 * 
 *    Rev 1.3   21 May 2014 17:51:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised to remove unnessary casting.
 * Revised to correct casting warnings.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 May 2014 11:17:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed DWORD to DWORD_PTR in EndEditing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Oct 2013 06:48:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:03:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   27 Jun 2005 15:05:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed text literal to SVEXCLUDECHARS_***
 * cleaned up file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 13:06:36   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:38:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/