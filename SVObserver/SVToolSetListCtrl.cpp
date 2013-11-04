//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolSetListCtrl
//* .File Name       : $Workfile:   SVToolSetListCtrl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 May 2013 16:29:44  $
//******************************************************************************

#include "stdafx.h"
#include "SVToolSetListCtrl.h"

#include "SVTool.h"
#include "SVTaskObjectList.h"
#include "SVToolSetTabView.h"
#include "SVObserver.h"
#include "SVIPDoc.h"
#include "SVSVIMStateClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVToolSetListCtrlClass

IMPLEMENT_DYNCREATE(SVToolSetListCtrlClass, SVTaskObjectListCtrlClass)

SVToolSetListCtrlClass::SVToolSetListCtrlClass()
: SVTaskObjectListCtrlClass()
{
	m_iNone = -1;
	m_iInvalid = -1;
	m_iDisabled = -1;
	m_iPass = -1;
	m_iFail = -1;
	m_iWarn = -1;
	m_iUnknown = -1;
	m_iTopIndex = 0;
}

SVToolSetListCtrlClass::~SVToolSetListCtrlClass()
{
}

void SVToolSetListCtrlClass::Rebuild()
{
	if( m_oStateImageList.GetSafeHandle() == NULL )
	{
		CreateImageLists();
	}

	DeleteAllItems();
	
	if( pOwnerTaskList != NULL )
	{
		if( pOwnerTaskList->GetSize() <= 0 )
		{
			CString strEmpty;
			strEmpty.LoadString( IDS_EMPTY_STRING );
			InsertItem( LVIF_TEXT | LVIF_STATE,
				0, strEmpty,
				INDEXTOSTATEIMAGEMASK( m_iNone + 1 ),	// state
				LVIS_STATEIMAGEMASK,		// stateMask
				0, 0 );						// Set item data to NULL
			return;
		}
		
		int itemNo = 0;
		int l_iIndex = -1;
		
		int l_iCount = pOwnerTaskList->GetSize();
		
		for( int i = 0; i < l_iCount; i++ )
		{
			SVToolClass *l_psvTool = NULL;

			try
			{
				l_psvTool = dynamic_cast<SVToolClass *>( pOwnerTaskList->GetAt( i ) );
			}
			catch( ... )
			{
				l_psvTool = NULL;
			}
			
			if( l_psvTool )
			{
				if( ! l_psvTool->IsValid() )
				{
					l_iIndex = InsertItem( LVIF_TEXT | LVIF_STATE,
						itemNo, l_psvTool->GetName(),
						INDEXTOSTATEIMAGEMASK( m_iInvalid + 1 ),	// state
						LVIS_STATEIMAGEMASK,		// stateMask
						0, 0 );						// Set item data to NULL
				}
				else
				{
					l_iIndex = InsertItem( LVIF_TEXT | LVIF_STATE,
						itemNo, l_psvTool->GetName(),
						INDEXTOSTATEIMAGEMASK( m_iNone + 1 ),	// state
						LVIS_STATEIMAGEMASK,		// stateMask
						0, 0 );						// Set item data to NULL
				}
				
				SetItemData( l_iIndex, (DWORD) l_psvTool );
				
				itemNo++;
			}
		}
		
		l_iIndex = InsertItem( LVIF_TEXT | LVIF_STATE,
			itemNo, _T( "-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------" ),
			INDEXTOSTATEIMAGEMASK( m_iNone + 1 ),	// state
			LVIS_STATEIMAGEMASK,		// stateMask
			0, 0 );						// Set item data to NULL
		
		SetItemData( l_iIndex, (DWORD) NULL );
	}
	
	RebuildImages();
}

void SVToolSetListCtrlClass::RebuildImages()
{
	if( m_oStateImageList.GetSafeHandle() == NULL )
	{
		CreateImageLists();
	}

	int l_iCount = GetItemCount();

	for( int i = 0; i < l_iCount; i++ )
	{
		SVToolClass *l_psvTool = NULL;

		try
		{
			l_psvTool = dynamic_cast<SVToolClass *>( reinterpret_cast<SVTaskObjectListClass*>( GetItemData( i ) ) );
		}
		catch( ... )
		{
			l_psvTool = NULL;
		}

		if( l_psvTool != NULL )
		{
			if( ! l_psvTool->IsValid() )
			{
				SetItemState( i, INDEXTOSTATEIMAGEMASK( m_iInvalid + 1 ),	LVIS_STATEIMAGEMASK );
			}
			else
			{
				SetItemState( i, INDEXTOSTATEIMAGEMASK( m_iNone + 1 ),	LVIS_STATEIMAGEMASK );
			}
		}
		else
		{
			if( GetItemData( i ) != NULL )
			{
				SetItemState( i, INDEXTOSTATEIMAGEMASK( m_iInvalid + 1 ),	LVIS_STATEIMAGEMASK );
			}
			else
			{
				SetItemState( i, INDEXTOSTATEIMAGEMASK( m_iNone + 1 ),	LVIS_STATEIMAGEMASK );
			}
		}
	}
}

