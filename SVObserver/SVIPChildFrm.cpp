//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIPSplitterFrame
//* .File Name       : $Workfile:   SVIPChildFrm.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   02 Sep 2014 12:15:38  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVIPChildFrm.h"
#include "SVGlobal.h"
#include "SVIPDoc.h"
#include "SVResultView.h"
#include "ToolSetView.h"
#include "SVUserMessage.h"
#include "SVMainFrm.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVIPSplitterFrame
// Create a splitter window which splits a toolset view, 
// image view and result view
//                   |
//                   |
//                   |
//    Tool Set View  | Image View
//                   |
//                   |
//                   |
//    --------------------------------------------------
//
//           Result View
//

IMPLEMENT_DYNCREATE( SVIPSplitterFrame, CMDIChildWnd )

BEGIN_MESSAGE_MAP(SVIPSplitterFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(SVIPSplitterFrame)
	ON_MESSAGE(SV_MDICHILDFRAME_UPDATE_ALL_DATA, OnUpdateAllData)
	//}}AFX_MSG_MAP
	ON_WM_MDIACTIVATE()
END_MESSAGE_MAP()
#pragma endregion Declarations

LRESULT SVIPSplitterFrame::OnUpdateAllData(WPARAM wParam, LPARAM lParam)
{
	if( m_pIPDoc != nullptr )
	{
		m_pIPDoc->UpdateAllData();
	}

	return S_OK;
}

#pragma region Constructor
SVIPSplitterFrame::SVIPSplitterFrame()
: CMDIChildWnd(), m_pIPDoc( nullptr )
{
}

SVIPSplitterFrame::~SVIPSplitterFrame()
{
}
#pragma endregion Constructor

BOOL SVIPSplitterFrame::PreCreateWindow( CREATESTRUCT& cs )
{
	cs.cx = SV_DEFAULT_IMAGE_VIEW_WIDTH + 100;
	cs.cy = SV_DEFAULT_IMAGE_VIEW_HEIGHT + 100;

	return CMDIChildWnd::PreCreateWindow( cs );
}

void SVIPSplitterFrame::SetViewSize( CWnd *l_pView, CSize &p_rViewSize )
{
	CSplitterWnd *l_pHeightWnd = nullptr;
	int l_iHeightIndex = 0;

	CSplitterWnd *l_pWidthWnd = nullptr;
	int l_iWidthIndex = 0;

	if ( l_pView == m_oWndSplitter1.GetPane( 1, 0 ) ) // ResultView
	{
		l_pHeightWnd = &m_oWndSplitter1;
		l_iHeightIndex = 1;
	}
	else if ( l_pView == m_oWndSplitter2.GetPane( 0, 0 ) ) // ToolSetView
	{
		l_pHeightWnd = &m_oWndSplitter1;
		l_iHeightIndex = 0;

		l_pWidthWnd = &m_oWndSplitter2;
		l_iWidthIndex = 0;
	}
	else if ( l_pView == m_oWndSplitter4.GetPane( 0, 0 ) ) // ImageScrollView #1
	{
		l_pHeightWnd = &m_oWndSplitter3;
		l_iHeightIndex = 0;

		l_pWidthWnd = &m_oWndSplitter4;
		l_iWidthIndex = 0;
	}
	else if ( l_pView == m_oWndSplitter4.GetPane( 0, 1 ) ) // ImageScrollView #2
	{
		l_pHeightWnd = &m_oWndSplitter3;
		l_iHeightIndex = 0;

		l_pWidthWnd = &m_oWndSplitter4;
		l_iWidthIndex = 1;
	}
	else if ( l_pView == m_oWndSplitter5.GetPane( 0, 0 ) ) // ImageScrollView #3
	{
		l_pHeightWnd = &m_oWndSplitter3;
		l_iHeightIndex = 1;

		l_pWidthWnd = &m_oWndSplitter5;
		l_iWidthIndex = 0;
	}
	else if ( l_pView == m_oWndSplitter5.GetPane( 0, 1 ) ) // ImageScrollView #4
	{
		l_pHeightWnd = &m_oWndSplitter3;
		l_iHeightIndex = 1;

		l_pWidthWnd = &m_oWndSplitter5;
		l_iWidthIndex = 1;
	}
	else if ( l_pView == m_oWndSplitter5.GetPane( 0, 2 ) ) // ImageScrollView #5
	{
		l_pHeightWnd = &m_oWndSplitter3;
		l_iHeightIndex = 1;

		l_pWidthWnd = &m_oWndSplitter5;
		l_iWidthIndex = 2;
	}
	else if ( l_pView == m_oWndSplitter6.GetPane( 0, 0 ) ) // ImageScrollView #6
	{
		l_pHeightWnd = &m_oWndSplitter3;
		l_iHeightIndex = 1;

		l_pWidthWnd = &m_oWndSplitter6;
		l_iWidthIndex = 0;
	}
	else if ( l_pView == m_oWndSplitter6.GetPane( 0, 1 ) ) // ImageScrollView #7
	{
		l_pHeightWnd = &m_oWndSplitter3;
		l_iHeightIndex = 1;

		l_pWidthWnd = &m_oWndSplitter6;
		l_iWidthIndex = 1;
	}
	else if ( l_pView == m_oWndSplitter6.GetPane( 0, 2 ) ) // ImageScrollView #8
	{
		l_pHeightWnd = &m_oWndSplitter3;
		l_iHeightIndex = 1;

		l_pWidthWnd = &m_oWndSplitter6;
		l_iWidthIndex = 2;
	}

	if ( l_pHeightWnd != nullptr )
	{
		l_pHeightWnd->SetRowInfo( l_iHeightIndex, p_rViewSize.cy, 10 );

		l_pHeightWnd->RecalcLayout();
	}

	if ( l_pWidthWnd != nullptr )
	{
		l_pWidthWnd->SetColumnInfo( l_iWidthIndex, p_rViewSize.cx, 10 );

		l_pWidthWnd->RecalcLayout();
	}
}

BOOL SVIPSplitterFrame::OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* PContext )
{
	if( PContext != nullptr )
	{
		m_pIPDoc = dynamic_cast< SVIPDoc* >( PContext->m_pCurrentDoc );
	}

	if( m_pIPDoc != nullptr )
	{
		m_pIPDoc->SetMDIChild( this );
	}
	//
	// Use these values for the creation of view windows.
	//
	long l;

	//
	// Create a splitter with 2 rows, 1 column
	//
	if( !m_oWndSplitter1.CreateStatic( this, 2, 1, WS_CHILD | WS_VISIBLE, 
										AFX_IDW_PANE_FIRST ) )
	{
		return FALSE;
	}

	//
	// Add the first splitter pane - which is a nested splitter with 2 columns
	// for the Tool Set View
	// Create the nested splitter with 1 row, 2 columns
	//
	if( ! m_oWndSplitter2.CreateStatic( &m_oWndSplitter1, 1, 2,
										WS_CHILD | WS_VISIBLE | WS_BORDER,
										m_oWndSplitter1.IdFromRowCol( 0, 0 ) ) )
	{
		return FALSE;
	}

	//
	// Attach the ToolSetView to the top left pane
	//
	if( ! m_oWndSplitter2.CreateView( 0, 0, RUNTIME_CLASS( ToolSetView ),
								CSize( SV_DEFAULT_TOOL_SET_VIEW_WIDTH, SV_DEFAULT_TOOL_SET_VIEW_HEIGHT ), 
								PContext ) )
	{
		return FALSE;
	}

	//
	// Add the next splitter pane - which is a nested splitter with 3 rows
	// for the three rows or image views
	// Create the nested splitter with 3 rows, 1 column
	//
	if( ! m_oWndSplitter3.CreateStatic( &m_oWndSplitter2, 3, 1,
										WS_CHILD | WS_VISIBLE | WS_BORDER,
										m_oWndSplitter2.IdFromRowCol( 0, 1 ) ) )
	{
		return FALSE;
	}

	//
	// Add the next splitter pane - which is a nested splitter with 2 columns
	// for the two main image views
	// Create the nested splitter with 1 rows, 2 column
	//
	if( ! m_oWndSplitter4.CreateStatic( &m_oWndSplitter3, 1, 2,
										WS_CHILD | WS_VISIBLE | WS_BORDER,
										m_oWndSplitter3.IdFromRowCol( 0, 0 ) ) )
	{
		return FALSE;
	}

	for( l = 0; l < 2; l++ )
	{
		if( ! m_oWndSplitter4.CreateView( 0, l, PContext->m_pNewViewClass, 
										CSize( 100, 100 ), PContext ) )
		{
			return FALSE;
		}// end if
	}// end for

	//
	// Add the next splitter pane - which is a nested splitter with 3 columns
	// for three tool image views
	// Create the nested splitter with 1 rows, 3 column
	//
	if( ! m_oWndSplitter5.CreateStatic( &m_oWndSplitter3, 1, 3,
										WS_CHILD | WS_VISIBLE | WS_BORDER,
										m_oWndSplitter3.IdFromRowCol( 1, 0 ) ) )
	{
		return FALSE;
	}

	for( l = 0; l < 3; l++ )
	{
		if( ! m_oWndSplitter5.CreateView( 0, l, PContext->m_pNewViewClass, 
										CSize( 100, 100 ), PContext ) )
		{
			return FALSE;
		}// end if
	}// end for

	//
	// Add the next splitter pane - which is a nested splitter with 3 columns
	// for three tool image views
	// Create the nested splitter with 1 rows, 3 column
	//
	if( ! m_oWndSplitter6.CreateStatic( &m_oWndSplitter3, 1, 3,
										WS_CHILD | WS_VISIBLE | WS_BORDER,
										m_oWndSplitter3.IdFromRowCol( 2, 0 ) ) )
	{
		return FALSE;
	}

	for( l = 0; l < 3; l++ )
	{
		if( ! m_oWndSplitter6.CreateView( 0, l, PContext->m_pNewViewClass, 
										CSize( 100, 100 ), PContext ) )
		{
			return FALSE;
		}// end if
	}// end for

	// Add the result view to the bottom row but after the toolset tab view
	//
	// SVResultViewClass view window
	//
	if( !m_oWndSplitter1.CreateView( 1, 0, RUNTIME_CLASS( SVResultViewClass ), 
									CSize( SV_DEFAULT_RESULT_VIEW_WIDTH, SV_DEFAULT_RESULT_VIEW_HEIGHT ), 
									PContext ) )
	{
		return FALSE;
	}

	// Set min and ideal sizes...
	// NOTE: 
	// The height of tool set view and image view panes, and the width of the 
	// image view pane will be set in BOOL SVImageViewClass::BuildImageWindow()

	// Set width of tool set view pane...
	m_oWndSplitter2.SetColumnInfo( 0, SV_DEFAULT_TOOL_SET_VIEW_WIDTH, 10 );

	// Set height of result view pane...
	m_oWndSplitter1.SetRowInfo( 1, SV_DEFAULT_RESULT_VIEW_HEIGHT, 10 );

	//activate ImageScrollView #1
	CView *pView = dynamic_cast< CView* >( m_oWndSplitter4.GetPane( 0, 0 ) );
	if(pView)
	{
		SetActiveView( pView );
	}

	return TRUE;
}// end OnCreateClient

