//******************************************************************************
//* Copyright (c) 1998-2005
//* All Rights Reserved
//* Author           : Chris Maunder <chris@codeproject.com>
//* License          : The Code Project Open License (CPOL)
//* From             : http://www.codeproject.com/Articles/8/MFC-Grid-control
//******************************************************************************
//* .Module Name     : GridDropTarget
//* .File Name       : $Workfile:   GridDropTarget.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Oct 2014 10:32:32  $
//******************************************************************************

//////////////////////////////////////////////////////////////////////
// GridDropTarget.h : header file
//
// MFC Grid Control - Drag/Drop target implementation
//
// Written by Chris Maunder <chris@codeproject.com>
// Copyright (c) 1998-2005. All Rights Reserved.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name and all copyright 
// notices remains intact. 
//
// An email letting me know how you are using it would be nice as well. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// For use with GridCtrl v2.10+
//
//////////////////////////////////////////////////////////////////////

#pragma once

//Moved to precompiled header:  #include <afxole.h>

namespace SvGcl
{
	class GridCtrl;

	class GridDropTarget : public COleDropTarget
	{
	public:
		GridDropTarget();
		virtual ~GridDropTarget();

	// Attributes
	public:
		GridCtrl* m_pGridCtrl;
		BOOL       m_bRegistered;

	// Operations
	public:
		BOOL Register(GridCtrl *pGridCtrl);
		virtual void Revoke();

		BOOL        OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
		DROPEFFECT  OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
		void        OnDragLeave(CWnd* pWnd);
		DROPEFFECT  OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
		DROPEFFECT  OnDragScroll(CWnd* pWnd, DWORD dwKeyState, CPoint point);

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(GridDropTarget)
		//}}AFX_VIRTUAL

	// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(GridDropTarget)
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()
	};

} // namespace SvGcl


