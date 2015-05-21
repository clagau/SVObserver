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
#include <boost/config.hpp>
#include <boost/bind.hpp>
#include "SVRemoteOutputsView.h"

#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"

#include "SVConfigurationObject.h"
#include "SVIODoc.h"
#include "SVObserver.h"
#include "SVPPQObject.h"
#include "SVRemoteOutputDataController.h"
#include "SVRemoteOutputEditDialog.h"
#include "SVRemoteOutputGroup.h"
#include "SVSVIMStateClass.h"
#include "ErrorNumbers.h"
#include "SVStatusLibrary/ExceptionManager.h"
#include "TextDefinesSvO.h"
#pragma endregion Includes

IMPLEMENT_DYNCREATE(SVRemoteOutputsView, CListView )

SVRemoteOutputsView::SVRemoteOutputsView()
{
	m_pDocument=dynamic_cast<SVIODoc*>(CListView::GetDocument());
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

BEGIN_MESSAGE_MAP(SVRemoteOutputsView, CListView)
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_PLC_PROPERTIES, &SVRemoteOutputsView::OnRemoteOutputProperties)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PLC_ADD, &SVRemoteOutputsView::OnRemoteOutputAdd)
	ON_COMMAND(ID_PLC_DELETE, &SVRemoteOutputsView::OnRemoteOutputDelete)
	ON_COMMAND(ID_PLC_EDIT, &SVRemoteOutputsView::OnRemoteOutputEdit)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


#ifdef _DEBUG
void SVRemoteOutputsView::AssertValid() const
{
	CListView::AssertValid();
}

void SVRemoteOutputsView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

SVIODoc* SVRemoteOutputsView::GetDocument() // Die endgültige (nicht zur Fehlersuche kompilierte) Version ist Inline
{
	if( m_pDocument == NULL )
	{
		m_pDocument = dynamic_cast<SVIODoc*>(CListView::GetDocument());
	}
	return m_pDocument;
}

int SVRemoteOutputsView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->style |= LVS_SINGLESEL ;
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CListCtrl& lc = GetListCtrl();

	ImageList.Create( 16, 16, TRUE, 5, 5 );
	ImageList.Add( AfxGetApp()->LoadIcon( IDI_IOITEM_ICON ) );
	ImageList.Add( AfxGetApp()->LoadIcon( IDI_NOIOITEM_ICON ) );
	ImageList.Add( AfxGetApp()->LoadIcon( IDI_COLLAPSE ) );
	ImageList.Add( AfxGetApp()->LoadIcon( IDI_EXPAND ) );

	StateImageList.Create( 16, 16, TRUE, 2, 2 );
	StateImageList.Add( AfxGetApp()->LoadIcon( IDI_PPQ_ICON ) );
	StateImageList.Add( AfxGetApp()->LoadIcon( IDI_PLC_OUTPUT_ICON ) );


	lc.SetImageList( &StateImageList, LVSIL_STATE );
	lc.SetImageList( &ImageList, LVSIL_NORMAL );
	lc.SetImageList( &ImageList, LVSIL_SMALL );

	// insert columns
	lc.InsertColumn( 0, _T( "Remote Output" ), LVCFMT_LEFT, -1, -1 );
	lc.InsertColumn( 1, _T( "PPQ" ), LVCFMT_LEFT, -1, -1 );
	//lc.InsertColumn( 2, _T( "Size" ), LVCFMT_LEFT, -1, -1 );
	
	lc.SetColumnWidth( 0, 500 );
	lc.SetColumnWidth( 1, 125 );
	//lc.SetColumnWidth( 2,  50 );

	// We can show grid lines if we want to .
	// ListView_SetExtendedListViewStyle(m_hWnd, LVS_EX_GRIDLINES );

	lc.SetExtendedStyle( lc.GetExtendedStyle() | LVS_EX_FULLROWSELECT );
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

