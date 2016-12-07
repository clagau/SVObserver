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
			virtual BOOL PreTranslateMessage(MSG* pMsg) override;
		protected:
			virtual void PostNcDestroy() override;
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

