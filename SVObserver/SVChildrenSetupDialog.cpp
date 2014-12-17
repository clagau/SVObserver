//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVChildrenSetupDialog
//* .File Name       : $Workfile:   SVChildrenSetupDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   16 Dec 2014 17:56:48  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVChildrenSetupDialog.h"
#include "SVClassInfoStruct.h"
#include "SVTaskObjectList.h"
#include "SVInspectionProcess.h"
#include "SVIPDoc.h"
#include "SVShowDependentsDialog.h"
#include "SVSetupDialogManager.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#pragma endregion Includes

#pragma region Declarations
using namespace Seidenader::ObjectSelectorLibrary;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVChildrenSetupDialogClass::SVChildrenSetupDialogClass(CWnd* pParent /*=NULL*/)
: CDialog(SVChildrenSetupDialogClass::IDD, pParent)
, m_pAvailableChildrenList( nullptr )
, m_pParentObject( nullptr )
, m_pParentOwner( nullptr )
, m_AllowMultipleChildrenInstances( FALSE )
{
	//{{AFX_DATA_INIT(SVChildrenSetupDialogClass)
	//}}AFX_DATA_INIT
}

void SVChildrenSetupDialogClass::redrawLists()
{
	if( m_pAvailableChildrenList && m_pParentObject )
	{
		m_ChildrenListCtrl.Rebuild();

		m_AvailableChildrenListCtrl.DeleteAllItems();
		for( int i = 0; i < m_pAvailableChildrenList->GetSize(); ++i )
		{
			SVClassInfoStruct& rChildrenInfo = m_pAvailableChildrenList->ElementAt( i );

			// Make simple name comparison...
			LVFINDINFO findInfo;
			findInfo.flags = LVFI_STRING;
			findInfo.psz   = rChildrenInfo.ClassName;
			if( m_AllowMultipleChildrenInstances || m_ChildrenListCtrl.FindItem( &findInfo ) < 0 )
			{
				m_AvailableChildrenListCtrl.SetItemData( m_AvailableChildrenListCtrl.InsertItem( 0, rChildrenInfo.ClassName ), static_cast<DWORD_PTR>(i) );
			}
		}

		GetAndEnableWindow( this, IDC_PUBLISH_BUTTON, FALSE );
		GetAndEnableWindow( this, IDC_SETUP_BUTTON, FALSE );
		GetAndEnableWindow( this, IDC_ADD_BUTTON, FALSE );
		GetAndEnableWindow( this, IDC_REMOVE_BUTTON, FALSE );

		UpdateData( FALSE ); // set data to dialog
	}
}

void SVChildrenSetupDialogClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVChildrenSetupDialogClass)
	DDX_Control(pDX, IDC_CHILDREN_LIST, m_ChildrenListCtrl);
	DDX_Control(pDX, IDC_AVAILABLE_CHILDREN_LIST, m_AvailableChildrenListCtrl);
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
	
	if( m_pAvailableChildrenList && m_pParentObject && ( m_pParentOwner = m_pParentObject->GetOwner() ) )
	{
		// Setup available list...
		m_AvailableChildrenListCtrl.ModifyStyle( 0, LVS_REPORT );
		// Set column witdh...
		CRect rect;
		m_AvailableChildrenListCtrl.GetClientRect( &rect );
		m_AvailableChildrenListCtrl.InsertColumn( 0, _T( "" ), LVCFMT_LEFT, rect.Width(), -1 );

		// Setup children list...
		m_ChildrenListCtrl.ModifyStyle( 0, LVS_REPORT );
		// Set column witdh...
		m_ChildrenListCtrl.GetClientRect( &rect );
		m_ChildrenListCtrl.InsertColumn( 0, _T( "" ), LVCFMT_LEFT, rect.Width(), -1 );
		// Populate...
		m_ChildrenListCtrl.SetTaskObjectList( m_pParentObject );

		if( !StrTitle.IsEmpty() ) { SetWindowText( StrTitle ); }

		// Refresh...
		redrawLists();
	}
	else
	{
		// Not valid call...
		SendMessage( WM_CLOSE );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void SVChildrenSetupDialogClass::OnAddButton()
{
	CWaitCursor l_cwcMouse;

	if( m_pAvailableChildrenList && m_pParentObject )
	{
		int item = -1;
		int listIndex;
		// Get all selected items...
		while( ( item = m_AvailableChildrenListCtrl.GetNextItem( item, LVNI_ALL | LVNI_SELECTED ) ) >= 0 )
		{
			listIndex = static_cast< int >( m_AvailableChildrenListCtrl.GetItemData( item ) );
			if( listIndex >= 0 && listIndex < m_pAvailableChildrenList->GetSize() )
			{
				// It´s a valid index ...

				SVClassInfoStruct& rChildrenInfo = m_pAvailableChildrenList->ElementAt( listIndex );
				// Construct Children...
				SVTaskObjectClass* pObject = static_cast< SVTaskObjectClass* >( rChildrenInfo.Construct() );
				if( SV_IS_KIND_OF( pObject, SVTaskObjectClass ) )
				{
					// Add children to parent...
					m_pParentObject->Add( pObject );

					// Ensure this Object's inputs get connected
					::SVSendMessage( pObject, SVM_CONNECT_ALL_INPUTS, NULL, NULL );

					// And finally try to create the child object...
					if( ::SVSendMessage( m_pParentObject, SVM_CREATE_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(pObject), SVMFSetDefaultInputs | SVMFResetInspection ) != SVMR_SUCCESS )
					{
						CString strMessage;
						AfxFormatString1( strMessage, IDS_CRITICAL_CREATION_OF_FAILED, pObject->GetName() );
						AfxMessageBox( strMessage );

						// Remove it from the Tool TaskObjectList ( Destruct it )
						GUID objectID = pObject->GetUniqueObjectID();
						if( objectID != SVInvalidGUID )
						{
							m_pParentObject->Delete( objectID );
						}
						else
						{
							delete pObject;
						}

						pObject = nullptr;
						continue;
					}
				}
				else
				{
					if( pObject != nullptr ) { delete pObject; }
				}
			}
		}

		// Refresh...
		redrawLists();
	}
}

void SVChildrenSetupDialogClass::OnRemoveButton()
{
	if( m_pAvailableChildrenList && m_pParentObject )
	{
		int item = -1;
		SVTaskObjectClass* pTaskObject;
		// Get all selected items...
		while( ( item = m_ChildrenListCtrl.GetNextItem( item, LVNI_ALL | LVNI_SELECTED ) ) >= 0 )
		{
			pTaskObject = reinterpret_cast< SVTaskObjectClass* >( m_ChildrenListCtrl.GetItemData( item ) );
			if( SV_IS_KIND_OF( pTaskObject, SVTaskObjectClass ) )
			{
				int rc = checkOkToDelete( pTaskObject );

				if( rc )
				{
					// Close, Disconnect and Delete Children...
					::SVSendMessage( m_pParentObject, SVM_DESTROY_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(pTaskObject), SVMFSetDefaultInputs | SVMFResetInspection );
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
	ASSERT(m_pDocument);
	if(m_pDocument)
	{
		m_pDocument->SetModifiedFlag();   // Mark document as changed.
	}

	CDialog::OnOK();
}

void SVChildrenSetupDialogClass::OnSetupButton()
{
	if( m_pAvailableChildrenList && m_pParentObject )
	{
		int item = -1;
		// Get all selected items...
		while( ( item = m_ChildrenListCtrl.GetNextItem( item, LVNI_ALL | LVNI_SELECTED ) ) >= 0 )
		{
			SVObjectClass* pObject = reinterpret_cast< SVObjectClass* >( m_ChildrenListCtrl.GetItemData( item ) );
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
	if( nullptr == m_pParentObject || nullptr == m_pParentOwner ) { return; }

	ObjectTreeGenerator::Instance().setSelectorType( ObjectTreeGenerator::SelectorTypeEnum::TypeSetAttributes );
	ObjectTreeGenerator::Instance().setAttributeFilters( SV_PUBLISHABLE );
	ObjectTreeGenerator::Instance().setLocationFilter( ObjectTreeGenerator::FilterInput, m_pParentOwner->GetCompleteObjectName(), SVString( _T("") ) );

	SVOutputInfoListClass OutputList;
	m_pParentObject->GetOutputList( OutputList );
	ObjectTreeGenerator::Instance().insertOutputList( OutputList );

	CString PublishableResults;
	PublishableResults.LoadString( IDS_PUBLISHABLE_RESULTS );
	SVString Title;
	SVString ToolName( m_pParentOwner->GetName() );
	Title.Format( _T("%s - %s"), PublishableResults, ToolName.c_str() );
	SVString mainTabTitle = PublishableResults;
	CString Filter;
	Filter.LoadString( IDS_FILTER );
	SVString filterTabTitle = Filter;
	INT_PTR Result = ObjectTreeGenerator::Instance().showDialog( Title, mainTabTitle, filterTabTitle, this );

	if( IDOK == Result )
	{
		// refresh the publish list
		if( m_pDocument )
		{
			m_pDocument->RefreshPublishedList();
		}
	}
}

void SVChildrenSetupDialogClass::OnItemChangedChildrenList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = reinterpret_cast< NM_LISTVIEW* >( pNMHDR );

	//
	// Check for a selection count and if selection is not the
	// 'Empty' string.
	//
	UINT nSelectedCount = m_ChildrenListCtrl.GetSelectedCount();
	int nItemCount = m_ChildrenListCtrl.GetItemCount();
	BOOL bValidSelection = FALSE;
	if(nItemCount > 0)
	{
		bValidSelection = m_ChildrenListCtrl.IsValidSelection(0);
	}

	if( (nSelectedCount == 0) ||
		( ( (nItemCount == 1) && (nSelectedCount == 1)) &&
		!bValidSelection ) )
	{
		//
		// Disable the inappropriate buttons.
		//
		GetAndEnableWindow( this, IDC_PUBLISH_BUTTON, FALSE );
		GetAndEnableWindow( this, IDC_SETUP_BUTTON, FALSE );
		GetAndEnableWindow( this, IDC_REMOVE_BUTTON, FALSE );
	}
	else
	{
		//
		// Enable the appropriate buttons.
		//
		GetAndEnableWindow( this, IDC_PUBLISH_BUTTON );
		GetAndEnableWindow( this, IDC_SETUP_BUTTON );
		GetAndEnableWindow( this, IDC_REMOVE_BUTTON );
	}

	UpdateData( FALSE ); // set data to dialog

	*pResult = 0;
}

void SVChildrenSetupDialogClass::OnItemChangedAvailableChildrenList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = reinterpret_cast< NM_LISTVIEW* >( pNMHDR );

	BOOL enable = m_AvailableChildrenListCtrl.GetSelectedCount();
	GetAndEnableWindow( this, IDC_ADD_BUTTON, enable );

	UpdateData( FALSE ); // set data to dialog

	*pResult = 0;
}

BOOL SVChildrenSetupDialogClass::checkOkToDelete( SVTaskObjectClass* pTaskObject )
{
	BOOL bRetVal = TRUE;

	// show dependents dialog
	SVShowDependentsDialog dlg;
	dlg.PTaskObject = pTaskObject;

	INT_PTR rc = dlg.DoModal();

	if( IDCANCEL == rc ) { bRetVal = FALSE; }

	return bRetVal;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVChildrenSetupDialog.cpp_v  $
 * 
 *    Rev 1.5   16 Dec 2014 17:56:48   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  933
 * SCR Title:  Add Filter Tab to Object Selector (SVO-377)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed the OnPublishButton method to specify the title of the Object Selector's Filter Page.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Jul 2014 18:22:00   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Replace ResultPicker Dialog with Object Selector Dialog
 * Code review changes
 * Methods changed: OnPublishButton
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 May 2014 11:10:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised SVSendMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Feb 2014 10:23:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Oct 2013 12:16:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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