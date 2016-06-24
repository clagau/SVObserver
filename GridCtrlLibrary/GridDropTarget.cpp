//******************************************************************************
//* Copyright (c) 1998-2005
//* All Rights Reserved
//* Author           : Chris Maunder <chris@codeproject.com>
//* License          : The Code Project Open License (CPOL)
//* From             : http://www.codeproject.com/Articles/8/MFC-Grid-control
//******************************************************************************
//* .Module Name     : GridDropTarget
//* .File Name       : $Workfile:   GridDropTarget.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Oct 2014 10:35:18  $
//******************************************************************************

// GridDropTarget.cpp : implementation file
//
// MFC Grid Control - Drag/Drop target implementation
//
// CGridDropTarget is an OLE drop target for CGridCtrl. All it does
// is handle the drag and drop windows messages and pass them
// directly onto the grid control.
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
// For use with CGridCtrl v2.10+
//
/////////////////////////////////////////////////////////////////////////////

#pragma region Includes
#include "stdafx.h"
#include "GridCtrl.h"

bool getRidOfLNK4221a = true; //avoids linker warning
#ifndef GRIDCONTROL_NO_DRAGDROP
#include "GridDropTarget.h"
#pragma endregion Includes

#pragma region Declarations

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace Seidenader { namespace GridCtrlLibrary
{
	/////////////////////////////////////////////////////////////////////////////
	// CGridDropTarget

	CGridDropTarget::CGridDropTarget()
	{
		m_pGridCtrl = nullptr;
		m_bRegistered = FALSE;
	}

	CGridDropTarget::~CGridDropTarget()
	{
	}

	// Overloaded Register() function performs the normal COleDropTarget::Register
	// but also serves to connect this COleDropTarget with the parent grid control,
	// where all drop messages will ultimately be forwarded.
	BOOL CGridDropTarget::Register(CGridCtrl *pGridCtrl)
	{
		if (m_bRegistered)
			return FALSE;

		// Stop re-entry problems
		static BOOL bInProcedure = FALSE;
		if (bInProcedure)
			return FALSE;
		bInProcedure = TRUE;

		ASSERT(pGridCtrl->IsKindOf(RUNTIME_CLASS(CGridCtrl)));
		ASSERT(pGridCtrl);

		if (!pGridCtrl || !pGridCtrl->IsKindOf(RUNTIME_CLASS(CGridCtrl)))
		{
			bInProcedure = FALSE;
			return FALSE;
		}

		m_pGridCtrl = pGridCtrl;

		m_bRegistered = COleDropTarget::Register(pGridCtrl);

		bInProcedure = FALSE;
		return m_bRegistered;
	}

	void CGridDropTarget::Revoke()
	{
		m_bRegistered = FALSE;
		COleDropTarget::Revoke();
	}

	BEGIN_MESSAGE_MAP(CGridDropTarget, COleDropTarget)
		//{{AFX_MSG_MAP(CGridDropTarget)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CGridDropTarget message handlers

	DROPEFFECT CGridDropTarget::OnDragScroll(CWnd* pWnd, DWORD dwKeyState, CPoint /*point*/)
	{
		if (pWnd->GetSafeHwnd() == m_pGridCtrl->GetSafeHwnd())
		{
			if (dwKeyState & MK_CONTROL)
				return DROPEFFECT_COPY;
			else
				return DROPEFFECT_MOVE;
		} else
			return DROPEFFECT_NONE;
	}

	DROPEFFECT CGridDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, 
											DWORD dwKeyState, CPoint point)
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_GRIDCTRL)
		TRACE(_T("In CGridDropTarget::OnDragEnter\n"));
#endif
		ASSERT(m_pGridCtrl);

		if (pWnd->GetSafeHwnd() == m_pGridCtrl->GetSafeHwnd())
			return m_pGridCtrl->OnDragEnter(pDataObject, dwKeyState, point);
		else
			return DROPEFFECT_NONE;
	}

	void CGridDropTarget::OnDragLeave(CWnd* pWnd)
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_GRIDCTRL)
		TRACE(_T("In CGridDropTarget::OnDragLeave\n"));
#endif
		ASSERT(m_pGridCtrl);

		if (pWnd->GetSafeHwnd() == m_pGridCtrl->GetSafeHwnd())
			m_pGridCtrl->OnDragLeave();
	}

	DROPEFFECT CGridDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, 
										   DWORD dwKeyState, CPoint point)
	{
		ASSERT(m_pGridCtrl);

		if (pWnd->GetSafeHwnd() == m_pGridCtrl->GetSafeHwnd())
			return m_pGridCtrl->OnDragOver(pDataObject, dwKeyState, point);
		else
			return DROPEFFECT_NONE;
	}

	BOOL CGridDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
								 DROPEFFECT dropEffect, CPoint point)
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_GRIDCTRL)
		TRACE(_T("In CGridDropTarget::OnDrop\n"));
#endif
		ASSERT(m_pGridCtrl);

		if (pWnd->GetSafeHwnd() == m_pGridCtrl->GetSafeHwnd())
			return m_pGridCtrl->OnDrop(pDataObject, dropEffect, point);
		else
			return FALSE;
	}

} /*namespace GridCtrlLibrary*/ } /*namespace Seidenader*/

#endif // GRIDCONTROL_NO_DRAGDROP

