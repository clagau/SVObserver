//******************************************************************************
/// \copyright COPYRIGHT (c) 2015 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \Author: Steve Jones
//******************************************************************************

#include "stdafx.h"
#include "SVSliderCtrl.h"

namespace SvMc
{
	BEGIN_MESSAGE_MAP(SVSliderCtrl, CSliderCtrl)
		//{{AFX_MSG_MAP(SVSliderCtrl)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()


	SVSliderCtrl::SVSliderCtrl()
		: m_bInvertVerticalArrowKeys(false)
		, m_isClicked(false)
	{
	}

	SVSliderCtrl::~SVSliderCtrl()
	{
	}

	void SVSliderCtrl::SetInvertVerticalArrowKeys(bool bInvert)
	{
		m_bInvertVerticalArrowKeys = bInvert;
	}

	BOOL SVSliderCtrl::PreTranslateMessage(MSG* pMsg)
	{
		if (m_bInvertVerticalArrowKeys)
		{
			if (pMsg->message == WM_KEYDOWN)
			{
				if (pMsg->wParam == VK_UP)
				{
					pMsg->wParam = VK_DOWN;
				}
				else if (pMsg->wParam == VK_DOWN)
				{
					pMsg->wParam = VK_UP;
				}
			}
		}
		return CSliderCtrl::PreTranslateMessage(pMsg);
	}
} //namespace SvMc
