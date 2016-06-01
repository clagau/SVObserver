//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDiscreteOutputsView
//* .File Name       : $Workfile:   SVDiscreteOutputsView.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   01 Oct 2013 12:48:26  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVDiscreteOutputsView.h"
#include "SVIOLibrary\SVIOConfigurationInterfaceClass.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVObserver.h"
#include "SVIODoc.h"
#include "SVIOAdjustDialog.h"
#include "SVDigitalOutputObject.h"
#include "SVSVIMStateClass.h"
#include "SVPPQObject.h"
#include "SVConfigurationObject.h"
#include "SVOutputObjectList.h"
#include "SVMessage/SVMessage.h"
#include "SVIOController.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvO.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(SVDiscreteOutputsView, CListView)

BEGIN_MESSAGE_MAP(SVDiscreteOutputsView, CListView)
	//{{AFX_MSG_MAP(SVDiscreteOutputsView)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CListView::OnFilePrintPreview)
END_MESSAGE_MAP()

SVDiscreteOutputsView::SVDiscreteOutputsView()
: m_Items( boost::bind( &( CListCtrl::GetItemData ), &( GetListCtrl() ), _1 ) , boost::bind( &( CListCtrl::SetItemData ), &( GetListCtrl() ), _1, _2 ) )
{
}

SVDiscreteOutputsView::~SVDiscreteOutputsView()
{
}

BOOL SVDiscreteOutputsView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CListView::PreCreateWindow(cs);
}

void SVDiscreteOutputsView::OnDestroy()
{
	CListView::OnDestroy();
}

void SVDiscreteOutputsView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();
}

#ifdef _DEBUG
void SVDiscreteOutputsView::AssertValid() const
{
	CListView::AssertValid();
}

void SVDiscreteOutputsView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

SVIODoc* SVDiscreteOutputsView::GetDocument() // Die endgültige (nicht zur Fehlersuche kompilierte) Version ist Inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(SVIODoc)));
	return (SVIODoc*)m_pDocument;
}
#endif //_DEBUG

BOOL SVDiscreteOutputsView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	dwStyle |= LVS_REPORT;
	
	BOOL RetVal = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

	ImageList.Create( 16, 16, TRUE, 5, 5 );                  // 20 Apr 1999 - frb.
	ImageList.Add( AfxGetApp()->LoadIcon( IDI_IOITEM_ICON ) );
	ImageList.Add( AfxGetApp()->LoadIcon( IDI_NOIOITEM_ICON ) );

	StateImageList.Create( 16, 16, TRUE, 2, 2 );             // 20 Apr 1999 - frb.
	StateImageList.Add( AfxGetApp()->LoadIcon( IDI_INPUT_ICON ) );
	StateImageList.Add( AfxGetApp()->LoadIcon( IDI_OUTPUT_ICON ) );


	GetListCtrl().SetImageList( &StateImageList, LVSIL_STATE );
	GetListCtrl().SetImageList( &ImageList, LVSIL_NORMAL );
	GetListCtrl().SetImageList( &ImageList, LVSIL_SMALL );


	GetListCtrl().InsertColumn( 0, _T( "Outputs" ), LVCFMT_LEFT, -1, -1 );
	GetListCtrl().InsertColumn( 1, _T( "Description" ), LVCFMT_LEFT, -1, -1 );
	GetListCtrl().InsertColumn( 2, _T( "Forced" ), LVCFMT_LEFT, -1, -1 );
	GetListCtrl().InsertColumn( 3, _T( "Inverted" ), LVCFMT_LEFT, -1, -1 );
	
	GetListCtrl().SetColumnWidth( 0, 125 );
	GetListCtrl().SetColumnWidth( 1, 500 );
	GetListCtrl().SetColumnWidth( 2,  50 );
	GetListCtrl().SetColumnWidth( 3,  55 );

	return RetVal;
}