void SVRemoteOutputsView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	SVIODoc* pIODoc = GetDocument();

	SVConfigurationObject* pConfig = nullptr;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( pIODoc && ::IsWindow( m_hWnd ) )
	{
		GetListCtrl().SetRedraw(false);

		// First clean up list view
		GetListCtrl().DeleteAllItems();
		
		CString strItem;
		long lPPQSize;
		int j = 0;
		int k = 1;
		int iCurrentPPQ = 0;
		int l_PPQNum = 0;
	
		// Get the number of PPQs
		if( nullptr == pConfig || !pConfig->GetPPQCount( lPPQSize ) )
		{
			SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorGettingPPQCount, StdExceptionParams, Err_17051_SVRemoteOutputsView_OnUpdate_ErrorGettingPPQCount );
			DebugBreak();
		}

		// Check if any PPQs are here yet
		if( lPPQSize == 0 )
			return;

		// Result Outputs
		DWORD maxOutput = 0;
		// Get the DLL List...

		std::vector<CString> l_astrGroupNames;
		pConfig->GetRemoteOutputGroupNames( l_astrGroupNames );

		// This is a Hack! this should be called from a different area when objects change.
		pConfig->RemoteOutputValidateInputs();

		std::vector<CString>::iterator l_it;

		for( l_it = l_astrGroupNames.begin() ; l_it != l_astrGroupNames.end() ; ++l_it )
		{
			CString l_strGroupName = *l_it;

			int lSize = static_cast<int>(pConfig->GetRemoteOutputGroupItemCount( l_strGroupName ));
			if( lSize == 0 )
			{
				continue;
			}

			SVRemoteOutputGroup* l_pControl = pConfig->GetRemoteOutputGroup( l_strGroupName );
			if( l_pControl == NULL )
			{
				continue;
			}

			SVGroupStateMap::const_iterator l_Iter = m_GroupStates.find( l_strGroupName );

			bool l_bCollapse = ( l_Iter != m_GroupStates.end() ) && ( l_Iter->second );

			strItem.Format( _T( "%s" ),l_strGroupName );

			int ipos = GetListCtrl().InsertItem( LVIF_IMAGE | LVIF_TEXT | LVIF_STATE, 
				k, strItem, 
				INDEXTOSTATEIMAGEMASK( 1 ),	// state
				LVIS_STATEIMAGEMASK,		// stateMask
				2 + l_bCollapse,			// Image
				0 );	// Set item data

			GetListCtrl().SetItemData( ipos, reinterpret_cast<DWORD_PTR>(l_pControl) );

			GetListCtrl().SetItemText( ipos, 1, l_pControl->GetPPQName().c_str() );

			int lInsertedEntry = 0;

			if( !l_bCollapse )
			{
				// DLL outputs 
				for( j = 0; j < lSize; j++ )
				{
					SVRemoteOutputObject* l_pOutput = NULL;
					HRESULT l_hr = pConfig->GetRemoteOutputItem( l_strGroupName, j, l_pOutput);

					if( l_pOutput == NULL )
					{
						continue;
					}

					// First column: Result I/O
					CString l_strName = l_pOutput->GetInputValueObjectName().c_str();

					// Column: Description "Value Object Name"
					GetListCtrl().InsertItem( LVIF_IMAGE | LVIF_TEXT | LVIF_STATE, 
						lInsertedEntry + iCurrentPPQ + k, l_strName, 
						INDEXTOSTATEIMAGEMASK( 2 ),	// state
						LVIS_STATEIMAGEMASK,		// stateMask
						1,
						0);	// Set item data

					GetListCtrl().SetItem( lInsertedEntry + iCurrentPPQ + k, 0, LVIF_IMAGE, NULL, 0, 0, 0, 0 );

					// Store SVDLLOutputObject pointer in item data for editing
					GetListCtrl().SetItemData( lInsertedEntry + iCurrentPPQ + k, reinterpret_cast<DWORD>(l_pOutput) );


					// Column: Remote Output Destination Address ??????
					//CString l_strTargetID = "Target"
					//GetListCtrl().SetItemText( lInsertedEntry + iCurrentPPQ + k, 1, l_strTargetID );
					
					lInsertedEntry++;
				}// end for
			}

			iCurrentPPQ++;

			k += lInsertedEntry ;
			
		}// end for
		GetListCtrl().SetRedraw(true);
	}// end if
	
	//CListView::OnUpdate( pSender, lHint, pHint );   // This call will cause flicker
}

