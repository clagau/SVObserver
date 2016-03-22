// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : PropertyItemStatic.h
// * .File Name       : $Workfile:   PropTreeItemStatic.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 16:43:18  $
// ******************************************************************************

// PropertyItemStatic.h
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
#pragma once

#include "PropTreeItem.h"

class SVRPropertyItemStatic : public SVRPropertyItem
{
public:
	SVRPropertyItemStatic();
	virtual ~SVRPropertyItemStatic();

public:
	// The attribute area needs drawing
	virtual void DrawAttribute(CDC* pDC, const RECT& rc);

	// Retrieve the item's attribute value (in this case the CString)
	virtual bool GetItemValue(CString& strVal);
    //virtual bool GetItemValue(VARIANT& vtVal);

	// Set the item's attribute value
	virtual bool SetItemValue(LPCTSTR lpszVal);

protected:
	CString		m_sAttribute;
};

