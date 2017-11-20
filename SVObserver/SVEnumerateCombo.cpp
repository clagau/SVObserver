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

#pragma region Includes
#include "stdafx.h"
#include "SVEnumerateCombo.h"

#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


SVEnumerateComboClass::SVEnumerateComboClass()
{
}

SVEnumerateComboClass::~SVEnumerateComboClass()
{
}

bool SVEnumerateComboClass::SetEnumTypes( LPCTSTR szEnumList )
{
	bool Result = enumObject.SetEnumTypes( szEnumList );
	if( Result )
	{
		// Flush combo...
		ResetContent();

		// Populate Combo box...
		for (auto const&rEntry : enumObject.GetEnumVector())
		{
			// Add enumerator to combo list...
			int index = AddString( rEntry.first.c_str() );
			if( index >= 0 )
			{
				SetItemData( index, static_cast<DWORD_PTR> (rEntry.second) );
			}
			else
			{
				Result = false;
			}
		}
	}
	return Result;
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

