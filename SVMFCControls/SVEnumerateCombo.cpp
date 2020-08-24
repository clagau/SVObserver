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

namespace SvMc
{
	BEGIN_MESSAGE_MAP(SVEnumerateCombo, CComboBox)
		//{{AFX_MSG_MAP(SVEnumerateCombo)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	SVEnumerateCombo::SVEnumerateCombo()
	{
	}

	SVEnumerateCombo::~SVEnumerateCombo()
	{
	}

	bool SVEnumerateCombo::SetEnumTypes(const SvOi::NameValueVector& rEnumList)
	{
		bool Result{ true };
	
		m_EnumList = rEnumList;
		// Flush combo...
		ResetContent();
		// Populate Combo box...
		for (auto const&rEntry : m_EnumList)
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
		return Result;
	}

	int  SVEnumerateCombo::SetCurSelItemData( DWORD_PTR ItemData )
	{
		for(int i =0 ; i < GetCount(); i++)
		{
			if(GetItemData(i) == ItemData)
			{
				SetCurSel(i);
				return i;
			}
		}
		return -1;
	}

	DWORD_PTR  SVEnumerateCombo::GetCurSelItemData()
	{
		int sel = GetCurSel();
		if(0 <= sel && GetCount() > sel)
		{
			return GetItemData(sel);
		}
		return 0;
	}
} //namespace SvMc