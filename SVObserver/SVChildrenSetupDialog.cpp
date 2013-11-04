//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVChildrenSetupDialog
//* .File Name       : $Workfile:   SVChildrenSetupDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:53:10  $
//******************************************************************************

#include "stdafx.h"
#include "SVChildrenSetupDialog.h"
#include "SVClassInfoStruct.h"
#include "SVTaskObjectList.h"
#include "SVDlgResultPicker.h"
#include "SVInspectionProcess.h"
#include "SVIPDoc.h"
#include "SVShowDependentsDialog.h"
#include "SVSetupDialogManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVChildrenSetupDialogClass::SVChildrenSetupDialogClass(CWnd* pParent /*=NULL*/)
: CDialog(SVChildrenSetupDialogClass::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVChildrenSetupDialogClass)
	//}}AFX_DATA_INIT

	PAvailableChildrenList = NULL;
	PParentObject		   = NULL;
	pParentOwner		   = NULL;
	BAllowMultipleChildrenInstances = FALSE;
}

void SVChildrenSetupDialogClass::redrawLists()
{
	if( PAvailableChildrenList && PParentObject )
	{
		ChildrenListCtrl.Rebuild();

		AvailableChildrenListCtrl.DeleteAllItems();
		for( int i = 0; i < PAvailableChildrenList->GetSize(); ++ i )
		{
			SVClassInfoStruct& rChildrenInfo = PAvailableChildrenList->ElementAt( i );

			// Make simple name comparison...
			LVFINDINFO findInfo;
			findInfo.flags = LVFI_STRING;
			findInfo.psz   = rChildrenInfo.ClassName;
			if( BAllowMultipleChildrenInstances || ChildrenListCtrl.FindItem( &findInfo ) < 0 )
				AvailableChildrenListCtrl.SetItemData( AvailableChildrenListCtrl.InsertItem( 0, rChildrenInfo.ClassName ), ( DWORD ) i );
		}


		CWnd* pWnd = NULL;
		if( pWnd = GetDlgItem( IDC_PUBLISH_BUTTON ) )
			pWnd->EnableWindow( FALSE );

		if( pWnd = GetDlgItem( IDC_SETUP_BUTTON ) )
			pWnd->EnableWindow( FALSE );

		if( pWnd = GetDlgItem( IDC_ADD_BUTTON ) )
			pWnd->EnableWindow( FALSE );

		if( pWnd = GetDlgItem( IDC_REMOVE_BUTTON ) )
			pWnd->EnableWindow( FALSE );

		UpdateData( FALSE ); // set data to dialog
	}
}


void SVChildrenSetupDialogClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVChildrenSetupDialogClass)
	DDX_Control(pDX, IDC_CHILDREN_LIST, ChildrenListCtrl);
	DDX_Control(pDX, IDC_AVAILABLE_CHILDREN_LIST, AvailableChildrenListCtrl);
	//}}AFX_DATA_MAP
}

//******************************************************************************
// Message Map Entries
//******************************************************************************

BEGIN_MESSAGE_MAP(SVChildrenSetupDialogClass, CDialog)
	//{{AFX_MSG_MAP(SVChildrenSetupDialogClass)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_REMOVE_BUTTON, OnRemoveButton)
	ON_BN_CLICKED(IDC_SETUP_BUTTON, OnSetupButton)
	ON_BN_CLICKED(IDC_PUBLISH_BUTTON, OnPublishButton)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CHILDREN_LIST, OnItemChangedChildrenList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_AVAILABLE_CHILDREN_LIST, OnItemChangedAvailableChildrenList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//******************************************************************************
// Message Handler
//******************************************************************************