void SVRemoteOutputsView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CString strAddress;
	SVRemoteOutputObject* pRemoteOutput = nullptr;
	UINT flags;

	int l_item = GetListCtrl().HitTest( point, &flags );

	if ( l_item >= 0 && ((flags & LVHT_ONITEMLABEL) == LVHT_ONITEMLABEL) && ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&
		 TheSVObserverApp.OkToEdit() )
	{

		SVConfigurationObject* pConfig = nullptr;
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		long lPPQSize = 0;

		// Get the number of PPQs
		if( nullptr == pConfig || !pConfig->GetPPQCount( lPPQSize ) )
		{
			SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorGettingPPQCount, StdExceptionParams, Err_17052_SVRemoteOutputsView_OnLButtonDblClk_ErrorGettingPPQCount );
			DebugBreak();
		}

		SVSVIMStateClass::AddState( SV_STATE_EDITING );

		pRemoteOutput = dynamic_cast<SVRemoteOutputObject*>( reinterpret_cast<SVObjectClass*>(GetListCtrl().GetItemData( l_item )));
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
			SVRemoteOutputGroup* pOutputGroup = dynamic_cast<SVRemoteOutputGroup*>( reinterpret_cast<SVObjectClass*>(GetListCtrl().GetItemData( l_item )));
			if( pOutputGroup )
			{
				OnRemoteOutputProperties();
			}
		}

		SVSVIMStateClass::RemoveState( SV_STATE_EDITING );
		OnUpdate( NULL, NULL, NULL );
		GetListCtrl().SetItemState( l_item, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
	} // if ( item >= 0 && ! SVSVIMStateClass::CheckState
}// OnLButtonDblClk(

