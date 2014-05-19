//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVAvailableFilterListComboBoxClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   15 May 2014 10:14:10  $
//******************************************************************************

#include "stdafx.h"
#include "SVAvailableFilterListComboBoxClass.h"
#include "SVClassInfoStruct.h"

BEGIN_MESSAGE_MAP(SVAvailableFilterListComboBoxClass, CComboBox)
	//{{AFX_MSG_MAP(SVAvailableFilterListComboBoxClass)
		// HINWEIS - Der Klassen-Assistent fügt hier Zuordnungsmakros ein und entfernt diese.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVAvailableFilterListComboBoxClass::SVAvailableFilterListComboBoxClass()
{
}

SVAvailableFilterListComboBoxClass::~SVAvailableFilterListComboBoxClass()
{
}

void SVAvailableFilterListComboBoxClass::init( SVClassInfoStructListClass* L )
{
	ResetContent();

	int index;
	if( L->GetSize() <= 0 )
	{
		index = AddString( "(No Filter Available)" );
		SetItemData( index, static_cast<DWORD_PTR>(-1) );
	}
	else
	{
		for( int i = 0; i < L->GetSize(); ++i )
		{
			index = AddString( L->GetAt( i ).ClassName );
			SetItemData( index, static_cast<DWORD_PTR>(i) );
		}
	}
	
	SetCurSel( 0 );
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVAvailableFilterListComboBoxClass.cpp_v  $
 * 
 *    Rev 1.1   15 May 2014 10:14:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised SetItemData to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   12 Aug 2013 16:21:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Created from SVOperator.cpp.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/