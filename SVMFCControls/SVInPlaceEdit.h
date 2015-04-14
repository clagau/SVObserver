// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVInPlaceEdit
// * .File Name       : $Workfile:   SVInPlaceEdit.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:37:02  $
// ******************************************************************************

#pragma once

namespace Seidenader
{
	namespace SVMFCControls
	{
		class SVInPlaceEdit : public CEdit
		{
			// Construction
		public:
			SVInPlaceEdit (CWnd* pParent,
				CRect& rect,
				DWORD dwStyle,
				UINT nID,
				int nRow, int nCol,
				CString sInitText,
				UINT nFirstChar,
				BOOL bExitOnArrows=TRUE);

			// Attributes
		public:

			// Operations
		public:
			void EndEdit(void);

			// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(SVInPlaceEdit)
		public:
			virtual BOOL PreTranslateMessage(MSG* pMsg);
		protected:
			virtual void PostNcDestroy();
			//}}AFX_VIRTUAL

			// Implementation
		public:
			virtual ~SVInPlaceEdit();

			// Generated message map functions
		protected:
			//{{AFX_MSG(SVInPlaceEdit)
			afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
			afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
			afx_msg void OnKillFocus(CWnd* pNewWnd);
			afx_msg void OnUpdate();
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()

		private:
			int     m_nRow;
			int     m_nCol;
			CString m_sInitText;
			UINT    m_nLastChar;
			BOOL    m_bExitOnArrows;
			BOOL		m_bEditNumber;
			BOOL		m_bDecimal;
			BOOL		m_bSign;
		};
	} //SVMFCControls
} //Seidenader

namespace SvMc = Seidenader::SVMFCControls;

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVInPlaceEdit.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:37:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Mar 2001 13:40:50   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  116
 * SCR Title:  Enhancements to SVListCtrl class and SVLibrary.Lib
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Replacement classes for the following:
 * SVFEEdit h/cpp
 * SVLVEdit h/cpp
 * SVComboEdit h/cpp
 * Modifications make the preceeding files obsolete.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   06 Dec 2000 11:55:00   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  75
 * SCR Title:  New Library Classes
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Split existing classes into their own file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/