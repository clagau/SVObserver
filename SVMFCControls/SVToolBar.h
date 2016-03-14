//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolBarClass
//* .File Name       : $Workfile:   SVToolBar.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:42:02  $
//******************************************************************************

#if !defined(AFX_SVToolbar_H__3BD1EE4E_079D_11D3_A74F_00106F000C78__INCLUDED_)
#define AFX_SVToolbar_H__3BD1EE4E_079D_11D3_A74F_00106F000C78__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace Seidenader
{
	namespace SVMFCControls
	{
		#define SV_TOOLBAR_BORDER_W 4
		#define SV_TOOLBAR_BORDER_H	4

		class SVToolBarClass : public CToolBarCtrl
		{
			// Construction
		public:
			SVToolBarClass();

			// Attributes
		public:

		private:
			int         m_nButtonCount;
			TBBUTTON    *m_pTBButtons;

			// Operations
		public:

			virtual BOOL Create(	DWORD dwStyle, const RECT& rect, 
				CWnd* pParentWnd, UINT nID, 
				UINT nBaseID, int nCount,const SIZE &size);
			// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(SVToolBarClass)
			//}}AFX_VIRTUAL

			// Implementation
		public:
			virtual ~SVToolBarClass();

			// Generated message map functions
		protected:
			//{{AFX_MSG(SVToolBarClass)
			// NOTE - the ClassWizard will add and remove member functions here.
			//}}AFX_MSG

			DECLARE_MESSAGE_MAP()
		};
	}
}

namespace SvMc = Seidenader::SVMFCControls;

#endif // !defined(AFX_SVToolbar_H__3BD1EE4E_079D_11D3_A74F_00106F000C78__INCLUDED_)

