//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVOEditorWnd
//* .File Name       : $Workfile:   SVOEditorWnd.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:03:56  $
//******************************************************************************

#pragma once

namespace Seidenader
{
	namespace SVMFCControls
	{
		/*
		*		@doc
		*
		*		@class Edit box for in-place editing of list box items
		*/
		class CSVOEditorWnd : public CEdit
		{
			// Construction
		public:
			//Constructor
			CSVOEditorWnd( CListBox& rListBox, LPCTSTR ExcludeChar );

		protected:
			//Destructor
			virtual ~CSVOEditorWnd();

			// Attributes
		public:

			// Operations
		public:
			// Starts editing
			bool Edit( int n );

			void WaitForDoneEditing();

			// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CSVOEditorWnd)
			//}}AFX_VIRTUAL

			// Implementation

			// Generated message map functions
		protected:
			// @cmember Ends edit sequence, optionally updating the list box
			void EndEditing( bool b = true );


			//{{AFX_MSG(CSVOEditorWnd)
			afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
			afx_msg void OnNcDestroy();
			afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
			afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
			afx_msg UINT OnGetDlgCode();
			afx_msg void OnKillfocus();
			afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
			//}}AFX_MSG


			afx_msg LRESULT OnWaitForDoneEditing(WPARAM, LPARAM);

			void YieldMessages();

			CListBox& m_rListBox;		//Reference to list box containing item being edited
			CString m_ExcludeChar;		//List of characters to exclude
			int m_edit_index;			//Index of item being edited
			bool m_edit_ended ;			//Flag to prevent EndEditing() from executing more than once
			bool m_bDoneEditing;		//Flag to indicate editing is done

			DECLARE_MESSAGE_MAP()
		};
	} //SVMFCControls
} //Seidenader

namespace SvMc = Seidenader::SVMFCControls;

