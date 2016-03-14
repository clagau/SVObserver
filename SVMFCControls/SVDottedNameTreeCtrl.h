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

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************

namespace Seidenader
{
	namespace SVMFCControls
	{
		class SVDottedNameTreeCtrlClass : public CTreeCtrl
		{
			DECLARE_DYNCREATE(SVDottedNameTreeCtrlClass)

			// Construction
		public:
			SVDottedNameTreeCtrlClass();

			// Attributes
		public:
			BOOL		AddItem( CString StrDottedItemName, DWORD_PTR DwItemValue = NULL, BOOL BExpand = TRUE );
			HTREEITEM	GetItem( CString StrDottedItemName );
			DWORD_PTR	GetSelectedItemValue();

			// Operations
		public:

			// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(SVDottedNameTreeCtrlClass)
			//}}AFX_VIRTUAL

			// Implementation
		public:
			virtual ~SVDottedNameTreeCtrlClass();

			// Generated message map functions
		protected:
			//{{AFX_MSG(SVDottedNameTreeCtrlClass)
			// NOTE - the ClassWizard will add and remove member functions here.
			//}}AFX_MSG

			DECLARE_MESSAGE_MAP()
		};
	} //SVMFCControls
} //Seidenader

namespace SvMc = Seidenader::SVMFCControls;

