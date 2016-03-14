//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolIOView
//* .File Name       : $Workfile:   SVToolIOView.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:45:28  $
//******************************************************************************

#include "stdafx.h"
#include "SVObserver.h"

#include "SVIODoc.h"
#include "SVIOController.h"
#include "SVIOAdjustDialog.h"

#include "SVDDEOutputObject.h"
#include "SVDigitalOutputObject1.h"

#include "SVToolIOView.h"
#include "SVSVIMStateClass.h"
#include "SVPPQObject.h"
#include "SVConfigurationObject.h"
#include "SVOutputObjectList.h"
#include "SVMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVToolIOView

IMPLEMENT_DYNCREATE(SVToolIOView, CListView)

BEGIN_MESSAGE_MAP(SVToolIOView, CListView)
	//{{AFX_MSG_MAP(SVToolIOView)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	// Standard-Druckbefehle
	ON_COMMAND(ID_FILE_PRINT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CListView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVToolIOView Konstruktion/Destruktion

SVToolIOView::SVToolIOView()
: CListView()
{
}

SVToolIOView::~SVToolIOView()
{
}

BOOL SVToolIOView::PreCreateWindow(CREATESTRUCT& cs)
{
	// ZU ERLEDIGEN: Ändern Sie hier die Fensterklasse oder das Erscheinungsbild, indem Sie
	//  CREATESTRUCT cs modifizieren.

	return CListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// SVToolIOView Zeichnen

void SVToolIOView::OnDraw( CDC* pDC )
{
	SVIODoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}

void SVToolIOView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();
}

/////////////////////////////////////////////////////////////////////////////
// SVToolIOView Drucken

/////////////////////////////////////////////////////////////////////////////
// SVToolIOView Diagnose

#ifdef _DEBUG
void SVToolIOView::AssertValid() const
{
	CListView::AssertValid();
}

void SVToolIOView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

SVIODoc* SVToolIOView::GetDocument() // Die endgültige (nicht zur Fehlersuche kompilierte) Version ist Inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(SVIODoc)));
	return (SVIODoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// SVToolIOView Nachrichten-Handler

BOOL SVToolIOView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	dwStyle |= LVS_REPORT; //LVS_ICON; //LVS_REPORT;
	
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

void SVToolIOView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint ) 
{
	SVIODoc* pIODoc = dynamic_cast< SVIODoc* >( GetDocument() );
	
	if( pIODoc && ::IsWindow( m_hWnd ) )
	{
		SVIOController* l_pIOController( TheSVObserverApp.m_pConfiguration->m_pIOController );

		// First clean up list view
		GetListCtrl().DeleteAllItems();
		
		CString strItem;
		long lSize;
		long lChannel;
		long lPPQSize;
		int h;
		int j;
		int i;
		int k;
		SVPPQObject* pPPQ;
		SVDigitalOutputObject *pDigOutput;
		SVDDEOutputObject *pDdeOutput;
		SVIOEntryStruct **ppIOEntries;
		SVIOEntryStruct *pIOEntry;
		
		// Get the number of PPQs
		if( !TheSVObserverApp.m_pConfiguration->GetPPQCount( lPPQSize ) )
			DebugBreak();

		// Check if any PPQs are here yet
		if( lPPQSize == 0 )
			return;
		
		// Result Outputs
		DWORD maxOutput = 0;
		TheSVObserverApp.m_svIOConfig.GetDigitalOutputCount( maxOutput );
		for( i = 0; i < (long) maxOutput; ++i )
		{
			// First column: Result I/O
			strItem.Format( _T( "Digital Output %d" ), i + 1 );
			GetListCtrl().InsertItem( LVIF_IMAGE | LVIF_TEXT | LVIF_STATE, 
				i, strItem, 
				INDEXTOSTATEIMAGEMASK( 2 ),	// state
				LVIS_STATEIMAGEMASK,		// stateMask
				1, 0 );						// Set item data to NULL
			
			// Check Module Ready first
			pIOEntry = l_pIOController->GetModuleReady();
			pDigOutput = (SVDigitalOutputObject*) pIOEntry->pIOObject;
			if( pDigOutput )
			{			
				if( !pDigOutput->GetChannel( lChannel ) )
					DebugBreak();
				
				if( i == lChannel )
				{
					GetListCtrl().SetItem( i, 0, LVIF_IMAGE, NULL, 0, 0, 0, 0 );
					GetListCtrl().SetItemData( i, (DWORD) pIOEntry );
					
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
			// Check m_pRaidErrorBit first
			pIOEntry = l_pIOController->GetRaidErrorBit();
			pDigOutput = (SVDigitalOutputObject*) pIOEntry->pIOObject;
			if( pDigOutput )
			{			
				if( !pDigOutput->GetChannel( lChannel ) )
					DebugBreak();
				
				if( i == lChannel )
				{
					GetListCtrl().SetItem( i, 0, LVIF_IMAGE, NULL, 0, 0, 0, 0 );
					GetListCtrl().SetItemData( i, (DWORD) pIOEntry );
					
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
				// Get the number of PPQs
				if( !TheSVObserverApp.m_pConfiguration->GetPPQ( k, &pPPQ ) )
					DebugBreak();
				
				// Get list of available outputs
				if( !pPPQ->GetAllOutputs( lSize, ppIOEntries ) )
					DebugBreak();
				
				// Find each digital output
				for( j = 0; j < lSize; j++ )
				{
					pIOEntry = ppIOEntries[j];
					if( pIOEntry->eObjectType != IO_DIGITAL_OUTPUT )
						continue;
				
					pDigOutput = (SVDigitalOutputObject*) pIOEntry->pIOObject;
					
					if( !pDigOutput )
						continue;
					
					if( !pDigOutput->GetChannel( lChannel ) )
						DebugBreak();
					
					if( i == lChannel )
					{
						GetListCtrl().SetItem( i, 0, LVIF_IMAGE, NULL, 0, 0, 0, 0 );
						GetListCtrl().SetItemData( i, (DWORD) pIOEntry );
						
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

				delete [] ppIOEntries;
				
			}// end for
			
		}// end for
		
		h = 0;			
		for( k = 0; k < lPPQSize; k++ )
		{
			// Get the number of PPQs
			if( !TheSVObserverApp.m_pConfiguration->GetPPQ( k, &pPPQ ) )
				DebugBreak();
			
			// Get list of available outputs
			if( !pPPQ->GetAllOutputs( lSize, ppIOEntries ) )
				DebugBreak();
			
			// Find each DDE output
			for( j = 0; j < lSize; j++ )
			{					
				pIOEntry = ppIOEntries[j];
				if( pIOEntry->eObjectType != IO_DDE_OUTPUT )
					continue;

				pDdeOutput = (SVDDEOutputObject*) pIOEntry->pIOObject;
				
				if( !pDdeOutput || !pIOEntry->bEnabled )
					continue;
					
				// First column: Result I/O
				strItem.Format( _T( "DDE Output %d" ), h + 1 );
				GetListCtrl().InsertItem( LVIF_IMAGE | LVIF_TEXT | LVIF_STATE, 
					h + i, strItem, 
					INDEXTOSTATEIMAGEMASK( 2 ),	// state
					LVIS_STATEIMAGEMASK,		// stateMask
					1, 0 );						// Set item data to NULL

				GetListCtrl().SetItem( h + i, 0, LVIF_IMAGE, NULL, 0, 0, 0, 0 );
				GetListCtrl().SetItemData( h + i, (DWORD) pIOEntry );
				
				// Column: Description
				CString strDde;
				CString strItem;
				CString strTemp;

				pDdeOutput->GetItemName( strItem );
				strDde = pDdeOutput->GetName();
				strTemp.Format( "%s - (%s)", strDde, strItem );
				GetListCtrl().SetItemText( h + i, 1, strTemp );

				// Increment the number of dde outputs
				h++;
					
			}// end for

			delete [] ppIOEntries;
				
		}// end for
		
	}// end if
	
}// end OnUpdate

void SVToolIOView::OnLButtonDblClk( UINT nFlags, CPoint point ) 
{
	SVSVIMStateClass svState;
	CString strName;
	SVIOEntryStruct *pIOEntry;
	SVOutputObjectList *pOutputList;
	SVDigitalOutputObject *pDigOutput;
	SVOutputObject *pOutput;
	UINT flags;

	int item = GetListCtrl().HitTest( point, &flags );
	SVIODoc* pIODoc = dynamic_cast< SVIODoc* >( GetDocument() );
	SVIOController* l_pIOController( pIODoc->GetIOController() );

	if ( ! svState.CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&
		 TheSVObserverApp.OkToEdit() &&
	     pIODoc )
	{
		if( item >= 0 && item < GetListCtrl().GetItemCount() && 
			( flags & ( LVHT_ONITEMSTATEICON | LVHT_ONITEMICON | LVHT_ONITEMLABEL ) ) )
		{
			SVIOAdjustDialogClass dlg;

			// Search for In or Out
			pIOEntry = (SVIOEntryStruct*) GetListCtrl().GetItemData( item );
			if( pIOEntry )
				pOutput = (SVOutputObject*) pIOEntry->pIOObject;
			else
			{
				strName = _T( "" );

				pDigOutput = new SVDigitalOutputObject( &TheSVObserverApp.m_svIOConfig );
				
				pDigOutput->SetChannel( item );
				pDigOutput->SetName( strName );
				pDigOutput->Force( FALSE, FALSE );
				pDigOutput->Invert( TRUE );

				pOutput = pDigOutput;
			}// end else

			if( SV_IS_KIND_OF( pOutput, SVDDEOutputObject ) )
			{
				GetDocument()->OnExtrasEditDDEOutputs();
			}// end if
			else if( SV_IS_KIND_OF( pOutput, SVDigitalOutputObject ) )
			{
				pDigOutput = (SVDigitalOutputObject*) pOutput;
				if( pDigOutput )
				{
					dlg.StrIOName = _T( "Result " ) + GetListCtrl().GetItemText( item, 0 );
					dlg.StrIOName += _T( ", " ) + GetListCtrl().GetItemText( item, 1 );
					dlg.StrIOValue.Format( "%d", pDigOutput->GetValue() ? 1 : 0 );
					dlg.m_pIOEntry   = pIOEntry;
					dlg.m_pDigOutput = pDigOutput;
					dlg.m_bInputMode = FALSE;
					dlg.m_pIODoc	 = GetDocument();
					
					svState.AddState( SV_STATE_EDITING );
					switch( dlg.DoModal() )
					{
						case IDOK:
						{
							SVSVIMStateClass oState;
							oState.AddState( SV_STATE_MODIFIED );
							// Check if they picked a new output
							if( dlg.m_pIOEntry != pIOEntry )
							{
								SVOutputObjectList *pOutputList;
								SVPPQObject *pPPQ;
								long lPPQSize;
								long k;

								if( pIOEntry )
								{									
									// Make sure that we first reset the old output
									TheSVObserverApp.m_pConfiguration->GetOutputObjectList( &pOutputList );
									if( pOutputList )
									{
										pOutputList->ResetOutput( pIOEntry );
									}// end if

									pIOEntry->bEnabled = FALSE;
								}// end if
								
								TheSVObserverApp.m_pConfiguration->GetOutputObjectList( &pOutputList );
								if( !dlg.m_pIOEntry )
								{
									pOutputList->RemoveOutput( pDigOutput );
									delete pDigOutput;

									if( pIOEntry == l_pIOController->GetModuleReady() )
									{
										pIOEntry->eObjectType = IO_INVALID_OBJECT;
										pIOEntry->pIOObject	= NULL;
										pIOEntry->lIOIndex	= -1L;
									}// end if
									if( pIOEntry == l_pIOController->GetRaidErrorBit() )
									{
										pIOEntry->eObjectType = IO_INVALID_OBJECT;
										pIOEntry->pIOObject	= NULL;
										pIOEntry->lIOIndex	= -1L;
									}// end if

								}// end if
								else
								{
									dlg.m_pIOEntry->bEnabled = TRUE;
									dlg.m_pDigOutput->SetName( dlg.m_pIOEntry->pValueObject->GetCompleteObjectName() );
									if( pIOEntry == NULL )
									{
										pOutputList->AddOutput( pDigOutput );
									}// end if

									if( dlg.m_pIOEntry == l_pIOController->GetModuleReady() )
									{
										long lIndex;										
										pOutputList->FindOutputIndex( pDigOutput->GetName(), lIndex );
										dlg.m_pIOEntry->eObjectType = IO_DIGITAL_OUTPUT;
										dlg.m_pIOEntry->pIOObject	= dlg.m_pDigOutput;
										dlg.m_pIOEntry->lIOIndex	= lIndex;
									}// end if

									if( dlg.m_pIOEntry == l_pIOController->GetRaidErrorBit() )
									{
										long lIndex;										
										pOutputList->FindOutputIndex( pDigOutput->GetName(), lIndex );
										dlg.m_pIOEntry->eObjectType = IO_DIGITAL_OUTPUT;
										dlg.m_pIOEntry->pIOObject	= dlg.m_pDigOutput;
										dlg.m_pIOEntry->lIOIndex	= lIndex;
									}// end if


								}// end else

								// Force the PPQs to rebuild
								TheSVObserverApp.m_pConfiguration->GetPPQCount( lPPQSize );

								// Rebuild Outputs
								for( k = 0; k < lPPQSize; k++ )
								{
									// Get the number of PPQs
									TheSVObserverApp.m_pConfiguration->GetPPQ( k, &pPPQ );

									pPPQ->RebuildOutputList();
								}// end for
														
							}// end if

							// Force IO board to update if they still have one selected
							if( dlg.m_pIOEntry )
							{
								TheSVObserverApp.m_pConfiguration->GetOutputObjectList( &pOutputList );
								if( pOutputList )
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

					svState.RemoveState( SV_STATE_EDITING );
					OnUpdate( NULL, NULL, NULL );
				}
				else
				{
					SVSVIMStateClass svState;
					dlg.StrIOName	= _T( "" );
					dlg.StrIOValue.Format( "%d", 0 );
					
					switch( dlg.DoModal() )
					{
					case IDOK:
						svState.AddState( SV_STATE_MODIFIED );
						break;
						
					case IDCANCEL:
					default:
						break;
					}// end switch
					
					OnUpdate( NULL, NULL, NULL );
				}// end else

			}// end else if

		}// end if

	}// end if

}// end OnLButtonDblClk

