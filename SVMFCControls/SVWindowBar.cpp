//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVWindowBar
//* .File Name       : $Workfile:   SVWindowBar.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 16:31:14  $
//******************************************************************************

#include "stdafx.h"
#include "SVWindowBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvMc
{
	IMPLEMENT_DYNCREATE( SVWindowBarClass, CControlBar )

		SVWindowBarClass::SVWindowBarClass()
		: CControlBar()
	{
		windowTitle = _T( "SVWindowBarClass" );
		windowSize  = CSize( 0, 0 );
		m_hWindowBackgroundColor = ::CreateSolidBrush( ::GetSysColor( COLOR_BTNHILIGHT ));
	}

	SVWindowBarClass::~SVWindowBarClass()
	{
		if( m_hWindowBackgroundColor )
		{
			DeleteObject( m_hWindowBackgroundColor );
		}
	}

	BEGIN_MESSAGE_MAP(SVWindowBarClass, CControlBar)
		//{{AFX_MSG_MAP(SVWindowBarClass)
		ON_WM_SIZING()
		ON_WM_PAINT()
		ON_WM_CREATE()
		ON_WM_WINDOWPOSCHANGED()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// Behandlungsroutinen f�r Nachrichten SVWindowBarClass 

	void SVWindowBarClass::OnUpdateCmdUI( CFrameWnd* pTarget, BOOL bDisableIfNoHndler )
	{
		UpdateDialogControls(pTarget, bDisableIfNoHndler);
	}

	BOOL SVWindowBarClass::Create( LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext ) 
	{
		dwStyle |= CBRS_BORDER_ANY;

		// save the style
		m_dwStyle = ( dwStyle & CBRS_ALL );

		return CControlBar::Create( lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext );
	}

	BOOL SVWindowBarClass::Create( CWnd* pParentWnd, DWORD dwStyle, UINT nID ) 
	{
		ASSERT( nullptr != pParentWnd );
		ASSERT_KINDOF( CFrameWnd, pParentWnd );

		CRect rect;
		rect.SetRectEmpty();

		dwStyle |= WS_BORDER | WS_GROUP | WS_CLIPSIBLINGS; // | WS_MAXIMIZE; // | WS_CAPTION | WS_VISIBLE | WS_OVERLAPPEDWINDOW;

		if( windowTitle == _T( "" ) )
			windowTitle = _T( "SVWindowBarClass" );

		LPCTSTR lpszClassName = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW, 
			AfxGetApp()->LoadStandardCursor( IDC_ARROW ),
			m_hWindowBackgroundColor );

		return Create( lpszClassName, windowTitle, dwStyle, rect, pParentWnd, nID );
	}


	CSize SVWindowBarClass::CalcFixedLayout( BOOL bStretch, BOOL bHorz )
	{
		CSize size = windowSize;
		int maxCX = 32767;
		int maxCY = 16000;

		CRect clientRect;
		CWnd* parent = AfxGetApp()->GetMainWnd();
		if( nullptr != parent )
		{
			parent->GetClientRect( &clientRect );

			maxCX = clientRect.Width();
			maxCY = clientRect.Height();
		}

		size.cx = ( bStretch && bHorz ? maxCX : windowSize.cx );
		size.cy = ( bStretch && !bHorz ? maxCY : windowSize.cy );

		return size;
	}

	CSize SVWindowBarClass::CalcDynamicLayout( int nLength, DWORD dwMode )
	{
		if( ( nLength == -1 ) && !( dwMode & LM_MRUWIDTH ) && !( dwMode & LM_COMMIT ) &&
			( ( dwMode & LM_HORZDOCK ) || ( dwMode & LM_VERTDOCK ) ) )
		{
			return CalcFixedLayout( ( dwMode & LM_STRETCH ), dwMode & LM_HORZ );
		}

		return windowSize;
	}


	void SVWindowBarClass::CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType) 
	{

		CControlBar::CalcWindowRect(lpClientRect, nAdjustType);
	}

	int SVWindowBarClass::OnCreate(LPCREATESTRUCT lpCreateStruct) 
	{
		if (CControlBar::OnCreate(lpCreateStruct) == -1)
			return -1;

		// Create slide bar...
		LPCTSTR lpszClassName = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW, 
			AfxGetApp()->LoadStandardCursor( IDC_ARROW ),
			m_hWindowBackgroundColor );

		CRect rect;
		rect.left   = 0;
		rect.top    = 0;
		rect.right  = 1;
		rect.bottom = 1;
		slidebar.Create( lpszClassName, _T( "SVSlideBar" ), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, rect, this, 0 );

		return 0;
	}

	void SVWindowBarClass::OnSizing(UINT fwSide, LPRECT pRect) 
	{
		CControlBar::OnSizing(fwSide, pRect);
		windowSize.cx = pRect->right - pRect->left;
		windowSize.cy = pRect->bottom - pRect->top;

		CMDIFrameWnd* parent = dynamic_cast<CMDIFrameWnd*>(AfxGetApp()->GetMainWnd());
		if( nullptr != parent )
			parent->RecalcLayout();
	}

	void SVWindowBarClass::OnPaint() 
	{
		CPaintDC dc(this); // device context for painting

		CRect borderRect = GetBorders();
		if( ( m_dwStyle & CBRS_ALIGN_TOP ) || ( m_dwStyle & CBRS_ALIGN_LEFT ) )
		{
			borderRect.bottom = 6;
			borderRect.top	  = 1;
			SetBorders( &borderRect );
		}
		else
			if( ( m_dwStyle & CBRS_ALIGN_BOTTOM ) || ( m_dwStyle & CBRS_ALIGN_RIGHT ) )
			{
				borderRect.bottom = 1;
				borderRect.top	  = 6;
				SetBorders( &borderRect );
			}


			DoPaint( &dc );

			// Do not call CControlBar::OnPaint() for painting messages
	}

	void SVWindowBarClass::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
	{
		CControlBar::OnWindowPosChanged(lpwndpos);

		// should only be called once, when floated.
		if( IsFloating() )
		{
			if( m_pDockBar && !m_bMenuRemoved )
			{
				CWnd* pParent = m_pDockBar->GetParent();
				if( pParent->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)))
				{
					pParent->ModifyStyle( WS_SYSMENU, 0, 0 );
					m_bMenuRemoved = TRUE;
				}
			}
		}
		else if( m_bMenuRemoved )
		{
			m_bMenuRemoved = FALSE;
		}

		if( ::IsWindow( slidebar.m_hWnd ) )
			slidebar.MoveWindow( 0, 0, 1, 1, TRUE );

	}



#ifdef _DEBUG
	void SVWindowBarClass::AssertValid() const
	{
		CControlBar::AssertValid();
	}

	void SVWindowBarClass::Dump(CDumpContext& dc) const
	{
		CControlBar::Dump(dc);
	}
#endif
} //namespace SvMc
