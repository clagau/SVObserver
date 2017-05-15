//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEditHelper
//* .File Name       : $Workfile:   SVEditHelper.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 14:12:22  $
//******************************************************************************

#include "stdafx.h"
#include "SVEditHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvMc
{
	SVEditHelper::SVEditHelper()
	{
	}

	SVEditHelper::~SVEditHelper()
	{
	}

	BEGIN_MESSAGE_MAP(SVEditHelper, CEdit)
		//{{AFX_MSG_MAP(SVEditHelper)
		ON_WM_KEYUP()
		ON_WM_KILLFOCUS()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// SVEditHelper message handlers

	void SVEditHelper::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
	{
		switch( nChar )
		{
		case VK_RETURN:
			GetParent()->SendMessage( WM_APP_ED_EDIT_FINISHED, (WPARAM)TRUE );	// Commit changes
			break;

		case VK_ESCAPE:
			GetParent()->SendMessage( WM_APP_ED_EDIT_FINISHED, (WPARAM)FALSE );	// Disregard changes
			break;
		}

		CEdit::OnKeyUp(nChar, nRepCnt, nFlags);
	}

	void SVEditHelper::OnKillFocus(CWnd* pNewWnd) 
	{
		GetParent()->SendMessage( WM_APP_ED_EDIT_FINISHED, (WPARAM)TRUE );	// Commit changes

		CEdit::OnKillFocus(pNewWnd);
	}


	BOOL SVEditHelper::PreTranslateMessage(MSG* pMsg)
	{
		if( pMsg->message == WM_KEYDOWN )
		{
			UINT nChar = static_cast<UINT>(pMsg->wParam);
			if (nChar == VK_ESCAPE || nChar == VK_RETURN || nChar == VK_TAB)
			{
				GetParent()->SetFocus();
				return TRUE;
			}
		}

		return CEdit::PreTranslateMessage(pMsg);
	}
} //namespace SvMc
