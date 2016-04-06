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

#pragma once

#pragma region Includes
#include "SVValueObjectImpl.h"
#pragma endregion Includes

class SVEnumerateComboClass : public CComboBox
{

public:
	SVEnumerateComboClass();
	virtual ~SVEnumerateComboClass();

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

protected:
	SVEnumerateValueObjectClass		enumObject;
};

