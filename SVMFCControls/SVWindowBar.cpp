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

namespace Seidenader
{
	namespace SVMFCControls
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
		// Behandlungsroutinen für Nachrichten SVWindowBarClass 

		void SVWindowBarClass::OnUpdateCmdUI( CFrameWnd* pTarget, BOOL bDisableIfNoHndler )
		{
			UpdateDialogControls(pTarget, bDisableIfNoHndler);
		}


		BOOL SVWindowBarClass::Create( LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext ) 
		{
			//SetBorders( 0, 3, 0, 3 );

			dwStyle |= CBRS_BORDER_ANY;

			// save the style
			m_dwStyle = ( dwStyle & CBRS_ALL );

			return CControlBar::Create( lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext );
		}

		BOOL SVWindowBarClass::Create( CWnd* pParentWnd, DWORD dwStyle, UINT nID ) 
		{
			ASSERT( pParentWnd != NULL );
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
				// AFX_IDW_DOCKBAR_TOP
				//AfxMessageBox( "h1" );
				borderRect.bottom = 6;
				borderRect.top	  = 1;
				SetBorders( &borderRect );
			}
			else
				if( ( m_dwStyle & CBRS_ALIGN_BOTTOM ) || ( m_dwStyle & CBRS_ALIGN_RIGHT ) )
				{
					//AfxMessageBox( "h2" );
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

			//	ASSERT(m_nCount == 0 || m_pData != NULL);
			//	ASSERT((m_dwStyle & CBRS_ALL) == m_dwStyle);
		}

		void SVWindowBarClass::Dump(CDumpContext& dc) const
		{
			CControlBar::Dump(dc);
			/*
			dc << "\nm_cxLeftBorder = " << m_cxLeftBorder;
			dc << "\nm_cxRightBorder = " << m_cxRightBorder;
			dc << "\nm_cyTopBorder = " << m_cyTopBorder;
			dc << "\nm_cyBottomBorder = " << m_cyBottomBorder;
			dc << "\nm_cxDefaultGap = " << m_cxDefaultGap;
			dc << "\nm_nCount = " << m_nCount;
			dc << "\nm_bAutoDelete = " << m_bAutoDelete;

			dc << "\n";
			*/
		}
#endif
	} //SVMFCControls
} //Seidenader

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVWindowBar.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 16:31:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.0.1.5   01 Jun 2010 11:13:02   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.0.1.4   30 Oct 2007 11:01:36   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  609
   SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Fixed HBRUSH GDI leak.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.0.1.3   22 Apr 2003 17:30:30   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
