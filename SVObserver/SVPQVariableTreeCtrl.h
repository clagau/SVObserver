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

#pragma once
#pragma region Includes
#include "SVMFCControls/SVTreeCtrl.h"
#pragma endregion Includes

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

