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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace Seidenader
{
	namespace SVMFCControls
	{
		CSVOEditorWnd::CSVOEditorWnd( CListBox& rListBox, LPCTSTR ExcludeChar ) :
			m_rListBox( rListBox )
		,	m_ExcludeChar( ExcludeChar )
		,	m_edit_ended( false )
		,	m_bDoneEditing( false )
		{
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
		bool CSVOEditorWnd::Edit( int n	)
		{
			// 
			//	Invalid index, return false, deletes object
			//
			if ( n < 0 || n >= m_rListBox.GetCount() )
			{
				delete this ;
				return false ;
			}

			m_bDoneEditing = false;
			m_edit_index = n ;

			CString s ;
			m_rListBox.GetText( n, s ) ;

			CRect r ;
			m_rListBox.GetItemRect( n, &r ) ;
			r.InflateRect( 1, 1 ) ;

			Create( WS_BORDER | WS_CHILD | WS_VISIBLE | ES_WANTRETURN, r, &m_rListBox, 1 ) ;
			SetFont( m_rListBox.GetFont()) ;

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
				if ( m_rListBox.FindStringExact(-1,s) == LB_ERR )
				{
					DWORD_PTR pointer = m_rListBox.GetItemData(m_edit_index);
					m_rListBox.DeleteString( m_edit_index ) ;
					m_rListBox.InsertString( m_edit_index, s ) ;
					m_rListBox.SetItemData(m_edit_index, pointer);
					m_rListBox.SetCurSel( m_edit_index ) ;
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
			switch (nChar)
			{
			case VK_BACK:
				{
					break;
				}
			default:
				{
					if (::_tcschr(m_ExcludeChar, nChar) )
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
	} //SVMFCControls
} //Seidenader

