//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDiscreteInputsView.cpp
//* .File Name       : $Workfile:   SVDiscreteInputsView.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   01 Oct 2013 12:48:26  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/bind.hpp>

#include "SVDiscreteInputsView.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVOResource\ConstGlobalSvOr.h"
#include "SVObserver.h"
#include "SVIODoc.h"
#include "SVIOAdjustDialog.h"
#include "SVInputObjectList.h"
#include "SVDigitalInputObject.h"
#include "SVInfoStructs.h"
#include "SVConfigurationObject.h"
#include "SVSVIMStateClass.h"
#include "SVMessage\SVMessage.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvO.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(SVDiscreteInputsView, CListView)

BEGIN_MESSAGE_MAP(SVDiscreteInputsView, CListView)
	//{{AFX_MSG_MAP(SVDiscreteInputsView)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_DESTROY( )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVDiscreteInputsView::SVDiscreteInputsView()
: m_Items( boost::bind( &( CListCtrl::GetItemData ), &( GetListCtrl() ), _1 ) , boost::bind( &( CListCtrl::SetItemData ), &( GetListCtrl() ), _1, _2 ) )
{
}

SVDiscreteInputsView::~SVDiscreteInputsView()
{
}

BOOL SVDiscreteInputsView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CListView::PreCreateWindow(cs);
}

void SVDiscreteInputsView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();
}

#ifdef _DEBUG
void SVDiscreteInputsView::AssertValid() const
{
	CListView::AssertValid();
}

void SVDiscreteInputsView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

SVIODoc* SVDiscreteInputsView::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(SVIODoc)));
	return (SVIODoc*)m_pDocument;
}
#endif //_DEBUG

BOOL SVDiscreteInputsView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	dwStyle |= LVS_REPORT;

	BOOL RetVal = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

	ImageList.Create( SvOr::IconSize, SvOr::IconSize, ILC_COLOR | ILC_MASK, 2, 1 );
	ImageList.Add( AfxGetApp()->LoadIcon( IDI_IOITEM_ICON ) );
	ImageList.Add( AfxGetApp()->LoadIcon( IDI_NOIOITEM_ICON ) );

	StateImageList.Create( SvOr::IconSize, SvOr::IconSize, ILC_COLOR | ILC_MASK, 2, 1 );
	StateImageList.Add( AfxGetApp()->LoadIcon( IDI_INPUT_ICON ) );
	StateImageList.Add( AfxGetApp()->LoadIcon( IDI_OUTPUT_ICON ) );

	GetListCtrl().SetImageList( &StateImageList, LVSIL_STATE );
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

