//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDottedNameTreeCtrl
//* .File Name       : $Workfile:   SVDottedNameTreeCtrl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   21 May 2014 12:21:46  $
//******************************************************************************

#pragma once

namespace SvMc
{
	class SVDottedNameTreeCtrl : public CTreeCtrl
	{
		DECLARE_DYNCREATE(SVDottedNameTreeCtrl)

		// Construction
	public:
		SVDottedNameTreeCtrl();

		// Attributes
	public:
		BOOL		AddItem( LPCTSTR DottedName, DWORD_PTR DwItemValue = 0, BOOL BExpand = true );
		HTREEITEM	GetItem( LPCTSTR DottedName );
		DWORD_PTR	GetSelectedItemValue();

		// Operations
	public:

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(SVDottedNameTreeCtrl)
		//}}AFX_VIRTUAL

		// Implementation
	public:
		virtual ~SVDottedNameTreeCtrl();

		// Generated message map functions
	protected:
		//{{AFX_MSG(SVDottedNameTreeCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()
	};
} //namespace SvMc