SVGUID SVToolSetListCtrlClass::GetCurrentSelectionID()
{
	SVGUID l_Result;

	int cur = GetNextItem( -1, LVNI_SELECTED );
	if (cur != -1)
	{
		if ( !IsValidSelection( cur ) )
		{
			cur = -1;
		}
	}

	// Translate list index to index into ToolSet TaskObjectList - SEJ
	if( cur != -1 )
	{
		LVITEM item;

		memset( &item, '\0', sizeof( LVITEM ) );
		item.mask = LVIF_PARAM;
		item.iItem = cur;

		if( GetItem( &item ) )
		{
			SVTaskObjectClass* pObject = ( SVTaskObjectClass * )( item.lParam );

			if( pObject != NULL )
			{
				l_Result = pObject->GetUniqueObjectID();
			}
		}
	}

	return l_Result;
}

int SVToolSetListCtrlClass::GetCurrentSelection( int StartAt /* = -1 -> start searching at top */ )
{
	int cur = GetNextItem( StartAt, LVNI_SELECTED );
	if (cur != -1)
	{
		if ( !IsValidSelection( cur ) )
		{
			cur = -1;
		}
	}
	// Translate list index to index into ToolSet TaskObjectList - SEJ
	if( cur != -1 )
	{
		LVITEM item;

		memset( &item, '\0', sizeof( LVITEM ) );
		item.mask = LVIF_PARAM;
		item.iItem = cur;

		if( GetItem( &item ) )
		{
			SVTaskObjectClass* pObject = ( SVTaskObjectClass * )( item.lParam );
			SVTaskObjectListClass* pOwner = ( SVTaskObjectListClass * )pObject->GetOwner();
			if( pOwner )
			{
				for( int i = 0;i < pOwner->GetSize();i++ )
				{
					if( pObject == pOwner->GetAt( i ) )
					{
						cur = i;
						break;
					}
				}
			}
			else
				cur = -1;
		}
	}
	return cur;
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
//
int SVToolSetListCtrlClass::SetCurrentSelection( int nIndex /*= -1 to deselect*/ )
{
    BOOL bResult;
    bResult = SetItemState(
        nIndex,          // int nItem, 
        LVIS_SELECTED,   // UINT nState, 
        LVIS_SELECTED    // UINT nMask 
    );

	Invalidate();           // cause WM_PAINT to refresh list control.

    int nSelected = GetCurrentSelection();
    return nSelected;
}

// OnKeyDown added to handle down arrow and up arrow, tool selection and scroll bars. 
void SVToolSetListCtrlClass::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool l_bUpdate = false;
	int l_iNext = 0;
	int l_iSelected = GetNextItem( -1, LVNI_SELECTED ) ;

	if( nChar == VK_DOWN)
	{
		int iCount = GetItemCount();
		l_iNext = l_iSelected + 1;
		if( l_iNext < iCount )
		{
			l_bUpdate = true;
		}
	}
	if( nChar == VK_UP)
	{
		l_iNext = l_iSelected - 1;
		if( l_iNext >= 0 )
		{
			l_bUpdate = true;
		}
	}
	if( l_bUpdate )
	{
		SVToolSetTabViewClass* l_pParent = static_cast<SVToolSetTabViewClass*>(GetParent());
		if(l_pParent )
		{
			SVIPDoc* pCurrentDocument = dynamic_cast< SVIPDoc* >( l_pParent->GetDocument() );
			if( pCurrentDocument )
			{
				SetItemState(	l_iSelected, 0,	LVIS_SELECTED ); // un-select
				SetCurrentSelection( l_iNext ); // Select.
				EnsureVisible( l_iNext, FALSE);
				SVGUID l_Guid = GetCurrentSelectionID();
				if( !l_Guid.empty() )
				{
					pCurrentDocument->SetSelectedToolID( GetCurrentSelectionID() );

					if ( !SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&
						TheSVObserverApp.OkToEdit() )
					{
						l_pParent->PostMessage(WM_COMMAND,ID_RUN_ONCE);
					}
				}
			}
		}
	}
}
// Used to save scroll position before rebuilding list.
void SVToolSetListCtrlClass::SaveScrollPos()
{
	m_iTopIndex = GetTopIndex();
}

