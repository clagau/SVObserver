//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteInputsView
//* .File Name       : $Workfile:   SVRemoteInputsView.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   02 Oct 2013 07:58:46  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include <boost\config.hpp>
#include <boost\bind.hpp>
#include "SVRemoteInputsView.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVObserver.h"
#include "SVIODoc.h"
#include "SVRemoteIOAdjustDialog.h"
#include "SVInputObjectList.h"
#include "SVRemoteInputObject.h"
#include "SVDigitalInputObject1.h"
#include "SVConfigurationObject.h"
#include "SVSVIMStateClass.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvO.h"
#pragma endregion Includes

IMPLEMENT_DYNCREATE(SVRemoteInputsView, CListView)

SVRemoteInputsView::SVRemoteInputsView()
: m_Items( boost::bind( &( CListCtrl::GetItemData ), &( GetListCtrl() ), _1 ) , boost::bind( &( CListCtrl::SetItemData ), &( GetListCtrl() ), _1, _2 ) )
{
}

SVRemoteInputsView::~SVRemoteInputsView()
{
}

BEGIN_MESSAGE_MAP(SVRemoteInputsView, CListView)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_DESTROY( )
END_MESSAGE_MAP()

#ifdef _DEBUG
void SVRemoteInputsView::AssertValid() const
{
	CListView::AssertValid();
}

void SVRemoteInputsView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
SVIODoc* SVRemoteInputsView::GetDocument() // Die endgültige (nicht zur Fehlersuche kompilierte) Version ist Inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(SVIODoc)));
	return (SVIODoc*)m_pDocument;
}
#endif //_DEBUG

BOOL SVRemoteInputsView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	dwStyle |= LVS_REPORT; //LVS_ICON; //LVS_REPORT;

	BOOL RetVal = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

	ImageList.Create( 16, 16, TRUE, 5, 5 );                      // 20 Apr 1999 - frb.
	ImageList.Add( AfxGetApp()->LoadIcon( IDI_IOITEM_ICON ) );
	ImageList.Add( AfxGetApp()->LoadIcon( IDI_NOIOITEM_ICON ) );

	StateImageList.Create( 16, 16, TRUE, 2, 2 );                 // 20 Apr 1999 - frb.
	StateImageList.Add( AfxGetApp()->LoadIcon( IDI_INPUT_ICON ) );
	StateImageList.Add( AfxGetApp()->LoadIcon( IDI_OUTPUT_ICON ) );

	GetListCtrl().SetImageList( &StateImageList, LVSIL_STATE );
	GetListCtrl().SetImageList( &ImageList, LVSIL_NORMAL );
	GetListCtrl().SetImageList( &ImageList, LVSIL_SMALL );

	GetListCtrl().InsertColumn( 0, _T( "Inputs" ), LVCFMT_LEFT, -1, -1 );
	GetListCtrl().InsertColumn( 1, _T( "Description" ), LVCFMT_LEFT, -1, -1 );
	GetListCtrl().InsertColumn( 2, _T( "Forced" ), LVCFMT_LEFT, -1, -1 );
	GetListCtrl().InsertColumn( 3, _T( "Inverted" ), LVCFMT_LEFT, -1, -1 );

	GetListCtrl().SetColumnWidth( 0, 125 );
	GetListCtrl().SetColumnWidth( 1, 500 );
	GetListCtrl().SetColumnWidth( 2,  50 );
	GetListCtrl().SetColumnWidth( 3,  55 );

	return RetVal;
}