void SVDiscreteOutputsView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint ) 
{
	SVIODoc* pIODoc = (SVIODoc*) GetDocument();
	SVIOController* l_pIOController = nullptr;

	if( nullptr != pIODoc )
	{
		l_pIOController = pIODoc->GetIOController();
	}

	if( nullptr != l_pIOController && ::IsWindow( m_hWnd ) )
	{
		GetListCtrl().SetRedraw(false);

		// First clean up list view
		GetListCtrl().DeleteAllItems();

		m_Items.clear();

		CString strItem;
		long lSize;
		int j;
		int i;
		int k;
		SVPPQObject* pPPQ( nullptr );
		SVDigitalOutputObject *pDigOutput( nullptr );
		SVIOEntryHostStructPtrList ppIOEntries;
		SVIOEntryHostStructPtr pIOEntry;

		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		// Get the number of PPQs
		long lPPQSize = 0;
		if( nullptr != pConfig )
		{
			lPPQSize = pConfig->GetPPQCount( );
		}
		else
		{
			SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorGettingPPQCount, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17010_ErrorGettingPPQCount );
			DebugBreak();
		}

		// Check if any PPQs are here yet
		if( 0 == lPPQSize ) { return; }

		// Result Outputs
		DWORD maxOutput = 0;
		SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputCount( maxOutput );
		for( i = 0; i < static_cast<int>(maxOutput); ++i )
		{
			// First column: Result I/O
			strItem.Format( _T( "Digital Output %d" ), i + 1 );
			GetListCtrl().InsertItem( LVIF_IMAGE | LVIF_TEXT | LVIF_STATE, 
				i, strItem, 
				INDEXTOSTATEIMAGEMASK( 2 ),	// state
				LVIS_STATEIMAGEMASK,		// stateMask
				1, 0 );						// Set item data to nothing

			// Check Module Ready first
			pIOEntry = l_pIOController->GetModuleReady();
			pDigOutput = dynamic_cast< SVDigitalOutputObject* >( SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId ) );
			if( pDigOutput )
			{
				if( i == pDigOutput->GetChannel() )
				{
					GetListCtrl().SetItem( i, 0, LVIF_IMAGE, nullptr, 0, 0, 0, 0 );

					m_Items.SetItemData( i, pIOEntry );

					// Column: Description
					GetListCtrl().SetItemText( i, 1, pDigOutput->GetName() );

					// Column: Force
					if( pDigOutput->IsForced() )
					{
						strItem.Format( _T( "%d" ), pDigOutput->GetForcedValue() ? 1 : 0 );
						GetListCtrl().SetItemText( i, 2, strItem );
					}// end if

					// Column: Inverted
					strItem.Format( _T( "%s" ), pDigOutput->IsInverted() ? _T( "1" ) : _T( "" ) );
					GetListCtrl().SetItemText( i, 3, strItem );

					continue;
				}// end if
			}// end if

			// Check GetRaidErrorBit() first
			pIOEntry = l_pIOController->GetRaidErrorBit();
			pDigOutput = dynamic_cast< SVDigitalOutputObject* >( SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId ) );
			if( pDigOutput )
			{
				if( i == pDigOutput->GetChannel() )
				{
					GetListCtrl().SetItem( i, 0, LVIF_IMAGE, nullptr, 0, 0, 0, 0 );

					m_Items.SetItemData( i, pIOEntry );

					// Column: Description
					GetListCtrl().SetItemText( i, 1, pDigOutput->GetName() );

					// Column: Force
					if( pDigOutput->IsForced() )
					{
						strItem.Format( _T( "%d" ), pDigOutput->GetForcedValue() ? 1 : 0 );
						GetListCtrl().SetItemText( i, 2, strItem );
					}// end if

					// Column: Inverted
					strItem.Format( _T( "%s" ), pDigOutput->IsInverted() ? _T( "1" ) : _T( "" ) );
					GetListCtrl().SetItemText( i, 3, strItem );

					continue;
				}// end if
			}// end if

			for( k = 0; k < lPPQSize; k++ )
			{
				pPPQ = pConfig->GetPPQ( k );
				if( nullptr == pPPQ )
				{
					SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
					e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorGettingPPQ, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17011_ErrorGettingPPQ );
					DebugBreak();
				}

				// Get list of available outputs
				if( !pPPQ->GetAllOutputs( ppIOEntries ) )
				{
					SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
					e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorGettingOutputs, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17012_ErrorGettingOutputs );
					DebugBreak();
				}

				lSize = static_cast< long >( ppIOEntries.size() );

				// Find each digital output
				for( j = 0; j < lSize; j++ )
				{
					pIOEntry = ppIOEntries[j];
					if( pIOEntry->m_ObjectType != IO_DIGITAL_OUTPUT ) { continue; }

					pDigOutput = dynamic_cast< SVDigitalOutputObject* >( SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId ) );

					if( !pDigOutput ) { continue; }

					if( i == pDigOutput->GetChannel() )
					{
						GetListCtrl().SetItem( i, 0, LVIF_IMAGE, nullptr, 0, 0, 0, 0 );

						m_Items.SetItemData( i, pIOEntry );

						// Column: Description
						GetListCtrl().SetItemText( i, 1, pDigOutput->GetName() );

						// Column: Force
						if( pDigOutput->IsForced() )
						{
							strItem.Format( _T( "%d" ), pDigOutput->GetForcedValue() ? 1 : 0 );
							GetListCtrl().SetItemText( i, 2, strItem );
						}// end if

						// Column: Inverted
						strItem.Format( _T( "%s" ), pDigOutput->IsInverted() ? _T( "1" ) : _T( "" ) );
						GetListCtrl().SetItemText( i, 3, strItem );

						break;
					}// end if
				}// end for
			}// end for
		}// end for
		GetListCtrl().SetRedraw(true);
	}// end if

	//CListView::OnUpdate( pSender, lHint, pHint );   //This call will cause flicker
}// end OnUpdate

