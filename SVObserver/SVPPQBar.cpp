//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPPQBar
//* .File Name       : $Workfile:   SVPPQBar.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:32:54  $
//******************************************************************************

#include "stdafx.h"
#include "SVPPQBar.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVObserver.h"
#include "SVConfigurationObject.h"
#include "SVMessage/SVMessage.h"
#include "SVPPQEntryDialog.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SV_PPQ_BUTTON_WIDTH 30

IMPLEMENT_DYNCREATE( SVPPQBarClass, SVWindowBarClass )

SVPPQBarClass::SVPPQBarClass()
: SVWindowBarClass()
{
}

SVPPQBarClass::~SVPPQBarClass()
{
}

CSize SVPPQBarClass::CalcFixedLayout( BOOL , BOOL  )
{
	return SVWindowBarClass::CalcFixedLayout( TRUE, TRUE );
}

BEGIN_MESSAGE_MAP(SVPPQBarClass, SVWindowBarClass)
	//{{AFX_MSG_MAP(SVPPQBarClass)
	ON_NOTIFY(TCN_SELCHANGE, IDC_PQ_TAB, OnSelChangePQTab)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_SIZEPARENT, OnSizeParent)
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_SIZE()
	ON_WM_SIZING()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten SVPPQBarClass 

BOOL SVPPQBarClass::Create( CWnd* pParentWnd, DWORD dwStyle, UINT nID ) 
{
	assert(nullptr != pParentWnd);
	ASSERT_KINDOF(CFrameWnd, pParentWnd);

	windowTitle = _T( "PPQ Bar" );

	windowSize = CSize( 640, 120 );

	return SVWindowBarClass::Create( pParentWnd, dwStyle, nID );
}

void SVPPQBarClass::OnSize(UINT nType, int cx, int cy) 
{
	SVWindowBarClass::OnSize(nType, cx, cy);
}

int SVPPQBarClass::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (SVWindowBarClass::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect;
	GetClientRect( &rect );
	CalcInsideRect( rect, TRUE );

	// Create Tab Control...
	pqTabCtrl.Create( WS_CHILD | WS_VISIBLE, rect, this, IDC_PQ_TAB );
	pqTabCtrl.AdjustRect( FALSE, &rect );

	// Create Button Window...
	PPQWindow.CreateEx( WS_EX_STATICEDGE, nullptr, _T( "PPQ Entry Settings" ), WS_CHILD | WS_BORDER | WS_GROUP | WS_VISIBLE | WS_HSCROLL, rect, this, 0 );

	return 0;
}

void SVPPQBarClass::OnDestroy() 
{
	SVWindowBarClass::OnDestroy();
}

void SVPPQBarClass::OnSelChangePQTab( NMHDR*, LRESULT* pResult ) 
{
	int index = pqTabCtrl.GetCurSel();
	if( index >= 0 )
	{
		TC_ITEM tabCtrlItem;
		tabCtrlItem.mask = TCIF_PARAM;
		if( pqTabCtrl.GetItem( index, &tabCtrlItem ) )
		{
			CRect rect;
			GetClientRect( &rect );
			CalcInsideRect( rect, TRUE );

			// Size Tab Control...
			if( ::IsWindow( pqTabCtrl.m_hWnd ) )
			{
				pqTabCtrl.MoveWindow( &rect );
				pqTabCtrl.AdjustRect( FALSE, &rect );
			}

			// Size Button Window...
			if( ::IsWindow( PPQWindow.m_hWnd ) )
			{
				PPQWindow.MoveWindow( &rect );
				PPQWindow.ShowWindow( SW_SHOW );
				PPQWindow.BringWindowToTop();
			}// end if

			PPQWindow.BuildButtons( reinterpret_cast<SVPPQObject*> (tabCtrlItem.lParam) );

		}// end if
	
	}// end if

	*pResult = 0;
}// end OnSelChangePQTab


