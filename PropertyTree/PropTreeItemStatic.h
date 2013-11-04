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

#if !defined(AFX_PROPERTYITEMSTATIC_H__22BD9C18_A68C_4BB8_B7FC_C4A7DA0E1EBF__INCLUDED_)
#define AFX_PROPERTYITEMSTATIC_H__22BD9C18_A68C_4BB8_B7FC_C4A7DA0E1EBF__INCLUDED_

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

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYITEMSTATIC_H__22BD9C18_A68C_4BB8_B7FC_C4A7DA0E1EBF__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\PropertyTree\PropTreeItemStatic.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 16:43:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Jan 2013 15:11:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed virtual method GetItemValue(VARIANT&)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 15:42:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of the file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/