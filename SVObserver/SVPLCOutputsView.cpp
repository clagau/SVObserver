//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPLCOutputsView
//* .File Name       : $Workfile:   SVPLCOutputsView.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   12 Jun 2014 16:11:36  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#ifndef _WIN64
#include "SVObserver.h"
#include "SVPLCOutputsView.h"
#include "SVPPQObject.h"
#include "SVIODoc.h"
#include "SVPLCOutput.h"
#include "SVPLCDataController.h"
#include "SVConfigurationObject.h"
#include "SVPLCOutputEditDialog.h"
#include "SVSVIMStateClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVPLCPropSheet.h"
#include "SVPLCPropPageStart.h"
#include "SVPLCPropPageList.h"
#include <boost/config.hpp>
#include <boost/bind.hpp>
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVPLCAddRemoveDlg.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVStatusLibrary/ExceptionManager.h"
#include "TextDefinesSvO.h"
#pragma endregion Includes

IMPLEMENT_DYNCREATE(SVPLCOutputsView, CListView )

SVPLCOutputsView::SVPLCOutputsView()
{
	m_pDocument=dynamic_cast<SVIODoc*>(CListView::GetDocument());
	VERIFY(m_ContextMenuProp.LoadMenu(IDR_PLC_CONTEXT_MENU_PROP));
	VERIFY(m_ContextMenuItem.LoadMenu(IDR_PLC_CONTEXT_MENU_ITEM));
	VERIFY(m_ContextMenuItemNoDelete.LoadMenu(IDR_PLC_CONTEXT_MENU_ITEM_NO_DELETE));
	
}

SVPLCOutputsView::~SVPLCOutputsView()
{
	m_ContextMenuProp.DestroyMenu();
	m_ContextMenuItem.DestroyMenu();
	m_ContextMenuItemNoDelete.DestroyMenu();
}

BEGIN_MESSAGE_MAP(SVPLCOutputsView, CListView)
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_PLC_PROPERTIES, &SVPLCOutputsView::OnPlcProperties)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PLC_ADD, &SVPLCOutputsView::OnPlcAdd)
	ON_COMMAND(ID_PLC_DELETE, &SVPLCOutputsView::OnPlcDelete)
	ON_COMMAND(ID_PLC_EDIT, &SVPLCOutputsView::OnPlcEdit)
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_ADD_TRANSFER_BLOCK, &SVPLCOutputsView::OnAddTransferBlock)
END_MESSAGE_MAP()

#ifdef _DEBUG
void SVPLCOutputsView::AssertValid() const
{
	CListView::AssertValid();
}

void SVPLCOutputsView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

SVIODoc* SVPLCOutputsView::GetDocument() // Die endgültige (nicht zur Fehlersuche kompilierte) Version ist Inline
{
	if( m_pDocument == NULL )
	{
		m_pDocument = dynamic_cast<SVIODoc*>(CListView::GetDocument());
	}
	return m_pDocument;
}

int SVPLCOutputsView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->style |= LVS_SINGLESEL ;
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CListCtrl& lc = GetListCtrl();

	ImageList.Create( 16, 16, TRUE, 5, 5 );                  // 20 Apr 1999 - frb.
	ImageList.Add( AfxGetApp()->LoadIcon( IDI_IOITEM_ICON ) );
	ImageList.Add( AfxGetApp()->LoadIcon( IDI_NOIOITEM_ICON ) );
	ImageList.Add( AfxGetApp()->LoadIcon( IDI_COLLAPSE ) );
	ImageList.Add( AfxGetApp()->LoadIcon( IDI_EXPAND ) );

	StateImageList.Create( 16, 16, TRUE, 2, 2 );             // 20 Apr 1999 - frb.
	StateImageList.Add( AfxGetApp()->LoadIcon( IDI_PPQ_ICON ) );
	StateImageList.Add( AfxGetApp()->LoadIcon( IDI_PLC_OUTPUT_ICON ) );


	lc.SetImageList( &StateImageList, LVSIL_STATE );
	lc.SetImageList( &ImageList, LVSIL_NORMAL );
	lc.SetImageList( &ImageList, LVSIL_SMALL );

	// insert columns
	lc.InsertColumn( 0, _T( "Output Data" ), LVCFMT_LEFT, -1, -1 );
	lc.InsertColumn( 1, _T( "PLC Address" ), LVCFMT_LEFT, -1, -1 );
	lc.InsertColumn( 2, _T( "Size" ), LVCFMT_LEFT, -1, -1 );
	
	lc.SetColumnWidth( 0, 500 );
	lc.SetColumnWidth( 1, 125 );
	lc.SetColumnWidth( 2,  50 );

	// We can show grid lines if we want to .
	// ListView_SetExtendedListViewStyle(m_hWnd, LVS_EX_GRIDLINES );

	lc.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	return 0;
}

