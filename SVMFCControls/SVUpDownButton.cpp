//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAdjustToolSizePositionDlg
//* .File Name       : $Workfile:   SVUpDownButton.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:55:48  $
//******************************************************************************

#include "stdafx.h"
#include "SVUpDownButton.h"

namespace SvMc
{
	BEGIN_MESSAGE_MAP(SVUpDownButton, CButton)
		ON_WM_LBUTTONDOWN()
		ON_WM_LBUTTONUP()
		ON_WM_KILLFOCUS()
	END_MESSAGE_MAP()

	SVUpDownButton::SVUpDownButton() : m_bLButtonDown(false)
	{
	}

	void SVUpDownButton::OnLButtonDown(UINT nFlags, CPoint point)
	{
		ASSERT(GetParent());
		m_bLButtonDown = true;
		SetFocus();
		CButton::OnLButtonDown(nFlags, point);
		GetParent()->SendMessage(WM_SV_NOTIFY_LBUTTONDOWN, nFlags, (LPARAM) this);
	}

	void SVUpDownButton::OnLButtonUp(UINT nFlags, CPoint point)
	{
		ASSERT(GetParent());
		m_bLButtonDown = false;
		CButton::OnLButtonUp(nFlags, point);
		GetParent()->SendMessage(WM_SV_NOTIFY_LBUTTONUP, nFlags, (LPARAM) this);
	}

	void SVUpDownButton::OnKillFocus(CWnd* pNewWnd)
	{
		if (m_bLButtonDown)
		{
			ASSERT(GetParent());
			CButton::OnKillFocus(pNewWnd);
			GetParent()->SendMessage(WM_SV_NOTIFY_LBUTTONUP, 0, (LPARAM) this);
		}
	}
} //namespace SvMc
