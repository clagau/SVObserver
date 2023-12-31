//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageViewScroll
//* .File Name       : $Workfile:   SVImageViewScroll.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   02 Sep 2014 12:13:52  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <comdef.h>
#include "SVImageViewScroll.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "ObjectInterfaces/IObjectWriter.h"
#include "Definitions/GlobalConst.h"
#include "SVImageView.h"
#include "SVStatusLibrary/SvimState.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVIPSplitterFrame.h"
#include "SVMainFrm.h"
#pragma endregion Includes

IMPLEMENT_DYNCREATE( SVImageViewScroll, CScrollView )

BEGIN_MESSAGE_MAP(SVImageViewScroll, CScrollView)
	//{{AFX_MSG_MAP(SVImageViewScroll)
	ON_WM_MOUSEMOVE()
	ON_WM_CONTEXTMENU()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	//! When using Range the IDs must be sequential
	ON_COMMAND_RANGE(ID_ZOOM_SMALLEST, ID_ZOOM_FIT_HEIGHT, OnZoomTypeChanged)
	ON_UPDATE_COMMAND_UI_RANGE(ID_ZOOM_SMALLEST, ID_ZOOM_FIT_HEIGHT, OnUpdateZoomTypeChanged)
	ON_COMMAND(ID_ZOOM_SLIDER_MOVED, &SVImageViewScroll::OnZoomSliderMoved)
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

SVImageViewScroll::SVImageViewScroll()
: CScrollView()
{
	CRuntimeClass* pImageRunTime = RUNTIME_CLASS(SVImageView);
	m_pView = static_cast< SVImageView* >( pImageRunTime->CreateObject() );
	Log_Assert(m_pView->IsKindOf(RUNTIME_CLASS(SVImageView)));

	m_oOldScrollPoint = CPoint( 0, 0 );
	m_oOldMaxScrollPoint = CPoint( 0, 0 );
	m_hWindowBackgroundColor = ::CreateSolidBrush( SvDef::cDefaultImageViewBackgroundColor );
	m_SizeTotal = CSize( 0, 0 );
}

SVImageViewScroll::~SVImageViewScroll()
{
	if( m_hWindowBackgroundColor )
	{
		DeleteObject( m_hWindowBackgroundColor );
	}
}

BOOL SVImageViewScroll::OnEraseBkgnd( CDC* p_pDC )
{
	BOOL l_bOk = TRUE;

	int l_iMapMode;
	SIZE l_oSizeTotal;
	SIZE l_oSizePage;
	SIZE l_oSizeLine;

	GetDeviceScrollSizes( l_iMapMode, l_oSizeTotal, l_oSizePage, l_oSizeLine );

	CRect l_oRect( CPoint( 0, 0 ), l_oSizeTotal );

	ValidateRect( l_oRect ); // Removed painted area

	l_bOk = CScrollView::OnEraseBkgnd( p_pDC ); // Clears non-painted area

	InvalidateRect( l_oRect, false );  // invalidates painted area

	return l_bOk;
}

void SVImageViewScroll::OnDraw(CDC*)
{
}

void SVImageViewScroll::OnInitialUpdate()
{
	int l_iMapMode;
	SIZE l_oSizeTotal;
	SIZE l_oSizePage;
	SIZE l_oSizeLine;

	GetDeviceScrollSizes( l_iMapMode, l_oSizeTotal, l_oSizePage, l_oSizeLine );

	SetImageSize( l_oSizeTotal );

	CScrollView::OnInitialUpdate();
}

