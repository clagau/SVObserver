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
#include <boost/config.hpp>
#include <boost/bind.hpp>
#include "SVDiscreteInputsView.h"
#include "SVIOLibrary\SVIOConfigurationInterfaceClass.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVObserver.h"
#include "SVIODoc.h"
#include "SVIOAdjustDialog.h"
#include "SVInputObjectList.h"
#include "SVDigitalInputObject1.h"
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
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorGettingInputObjectList, StdMessageParams, SvOi::Err_17006_ErrorGettingInputObjectList );
			DebugBreak();
			return; 
		}

		if( !pInputList->FillInputs( ppIOEntries ) )
		{
			SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorFillingInputs, StdMessageParams, SvOi::Err_17007_ErrorFillingInputs );
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
									  1, 0 );						// Set item data to NULL

			// Find each digital input
			for( j = 0; j < lSize; j++ )
			{
				pIOEntry = ppIOEntries[j];
				if( pIOEntry->m_ObjectType != IO_DIGITAL_INPUT ) { continue; }

				pDigInput = dynamic_cast< SVDigitalInputObject* >( SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId ) );

				if( !pDigInput ) { continue; }

				if( i == pDigInput->GetChannel() )
				{
					GetListCtrl().SetItem( i, 0, LVIF_IMAGE, NULL, 0, 0, 0, 0 );

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
				e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorGettingInputObjectList, StdMessageParams, SvOi::Err_17008_ErrorGettingInputObjectList );
				DebugBreak();
			}

			if( !pInputList->FillInputs( ppIOEntries ) )
			{
				SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
				e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorFillingInputs, StdMessageParams, SvOi::Err_17009_ErrorFillingInputs );
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

				OnUpdate( NULL, NULL, NULL );

				SVSVIMStateClass::RemoveState( SV_STATE_EDITING );
			}// end if
		}// end if
	}// end if
}// end OnLButtonDblClk

void SVDiscreteInputsView::OnDestroy()
{
	CListView::OnDestroy();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVDiscreteInputsView.cpp_v  $
 * 
 *    Rev 1.4   01 Oct 2013 12:48:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   07 Aug 2013 13:25:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Renamed SVIOEntryHostStructPtrVector to SVIOEntryHostStructPtrList.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 May 2013 16:29:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 015.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   08 May 2013 09:35:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  826
 * SCR Title:  Fix Memory Leaks in I/O Adjustment Dialogs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Used new GetItemData method that combined the gui get and the data get into one method.  This method returns an iterator to the data container for extraction and updates(if non-const) of data element.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:02:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   24 Apr 2013 13:53:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  826
 * SCR Title:  Fix Memory Leaks in I/O Adjustment Dialogs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add SVDataItemManagerTemplate to manage item data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:13:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   15 Aug 2012 14:35:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new Singleton version of the SVIOConfigurationInterfaceClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   07 Aug 2012 16:09:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated views to reduce flicker.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   09 Jul 2012 13:59:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the include for SVIM State.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   02 Jul 2012 16:47:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   19 Jun 2012 12:38:28   jspila
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
 *    Rev 1.7   19 Apr 2011 16:09:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix issues with IO Assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   18 Apr 2011 09:27:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   15 Dec 2010 09:53:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix connections and synchronization issues with the IO Sub-system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   13 Dec 2010 11:17:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Change the usage of _variant_t to clear the variable after use to eliminate invalid variant type for assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 Dec 2010 07:59:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Nov 2010 10:50:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed un-necessary include statements.
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
 *    Rev 1.0   14 Nov 2008 14:08:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   02 Aug 2005 13:52:50   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  455
   SCR Title:  New Security for SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added Functions OkToEdit and OkToEditMove to make menus more consistant.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   01 Aug 2005 08:40:32   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  455
   SCR Title:  New Security for SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Modified Access Points for "Editing the tool set" to make their behavior more consistant
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   21 Jun 2005 13:07:38   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  455
   SCR Title:  New Security for SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Initial changes to add SVSecurity
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   18 Aug 2003 15:51:08   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  322
   SCR Title:  Add Additional Digital IO Resources to SVObserver
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Removed SVDigitalIOClass from class methods.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   08 Jul 2003 12:08:10   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  322
   SCR Title:  Add Additional Digital IO Resources to SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     changed OnUpdate and OnInitialUpdate for new IO DLL interface
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   01 May 2003 16:14:32   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  341
   SCR Title:  DDE Input and Outputs are no longer working correctly
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Fixed some small bugs and changed the display on the I/O Page.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   22 Apr 2003 11:24:34   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   20 Nov 2002 09:03:04   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  272
   SCR Title:  Outputs do not always get set correctly.
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     changes made due to change in how the IO work.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   11 Nov 2002 11:52:42   cschmittinger
   Project:  SVObserver
   Change Request (SCR) nbr:  256
   SCR Title:  Improvements to SVObserver's Configuration Report layout.
   Checked in by:  cSchmittinger;  Carl Schmittinger
   Change Description:  
     Removed all print features and moved them in SVConfigurationPrint to consolidate code.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