// Used in conjunction with SaveScrollPos to restore 
// scroll bars after a rebuild.
void SVToolSetListCtrlClass::RestoreScrollPos()
{
	if( m_iTopIndex > 0 )
	{
		EnsureVisible(GetItemCount()-1, FALSE);
		EnsureVisible(m_iTopIndex, FALSE );
	}
}

int SVToolSetListCtrlClass::SetCurrentSelection( const SVGUID& p_rObjectID )
{
	for( int i = 0; i < GetItemCount(); ++i )
	{
		if( IsValidSelection( i ) )
		{
			LVITEM item;

			memset( &item, '\0', sizeof( LVITEM ) );
			item.mask = LVIF_PARAM;
			item.iItem = i;

			if( GetItem( &item ) )
			{
				SVTaskObjectClass* pObject = reinterpret_cast< SVTaskObjectClass* >( item.lParam );

				if( pObject != NULL && p_rObjectID == pObject->GetUniqueObjectID() )
				{
					SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );
				}
				else
				{
					SetItemState( i, 0, LVIS_SELECTED );
				}
			}
		}
	}

    Invalidate(); // cause WM_PAINT to refresh list control.

    int l_Index = GetCurrentSelection();
    return l_Index;
}


BEGIN_MESSAGE_MAP(SVToolSetListCtrlClass, SVTaskObjectListCtrlClass)
	//{{AFX_MSG_MAP(SVToolSetListCtrlClass)
	//}}AFX_MSG_MAP
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


void SVToolSetListCtrlClass::CreateImageLists()
{
	m_oStateImageList.Create( 16, 16, TRUE, 2, 2 );

	m_iNone = m_oStateImageList.Add( AfxGetApp()->LoadIcon( IDI_STATUS_NONE ) );
	m_iInvalid = m_oStateImageList.Add( AfxGetApp()->LoadIcon( IDI_STATUS_BLACK ) );
	m_iDisabled = m_oStateImageList.Add( AfxGetApp()->LoadIcon( IDI_STATUS_GRAY ) );
	m_iPass = m_oStateImageList.Add( AfxGetApp()->LoadIcon( IDI_STATUS_GREEN ) );
	m_iFail = m_oStateImageList.Add( AfxGetApp()->LoadIcon( IDI_STATUS_RED ) );
	m_iWarn = m_oStateImageList.Add( AfxGetApp()->LoadIcon( IDI_STATUS_YELLOW ) );
	m_iUnknown = m_oStateImageList.Add( AfxGetApp()->LoadIcon( IDI_STATUS_BLUE ) );

	SetImageList( &m_oStateImageList, LVSIL_STATE );
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVToolSetListCtrl.cpp_v  $
 * 
 *    Rev 1.1   13 May 2013 16:29:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 015.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.13   08 May 2013 10:49:04   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  834
   SCR Title:  Modify tool set list to maintain position when tools are deleted
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added functions to support selected tool with keyboard and scroll bars.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.0   23 Apr 2013 15:47:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   02 Jul 2012 17:50:40   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to promote new display functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   29 Jun 2010 14:08:46   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source to change object validity test and casting changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   01 Jun 2010 11:10:08   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   18 Aug 2005 12:06:46   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  506
   SCR Title:  Update Tool Set List Tab to include an indicator when a tool is invalid
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update RebuildImages to handle invalid tool vs empty pockets in the list control.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   17 Aug 2005 15:05:12   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update RebuildImages to get data from list control instead of Task Object List array.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   17 Aug 2005 14:58:00   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     fix problem in iterating the list
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   17 Aug 2005 13:36:56   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated the status of a null tool to invalid.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   17 Aug 2005 13:33:40   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added try/catch around a dynamic_cast to prevent a crash.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   16 Aug 2005 15:25:36   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  506
   SCR Title:  Update Tool Set List Tab to include an indicator when a tool is invalid
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added functionality to paint tool status Icons in front of the tool names.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   22 Apr 2003 16:56:02   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   20 Nov 2002 14:42:18   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Removed OLE Automation
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