BOOL SVImageViewScroll::Create(LPCTSTR LPSZClassName, LPCTSTR LPSZWindowName, DWORD DWStyle, const RECT& Rect, CWnd* PParentWnd, UINT NID, CCreateContext* PContext)
{
	BOOL Result( TRUE );

	WNDCLASS WindowClass;
	if( !::GetClassInfo( AfxGetInstanceHandle(), _T( "Image Scroll" ),  &WindowClass) )
	{
		memset( &WindowClass, 0, sizeof(WNDCLASS) );
		WindowClass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		WindowClass.lpfnWndProc = ::DefWindowProc; 
		WindowClass.hInstance = AfxGetInstanceHandle();
		WindowClass.hbrBackground = m_hWindowBackgroundColor;
		WindowClass.lpszMenuName = nullptr;
		WindowClass.lpszClassName = _T( "Image Scroll" );

		Result = AfxRegisterClass(&WindowClass);
	}
	
	if( Result )
	{
		if (CWnd::Create( WindowClass.lpszClassName, _T( "Untitled Image Scroll" ), DWStyle, Rect, PParentWnd, NID, PContext ))
		{
			Result = m_pView->Create(LPSZClassName, LPSZWindowName, DWStyle, Rect, static_cast<CWnd*> (this), NID, PContext);
		}
	}

	return Result;
}

void SVImageViewScroll::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if( !SvimState::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) )
	{
		int l_iMapMode;
		SIZE l_oSizeTotal;
		SIZE l_oSizePage;
		SIZE l_oSizeLine;

		GetDeviceScrollSizes( l_iMapMode, l_oSizeTotal, l_oSizePage, l_oSizeLine );

		if( m_SizeTotal.cx != l_oSizeTotal.cx || m_SizeTotal.cy != l_oSizeTotal.cy )
		{
			SetImageSize( l_oSizeTotal );

			CScrollView::OnUpdate( pSender, lHint, pHint );   // This call will cause flicker
		}
	}
}

void SVImageViewScroll::OnMouseMove(UINT nFlags, CPoint point)
{
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

	CScrollView::OnMouseMove(nFlags, point);
}

BOOL SVImageViewScroll::OnCommand(WPARAM wParam, LPARAM lParam)
{
	BOOL l_bOk = TRUE;

	if (nullptr != m_pView)
	{
		l_bOk = m_pView->OnCommand (wParam, lParam);
	}

	l_bOk = CScrollView::OnCommand(wParam, lParam) && l_bOk;

	return l_bOk;
}

void SVImageViewScroll::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (nullptr != m_pView)
	{
		m_pView->OnContextMenu( pWnd, point );
	}
}

void SVImageViewScroll::SetViewSize( CSize &p_rcSize )
{
	CWnd *l_pcWnd = this;
	SVIPSplitterFrame *l_pSplitterFrame = nullptr;

	do
	{
		l_pcWnd = l_pcWnd->GetParent();

		if (nullptr != l_pcWnd)
		{
			l_pSplitterFrame = dynamic_cast<SVIPSplitterFrame *>( l_pcWnd );
		}
	}
	while (nullptr == l_pSplitterFrame && nullptr != l_pcWnd);

	if (nullptr != l_pSplitterFrame)
	{
		l_pSplitterFrame->SetViewSize( this, p_rcSize );
	}
}

