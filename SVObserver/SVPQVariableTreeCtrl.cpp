//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPQVariableTreeCtrl
//* .File Name       : $Workfile:   SVPQVariableTreeCtrl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   21 May 2014 17:51:20  $
//******************************************************************************

#include "stdafx.h"
#include "SVPQVariableTreeCtrl.h"
#include "SVObjectLibrary/SVObjectClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(SVPQVariableTreeCtrlClass, SVTreeCtrlClass)

SVPQVariableTreeCtrlClass::SVPQVariableTreeCtrlClass()
: SVTreeCtrlClass()
{
}

SVPQVariableTreeCtrlClass::~SVPQVariableTreeCtrlClass()
{
}

// Move tree item states to the object attributes
BOOL SVPQVariableTreeCtrlClass::UpdateAttributes()
{
	// Iterate the tree and get all items and process.
	HTREEITEM hItem = GetRootItem();

    WalkTheTree( hItem );

	return TRUE;
}// end UpdateAttributes

void SVPQVariableTreeCtrlClass::WalkTheTree( HTREEITEM hItem )
{
    HTREEITEM hItemLast = NULL;

	while( hItem )
	{
		// Process the attributes if any.
		ProcessItemAttributes( hItem );

        hItemLast = hItem;

		hItem = GetChildItem( hItem );
		if( hItem )
		{
			WalkTheTree( hItem );
		}
		ASSERT(hItemLast);

		hItem = GetNextSiblingItem( hItemLast );
	}// end while

}// end WalkTheTree

void SVPQVariableTreeCtrlClass::ProcessItemAttributes( HTREEITEM hItem )
{
	DWORD_PTR dwItemData = GetItemData(hItem);
	SVObjectClass* pObject = reinterpret_cast<SVObjectClass*>(dwItemData);
	if( pObject )
	{
		BOOL bCheckState = GetCheckState(hItem);

		if( pObject )
		{
			if( bCheckState )
			{
				// Set the attributes bits.
				UINT uAttributesSet = pObject->ObjectAttributesSet();
				uAttributesSet |= uRequiredAttributes;
				pObject->ObjectAttributesSetRef() = uAttributesSet;
			}
			else
			{
				// Clear the attributes bits.
				UINT uAttributesSet = pObject->ObjectAttributesSet();
				uAttributesSet &= ~uRequiredAttributes;
				pObject->ObjectAttributesSetRef() = uAttributesSet;
			}// end else
		}// end if
	}// end if
}// end ProcessItemAttributes

BEGIN_MESSAGE_MAP(SVPQVariableTreeCtrlClass, SVTreeCtrlClass)
	//{{AFX_MSG_MAP(SVPQVariableTreeCtrlClass)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

