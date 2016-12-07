//******************************************************************************
//* Copyright (c) 1998-2005
//* All Rights Reserved
//* Author           : Andrew Truckle [ajtruckle@wsatkins.co.uk]
//* License          : The Code Project Open License (CPOL)
//* From             : http://www.codeproject.com/Articles/8/MFC-Grid-control
//******************************************************************************
//* .Module Name     : GridCellNumeric
//* .File Name       : $Workfile:   GridCellNumeric.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Oct 2014 10:32:32  $
//******************************************************************************

// GridCellNumeric.h: interface for the CGridCellNumeric class.
//
// Written by Andrew Truckle [ajtruckle@wsatkins.co.uk]
//
//////////////////////////////////////////////////////////////////////
#pragma once

#pragma region Includes
#include "GridCell.h"
#pragma endregion Includes

namespace Seidenader { namespace GridCtrlLibrary
{
	class CGridCellNumeric : public CGridCell  
	{
		DECLARE_DYNCREATE(CGridCellNumeric)

	public:
		virtual BOOL Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar) override;
		virtual void EndEdit() override;

	};

} /*namespace GridCtrlLibrary*/ } /*namespace Seidenader*/

namespace SvGcl = Seidenader::GridCtrlLibrary;
