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
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObserver.h"
#include "SVIODoc.h"
#include "SVIOAdjustDialog.h"
#include "SVDigitalOutputObject1.h"
#include "SVSVIMStateClass.h"
#include "SVPPQObject.h"
#include "SVConfigurationObject.h"
#include "SVOutputObjectList.h"
#include "SVMessage/SVMessage.h"
#include "SVIOController.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVStatusLibrary/ExceptionManager.h"
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
	SVIOController* l_pIOController = NULL;

	if( pIODoc != NULL )
	{
		l_pIOController = pIODoc->GetIOController();
	}

	if( l_pIOController != NULL && ::IsWindow( m_hWnd ) )
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
			SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorGettingPPQCount, StdExceptionParams, SvOi::Err_17010_ErrorGettingPPQCount );
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
				1, 0 );						// Set item data to NULL

			// Check Module Ready first
			pIOEntry = l_pIOController->GetModuleReady();
			pDigOutput = dynamic_cast< SVDigitalOutputObject* >( SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId ) );
			if( pDigOutput )
			{
				if( i == pDigOutput->GetChannel() )
				{
					GetListCtrl().SetItem( i, 0, LVIF_IMAGE, NULL, 0, 0, 0, 0 );

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
					GetListCtrl().SetItem( i, 0, LVIF_IMAGE, NULL, 0, 0, 0, 0 );

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
					SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
					e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorGettingPPQ, StdExceptionParams, SvOi::Err_17011_ErrorGettingPPQ );
					DebugBreak();
				}

				// Get list of available outputs
				if( !pPPQ->GetAllOutputs( ppIOEntries ) )
				{
					SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
					e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorGettingOutputs, StdExceptionParams, SvOi::Err_17012_ErrorGettingOutputs );
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
						GetListCtrl().SetItem( i, 0, LVIF_IMAGE, NULL, 0, 0, 0, 0 );

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
	SVOutputObject *pOutput ( nullptr );
	UINT flags;

	int item = GetListCtrl().HitTest( point, &flags );
	SVIODoc* pIODoc = ( SVIODoc* ) GetDocument();

	SVIOController* l_pIOController = NULL;

	if( pIODoc != NULL )
	{
		l_pIOController = pIODoc->GetIOController();
	}
	if ( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&
		 TheSVObserverApp.OkToEdit() &&
	     l_pIOController != NULL )
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
				pOutput = dynamic_cast< SVOutputObject* >( SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId ) );
			else
			{
				strName = _T( "" );

				pDigOutput = new SVDigitalOutputObject;
				
				pDigOutput->SetChannel( item );
				pDigOutput->SetName( strName );
				pDigOutput->Force( FALSE, FALSE );
				pDigOutput->Invert( TRUE );

				pOutput = pDigOutput;
			}// end else

			if( SV_IS_KIND_OF( pOutput, SVDigitalOutputObject ) )
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
									pDigOutput = NULL;

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
					OnUpdate( NULL, NULL, NULL );
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
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVDiscreteOutputsView.cpp_v  $
 * 
 *    Rev 1.5   01 Oct 2013 12:48:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   07 Aug 2013 13:25:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Renamed SVIOEntryHostStructPtrVector to SVIOEntryHostStructPtrList.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 May 2013 19:45:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   09 May 2013 14:53:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  826
 * SCR Title:  Fix Memory Leaks in I/O Adjustment Dialogs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified OnLButtonDblClk to use new manager. template.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 May 2013 16:02:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   24 Apr 2013 13:53:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  826
 * SCR Title:  Fix Memory Leaks in I/O Adjustment Dialogs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add SVDataItemManagerTemplate to manage item data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 20:06:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   16 Apr 2013 14:24:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:13:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   15 Aug 2012 14:35:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new Singleton version of the SVIOConfigurationInterfaceClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   07 Aug 2012 16:09:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated views to reduce flicker.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   09 Jul 2012 13:59:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the include for SVIM State.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   02 Jul 2012 16:47:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   19 Jun 2012 12:38:26   jspila
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
 *    Rev 1.8   19 Apr 2011 16:09:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix issues with IO Assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   18 Apr 2011 09:26:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   15 Feb 2011 07:12:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  714
 * SCR Title:  Fix issue with Module Ready disappearing from the available outputs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed code so that Module Ready would not disappear from available outputs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   24 Jan 2011 10:01:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   fixed issue with removing an output from the DiscreteOutputs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Dec 2010 09:53:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix connections and synchronization issues with the IO Sub-system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   13 Dec 2010 11:17:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Change the usage of _variant_t to clear the variable after use to eliminate invalid variant type for assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Dec 2010 07:59:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Dec 2009 15:56:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:08:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
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