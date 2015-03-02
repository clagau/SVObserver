//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVFilterListBoxClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   15 May 2014 12:40:50  $
//******************************************************************************

#include "stdafx.h"
#include "ObjectInterfaces\ITaskObjectListClass.h"
#include "SVFilterListBoxClass.h"

namespace Seidenader
{
	namespace SVOGui
	{
		BEGIN_MESSAGE_MAP(SVFilterListBoxClass, CListBox)
			//{{AFX_MSG_MAP(SVFilterListBoxClass)
			ON_WM_CREATE()
			//}}AFX_MSG_MAP
		END_MESSAGE_MAP()

		SVFilterListBoxClass::SVFilterListBoxClass()
		{
		}

		SVFilterListBoxClass::~SVFilterListBoxClass()
		{
		}

		void SVFilterListBoxClass::init( SvOi::ITaskObjectListClass* List )
		{
			int index;

			ResetContent();

			for( int i = 0;i < List->GetSize(); ++i )
			{
				SvOi::IObjectClass *pOperator = List->GetInterfaceAt( i );

				if( pOperator && pOperator->GetObjectType() == SVFilterObjectType ) // Only Filter!
				{
					index = AddString( pOperator->GetName() );
					SetItemData( index, reinterpret_cast<DWORD_PTR>(pOperator) );
				}
			}

			if( GetCount() <= 0 )
			{
				SetItemData( AddString( _T( "(No Filter)" ) ), static_cast<DWORD_PTR>(NULL) );
				index = 0;
			}
			else
			{
				index = AddString( _T( "--------------------------------------------" ) );
				SetItemData( index, static_cast<DWORD_PTR>(NULL) );
			}

			if( index >= GetCount() )
				index = GetCount() - 1;

			if( index == LB_ERR ) // First init, but existing Filter
				index = 0;

			SetCurSel( index );
		}

		BOOL SVFilterListBoxClass::PreCreateWindow(CREATESTRUCT& cs) 
		{
			cs.style ^= LBS_SORT;
			return CListBox::PreCreateWindow(cs);
		}

		int SVFilterListBoxClass::OnCreate(LPCREATESTRUCT lpCreateStruct) 
		{
			if (CListBox::OnCreate(lpCreateStruct) == -1)
				return -1;

			lpCreateStruct->style ^= LBS_SORT;


			return 0;
		}
	}  //end namespace SVOGUI
}  //end namespace Seidenader

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVFilterListBoxClass.cpp_v  $
 * 
 *    Rev 1.1   15 May 2014 12:40:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised SetItemData to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   12 Aug 2013 16:22:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Created from SVOperator.cpp.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
