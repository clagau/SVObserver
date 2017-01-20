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
#include "SVOCore/SVClassInfoStruct.h"
#include "SVOCore/SVTaskObjectList.h"
#include "SVInspectionProcess.h"
#include "SVIPDoc.h"
#include "SVOGui/SVShowDependentsDialog.h"
#include "SVSetupDialogManager.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "SVOGui/NoSelector.h"
#include "SVOGui/ToolSetItemSelector.h"
#include "SVStatusLibrary/MessageManager.h"
#pragma endregion Includes

#pragma region Declarations

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

//******************************************************************************
// Method:		GetAndEnableWindow
// Description:	Gets a control from a dialog and enables or disables it.
// Parameter:	dialog - This should be a pointer the dialog containing the 
//		control to be enabled or disabled.
// Parameter:	ctrlId - This should be the identifier for the control.
// Parameter:	enable - Indicates if the control should be enabled {TRUE, default}
//		or disabled {FALSE}.
// Returns:		HRESULT -
//		S_OK - The control was not previously disabled.
//		S_FALSE - The control was not previously disabled.
//		E_FAIL - ctrlId was not found in dialog.
//******************************************************************************
HRESULT GetAndEnableWindow( CWnd* dialog, const int ctrlId, const BOOL enable = TRUE )
{
	HRESULT hr = E_FAIL;

	if ( nullptr != dialog )
	{
		CWnd* item = dialog->GetDlgItem( ctrlId );
		if ( nullptr != item )
		{
			BOOL wasDisabled = item->EnableWindow( enable );
			hr = wasDisabled == FALSE ? S_FALSE : S_OK;
		}
	}

	return hr;
}

SVChildrenSetupDialogClass::SVChildrenSetupDialogClass(CWnd* pParent /*=nullptr*/)
: CDialog(SVChildrenSetupDialogClass::IDD, pParent)
, m_pAvailableChildrenList( nullptr )
, m_pParentObject( nullptr )
, m_pParentOwner( nullptr )
, m_AllowMultipleChildrenInstances( FALSE )
{
	//{{AFX_DATA_INIT(SVChildrenSetupDialogClass)
	//}}AFX_DATA_INIT
}

SVChildrenSetupDialogClass::~SVChildrenSetupDialogClass()
{
}

