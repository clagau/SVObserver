//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTreeCtrl
//* .File Name       : $Workfile:   SVTreeCtrl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 12:27:42  $
//******************************************************************************

#pragma once

namespace SvMc
{
	class SVTreeCtrlClass : public CTreeCtrl
	{
		DECLARE_DYNCREATE(SVTreeCtrlClass)

		// Construction
	public:
		SVTreeCtrlClass();
		BOOL InitTreeCtrl();

		// Attributes

	protected:
		CImageList StateImageList;

		// Operations
	public:
		BOOL GetCheckState(	HTREEITEM hItem	);

		BOOL SetCheckState(	HTREEITEM hItem, BOOL fCheck );

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(SVTreeCtrlClass)
		//}}AFX_VIRTUAL

		// Implementation
	public:
		virtual ~SVTreeCtrlClass();

		// Generated message map functions
	protected:
		//{{AFX_MSG(SVTreeCtrlClass)
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()
	};
} //namespace SvMc