void SVImageViewScroll::SetImageSize( SIZE p_oSize )
{
	int l_iMapMode;
	SIZE l_oSizeTotal;
	SIZE l_oSizePage;
	SIZE l_oSizeLine;

	CRect l_oWindowRect;
	CRect l_oClientRect;

	CPoint l_oPoint( 0, 0 );

	GetDeviceScrollSizes( l_iMapMode, l_oSizeTotal, l_oSizePage, l_oSizeLine );

	if( l_iMapMode == MM_TEXT )
	{
		l_oPoint = GetScrollPosition();
	}

	SetScrollSizes( MM_TEXT, p_oSize );

	m_SizeTotal = p_oSize;

	ScrollToPosition( CPoint( 0, 0 ) );

	if (nullptr != m_pView)
	{
		CRect l_oRect( CPoint( 0, 0 ), p_oSize );

		m_pView->MoveWindow( l_oRect );
	}

	GetClientRect( l_oClientRect );
	GetWindowRect( l_oWindowRect );

	double l_dX = l_oPoint.x;
	double l_dY = l_oPoint.y;

	if( l_oWindowRect.Size().cy == l_oClientRect.Size().cy )
	{
		l_dX = 0.0;

		if( l_oWindowRect.Size().cx <= l_oSizeTotal.cx )
		{
			m_oOldScrollPoint.x = l_oPoint.x;

			m_oOldMaxScrollPoint.x = l_oSizeTotal.cx - l_oWindowRect.Size().cx;
		}
	}
	else
	{
		if( l_oSizeTotal.cx < l_oWindowRect.Size().cx )
		{
			l_dX = static_cast< double >( m_oOldScrollPoint.x );
		}

		if( m_oOldMaxScrollPoint.x != p_oSize.cx - l_oWindowRect.Size().cx )
		{
			if( 0 < m_oOldMaxScrollPoint.x )
			{
				l_dX = l_dX * static_cast< double >(p_oSize.cx - l_oWindowRect.Size().cx) / static_cast< double >( m_oOldMaxScrollPoint.x );
			}

			m_oOldMaxScrollPoint.x = p_oSize.cx - l_oWindowRect.Size().cx;
		}

		m_oOldScrollPoint.x = static_cast< long >( l_dX );
	}

	if( l_oWindowRect.Size().cx == l_oClientRect.Size().cx )
	{
		l_dY = 0;

		if( l_oWindowRect.Size().cy <= l_oSizeTotal.cy )
		{
			m_oOldScrollPoint.y = l_oPoint.y;

			m_oOldMaxScrollPoint.y = l_oSizeTotal.cy - l_oWindowRect.Size().cy;
		}
	}
	else
	{
		if( l_oSizeTotal.cy < l_oWindowRect.Size().cy )
		{
			l_dY = static_cast< double >( m_oOldScrollPoint.y );
		}

		if( m_oOldMaxScrollPoint.y != p_oSize.cy - l_oWindowRect.Size().cy )
		{
			if( 0 < m_oOldMaxScrollPoint.y )
			{
				l_dY = l_dY * static_cast< double >(p_oSize.cy - l_oWindowRect.Size().cy) / static_cast< double >( m_oOldMaxScrollPoint.y );
			}

			m_oOldMaxScrollPoint.y = p_oSize.cy - l_oWindowRect.Size().cy;
		}

		m_oOldScrollPoint.y = static_cast< long >( l_dY );
	}

	l_oPoint.x = static_cast< long >( l_dX );
	l_oPoint.y = static_cast< long >( l_dY );

	ScrollToPosition( l_oPoint );
}

void SVImageViewScroll::GetParameters(SvOi::IObjectWriter& rWriter)
{
	CRect l_WindowRect;
	CSize l_ScrollSize = GetTotalSize();
	CPoint l_ScrollPoint = GetScrollPosition();

	GetWindowRect( l_WindowRect );

	_variant_t svVariant;

	rWriter.StartElement(SvXml::CTAG_IMAGE_VIEW);
	m_pView->GetParameters(rWriter);
	rWriter.EndElement();

	rWriter.StartElement(SvXml::CTAG_SCROLL_SIZE);
	svVariant = l_ScrollSize.cx;
	rWriter.WriteAttribute(SvXml::CTAG_CX, svVariant);
	svVariant.Clear();

	svVariant = l_ScrollSize.cy;
	rWriter.WriteAttribute(SvXml::CTAG_CY, svVariant);
	svVariant.Clear();

	rWriter.EndElement();

	rWriter.StartElement(SvXml::CTAG_SCROLL_POINT);
	svVariant = l_ScrollPoint.x;
	rWriter.WriteAttribute(SvXml::CTAG_X, svVariant);
	svVariant.Clear();

	svVariant = l_ScrollPoint.y;
	rWriter.WriteAttribute(SvXml::CTAG_Y, svVariant);
	svVariant.Clear();

	rWriter.EndElement();

	svVariant = l_WindowRect.Height();
	rWriter.WriteAttribute(SvXml::CTAG_CELL_HEIGHT, svVariant);
	svVariant.Clear();

	svVariant = l_WindowRect.Width();
	rWriter.WriteAttribute(SvXml::CTAG_CELL_WIDTH, svVariant);
	svVariant.Clear();
}

