//******************************************************************************
//* Copyright (c) 1998-2005
//* All Rights Reserved
//* Author           : Chris Maunder <chris@codeproject.com>
//* License          : The Code Project Open License (CPOL)
//* From             : http://www.codeproject.com/Articles/8/MFC-Grid-control
//******************************************************************************
//* .Module Name     : GridCellCheck
//* .File Name       : $Workfile:   GridCellCheck.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Oct 2014 10:32:30  $
//******************************************************************************

/////////////////////////////////////////////////////////////////////////////
// GridCellCheck.h : header file
//
// MFC Grid Control - Grid combo cell class header file
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
// For use with GridCtrl v2.22+
//
//////////////////////////////////////////////////////////////////////

#pragma once

#pragma region Includes
#include "GridCell.h"
#pragma endregion Includes

namespace SvGcl
{
	class GridCellCheck : public GridCell
	{
		friend class GridCtrl;
		DECLARE_DYNCREATE(GridCellCheck)

	public:
		GridCellCheck();

	public:
		BOOL SetCheck(BOOL bChecked = TRUE);
		BOOL GetCheck();

	// Operations
		virtual CSize GetCellExtent(CDC* pDC) override;
		virtual void OnClick( CPoint PointCellRelative) override;
		virtual BOOL GetTextRect( LPRECT pRect) override;

	protected:
		CRect GetCheckPlacement();

		virtual BOOL Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE) override;

	protected:
		BOOL  m_bChecked;
		CRect m_Rect;
	};

} // namespace SvGcl


