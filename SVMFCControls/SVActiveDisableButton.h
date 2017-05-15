//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVActiveDisableButton
//* .File Name       : $Workfile:   SVActiveDisableButton.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 12:58:54  $
//******************************************************************************

#pragma once

namespace SvMc
{
	class SVActiveDisableButton : public CButton
	{
		// Construction
	public:
		SVActiveDisableButton();
		virtual ~SVActiveDisableButton();

		BOOL EnableWindow(BOOL bEnable = TRUE);

		// Overrides

		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(SVActiveDisableButton)
		//}}AFX_VIRTUAL

		// Generated message map functions
	protected:
		//{{AFX_MSG(SVActiveDisableButton)
		afx_msg void OnPaint();
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
		afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
		afx_msg void OnSetFocus(CWnd* pOldWnd);
		afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
		//}}AFX_MSG
		void Emboss( CDC& dc, const CRect& rc ) const;

		bool m_bDisabled;
		DECLARE_MESSAGE_MAP()
	};
} //namespace SvMc
