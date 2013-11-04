// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVNotifyButton
// * .File Name       : $Workfile:   SVNotifyButton.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 16:43:20  $
// ******************************************************************************

#pragma once
#ifndef __NOTIFYBUTTON_H__
#define __NOTIFYBUTTON_H__

class SVNotifyButton : public CButton
{
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus( CWnd* pNewWnd );
	DECLARE_MESSAGE_MAP()
};


#endif	//#ifndef __NOTIFYBUTTON_H__

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\PropertyTree\SVNotifyButton.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 16:43:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   13 Aug 2003 13:28:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration - moved from PropTreeItemCombo
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/