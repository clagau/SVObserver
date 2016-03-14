//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEditNumbers
//* .File Name       : $Workfile:   SVEditNumbers.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:26:16  $
//******************************************************************************

#if !defined(AFX_EDITNUMBERS_H__)
#define AFX_EDITNUMBERS_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

namespace Seidenader
{
	namespace SVMFCControls
	{
		class CEditNumbers : public CEdit
		{
			// Construction
		public:
			CEditNumbers();

			// Attributes
		public:

			// Operations
		public:

			// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CEditNumbers)
			//}}AFX_VIRTUAL

			// Implementation
		public:
			virtual ~CEditNumbers();

			// Generated message map functions
		protected:
			//{{AFX_MSG(CEditNumbers)
			afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
			afx_msg void OnSetFocus(CWnd* pOldWnd);
			//}}AFX_MSG

			DECLARE_MESSAGE_MAP()
		};

		//{{AFX_INSERT_LOCATION}}
		// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

		/////////////////////////////////////////////////////////////////////////////
		// CEditNumbersFloat window

		class CEditNumbersFloat : public CEdit
		{
			// Construction
		public:
			CEditNumbersFloat();

			// Attributes
		public:

			// Operations
		public:

			// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CEditNumbersFloat)
			//}}AFX_VIRTUAL

			// Implementation
		public:
			virtual ~CEditNumbersFloat();

			// Generated message map functions
		protected:
			//{{AFX_MSG(CEditNumbersFloat)
			afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
			afx_msg void OnSetFocus(CWnd* pOldWnd);
			//}}AFX_MSG

			DECLARE_MESSAGE_MAP()
		};
	} //SVMFCControls
} //Seidenader

namespace SvMc = Seidenader::SVMFCControls;

#endif // !defined(AFX_EDITNUMBERS_H__)

