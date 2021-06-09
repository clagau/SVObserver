//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteOutputsView
//* .File Name       : $Workfile:   SVRemoteOutputsView.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   12 Jun 2014 16:11:36  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVRemoteOutputsView.h"
#include "SVConfigurationObject.h"
#include "SVIODoc.h"
#include "SVRemoteOutputDataController.h"
#include "SVRemoteOutputEditDialog.h"
#include "SVRemoteOutputGroup.h"
#include "SVObserver.h"
#include "SVPPQObject.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/StringTypeDef.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVOResource/ConstGlobalSvOr.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#pragma endregion Includes

IMPLEMENT_DYNCREATE(SVRemoteOutputsView, CListView )

BEGIN_MESSAGE_MAP(SVRemoteOutputsView, CListView)
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_REMOTE_OUTPUT_PROPERTIES, &SVRemoteOutputsView::OnRemoteOutputProperties)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_REMOTE_OUTPUT_ADD, &SVRemoteOutputsView::OnRemoteOutputAdd)
	ON_COMMAND(ID_REMOTE_OUTPUT_DELETE, &SVRemoteOutputsView::OnRemoteOutputDelete)
	ON_COMMAND(ID_REMOTE_OUTPUT_EDIT, &SVRemoteOutputsView::OnRemoteOutputEdit)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

SVRemoteOutputsView::SVRemoteOutputsView() :
	m_rCtrl(GetListCtrl())
{
	VERIFY(m_ContextMenuProp.LoadMenu(IDR_REMOTE_OUTPUT_CONTEXT_MENU_PROP));
	VERIFY(m_ContextMenuItem.LoadMenu(IDR_REMOTE_OUTPUT_CONTEXT_MENU_ITEM));
	VERIFY(m_ContextMenuItemNoDelete.LoadMenu(IDR_REMOTE_OUTPUT_CONTEXT_MENU_ITEM_NO_DELETE));
}

SVRemoteOutputsView::~SVRemoteOutputsView()
{
	m_ContextMenuProp.DestroyMenu();
	m_ContextMenuItem.DestroyMenu();
	m_ContextMenuItemNoDelete.DestroyMenu();
}

int SVRemoteOutputsView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->style |= LVS_SINGLESEL ;
	if (CListView::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	m_ImageList.Create( SvOr::IconSize, SvOr::IconSize, ILC_COLOR | ILC_MASK, 4, 1 );
	m_ImageList.Add( AfxGetApp()->LoadIcon( IDI_IOITEM_ICON ) );
	m_ImageList.Add( AfxGetApp()->LoadIcon( IDI_NOIOITEM_ICON ) );
	m_ImageList.Add( AfxGetApp()->LoadIcon( IDI_COLLAPSE ) );
	m_ImageList.Add( AfxGetApp()->LoadIcon( IDI_EXPAND ) );

	m_StateImageList.Create( SvOr::IconSize, SvOr::IconSize, ILC_COLOR | ILC_MASK, 2, 1 );
	m_StateImageList.Add( AfxGetApp()->LoadIcon( IDI_PPQ_ICON ) );
	m_StateImageList.Add( AfxGetApp()->LoadIcon( IDI_REMOTE_OUTPUT_ICON ) );

	m_rCtrl.SetImageList( &m_StateImageList, LVSIL_STATE );
	m_rCtrl.SetImageList( &m_ImageList, LVSIL_SMALL );

	// insert columns
	m_rCtrl.InsertColumn( 0, _T( "Remote Output" ), LVCFMT_LEFT, -1, -1 );
	m_rCtrl.InsertColumn( 1, _T( "PPQ" ), LVCFMT_LEFT, -1, -1 );
		
	m_rCtrl.SetColumnWidth( 0, 500 );
	m_rCtrl.SetColumnWidth( 1, 125 );
	
	m_rCtrl.SetExtendedStyle( m_rCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT );

	m_pDocument = dynamic_cast<SVIODoc*>(CListView::GetDocument());
	return 0;
}

BOOL SVRemoteOutputsView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |= (LVS_SHOWSELALWAYS | LVS_REPORT | 
               LVS_NOSORTHEADER);
	cs.style &= ~LVS_SORTASCENDING;
	cs.style &= ~LVS_SORTDESCENDING;
	cs.dwExStyle |= LVS_EX_GRIDLINES;

	return CListView::PreCreateWindow(cs);
}