BOOL SVChildrenSetupDialogClass::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if( PAvailableChildrenList && PParentObject && ( pParentOwner = PParentObject->GetOwner() ) )
	{
		// Setup available list...
		AvailableChildrenListCtrl.ModifyStyle( 0, LVS_REPORT );
		// Set column witdh...
		CRect rect;
		AvailableChildrenListCtrl.GetClientRect( &rect );
		AvailableChildrenListCtrl.InsertColumn( 0, _T( "" ), LVCFMT_LEFT, rect.Width(), -1 );


		// Setup children list...
		ChildrenListCtrl.ModifyStyle( 0, LVS_REPORT );
		// Set column witdh...
		ChildrenListCtrl.GetClientRect( &rect );
		ChildrenListCtrl.InsertColumn( 0, _T( "" ), LVCFMT_LEFT, rect.Width(), -1 );
		// Populate...
		ChildrenListCtrl.SetTaskObjectList( PParentObject );

		if( ! StrTitle.IsEmpty() )
			SetWindowText( StrTitle );

		// Refresh...		
		redrawLists();
	}
	else
		// Not valid call...
		SendMessage( WM_CLOSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void SVChildrenSetupDialogClass::OnAddButton() 
{
	CWaitCursor l_cwcMouse;

	if( PAvailableChildrenList && PParentObject )
	{
		int item = -1;
		int listIndex;
		// Get all selected items...
		while( ( item = AvailableChildrenListCtrl.GetNextItem( item, LVNI_ALL | LVNI_SELECTED ) ) >= 0 )
		{
			listIndex = ( int ) AvailableChildrenListCtrl.GetItemData( item );
			if( listIndex >= 0 && listIndex < PAvailableChildrenList->GetSize() )
			{
				// It´s a valid index ...

				SVClassInfoStruct& rChildrenInfo = PAvailableChildrenList->ElementAt( listIndex );
				// Construct Children...
				SVTaskObjectClass* pObject = ( SVTaskObjectClass* ) rChildrenInfo.Construct();
				if( SV_IS_KIND_OF( pObject, SVTaskObjectClass ) )
				{
					// Add children to parent...
					PParentObject->Add( pObject );

					// Ensure this Object's inputs get connected
					::SVSendMessage( pObject, SVM_CONNECT_ALL_INPUTS, NULL, NULL );

					// And finally try to create the child object...
					if( ::SVSendMessage( PParentObject, SVM_CREATE_CHILD_OBJECT, ( DWORD ) pObject, SVMFSetDefaultInputs | SVMFResetInspection ) != SVMR_SUCCESS )
					{
						CString strMessage;
						AfxFormatString1( strMessage, IDS_CRITICAL_CREATION_OF_FAILED, pObject->GetName() ); 
						AfxMessageBox( strMessage );
						
						// Remove it from the Tool TaskObjectList ( Destruct it )
						GUID objectID = pObject->GetUniqueObjectID();
						if( objectID != SVInvalidGUID )
							PParentObject->Delete( objectID );
						else
							delete pObject;

						pObject = NULL;
						continue;
					}
				}
				else
				{
					if( pObject )
						delete pObject;
				}
			}
		}

		// Refresh...
		redrawLists();
	}
}

void SVChildrenSetupDialogClass::OnRemoveButton() 
{
	if( PAvailableChildrenList && PParentObject )
	{
		int item = -1;
		SVTaskObjectClass* pTaskObject;
		// Get all selected items...
		while( ( item = ChildrenListCtrl.GetNextItem( item, LVNI_ALL | LVNI_SELECTED ) ) >= 0 )
		{
			pTaskObject = ( SVTaskObjectClass* ) ChildrenListCtrl.GetItemData( item );
			if( SV_IS_KIND_OF( pTaskObject, SVTaskObjectClass ) )
			{
				int rc = checkOkToDelete( pTaskObject );

				if( rc )
				{
					// Close, Disconnect and Delete Children...
					::SVSendMessage( PParentObject, SVM_DESTROY_CHILD_OBJECT, ( DWORD ) pTaskObject, SVMFSetDefaultInputs | SVMFResetInspection );
				}
			}
		}

		// Refresh...
		redrawLists();
	}
}

void SVChildrenSetupDialogClass::OnCancel() 
{
	CDialog::OnCancel();
}

void SVChildrenSetupDialogClass::OnOK() 
{
    ASSERT(PDocument);
    if(PDocument)
    {
        PDocument->SetModifiedFlag();   // Mark document as changed.
    }

	CDialog::OnOK();
}

void SVChildrenSetupDialogClass::OnSetupButton() 
{
	if( PAvailableChildrenList && PParentObject )
	{
		int item = -1;
		SVObjectClass* pObject;
		// Get all selected items...
		while( ( item = ChildrenListCtrl.GetNextItem( item, LVNI_ALL | LVNI_SELECTED ) ) >= 0 )
		{
			pObject = ( SVObjectClass* ) ChildrenListCtrl.GetItemData( item );
			if( SV_IS_KIND_OF( pObject, SVObjectClass ) )
			{
				SVSetupDialogManager::Instance().SetupDialog( pObject->GetClassID(), pObject->GetUniqueObjectID(), this );
			}
		}

		// Refresh...
		redrawLists();
	}
}

void SVChildrenSetupDialogClass::OnPublishButton() 
{
	SVDlgResultPicker   dlg;

    CString  publishedResultString;

    dlg.PTaskObjectList = PParentObject;
    dlg.uAttributesDesired = SV_PUBLISHABLE;

    publishedResultString.LoadString ( IDS_PUBLISHABLE_RESULTS );
    dlg.SetCaptionTitle (publishedResultString);

    int rc = dlg.DoModal ();
	if( rc == IDOK )
	{
		// refresh the publish list
		if( PDocument )
		{
			PDocument->RefreshPublishedList();
		}
	}
}

void SVChildrenSetupDialogClass::OnItemChangedChildrenList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	CWnd* pWnd = NULL;

	//
	// Check for a selection count and if selection is not the
	// 'Empty' string.
	//
	int nSelectedCount = ChildrenListCtrl.GetSelectedCount();
	int nItemCount = ChildrenListCtrl.GetItemCount();
	BOOL bValidSelection = FALSE;
	if(nItemCount > 0)
	{
	   bValidSelection = ChildrenListCtrl.IsValidSelection(0);
	}

	if( (nSelectedCount == 0) ||
		( ( (nItemCount == 1) && (nSelectedCount == 1)) &&
		!bValidSelection ) )
	{
		//
		// Disable the appropriate buttons.
		//
		if( pWnd = GetDlgItem( IDC_PUBLISH_BUTTON ) )
			pWnd->EnableWindow( FALSE );

		if( pWnd = GetDlgItem( IDC_SETUP_BUTTON ) )
			pWnd->EnableWindow( FALSE );

		if( pWnd = GetDlgItem( IDC_REMOVE_BUTTON ) )
			pWnd->EnableWindow( FALSE );
	}
	else
	{
		//
		// Enable the appropriate buttons.
		//
		if( pWnd = GetDlgItem( IDC_PUBLISH_BUTTON ) )
			pWnd->EnableWindow( TRUE );

		if( pWnd = GetDlgItem( IDC_SETUP_BUTTON ) )
			pWnd->EnableWindow( TRUE );

		if( pWnd = GetDlgItem( IDC_REMOVE_BUTTON ) )
			pWnd->EnableWindow( TRUE );
	}
	
	UpdateData( FALSE ); // set data to dialog

	*pResult = 0;
}

