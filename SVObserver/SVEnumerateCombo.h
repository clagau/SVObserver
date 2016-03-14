//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEnumerateCombo
//* .File Name       : $Workfile:   SVEnumerateCombo.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:26:34  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#ifndef SVENUMERATECOMBO_H
#define SVENUMERATECOMBO_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// Includes which are necessary to declare or define types, constants and macros
////////////////////////////////////////////////////////////////////////////////
#include "SVValueObjectImpl.h"

//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************


/////////////////////////////////////////////////////////////////////////////
// SVEnumerateComboClass window

class SVEnumerateComboClass : public CComboBox
{

public:
	SVEnumerateComboClass();
	virtual ~SVEnumerateComboClass();

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
	BOOL SetEnumTypes( LPCTSTR szEnumList );

	int SetCurSelItemData( DWORD_PTR dwItemData );
	DWORD_PTR  GetCurSelItemData();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVEnumerateComboClass)
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions
	//{{AFX_MSG(SVEnumerateComboClass)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
	SVEnumerateValueObjectClass		enumObject;
};


/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#endif	//	SVENUMERATECOMBO_H