BOOL SVPPQBarClass::BuildButtons() 
{
	// Rebuild all PQ Tabs...
	int tabIndex = 0;

	pqTabCtrl.DeleteAllItems();

	SVPPQObject* pPPQ( nullptr );
	SVPPQObject* pPPQ1( nullptr );
	long lCount = 0;
	long lPPQ = 0;

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	//If the pointer is a nullptr then the count will be 0
	if( nullptr != pConfig){ lCount = pConfig->GetPPQCount( ); }
	for( lPPQ = 0; lPPQ < lCount; lPPQ++ )
	{
		pPPQ = pConfig->GetPPQ( lPPQ );

		if( lPPQ == 0 )
		{
			pPPQ1 = pPPQ;
		}

		if( nullptr != pPPQ )
		{
			TC_ITEM tabCtrlItem;
			tabCtrlItem.mask = TCIF_TEXT | TCIF_PARAM;
			//Use GetName as the pointer is longer valid while GetCompleteName returns a std::string which is only valid on return!
			tabCtrlItem.pszText = const_cast<LPSTR> (pPPQ->GetName());
			tabCtrlItem.lParam	= reinterpret_cast<DWORD_PTR>(pPPQ);
			pqTabCtrl.InsertItem( tabIndex++, &tabCtrlItem );
		}// end if

	}// end for

	CRect rect;
	GetClientRect( &rect );
	CalcInsideRect( rect, TRUE );
	
	// Size Tab Control...
	if( ::IsWindow( pqTabCtrl.m_hWnd ) )
	{
		pqTabCtrl.MoveWindow( &rect );
		pqTabCtrl.AdjustRect( FALSE, &rect );
	}
	
	// Size Button Window...
	if( ::IsWindow( PPQWindow.m_hWnd ) )
	{
		PPQWindow.MoveWindow( &rect );
		PPQWindow.ShowWindow( SW_SHOW );
		PPQWindow.BringWindowToTop();
	}

	return PPQWindow.BuildButtons( pPPQ1 );

}// end BuildButtons

BOOL SVPPQBarClass::DestroyButtons() 
{
	return PPQWindow.DestroyButtons();
}

void SVPPQBarClass::OnWindowPosChanged( WINDOWPOS FAR* lpwndpos ) 
{
	SVWindowBarClass::OnWindowPosChanged(lpwndpos);

	CRect rect;
	GetClientRect( &rect );
	CalcInsideRect( rect, TRUE );

	// Size Tab Control...
	if( ::IsWindow( pqTabCtrl.m_hWnd ) )
	{
		pqTabCtrl.MoveWindow( &rect );
		pqTabCtrl.AdjustRect( FALSE, &rect );
	}

	// Size Buttons...
	if( ::IsWindow( PPQWindow.m_hWnd ) )
	{
		PPQWindow.MoveWindow( &rect );
		PPQWindow.ShowWindow( SW_SHOW );
	}
}// end OnWindowPosChanged

void SVPPQBarClass::OnSizing(UINT fwSide, LPRECT pRect) 
{
	SVWindowBarClass::OnSizing(fwSide, pRect);
}

LRESULT SVPPQBarClass::OnSizeParent( WPARAM, LPARAM lParam )
{
	AFX_SIZEPARENTPARAMS *lpLayout = (AFX_SIZEPARENTPARAMS*) lParam;
	DWORD dwStyle = RecalcDelayShow( lpLayout );

	if( ( dwStyle & WS_VISIBLE ) && ( dwStyle & CBRS_ALIGN_ANY ) != 0 )
	{
		// align the control bar
		CRect rect;
		rect.CopyRect( &lpLayout->rect );

		CSize sizeAvail = rect.Size();  // maximum size available

		// get maximum requested size
		DWORD dwMode = lpLayout->bStretch ? LM_STRETCH : 0;
		if( ( m_dwStyle & CBRS_SIZE_DYNAMIC ) && ( m_dwStyle & CBRS_FLOATING ) )
			dwMode |= LM_HORZ | LM_MRUWIDTH;
		else 
			if( dwStyle & CBRS_ORIENT_HORZ )
				dwMode |= LM_HORZ | LM_HORZDOCK;
			else
				dwMode |=  LM_VERTDOCK;

		CSize size = CalcDynamicLayout( -1, dwMode );

		size.cx = std::min( size.cx, sizeAvail.cx );
		size.cy = std::min( size.cy, sizeAvail.cy );

		if( dwStyle & CBRS_ORIENT_HORZ )
		{
			lpLayout->sizeTotal.cy += size.cy;
			lpLayout->sizeTotal.cx = std::max(lpLayout->sizeTotal.cx, size.cx);
			if( dwStyle & CBRS_ALIGN_TOP )
				lpLayout->rect.top += size.cy;
			else 
				if( dwStyle & CBRS_ALIGN_BOTTOM )
				{
					rect.top = rect.bottom - size.cy;
					lpLayout->rect.bottom -= size.cy;
				}
		}
		else 
			if( dwStyle & CBRS_ORIENT_VERT )
			{
				lpLayout->sizeTotal.cx += size.cx;
				lpLayout->sizeTotal.cy = std::max( lpLayout->sizeTotal.cy, size.cy );
				if( dwStyle & CBRS_ALIGN_LEFT )
					lpLayout->rect.left += size.cx;
				else 
					if( dwStyle & CBRS_ALIGN_RIGHT )
					{
						rect.left = rect.right - size.cx;
						lpLayout->rect.right -= size.cx;
					}
			}
			else
			{
				assert( FALSE );      // can never happen
			}

		rect.right = rect.left + size.cx;
		rect.bottom = rect.top + size.cy;

		// only resize the window if doing layout and not just rect query
		if( nullptr != lpLayout->hDWP )
		{
			AfxRepositionWindow( lpLayout, m_hWnd, &rect );
		}
	}
	return 0;
}