bool SVImageViewScroll::SetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent )
{
	bool bOk = false;

	_variant_t svVariant;

	SVTreeType::SVBranchHandle htiData = nullptr;

	CSize l_ViewSize;
	CSize l_ScrollSize;
	CPoint l_ScrollPoint;

	bOk = SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_IMAGE_VIEW, htiParent, htiData );
	if ( bOk )
	{
		bOk = m_pView->SetParameters( rTree, htiData );
	}

	if ( bOk )
	{
		bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_CELL_HEIGHT, htiParent, svVariant );
		if ( bOk )
		{
			l_ViewSize.cy = svVariant;
		}
	}

	if ( bOk )
	{
		bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_CELL_WIDTH, htiParent, svVariant );
		if ( bOk )
		{
			l_ViewSize.cx = svVariant;
		}
	}

	if ( bOk )
	{
		SetViewSize( l_ViewSize );
	}

	CRect l_WindowRect;

	GetWindowRect( l_WindowRect );

	if ( bOk )
	{
		bOk = SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_SCROLL_SIZE, htiParent, htiData );
		if ( bOk )
		{
			bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_CX, htiData, svVariant );
			if ( bOk )
			{
				l_ScrollSize.cx = svVariant;
			}

			if ( bOk )
			{
				bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_CY, htiData, svVariant );
				if ( bOk )
				{
					l_ScrollSize.cy = svVariant;
				}
			}

			if( bOk )
			{
				SetImageSize( l_ScrollSize );
			}
		}
	}

	if ( bOk )
	{
		bOk = SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_SCROLL_POINT, htiParent, htiData );
		if ( bOk )
		{
			bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_X, htiData, svVariant );
			if ( bOk )
			{
				l_ScrollPoint.x = svVariant;
			}

			if ( bOk )
			{
				bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_Y, htiData, svVariant );
				if ( bOk )
				{
					l_ScrollPoint.y = svVariant;
				}
			}
		}

		if ( bOk )
		{
			ScrollToPosition( l_ScrollPoint );
			UpdateZoomToolbar();
		}
	}

	return bOk;
}

bool SVImageViewScroll::CheckParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent )
{
	bool bOk = false;

	_variant_t svVariant;

	SVTreeType::SVBranchHandle htiData = nullptr;

	CSize l_ViewSize;
	CSize l_ScrollSize;
	CPoint l_ScrollPoint;

	CRect l_CheckWindowRect;
	CSize l_CheckScrollSize = GetTotalSize();
	CPoint l_CheckScrollPoint =	GetScrollPosition();

	GetWindowRect( l_CheckWindowRect );

	bOk = SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_IMAGE_VIEW, htiParent, htiData );
	if ( bOk )
	{
		bOk = m_pView->SetParameters( rTree, htiData );
	}

	if ( bOk )
	{
		bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_CELL_HEIGHT, htiParent, svVariant );
		if ( bOk )
		{
			l_ViewSize.cy = svVariant;
		}
	}

	if ( bOk )
	{
		bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_CELL_WIDTH, htiParent, svVariant );
		if ( bOk )
		{
			l_ViewSize.cx = svVariant;
		}
	}

	if ( bOk )
	{
		if( l_ViewSize.cx != l_CheckWindowRect.Width() ||
			l_ViewSize.cy != l_CheckWindowRect.Height() )
		{
			SetViewSize( l_ViewSize );

			GetWindowRect( l_CheckWindowRect );
		}
	}

	if ( bOk )
	{
		bOk = SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_SCROLL_SIZE, htiParent, htiData );
		if ( bOk )
		{
			bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_CX, htiData, svVariant );
			if ( bOk )
			{
				l_ScrollSize.cx = svVariant;
			}

			if ( bOk )
			{
				bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_CY, htiData, svVariant );
				if ( bOk )
				{
					l_ScrollSize.cy = svVariant;
				}
			}

			if( bOk )
			{
				if( l_ScrollSize != l_CheckScrollSize )
				{
					SetImageSize( l_ScrollSize );
				}
			}
		}
	}

	if ( bOk )
	{
		bOk = SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_SCROLL_POINT, htiParent, htiData );
		if ( bOk )
		{
			bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_X, htiData, svVariant );
			if ( bOk )
			{
				l_ScrollPoint.x = svVariant;
			}

			if ( bOk )
			{
				bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_Y, htiData, svVariant );
				if ( bOk )
				{
					l_ScrollPoint.y = svVariant;
				}
			}
		}

		if ( bOk )
		{
			if( l_ScrollPoint != l_CheckScrollPoint )
			{
				ScrollToPosition( l_ScrollPoint );
			}
		}
	}

	return bOk;
}

