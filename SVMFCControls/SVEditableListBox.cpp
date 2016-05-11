//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEditableListBox
//* .File Name       : $Workfile:   SVEditableListBox.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:24:16  $
//******************************************************************************
#include "StdAfx.h"
#include "SVEditableListBox.h"

namespace Seidenader
{
	namespace SVMFCControls
	{
		SVEditableListBox::SVEditableListBox(void)
		{
			m_iLastSelected=-1;
		}

		SVEditableListBox::~SVEditableListBox(void)
		{
			if( nullptr != m_ceEdit.GetSafeHwnd() )
				m_ceEdit.DestroyWindow();
		}

		BEGIN_MESSAGE_MAP(SVEditableListBox, CListBox)
			//{{AFX_MSG_MAP(SVEditableListBox)
			ON_WM_HSCROLL()
			ON_WM_VSCROLL()
			//}}AFX_MSG_MAP
			ON_MESSAGE(WM_APP_ED_EDIT_FINISHED, OnEditFinished)
			ON_WM_KEYUP()
			ON_WM_LBUTTONUP()
			ON_WM_SETFOCUS()
		END_MESSAGE_MAP()


		void SVEditableListBox::EditStarts()
		{
			int iSel = GetCurSel();

			// Make sure the edit is not already started.
			if( nullptr != m_ceEdit.GetSafeHwnd() )
				return;

			// Anything selected?
			if( LB_ERR != iSel )
			{
				CRect rItem;

				// Get the rectangle and position
				// this item occupies in the listbox
				GetItemRect( iSel, rItem );

				// Make the rectangle a bit larger
				// top-to-bottom. Not necessary but looks better to me
				rItem.InflateRect( 0, 2, 0, 2 );

				// Create the edit control
				m_ceEdit.Create( WS_VISIBLE | WS_CHILD | 
					WS_BORDER | ES_LEFT | ES_AUTOHSCROLL , 
					rItem, this, 1 );

				// Give it the same font as the listbox
				m_ceEdit.SetFont( this->GetFont() );

				// Now add the item's text to it
				// and selected for convenience
				CString csItem;

				GetText( iSel, csItem );

				// Trim off text in (..)
				int l_pos = csItem.Find("(");
				if( l_pos > 0 )
				{
					csItem = csItem.Left( l_pos );
				}

				m_ceEdit.SetWindowText( csItem );
				m_ceEdit.SetSel( 0, -1, TRUE );

				// Set the focus on it
				m_ceEdit.SetFocus();

				// Record the item position
				m_iItemBeingEdited = iSel;
			}
		}

		void SVEditableListBox::EditEnds( BOOL bCommitText /* = TRUE */ )
		{
			if( nullptr != m_ceEdit.GetSafeHwnd() )
			{
				// Do we want the text entered by the user
				// to replace the selected lb item's text?
				if( bCommitText && m_iItemBeingEdited != -1 )
				{
					CString csOldItem, csNewItem;

					GetText( m_iItemBeingEdited, csOldItem );
					m_ceEdit.GetWindowText( csNewItem );

					// If the new text is the same as the old, why bother
					if( csOldItem.Compare( csNewItem ) != 0 )
					{
						// Get rid of the lb item that we are replacing
						DeleteString( m_iItemBeingEdited );

						// If the listbox has the "sort" attribute on,
						// we add the new text and let it sort it out
						// Otherwise, we insert the new text
						// precisely where the old one was
						if( GetStyle() & LBS_SORT )
							m_iItemBeingEdited = AddString( csNewItem );
						else
							m_iItemBeingEdited = 
							InsertString( m_iItemBeingEdited, csNewItem );

						// Select the lb item
						SetCurSel( m_iItemBeingEdited );

						// Let the parent know if LBS_NOTIFY is flagged
						if( GetStyle() & LBS_NOTIFY )
							GetParent()->SendMessage( WM_APP_LB_ITEM_EDITED, (WPARAM)m_iItemBeingEdited, (LPARAM)(LPCTSTR)csOldItem );
					}
				}

				// The editing is done, nothing is marked for editing
				m_iItemBeingEdited = -1;

				// Get rid of the editing window
				m_ceEdit.DestroyWindow();
			}
		}

		void SVEditableListBox::OnHScroll(UINT nSBCode, 
			UINT nPos, CScrollBar* pScrollBar)
		{
			EditEnds();

			CListBox::OnHScroll(nSBCode, nPos, pScrollBar);
		}

		void SVEditableListBox::OnVScroll(UINT nSBCode, 
			UINT nPos, CScrollBar* pScrollBar)
		{
			EditEnds();

			CListBox::OnVScroll(nSBCode, nPos, pScrollBar);
		}

		LRESULT SVEditableListBox::OnEditFinished(WPARAM wParam, LPARAM /*lParam*/)
		{
			EditEnds( (BOOL)wParam );

			return FALSE;
		}


		void SVEditableListBox::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
		{
			if( nChar == VK_INSERT )
				EditStarts();
			CListBox::OnKeyUp(nChar, nRepCnt, nFlags);
		}


		void SVEditableListBox::OnLButtonUp(UINT nFlags, CPoint point)
		{
			int l_sel = GetCurSel();

			if( l_sel == m_iLastSelected )
			{
				EditStarts();
			}
			else
			{
				m_iLastSelected = l_sel;
			}

			CListBox::OnLButtonUp(nFlags, point);
		}

		void SVEditableListBox::OnSetFocus(CWnd* pOldWnd)
		{
			CListBox::OnSetFocus(pOldWnd);

			m_iLastSelected = -1;
		}
	} //SVMFCControls
} //Seidenader