void SVRemoteOutputsView::OnUpdate(CView* , LPARAM , CObject* )
{
	SVConfigurationObject* pConfig = nullptr;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if(nullptr != m_pDocument && ::IsWindow( m_hWnd ) )
	{
		m_rCtrl.SetRedraw(false);

		// First clean up list view
		m_rCtrl.DeleteAllItems();
		
		long lPPQSize = 0;
		int j = 0;
		int k = 1;
		int iCurrentPPQ = 0;
	
		// Get the number of PPQs
		if( nullptr == pConfig  )
		{
			SvStl::MessageManager e(SvStl::MsgType::Log );
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingPPQCount, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17051_ErrorGettingPPQCount );
			DebugBreak();
		}
		else
		{
			lPPQSize = pConfig->GetPPQCount( );
		}

		// Check if any PPQs are here yet
		if( lPPQSize == 0 )
			return;

		// Get the DLL List...

		SvDef::StringVector GroupNames;
		pConfig->GetRemoteOutputGroupNames( GroupNames );

		// This is a Hack! this should be called from a different area when objects change.
		pConfig->RemoteOutputValidateInputs();

		SvDef::StringVector::const_iterator Iter;

		for( Iter = GroupNames.begin() ; Iter != GroupNames.end() ; ++Iter )
		{
			const std::string& rGroupName = *Iter;

			int lSize = static_cast<int>(pConfig->GetRemoteOutputGroupItemCount( rGroupName ));
			if( lSize == 0 )
			{
				continue;
			}

			SVRemoteOutputGroup* l_pControl = pConfig->GetRemoteOutputGroup( rGroupName );
			if( nullptr == l_pControl )
			{
				continue;
			}

			const auto l_Iter = m_GroupStates.find( rGroupName );

			bool l_bCollapse = ( l_Iter != m_GroupStates.end() ) && ( l_Iter->second );

			int ipos = m_rCtrl.InsertItem( LVIF_IMAGE | LVIF_TEXT | LVIF_STATE, 
				k, rGroupName.c_str(), 
				INDEXTOSTATEIMAGEMASK( 1 ),	// state
				LVIS_STATEIMAGEMASK,		// stateMask
				2 + l_bCollapse,			// Image
				0 );	// Set item data

			m_rCtrl.SetItemData( ipos, reinterpret_cast<DWORD_PTR>(l_pControl) );

			m_rCtrl.SetItemText( ipos, 1, l_pControl->GetPPQName().c_str() );

			int lInsertedEntry = 0;

			if( !l_bCollapse )
			{
				// DLL outputs 
				for( j = 0; j < lSize; j++ )
				{
					SVRemoteOutputObject* l_pOutput = nullptr;
					pConfig->GetRemoteOutputItem( rGroupName, j, l_pOutput);

					if( nullptr == l_pOutput )
					{
						continue;
					}

					// First column: Result I/O
					std::string Name = l_pOutput->GetInputValueObjectName();

					// Column: Description "Value Object Name"
					m_rCtrl.InsertItem( LVIF_IMAGE | LVIF_TEXT | LVIF_STATE, 
						lInsertedEntry + iCurrentPPQ + k, Name.c_str(), 
						INDEXTOSTATEIMAGEMASK( 2 ),	// state
						LVIS_STATEIMAGEMASK,		// stateMask
						1,
						0);	// Set item data

					m_rCtrl.SetItem( lInsertedEntry + iCurrentPPQ + k, 0, LVIF_IMAGE, nullptr, 0, 0, 0, 0 );

					// Store SVDLLOutputObject pointer in item data for editing
					m_rCtrl.SetItemData( lInsertedEntry + iCurrentPPQ + k, reinterpret_cast<DWORD_PTR>(l_pOutput) );
					
					lInsertedEntry++;
				}// end for
			}

			iCurrentPPQ++;

			k += lInsertedEntry ;
			
		}// end for
		m_rCtrl.SetRedraw(true);
	}// end if
	
	//CListView::OnUpdate( pSender, lHint, pHint );   // This call will cause flicker
}