bool SVImageViewScroll::IsZoomAllowed() const
{
	bool Result{false};

	if (nullptr != m_pView)
	{
		Result = m_pView->IsZoomAllowed();
	}
	return Result;
}

void SVImageViewScroll::OnZoomTypeChanged(UINT nId)
{
	if(ID_ZOOM_SMALLEST <= nId && ID_ZOOM_FIT_HEIGHT >= nId)
	{
		ZoomEnum ZoomType = static_cast<ZoomEnum> (nId - ID_ZOOM_SMALLEST);
		if(nullptr != m_pView)
		{
			m_pView->SetZoom(ZoomType);
		}
	}
}

void SVImageViewScroll::OnUpdateZoomTypeChanged(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(IsZoomAllowed());
	//For the Zoom Toolbar check once, when ID_ZOOM_NORMAL is checked to set the slider
	if(ID_ZOOM_NORMAL == pCmdUI->m_nID)
	{
		SVMainFrame* pFrame = dynamic_cast<SVMainFrame*>(AfxGetMainWnd());
		if(nullptr != pFrame)
		{
			pFrame->EnableZoomToolbar(IsZoomAllowed());
		}
	}
}

void SVImageViewScroll::OnZoomSliderMoved()
{
	SVMainFrame* pFrame = dynamic_cast<SVMainFrame*>( AfxGetMainWnd() );
	double Value = 1.0;

	if(nullptr != pFrame)
	{
		Value = pFrame->GetZoomToolbarValue();
	}

	if(nullptr != m_pView)
	{
		m_pView->SetZoomValue(Value);
	}
}

void SVImageViewScroll::OnKillFocus(CWnd* pNewWnd)
{
	CScrollView::OnKillFocus(pNewWnd);
	SVIPSplitterFrame* pSplitterFrame = nullptr;
	CMDIFrameWnd* pMDIFrame = dynamic_cast< CMDIFrameWnd*>(AfxGetApp()->m_pMainWnd);

	if(nullptr != pMDIFrame)
	{
		pSplitterFrame = dynamic_cast< SVIPSplitterFrame* >(pMDIFrame->GetActiveFrame());
	}

	if(nullptr != pSplitterFrame)
	{
		pSplitterFrame->RefreshAllSplitters();
	}
}

void SVImageViewScroll::UpdateZoomToolbar()
{
	//only when we have the focus 
	SVImageViewScroll* pFocus = dynamic_cast<SVImageViewScroll*>(GetFocus());

	SVMainFrame* pFrame = dynamic_cast<SVMainFrame*>( AfxGetMainWnd() );

	if(nullptr != pFrame && pFocus == this )
	{
		if( IsZoomAllowed() && nullptr != m_pView )
		{
			pFrame->SetZoomToolbar(m_pView->GetZoomHelper());
		}
	}
}

void SVImageViewScroll::OnSetFocus(CWnd* pOldWnd)
{
	CScrollView::OnSetFocus(pOldWnd);
	SVMainFrame* pFrame = dynamic_cast<SVMainFrame*>( AfxGetMainWnd() );

	if(nullptr != pFrame)
	{
		SVIPSplitterFrame* pSplitterFrame = dynamic_cast< SVIPSplitterFrame* >(pFrame->GetActiveFrame());

		if( nullptr != pSplitterFrame )
		{
			pSplitterFrame->RefreshAllSplitters();
		}

		if( IsZoomAllowed() && nullptr != m_pView )
		{
			pFrame->SetZoomToolbar(m_pView->GetZoomHelper());
		}
	}
}