BOOL SVPLCOutputsView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |= (LVS_SHOWSELALWAYS | LVS_REPORT | 
               LVS_NOSORTHEADER);
	cs.dwExStyle |= LVS_EX_GRIDLINES;

	return CListView::PreCreateWindow(cs);
}

void SVPLCOutputsView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	SVIODoc* pIODoc = GetDocument();
	
	if( pIODoc && ::IsWindow( m_hWnd ) )
	{
		GetListCtrl().SetRedraw(false);

		// First clean up list view
		GetListCtrl().DeleteAllItems();

		CString strItem;
		long lSize;
		long lPPQSize;
		int j = 0;
		int k = 1;
		int iCurrentPPQ = 0;
		int l_PPQNum = 0;

		SVPPQObject* pPPQ( nullptr );

		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		// Get the number of PPQs
		if( nullptr == pConfig || !pConfig->GetPPQCount( lPPQSize ) )
		{
			SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorGettingPPQCount, StdExceptionParams, SvOi::Err_17037_ErrorGettingPPQCount );
			DebugBreak();
		}

		// Check if any PPQs are here yet
		if( lPPQSize == 0 ) { return; }

		// Result Outputs
		DWORD maxOutput = 0;
		// Get the PLC List...

		m_aPPQGUIDs.clear();	// Empty the PPQ GUID List
		for( int iPPQLoop = 0; iPPQLoop < lPPQSize; iPPQLoop++ )
		{
			// Get the PPQ
			if( !pConfig->GetPPQ( iPPQLoop, &pPPQ ) )
			{
				SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
				e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorGettingPPQ, StdExceptionParams, SvOi::Err_17038_ErrorGettingPPQ );
				DebugBreak();
				continue;
			}

			if( pConfig->GetPLCData() == nullptr )
			{
				SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
				e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorGettingPLCData, StdExceptionParams, SvOi::Err_17039_ErrorGettingPLCData );
				DebugBreak();
				continue;
			}

			SVPLCDataController& l_PLCData = *( pConfig->GetPLCData() );

			// This is a Hack! this should be called from a different area when objects change.
			l_PLCData.ValidateInputs();

			CString l_strPLCName = pPPQ->GetPLCName();

			lSize = l_PLCData.GetItemCount(l_strPLCName);
			if( 0 == lSize ) { continue; }

			bool l_bCollapse = l_PLCData.GetCollapse( l_strPLCName );
			strItem.Format( _T( "%s       Block Count %d" ),pPPQ->GetName(),
				l_PLCData.GetBlockCount( l_strPLCName ) );

			int ipos = GetListCtrl().InsertItem( LVIF_IMAGE | LVIF_TEXT | LVIF_STATE, 
				iCurrentPPQ + k, strItem, 
				INDEXTOSTATEIMAGEMASK( 1 ),	// state
				LVIS_STATEIMAGEMASK,		// stateMask
				2 + l_bCollapse,			// Image
				0 );						// Set item data to NULL
			GetListCtrl().SetItemData( ipos, reinterpret_cast<DWORD_PTR>(pPPQ) );

			// size of the blocks
			SVPLCControlPar* l_pControl = nullptr;
			CString l_strFirstBlockAddress;
			if( l_PLCData.GetControlPar( l_strPLCName, &l_pControl ) == S_OK )
			{
				l_strFirstBlockAddress = l_pControl->GetBlockAddress( 0);
			}

			// Keep track of the PPQ GUID so when we click on the item we will know which ppq.
			if( nullptr == pPPQ ) { continue; }

			int lInsertedEntry = 0;

			if( !l_bCollapse )
			{
				// PLC outputs 
				for( j = 0; j < lSize; j++ )
				{
					SVPLCOutputObject* l_pPLCOutput = nullptr;
					HRESULT l_hr = l_PLCData.GetItem( l_strPLCName, j, l_pPLCOutput);

					if( nullptr == l_pPLCOutput )
					{
						continue;
					}
					else
					{
						CString l_strPLCName;
						l_pPLCOutput->GetPLCName(l_strPLCName);
						if( pPPQ->GetPLCName() != l_strPLCName ) { continue; }
					}

					// First column: Result I/O
					CString l_strName = l_pPLCOutput->GetInputValueObjectName();
					GetListCtrl().InsertItem( LVIF_IMAGE | LVIF_TEXT | LVIF_STATE, 
						lInsertedEntry + iCurrentPPQ + k, l_strName, 
						INDEXTOSTATEIMAGEMASK( 2 ),	// state
						LVIS_STATEIMAGEMASK,		// stateMask
						1, 0 );						// Set item data to NULL

					GetListCtrl().SetItem( lInsertedEntry + iCurrentPPQ + k, 0, LVIF_IMAGE, NULL, 0, 0, 0, 0 );

					// Store SVPLCOutputObject pointer in item data for editing
					GetListCtrl().SetItemData( lInsertedEntry + iCurrentPPQ + k, reinterpret_cast<DWORD_PTR>(l_pPLCOutput) );

					// Column: Description "Value Object Name"

					// Column: PLC Address
					CString l_strAddress = l_strFirstBlockAddress ;
					l_PLCData.AddOffsetToAddress( l_strAddress, l_pPLCOutput->GetElement( ) );
					if( l_pPLCOutput->SizeInBits() )
					{
						CString l_strTmp;
						l_strTmp.Format( _T("/%d"), l_pPLCOutput->GetBit());
						l_strAddress+= l_strTmp;
					}

					//l_strAddress.Format( _T("%d"), l_pPLCOutput->GetElement( ));
					GetListCtrl().SetItemText( lInsertedEntry + iCurrentPPQ + k, 1, l_strAddress );

					// Column: Size - Number of PLC Elements
					if( l_pPLCOutput->SizeInBits() )
					{
						strItem.Format( _T( "%d(Bits)" ), l_pPLCOutput->GetSize() );
					}
					else
					{
						strItem.Format( _T( "%d" ), l_pPLCOutput->GetSize() );
					}
					GetListCtrl().SetItemText( lInsertedEntry + iCurrentPPQ + k, 2, strItem );

					lInsertedEntry++;
				}// end for
			}

			iCurrentPPQ++;

			k += lInsertedEntry ;
		}// end for
		GetListCtrl().SetRedraw(true);
	}// end if

	//CListView::OnUpdate( pSender, lHint, pHint );   //This call will cause flicker
}

