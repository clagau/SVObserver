//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEditableListBox
//* .File Name       : $Workfile:   SVEditableListBox.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:24:24  $
//******************************************************************************
#pragma once
#include "afxwin.h"
#include "SVEditHelper.h"

namespace Seidenader
{
	namespace SVMFCControls
	{
		#define		WM_APP_LB_ITEM_EDITED			( WM_APP + 04100 )

		class SVEditableListBox :
			public CListBox
		{
		public:
			SVEditableListBox(void);
			~SVEditableListBox(void);

		protected:
			void    EditStarts  ();
			void    EditEnds    ( BOOL bCommitText = TRUE );

			SVEditHelper   m_ceEdit;
			int     m_iItemBeingEdited;
			int     m_iLastSelected;

			//{{AFX_MSG(SVEditableListBox)
			afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
			afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
			//}}AFX_MSG
			afx_msg LRESULT OnEditFinished(WPARAM wParam, LPARAM lParam);
			DECLARE_MESSAGE_MAP()

		public:
			afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
			afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
			afx_msg void OnSetFocus(CWnd* pOldWnd);
		};
	} //SVMFCControls
} //Seidenader

namespace SvMc = Seidenader::SVMFCControls;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVEditableListBox.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:24:24   bWalter
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
