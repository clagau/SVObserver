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
#include "SVObserver.h"
#include "SVConfigurationObject.h"
#include "SVMessage/SVMessage.h"
#include "SVPPQEntryDialog.h"
#include "SVIPDoc.h"
#include "SVSVIMStateClass.h"

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

CSize SVPPQBarClass::CalcFixedLayout( BOOL bStretch, BOOL bHorz )
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
	ASSERT(pParentWnd != NULL);
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
	PPQWindow.CreateEx( WS_EX_STATICEDGE, NULL, _T( "PPQ Entry Settings" ), WS_CHILD | WS_BORDER | WS_GROUP | WS_VISIBLE | WS_HSCROLL, rect, this, 0 );

	return 0;
}

void SVPPQBarClass::OnDestroy() 
{
	SVWindowBarClass::OnDestroy();
}

void SVPPQBarClass::OnSelChangePQTab( NMHDR* pNMHDR, LRESULT* pResult ) 
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

			PPQWindow.BuildButtons( (SVPPQObject*) tabCtrlItem.lParam );

		}// end if
	
	}// end if

	*pResult = 0;
}// end OnSelChangePQTab


BOOL SVPPQBarClass::BuildButtons() 
{
	// Rebuild all PQ Tabs...
	int tabIndex = 0;

	pqTabCtrl.DeleteAllItems();

	SVPPQObject *pPPQ;
	SVPPQObject *pPPQ1;
	long lCount;
	long lPPQ;

	SVConfigurationObject* pConfig = NULL;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	pConfig->GetPPQCount( lCount );
	for( lPPQ = 0; lPPQ < lCount; lPPQ++ )
	{
		pConfig->GetPPQ( lPPQ, &pPPQ );

		if( lPPQ == 0 )
			pPPQ1 = pPPQ;

		if( pPPQ )
		{
			TC_ITEM tabCtrlItem;
			CString l_Name = pPPQ->GetCompleteObjectName();
			tabCtrlItem.mask = TCIF_TEXT | TCIF_PARAM;
			tabCtrlItem.pszText = const_cast< LPSTR >( l_Name.GetString() );
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

		size.cx = min( size.cx, sizeAvail.cx );
		size.cy = min( size.cy, sizeAvail.cy );

		if( dwStyle & CBRS_ORIENT_HORZ )
		{
			lpLayout->sizeTotal.cy += size.cy;
			lpLayout->sizeTotal.cx = max(lpLayout->sizeTotal.cx, size.cx);
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
				lpLayout->sizeTotal.cy = max( lpLayout->sizeTotal.cy, size.cy );
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
				ASSERT( FALSE );      // can never happen
			}

		rect.right = rect.left + size.cx;
		rect.bottom = rect.top + size.cy;

		// only resize the window if doing layout and not just rect query
		if( lpLayout->hDWP != NULL )
			AfxRepositionWindow( lpLayout, m_hWnd, &rect );
	}
	return 0;
}


//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVPPQWindowClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

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

	if( !m_pPPQ )
		return FALSE;
	
	int dimension = SV_PPQ_BUTTON_WIDTH;
	if( ::IsWindow( m_hWnd ) )
	{
		CRect clientRect;
		GetClientRect( &clientRect );
		dimension = __max( dimension, clientRect.Height() );
		
		long lPPQLength;
		long lPosition;
		
		m_pPPQ->GetPPQLength( lPPQLength );
		
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
			CString strCaption;
			strCaption.Format( "%d", i + 1 );
			DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP;
			buttonList.Add( pButton );
			if( i != lPPQLength )
			{
				pButton->Create( strCaption, dwStyle, rect, this, i + 50 );
			}
			
			else
			{
				CRect rectX( rect.left, rect.top, rect.left + 2000, rect.bottom );
				pButton->Create( NULL, dwStyle | WS_DISABLED, rectX, this, i + 50 );
			}
			
			rect.left += dimension;
			rect.right += dimension;
			
		}// end for
		
		std::deque< SVVirtualCamera* > l_Cameras;

		m_pPPQ->GetCameraList( l_Cameras );

		std::deque< SVVirtualCamera* >::iterator l_Iter = l_Cameras.begin();

		while( l_Iter != l_Cameras.end() )
		{
			SVVirtualCamera* pCamera = ( *l_Iter );

			m_pPPQ->GetCameraPPQPosition( lPosition, pCamera );

			if( lPosition >= 0 && lPosition < buttonList.GetSize() && buttonList.GetAt( lPosition ) )
			{
				CString strCaption;
				strCaption.Format( "*%d*", lPosition + 1 );
				buttonList.GetAt( lPosition )->SetWindowText( strCaption );
			}// end if

			++l_Iter;
		}// end for
		
		// Check if DDE Inputs are applied...
		for( int j = 0; j < 0; ++ j )
		{
			int pos = lPosition;
			if( pos >= 0 && pos < buttonList.GetSize() && buttonList.GetAt( pos ) )
			{
				CString strCaption;
				strCaption.Format( "*%d*", pos + 1 );
				buttonList.GetAt( pos )->SetWindowText( strCaption );
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

	return 0; //( BuildButtons() ? 0 : -1 );
}

BOOL SVPPQWindowClass::OnCmdMsg( UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo ) 
{
	// Click on PPQEntry button 
	if( TheSVObserverApp.m_svSecurityMgr.SVValidate( SECURITY_POINT_VIEW_MENU_PPQ_BAR ) == S_OK ) // CheckCurrentUserAccess( ID_ADJUST_PPQ_ENTRY )  && nCode == BN_CLICKED )
	{
		if ( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) )
		{
			int pos = nID - 50;
	
			CString strCaption = _T( "PPQ Entry Dialog" );
			
			SVPPQEntryDialogPropertySheetClass dlg( strCaption );
			dlg.m_pPPQ				= m_pPPQ;
			dlg.m_lCurrentPosition	= pos;
			//remove Apply button
			dlg.m_psh.dwFlags |= PSH_NOAPPLYNOW;

            if ( dlg.DoModal() == IDOK )
            {
				SVConfigurationObject* l_pConfig = NULL;

				SVObjectManagerClass::Instance().GetConfigurationObject( l_pConfig );

				if( l_pConfig != NULL )
				{
					l_pConfig->RebuildInputOutputLists();
				}

				TheSVObserverApp.RefreshAllIPDocuments();

				if( buttonList.GetSize() > pos && buttonList.GetAt( pos ) )
			    {
				    if( dlg.m_bIsTaken )
				    {
					    CString strCaption;
					    strCaption.Format( "*%d*", pos + 1 );
					    buttonList.GetAt( pos )->SetWindowText( strCaption );

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
		}
		else
		{
			// Access during Test or Run Mode denied!
			CString message;
			message.LoadString( IDS_USER_INFORMATION_NO_ACCESS_DURING_TEST_RUN_MODE );
			TheSVObserverApp.SetStatusText( message );
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVPPQBar.cpp_v  $
 * 
 *    Rev 1.2   15 May 2014 12:32:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SetItemData to cast values to DWORD_PTR.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Oct 2013 07:01:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:30:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   09 Jul 2012 14:11:56   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to change the include for SVIM State.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   02 Jul 2012 17:30:16   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to promote new display functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   19 Jun 2012 14:04:02   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  744
   SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
   Centralized the location of the configuration object identifier and removed synchronization problems.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   16 Sep 2011 16:18:06   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  730
   SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Updated objects with new device and resource management functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   27 Jan 2011 11:50:36   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with Matrox Buffer Management.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   15 Dec 2010 10:36:00   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix connections and synchronization issues with the IO Sub-system.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   08 Dec 2010 13:26:00   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to include changes in notification functionality using the Observer Design Pattern.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   01 Jun 2010 11:03:42   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   16 Dec 2009 12:29:28   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fix issues with includes and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   09 Apr 2009 14:58:56   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  657
   SCR Title:  Remove Apply button from the Tool Adjust Dialogs
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     removed the apply button
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   14 Nov 2008 12:56:08   jbrown
   Project:  SVObserver
   Change Request (SCR) nbr:  641
   SCR Title:  BoundsChecker results
   Checked in by:  JimAdmin;  James A. Brown
   Change Description:  
     Replaced delete pszTabCaption with delete [] 
   ppszTabCaption within 
   SVPPQBar::BuildButtons ()
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   22 Aug 2005 10:27:04   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  504
   SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated methods to include changes to SVProductInfoStruct and supporting classes to handle proper assignment and copy construction.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   21 Jun 2005 13:07:38   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  455
   SCR Title:  New Security for SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Initial changes to add SVSecurity
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   25 Apr 2003 11:25:50   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  341
   SCR Title:  DDE Input and Outputs are no longer working correctly
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Fixed several problems so that the DDE inputs and outputs will work like they used to in pre SVO 4.00 versions.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   22 Apr 2003 13:52:18   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   06 Mar 2003 10:59:22   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  332
   SCR Title:  Module Ready not available after being unassigned and other small I/O bugs
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Forced the ResultView to update after removing inputs from the PPQBar. Also changed code to not go online if validating the inspection failed.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   20 Nov 2002 10:46:10   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  272
   SCR Title:  Outputs do not always get set correctly.
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Removed COM support
   Changed reference of SVPPQObjectClass to SVPPQObject
    Changed reference of SVDigitizerObjectClass to SVPPQObject
    Reworked to work with new architecture
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   10 May 2001 15:49:04   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  197
   SCR Title:  Fix lock-ups in application when loading configuration from SVFocusNT
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Changed SVObserver state code in:
   OnCmdMsg
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.1   06 Sep 2000 16:34:16   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  179
   SCR Title:  Create Color SVObserver
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated files to include color SVIM changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.0   23 Nov 1999 13:26:14   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  61
   SCR Title:  Update PVCS versioning to version 3.0.
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.4   08 Nov 1999 17:15:44   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  22
   SCR Title:  Port Multiple Position Queue code from program version 2.4.2
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised Setup and handling of Scrollbar
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.3   Sep 07 1999 09:08:08   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  12
   SCR Title:  Port OCR functionallity from custom 2.xx version.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Fixed memory leaks.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.2   Aug 23 1999 11:03:08   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  22
   SCR Title:  Port Multiple Position Queue code from program version 2.4.2
   Checked in by:  Steve Jones
   Change Description:  
     Revised for new PPQ architechture.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
