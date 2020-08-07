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
#pragma region Inlcudes
#include "SVEditHelper.h"
#pragma endregion Inlcudes

namespace SvMc
{
	#define		WM_APP_LB_ITEM_EDITED			( WM_APP + 04100 )

	class SVEditableListBox :
		public CListBox
	{
	public:
		SVEditableListBox(void);
		virtual ~SVEditableListBox(void);

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
} //namespace SvMc
