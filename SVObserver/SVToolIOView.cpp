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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVToolIOView.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:45:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   08 Dec 2010 13:45:16   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to include changes in notification functionality using the Observer Design Pattern.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   09 Nov 2010 16:30:32   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  704
   SCR Title:  Upgrade SVObserver Version for 5.33 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to remove duplicate container objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   01 Jun 2010 11:10:08   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   22 Jan 2007 10:36:38   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  588
   SCR Title:  Add the RAID Error Bit for the digital IO
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added the new RaidError bit
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   02 Aug 2005 13:56:30   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  455
   SCR Title:  New Security for SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added OKToEdit() to make menu security more consistant.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   01 Aug 2005 08:40:32   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  455
   SCR Title:  New Security for SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Modified Access Points for "Editing the tool set" to make their behavior more consistant
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   28 Jul 2005 16:11:00   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  455
   SCR Title:  New Security for SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Updated GUI Editing Flag to block mode changes from SIAC
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   21 Jun 2005 13:07:34   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  455
   SCR Title:  New Security for SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Initial changes to add SVSecurity
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   18 Aug 2003 16:06:34   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  322
   SCR Title:  Add Additional Digital IO Resources to SVObserver
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Removed SVDigitalIOControlClass from class.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   08 Jul 2003 09:08:02   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  322
   SCR Title:  Add Additional Digital IO Resources to SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Updated OnInitialUpdate, OnUpdate, and OnLButtonDblClk methods to implement change in SVDigitalIOControlClass.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   01 May 2003 16:14:36   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  341
   SCR Title:  DDE Input and Outputs are no longer working correctly
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Fixed some small bugs and changed the display on the I/O Page.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   25 Apr 2003 11:25:56   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  341
   SCR Title:  DDE Input and Outputs are no longer working correctly
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Fixed several problems so that the DDE inputs and outputs will work like they used to in pre SVO 4.00 versions.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   22 Apr 2003 17:07:50   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
 */
