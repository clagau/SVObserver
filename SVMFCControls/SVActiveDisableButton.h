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

namespace Seidenader
{
	namespace SVMFCControls
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
	} //SVMFCControls
} //Seidenader

namespace SvMc = Seidenader::SVMFCControls;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVActiveDisableButton.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 12:58:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Jul 2004 11:35:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/