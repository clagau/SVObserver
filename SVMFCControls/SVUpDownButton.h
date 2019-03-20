//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUpDownButton
//* .File Name       : $Workfile:   SVUpDownButton.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:56:00  $
//******************************************************************************

#pragma once

namespace SvMc
{
	enum
	{
		WM_SV_NOTIFY_LBUTTONDOWN = WM_APP + 200,
		WM_SV_NOTIFY_LBUTTONUP   = WM_APP + 201,
	};

	class SVUpDownButton : public CButton
	{
	public:
		SVUpDownButton();
		afx_msg void OnLButtonDown( UINT nFlags, CPoint point );
		afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
		afx_msg void OnKillFocus( CWnd* pNewWnd );

	protected:
		bool m_bLButtonDown;

		DECLARE_MESSAGE_MAP()
	};
} //namespace SvMc
