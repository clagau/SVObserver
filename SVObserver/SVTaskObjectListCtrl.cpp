//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTaskObjectListCtrl
//* .File Name       : $Workfile:   SVTaskObjectListCtrl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:34:48  $
//******************************************************************************

#include "stdafx.h"
#include "SVTaskObjectList.h"
#include "SVTaskObjectListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVTaskObjectListCtrlClass

IMPLEMENT_DYNCREATE(SVTaskObjectListCtrlClass, CListCtrl)

SVTaskObjectListCtrlClass::SVTaskObjectListCtrlClass()
: CListCtrl()
{
	pOwnerTaskList = NULL;
}

SVTaskObjectListCtrlClass::~SVTaskObjectListCtrlClass()
{
}

BOOL SVTaskObjectListCtrlClass::SetTaskObjectList( SVTaskObjectListClass* POwnerTaskList )
{
	if( POwnerTaskList != NULL )
	{
		int cnt = 0;
		// Get the Header Control
		CHeaderCtrl* pHeaderCtrl = ( CHeaderCtrl* )GetDlgItem( 0 );
		if( pHeaderCtrl )
		{
			// turn off button style
			pHeaderCtrl->ModifyStyle( HDS_BUTTONS, 0, 0 );

			cnt = pHeaderCtrl->GetItemCount();
		}

		pOwnerTaskList = POwnerTaskList;

		if( cnt )
		{
			DeleteColumn( 0 );
		}
		InsertColumn( 0, pOwnerTaskList->GetName(), LVCFMT_LEFT, -1, -1 );

		// Set the column Width
		CRect rect;
		GetClientRect( &rect );
		SetColumnWidth( 0, rect.Width() );	

		// Rebuild list...
		Rebuild();

		return TRUE;
	}
	return FALSE;
}


void SVTaskObjectListCtrlClass::Rebuild()
{
	DeleteAllItems();

	if( pOwnerTaskList != NULL )
	{
		if( pOwnerTaskList->GetSize() <= 0 )
		{
			CString strEmpty;
			strEmpty.LoadString( IDS_EMPTY_STRING );
			SetItemData( InsertItem( 0, strEmpty ), NULL );
			return;
		}

		for( int i = 0; i < pOwnerTaskList->GetSize(); i++ )
			if( pOwnerTaskList->GetAt( i ) )
				SetItemData( InsertItem( i, pOwnerTaskList->GetAt( i )->GetName() ), ( DWORD ) pOwnerTaskList->GetAt( i ) );
			else
				SetItemData( InsertItem( i, _T( "&&&&&&" ) ), ( DWORD ) NULL );

		// 29 Oct 1999 - frb.
		//SetItemData( InsertItem( i, _T( "-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------" ) ), NULL );
	}
}

int SVTaskObjectListCtrlClass::GetCurrentSelection( int StartAt /* = -1 -> start searching at top */ )
{
	/*
	LVITEM item;
	int cur = -1;

	memset( &item, '\0', sizeof( LVITEM ) );
	item.mask = LVIF_STATE;
	item.stateMask = LVIS_SELECTED;
	
	int cnt = GetItemCount();
	
	for (int i = 0;i < cnt;i++)
	{
		item.iItem = i;

		if ( GetItem( &item )  && item.state == LVIS_SELECTED)
		{
			if ( IsValidSelection( i ) )
			{
				cur = i;
				break;
			}
		}
	}
	*/
	int cur = GetNextItem( StartAt, LVNI_SELECTED );
	if (cur != -1)
	{
		if ( !IsValidSelection( cur ) )
		{
			cur = -1;
		}
	}
	return cur;
}

BOOL SVTaskObjectListCtrlClass::IsValidSelection( int index )
{
	BOOL valid = FALSE;
	LVITEM item;

	memset( &item, '\0', sizeof( LVITEM ) );
	item.mask = LVIF_PARAM;
	item.iItem = index;

    //
	// Check for 'empty' string as item string.
	//
	CString csItemText = GetItemText(index,0);
	CString strEmpty;
	strEmpty.LoadString( IDS_EMPTY_STRING );
	if(csItemText == strEmpty)
	{
		return valid;         // set to FALSE
	}

	if( GetItem( &item ) )
	{
		TCHAR *param = (TCHAR *)(item.lParam);
		if( param )
		{
			valid = TRUE;
		}
	}

	return valid;
}

BEGIN_MESSAGE_MAP(SVTaskObjectListCtrlClass, CListCtrl)
	//{{AFX_MSG_MAP(SVTaskObjectListCtrlClass)
	ON_WM_SHOWWINDOW()
	ON_WM_WINDOWPOSCHANGED()
	ON_NOTIFY(HDN_BEGINTRACKA, 0, OnBeginTrack)
	ON_NOTIFY(HDN_BEGINTRACKW, 0, OnBeginTrack)
	//}}AFX_MSG_MAP
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVTaskObjectListCtrlClass message handlers

void SVTaskObjectListCtrlClass::OnWindowPosChanged( WINDOWPOS* lpwndpos )
{
	CListCtrl::OnWindowPosChanged( lpwndpos );

	if( !( lpwndpos->flags & SWP_NOSIZE )  )
	{
		CRect rect;
		GetClientRect( &rect );
		SetColumnWidth( 0, rect.Width() );
	}
}

void SVTaskObjectListCtrlClass::OnShowWindow( BOOL bShow, UINT nStatus ) 
{
	CListCtrl::OnShowWindow( bShow, nStatus );

	//if( bShow )	
	//	Rebuild();	
}

void SVTaskObjectListCtrlClass::OnBeginTrack( NMHDR* pNMHDR, LRESULT* pResult )
{
     switch ( pNMHDR->code )
     {
		 // This is due to a bug in NT where the code is not translated
		 // to ANSI/UNICODE properly (in NT - UNICODE is always sent)
		case HDN_BEGINTRACKW:
		case HDN_BEGINTRACKA:
		case HDN_DIVIDERDBLCLICKA:
		case HDN_DIVIDERDBLCLICKW:
		{
			*pResult = TRUE;		// disable tracking
		}
		break;

		default:
		{
			*pResult = 0;
		}
		break;
     } 
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTaskObjectListCtrl.cpp_v  $
 * 
 *    Rev 1.0   24 Apr 2013 11:34:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   29 Jun 2010 14:08:44   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source to change object validity test and casting changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   01 Jun 2010 11:10:06   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   22 Apr 2003 16:36:44   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   20 Nov 2002 13:52:32   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Removed old Automation, ole dispatch map
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
