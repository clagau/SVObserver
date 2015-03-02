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
#include "ObjectInterfaces\IClassInfoStructList.h"
#include "SVAvailableFilterListComboBoxClass.h"

namespace Seidenader
{
	namespace SVOGui
	{
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

		void SVAvailableFilterListComboBoxClass::init( SvOi::IClassInfoStructList* L )
		{
			ResetContent();

			int index;
			if( L->Size() <= 0 )
			{
				index = AddString( "(No Filter Available)" );
				SetItemData( index, static_cast<DWORD_PTR>(-1) );
			}
			else
			{
				for( int i = 0; i < L->Size(); ++i )
				{
					index = AddString( L->GetClassName(i) );
					SetItemData( index, static_cast<DWORD_PTR>(i) );
				}
			}

			SetCurSel( 0 );
		}
	}  //end namespace SVOGUI
}  //end namespace Seidenader

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