void SVDiscreteInputsView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint )
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
		int i;
		SVInputObjectList* pInputList = nullptr;
		SVDigitalInputObject* pDigInput;
		SVIOEntryHostStructPtrList ppIOEntries;
		SVIOEntryHostStructPtr pIOEntry;

		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		// Get list of available inputs
		if ( nullptr != pConfig ) { pInputList = pConfig->GetInputObjectList(); }
		if ( nullptr == pInputList )
		{
			SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorGettingInputObjectList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17006_ErrorGettingInputObjectList );
			DebugBreak();
			return; 
		}

		if( !pInputList->FillInputs( ppIOEntries ) )
		{
			SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorFillingInputs, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17007_ErrorFillingInputs );
			DebugBreak();
		}

		lSize = static_cast< long >( ppIOEntries.size() );

		// Module Inputs
		DWORD maxInput = 0;
		SVIOConfigurationInterfaceClass::Instance().GetDigitalInputCount( maxInput );
		for( i = 0; i < static_cast<long>(maxInput); ++i )
		{
			// First column: Module I/O
			strItem.Format( _T( "Digital Input %d" ), i + 1 );
			GetListCtrl().InsertItem( LVIF_IMAGE | LVIF_TEXT | LVIF_STATE,
									  i, strItem,
									  INDEXTOSTATEIMAGEMASK( 1 ),	// state
									  LVIS_STATEIMAGEMASK,			// stateMask
									  1, 0 );						// Set item data to nothing

			// Find each digital input
			for( j = 0; j < lSize; j++ )
			{
				pIOEntry = ppIOEntries[j];
				if( pIOEntry->m_ObjectType != IO_DIGITAL_INPUT ) { continue; }

				pDigInput = dynamic_cast< SVDigitalInputObject* >( SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId ) );

				if( !pDigInput ) { continue; }

				if( i == pDigInput->GetChannel() )
				{
					GetListCtrl().SetItem( i, 0, LVIF_IMAGE, nullptr, 0, 0, 0, 0 );

					m_Items.SetItemData( i, pIOEntry );

					// Column: Description
					GetListCtrl().SetItemText( i, 1, pDigInput->GetName() );

					// Column: Force
					if( pDigInput->IsForced() )
					{
						strItem.Format( _T( "%d" ), pDigInput->GetForcedValue() ? 1 : 0 );
						GetListCtrl().SetItemText( i, 2, strItem );
					}// end if

					// Column: Inverted
					strItem.Format( _T( "%s" ), pDigInput->IsInverted() ? _T( "1" ) : _T( "" ) );
					GetListCtrl().SetItemText( i, 3, strItem );

					break;
				}// end if
			}// end for
		}// end for
		GetListCtrl().SetRedraw(true);
	}// end if

	//CListView::OnUpdate( pSender, lHint, pHint );   //This call will cause flicker
}// end OnUpdate

void SVDiscreteInputsView::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	SVDigitalInputObject *pDigInput;
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
		if( item >= 0 && item < GetListCtrl().GetItemCount() &&
			( flags & ( LVHT_ONITEMSTATEICON | LVHT_ONITEMICON | LVHT_ONITEMLABEL ) ) )
		{
			SVIOAdjustDialogClass dlg;
			SVConfigurationObject* pConfig( nullptr );
			SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

			// Get list of available inputs
			SVInputObjectList *pInputList = nullptr;
			if ( nullptr != pConfig ) { pInputList = pConfig->GetInputObjectList(); }
			if ( nullptr == pInputList )
			{
				SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
				e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorGettingInputObjectList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17008_ErrorGettingInputObjectList );
				DebugBreak();
			}

			if( !pInputList->FillInputs( ppIOEntries ) )
			{
				SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
				e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorFillingInputs, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17009_ErrorFillingInputs );
				DebugBreak();
			}

			lSize = static_cast< long >( ppIOEntries.size() );

			SVDataItemManager::const_iterator l_Iter = m_Items.GetItemData( item );

			if( l_Iter != m_Items.end() )
			{
				pIOEntry = l_Iter->second;
			}

			pDigInput = dynamic_cast< SVDigitalInputObject* >( SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId ) );
			if( pDigInput )
			{
				dlg.StrIOName = _T( "Module " ) + GetListCtrl().GetItemText( item, 0 );
				dlg.StrIOName += _T( ", " ) + GetListCtrl().GetItemText( item, 1 );
				dlg.StrIOValue.Format( "%d", pDigInput->GetValue() ? 1 : 0 );
				dlg.m_pDigInput = pDigInput;
				dlg.m_pIOEntry  = pIOEntry;
				dlg.m_bInputMode = TRUE;

				SVSVIMStateClass::AddState( SV_STATE_EDITING );

				switch( dlg.DoModal() )
				{
				case IDOK:
					SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
					break;

				case IDCANCEL:
				default:
					break;
				}// end switch

				OnUpdate( nullptr, 0, nullptr );

				SVSVIMStateClass::RemoveState( SV_STATE_EDITING );
			}// end if
		}// end if
	}// end if
}// end OnLButtonDblClk

void SVDiscreteInputsView::OnDestroy()
{
	CListView::OnDestroy();
}