BOOL SVRemoteOutputsView::PreTranslateMessage(MSG* pMsg)
{
	BOOL l_bRet = FALSE;
	if(pMsg->message==WM_KEYDOWN && TheSVObserverApp.OkToEdit() )
	{
		POSITION l_Pos = GetListCtrl().GetFirstSelectedItemPosition();
		if( l_Pos != NULL )
		{
			int l_item = GetListCtrl().GetNextSelectedItem( l_Pos );
			DWORD_PTR l_pdwItemData = GetListCtrl().GetItemData( l_item );
			if( l_item >= 0 )
			{
				if(pMsg->wParam== VK_DELETE )
				{
					OnRemoteOutputDelete();
					l_bRet = TRUE;
				}
				else
				if(pMsg->wParam== VK_INSERT)
				{
					CString l_strGroupName;
					if( RemoteOutputGroupNameAtItem( l_strGroupName, l_item ) == S_OK )
					{
						// New Entry...
						SVSVIMStateClass::AddState( SV_STATE_EDITING );
						if( AddOutput( l_item ) )
						{
							SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
							l_item++;
						}
						SVSVIMStateClass::RemoveState( SV_STATE_EDITING );
						OnUpdate( NULL, NULL, NULL );
						GetListCtrl().SetItemState( l_item, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
						l_bRet = TRUE;
					}
				}
				else
				if(pMsg->wParam == VK_RETURN )
				{
					CString l_strGroupName;
					if( RemoteOutputGroupNameAtItem( l_strGroupName, l_item ) == S_OK )
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
						OnUpdate( NULL, NULL, NULL );
						GetListCtrl().SetItemState( l_item, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
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

						OnUpdate( NULL, NULL, NULL );
						LVFINDINFOA l_fi;
						l_fi.flags = LVFI_PARTIAL | LVFI_STRING;
						l_fi.psz = pOutputGroup->GetGroupName().c_str();
						int l_iPos = GetListCtrl().FindItem(&l_fi );
						if( l_iPos > -1 )
						{
							GetListCtrl().SetItemState( l_iPos, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
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

						OnUpdate( NULL, NULL, NULL );

						LVFINDINFOA l_fi;
						l_fi.flags = LVFI_PARTIAL | LVFI_STRING;
						l_fi.psz = pOutputGroup->GetGroupName().c_str();
						int l_iPos = GetListCtrl().FindItem(&l_fi );
						if( l_iPos > -1 )
						{
							GetListCtrl().SetItemState( l_iPos, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
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

HRESULT SVRemoteOutputsView::RemoteOutputGroupNameAtItem( CString& p_rstrGroupName, int p_iItem )
{
	HRESULT l_hr = S_FALSE;
	for( int i = p_iItem ; i > -1 ; i-- )
	{
		DWORD_PTR pdwItemData;
		pdwItemData = GetListCtrl().GetItemData( i );
		SVRemoteOutputObject* l_pOutput = dynamic_cast<SVRemoteOutputObject*>(reinterpret_cast<SVObjectClass*>(pdwItemData));
		if( l_pOutput != NULL )
		{
			p_rstrGroupName = l_pOutput->GetGroupID().c_str();
			l_hr = S_OK;
			break;
		}
		else
		{
			SVRemoteOutputGroup* l_pGroup = dynamic_cast<SVRemoteOutputGroup*>(reinterpret_cast<SVObjectClass*>(pdwItemData));
			if( l_pGroup != NULL )
			{
				p_rstrGroupName = l_pGroup->GetGroupName().c_str();
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
		POSITION l_Pos = GetListCtrl().GetFirstSelectedItemPosition();
		int l_item = -1;
		if( l_Pos )
		{
			l_item = GetListCtrl().GetNextSelectedItem(l_Pos);
		}
		SVSVIMStateClass::AddState( SV_STATE_EDITING );

		if( AddOutput( l_item ) )
		{
			SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
		}

		SVSVIMStateClass::RemoveState( SV_STATE_EDITING );
		OnUpdate( NULL, NULL, NULL );
		if( l_item > -1 )
		{
			GetListCtrl().SetItemState( l_item, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
		}
	}
}

void SVRemoteOutputsView::OnRemoteOutputProperties()
{
	if( TheSVObserverApp.OkToEdit() )
	{
		SVConfigurationObject* pConfig = NULL;
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		if( nullptr != pConfig){ pConfig->SetupRemoteOutput(); }
	}
}

void SVRemoteOutputsView::OnContextMenu(CWnd* /*pWnd*/, CPoint point )
{
	UINT nFlags = 0;
	CPoint ListPoint = point;

	GetListCtrl().ScreenToClient( &ListPoint);
	// store the item in this class
	m_CurrentItem = GetListCtrl().HitTest( ListPoint, &nFlags );

	if ( m_CurrentItem >= 0 )
	{
		SVRemoteOutputGroup* pOutputGroup = dynamic_cast<SVRemoteOutputGroup*>( reinterpret_cast<SVObjectClass*>(GetListCtrl().GetItemData( m_CurrentItem )));

		if( pOutputGroup )
		{
			// Context Menu...Add and Properties
			CMenu* pPopup = m_ContextMenuProp.GetSubMenu(0);
			ASSERT(pPopup != NULL);
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
			SVRemoteOutputObject* l_pOutput=dynamic_cast<SVRemoteOutputObject*>(reinterpret_cast<SVObjectClass*>(GetListCtrl().GetItemData( m_CurrentItem )));
			if( l_pOutput )
			{
				SVRemoteOutputObject* l_pPrevOutput = NULL;
				CMenu* pPopup;
				CWnd* pWndPopupOwner = this;
				if( m_CurrentItem > 1 )
				{
					l_pPrevOutput=dynamic_cast<SVRemoteOutputObject*>(reinterpret_cast<SVObjectClass*>(GetListCtrl().GetItemData( m_CurrentItem-1 )));
				}
				int pos = static_cast<int>(l_pOutput->GetInputValueObjectName().find( _T("Trigger Count") ));
				if( pos != SVString::npos && (l_pPrevOutput == NULL) ) // If the name is trigger count
				{
					pPopup = m_ContextMenuItemNoDelete.GetSubMenu(0);
				}
				else
				{
					pPopup = m_ContextMenuItem.GetSubMenu(0);
				}

				ASSERT(pPopup != NULL);
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
		POSITION l_Pos = GetListCtrl().GetFirstSelectedItemPosition();
		if( l_Pos != NULL )
		{
			SVConfigurationObject* pConfig( nullptr );
			SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

			int l_item = GetListCtrl().GetNextSelectedItem( l_Pos );
			SVRemoteOutputObject* pRemoteOutput = dynamic_cast<SVRemoteOutputObject*>( reinterpret_cast<SVObjectClass*>(GetListCtrl().GetItemData( l_item )));
			if( nullptr != pRemoteOutput && nullptr != pConfig )
			{
				// The user clicked on an output
				CString l_strRemoteGroup = pRemoteOutput->GetGroupID().c_str();// GetInputValueObjectName();

				bool bFirst = (pConfig->GetFirstRemoteOutputObject( l_strRemoteGroup ) == pRemoteOutput); 
				int pos = static_cast<int>(pRemoteOutput->GetInputValueObjectName().find(_T("Trigger Count") ));
				if( bFirst && pos != SVString::npos )
				{
					AfxMessageBox(_T("Trigger Count Cannot Be deleted"),MB_OK );
				}
				else
				{
					int l_iResponse = AfxMessageBox(_T("Are You Sure you want to delete this Output?"), MB_YESNO );
					if( l_iResponse == IDYES )
					{
						pConfig->DeleteRemoteOutputEntry( l_strRemoteGroup, pRemoteOutput );
						SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
//						TheSVObserverApp.m_DLLManager.Startup( TheSVObserverApp.m_pConfiguration );
						OnUpdate( NULL, NULL, NULL );
						if( l_item >= GetListCtrl().GetItemCount() )
						{
							l_item--;
							if( l_item < 0 )
								l_item = 0;
						}
						GetListCtrl().SetItemState( l_item, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
					}
				}
			}
			else
			{
				// The user clicked on a DLL
				// Check which DLL to delete.
				SVRemoteOutputGroup* pOutputGroup = dynamic_cast<SVRemoteOutputGroup*>( reinterpret_cast<SVObjectClass*>(GetListCtrl().GetItemData( l_item )));;
				if( pOutputGroup )
				{
					CString l_strGroup = pOutputGroup->GetGroupName().c_str();
					CString l_strMsg;
					l_strMsg.Format( _T("Are you sure you want to delete All Entries for %s"), l_strGroup );
					int imbRet = AfxMessageBox( l_strMsg, MB_YESNO );
					if( imbRet == IDYES )
					{
						// Delete all DLL entries
						pConfig->DeleteRemoteOutput( l_strGroup );
						SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
						OnUpdate( NULL, NULL, NULL );
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
		OnUpdate( NULL, NULL, NULL );
	}
}

bool SVRemoteOutputsView::AddOutput(int p_iWhere)
{
	bool l_bRet = false;

	// New Entry...
	SVRemoteOutputEditDialog dlg;
	CString l_strGroup;

	if( RemoteOutputGroupNameAtItem( l_strGroup, p_iWhere ) == S_OK)
	{
		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		SVRemoteOutputGroup* pGroupParameters( nullptr );
		if( nullptr != pConfig ){ pConfig->GetRemoteOutputGroup(l_strGroup); }

		SVPPQObject* pPPQ( nullptr );
		SVString PPQName;
		//If pointer is nullptr then PPQName stays empty
		if( nullptr != pGroupParameters){ PPQName = pGroupParameters->GetPPQName(); }
		if( !PPQName.empty() )
		{
			pConfig->GetChildObjectByName( PPQName.c_str(), &pPPQ );
			ASSERT( nullptr != pPPQ );
			if( nullptr != pPPQ )
			{ 
				dlg.m_InputObjectGUID = pPPQ->m_voTriggerCount.GetUniqueObjectID();
				dlg.m_strGroupName = l_strGroup;

				if( dlg.DoModal() == IDOK )
				{
					SVRemoteOutputObject* l_pNewOutput = NULL;
					pConfig->AddRemoteOutputItem(l_strGroup, 
						l_pNewOutput, 
						dlg.m_InputObjectGUID,
						PPQName.c_str());
					OnUpdate( NULL, NULL, NULL );
					l_bRet = true;
				}
				if( p_iWhere >= GetListCtrl().GetItemCount() )
				{
					p_iWhere--;
					if( p_iWhere < 0 )
						p_iWhere = 0;
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


	SVRemoteOutputObject* pRemoteOutput=NULL;
	pRemoteOutput = dynamic_cast<SVRemoteOutputObject*>( reinterpret_cast<SVObjectClass*>(GetListCtrl().GetItemData( p_iWhere )));
	if( pRemoteOutput )
	{
		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		// The User clicked on the Item
		dlg.m_strGroupName = pRemoteOutput->GetGroupID().c_str();
		dlg.m_strValueObjectSourceName = pRemoteOutput->GetInputValueObjectName().c_str();
		pRemoteOutput->GetInputValueObjectGUID( dlg.m_InputObjectGUID ); 

		// if this is the first (Trigger Count) item in the list then gray out the object.
		if(nullptr != pConfig && pConfig->GetFirstRemoteOutputObject( dlg.m_strGroupName ) == pRemoteOutput )
		{
			return l_bRet;
		}
	}

	switch( dlg.DoModal() )
	{
		case IDOK:
		{
			pRemoteOutput->SetInputObjectId( dlg.m_InputObjectGUID );
			pRemoteOutput->SetGroupID( dlg.m_strGroupName );
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

	int l_item = GetListCtrl().HitTest( point, &uFlags);
	if( (uFlags & LVHT_ONITEMSTATEICON) || (uFlags & LVHT_ONITEMICON) )
	{
		// Toggle Collapse.
		if( l_item > -1 )
		{
			DWORD_PTR l_pdwItemData = GetListCtrl().GetItemData( l_item );
			SVRemoteOutputGroup* pOutputGroup = dynamic_cast<SVRemoteOutputGroup*>( reinterpret_cast<SVObjectClass*>(l_pdwItemData));;
			if( pOutputGroup )
			{
				SVGroupStateMap::const_iterator l_Iter = m_GroupStates.find( pOutputGroup->GetGroupName().c_str() );
				bool l_bCollapse = ( l_Iter != m_GroupStates.end() ) && ( l_Iter->second );

				m_GroupStates[ pOutputGroup->GetGroupName().c_str() ] = !l_bCollapse;

				OnUpdate( NULL, NULL, NULL );
				LVFINDINFOA l_fi;
				l_fi.flags = LVFI_PARTIAL | LVFI_STRING;		
				l_fi.psz = pOutputGroup->GetGroupName().c_str();
				int l_iPos = GetListCtrl().FindItem(&l_fi );
				if( l_iPos > -1 )
				{
					GetListCtrl().SetItemState( l_iPos, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
				}
			}
		}
		
	}
	CListView::OnLButtonDown(nFlags, point);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVRemoteOutputsView.cpp_v  $
 * 
 *    Rev 1.3   12 Jun 2014 16:11:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised IDI_PPQ_EXPAND to IDI_EXPAND
 * Revised IDI_PPQ_COLLAPSE to IDI_COLLAPSE
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Oct 2013 08:17:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   23 Jul 2013 18:14:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Revised destructor to fix resource leak
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:54:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   18 Sep 2012 18:32:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   14 Aug 2012 11:04:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix update issues with remote outputs view.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   07 Aug 2012 16:18:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated views to remove flicker.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   09 Jul 2012 14:15:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the include for SVIM State.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   02 Jul 2012 17:33:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   19 Jun 2012 14:15:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * Centralized the location of the configuration object identifier and removed synchronization problems.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   06 Mar 2012 11:44:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed issues associated with PVCS headers and footers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/