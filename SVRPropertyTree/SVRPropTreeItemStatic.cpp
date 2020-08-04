// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : PropTreeItemStatic.cpp
// * .File Name       : $Workfile:   PropTreeItemStatic.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 16:43:18  $
// ******************************************************************************

// PropTreeItemStatic.cpp
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

#include "SVRPropTreeItemStatic.h"


SVRPropertyItemStatic::SVRPropertyItemStatic()
{
}


SVRPropertyItemStatic::~SVRPropertyItemStatic()
{
}


void SVRPropertyItemStatic::DrawAttribute(CDC* pDC, const RECT& rRect)
{
	assert(nullptr != m_pProp);
	CFont*	l_pOldFont;


//- JAB110708 - GetNormalFont is a static function getting a static member.
	l_pOldFont = pDC->SelectObject(m_pProp->GetNormalFont());
	pDC->SetTextColor(RGB(0,0,0));
	pDC->SetBkMode(TRANSPARENT);

	CRect DrawRect( rRect );
	pDC->DrawText(m_Attribute.c_str(), DrawRect, DT_SINGLELINE|DT_VCENTER);

//- JAB110708
	pDC->SelectObject(l_pOldFont);
}


bool SVRPropertyItemStatic::GetItemValue(std::string& rValue)
{
	rValue = m_Attribute;
	return true;
}


bool SVRPropertyItemStatic::SetItemValue(LPCTSTR lpszVal)
{
	m_Attribute = lpszVal;
	return true;
}