void SVRemoteInputsView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint )
{
	SVIODoc* pIODoc = (SVIODoc*) GetDocument();

	if( pIODoc && ::IsWindow( m_hWnd ) )
	{
		GetListCtrl().SetRedraw(false);

		// First clean up list view
		GetListCtrl().DeleteAllItems();

		m_Items.clear();

		CString strItem;
		long lSize;
		int j;
		SVInputObjectList* pInputList = nullptr;
		SVRemoteInputObject* pRemInput;
		SVIOEntryHostStructPtrList ppIOEntries;
		SVIOEntryHostStructPtr pIOEntry;

		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		// Get list of available inputs
		if( nullptr != pConfig ) { pInputList = pConfig->GetInputObjectList(); }
		if( nullptr == pInputList )
		{
			SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorGettingInputObjectList, StdMessageParams, SvOi::Err_17044_ErrorGettingInputObjectList );
			DebugBreak();
			return;
		}

		if( !pInputList->FillInputs( ppIOEntries ) )
		{
			SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorFillingInputs, StdMessageParams, SvOi::Err_17045_ErrorFillingInputs );
			DebugBreak();
		}

		lSize = static_cast<long>(ppIOEntries.size());

		int h = 0;
		// Find each remote input
		for( j = 0; j < lSize; j++ )
		{
			strItem.Format( _T( "Remote Input %d" ), h + 1 );

			BOOL bFound = FALSE;

			//items are in a map, so they are not ordered.  loop thru each item to find correct remote input
			for ( int i = 0; (i < lSize && !bFound); i++ )
			{
				pIOEntry = ppIOEntries[i];

				if( pIOEntry->m_ObjectType != IO_REMOTE_INPUT ) { continue; }

				SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId );

				pRemInput = dynamic_cast< SVRemoteInputObject* >( l_pObject );

				if( !pRemInput ) { continue; }

				if ( strItem == pRemInput->GetName() ) { bFound = TRUE; }
			}

			if ( bFound )
			{
				// First column: Result I/O
				//strItem.Format( _T( "Remote Input %d" ), h + 1 );
				GetListCtrl().InsertItem( LVIF_IMAGE | LVIF_TEXT | LVIF_STATE,
					h , strItem,
					INDEXTOSTATEIMAGEMASK( 1 ),	// state
					LVIS_STATEIMAGEMASK,		// stateMask
					1, 0 );						// Set item data to NULL

				GetListCtrl().SetItem( h , 0, LVIF_IMAGE, NULL, 0, 0, 0, 0 );

				m_Items.SetItemData( h, pIOEntry );

				// Column: Description
				GetListCtrl().SetItemText( h , 1, pRemInput->GetName() );

				// Increment the number of remote outputs
				h++;
			}// if bFound
		}// end for
		GetListCtrl().SetRedraw(true);
	}// end if

	//CListView::OnUpdate( pSender, lHint, pHint );   //This call will cause flicker
}// end OnUpdate

void SVRemoteInputsView::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	SVIOEntryHostStructPtrList ppIOEntries;
	SVIOEntryHostStructPtr pIOEntry;
	UINT flags;
	long lSize;

	int item = GetListCtrl().HitTest( point, &flags );
	SVIODoc* pIODoc = ( SVIODoc* ) GetDocument();

	if ( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&
		TheSVObserverApp.OkToEdit() &&
	     pIODoc )
	{
		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		if( item >= 0 && item < GetListCtrl().GetItemCount() &&
			( flags & ( LVHT_ONITEMSTATEICON | LVHT_ONITEMICON | LVHT_ONITEMLABEL ) ) )
		{
			// Get list of available inputs
			SVInputObjectList* pInputList = nullptr;
			if( nullptr != pConfig ) { pInputList = pConfig->GetInputObjectList( ); }
			if( nullptr == pInputList )
			{
				SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
				e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorGettingInputObjectList, StdMessageParams, SvOi::Err_17046_ErrorGettingInputObjectList );
				DebugBreak();
			}

			if( !pInputList->FillInputs( ppIOEntries ) )
			{
				SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
				e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorFillingInputs, StdMessageParams, SvOi::Err_17047_ErrorFillingInputs );
				DebugBreak();
			}

			lSize = static_cast<long>(ppIOEntries.size());

			SVDataItemManager::const_iterator l_Iter = m_Items.GetItemData( item );

			if( l_Iter != m_Items.end() )
			{
				pIOEntry = l_Iter->second;
			}

			if( !( pIOEntry.empty() ) )
			{
				SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId );

				SVRemoteInputObject* pRemoteInput = dynamic_cast< SVRemoteInputObject* >( l_pObject );
				if( pRemoteInput )
				{
					CString l_Name;
					_variant_t l_Value;
					SVRemoteIOAdjustDialog dlg;

					l_Name = GetListCtrl().GetItemText( item, 1 );

					pRemoteInput->GetCache( l_Value );

					dlg.SetIOName( l_Name );
					dlg.SetIOValue( l_Value );

					SVSVIMStateClass::AddState( SV_STATE_EDITING );

					switch( dlg.DoModal() )
					{
					case IDOK:
						{
							dlg.GetIOValue( l_Value );

							pRemoteInput->WriteCache( l_Value );

							SVVariantValueObjectClass* l_pValueObject = dynamic_cast< SVVariantValueObjectClass* >( pIOEntry->m_pValueObject );
							if( l_pValueObject != NULL )
							{
								l_pValueObject->SetDefaultValue( l_Value, true );
							}

							SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
							break;
						}

					case IDCANCEL:
					default:
						break;
					}// end switch

					OnUpdate( NULL, NULL, NULL );

					SVSVIMStateClass::RemoveState( SV_STATE_EDITING );
				}
			}// end else if
		}// end if
	}// end if
}// end OnLButtonDblClk

void SVRemoteInputsView::OnDestroy()
{
	CListView::OnDestroy();
}