void SVPLCOutputsView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CString strAddress;
	SVPLCOutputObject* pPLCOutput = nullptr;
	UINT flags;

	SVConfigurationObject* pConfig = nullptr;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	int l_item = GetListCtrl().HitTest( point, &flags );

	if ( l_item >= 0 && ((flags & LVHT_ONITEMLABEL) == LVHT_ONITEMLABEL) && ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&
		 TheSVObserverApp.OkToEdit() )
	{
		long lPPQSize=0;

		// Get the number of PPQs
		if( nullptr == pConfig || !pConfig->GetPPQCount( lPPQSize ) )
		{
			SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorGettingPPQCount, StdExceptionParams, SvOi::Err_17040_ErrorGettingPPQCount );
			DebugBreak();
		}

		SVSVIMStateClass::AddState( SV_STATE_EDITING );

		pPLCOutput = dynamic_cast<SVPLCOutputObject*>( reinterpret_cast<SVObjectClass*>(GetListCtrl().GetItemData( l_item )));
		if( pPLCOutput )
		{
			// The User clicked on the Item 
			if( EditOutput( l_item ) )
			{
				SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
			}
		}
		else
		{	// The user clicked on the PPQ which means a new object.

			// Edit PLC Properties.
			SVPPQObject* pPPQ = dynamic_cast<SVPPQObject*>( reinterpret_cast<SVObjectClass*>(GetListCtrl().GetItemData( l_item )));
			if( nullptr != pPPQ )
			{
				m_strCurrentPLCId = pPPQ->GetPLCName();
				OnPlcProperties();
			}
		}

		SVSVIMStateClass::RemoveState( SV_STATE_EDITING );
		OnUpdate( NULL, NULL, NULL );
		GetListCtrl().SetItemState( l_item, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
	} // if ( item >= 0 && ! SVSVIMStateClass::CheckState
}// OnLButtonDblClk(