SVPPQWindowClass::SVPPQWindowClass()
{
}

SVPPQWindowClass::~SVPPQWindowClass()
{
	if( buttonList.GetSize() > 0 )
		for( int i = 0; i < buttonList.GetSize(); ++i )
			delete( buttonList.GetAt( i ) );
}

BEGIN_MESSAGE_MAP(SVPPQWindowClass, CWnd)
	//{{AFX_MSG_MAP(SVPPQWindowClass)
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten SVPPQWindowClass 

BOOL SVPPQWindowClass::BuildButtons( SVPPQObject* pSelectedPPQ ) 
{
	DestroyButtons();

	m_pPPQ = pSelectedPPQ;

	if( nullptr == m_pPPQ )
	{
		return FALSE;
	}
	
	int dimension = SV_PPQ_BUTTON_WIDTH;
	if( ::IsWindow( m_hWnd ) )
	{
		CRect clientRect;
		GetClientRect( &clientRect );
		dimension = __max( dimension, clientRect.Height() );
		
		long lPPQLength = m_pPPQ->getPPQLength();
		
		// Calculate Scroll range
		int w = lPPQLength * dimension;
		
		SCROLLINFO scrollInfo;
		
		scrollInfo.cbSize = sizeof(SCROLLINFO);
		scrollInfo.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
		scrollInfo.nPage = clientRect.Width();
		scrollInfo.nPos = 0;
		scrollInfo.nTrackPos = 0;
		scrollInfo.nMin = 0;
		scrollInfo.nMax = w;
		SetScrollInfo(SB_HORZ,&scrollInfo);
		
		CRect rect( 0, 0, dimension, dimension );
		for( int i = 0; i < lPPQLength + 1; ++ i )
		{
			CButton* pButton = new CButton();
			std::string Caption = SvUl::Format( _T("%d"), i + 1 );
			DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP;
			buttonList.Add( pButton );
			if( i != lPPQLength )
			{
				pButton->Create( Caption.c_str(), dwStyle, rect, this, i + 50 );
			}
			
			else
			{
				CRect rectX( rect.left, rect.top, rect.left + 2000, rect.bottom );
				pButton->Create( nullptr, dwStyle | WS_DISABLED, rectX, this, i + 50 );
			}
			
			rect.left += dimension;
			rect.right += dimension;
			
		}// end for
		
		SvIe::SVVirtualCameraPtrVector cameraVector = m_pPPQ->GetVirtualCameras();
		long lPosition{0L};

		for(const auto* const pCamera : cameraVector)
		{
			if( nullptr != pCamera )
			{
				m_pPPQ->GetCameraPPQPosition( lPosition, pCamera );

				if( lPosition >= 0 && lPosition < buttonList.GetSize() && buttonList.GetAt( lPosition ) )
				{
					std::string Caption = SvUl::Format( _T("*%d*"),lPosition + 1 );
					buttonList.GetAt( lPosition )->SetWindowText( Caption.c_str() );
				}// end if
			}
		}// end for
		
		// Check if DDE Inputs are applied...
		for( int j = 0; j < 0; ++ j )
		{
			int pos = lPosition;
			if( pos >= 0 && pos < buttonList.GetSize() && buttonList.GetAt( pos ) )
			{
				std::string Caption = SvUl::Format( _T("*%d*"), pos + 1 );
				buttonList.GetAt( pos )->SetWindowText( Caption.c_str() );
			}// end if

		}// end for
		
		return TRUE;
		
	}// end if
	
	return FALSE;
	
}// end BuildButtons


BOOL SVPPQWindowClass::DestroyButtons() 
{
	// It will return TRUE, if there is any button destroyed.
	// Otherwise, it will return FALSE.
	if( buttonList.GetSize() > 0 )
	{
		for( int i = 0; i < buttonList.GetSize(); ++i )
			if( buttonList.GetAt( i ) )
			{
				buttonList.GetAt( i )->SendMessage( WM_DESTROY );
				delete( buttonList.GetAt( i ) );
			}
		buttonList.RemoveAll();
		buttonList.FreeExtra();
		
		return TRUE;
	}
	return FALSE;
}// end DestroyButtons


