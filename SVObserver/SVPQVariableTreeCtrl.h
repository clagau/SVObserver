//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPQVariableTreeCtrl
//* .File Name       : $Workfile:   SVPQVariableTreeCtrl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:34:28  $
//******************************************************************************

#if !defined(AFX_SVPQVariableTreeCtrl_H__A4C49D6B_24DA_11d4_A8F2_00106F030D16__INCLUDED_)
#define AFX_SVPQVariableTreeCtrl_H__A4C49D6B_24DA_11d4_A8F2_00106F030D16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVMFCControls/SVTreeCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// SVPQVariableTreeCtrlClass window

class SVPQVariableTreeCtrlClass : public SvMc::SVTreeCtrlClass
{
	DECLARE_DYNCREATE(SVPQVariableTreeCtrlClass)

// Construction
public:
	SVPQVariableTreeCtrlClass();

// Attributes
	DWORD uRequiredAttributes;

protected:

// Operations
public:
	BOOL UpdateAttributes();

protected:
	
	void WalkTheTree( HTREEITEM hItem );
	void ProcessItemAttributes( HTREEITEM hItem );

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVPQVariableTreeCtrlClass)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~SVPQVariableTreeCtrlClass();

	// Generated message map functions
protected:
	//{{AFX_MSG(SVPQVariableTreeCtrlClass)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVPQVariableTreeCtrl_H__A4C49D6B_24DA_11d4_A8F2_00106F030D16__INCLUDED_)