BOOL SVPLCOutputsView::PreTranslateMessage(MSG* pMsg)
{
	BOOL l_bRet = FALSE;

	SVConfigurationObject* pConfig = NULL;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

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
					OnPlcDelete();
					l_bRet = TRUE;
				}
				else if( VK_INSERT == pMsg->wParam )
				{
					CString l_strPLCName;
					if( PLCNameAtItem( l_strPLCName, l_item ) == S_OK )
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
				else if( VK_RETURN == pMsg->wParam )
				{
					CString l_strPLCName;
					if( PLCNameAtItem( l_strPLCName, l_item ) == S_OK )
					{
						// New Entry...
						SVSVIMStateClass::AddState( SV_STATE_EDITING );
						SVPLCOutputObject* pPLCOutput = dynamic_cast<SVPLCOutputObject*>( reinterpret_cast<SVObjectClass*>(l_pdwItemData));
						if( pPLCOutput )
						{
							if( EditOutput( l_item ) )
							{
								SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
							}
						}
						else
						{
							// Edit PLC Properties.
							m_strCurrentPLCId = l_strPLCName;
							OnPlcProperties();
						}
						SVSVIMStateClass::RemoveState( SV_STATE_EDITING );
						OnUpdate( NULL, NULL, NULL );
						GetListCtrl().SetItemState( l_item, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
						l_bRet = TRUE;
					}
				}
				else if( VK_ADD == pMsg->wParam )
				{
					// Expand
					SVPPQObject* pPPQ = dynamic_cast<SVPPQObject*>( reinterpret_cast<SVObjectClass*>(l_pdwItemData));
					if( nullptr != pPPQ && nullptr != pConfig && nullptr != pConfig->GetPLCData() )
					{
						SVPLCDataController& l_PLCData = *( pConfig->GetPLCData() );
						l_PLCData.SetCollapse(pPPQ->GetPLCName(),false );
						OnUpdate( NULL, NULL, NULL );
						LVFINDINFOA l_fi;
						l_fi.flags = LVFI_PARTIAL | LVFI_STRING;
						l_fi.psz = pPPQ->GetName();
						int l_iPos = GetListCtrl().FindItem(&l_fi );
						if( l_iPos > -1 )
						{
							GetListCtrl().SetItemState( l_iPos, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
						}
						l_bRet = TRUE;
					}
				}

				if( VK_SUBTRACT == pMsg->wParam )
				{
					// Collapse
					SVPPQObject* pPPQ = dynamic_cast<SVPPQObject*>( reinterpret_cast<SVObjectClass*>(l_pdwItemData));
					if( nullptr != pPPQ && nullptr != pConfig && nullptr != pConfig->GetPLCData() )
					{
						SVPLCDataController& l_PLCData = *( pConfig->GetPLCData() );
						l_PLCData.SetCollapse(pPPQ->GetPLCName(),true );
						OnUpdate( NULL, NULL, NULL );
						LVFINDINFOA l_fi;
						l_fi.flags = LVFI_PARTIAL | LVFI_STRING;
						l_fi.psz = pPPQ->GetName();
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

HRESULT SVPLCOutputsView::PLCNameAtItem( CString& p_rstrPPQName, int p_iItem )
{
	HRESULT l_hr = S_FALSE;
	for( int i = p_iItem ; i > -1 ; i-- )
	{
		// Get the PPQ for this list position
		DWORD_PTR pdwItemData;
		pdwItemData = GetListCtrl().GetItemData( i );
		SVPPQObject* pPPQ = dynamic_cast<SVPPQObject*>(reinterpret_cast<SVObjectClass*>(pdwItemData));
		if( nullptr != pPPQ )
		{
			p_rstrPPQName = pPPQ->GetPLCName( );
			l_hr = S_OK;
			break;
		}
	}
	return l_hr;
}

void SVPLCOutputsView::OnPlcAdd()
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

void SVPLCOutputsView::OnPlcProperties()
{
	if( TheSVObserverApp.OkToEdit() )
	{
		// Dialog to edit Blocks and Queue Size.
		SVPLCPropSheet l_dlg(_T("PLC Wizzard"), this);
		l_dlg.AddPages();

		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
		//Without valid config pointer don't need to go further
		ASSERT( nullptr != pConfig );
		if( nullptr == pConfig )
		{
			return;
		}

		HRESULT l_hr = pConfig->GetPLCControlData( l_dlg.m_ControlMaterials, m_strCurrentPLCId );

		l_dlg.m_ValidateBlocks = boost::bind(&SVPLCDataController::ValidateBlockAddresses, pConfig->GetPLCData(),_1, _2);
		l_dlg.m_strCurrentPLCId = m_strCurrentPLCId;

		if( l_dlg.DoModal() == ID_WIZFINISH )
		{
			HRESULT l_hr = pConfig->SetPLCControlData( l_dlg.m_ControlMaterials, m_strCurrentPLCId );
			ASSERT( l_hr == S_OK );

			// Send the Queue size to the PLC Dll device..
			_variant_t l_vQueueSize;
			l_dlg.m_ControlMaterials.GetMaterial( CTAG_PLC_QUEUE_SIZE, l_vQueueSize );
			TheSVObserverApp.m_PLCManager.SetParameterValue( m_strCurrentPLCId, 0, l_vQueueSize.Detach() );
			// Make sure the logical plc is started.
			TheSVObserverApp.m_PLCManager.Startup( pConfig );

			// Set Modified flag.
			SVSVIMStateClass::AddState( SV_STATE_MODIFIED );

			OnUpdate( NULL, NULL, NULL );
		}
	}
}

void SVPLCOutputsView::OnContextMenu(CWnd* /*pWnd*/, CPoint point )
{
	UINT nFlags = 0;
	CPoint ListPoint = point;

	GetListCtrl().ScreenToClient( &ListPoint);
	// store the item in this class
	m_CurrentItem = GetListCtrl().HitTest( ListPoint, &nFlags );

	if ( m_CurrentItem >= 0 )
	{
		SVPPQObject* pPPQ=dynamic_cast<SVPPQObject*>(reinterpret_cast<SVObjectClass*>(GetListCtrl().GetItemData( m_CurrentItem )));

		if( nullptr != pPPQ )
		{
			// Context Menu...Add and Properties
			CMenu* pPopup = m_ContextMenuProp.GetSubMenu(0);
			ASSERT(pPopup != NULL);
			CWnd* pWndPopupOwner = this;

			while(pWndPopupOwner->GetStyle() & WS_CHILD)
			{
				pWndPopupOwner = pWndPopupOwner->GetParent();
			}

			// Store the PLC id so we know which plc settings to edit.
			m_strCurrentPLCId = pPPQ->GetPLCName();

			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
									point.x, 
									point.y,
									pWndPopupOwner);
		}
		else
		{
			// Context Menu...Add and Properties
			SVPLCOutputObject* l_pOutput=dynamic_cast<SVPLCOutputObject*>(reinterpret_cast<SVObjectClass*>(GetListCtrl().GetItemData( m_CurrentItem )));
			if( l_pOutput )
			{
				CMenu* pPopup;
				CWnd* pWndPopupOwner = this;
				if( l_pOutput->GetName() == _T("Trigger Count") ) // If the name is trigger count
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

void SVPLCOutputsView::OnPlcDelete()
{
	if( TheSVObserverApp.OkToEdit() )
	{
		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		POSITION l_Pos = GetListCtrl().GetFirstSelectedItemPosition();
		if( l_Pos != NULL && nullptr != pConfig && nullptr != pConfig->GetPLCData() )
		{
			int l_item = GetListCtrl().GetNextSelectedItem( l_Pos );
			SVPLCOutputObject* pPLCOutput = dynamic_cast<SVPLCOutputObject*>( reinterpret_cast<SVObjectClass*>(GetListCtrl().GetItemData( l_item )));
			SVPLCDataController& l_PLCData = *( pConfig->GetPLCData() );
			if( pPLCOutput )
			{
				// The user clicked on an output
				CString l_strPLCName = pPLCOutput->GetPLCName();
				if( l_PLCData.GetFirstObject( l_strPLCName ) == pPLCOutput 
					&& pPLCOutput->GetInputValueObjectName().Find(_T("Trigger Count") ) > -1 )
				{
					AfxMessageBox(_T("Trigger Count Cannot Be deleted"),MB_OK );
				}
				else
				{
					int l_iResponse = AfxMessageBox(_T("Are You Sure you want to delete this Output?"), MB_YESNO );
					if( l_iResponse == IDYES )
					{
						l_PLCData.DeletePLCEntry( pPLCOutput->GetPLCName(), pPLCOutput );

						SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
						TheSVObserverApp.m_PLCManager.Startup( pConfig );
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
				// The user clicked on a PLC
				// Check which plc to delete.
				SVPPQObject* pPPQ = dynamic_cast<SVPPQObject*>( reinterpret_cast<SVObjectClass*>(GetListCtrl().GetItemData( l_item )));;
				if( nullptr != pPPQ )
				{
					CString l_strPlc = pPPQ->GetName(); 
					CString l_strMsg;
					l_strMsg.Format( _T("Are you sure you want to delete All PLC Entries for %s"), l_strPlc );
					int imbRet = AfxMessageBox( l_strMsg, MB_YESNO );
					if( imbRet == IDYES )
					{
						// Delete all plc entries
						l_PLCData.DeletePLC( pPPQ->GetPLCName() );
						SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
						OnUpdate( NULL, NULL, NULL );
						if( pConfig->GetPLCCount() == 0 )
						{
							// Hide the PLC Tab if no outputs exist.
							TheSVObserverApp.HidePLCTab( );
						}
					}
				}
			}
		}
	}
}

void SVPLCOutputsView::OnPlcEdit()
{
	if( TheSVObserverApp.OkToEdit() )
	{
		SVSVIMStateClass::AddState( SV_STATE_EDITING );

		EditOutput(m_CurrentItem);

		SVSVIMStateClass::RemoveState( SV_STATE_EDITING );
		OnUpdate( NULL, NULL, NULL );
	}
}

bool SVPLCOutputsView::AddOutput(int p_iWhere)
{
	bool l_bRet = false;
	CString l_strPLCName;
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( PLCNameAtItem( l_strPLCName, p_iWhere ) == S_OK && nullptr != pConfig && nullptr != pConfig->GetPLCData() )
	{
		// New Entry...
		SVPLCDataController& l_PLCData = *( pConfig->GetPLCData() );
		SVPLCOutputEditDialog dlg;

		dlg.m_strPLCId = l_strPLCName;

		SVPPQObject* pPPQ=dynamic_cast<SVPPQObject*>(reinterpret_cast<SVObjectClass*>(GetListCtrl().GetItemData( p_iWhere )));
		if( nullptr != pPPQ )
		{
			dlg.m_InputObjectGUID = pPPQ->m_voTriggerCount.GetUniqueObjectID();
			dlg.m_bNewObject = true;
		}

		// Check if this is the first.
		if( l_PLCData.GetFirstObject( l_strPLCName ) == NULL )
		{
			// First Item in the list...
			dlg.m_bDisableObject = true;
			dlg.m_bDisableElement = true;
			dlg.m_lElement = 0;
		}
		else
		{
			// Not the first item in the list..
			long l_lNewElement;
			l_PLCData.GetNextValidElement( l_lNewElement, l_PLCData.GetLastObject( dlg.m_strPLCId ) );
			dlg.m_lElement = l_lNewElement;
		}

		dlg.m_validateFunc = boost::bind(&SVPLCDataController::ValidateElement, &l_PLCData, _1, _2, _3, _4, _5, _6);
		dlg.m_CalcNewSizeCallBack = boost::bind( &SVPLCDataController::CalcNewElementSize, &l_PLCData, _1, _2 );

		if( dlg.DoModal() == IDOK )
		{
			SVPLCOutputObject* l_pNewOutput = NULL;
			l_PLCData.AddItem(l_strPLCName, 
				l_pNewOutput, 
				dlg.m_lElement, 
				dlg.m_lSize, 
				dlg.m_InputObjectGUID, 
				dlg.m_lBitNum, 
				dlg.GetUseBitSize());
			OnUpdate( NULL, NULL, NULL );
			l_PLCData.SortPLCLists();
			TheSVObserverApp.m_PLCManager.Startup( pConfig );
			l_bRet = true;
		}

		if( p_iWhere >= GetListCtrl().GetItemCount() )
		{
			p_iWhere--;
			if( p_iWhere < 0 )
				p_iWhere = 0;
		}
	}

	return l_bRet;
}

bool SVPLCOutputsView::EditOutput(int p_iWhere)
{
	bool l_bRet = false;

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig && nullptr != pConfig->GetPLCData() )
	{
		SVPLCDataController& l_PLCData = *( pConfig->GetPLCData() );

		SVPLCOutputEditDialog dlg;

		SVPLCOutputObject* pPLCOutput=NULL;
		pPLCOutput = dynamic_cast<SVPLCOutputObject*>( reinterpret_cast<SVObjectClass*>(GetListCtrl().GetItemData( p_iWhere )));
		if( pPLCOutput )
		{
			// The User clicked on the Item
			pPLCOutput->GetPLCName( dlg.m_strPLCId );
			l_PLCData.GetListIndex( pPLCOutput, dlg.m_strPLCId, dlg.m_lOriginalIndex );
			dlg.m_validateFunc = boost::bind(&SVPLCDataController::ValidateElement, &l_PLCData,_1,_2,_3,_4,_5,_6 );
			dlg.m_CalcNewSizeCallBack = boost::bind( &SVPLCDataController::CalcNewElementSize, &l_PLCData, _1,_2 );
			dlg.m_strOutputSourceName = pPLCOutput->GetInputValueObjectName();
			pPLCOutput->GetInputValueObjectGUID( dlg.m_InputObjectGUID ); 
			dlg.m_lElement = pPLCOutput->GetElement();
			dlg.m_lSize = pPLCOutput->GetSize();
			dlg.m_lBitNum = pPLCOutput->GetBit();
			dlg.m_bUseBitSize = pPLCOutput->SizeInBits();

			// if this is the first (Trigger Count) item in the list then gray out the object.
			if( l_PLCData.GetFirstObject( dlg.m_strPLCId ) == pPLCOutput )
			{
				dlg.m_bDisableObject = true;
				dlg.m_bDisableElement = true;
			}
		}

		switch( dlg.DoModal() )
		{
			case IDOK:
			{
				pPLCOutput->SetElement( dlg.m_lElement );
				pPLCOutput->SetInputObjectId( dlg.m_InputObjectGUID );
				pPLCOutput->SetSize( dlg.m_lSize );
				pPLCOutput->SetPLCName( dlg.m_strPLCId );
				pPLCOutput->SetBit( dlg.m_lBitNum );
				pPLCOutput->SetSizeInBits( dlg.GetUseBitSize());
				TheSVObserverApp.m_PLCManager.Startup( pConfig );
				l_PLCData.SortPLCLists();
				l_bRet = true;
				break;
			}
			case IDCANCEL: // fall through...
			default:
			{
				break;
			}
		}
	}

	return l_bRet;
}

// Collapse and Expand logic if the user clicks on the PPQ item in the list.
void SVPLCOutputsView::OnLButtonDown(UINT nFlags, CPoint point)
{
	UINT uFlags;

	int l_item = GetListCtrl().HitTest( point, &uFlags);
	if( (uFlags & LVHT_ONITEMSTATEICON) || (uFlags & LVHT_ONITEMICON) )
	{
		// Toggle Collapse.
		if( l_item > -1 )
		{
			SVConfigurationObject* pConfig( nullptr );
			SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

			DWORD_PTR l_pdwItemData = GetListCtrl().GetItemData( l_item );
			SVPPQObject* pPPQ = dynamic_cast<SVPPQObject*>( reinterpret_cast<SVObjectClass*>(l_pdwItemData));
			if( nullptr != pPPQ && nullptr != pConfig && nullptr != pConfig->GetPLCData() )
			{
				SVPLCDataController& l_PLCData = *( pConfig->GetPLCData() );
				bool l_bCollapse = l_PLCData.GetCollapse( pPPQ->GetPLCName());
				l_PLCData.SetCollapse(pPPQ->GetPLCName(), !l_bCollapse );
				OnUpdate( NULL, NULL, NULL );
				LVFINDINFOA l_fi;
				l_fi.flags = LVFI_PARTIAL | LVFI_STRING;
				l_fi.psz = pPPQ->GetName();
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

void SVPLCOutputsView::OnAddTransferBlock()
{
	CStringVec l_OriginalList;

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig )
	{
		pConfig->GetPLCData()->GetPPQs( l_OriginalList, pConfig );
		pConfig->GetPLCData()->SetupPLCPPQs(pConfig, l_OriginalList );

		TheSVObserverApp.m_PLCManager.Startup( pConfig );
		OnUpdate( NULL, NULL, NULL );

		TheSVObserverApp.GetIODoc()->SetModifiedFlag();
		if( pConfig->GetPLCCount() > 0 )
		{
			TheSVObserverApp.ShowIOTab( SVIOPLCOutputsViewID );
				// Set Active IO Tabbed view to the PLC Outputs Tab
			TheSVObserverApp.SetActiveIOTabView( SVIOPLCOutputsViewID );
		}
		else
		{
			TheSVObserverApp.HidePLCTab();
		}
	}
}
#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVPLCOutputsView.cpp_v  $
 * 
 *    Rev 1.5   12 Jun 2014 16:11:36   sjones
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
 *    Rev 1.4   15 May 2014 12:32:54   tbair
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
 *    Rev 1.3   11 Dec 2013 14:03:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  873
 * SCR Title:  Fix inconsistant GUI labels and functionality on IO pages
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Hide PLCIOTab when PLCs are deleted from the context menu.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Nov 2013 11:41:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  873
 * SCR Title:  Fix inconsistant GUI labels and functionality on IO pages
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed PLC behavior when right clicking on plc transfer block. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Oct 2013 11:00:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added #ifndef _WIN64 to remove deprecated code from the 64bit solution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:26:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   18 Sep 2012 18:29:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   07 Aug 2012 16:18:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated views to remove flicker.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   09 Jul 2012 14:11:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the include for SVIM State.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   02 Jul 2012 17:30:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   19 Jun 2012 14:04:02   jspila
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
 *    Rev 1.6   30 Jan 2012 11:47:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Modified source code to consolidate PLC and New Output stream functionality to I/O Controller.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   08 Dec 2010 13:26:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Dec 2009 12:25:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   14 Jan 2009 16:12:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified PLC GUI to match Functional Requirement.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   10 Dec 2008 10:14:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed bug where double clicking on the list icon in the list brings up the properties dialog. Prevent double click on the icon.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   04 Dec 2008 09:40:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Change function name in SVPLCDataController to make its function more clear.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:09:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/