/////////////////////////////////////////////////////////////////////////////
//
//
//
// .History
//	 Date		Author		Comment                                       
//  :16.06.1999	FRB			First Implementation
//	:
//
void SVIPSplitterFrame::GetToolWidthAndResultHeight( int& nWidthToolSetView, int& nHeightResultView )
{
	//
	// Work on tool set view..
	//
	int nWidthMin;

	m_oWndSplitter2.GetColumnInfo( 0, nWidthToolSetView, nWidthMin );

	//
	// Work on result view...
	//
	int nHeightMin;

	m_oWndSplitter1.GetRowInfo( 1, nHeightResultView, nHeightMin );
}

void SVIPSplitterFrame::SetDefaultPaneSizes(RECT &BoundingRect)
{
	CRect oRect = BoundingRect;
	long lHeight = oRect.Height();
	long lWidth = oRect.Width();

	m_oWndSplitter1.SetRowInfo( 0, SV_DEFAULT_TOOL_SET_VIEW_HEIGHT, 10 );
	m_oWndSplitter1.SetRowInfo( 1, lHeight - SV_DEFAULT_TOOL_SET_VIEW_HEIGHT, 10 );

	m_oWndSplitter2.SetColumnInfo( 0, SV_DEFAULT_TOOL_SET_VIEW_WIDTH, 10 );
	m_oWndSplitter2.SetColumnInfo( 1, lWidth - SV_DEFAULT_TOOL_SET_VIEW_WIDTH, 10 );

	m_oWndSplitter3.SetRowInfo( 0, SV_DEFAULT_TOOL_SET_VIEW_HEIGHT, 10 );
	m_oWndSplitter3.SetRowInfo( 1, 0, 10 );
	m_oWndSplitter3.SetRowInfo( 2, 0, 10 );

	m_oWndSplitter4.SetColumnInfo( 0, lWidth - SV_DEFAULT_TOOL_SET_VIEW_WIDTH, 10 );
	m_oWndSplitter4.SetColumnInfo( 1, 0, 10 );

	m_oWndSplitter5.SetColumnInfo( 0, lWidth - SV_DEFAULT_TOOL_SET_VIEW_WIDTH, 10 );
	m_oWndSplitter5.SetColumnInfo( 1, 0, 10 );
	m_oWndSplitter5.SetColumnInfo( 2, 0, 10 );

	m_oWndSplitter6.SetColumnInfo( 0, lWidth - SV_DEFAULT_TOOL_SET_VIEW_WIDTH, 10 );
	m_oWndSplitter6.SetColumnInfo( 1, 0, 10 );
	m_oWndSplitter6.SetColumnInfo( 2, 0, 10 );

	m_oWndSplitter1.RecalcLayout();
	m_oWndSplitter2.RecalcLayout();
	m_oWndSplitter3.RecalcLayout();
	m_oWndSplitter4.RecalcLayout();
	m_oWndSplitter5.RecalcLayout();
	m_oWndSplitter6.RecalcLayout();
}// end SetDefaultPaneSizes

