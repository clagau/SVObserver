//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolBar
//* .File Name       : $Workfile:   SVToolBar.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:41:48  $
//******************************************************************************

#include "stdafx.h"
#include "SVToolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvMc
{
	BEGIN_MESSAGE_MAP(SVToolBar, CToolBarCtrl)
		//{{AFX_MSG_MAP(SVToolBar)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	SVToolBar::SVToolBar()
	{
		m_pTBButtons = nullptr;
	}

	SVToolBar::~SVToolBar()
	{
		if (m_pTBButtons)
			delete []m_pTBButtons;
	}


	BOOL SVToolBar::Create(	DWORD dwStyle, const RECT& rect, 
		CWnd* pParentWnd, UINT nID, UINT nBaseID,
		int nCount, const SIZE& size )
	{
		int nIndex( 0 );

		BOOL bRet = CToolBarCtrl::Create(	dwStyle, rect, 
			pParentWnd, nID);

		m_nButtonCount = nCount;

		CSize bSize = size;
		CToolBarCtrl::SetBitmapSize(size);
		bSize.cx += SV_TOOLBAR_BORDER_W;
		bSize.cy += SV_TOOLBAR_BORDER_H;
		CToolBarCtrl::SetButtonSize(bSize);

		VERIFY(AddBitmap(m_nButtonCount,nID) != -1);

		m_pTBButtons = new TBBUTTON[m_nButtonCount];

		for (nIndex = 0; nIndex < m_nButtonCount; nIndex++)
		{
			m_pTBButtons[nIndex].fsState = TBSTATE_ENABLED;
			m_pTBButtons[nIndex].fsStyle = TBSTYLE_BUTTON;
			m_pTBButtons[nIndex].dwData = 0;
			m_pTBButtons[nIndex].iBitmap = nIndex;
			m_pTBButtons[nIndex].idCommand = nIndex + nBaseID;
			m_pTBButtons[nIndex].iString = 0;
		}

		for (nIndex = 0; nIndex < m_nButtonCount; nIndex++)
		{
			VERIFY(AddButtons(1,&m_pTBButtons[nIndex]));
		}

		return bRet;
	}
} //namespace SvMc
