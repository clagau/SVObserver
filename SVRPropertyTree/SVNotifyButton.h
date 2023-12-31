// ******************************************************************************
// * COPYRIGHT (c) 2000 by K�rber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVNotifyButton
// * .File Name       : $Workfile:   SVNotifyButton.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 16:43:20  $
// ******************************************************************************

#pragma once

class SVNotifyButton : public CButton
{
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus( CWnd* pNewWnd );
	DECLARE_MESSAGE_MAP()
};

