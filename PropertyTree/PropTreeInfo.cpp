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
#include "PropTree.h"
//#include "Resource.h"
#include "PropTreeInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVRPropertyInfo

SVRPropertyInfo::SVRPropertyInfo() :
	m_pProp(NULL)
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
	CRect rc;

	GetClientRect(rc);

	dc.SelectObject(GetSysColorBrush(COLOR_BTNFACE));
	dc.PatBlt(rc.left, rc.top, rc.Width(), rc.Height(), PATCOPY);

	dc.DrawEdge(&rc, BDR_SUNKENOUTER, BF_RECT);
	rc.DeflateRect(4, 4);

	ASSERT(m_pProp!=NULL);

	SVRPropertyItem* pItem = m_pProp->GetFocusedItem();

	if (!m_pProp->IsWindowEnabled())
		dc.SetTextColor(GetSysColor(COLOR_GRAYTEXT));
	else
		dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));

	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(m_pProp->GetBoldFont());

	CString txt;

	if (!pItem)
		txt = _T("No Item Selected.");
	else
		txt = pItem->GetLabelText();

	CRect ir;
	ir = rc;

	// draw label
	dc.DrawText(txt, &ir, DT_SINGLELINE|DT_CALCRECT);
	dc.DrawText(txt, &ir, DT_SINGLELINE);

	ir.top = ir.bottom;
	ir.bottom = rc.bottom;
	ir.right = rc.right;

	if (pItem)
		txt = pItem->GetInfoText();
	else
		txt = _T("Select an item to see its description");

	dc.SelectObject(m_pProp->GetNormalFont());
	dc.DrawText(txt, &ir, DT_WORDBREAK);
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\PropertyTree\PropTreeInfo.cpp_v  $
 * 
 *    Rev 1.0   18 Apr 2013 16:42:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 15:41:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of the files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/