void SVDiscreteOutputsView::OnLButtonDblClk( UINT nFlags, CPoint point ) 
{
	CString strName;
	SVIOEntryHostStructPtr pIOEntry;
	SVOutputObjectList *pOutputList( nullptr );
	SVDigitalOutputObject *pDigOutput ( nullptr );
	UINT flags;

	int item = GetListCtrl().HitTest( point, &flags );
	SVIODoc* pIODoc = ( SVIODoc* ) GetDocument();

	SVIOController* l_pIOController = nullptr;

	if( nullptr != pIODoc )
	{
		l_pIOController = pIODoc->GetIOController();
	}
	if ( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&
		 TheSVObserverApp.OkToEdit() &&
	     nullptr != l_pIOController )
	{
		if( item >= 0 && item < GetListCtrl().GetItemCount() && 
			( flags & ( LVHT_ONITEMSTATEICON | LVHT_ONITEMICON | LVHT_ONITEMLABEL ) ) )
		{
			SVIOAdjustDialogClass dlg;

			// Search for In or Out
			SVDataItemManager::const_iterator l_Iter = m_Items.GetItemData(item);
			if( l_Iter != m_Items.end() )
			{
				pIOEntry = l_Iter->second;
			}

			if( !( pIOEntry.empty() ) )
			{
				pDigOutput = dynamic_cast< SVDigitalOutputObject* >( SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId ) );
			}
			else
			{
				strName = _T( "" );

				pDigOutput = new SVDigitalOutputObject;

				pDigOutput->SetChannel( item );
				pDigOutput->SetName( strName );
				pDigOutput->Force( FALSE, FALSE );
				pDigOutput->Invert( TRUE );

			}// end else

			if( nullptr != pDigOutput )
			{
				dlg.StrIOName = _T( "Result " ) + GetListCtrl().GetItemText( item, 0 );
				dlg.StrIOName += _T( ", " ) + GetListCtrl().GetItemText( item, 1 );
				dlg.StrIOValue.Format( "%d", pDigOutput->GetValue() ? 1 : 0 );
				dlg.m_pIOEntry   = pIOEntry;
				dlg.m_pDigOutput = pDigOutput;
				dlg.m_bInputMode = FALSE;
				dlg.m_pIODoc	 = GetDocument();

				SVSVIMStateClass::AddState( SV_STATE_EDITING );
				switch( dlg.DoModal() )
				{
				case IDOK:
					{
						SVSVIMStateClass::AddState( SV_STATE_MODIFIED );

						SVConfigurationObject* pConfig( nullptr );
						SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

						// Check if they picked a new output
						if( dlg.m_pIOEntry != pIOEntry )
						{
							SVOutputObjectList* pOutputList( nullptr );
							SVPPQObject* pPPQ( nullptr );
							long k = 0;

							if ( nullptr != pConfig ){ pOutputList = pConfig->GetOutputObjectList( ); }
							if( !( pIOEntry.empty() ) )
							{									
								// Make sure that we first reset the old output
								if( nullptr != pOutputList )
								{
									pOutputList->ResetOutput( pIOEntry );
								}// end if

								pIOEntry->m_Enabled = FALSE;
								pIOEntry->m_IOId.clear();
							}// end if

							if( dlg.m_pIOEntry.empty() )
							{
								if( nullptr != pOutputList )
								{
									pOutputList->DetachOutput( pDigOutput->GetUniqueObjectID() );
								}
								pDigOutput = nullptr;

								if( pIOEntry == l_pIOController->GetModuleReady() )
								{
									pIOEntry->m_ObjectType = IO_INVALID_OBJECT;
									pIOEntry->m_IOId.clear();
								}// end if
								if( pIOEntry == l_pIOController->GetRaidErrorBit())
								{
									pIOEntry->m_ObjectType = IO_INVALID_OBJECT;
									pIOEntry->m_IOId.clear();
								}// end if

							}// end if
							else
							{
								dlg.m_pIOEntry->m_Enabled = TRUE;
								dlg.m_pDigOutput->SetName( dlg.m_pIOEntry->m_pValueObject->GetCompleteObjectName() );
								if( pIOEntry.empty() )
								{
									pOutputList->AttachOutput( pDigOutput );
								}// end if

								if( dlg.m_pIOEntry == l_pIOController->GetModuleReady() )
								{
									dlg.m_pIOEntry->m_ObjectType = IO_DIGITAL_OUTPUT;
									dlg.m_pIOEntry->m_IOId = dlg.m_pDigOutput->GetUniqueObjectID();
								}// end if

								if( dlg.m_pIOEntry == l_pIOController->GetRaidErrorBit() )
								{
									dlg.m_pIOEntry->m_ObjectType = IO_DIGITAL_OUTPUT;
									dlg.m_pIOEntry->m_IOId	= dlg.m_pDigOutput->GetUniqueObjectID();
								}// end if
							}// end else

							long lPPQSize = 0;
							// Force the PPQs to rebuild
							if ( nullptr != pConfig ){ lPPQSize = pConfig->GetPPQCount( ); }

							// Rebuild Outputs
							for( k = 0; k < lPPQSize; k++ )
							{
								pPPQ = pConfig->GetPPQ( k );

								if( nullptr != pPPQ ){ pPPQ->RebuildOutputList(); }
							}// end for

						}// end if

						// Force IO board to update if they still have one selected
						if( !( dlg.m_pIOEntry.empty() ) )
						{
							pOutputList = nullptr;
							if ( nullptr != pConfig ){ pOutputList = pConfig->GetOutputObjectList( ); }
							if( nullptr != pOutputList )
							{
								pOutputList->ResetOutput( dlg.m_pIOEntry );
							}// end if
						}// end if
						break;
					}

				case IDCANCEL:
				default:
					break;
				}// end switch

				SVSVIMStateClass::RemoveState( SV_STATE_EDITING );
				OnUpdate( nullptr, 0, nullptr );
			}
			else
			{
				dlg.StrIOName	= _T( "" );
				dlg.StrIOValue.Format( "%d", 0 );

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
			}// end else
		}// end if
	}// end if
}// end OnLButtonDblClk

