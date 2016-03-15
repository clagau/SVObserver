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