void SVIPSplitterFrame::RefreshAllSplitters()
{
	m_oWndSplitter1.RefreshSplitBars();
	m_oWndSplitter2.RefreshSplitBars();
	m_oWndSplitter3.RefreshSplitBars();
	m_oWndSplitter4.RefreshSplitBars();
	m_oWndSplitter5.RefreshSplitBars();
	m_oWndSplitter6.RefreshSplitBars();
}

void SVIPSplitterFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);

	if(FALSE == bActivate)
	{
		RefreshAllSplitters();
		SVMainFrame* pFrame = dynamic_cast<SVMainFrame*>( AfxGetMainWnd() );
		if(nullptr != pFrame)
		{
			pFrame->EnableZoomToolbar(false);
		}
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVIPChildFrm.cpp_v  $
 * 
 *    Rev 1.2   02 Sep 2014 12:15:38   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  921
 * SCR Title:  Add more complete zoom functionality. (runpage)
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Changed method OnCreateClient to check before calling SetActiveView.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Aug 2014 16:03:22   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  921
 * SCR Title:  Add more complete zoom functionality. (runpage)
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   blue frame around active panel: new functions 
 * RefreshallSplitter, OnMDIActivate
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 11:16:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.15   02 Jul 2012 16:58:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.14   07 Oct 2011 14:22:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated MDI Child Window and IP Document to fix threading problems by moving the data through a lockless queue and notifying the IP Document via the MDI Child Window with a windows message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.13   27 Jan 2011 11:20:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.12   01 Jun 2010 10:43:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.11   17 Feb 2005 14:35:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.10   22 Apr 2003 10:31:26   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.9   14 Mar 2003 16:49:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Modified restoring of image panes to revert to saved position.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.8   19 Nov 2002 14:39:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed COM support
 *             PreCreateWindow
 *             Initial size = 100x100
 *             Handle additional splitters
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.7   04 Oct 2001 15:43:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  229
 * SCR Title:  Upgrade SVObserver to Version 3.34 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified source code to remove compiler warning message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.6   14 Mar 2000 14:32:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  96
 * SCR Title:  Scroll/Zoom images displayed in the inspection document.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Revised SetSavedPaneSizes to coorectly restore height of the views.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.5   23 Feb 2000 14:53:34   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  96
 * SCR Title:  Scroll/Zoom images displayed in the inspection document.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Changed SetDefaultPaneSizes to increase the size of the initial results view window
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.4   16 Feb 2000 16:49:30   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  96
 * SCR Title:  Scroll/Zoom images displayed in the inspection document.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Added multi-image display to the IPD
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.3   08 Feb 2000 11:03:06   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  96
 * SCR Title:  Scroll/Zoom images displayed in the inspection document.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Changes to initialization (SVImageViewScroll is now the
 * child view).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.2   Feb 01 2000 10:18:48   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  92
 * SCR Title:  Remove Obsolete SVChildFrame class
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Removed obsolete class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/