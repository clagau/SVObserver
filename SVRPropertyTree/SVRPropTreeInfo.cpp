// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : D:\...\SRC\PropTreeInfo.cpp
// * .File Name       : $Workfile:   PropTreeInfo.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 16:42:02  $
// ******************************************************************************

// PropTreeInfo.cpp : implementation file
//
//  Copyright (C) 1998-2001 Scott Ramsay
//	sramsay@gonavi.com
//	http://www.gonavi.com
//
//  This material is provided "as is", with absolutely no warranty expressed
//  or implied. Any use is at your own risk.
// 
//  Permission to use or copy this software for any purpose is hereby granted 
//  without fee, provided the above notices are retained on all copies.
//  Permission to modify the code and to distribute modified code is granted,
//  provided the above notices are retained, and a notice that the code was
//  modified is included with the above copyright notice.
// 
//	If you use this code, drop me an email.  I'd like to know if you find the code
//	useful.

#include "stdafx.h"
#include "SVRPropTree.h"
//#include "Resource.h"
#include "SVRPropTreeInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVRPropertyInfo

SVRPropertyInfo::SVRPropertyInfo() :
	m_pProp(nullptr)
{
}

SVRPropertyInfo::~SVRPropertyInfo()
{
}


BEGIN_MESSAGE_MAP(SVRPropertyInfo, CStatic)
	//{{AFX_MSG_MAP(SVRPropertyInfo)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVRPropertyInfo message handlers

void SVRPropertyInfo::SetPropOwner(SVRPropTree* pProp)
{
	m_pProp = pProp;
}

void SVRPropertyInfo::OnPaint() 
{
	CPaintDC dc(this);
	CRect ClientRect;

	GetClientRect(ClientRect);

	dc.SelectObject(GetSysColorBrush(COLOR_BTNFACE));
	dc.PatBlt(ClientRect.left, ClientRect.top, ClientRect.Width(), ClientRect.Height(), PATCOPY);

	dc.DrawEdge(&ClientRect, BDR_SUNKENOUTER, BF_RECT);
	ClientRect.DeflateRect(4, 4);

	ASSERT(nullptr != m_pProp);

	SVRPropertyItem* pItem = m_pProp->GetFocusedItem();

	if (!m_pProp->IsWindowEnabled())
		dc.SetTextColor(GetSysColor(COLOR_GRAYTEXT));
	else
		dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));

	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(m_pProp->GetBoldFont());

	SVString Text;

	if( nullptr != pItem)
	{
		Text = pItem->GetLabelText();
	}
	else
	{
		Text = _T("No Item Selected.");
	}

	CRect DrawRect;
	DrawRect = ClientRect;

	// draw label
	dc.DrawText( Text.c_str(), &DrawRect, DT_SINGLELINE|DT_CALCRECT );
	dc.DrawText( Text.c_str(), &DrawRect, DT_SINGLELINE );

	DrawRect.top = DrawRect.bottom;
	DrawRect.bottom = ClientRect.bottom;
	DrawRect.right = ClientRect.right;

	if(nullptr != pItem)
	{
		Text = pItem->GetInfoText();
	}
	else
	{
		Text = _T("Select an item to see its description");
	}

	dc.SelectObject(m_pProp->GetNormalFont());
	dc.DrawText(Text.c_str(), &DrawRect, DT_WORDBREAK);
}

