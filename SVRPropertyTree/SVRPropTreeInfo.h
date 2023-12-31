// ******************************************************************************
// * COPYRIGHT (c) 2000 by K�rber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : PropertyInfo.h
// * .File Name       : $Workfile:   PropTreeInfo.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 16:42:02  $
// ******************************************************************************

// PropertyInfo.h : header file
//
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

class SVRPropTree;

/////////////////////////////////////////////////////////////////////////////
// SVRPropertyInfo window

class SVRPropertyInfo : public CStatic
{
// Construction
public:
	SVRPropertyInfo();

// Attributes
public:
	// SVRPropTree class that this class belongs
	void SetPropOwner(SVRPropTree* pProp);

protected:
	SVRPropTree*		m_pProp;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVRPropertyInfo)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~SVRPropertyInfo();

	// Generated message map functions
protected:
	//{{AFX_MSG(SVRPropertyInfo)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

