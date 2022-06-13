//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "InspectionEngine/SVTaskObjectList.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "SVInspectionProcess.h"
#include "SVIPDoc.h"
#include "SVOGui/SVShowDependentsDialog.h"
#include "SVSetupDialogManager.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "SVStatusLibrary/MessageManager.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVOResource/ConstGlobalSvOr.h"
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

SVChildrenSetupDialog::SVChildrenSetupDialog(CWnd* pParent /*=nullptr*/)
: CDialog(SVChildrenSetupDialog::IDD, pParent)
, m_pAvailableChildrenList( nullptr )
, m_pParentObject( nullptr )
, m_pParentOwner( nullptr )
, m_AllowMultipleChildrenInstances( FALSE )
, m_pDocument (nullptr)
{
	//{{AFX_DATA_INIT(SVChildrenSetupDialog)
	//}}AFX_DATA_INIT
}

SVChildrenSetupDialog::~SVChildrenSetupDialog()
{
}

void SVChildrenSetupDialog::redrawLists()
{
	if( m_pAvailableChildrenList && m_pParentObject )
	{
		m_ChildrenListCtrl.DeleteAllItems();

		if( m_pParentObject )
		{
			if( m_pParentObject->GetSize() <= 0 )
			{
				std::string Empty = SvUl::LoadStdString( IDS_EMPTY_STRING );
				m_ChildrenListCtrl.SetItemData( m_ChildrenListCtrl.InsertItem( 0, Empty.c_str() ), 0 );
			}
			else
			{
				for( int i = 0; i < m_pParentObject->GetSize(); i++ )
				{
					if( m_pParentObject->getTaskObject( i ) )
					{
						m_ChildrenListCtrl.SetItemData( m_ChildrenListCtrl.InsertItem( i, m_pParentObject->getTaskObject( i )->GetName() ), reinterpret_cast<DWORD_PTR>(m_pParentObject->getTaskObject( i )) );
					}
					else
					{
						m_ChildrenListCtrl.SetItemData( m_ChildrenListCtrl.InsertItem( i, _T( "&&&&&&" ) ), 0 );
					}
				}
			}
		}

		m_AvailableChildrenListCtrl.DeleteAllItems();
		for( int i = 0; i < static_cast<int> (m_pAvailableChildrenList->size()); ++i )
		{
			SvIe::SVClassInfoStruct& rChildInfo = m_pAvailableChildrenList->at(i);

			// Make simple name comparison...
			LVFINDINFO findInfo;
			findInfo.flags = LVFI_STRING;
			findInfo.psz   = rChildInfo.m_ClassName.c_str();
			if( m_AllowMultipleChildrenInstances || m_ChildrenListCtrl.FindItem( &findInfo ) < 0 )
			{
				m_AvailableChildrenListCtrl.SetItemData( m_AvailableChildrenListCtrl.InsertItem( 0, rChildInfo.m_ClassName.c_str() ), static_cast<DWORD_PTR>(i) );
			}
		}

		GetAndEnableWindow( this, IDC_PUBLISH_BUTTON, FALSE );
		GetAndEnableWindow( this, IDC_SETUP_BUTTON, FALSE );
		GetAndEnableWindow( this, IDC_ADD_BUTTON, FALSE );
		GetAndEnableWindow( this, IDC_REMOVE_BUTTON, FALSE );

		UpdateData( FALSE ); // set data to dialog
	}
}

void SVChildrenSetupDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVChildrenSetupDialog)
	DDX_Control(pDX, IDC_CHILDREN_LIST, m_ChildrenListCtrl);
	DDX_Control(pDX, IDC_AVAILABLE_CHILDREN_LIST, m_AvailableChildrenListCtrl);
	//}}AFX_DATA_MAP
}

//******************************************************************************
// Message Map Entries
//******************************************************************************

BEGIN_MESSAGE_MAP(SVChildrenSetupDialog, CDialog)
	//{{AFX_MSG_MAP(SVChildrenSetupDialog)
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

BOOL SVChildrenSetupDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	if( nullptr != m_pAvailableChildrenList && nullptr != m_pParentObject && nullptr != (m_pParentOwner = m_pParentObject->GetParent()))
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
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void SVChildrenSetupDialog::OnAddButton()
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
			if( listIndex >= 0 && listIndex < static_cast<int> (m_pAvailableChildrenList->size()) )
			{
				// It´s a valid index ...

				SvIe::SVClassInfoStruct& rChildInfo = m_pAvailableChildrenList->at( listIndex );

				if (CreateSelectedResults(rChildInfo))
				{
					continue;
				}
				// Construct Children...
			}
		}

		// Refresh...
		redrawLists();
	}
}



bool SVChildrenSetupDialog::CreateSelectedResults(SvIe::SVClassInfoStruct& rChildInfo)
{
	// Construct Children...
	SvIe::SVTaskObjectClass* pObject = dynamic_cast<SvIe::SVTaskObjectClass*> (rChildInfo.Construct());
	if (nullptr != pObject)
	{
		// Add children to parent...
		m_pParentObject->Add(pObject);

		// Ensure this object's inputs get connected
		pObject->connectAllInputs();

		// And finally try to create the child object...
		if (!m_pParentObject->CreateChildObject(pObject, SvDef::SVMFSetDefaultInputs | SvDef::SVMFResetInspection))
		{
			SvDef::StringVector msgList;
			msgList.push_back(std::string(pObject->GetName()));
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_CreationFailed, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10043);

			// Remove it from the Tool TaskObjectList ( Destruct it )
			uint32_t objectID = pObject->getObjectId();
			if (SvDef::InvalidObjectId != objectID)
			{
				m_pParentObject->Delete(objectID);
			}
			else
			{
				delete pObject;
			}

			pObject = nullptr;
			return true;
		}
	}

	return false;
}