void SVChildrenSetupDialogClass::redrawLists()
{
	if( m_pAvailableChildrenList && m_pParentObject )
	{
		m_ChildrenListCtrl.DeleteAllItems();

		if( m_pParentObject )
		{
			if( m_pParentObject->GetSize() <= 0 )
			{
				SVString Empty = SvUl_SF::LoadSVString( IDS_EMPTY_STRING );
				m_ChildrenListCtrl.SetItemData( m_ChildrenListCtrl.InsertItem( 0, Empty.c_str() ), 0 );
			}
			else
			{
				for( int i = 0; i < m_pParentObject->GetSize(); i++ )
				{
					if( m_pParentObject->GetAt( i ) )
					{
						m_ChildrenListCtrl.SetItemData( m_ChildrenListCtrl.InsertItem( i, m_pParentObject->GetAt( i )->GetName() ), reinterpret_cast<DWORD_PTR>(m_pParentObject->GetAt( i )) );
					}
					else
					{
						m_ChildrenListCtrl.SetItemData( m_ChildrenListCtrl.InsertItem( i, _T( "&&&&&&" ) ), 0 );
					}
				}
			}
		}

		m_AvailableChildrenListCtrl.DeleteAllItems();
		for( int i = 0; i < m_pAvailableChildrenList->GetSize(); ++i )
		{
			SVClassInfoStruct& rChildrenInfo = m_pAvailableChildrenList->ElementAt( i );

			// Make simple name comparison...
			LVFINDINFO findInfo;
			findInfo.flags = LVFI_STRING;
			findInfo.psz   = rChildrenInfo.m_ClassName.c_str();
			if( m_AllowMultipleChildrenInstances || m_ChildrenListCtrl.FindItem( &findInfo ) < 0 )
			{
				m_AvailableChildrenListCtrl.SetItemData( m_AvailableChildrenListCtrl.InsertItem( 0, rChildrenInfo.m_ClassName.c_str() ), static_cast<DWORD_PTR>(i) );
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

		if( !m_Title.empty() ) { SetWindowText( m_Title.c_str() ); }

		// Refresh...
		redrawLists();
	}
	else
	{
		// Not valid call...
		SendMessage( WM_CLOSE );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zur�ckgeben
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
				// It�s a valid index ...

				SVClassInfoStruct& rChildrenInfo = m_pAvailableChildrenList->ElementAt( listIndex );
				// Construct Children...
				SVTaskObjectClass* pObject = dynamic_cast< SVTaskObjectClass* >( rChildrenInfo.Construct() );
				if( nullptr != pObject )
				{
					// Add children to parent...
					m_pParentObject->Add( pObject );

					// Ensure this Object's inputs get connected
					pObject->ConnectAllInputs();

					// And finally try to create the child object...
					if( !m_pParentObject->CreateChildObject(pObject, SVMFSetDefaultInputs | SVMFResetInspection ) )
					{
						SVStringVector msgList;
						msgList.push_back(SVString(pObject->GetName()));
						SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
						Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_CreationFailed, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10043 ); 

						// Remove it from the Tool TaskObjectList ( Destruct it )
						GUID objectID = pObject->GetUniqueObjectID();
						if( SV_GUID_NULL != objectID )
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
			//reinterpret_cast is dangerous, but a kind of or dynamic_cast afterwards do not help for more safety.
			if (nullptr != pTaskObject)
			{
				int rc = checkOkToDelete( pTaskObject );

				if( rc )
				{
					// Close, Disconnect and Delete Children...
					m_pParentObject->DestroyChildObject(pTaskObject, SVMFSetDefaultInputs | SVMFResetInspection );
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
			if( nullptr != pObject )
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

	SvOsl::ObjectTreeGenerator::Instance().setSelectorType( SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeSetAttributes );
	SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterInput, SVString(m_pParentOwner->GetCompleteName()), SVString( _T("") ) );

	SvOsl::SelectorOptions BuildOptions( m_pDocument->GetInspectionID(), SV_PUBLISHABLE, m_pParentObject->GetUniqueObjectID() );
	SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<SvOg::NoSelector, SvOg::NoSelector, SvOg::ToolSetItemSelector<>>( BuildOptions );

	SVString PublishableResults = SvUl_SF::LoadSVString( IDS_PUBLISHABLE_RESULTS );
	SVString Title = SvUl_SF::Format( _T("%s - %s"), PublishableResults.c_str(), m_pParentOwner->GetName() );
	SVString Filter = SvUl_SF::LoadSVString( IDS_FILTER );
	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title.c_str(), PublishableResults.c_str(), Filter.c_str(), this );

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
	bool bValidSelection = true;
	if( 1 == nItemCount )
	{
		//
		// Check for 'empty' string as item string.
		//
		SVString ItemText = m_ChildrenListCtrl.GetItemText(0,0);
		SVString Empty = SvUl_SF::LoadSVString( IDS_EMPTY_STRING );
		if( ItemText == Empty )
		{
			bValidSelection = false;
		}
	}

	if( 0 == nSelectedCount || !bValidSelection )
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
	BOOL bRetVal = false;

	// show dependents dialog
	if (pTaskObject)
	{
		SVGUID taskObjectID = pTaskObject->GetUniqueObjectID();
		SVGUID inspectionID = pTaskObject->GetInspection()->GetUniqueObjectID();
		INT_PTR rc = SvOg::SVShowDependentsDialog::StandardDialog( pTaskObject->GetName(), inspectionID, taskObjectID );

		bRetVal = ( IDCANCEL == rc ) ? false : true;
	}
	return bRetVal;
}

