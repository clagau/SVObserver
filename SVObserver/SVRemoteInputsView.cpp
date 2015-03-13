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
#include <boost/config.hpp>
#include <boost/bind.hpp>
#include "SVRemoteInputsView.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObserver.h"
#include "SVIODoc.h"
#include "SVRemoteIOAdjustDialog.h"
#include "SVInputObjectList.h"
#include "SVRemoteInputObject.h"
#include "SVDigitalInputObject1.h"
#include "SVConfigurationObject.h"
#include "SVSVIMStateClass.h"
#include "ErrorNumbers.h"
#include "SVStatusLibrary/ExceptionManager.h"
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
		SVInputObjectList* pInputList;
		SVRemoteInputObject* pRemInput;
		SVIOEntryHostStructPtrList ppIOEntries;
		SVIOEntryHostStructPtr pIOEntry;

		SVConfigurationObject* pConfig = nullptr;
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		// Get list of available inputs
		if( !pConfig->GetInputObjectList( &pInputList ) )
		{
			SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::c_textErrorGettingInputObjectList, StdExceptionParams, Err_17044_SVRemoteInputsView_OnUpdate_ErrorGettingInputObjectList );
			DebugBreak();
		}

		// Check if the list is up yet
		if( nullptr == pInputList ) { return; }

		if( !pInputList->FillInputs( ppIOEntries ) )
		{
			SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::c_textErrorFillingInputs, StdExceptionParams, Err_17045_SVRemoteInputsView_OnUpdate_ErrorFillingInputs );
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
	SVInputObjectList *pInputList;
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
		SVConfigurationObject* pConfig = NULL;
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		if( item >= 0 && item < GetListCtrl().GetItemCount() &&
			( flags & ( LVHT_ONITEMSTATEICON | LVHT_ONITEMICON | LVHT_ONITEMLABEL ) ) )
		{
			// Get list of available inputs
			if( !pConfig->GetInputObjectList( &pInputList ) )
			{
				SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
				e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::c_textErrorGettingInputObjectList, StdExceptionParams, Err_17046_SVRemoteInputsView_OnLButtonDblClk_ErrorGettingInputObjectList );
				DebugBreak();
			}

			if( !pInputList->FillInputs( ppIOEntries ) )
			{
				SvStl::ExceptionMgr1 e; // The default constructor sets the type to LogOnly.
				e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::c_textErrorFillingInputs, StdExceptionParams, Err_17047_SVRemoteInputsView_OnLButtonDblClk_ErrorFillingInputs );
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVRemoteInputsView.cpp_v  $
 * 
 *    Rev 1.4   02 Oct 2013 07:58:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   07 Aug 2013 13:21:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Renamed SVIOEntryHostStructPtrVector to SVIOEntryHostStructPtrList.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 May 2013 16:29:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 015.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   08 May 2013 09:35:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  826
 * SCR Title:  Fix Memory Leaks in I/O Adjustment Dialogs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Used new GetItemData method that combined the gui get and the data get into one method.  This method returns an iterator to the data container for extraction and updates(if non-const) of data element.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:31:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   24 Apr 2013 14:25:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  826
 * SCR Title:  Fix Memory Leaks in I/O Adjustment Dialogs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added SVDataItemManagerTemplate to manage Item Data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:48:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   07 Aug 2012 16:18:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated views to remove flicker.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   09 Jul 2012 14:15:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the include for SVIM State.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   02 Jul 2012 17:33:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   19 Jun 2012 14:15:42   jspila
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
 *    Rev 1.7   19 Apr 2011 16:24:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix issues with IO Assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   18 Apr 2011 11:03:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   09 Mar 2011 13:23:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   fixed issues with remote inputs with the descriptions not matching up with the input name
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Dec 2010 10:38:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix connections and synchronization issues with the IO Sub-system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   13 Dec 2010 11:31:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Change the usage of _variant_t to clear the variable after use to eliminate invalid variant type for assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Dec 2010 13:35:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Oct 2010 11:20:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change internal value object to variant value object from double value object.  Added persistance to configuration object.  Added new dialog to edit the Remote Input Value.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:10:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/