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
#include "Definitions/GlobalConst.h"
#include "SVIPDoc.h"
#include "ToolSetView.h"
#include "Definitions/SVUserMessage.h"
#include "SVMainFrm.h"
#include "ResultTabbedView.h"
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
	if( nullptr != m_pIPDoc )
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
	cs.cx = SvDef::cDefaultImageViewWidth + 100;
	cs.cy = SvDef::cDefaultImageViewHeight + 100;

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

	if ( nullptr != l_pHeightWnd )
	{
		l_pHeightWnd->SetRowInfo( l_iHeightIndex, p_rViewSize.cy, 10 );

		l_pHeightWnd->RecalcLayout();
	}

	if ( nullptr != l_pWidthWnd )
	{
		l_pWidthWnd->SetColumnInfo( l_iWidthIndex, p_rViewSize.cx, 10 );

		l_pWidthWnd->RecalcLayout();
	}
}

BOOL SVIPSplitterFrame::OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* PContext )
{
	if( nullptr != PContext )
	{
		m_pIPDoc = dynamic_cast< SVIPDoc* >( PContext->m_pCurrentDoc );
	}

	if( nullptr != m_pIPDoc )
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
	if( !m_oWndSplitter1.CreateStatic( this, 2, 1, WS_CHILD | WS_VISIBLE, AFX_IDW_PANE_FIRST ) )
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
								CSize( SvDef::cDefaultToolsetViewWidth, SvDef::cDefaultToolsetViewHeight ), 
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
	if( !m_oWndSplitter1.CreateView( 1, 0, RUNTIME_CLASS( ResultTabbedView ),
									CSize( SvDef::cDefaultResultViewWidth, SvDef::cDefaultResultViewHeight ), 
									PContext ) )
	{
		return FALSE;
	}

	// Set min and ideal sizes...
	// NOTE: 
	// The height of tool set view and image view panes, and the width of the 
	// image view pane will be set in BOOL SVImageViewClass::BuildImageWindow()

	// Set width of tool set view pane...
	m_oWndSplitter2.SetColumnInfo( 0, SvDef::cDefaultToolsetViewWidth, 10 );

	// Set height of result view pane...
	m_oWndSplitter1.SetRowInfo( 1, SvDef::cDefaultToolsetViewHeight, 10 );

	//activate ImageScrollView #1
	CView *pView = dynamic_cast< CView* >( m_oWndSplitter4.GetPane( 0, 0 ) );
	if(pView)
	{
		SetActiveView( pView );
	}

	return TRUE;
}// end OnCreateClient

void SVIPSplitterFrame::SetDefaultPaneSizes(RECT &BoundingRect)
{
	CRect oRect = BoundingRect;
	long lHeight = oRect.Height();
	long lWidth = oRect.Width();

	m_oWndSplitter1.SetRowInfo( 0, SvDef::cDefaultToolsetViewHeight, 10 );
	m_oWndSplitter1.SetRowInfo( 1, lHeight - SvDef::cDefaultToolsetViewHeight, 10 );

	m_oWndSplitter2.SetColumnInfo( 0, SvDef::cDefaultToolsetViewWidth, 10 );
	m_oWndSplitter2.SetColumnInfo( 1, lWidth - SvDef::cDefaultToolsetViewWidth, 10 );

	m_oWndSplitter3.SetRowInfo( 0, SvDef::cDefaultToolsetViewHeight, 10 );
	m_oWndSplitter3.SetRowInfo( 1, 0, 10 );
	m_oWndSplitter3.SetRowInfo( 2, 0, 10 );

	m_oWndSplitter4.SetColumnInfo( 0, lWidth - SvDef::cDefaultToolsetViewWidth, 10 );
	m_oWndSplitter4.SetColumnInfo( 1, 0, 10 );

	m_oWndSplitter5.SetColumnInfo( 0, lWidth - SvDef::cDefaultToolsetViewWidth, 10 );
	m_oWndSplitter5.SetColumnInfo( 1, 0, 10 );
	m_oWndSplitter5.SetColumnInfo( 2, 0, 10 );

	m_oWndSplitter6.SetColumnInfo( 0, lWidth - SvDef::cDefaultToolsetViewWidth, 10 );
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

