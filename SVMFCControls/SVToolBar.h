//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolBar
//* .File Name       : $Workfile:   SVToolBar.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:42:02  $
//******************************************************************************

#pragma once

namespace SvMc
{
	#define SV_TOOLBAR_BORDER_W 4
	#define SV_TOOLBAR_BORDER_H	4

	class SVToolBar : public CToolBarCtrl
	{
		// Construction
	public:
		SVToolBar();

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
		//{{AFX_VIRTUAL(SVToolBar)
		//}}AFX_VIRTUAL

		// Implementation
	public:
		virtual ~SVToolBar();

		// Generated message map functions
	protected:
		//{{AFX_MSG(SVToolBar)
		// NOTE - the ClassWizard will add and remove member functions here.
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()
	};
} //namespace SvMc
