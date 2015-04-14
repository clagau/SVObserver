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
			// @cmember Constructor
			CSVOEditorWnd( CListBox* pLB );

		protected:
			// @cmember Destructor
			virtual ~CSVOEditorWnd();

			// Attributes
		public:

			// Operations
		public:
			// @cmember Starts editing
			bool Edit( int n );

			void WaitForDoneEditing();

			// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CSVOEditorWnd)
			//}}AFX_VIRTUAL

			// Implementation

			// Generated message map functions
		protected:
			// @cmember Index of item being edited
			int m_edit_index;
			// @cmember List box containing item being edited
			CListBox* m_pLB ;
			// @cmember Flag to prevent EndEditing() from executing more than once
			bool m_edit_ended ;

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

			bool m_bDoneEditing;


			DECLARE_MESSAGE_MAP()
		};
	} //SVMFCControls
} //Seidenader

namespace SvMc = Seidenader::SVMFCControls;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOEditorWnd.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:03:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 13:05:36   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:38:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/