//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVAvailableAnalyzerListComboBoxClass.h"
#include "SVClassInfoStruct.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(SVAvailableAnalyzerListComboBoxClass, CComboBox)
	//{{AFX_MSG_MAP(SVAvailableAnalyzerListComboBoxClass)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVAvailableAnalyzerListComboBoxClass::SVAvailableAnalyzerListComboBoxClass()
{
}

SVAvailableAnalyzerListComboBoxClass::~SVAvailableAnalyzerListComboBoxClass()
{
}

int SVAvailableAnalyzerListComboBoxClass::init( SVClassInfoStructListClass* pList, int currentSelection )
{
	ResetContent();

	int index;
	int selection = LB_ERR;
	if( pList )
	{
		for( int i = 0; i < pList->GetSize(); ++i )
		{
			index = AddString( pList->GetAt( i ).ClassName );
			SetItemData( index, ( DWORD ) i );

			if (i == currentSelection)
				selection = currentSelection;
		}
	
		if( pList->GetSize() <= 0 )
		{
			index = AddString( _T( "(No Analyzer Available)" ) );
			SetItemData( index, LB_ERR );
		}
		else
		{
			index = AddString( _T( "(No Analyzer)" ) );
			SetItemData( index, LB_ERR );
		}
	
		// The List is sorted - so find the item to be selected
		if( selection == LB_ERR )
		{
			selection = 0;
		}
		else
		{
			for (int i = 0;i < GetCount(); i++ )
			{
				int item = static_cast<int>(GetItemData( i ));
				if( item == currentSelection )
				{
					selection = i;
					break;
				}
			}
		}
		SetCurSel( selection );

		return static_cast<int>(GetItemData( GetCurSel() ));
	}
	return LB_ERR;
}

void SVAvailableAnalyzerListComboBoxClass::add (LPCTSTR ClassName)
{
	long index = AddString( ClassName );
	SetItemData( index, LB_ERR );
	SetCurSel(index);

}

void SVAvailableAnalyzerListComboBoxClass::remove(LPCTSTR ClassName)
{
	int iIndex = FindString(0,ClassName);
	
	if ( iIndex != LB_ERR )
	{
		DeleteString(iIndex);
	}
}

