//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPQVariableTreeCtrl
//* .File Name       : $Workfile:   SVPQVariableTreeCtrl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 07:12:22  $
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
	DWORD dwItemData = static_cast<DWORD>(GetItemData(hItem));
	SVObjectClass *pObject;

	pObject = (SVObjectClass*) dwItemData;
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVPQVariableTreeCtrl.cpp_v  $
 * 
 *    Rev 1.1   02 Oct 2013 07:12:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:34:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   01 Jun 2010 11:03:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   16 Dec 2009 12:29:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   21 Jun 2005 08:27:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * value object functions now use HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   16 Feb 2005 15:04:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 Apr 2003 14:07:44   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   20 Nov 2002 11:01:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed to accommodate new architecture
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Oct 2001 16:59:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  229
 * SCR Title:  Upgrade SVObserver to Version 3.34 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified source code to remove compiler warning message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 May 2000 17:40:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  141
 * SCR Title:  View DDE Inputs/Outputs on Results View
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added UpdateAttrinbutes method.
 * Added WalkTheTree method.
 * Added ProcessItemAttributes method.
 * Added uRequiredAttributes variable.
 * Added SVPQDataLinkInfoListClass* variable.
 * Revised PopulateTree method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 May 2000 09:33:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  141
 * SCR Title:  View DDE Inputs/Outputs on Results View
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin.
 * Implements a tree control to display/select PPQ data items
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
