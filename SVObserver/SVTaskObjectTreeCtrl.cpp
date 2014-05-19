//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTaskObjectTreeCtrlClass
//* .File Name       : $Workfile:   SVTaskObjectTreeCtrl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   15 May 2014 13:13:44  $
//******************************************************************************

#include "stdafx.h"

#include "SVTaskObjectTreeCtrl.h"
#include "SVTaskObjectList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(SVTaskObjectTreeCtrlClass, CTreeCtrl)

SVTaskObjectTreeCtrlClass::SVTaskObjectTreeCtrlClass()
: CTreeCtrl()
{
	pOwnerTaskList = NULL;
}

SVTaskObjectTreeCtrlClass::~SVTaskObjectTreeCtrlClass()
{
}

void SVTaskObjectTreeCtrlClass::SetHasButtonsStyle(void)
{
	LONG style = GetWindowLong(m_hWnd,GWL_STYLE);
	style |= TVS_HASBUTTONS;
	SetWindowLong(m_hWnd,GWL_STYLE,style);
}

void SVTaskObjectTreeCtrlClass::SetNoEditLabelsStyle(void)
{
	LONG style = GetWindowLong(m_hWnd,GWL_STYLE);
	style &= ( ~TVS_EDITLABELS );
	SetWindowLong(m_hWnd,GWL_STYLE,style);
}

BOOL SVTaskObjectTreeCtrlClass::SetTaskObjectList( SVTaskObjectListClass* POwnerTaskList )
{
	if( POwnerTaskList != NULL )
	{
		pOwnerTaskList = POwnerTaskList;
		// Rebuild list...
		Rebuild();

		return TRUE;
	}
	return FALSE;
}

SVTaskObjectClass  * SVTaskObjectTreeCtrlClass::GetSelectedTaskObject()
{
    SVTaskObjectClass * pTaskObject = NULL;

	HTREEITEM hItem = CTreeCtrl::GetSelectedItem( );
	if( hItem )
	{
	    pTaskObject = (SVTaskObjectClass *)GetItemData( hItem );
		if(! SV_IS_KIND_OF( pTaskObject, SVTaskObjectClass ) )
		{
			AfxMessageBox( _T( "Task Object Not Selected" ) );
			return NULL;
		}
	}
	
	return pTaskObject;
}

HTREEITEM SVTaskObjectTreeCtrlClass::InsertTaskItem( HTREEITEM HParent, SVTaskObjectClass* PTask )
{
	HTREEITEM hReturn = NULL;
	if( PTask != NULL && PTask->GetClassID() != SVInvalidGUID )
	{
		if(	SV_IS_KIND_OF( PTask, SVTaskObjectListClass ) )
		{
			// task object item is task object list...	 
			SVTaskObjectListClass* pTaskList = static_cast< SVTaskObjectListClass*>( PTask );
			// ...insert item
			hReturn = InsertItem( pTaskList->GetName(), 0, 0, HParent );
			SetItemData( hReturn, reinterpret_cast<DWORD_PTR>(pTaskList) );

			//...insert list members
			for( int i = 0; i < pTaskList->GetSize(); ++ i )
				InsertTaskItem( hReturn, pTaskList->GetAt( i ) );

			Expand( hReturn, TVE_EXPAND );
		}
		else
		{
			// ...insert item
			hReturn = InsertItem( PTask->GetName(), 0, 0, HParent );

			SetItemData( hReturn, reinterpret_cast<DWORD_PTR>(PTask) );
		}
	}
	return hReturn;
}

void SVTaskObjectTreeCtrlClass::Rebuild()
{
	DeleteAllItems();

	if( pOwnerTaskList != NULL )
	{
		// Insert pOwnerTaskList
		HTREEITEM hRoot = InsertTaskItem( NULL, pOwnerTaskList );
	}
}

BEGIN_MESSAGE_MAP(SVTaskObjectTreeCtrlClass, CTreeCtrl)
	//{{AFX_MSG_MAP(SVTaskObjectTreeCtrlClass)
//SEJ 101	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
//SEJ 101	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
//SEJ 101	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
//SEJ 101	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVTaskObjectTreeCtrlClass message handlers
/* SEJ 101
void SVTaskObjectTreeCtrlClass::OnEditCopy() 
{
	if( pOwnerTaskList != NULL )
	{
		HTREEITEM hSelected = GetSelectedItem();
		SVObjectClass* pSelectedObject;
		if( hSelected && ( pSelectedObject = ( SVObjectClass* ) GetItemData( hSelected ) ) )
		{
			// Open clipboard and remove the current clipboard contents...
			if( OpenClipboard() ) 
			{    
				if( EmptyClipboard() )
				{
					// Get the currently selected data  
					CString script;
					CString alias;
					pSelectedObject->GetObjectScript( script, alias );
					alias += script;

					HGLOBAL hData = GlobalAlloc( GMEM_MOVEABLE,						// allocation attributes
												 ( DWORD ) alias.GetLength() + 2	// number of bytes to allocate
											   );

					LPTSTR tStrMem;
					if( hData && ( tStrMem = ( LPTSTR ) GlobalLock( hData ) ) )
					{
						_tcscpy( tStrMem, alias );
						// Unlock data...
						GlobalUnlock( hData );
					
						// For the appropriate data formats...
						if( ::SetClipboardData( CF_TEXT, hData ) == NULL )  
							AfxMessageBox( "Unable to set Clipboard data" );    
					}
					else
					{
						AfxMessageBox( "Unable to allocate memory" );    
						if( hData )
							GlobalFree( hData );
					}
				}
				CloseClipboard();
			}	
		}
	}
}

void SVTaskObjectTreeCtrlClass::OnEditCut() 
{
	// TODO: Add your command handler code here
}

void SVTaskObjectTreeCtrlClass::OnEditDelete() 
{
	// TODO: Add your command handler code here
}

void SVTaskObjectTreeCtrlClass::OnEditPaste() 
{
	// TODO: Add your command handler code here
}
*/

void SVTaskObjectTreeCtrlClass::OnShowWindow( BOOL bShow, UINT nStatus ) 
{
	CTreeCtrl::OnShowWindow( bShow, nStatus );
	
	//if( bShow )
		//Rebuild();	
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTaskObjectTreeCtrl.cpp_v  $
 * 
 *    Rev 1.1   15 May 2014 13:13:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SetItemData to cast values to DWORD_PTR.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Apr 2013 11:35:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   04 Sep 2012 15:56:36   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  602
   SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed dead code.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   29 Jun 2010 14:08:46   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source to change object validity test and casting changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   01 Jun 2010 11:10:06   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   18 Feb 2005 07:02:34   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Static cast added to fix bug with potential memory corruption in InsertTaskItem(.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   22 Apr 2003 16:36:54   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
 */
