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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVEditNumbers.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:26:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   22 Apr 2003 09:17:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:29:46   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 23 1999 13:54:42   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  37
 * SCR Title:  Version 3.00 Beta 3 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   New classes specific to this version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/