// ******************************************************************************
// * COPYRIGHT (c) 2013 by Seidenader Vision, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVPLCListCtrl
// * .File Name       : $Workfile:   SVPLCListCtrl.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 13:25:30  $
// ******************************************************************************

#pragma once
#include "SVMFCControls\svlistctrl.h"

class SVPLCListCtrl :
	public SvMc::SVListCtrl
{
public:
	void OnEditItem(int iItem, int iSubItem, CPoint point, UINT nChar);
	SVPLCListCtrl(void);
	~SVPLCListCtrl(void);
};

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPLCListCtrl.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:25:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/