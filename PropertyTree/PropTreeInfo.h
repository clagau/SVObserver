// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
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

#if !defined(AFX_PROPERTYINFO_H__22BD9C18_A68C_4BB8_B7FC_C4A7DA0E1EBF__INCLUDED_)
#define AFX_PROPERTYINFO_H__22BD9C18_A68C_4BB8_B7FC_C4A7DA0E1EBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
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

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYINFO_H__22BD9C18_A68C_4BB8_B7FC_C4A7DA0E1EBF__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\PropertyTree\PropTreeInfo.h_v  $
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
 *    Rev 1.0   15 Nov 2002 15:42:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of the file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/