int SVPPQWindowClass::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	if( CWnd::OnCreate( lpCreateStruct ) == -1 )
		return -1;

	return 0;
}

BOOL SVPPQWindowClass::OnCmdMsg( UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo ) 
{
	// Click on PPQEntry button 
	if( S_OK == TheSVObserverApp.m_svSecurityMgr.SVValidate( SECURITY_POINT_VIEW_MENU_PPQ_BAR ) )
	{
		if ( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) )
		{
			SVSVIMStateClass::AddState( SV_STATE_EDITING );

			int pos = nID - 50;
	
			SVPPQEntryDialogPropertySheetClass dlg( _T( "PPQ Entry Dialog" ) );
			dlg.m_pPPQ				= m_pPPQ;
			dlg.m_lCurrentPosition	= pos;
			//remove Apply button
			dlg.m_psh.dwFlags |= PSH_NOAPPLYNOW;

            if ( dlg.DoModal() == IDOK )
            {
				SVConfigurationObject* pConfig( nullptr );

				SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

				if( nullptr != pConfig )
				{
					pConfig->RebuildInputOutputLists();
				}

				TheSVObserverApp.RefreshAllIPDocuments();

				if( buttonList.GetSize() > pos && buttonList.GetAt( pos ) )
			    {
				    if( dlg.m_bIsTaken )
				    {
						std::string Caption = SvUl::Format( _T("*%d*"), pos + 1 );
					    buttonList.GetAt( pos )->SetWindowText( Caption.c_str() );

					    HICON hIc = AfxGetApp()->LoadIcon( IDI_ICON_CAMERA );
					    if( hIc )
					    {
						    buttonList.GetAt( pos )->SetIcon( hIc );
						    buttonList.GetAt( pos )->SetButtonStyle( BS_ICON );
					    }
				    }
				    else
				    {
					    CString strCaption;
					    strCaption.Format( "%d", pos + 1 );
					    buttonList.GetAt( pos )->SetWindowText( strCaption );
					    buttonList.GetAt( pos )->SetButtonStyle( BS_PUSHBUTTON );
				    }
			    }// end if( buttonList.GetSize() > pos && buttonList.GetAt( pos ) )
            }// end if (iResult == IDOK)
			SVSVIMStateClass::RemoveState( SV_STATE_EDITING );
		}
		else
		{
			// Access during Test or Run Mode denied!
			std::string Msg  = SvUl::LoadStdString( IDS_USER_INFORMATION_NO_ACCESS_DURING_TEST_RUN_MODE );
			TheSVObserverApp.SetStatusText( Msg.c_str() );
			MessageBeep( 0xffffffff );
		}
	}
	return CWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}// end OnCmdMsg


void SVPPQWindowClass::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int minPos,
		maxPos,
		curPos = GetScrollPos( SB_HORZ );

	int lineStep = SV_PPQ_BUTTON_WIDTH;
	CRect clientRect;
	GetClientRect( &clientRect );
	lineStep = __max( lineStep, clientRect.Height() );

	GetScrollRange( SB_HORZ, &minPos, &maxPos );

	switch( nSBCode )
	{
		case SB_PAGERIGHT:
			if( curPos <= ( maxPos - clientRect.Width() ) )
				curPos += clientRect.Width();
			else
				curPos = maxPos;
			break;

		case SB_PAGELEFT:
			if( curPos >= ( minPos + clientRect.Width() ) )
				curPos -= clientRect.Width();
			else
				curPos = minPos;
			break;

		case SB_LINERIGHT:	
			if( curPos <= ( maxPos - lineStep ) )
				curPos += lineStep;
			else
				curPos = maxPos;
			break;

		case SB_LINELEFT:		
			if( curPos >= ( minPos + lineStep ) )
				curPos -= lineStep;
			else
				curPos = minPos;
			break;

		case SB_THUMBPOSITION:
			curPos = nPos;
			break;

		case SB_THUMBTRACK:
			curPos = nPos;
			break;

		case SB_RIGHT:
			curPos = maxPos;
			break;

		case SB_LEFT:
			curPos = minPos;
			break;
	};

	// Set New position and get previous position
	int prevPos = SetScrollPos( SB_HORZ, curPos );

	// get the updated position
	curPos = GetScrollPos( SB_HORZ );

	// Scoll the Window accordingly
	ScrollWindow( prevPos - curPos, 0 );

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void SVPPQWindowClass::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
}

BOOL SVPPQWindowClass::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