void SVRemoteOutputsView::OnLButtonDblClk(UINT, CPoint point)
{
	CString strAddress;
	SVRemoteOutputObject* pRemoteOutput = nullptr;
	UINT flags;

	int l_item = m_rCtrl.HitTest( point, &flags );

	if ( l_item >= 0 && ((flags & LVHT_ONITEMLABEL) == LVHT_ONITEMLABEL) && ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&
		 TheSVObserverApp.OkToEdit() )
	{
		SVSVIMStateClass::AddState( SV_STATE_EDITING );

		pRemoteOutput = dynamic_cast<SVRemoteOutputObject*>( reinterpret_cast<SVObjectClass*>(m_rCtrl.GetItemData( l_item )));
		if( pRemoteOutput )
		{
			// The User clicked on the Item
			if( EditOutput( l_item ) )
			{
				SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
			}
		}
		else
		{	// The user clicked on the PPQ which means a new object.

			// Edit DLL Properties.
			SVRemoteOutputGroup* pOutputGroup = dynamic_cast<SVRemoteOutputGroup*>( reinterpret_cast<SVObjectClass*>(m_rCtrl.GetItemData( l_item )));
			if( pOutputGroup )
			{
				OnRemoteOutputProperties();
			}
		}

		SVSVIMStateClass::RemoveState( SV_STATE_EDITING );
		OnUpdate( nullptr, 0, nullptr );
		m_rCtrl.SetItemState( l_item, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
	} // if ( item >= 0 && ! SVSVIMStateClass::CheckState
}// OnLButtonDbm_rCtrllk(

BOOL SVRemoteOutputsView::PreTranslateMessage(MSG* pMsg)
{
	BOOL l_bRet = FALSE;
	if(pMsg->message==WM_KEYDOWN && TheSVObserverApp.OkToEdit() )
	{
		POSITION l_Pos = m_rCtrl.GetFirstSelectedItemPosition();
		if( nullptr != l_Pos )
		{
			int l_item = m_rCtrl.GetNextSelectedItem( l_Pos );
			DWORD_PTR l_pdwItemData = m_rCtrl.GetItemData( l_item );
			if( l_item >= 0 )
			{
				if( VK_DELETE == pMsg->wParam )
				{
					OnRemoteOutputDelete();
					l_bRet = TRUE;
				}
				else
				if( VK_INSERT == pMsg->wParam )
				{
					std::string GroupName;
					if( S_OK == RemoteOutputGroupNameAtItem( GroupName, l_item )  )
					{
						// New Entry...
						SVSVIMStateClass::AddState( SV_STATE_EDITING );
						if( AddOutput( l_item ) )
						{
							SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
							l_item++;
						}
						SVSVIMStateClass::RemoveState( SV_STATE_EDITING );
						OnUpdate( nullptr, 0, nullptr );
						m_rCtrl.SetItemState( l_item, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
						l_bRet = TRUE;
					}
				}
				else
				if( VK_RETURN == pMsg->wParam )
				{
					std::string GroupName;
					if( S_OK == RemoteOutputGroupNameAtItem( GroupName, l_item )  )
					{
						// New Entry...
						SVSVIMStateClass::AddState( SV_STATE_EDITING );
						SVRemoteOutputObject* pRemoteOutput = dynamic_cast<SVRemoteOutputObject*>( reinterpret_cast<SVObjectClass*>(l_pdwItemData));
						if( pRemoteOutput )
						{
							if( EditOutput( l_item ) )
							{
								SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
							}
						}
						else
						{
							// Edit DLL Properties.
							OnRemoteOutputProperties();
						}
						SVSVIMStateClass::RemoveState( SV_STATE_EDITING );
						OnUpdate( nullptr, 0, nullptr );
						m_rCtrl.SetItemState( l_item, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
						l_bRet = TRUE;
					}
				}
				else
				if(pMsg->wParam == VK_ADD )
				{
					// Expand
					SVRemoteOutputGroup* pOutputGroup = dynamic_cast<SVRemoteOutputGroup*>( reinterpret_cast<SVObjectClass*>(l_pdwItemData));
					if( pOutputGroup )
					{
						m_GroupStates[ pOutputGroup->GetGroupName().c_str() ] = false;

						OnUpdate( nullptr, 0, nullptr );
						LVFINDINFOA l_fi;
						l_fi.flags = LVFI_PARTIAL | LVFI_STRING;
						l_fi.psz = pOutputGroup->GetGroupName().c_str();
						int l_iPos = m_rCtrl.FindItem(&l_fi );
						if( l_iPos > -1 )
						{
							m_rCtrl.SetItemState( l_iPos, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
						}
						l_bRet = TRUE;
					}
				}
				if(pMsg->wParam == VK_SUBTRACT )
				{
					// Collapse
					SVRemoteOutputGroup* pOutputGroup = dynamic_cast<SVRemoteOutputGroup*>( reinterpret_cast<SVObjectClass*>(l_pdwItemData));
					if( pOutputGroup )
					{
						m_GroupStates[ pOutputGroup->GetGroupName().c_str() ] = true;

						OnUpdate( nullptr, 0, nullptr );

						LVFINDINFOA l_fi;
						l_fi.flags = LVFI_PARTIAL | LVFI_STRING;
						l_fi.psz = pOutputGroup->GetGroupName().c_str();
						int l_iPos = m_rCtrl.FindItem(&l_fi );
						if( l_iPos > -1 )
						{
							m_rCtrl.SetItemState( l_iPos, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
						}
						l_bRet = TRUE;
					}
				}
			}
		}
    }
	if( !l_bRet )
	{
		l_bRet = CListView::PreTranslateMessage(pMsg);
	}
	return l_bRet;
}

HRESULT SVRemoteOutputsView::RemoteOutputGroupNameAtItem( std::string& rGroupName, int p_iItem )
{
	HRESULT l_hr = S_FALSE;
	for( int i = p_iItem ; i > -1 ; i-- )
	{
		DWORD_PTR pdwItemData;
		pdwItemData = m_rCtrl.GetItemData( i );
		SVRemoteOutputObject* l_pOutput = dynamic_cast<SVRemoteOutputObject*>(reinterpret_cast<SVObjectClass*>(pdwItemData));
		if( nullptr != l_pOutput )
		{
			rGroupName = l_pOutput->GetGroupID();
			l_hr = S_OK;
			break;
		}
		else
		{
			SVRemoteOutputGroup* l_pGroup = dynamic_cast<SVRemoteOutputGroup*>(reinterpret_cast<SVObjectClass*>(pdwItemData));
			if( nullptr != l_pGroup )
			{
				rGroupName = l_pGroup->GetGroupName();
				l_hr = S_OK;
				break;
			}
		}
	}
	return l_hr;
}

void SVRemoteOutputsView::OnRemoteOutputAdd()
{
	if( TheSVObserverApp.OkToEdit() )
	{
		POSITION l_Pos = m_rCtrl.GetFirstSelectedItemPosition();
		int l_item = -1;
		if( l_Pos )
		{
			l_item = m_rCtrl.GetNextSelectedItem(l_Pos);
		}
		SVSVIMStateClass::AddState( SV_STATE_EDITING );

		if( AddOutput( l_item ) )
		{
			SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
		}

		SVSVIMStateClass::RemoveState( SV_STATE_EDITING );
		OnUpdate( nullptr, 0, nullptr );
		if( l_item > -1 )
		{
			m_rCtrl.SetItemState( l_item, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
		}
	}
}

void SVRemoteOutputsView::OnRemoteOutputProperties()
{
	if( TheSVObserverApp.OkToEdit() )
	{
		SVConfigurationObject* pConfig = nullptr;
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		if( nullptr != pConfig){ pConfig->SetupRemoteOutput(); }
	}
}

void SVRemoteOutputsView::OnContextMenu(CWnd* /*pWnd*/, CPoint point )
{
	UINT nFlags = 0;
	CPoint ListPoint = point;

	m_rCtrl.ScreenToClient( &ListPoint);
	// store the item in this class
	m_CurrentItem = m_rCtrl.HitTest( ListPoint, &nFlags );

	if ( m_CurrentItem >= 0 )
	{
		SVRemoteOutputGroup* pOutputGroup = dynamic_cast<SVRemoteOutputGroup*>( reinterpret_cast<SVObjectClass*>(m_rCtrl.GetItemData( m_CurrentItem )));

		if( pOutputGroup )
		{
			// Context Menu...Add and Properties
			CMenu* pPopup = m_ContextMenuProp.GetSubMenu(0);
			assert(nullptr != pPopup);
			CWnd* pWndPopupOwner = this;

			while(pWndPopupOwner->GetStyle() & WS_CHILD)
			{
				pWndPopupOwner = pWndPopupOwner->GetParent();
			}

			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
									point.x, 
									point.y,
									pWndPopupOwner);
		}
		else
		{
			// Context Menu...Add and Properties
			SVRemoteOutputObject* l_pOutput=dynamic_cast<SVRemoteOutputObject*>(reinterpret_cast<SVObjectClass*>(m_rCtrl.GetItemData( m_CurrentItem )));
			if( l_pOutput )
			{
				SVRemoteOutputObject* l_pPrevOutput = nullptr;
				CMenu* pPopup;
				CWnd* pWndPopupOwner = this;
				if( m_CurrentItem > 1 )
				{
					l_pPrevOutput=dynamic_cast<SVRemoteOutputObject*>(reinterpret_cast<SVObjectClass*>(m_rCtrl.GetItemData( m_CurrentItem-1 )));
				}
				size_t pos = l_pOutput->GetInputValueObjectName().find( _T("Trigger Count") );
				if( pos != std::string::npos && (nullptr == l_pPrevOutput) ) // If the name is trigger count
				{
					pPopup = m_ContextMenuItemNoDelete.GetSubMenu(0);
				}
				else
				{
					pPopup = m_ContextMenuItem.GetSubMenu(0);
				}

				assert(nullptr != pPopup);
				while(pWndPopupOwner->GetStyle() & WS_CHILD)
				{
					pWndPopupOwner = pWndPopupOwner->GetParent();
				}

				pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
											point.x, 
											point.y,
											pWndPopupOwner);
			}
		}
	}
}

void SVRemoteOutputsView::OnRemoteOutputDelete()
{
	if( TheSVObserverApp.OkToEdit() )
	{
		POSITION l_Pos = m_rCtrl.GetFirstSelectedItemPosition();
		if( nullptr != l_Pos )
		{
			SVConfigurationObject* pConfig( nullptr );
			SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

			int l_item = m_rCtrl.GetNextSelectedItem( l_Pos );
			SVRemoteOutputObject* pRemoteOutput = dynamic_cast<SVRemoteOutputObject*>( reinterpret_cast<SVObjectClass*>(m_rCtrl.GetItemData( l_item )));
			if( nullptr != pRemoteOutput && nullptr != pConfig )
			{
				std::string RemoteGroup = pRemoteOutput->GetGroupID();

				bool bFirst = (pConfig->GetFirstRemoteOutputObject( RemoteGroup ) == pRemoteOutput); 
				size_t pos = pRemoteOutput->GetInputValueObjectName().find(SvDef::FqnPpqTriggerCount);
				if( bFirst && pos != std::string::npos )
				{
					SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RemoteOutput_TriggerCountDeleteError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10194 ); 
				}
				else
				{
					SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
					INT_PTR result = Msg.setMessage(SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_RemoteOutput_DeletingOutput, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10195, SvDef::InvalidObjectId, MB_YESNO);
					if( IDYES == result )
					{
						pConfig->DeleteRemoteOutputEntry( RemoteGroup, pRemoteOutput );
						SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
						OnUpdate( nullptr, 0, nullptr );
						if( l_item >= m_rCtrl.GetItemCount() )
						{
							l_item--;
							if( l_item < 0 )
								l_item = 0;
						}
						m_rCtrl.SetItemState( l_item, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
					}
				}
			}
			else
			{
				// The user clicked on a DLL
				// Check which DLL to delete.
				SVRemoteOutputGroup* pOutputGroup = dynamic_cast<SVRemoteOutputGroup*>( reinterpret_cast<SVObjectClass*>(m_rCtrl.GetItemData( l_item )));;
				if( pOutputGroup )
				{
					std::string strGroup = pOutputGroup->GetGroupName();
					SvDef::StringVector msgList;
					msgList.push_back(strGroup);
					SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
					INT_PTR result = Msg.setMessage( SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_RemoteOutput_DeletingAllOutput, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10196, SvDef::InvalidObjectId, MB_YESNO );
					if( IDYES == result )
					{
						// Delete all DLL entries
						pConfig->DeleteRemoteOutput( strGroup.c_str() );
						SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
						OnUpdate( nullptr, 0, nullptr );
					}
				}
			}
		}
	}
}

void SVRemoteOutputsView::OnRemoteOutputEdit()
{
	if( TheSVObserverApp.OkToEdit() )
	{
		SVSVIMStateClass::AddState( SV_STATE_EDITING );

		EditOutput(m_CurrentItem);
		
		SVSVIMStateClass::RemoveState( SV_STATE_EDITING );
		OnUpdate( nullptr, 0, nullptr );
	}
}

bool SVRemoteOutputsView::AddOutput(int p_iWhere)
{
	bool l_bRet = false;

	// New Entry...
	std::string RemoteGroup;
	SVRemoteOutputEditDialog dlg;

	if( S_OK == RemoteOutputGroupNameAtItem( RemoteGroup, p_iWhere ) )
	{
		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		SVRemoteOutputGroup* pGroupParameters( nullptr );
		if( nullptr != pConfig ){ pGroupParameters = pConfig->GetRemoteOutputGroup(RemoteGroup); }

		SVPPQObject* pPPQ( nullptr );
		std::string PPQName;
		//If pointer is nullptr then PPQName stays empty
		if( nullptr != pGroupParameters){ PPQName = pGroupParameters->GetPPQName(); }
		if( !PPQName.empty() )
		{
			pConfig->GetChildObjectByName( PPQName.c_str(), &pPPQ );
			assert( nullptr != pPPQ );
			if( nullptr != pPPQ )
			{ 
				SvVol::BasicValueObjectPtr pPpqTriggerCount = pPPQ->getPpqVaraible(SvDef::FqnPpqTriggerCount);
				dlg.m_InputObjectID = (nullptr != pPpqTriggerCount) ? pPpqTriggerCount->getObjectId() : SvDef::InvalidObjectId;
				dlg.m_GroupName = RemoteGroup;

				if( dlg.DoModal() == IDOK )
				{
					SVRemoteOutputObject* l_pNewOutput = nullptr;
					pConfig->AddRemoteOutputItem(RemoteGroup, 
						l_pNewOutput, 
						dlg.m_InputObjectID,
						PPQName.c_str());
					OnUpdate( nullptr, 0, nullptr );
					l_bRet = true;
				}
			}
		}
	}
	return l_bRet;
}

bool SVRemoteOutputsView::EditOutput(int p_iWhere)
{
	bool l_bRet = false;

	SVRemoteOutputEditDialog dlg;


	SVRemoteOutputObject* pRemoteOutput = nullptr;
	pRemoteOutput = dynamic_cast<SVRemoteOutputObject*>( reinterpret_cast<SVObjectClass*>(m_rCtrl.GetItemData( p_iWhere )));
	if( pRemoteOutput )
	{
		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		// The User clicked on the Item
		dlg.m_GroupName = pRemoteOutput->GetGroupID();
		dlg.m_ValueObjectSourceName = pRemoteOutput->GetInputValueObjectName();
		dlg.m_InputObjectID = pRemoteOutput->GetInputValueObjectID(  );

		// if this is the first (Trigger Count) item in the list then gray out the object.
		if(nullptr != pConfig && pConfig->GetFirstRemoteOutputObject( dlg.m_GroupName ) == pRemoteOutput )
		{
			return l_bRet;
		}
	}

	switch( dlg.DoModal() )
	{
		case IDOK:
		{
			pRemoteOutput->SetInputObjectId( dlg.m_InputObjectID );
			pRemoteOutput->SetGroupID( dlg.m_GroupName );
			l_bRet = true;
			break;
		}
		case IDCANCEL:
		default:
		{
			break;
		}
	}
	return l_bRet;
}

// Collapse and Expand logic if the user clicks on the PPQ item in the list.
void SVRemoteOutputsView::OnLButtonDown(UINT nFlags, CPoint point)
{
	UINT uFlags;

	int l_item = m_rCtrl.HitTest( point, &uFlags);
	if( (uFlags & LVHT_ONITEMSTATEICON) || (uFlags & LVHT_ONITEMICON) )
	{
		// Toggle Collapse.
		if( l_item > -1 )
		{
			DWORD_PTR l_pdwItemData = m_rCtrl.GetItemData( l_item );
			SVRemoteOutputGroup* pOutputGroup = dynamic_cast<SVRemoteOutputGroup*>( reinterpret_cast<SVObjectClass*>(l_pdwItemData));;
			if( pOutputGroup )
			{
				const auto l_Iter = m_GroupStates.find( pOutputGroup->GetGroupName().c_str() );
				bool l_bCollapse = ( l_Iter != m_GroupStates.end() ) && ( l_Iter->second );

				m_GroupStates[ pOutputGroup->GetGroupName().c_str() ] = !l_bCollapse;

				OnUpdate( nullptr, 0, nullptr );
				LVFINDINFOA l_fi;
				l_fi.flags = LVFI_PARTIAL | LVFI_STRING;		
				l_fi.psz = pOutputGroup->GetGroupName().c_str();
				int l_iPos = m_rCtrl.FindItem(&l_fi );
				if( l_iPos > -1 )
				{
					m_rCtrl.SetItemState( l_iPos, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
				}
			}
		}
		
	}
	CListView::OnLButtonDown(nFlags, point);
}

