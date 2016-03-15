// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVNumericEdit
// * .File Name       : $Workfile:   SVNumericEdit.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 12:37:40  $
// ******************************************************************************

#if !defined(AFX_SVNUMERICEDIT_H_)
#define AFX_SVNUMERICEDIT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace Seidenader
{
	namespace SVMFCControls
	{
		class CSVNumericEdit : public CEdit
		{
			// Construction
		public:
			CSVNumericEdit();

			// Attributes
		public:

			// Operations
		public:

			// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CSVNumericEdit)
			//}}AFX_VIRTUAL

			// Implementation
		public:
			virtual ~CSVNumericEdit();

			// Generated message map functions
		protected:
			//{{AFX_MSG(CSVNumericEdit)
			afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
			//}}AFX_MSG

		private:
			CString m_sExcludedChars;
			CString m_sOrgExcludedChars;

			DECLARE_MESSAGE_MAP()
		};
	} //SVMFCControls
} //Seidenader


namespace SvMc = Seidenader::SVMFCControls;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVOCONFIGNAMEEDIT_H__CA45AA39_C2A2_4DED_918E_07A55DB442FA__INCLUDED_)

