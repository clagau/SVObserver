//******************************************************************************
//* Copyright (c) 1998-2005
//* All Rights Reserved
//* Author           : Andrew Truckle [ajtruckle@wsatkins.co.uk]
//* License          : The Code Project Open License (CPOL)
//* From             : http://www.codeproject.com/Articles/8/MFC-Grid-control
//******************************************************************************
//* .Module Name     : GridCellNumeric
//* .File Name       : $Workfile:   GridCellNumeric.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Oct 2014 10:35:16  $
//******************************************************************************

// GridCellNumeric.cpp: implementation of the GridCellNumeric class.
//
// Written by Andrew Truckle [ajtruckle@wsatkins.co.uk]
//
//////////////////////////////////////////////////////////////////////

#pragma region Includes
#include "stdafx.h"
#include "GridCtrl.h"
#include "InPlaceEdit.h"
#include "GridCellNumeric.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace SvGcl
{
	IMPLEMENT_DYNCREATE(GridCellNumeric, GridCell)

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	// Create a control to do the editing
	BOOL GridCellNumeric::Edit(int nRow, int nCol, CRect rect, CPoint /* point */, UINT nID, UINT nChar)
	{
		m_bEditing = TRUE;
    
		// InPlaceEdit auto-deletes itself
		m_pEditWnd = new InPlaceEdit(GetGrid(), rect, /*GetStyle() |*/ ES_NUMBER, nID, nRow, nCol,
			GetText(), nChar);

		return TRUE;
	}

	// Cancel the editing.
	void GridCellNumeric::EndEdit()
	{
		if (m_pEditWnd)
			((InPlaceEdit*)m_pEditWnd)->EndEdit();
	}

} // namespace SvGcl
