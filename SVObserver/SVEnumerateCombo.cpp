//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEnumerateCombo
//* .File Name       : $Workfile:   SVEnumerateCombo.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:26:24  $
//******************************************************************************

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// General Include File(s)
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

////////////////////////////////////////////////////////////////////////////////
// Other Necessary Include File(s) - Module Link(s)
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Prototyping
////////////////////////////////////////////////////////////////////////////////

#include "SVEnumerateCombo.h"

////////////////////////////////////////////////////////////////////////////////
// Other Necessary Include File(s) - Module Link(s)
////////////////////////////////////////////////////////////////////////////////


//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************




//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVEnumerateComboClass
//* Note(s)    : CComboBox
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

SVEnumerateComboClass::SVEnumerateComboClass()
{
}

SVEnumerateComboClass::~SVEnumerateComboClass()
{
}

BOOL SVEnumerateComboClass::SetEnumTypes( LPCTSTR szEnumList )
{
	BOOL bRetVal = enumObject.SetEnumTypes( szEnumList );
	if( bRetVal )
	{
		// Flush combo...
		ResetContent();

		// Populate Combo box...
		CString strEnum;
		long lValue = 0L;
		int it = enumObject.GetFirstEnumTypePos();
		while( enumObject.GetNextEnumType( it, strEnum, lValue ) )
		{
			// Add enumerator to combo list...
			int index = AddString( strEnum );
			if( index >= 0 )
			{
				SetItemData( index, ( DWORD ) lValue );
			}
			else
				bRetVal = FALSE;
		}
	}
	return bRetVal;
}

int  SVEnumerateComboClass::SetCurSelItemData( DWORD_PTR ItemData )
{
	for(int i =0 ; i < GetCount(); i++)
	{
		if(GetItemData(i)== ItemData)
		{
			SetCurSel(i);
			return i;
		}
	}
	return -1;
}

DWORD_PTR  SVEnumerateComboClass::GetCurSelItemData()
{
	int sel = GetCurSel();
	if(sel >0 && sel <= GetCount())
	{
		return GetItemData(sel);
	}
	return -1;
}


BEGIN_MESSAGE_MAP(SVEnumerateComboClass, CComboBox)
	//{{AFX_MSG_MAP(SVEnumerateComboClass)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