void SVChildrenSetupDialogClass::OnItemChangedAvailableChildrenList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	CWnd* pWnd = NULL;
	if( AvailableChildrenListCtrl.GetSelectedCount() )
	{
		if( pWnd = GetDlgItem( IDC_ADD_BUTTON ) )
			pWnd->EnableWindow( TRUE );
	}
	else
	{
		if( pWnd = GetDlgItem( IDC_ADD_BUTTON ) )
			pWnd->EnableWindow( FALSE );
	}
	
	UpdateData( FALSE ); // set data to dialog

	*pResult = 0;
}

BOOL SVChildrenSetupDialogClass::checkOkToDelete( SVTaskObjectClass* pTaskObject )
{
	BOOL bRetVal = TRUE;
	
	// show dependents dialog
	SVShowDependentsDialog dlg;
	dlg.PTaskObject = pTaskObject;
		
	int rc = dlg.DoModal();

	if( rc == IDCANCEL )
		bRetVal = FALSE;

	return bRetVal;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVChildrenSetupDialog.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:53:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   04 Sep 2012 14:49:58   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  602
   SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Remove dead functionality and attributes from dialog.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   02 Jul 2012 16:39:46   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to promote new display functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   13 Mar 2012 12:35:06   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  745
   SCR Title:  Fix contention issue during load process and display
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Modified On Add Button method, On Remove Button method, and On Publish Button method with consolidated functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   08 Dec 2010 07:45:14   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to include changes in notification functionality using the Observer Design Pattern.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   10 Aug 2005 12:27:40   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated methods to improve performance and reduce system delay.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   17 Feb 2005 13:21:50   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update class with new methodologies based on new base object, extents and reset structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   17 Apr 2003 17:08:04   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   19 Nov 2002 10:59:16   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Removed COM support
   Cosmetic change: 
             TheObjectManager -> TheSVObjectManager
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   30 Jan 2002 16:06:40   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  278
   SCR Title:  Error message is displayed when removing result objects used by other tools
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Added new function ::checkOkToDelete which calls SVShowDependentsDialog to show all objects connected to  the specified object.
   Modified ::OnRemoveButton to call ::checkOkToDelete passing the result object before removing it.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