void SVChildrenSetupDialog::OnRemoveButton()
{
	if( m_pAvailableChildrenList && m_pParentObject )
	{
		int item = -1;
		SvIe::SVTaskObjectClass* pTaskObject{nullptr};
		// Get all selected items...
		while( ( item = m_ChildrenListCtrl.GetNextItem( item, LVNI_ALL | LVNI_SELECTED ) ) >= 0 )
		{
			pTaskObject = reinterpret_cast<SvIe::SVTaskObjectClass*> (m_ChildrenListCtrl.GetItemData( item ));
			//reinterpret_cast is dangerous, but a kind of or dynamic_cast afterwards do not help for more safety.
			if (nullptr != pTaskObject)
			{
				int rc = checkOkToDelete( pTaskObject );

				if( rc )
				{
					// Close, Disconnect and Delete Children...
					m_pParentObject->DestroyChildObject(pTaskObject, SvDef::SVMFSetDefaultInputs | SvDef::SVMFResetInspection );
				}
			}
		}

		// Refresh...
		redrawLists();
	}
}

void SVChildrenSetupDialog::OnCancel()
{
	CDialog::OnCancel();
}

void SVChildrenSetupDialog::OnOK()
{
	assert(m_pDocument);
	if(m_pDocument)
	{
		m_pDocument->SetModifiedFlag();   // Mark document as changed.
	}

	CDialog::OnOK();
}

void SVChildrenSetupDialog::OnSetupButton()
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
				SVSetupDialogManager::Instance().SetupDialog( pObject->GetClassID(), pObject->getObjectId(), this );
			}
		}

		// Refresh...
		redrawLists();
	}
}

void SVChildrenSetupDialog::OnPublishButton()
{
	if( nullptr == m_pParentObject || nullptr == m_pParentOwner ) { return; }

	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	*requestCmd.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
		{SvPb::SearchArea::toolsetItems}, m_pDocument->GetInspectionID(),
		SvPb::publishable, m_pParentObject->getObjectId());

	SvCmd::InspectionCommands(m_pDocument->GetInspectionID(), requestCmd, &responseCmd);
	SvOsl::ObjectTreeGenerator::Instance().setSelectorType(SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeMultipleObject, IDD_PUBLISHED_RESULTS + SvOr::HELPFILE_DLG_IDD_OFFSET);
	if (responseCmd.has_getobjectselectoritemsresponse())
	{
		SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects(responseCmd.getobjectselectoritemsresponse().tree());
	}

	std::string PublishableResults = SvUl::LoadStdString( IDS_PUBLISHABLE_RESULTS );
	std::string Title = SvUl::Format( _T("%s - %s"), PublishableResults.c_str(), m_pParentOwner->GetName() );
	std::string Filter = SvUl::LoadStdString( IDS_FILTER );
	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title.c_str(), PublishableResults.c_str(), Filter.c_str(), this );

	if( IDOK == Result )
	{
		for (auto const& rEntry : SvOsl::ObjectTreeGenerator::Instance().getModifiedObjects())
		{
			SVObjectReference ObjectRef{rEntry};
			bool previousState = SvPb::publishable == (SvPb::publishable & ObjectRef.ObjectAttributesSet());
			SvOi::SetAttributeType attributeType = previousState ? SvOi::SetAttributeType::RemoveAttribute : SvOi::SetAttributeType::AddAttribute;
			ObjectRef.SetObjectAttributesSet(SvPb::publishable, attributeType);
		}
		
		// refresh the publish list
		if( m_pDocument )
		{
			m_pDocument->RefreshPublishedList();
		}
	}
}

void SVChildrenSetupDialog::OnItemChangedChildrenList(NMHDR*, LRESULT* pResult)
{
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
		std::string ItemText = m_ChildrenListCtrl.GetItemText(0,0).GetString();
		std::string Empty = SvUl::LoadStdString( IDS_EMPTY_STRING );
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

void SVChildrenSetupDialog::OnItemChangedAvailableChildrenList(NMHDR*, LRESULT* pResult)
{
	BOOL enable = m_AvailableChildrenListCtrl.GetSelectedCount();
	GetAndEnableWindow( this, IDC_ADD_BUTTON, enable );

	UpdateData( FALSE ); // set data to dialog

	*pResult = 0;
}

BOOL SVChildrenSetupDialog::checkOkToDelete(SvIe::SVTaskObjectClass* pTaskObject)
{
	BOOL bRetVal = false;

	// show dependents dialog
	if (pTaskObject)
	{
		std::set<uint32_t> rIdsOfObjectsDependedOn({pTaskObject->getObjectId()});

		std::string FormatText = SvUl::LoadStdString(IDS_DELETE_CHECK_DEPENDENCIES);
		std::string DisplayText = SvUl::Format(FormatText.c_str(), pTaskObject->GetName(), pTaskObject->GetName(), pTaskObject->GetName(), pTaskObject->GetName());

		INT_PTR rc = SvOg::showDependentsDialogIfNecessary(rIdsOfObjectsDependedOn, DisplayText);

		bRetVal = ( IDCANCEL == rc ) ? false : true;
	}
	return bRetVal;
}

