//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMainFrm
//* .File Name       : $Workfile:   SVMainFrm.CPP  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.17  $
//* .Check In Date   : $Date:   19 Dec 2014 04:14:46  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVMainFrm.h"
#include "SVSystemLibrary/SVMemoryChecker.h"
#include "SVSystemLibrary/SVVersionInfo.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVGlobal.h"
#include "SVIOController.h"
#include "SVIODoc.h"                 // 13 Jan 2000 - frb.
#include "SVIPDoc.h"
#include "SVObjectScriptParser.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObserver.h"
#include "SVParserProgressDialog.h"
#include "SVToolSet.h"               // 26 Jan 2000 - frb.
#include "SVSVIMStateClass.h"
#include "SVUserMessage.h"
#include "SVMessage/SVMessage.h"
#include "SVInspectionProcess.h"
#include "SVPPQObject.h"
#include "SVConfigurationObject.h"
#include "SVRegressionRunDlg.h"
#include "SVDirectX.h"
#include "ModelessMessageBox.h"
#include "SVXMLLibrary/SVNavigateTreeClass.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVObjectScriptParserSVX.h"	// new speedy
#include "SVInspectionTreeParser.h"		// new Inspection XML (NOT object script)
#include "RemoteCommand.h"
#include "EnvironmentObject.h"
#include "SVImageViewScroll.h"
#include "SVImageView.h"
#include "SVOLicenseManager/SVOLicenseManager.h"
#include "SVIPChildFrm.h" 
#include "AutoSaver.h"

#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVMainFrame
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

IMPLEMENT_DYNAMIC(SVMainFrame, CMDIFrameWnd)

//******************************************************************************
// Message Map Entries
//******************************************************************************
BEGIN_MESSAGE_MAP(SVMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(SVMainFrame)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CLOSE()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_VIEW_PPQ_BAR, OnViewPPQBar)
	ON_COMMAND(ID_TOOLBAR_STANDARD, OnViewToolBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PPQ_BAR, OnUpdateViewPPQBar)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_STANDARD, OnUpdateViewToolbar)
	ON_COMMAND(ID_VIEW_IMAGE_DISPLAY_UPDATE, OnViewImageDisplayUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_IMAGE_DISPLAY_UPDATE, OnUpdateViewImageDisplayUpdate)
	ON_COMMAND(ID_VIEW_RESULT_DISPLAY_UPDATE, OnViewResultDisplayUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESULT_DISPLAY_UPDATE, OnUpdateViewResultDisplayUpdate)
	ON_COMMAND(ID_EXTRAS_ENABLEAUTOSAVE, OnExtrasEnableAutoSave)
	ON_UPDATE_COMMAND_UI(ID_EXTRAS_ENABLEAUTOSAVE, OnUpdateExtrasEnableAutoSave)
	ON_WM_SIZE()
	ON_WM_SIZING()
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_INFO, OnUpdateStatusInfo)
	// Status Bar User information
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_USER, OnUpdateStatusInfo)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_MODE, OnUpdateStatusInfo)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_GIGE_LICENSE_ERROR, OnUpdateStatusInfo)
	//ON_UPDATE_COMMAND_UI(ID_INDICATOR_FREE_MEMORY, OnUpdateStatusInfo)
	// Globale Hilfebefehle
	ON_COMMAND(ID_HELP_FINDER, CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CMDIFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CMDIFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CMDIFrameWnd::OnHelpFinder)

	// Load and Run Last Configuration Automatically...
	//ON_MESSAGE(SV_AUTO_RUN_LAST_CONFIGURATION, OnAutoRunLastConfiguration)

	// Start Object Script Parser
	ON_MESSAGE(SV_PARSE_OBJECT_SCRIPT_END, OnEndObjectScriptParse)
	ON_MESSAGE(SV_PARSE_OBJECT_CREATE_DONE, OnEndObjectCreation)
	ON_MESSAGE(SV_LOGGED_ON_USER_CHANGED,OnUserChanged)
	ON_MESSAGE( SV_IOVIEW_HIDE_TAB, OnHideIOViewTab )
	ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChange)
	ON_WM_ACTIVATEAPP()
	ON_MESSAGE(SV_SET_MODE, OnSetMode)
	ON_MESSAGE(SV_LOAD_CONFIGURATION, OnLoadConfiguration)
	ON_MESSAGE(SV_REFRESH_STATUS_BAR, InvalidateStatusBar )
	ON_MESSAGE(SV_SHUTDOWN, Shutdown )
	ON_COMMAND(ID_TOOLBAR_ZOOM, &SVMainFrame::OnToolbarZoom)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_ZOOM, &SVMainFrame::OnUpdateToolbarZoom)
	ON_MESSAGE(SV_REGISTER_MONITOR_LIST, &SVMainFrame::RemoteRegisterMonitorList)

END_MESSAGE_MAP()

#pragma region Constructor
////////////////////////////////////////////////////////////////////////////////
// .Title       : // e.g. Standard constructor of class ...
// -----------------------------------------------------------------------------
// .Description : This constructor inits ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: // e.g. int&				MyInt				Index Handle 
//  :
// .Output(s)
//	: // e.g. int&				MyInt				Index Handle 
//  :
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
SVMainFrame::SVMainFrame()
{
	m_toolbarWasShown			= FALSE;
	m_PPQBarWasShown			= FALSE;
	m_toolbarZoomWasShown		= FALSE;

	m_notifyCommRC = FALSE;
	m_oemLargeIcon = NULL;
	m_oemSmallIcon = NULL;

	m_hDisplayChangedEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : // e.g. Standard destructor of class ...
// -----------------------------------------------------------------------------
// .Description : This destructor deletes ...
//              :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
SVMainFrame::~SVMainFrame()
{
	if( m_hDisplayChangedEvent != NULL )
	{
		::CloseHandle( m_hDisplayChangedEvent );
		m_hDisplayChangedEvent = NULL;
	}

	if( m_oemLargeIcon )
		::DestroyIcon( m_oemLargeIcon );
	
	if( m_oemSmallIcon )
		::DestroyIcon( m_oemSmallIcon );

	m_BackgroundBrush.DeleteObject();
}
#pragma endregion Constructor

////////////////////////////////////////////////////////////////////////////////
// Load OEM Icon
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::LoadOEMIcon()
{
	if( 0 == ::_access( _T("C:\\SVObserver\\Bin\\OEM\\oem32x32.ico"), 0 ) )
	{
		m_oemLargeIcon = (HICON)::LoadImage( NULL, 
										_T("C:\\SVObserver\\Bin\\OEM\\oem32x32.ico"),
										IMAGE_ICON,
										0,
										0,
										LR_DEFAULTCOLOR | LR_LOADFROMFILE );
		if( m_oemLargeIcon )
		{
			SetIcon( m_oemLargeIcon,TRUE );
		}

	}// end if

	if( 0 == ::_access( _T("C:\\SVObserver\\Bin\\OEM\\oem16x16.ico"), 0 ) )
	{
		m_oemSmallIcon = (HICON)::LoadImage( NULL, 
										_T("C:\\SVObserver\\Bin\\OEM\\oem16x16.ico"),
										IMAGE_ICON,
										0,
										0,
										LR_DEFAULTCOLOR | LR_LOADFROMFILE );
		if( m_oemSmallIcon )
		{
			SetIcon( m_oemSmallIcon,FALSE );
		}
	}// end if
}

//******************************************************************************
// Operator(s):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// Create Operator
////////////////////////////////////////////////////////////////////////////////

BOOL SVMainFrame::Create( LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext ) 
{
	return CWnd::Create( lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext );
}

////////////////////////////////////////////////////////////////////////////////
// Creating Operations(s)
////////////////////////////////////////////////////////////////////////////////
BOOL SVMainFrame::CreateClient(LPCREATESTRUCT lpCreateStruct, CMenu* pWindowMenu)
{
	ASSERT( m_hWnd != NULL );
	ASSERT( m_hWndMDIClient == NULL );
	DWORD dwStyle = WS_VISIBLE | WS_CHILD | WS_BORDER | 
		WS_CLIPCHILDREN | WS_CLIPSIBLINGS | 
		MDIS_ALLCHILDSTYLES;    // allow children to be created invisible
	DWORD dwExStyle = 0;
	// will be inset by the frame

	if( ( ( BYTE ) ::GetVersion() ) >= 4 )	
	// This is a Win4 application and that code will always be executed!
	// Only for backward compatibility, refer to MFC source code afxData.bWin4
	{
		// special styles for 3d effect on Win4
		dwStyle &= ~WS_BORDER;
		dwExStyle = WS_EX_CLIENTEDGE;
	}

	WNDCLASS	windowClass;
	if( !GetClassInfo( AfxGetInstanceHandle(), _T( "mdiclient" ), &windowClass ) )
		return FALSE; 

	// Use Körber Grey (RGB( 204, 204, 204 ) for the application's background.
	m_BackgroundBrush.CreateSolidBrush( RGB( 204, 204, 204 ) );
	windowClass.hbrBackground = m_BackgroundBrush.operator HBRUSH();
	windowClass.lpszClassName = _T( "SVObserverMDIClient" );

	if( !AfxRegisterClass( &windowClass ) )
		return( FALSE );

	CLIENTCREATESTRUCT ccs;
	ccs.hWindowMenu = pWindowMenu->GetSafeHmenu();
		// set hWindowMenu for MFC V1 backward compatibility
		// for MFC V2, window menu will be set in OnMDIActivate
	ccs.idFirstChild = AFX_IDM_FIRST_MDICHILD;

	if( lpCreateStruct->style &( WS_HSCROLL | WS_VSCROLL ) )
	{
		// parent MDIFrame's scroll styles move to the MDICLIENT
		dwStyle |=( lpCreateStruct->style &( WS_HSCROLL | WS_VSCROLL ) );

		// fast way to turn off the scrollbar bits( without a resize )
		ModifyStyle( WS_HSCROLL | WS_VSCROLL, 0, SWP_NOREDRAW | SWP_FRAMECHANGED );
	}

	// Create MDICLIENT control with special IDC
	if( ( m_hWndMDIClient = ::CreateWindowEx( dwExStyle,
											  _T(  "SVObserverMDIClient"  ), 
											  NULL,
											  dwStyle, 
											  0, 0, 0, 0,
											  m_hWnd,
											  ( HMENU )AFX_IDW_PANE_FIRST,
											  AfxGetInstanceHandle(),
											  ( LPVOID )&ccs ) ) == NULL )
	{
		return FALSE;
	}
	// Move it to the top of z-order
	::BringWindowToTop( m_hWndMDIClient );

	return TRUE;
};

//******************************************************************************
// Class Wizard Generated Virtual Function(s):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : // e.g. Run member function of class ...
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
BOOL SVMainFrame::PreCreateWindow( CREATESTRUCT& cs )
{
	return CMDIFrameWnd::PreCreateWindow( cs );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : setStatusInfoText
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::SetStatusInfoText( LPCTSTR SIText )
{
	size_t l_Size = ( SIText != NULL ) ? ::strlen( SIText ) : 0;

	if( 0 < l_Size )
	{
		CClientDC DC( NULL );
		int l_Width = DC.GetTextExtent( SIText, static_cast<int>(l_Size) ).cx - static_cast< int >( static_cast< double >( l_Size ) * 1.5 );

		m_wndStatusBar.SetPaneInfo( 1, ID_INDICATOR_INFO, 0, l_Width );
		m_wndStatusBar.SetPaneText( 1, SIText );
		m_wndStatusBar.Invalidate();
	}
	else
	{
		m_wndStatusBar.SetPaneInfo( 1, ID_INDICATOR_INFO, SBPS_NOBORDERS, 0 );
	}
}

void SVMainFrame::ShowToolBars(BOOL Show, BOOL RememberOldState /*= FALSE*/, DWORD Flags /*= Flag_Standard | Flag_Zoom */)
{
	if( TRUE == Show )
	{
		if( RememberOldState )
		{
			if(Flags & Flag_Standard)
			{
				ShowControlBar( &m_wndToolBar, m_toolbarWasShown, FALSE );
				ShowControlBar( &m_PPQBar, m_PPQBarWasShown, FALSE );
			}

			if(Flags & Flag_Zoom)
			{
				ShowControlBar( &m_wndToolBarZoom, m_toolbarZoomWasShown, FALSE );
				m_wndToolBarZoom.Enable(IsZoomingPossible());
			}
		}
		else
		{
			// Show all bars
			if(Flags & Flag_Standard)
			{
				ShowControlBar( &m_wndToolBar, TRUE, FALSE );
				ShowControlBar( &m_PPQBar, TRUE, FALSE );
			}
			if(Flags & Flag_Zoom)
			{
				ShowControlBar( &m_wndToolBarZoom, TRUE, FALSE );
				m_wndToolBarZoom.Enable(IsZoomingPossible());
			}
		}
	}
	else
	{
		if(Flags & Flag_Standard)
		{
			m_toolbarWasShown			= m_wndToolBar.IsWindowVisible();
			m_PPQBarWasShown			= m_PPQBar.IsWindowVisible();
			ShowControlBar( &m_wndToolBar, FALSE, FALSE );

			ShowControlBar( &m_PPQBar, FALSE, FALSE );
		}

		if(Flags & Flag_Zoom)
		{
			if(!SVSVIMStateClass::CheckState( SV_STATE_TEST  | SV_STATE_RUNNING))
			{
				m_toolbarZoomWasShown = m_wndToolBarZoom.IsWindowVisible();
			}
			ShowControlBar( &m_wndToolBarZoom, FALSE, FALSE );
		}

		SetStatusInfoText( _T(""));
	}

	RecalcLayout();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnUpdateFrameTitle
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::OnUpdateFrameTitle( BOOL bAddToTitle )
{
	if( ( GetStyle() & FWS_ADDTOTITLE ) == 0 )
		return;     // leave it alone!

	CMDIChildWnd* pActiveChild;
	CDocument* pDocument = GetActiveDocument();
	if ( bAddToTitle &&
		( pActiveChild = MDIGetActive() ) != NULL &&
		( pActiveChild->GetStyle() & WS_MAXIMIZE ) == 0 &&
		( pDocument != NULL || 
			( pDocument = pActiveChild->GetActiveDocument() ) != NULL ) )
	{
		UpdateFrameTitleForDocument( pDocument->GetTitle() );
	}
	else
	{
		UpdateFrameTitleForDocument( NULL );
	}
}

void SVMainFrame::UpdateFrameTitleForDocument( LPCTSTR lpszDocName )
{
	// Special for SVObserver
	// copy first part of title loaded at time of frame creation
	TCHAR szText[ 256 +_MAX_PATH ];

	CString l_Version = SeidenaderVision::SVVersionInfo::GetShortTitleVersion().c_str();

	if( GetStyle() & FWS_PREFIXTITLE )
	{
		szText[ 0 ] = '\0';   // start with nothing

		// get name of currently active view
		if( lpszDocName != NULL )
		{
			lstrcpy( szText, lpszDocName );
			// add current window # if needed
			if( m_nWindow > 0 )
				wsprintf( szText + lstrlen( szText ), _T( ":%d" ), m_nWindow );
			lstrcat( szText, _T( " - " ) );
		}
		lstrcat( szText, m_strTitle );
		lstrcat( szText, " " );
		lstrcat( szText, l_Version );
	}
	else
	{
		lstrcpy( szText, m_strTitle );
		lstrcat( szText, " " );
		lstrcat( szText, l_Version );

		// Get title of currently active SVC
		if( SVSVIMStateClass::CheckState( SV_STATE_READY | SV_STATE_RUNNING ) )
		{
			lstrcat( szText, _T( " - " ) );
			lstrcat( szText, TheSVObserverApp.getConfigFileName() );
		}

		// get name of currently active view
		if( lpszDocName != NULL )
		{
			lstrcat( szText, _T( " - " ) );
			lstrcat( szText, lpszDocName );
			// add current window # if needed
			if( m_nWindow > 0 )
				wsprintf( szText + lstrlen( szText ), _T( ":%d" ), m_nWindow );
		}
	}

	// set title if changed, but don't remove completely
	// Note: will be excessive for MDI Frame with maximized child
	int nNewLen = lstrlen( szText );
	TCHAR szOld[ 256 ];
	// fast check to see if text really changes (reduces flash in controls)
	if( nNewLen > 256 ||
		::GetWindowText( m_hWnd, szOld, 256) != nNewLen ||
		lstrcmp( szOld, szText ) != 0 )
	{
		// change it
		::SetWindowText( m_hWnd, szText );
	}
}

//******************************************************************************
// Message Operation(s):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : // e.g. Run member function of class ...
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
BOOL SVMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	return CMDIFrameWnd::OnCreateClient(lpcs, pContext);
	// This function call creates a legal CMenu pointer and calls the overriden
	// SVMainFrame::CreateClient function to perform the MDIClient window
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : // e.g. Run member function of class ...
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
int SVMainFrame::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if( CMDIFrameWnd::OnCreate( lpCreateStruct ) == -1 )
	{
		return -1;
	}

	if( !m_wndToolBar.Create( this ) ||
		!m_wndToolBar.LoadToolBar( IDR_MAINFRAME ) )
	{
		return -1;      // Fehler beim Erzeugen
	}

	if( !m_wndToolBarZoom.Create( this, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS, ID_ZOOMTOOLBAR ) )
	{
		return -1;      // Fehler beim Erzeugen
	}

	if( !m_wndStatusBar.Create( this ) ||
		!m_wndStatusBar.SetIndicators( indicators,
		  sizeof( indicators ) / sizeof( UINT ) ) )
	{
		return -1;      // Fehler beim Erzeugen
	}

	m_wndStatusBar.SetPaneInfo( 1, ID_INDICATOR_INFO, SBPS_NOBORDERS, 0 );

	if ( !m_PPQBar.Create( this ) )
	{
		return -1;      // Fehler beim Erzeugen
	}

	EnableDocking( CBRS_ALIGN_ANY );

	// Standard Toolbar
	m_wndToolBar.SetBarStyle( m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_GRIPPER | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );
	m_wndToolBar.EnableDocking( CBRS_ALIGN_ANY );

	m_wndToolBarZoom.EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM);
	m_wndToolBarZoom.SetWindowText(_T("Zoom Factor"));

	// PPQ Bar
	m_PPQBar.SetBarStyle( m_PPQBar.GetBarStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_GRIPPER | CBRS_FLYBY );
	m_PPQBar.EnableDocking( CBRS_ALIGN_BOTTOM | CBRS_ALIGN_TOP );

	DockControlBar( &m_wndToolBarZoom );
	DockControlBar( &m_wndToolBar );
	DockControlBar( &m_PPQBar, AFX_IDW_DOCKBAR_BOTTOM );

	// Remember old bar states...
	try
	{
		if ( VerifyBarStates(_T("Bar States")) )
		{
			LoadBarState(_T("Bar States"));
		}
	}
	catch (CException* e)
	{
		e->Delete();
		ASSERT(FALSE);
	}

	ShowControlBar( &m_wndStatusBar, TRUE, FALSE );
	ShowToolBars(TRUE, TRUE, Flag_Standard | Flag_Zoom);

	LoadOEMIcon();

	m_bSizeChanged = FALSE;
	m_PosSize = SIZE_MAXIMIZED;

	UpdateFrameTitleForDocument( NULL );

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : // e.g. Run member function of class ...
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CMDIFrameWnd::OnLButtonDown(nFlags, point);
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : // e.g. Run member function of class ...
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CMDIFrameWnd::OnRButtonDown(nFlags, point);
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : // e.g. Run member function of class ...
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::OnSetFocus(CWnd* pOldWnd) 
{
	CMDIFrameWnd::OnSetFocus(pOldWnd);
	
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : // e.g. Run member function of class ...
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen

	CMDIFrameWnd::OnLButtonDblClk(nFlags, point);
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : // e.g. Run member function of class ...
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::OnClose()
{
	bool l_bAllowAccess = false;

	if ( SVSVIMStateClass::CheckState( SV_STATE_REGRESSION ) )
	{
		//not allowed to exit during regression test
		return;
	}

	// If Running, check if access to exit run.
	if ( SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
	{
		if( TheSVObserverApp.m_svSecurityMgr.SVValidate( SECURITY_POINT_FILE_MENU_EXIT, 
			SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE ) == S_OK )
		{
			TheSVObserverApp.OnStop();
			l_bAllowAccess = true;
		}
	}
	else
	if( TheSVObserverApp.m_svSecurityMgr.SVValidate( SECURITY_POINT_FILE_MENU_EXIT ) == S_OK )
	{
		l_bAllowAccess = true;
	}

	if( l_bAllowAccess )
	{
		if( SVSVIMStateClass::CheckState( SV_STATE_TEST ) )
		{
			TheSVObserverApp.OnStop();
		}

		// Store current bar state...
		try
		{
			SaveBarState( _T( "Bar States" ) );
		}
		catch (CException* e)
		{
			e->Delete();
			ASSERT(FALSE);
		}

		// Log out...
		TheSVObserverApp.Logout();

		// Last checking...
		switch( TheSVObserverApp.CanCloseMainFrame() )
		{
			case S_OK:
			{
				CMDIFrameWnd::OnClose();

				break;
			}
			default:
			{
				break;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : // e.g. Run member function of class ...
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::OnUpdateStatusInfo( CCmdUI *pCmdUI )
{
	CStatusBarCtrl* pCtrl = ( CStatusBarCtrl* )pCmdUI->m_pOther;

	switch( pCmdUI->m_nID )
	{
		case ID_INDICATOR_INFO:
		{
			pCmdUI->Enable();
			break;
		}
		case ID_INDICATOR_USER:  // Status Bar User
		{
			BSTR l_bstUser = NULL;
			CString l_strUser = _T("User: ");
			if( TheSVObserverApp.m_svSecurityMgr.SVGetUseLogon() )
			{
				HRESULT l_hr = TheSVObserverApp.m_svSecurityMgr.SVGetUser( l_bstUser );
				if( l_bstUser != NULL )
				{
					l_strUser += l_bstUser;
					::SysFreeString( l_bstUser );
				}
			}
			pCmdUI->Enable();

			CClientDC DC( NULL );
			int l_Width = DC.GetTextExtent( l_strUser ).cx - ( l_strUser.GetLength() * 2 );

			m_wndStatusBar.SetPaneInfo( pCmdUI->m_nIndex, ID_INDICATOR_USER, 0, l_Width );
			pCmdUI->SetText( l_strUser );

			break;
		}
		case ID_INDICATOR_MODE:  // Status Bar Mode
		{
			CString l_strMode = _T("Mode: ");
			unsigned long l_lMode = 0;
			GlobalRCGetMode( &l_lMode );
			switch( l_lMode )
			{
				case SVIM_MODE_UNKNOWN:
				{
					m_wndStatusBar.m_lModeBKColor = RGB( 200, 200, 200 );
					m_wndStatusBar.m_lModeForeColor = RGB( 0, 0, 0 );
					break;
				}
				case SVIM_MODE_ONLINE:
				{
					m_wndStatusBar.m_lModeBKColor = RGB( 100, 255, 100 );
					m_wndStatusBar.m_lModeForeColor = RGB( 0, 0, 0 );
					l_strMode += _T("Run ");
					break;
				}
				case SVIM_MODE_OFFLINE:
				{
					m_wndStatusBar.m_lModeBKColor = RGB( 255, 128, 128 );
					m_wndStatusBar.m_lModeForeColor = RGB( 0, 0, 0 );
					l_strMode += _T("Stop ");
					break;
				}
				case SVIM_MODE_REGRESSION:
				{
					m_wndStatusBar.m_lModeBKColor = RGB( 255, 0, 255 );
					m_wndStatusBar.m_lModeForeColor = RGB( 0, 0, 0 );
					l_strMode += _T("Regression ");
					break;
				}
				case SVIM_MODE_TEST:
				{
					m_wndStatusBar.m_lModeBKColor = RGB( 0, 0, 200 );
					m_wndStatusBar.m_lModeForeColor = RGB( 255, 255, 255 );
					l_strMode += _T("Test ");
					break;
				}
				case SVIM_MODE_EDIT:
				{
					m_wndStatusBar.m_lModeBKColor = RGB( 255, 255, 75 );
					m_wndStatusBar.m_lModeForeColor = RGB( 0, 0, 0 );
					l_strMode += _T("Edit ");
					break;
				}
				case SVIM_MODE_CHANGING:
				{
					m_wndStatusBar.m_lModeBKColor = RGB( 255, 255, 255 );
					m_wndStatusBar.m_lModeForeColor = RGB( 0, 0, 0 );
					l_strMode += _T("Changing ");
					break;
				}
				default:
				{
					l_strMode += _T("Undefined");
				}
			}

			m_wndStatusBar.m_strModeText = l_strMode;

			pCmdUI->Enable();

			CClientDC DC( NULL );
			int l_Width = DC.GetTextExtent( l_strMode ).cx - ( l_strMode.GetLength() * 2 );

			m_wndStatusBar.SetPaneInfo( pCmdUI->m_nIndex, ID_INDICATOR_MODE, 0, l_Width );
			pCtrl->SetText( l_strMode, pCmdUI->m_nIndex, SBT_OWNERDRAW );

			break;
		}
		case ID_INDICATOR_GIGE_LICENSE_ERROR:
		{
			bool l_bMatroxError = false;
			bool l_bGigeError = false;
			CString l_strError = "";
			CString l_strMatroxError ="No Matrox License";
			CString l_strGigeError = "No GigE License";

			if ( !TheSVOLicenseManager().HasMatroxLicense() )
			{
				l_bMatroxError = true;
				l_strError += l_strMatroxError;
			}
			else
			{
				if ( !TheSVOLicenseManager().HasMatroxGigELicense() && TheSVObserverApp.IsMatroxGige() )
				{
					l_strError = l_strGigeError;
				}
			}

			if ( (l_bMatroxError) || (l_bGigeError) )
			{
				m_wndStatusBar.m_strErrorText = l_strError;

				pCmdUI->Enable();

				if (l_bMatroxError) 
				{
					m_wndStatusBar.m_lErrorBKColor = RGB( 255, 0, 0 );
					m_wndStatusBar.m_lErrorForeColor = RGB( 0, 0, 0 );
				}
				else
				{
					m_wndStatusBar.m_lErrorBKColor = RGB( 200, 200, 200 );
					m_wndStatusBar.m_lErrorForeColor = RGB( 0, 0, 0 );
				}

				CClientDC DC(NULL);
				int lWidth = DC.GetTextExtent( l_strError ).cx - ( l_strError.GetLength() *2 ) +20;
				m_wndStatusBar.SetPaneInfo( pCmdUI->m_nIndex, ID_INDICATOR_GIGE_LICENSE_ERROR, 0, lWidth );
				pCtrl->SetText( l_strError, pCmdUI->m_nIndex, SBT_OWNERDRAW );

			}
			else
			{
				m_wndStatusBar.SetPaneInfo( pCmdUI->m_nIndex,ID_INDICATOR_GIGE_LICENSE_ERROR,SBPS_DISABLED ,0);
				pCtrl->SetText( l_strError, pCmdUI->m_nIndex, SBT_OWNERDRAW );
			}
			break;
		}
		default:
		{
			// Do nothing.
			break;
		}
	}
	m_wndStatusBar.Invalidate();
}

LRESULT SVMainFrame::InvalidateStatusBar( WPARAM wParam, LPARAM lParam )
{
	m_wndStatusBar.Invalidate();
	return 0;
}

LRESULT SVMainFrame::RemoteRegisterMonitorList( WPARAM wParam, LPARAM lParam )
{
	TheSVObserverApp.GetIODoc()->GetIOController()->ShowMonitorListTab();
	return 0;
}

LRESULT SVMainFrame::Shutdown( WPARAM wParam, LPARAM lParam )
{
	long shutdownOption = static_cast<long>(wParam);
	char cmd[50];
	sprintf_s(cmd, "SVShutdown.exe %d 15", shutdownOption);
	system(cmd);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : // e.g. Run member function of class ...
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::OnMouseMove( UINT nFlags, CPoint point )
{
	SetStatusInfoText( NULL );
	
	CMDIFrameWnd::OnMouseMove( nFlags, point );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnViewPPQBar
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::OnViewPPQBar()
{
	bool l_IsWindowVisible = ( m_PPQBar.IsWindowVisible() != FALSE );
	bool l_ValidState = ( SVSVIMStateClass::CheckState( SV_STATE_READY | SV_STATE_RUNNING ) != FALSE );

	if ( ! l_IsWindowVisible && l_ValidState && TheSVObserverApp.OkToEdit() )
	{
		ShowControlBar( &m_PPQBar, TRUE, FALSE );
		m_PPQBarWasShown = TRUE;
		m_PPQBar.BuildButtons();
	}
	else
	{
		// Hide bar...
		ShowControlBar( &m_PPQBar, FALSE, FALSE );
		m_PPQBarWasShown = FALSE;
	}

	RecalcLayout();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnViewToolBar
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::OnViewToolBar()
{
	if( !m_wndToolBar.IsWindowVisible() )
	{
		ShowControlBar( &m_wndToolBar, TRUE, FALSE );
		m_toolbarWasShown = TRUE;
	}
	else
	{
		// Hide bar...
		ShowControlBar( &m_wndToolBar, FALSE, FALSE );
		m_toolbarWasShown = FALSE;
	}

	RecalcLayout();
}

void SVMainFrame::OnToolbarZoom()
{
	if( !m_wndToolBarZoom.IsWindowVisible() )
	{
		ShowControlBar( &m_wndToolBarZoom, TRUE, FALSE );
		m_toolbarZoomWasShown = TRUE;
	}
	else
	{
		// Hide bar...
		ShowControlBar( &m_wndToolBarZoom, FALSE, FALSE );
		m_toolbarZoomWasShown = FALSE;
	}

	RecalcLayout();
}





////////////////////////////////////////////////////////////////////////////////
// .Title       : OnUpdateViewPPQBar
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::OnUpdateViewPPQBar( CCmdUI* pCmdUI )
{
	BOOL bEnable = !SVSVIMStateClass::CheckState( SV_STATE_TEST|SV_STATE_RUNNING );
	bEnable &= TheSVObserverApp.OkToEdit() ? 1 : 0;
	bEnable &= !SVSVIMStateClass::CheckState( SV_STATE_REGRESSION );
	bEnable |= m_PPQBar.IsWindowVisible();
	pCmdUI->Enable(bEnable);

	if( m_PPQBar.IsWindowVisible() )
	{
		pCmdUI->SetCheck( 1 );
	}
	else
	{
		pCmdUI->SetCheck( 0 );
	}
	//@TODO mec disable submenu ???

	bool enableToolbars =  ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION );
	CMenu* pMenu = GetMenu();
	EnableParentMenu(pMenu,ID_TOOLBAR_ZOOM, enableToolbars,3 );
	

	
	
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnUpdateViewToolbar
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::OnUpdateViewToolbar( CCmdUI* pCmdUI )
{
	pCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION ) );

	if ( m_wndToolBar.IsWindowVisible() )
	{
		pCmdUI->SetCheck( 1 );
	}
	else
	{
		pCmdUI->SetCheck( 0 );
	}
}

void SVMainFrame::OnUpdateToolbarZoom(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( !SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION ) );

	if ( m_wndToolBarZoom.IsWindowVisible() )
	{
		pCmdUI->SetCheck( 1 );
	}
	else
	{
		pCmdUI->SetCheck( 0 );
	}
}

HRESULT SVMainFrame::ParseToolsetScripts( SVTreeType& p_rTree )
{
	HRESULT l_Status( S_OK );

	SVTreeType::SVBranchHandle hItemToolset = nullptr;
	SVTreeType::SVBranchHandle htiChild = nullptr;

	if( SVNavigateTreeClass::GetItemBranch( p_rTree, CTAG_INSPECTION, nullptr, htiChild ) )
	{
		// Set the Caption
		CString title = _T( "Loading Toolset(s) ..." );
		SVParserProgressDialog l_ParserProgressDialog( title, this );

		SVTreeType::SVBranchHandle htiSubChild = nullptr;
		p_rTree.GetFirstBranch( htiChild, htiSubChild );
		while( l_Status == S_OK && nullptr != htiSubChild )
		{
			SVSharedPtr<CString> pToolsetScript = new CString;
			SVGUID ownerGuid;

			SVInspectionProcess* pInspect( nullptr );
			SVTreeType::SVBranchHandle htiSVIPDoc = nullptr;
			SVTreeType::SVBranchHandle htiDataChild = nullptr;
			p_rTree.GetFirstBranch( htiSubChild, htiDataChild );

			while( nullptr != htiDataChild )
			{
				_bstr_t DataName;
				p_rTree.GetBranchName( htiDataChild, DataName.GetBSTR() );
				CString csDataName = DataName;

				if ( csDataName.Compare( CTAG_INSPECTION_PROCESS ) == 0 )
				{
					if( SVNavigateTreeClass::GetItemBranch( p_rTree, CTAG_TOOLSET_SET, htiDataChild, hItemToolset ) )
					{
						_variant_t svVariant;

						if( SVNavigateTreeClass::GetItem( p_rTree, CTAG_UNIQUE_REFERENCE_ID, htiDataChild, svVariant ) )
						{
							ownerGuid = svVariant;
						}
					}
				}
				else if ( csDataName.Compare( CTAG_SVIPDOC ) == 0 )
				{
					htiSVIPDoc = htiDataChild;
				}

				p_rTree.GetNextBranch( htiSubChild, htiDataChild );
			}

			if( nullptr != htiSVIPDoc )
			{
				_variant_t svVariant;

				if( ownerGuid.empty() )
				{
					if( SVNavigateTreeClass::GetItem( p_rTree, CTAG_UNIQUE_REFERENCE_ID, htiSVIPDoc, svVariant ) )
					{
						ownerGuid = svVariant;
					}
					else
					{
						l_Status = E_FAIL;
					}
				}

				if( S_OK == l_Status )
				{
					if( SVNavigateTreeClass::GetItem( p_rTree, CTAG_TOOLSET_SCRIPT, htiSVIPDoc, svVariant ) )
					{
						CString newString;
						newString.LoadString(IDS_OBJECTNAME_PAT_DELTAPOS);

						SVString l_String( svVariant );

						// Pre-Parse Typo fix.
						// This peice of code fixes the Angel to Angle typo.
						l_String.replace( _T("Axis Secondary Angel"), _T("Axis Secondary Angle") );

						// Pre-Parse Typo fix number two.
						// This peice of code fixes the Principle to Principal typo.
						l_String.replace( _T("Axis Principle Angle"), _T("Axis Principal Angle") );

						// Pre-Parse Type fix number three.
						// This piece of code changes "Angle delta Positive" to "Angle Delta Positive".
						l_String.replace( _T("Angle delta Positive"), static_cast< LPCTSTR >( newString ) );

						*pToolsetScript = l_String.ToString();
					}
					else if( nullptr == hItemToolset )
					{
						l_Status = E_FAIL;
					}
				}
			}
			else
			{
				l_Status = E_FAIL;
			}

			if( S_OK == l_Status )
			{
				if( !( ownerGuid.empty() ) )
				{
					pInspect = dynamic_cast< SVInspectionProcess* >( SVObjectManagerClass::Instance().GetObject( ownerGuid ) );

					if( nullptr == pInspect )
					{
						l_Status = E_FAIL;
					}
				}
				else
				{
					l_Status = E_FAIL;
				}
			}

			if( S_OK == l_Status )
			{
				SVConfigurationObject* pConfig = nullptr;
				SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

				unsigned long parserHandle = SVObjectScriptParserClass::GetParserHandle();

				unsigned long ulVersion = pConfig->GetFileVersion();

				if (nullptr == hItemToolset)
				{
					SVObjectScriptParserClass* pParser = nullptr;
					if ( ulVersion < 0x00042800 )	// SVO 4.40 minimum for new parser
					{
						pParser = new SVObjectScriptParserClass(new SVObjectScriptParserSVXClass(parserHandle, pToolsetScript, ownerGuid, pInspect, &l_ParserProgressDialog));
					}
					else
					{
						pParser = new SVObjectScriptParserClass(new SVObjectScriptParserSVXClass(parserHandle, pToolsetScript, ownerGuid, pInspect, &l_ParserProgressDialog));
					}
					if( nullptr != pParser )
					{
						// Set the Parser Object
						l_ParserProgressDialog.AddParser(parserHandle, pParser);
					}
					else
					{
						l_Status = E_FAIL;
					}
				}
				else // new parsing
				{
					SVConfigurationObject::updateConfTreeToNewestVersion(p_rTree, hItemToolset);
					SVObjectScriptParserClass* pParser = new SVObjectScriptParserClass(new SVInspectionTreeParser< SVTreeType >(p_rTree, hItemToolset, parserHandle, ownerGuid, pInspect, &l_ParserProgressDialog));
					if( nullptr != pParser )
					{
						// Set the Parser Object
						l_ParserProgressDialog.AddParser(parserHandle, pParser);
					}
					else
					{
						l_Status = E_FAIL;
					}
				}
			}
			p_rTree.GetNextBranch( htiChild, htiSubChild );
		}

		if( S_OK == l_Status )
		{
			// Show the Dialog
			l_ParserProgressDialog.DoModal();
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : BuildPPQButtons
// -----------------------------------------------------------------------------
// .Description : Calls PPQBar::BuildButtons
// -----------------------------------------------------------------------------
// .Return Value
//	:BOOL 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:BOOL PPQBar::BuildButtons()
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :08.01.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVMainFrame::BuildPPQButtons()
{
	return m_PPQBar.BuildButtons();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : DestroyButtons
// -----------------------------------------------------------------------------
// .Description : Calls PPQBar::DestroyButtons
// -----------------------------------------------------------------------------
// .Return Value
//	:BOOL 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:BOOL PPQBar::DestroyButtons()
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :05.05.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVMainFrame::DestroyPPQButtons()
{
	return m_PPQBar.DestroyButtons();
}

///////////////////////////////////////////////////////
// handler for End Object Script Parsing message
///////////////////////////////////////////////////////
LRESULT SVMainFrame::OnEndObjectScriptParse(WPARAM wParam, LPARAM lParam)
{
	BOOL rc = TRUE;
	return rc;
}

///////////////////////////////////////////////////////
// handler for End Object Script Parsing message
///////////////////////////////////////////////////////
LRESULT SVMainFrame::OnEndObjectCreation(WPARAM wParam, LPARAM lParam)
{
	BOOL rc = TRUE;
	return rc;
}

void SVMainFrame::OnConfigurationFinishedInitializing()
{
	//
	// Finally indicate to the 'world' that the configuration is loaded
	// and ready to run or test.
	//
	// All operations that need to happen after the configuration is fully
	// loaded and initialized should go here

	//
	// Update the IO views.  13 Jan 2000 - frb.
	//

	SVIODoc* l_pIODoc( TheSVObserverApp.GetIODoc() );

	if( l_pIODoc != NULL )
	{
		SVIOController* l_pIOController( l_pIODoc->GetIOController() );

		if( l_pIOController != NULL )
		{
			l_pIOController->RebuildOutputList();
		}

		l_pIODoc->UpdateAllViews( NULL );
	}// end if

	TheSVObserverApp.UpdateAllMenuExtrasUtilities();

	//
	// Set the tool selected for an operator to move if any.
	// 26 Jan 2000 - frb.
	//
	PostMessage( SV_SET_TOOL_SELECTED_IN_TOOL_VIEW, (WPARAM) TRUE );

	//
	// Indicate to the world that a configuration is loaded, parsed,
	// and is ready to run.
	//
	SVSVIMStateClass::AddState( SV_STATE_READY );
	SVSVIMStateClass::RemoveState( SV_STATE_UNAVAILABLE | SV_STATE_LOADING );

	//
	// If the configuration is required to start immediately after loading
	// or a GoOnline command was received from SVFocusNT,
	// then start the toolset, i.e. go 'online'.
	//
	if ( TheSVObserverApp.getShouldRunLastEnvironmentAutomatically() ||
		SVSVIMStateClass::CheckState( SV_STATE_START_PENDING ) )
	{
		PostMessage ( WM_COMMAND, MAKEWPARAM (ID_MODE_RUN, 0), 0 );
	}

	//
	// See if this configuration was downloaded from a remote computer.
	//
	if ( m_notifyCommRC )
	{
		m_notifyCommRC = FALSE;
	}

	TheSVObserverApp.RemoveUnusedFiles();
}

void SVMainFrame::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	CMDIFrameWnd::OnActivateApp( bActive, dwThreadID );

	if( bActive )
	{
		if( !( SVDirectX::Instance().empty() ) )
		{
			if( SVDirectX::Instance().IsLost() == DDERR_SURFACELOST )
			{
				HRESULT hr = SVDirectX::Instance().RestoreAllSurfaces();

				if (hr != DD_OK)
				{
					TRACE(_T( "SVMainFrm::OnActivateApp - RestoreAllSurfaces failed\n" ) );
				}
			}
		}
	}
}

LRESULT SVMainFrame::OnDisplayChange(WPARAM p_p1, LPARAM p_p2)
{
	LRESULT rc = FALSE;
	
	if( !( SVDirectX::Instance().empty() ) )
	{
		bool bResolutionChanged(false);

		// check for resolution change
		SIZE size = { 0, 0 };
		HRESULT hr = SVDirectX::GetPrimarySurfaceResolution(size);
		if (hr == DD_OK)
		{
			if (size.cx != LOWORD(p_p2) || size.cy != HIWORD(p_p2))
			{
				bResolutionChanged = true;
			}
		}
		else
		{
			bResolutionChanged = true;
		}

		// check for pixel depth change
		int l_PixelDepth(0);
		hr = SVDirectX::GetPrimarySurfacePixelDepth(l_PixelDepth);
		if (hr == DD_OK)
		{
			if (p_p1 / 8 != l_PixelDepth)
			{
				bResolutionChanged = true;
			}
		}
		else
		{
			bResolutionChanged = true;
		}
		
		// NetOp and Microsoft Remote Desktop will cause the DirectDraw7 Object to become invalid.
		// Recreate the DirectDraw7 Object and Primary Surface
		// Other surfaces will be recreated elsewhere (e.g ImageView)
		// TestCooperativeLevel will return DDERR_WRONGMODE when this happens
		hr = SVDirectX::TestCooperativeLevel();
		if (hr == DDERR_WRONGMODE)
		{
			hr = SVDirectX::Instance().Initialize();
			if (hr == DD_OK)
			{
				hr = SVDirectX::CreatePrimarySurface();
				if (hr == DD_OK)
				{
					// recreate Image Surfaces
					CMDIChildWnd* pMDIChild = MDIGetActive();
					while (pMDIChild != NULL)
					{
						CDocument* pCurrentDocument = pMDIChild->GetActiveDocument();
						if (pCurrentDocument)
						{
							SVIPDoc* pIPDoc = dynamic_cast<SVIPDoc *>(pCurrentDocument);
							if (pIPDoc)
							{
								pIPDoc->RecreateImageSurfaces();
							}
						}

						// get Next MDI Child Window
						pMDIChild = (CMDIChildWnd*) pMDIChild->GetWindow(GW_HWNDNEXT);
					}
				}
			}
			else
			{
				TRACE(_T( "SVMainFrm::OnDisplayChange - SVDirectX.Initialize() failed\n" ) );
			}
		}
		// restore surfaces
		else
		{
			hr = SVDirectX::Instance().RestoreAllSurfaces();

			if (hr != DD_OK)
			{
				TRACE(_T( "SVMainFrm::OnDisplayChange - RestoreAllSurfaces failed\n" ) );
			}
		}

		// if pixel depth changed - show mesage...
		if ( p_p1 / 8 != l_PixelDepth )
		//if (bResolutionChanged) // SEJ - what about resolution change ?
		{
			if( WAIT_TIMEOUT == WaitForSingleObject( m_hDisplayChangedEvent, 0 ) )
			{
				CString l_csMessage;
				l_csMessage.Format( "\n\n\nThe display color settings on the system have changed.\n"
					"Images may not display correctly until this configuration is reopened." );
				HANDLE l_hMessage = ::CreateEvent( NULL, TRUE, FALSE, NULL );
				::SetEvent( l_hMessage );
				ModelessMessageBox( l_csMessage, l_hMessage );
			}
			::SetEvent( m_hDisplayChangedEvent );
		}
		else
		{
			::ResetEvent( m_hDisplayChangedEvent );
		}
	}
	return rc;
}

LRESULT SVMainFrame::OnSetMode( WPARAM wParam, LPARAM lParam )
{
	return TheSVObserverApp.SetMode( static_cast<unsigned long>(lParam ));
}

LRESULT SVMainFrame::OnLoadConfiguration( WPARAM wParam, LPARAM lParam )
{
	return TheSVObserverApp.LoadConfiguration();
}


//******************************************************************************
// Debug Operation(s):
//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
// .Title       : // e.g. Run member function of class ...
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void SVMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : // e.g. Run member function of class ...
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

void SVMainFrame::SetNotifyCommRC()
{
	m_notifyCommRC = TRUE;
}


LRESULT SVMainFrame::OnUserChanged( WPARAM wParam, LPARAM lParam )
{
	BOOL recalcNeeded = FALSE;
	BOOL accessAllowed = FALSE;

	accessAllowed = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_VIEW_MENU_PPQ_BAR ) ;  // ID_VIEW_PPQ_BAR );

	if( !accessAllowed )
	{
		// Hide PPQ Bar
		if( m_PPQBar.IsWindowVisible() )
		{
			ShowControlBar( &m_PPQBar, FALSE, FALSE );
			recalcNeeded = TRUE;
		}
	}

	// Reposition windows if required
	if( recalcNeeded )
		RecalcLayout();

	return (LRESULT)TRUE;
}

void SVMainFrame::OnViewImageDisplayUpdate()
{
	if( TheSVObserverApp.m_svSecurityMgr.SVValidate( SECURITY_POINT_VIEW_MENU_IMAGE_DISPLAY_UPDATE ) == S_OK )
	{
		SVSVIMStateClass::AddState( SV_STATE_MODIFIED );

		BOOL ImageUpdate = FALSE;
		EnvironmentObject::getEnvironmentValue( ::EnvironmentImageUpdate, ImageUpdate );
		EnvironmentObject::setEnvironmentValue( ::EnvironmentImageUpdate, !ImageUpdate );
	}
}

void SVMainFrame::OnUpdateViewImageDisplayUpdate( CCmdUI* pCmdUI )
{
	BOOL bEnable = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_VIEW_MENU_IMAGE_DISPLAY_UPDATE );
	bEnable &= !SVSVIMStateClass::CheckState( SV_STATE_REGRESSION );
	pCmdUI->Enable( bEnable);
	BOOL ImageUpdate;
	EnvironmentObject::getEnvironmentValue( ::EnvironmentImageUpdate, ImageUpdate );
	pCmdUI->SetCheck( ImageUpdate );

	bool bEnableZoom = false;
	CMDIChildWnd *pChild = (CMDIChildWnd*) GetActiveFrame();
	CView*  pView =  pChild->GetActiveView();
	SVImageViewClass *pIView = NULL;
	SVImageViewScroll *pSView = NULL;
	pIView = dynamic_cast<SVImageViewClass*> (pView);
	if(pIView)
	{
		bEnableZoom = pIView->IsZoomAllowed();
	}
	else
	{
		pSView = dynamic_cast<SVImageViewScroll*>( pView);
	}
	if(pSView)
	{
		bEnableZoom = pSView->IsZoomAllowed();
	}

	CMenu* pMenu = GetMenu();
	if(pMenu)
	{
		EnableParentMenu(pMenu,ID_ZOOM_MINUS, bEnableZoom,3 );
	}
}

void SVMainFrame::OnViewResultDisplayUpdate()
{
	if( TheSVObserverApp.m_svSecurityMgr.SVValidate( SECURITY_POINT_VIEW_MENU_RESULT_DISPLAY_UPDATE ) == S_OK )
	{
		SVSVIMStateClass::AddState( SV_STATE_MODIFIED );

		BOOL ResultOnline = FALSE;
		EnvironmentObject::getEnvironmentValue( ::EnvironmentResultUpdate, ResultOnline );
		EnvironmentObject::setEnvironmentValue( ::EnvironmentResultUpdate, !ResultOnline );
	}
}

void SVMainFrame::OnUpdateViewResultDisplayUpdate( CCmdUI* pCmdUI )
{
	BOOL bEnable = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_VIEW_MENU_RESULT_DISPLAY_UPDATE );
	bEnable &= !SVSVIMStateClass::CheckState( SV_STATE_REGRESSION );
	pCmdUI->Enable( bEnable);

	BOOL ResultOnline;
	EnvironmentObject::getEnvironmentValue( ::EnvironmentResultUpdate, ResultOnline );
	pCmdUI->SetCheck( ResultOnline );
}

void SVMainFrame::OnExtrasEnableAutoSave()
{
	if( TheSVObserverApp.m_svSecurityMgr.SVValidate(
		SECURITY_POINT_EXTRAS_MENU_AUTOSAVE_CONFIGURATION) != S_OK )
	{
		return; //Arvid: not validated:do nothing
	}

	AutoSaver::Instance().ToggleEnable();
}

void SVMainFrame::OnUpdateExtrasEnableAutoSave(CCmdUI *pCmdUI)
{
	BOOL bEnable = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_EXTRAS_MENU_AUTOSAVE_CONFIGURATION );

	pCmdUI->Enable( bEnable );
	pCmdUI->SetCheck( AutoSaver::Instance().IsEnabled()==TRUE );
}

BOOL SVMainFrame::VerifyBarStates(LPCTSTR lpszProfileName)
{
	CDockState state;
	state.LoadState(lpszProfileName);

	for (int i = 0; i < state.m_arrBarInfo.GetSize(); i++)
	{
		CControlBarInfo* pInfo = (CControlBarInfo*)state.m_arrBarInfo[i];

		ASSERT(pInfo != NULL);

		int nDockedCount = static_cast<int>(pInfo->m_arrBarID.GetSize());
		if (nDockedCount > 0)
		{
			// dockbar
			for (int j = 0; j < nDockedCount; j++)
			{
				UINT nID = (UINT) pInfo->m_arrBarID[j];
				if (nID == 0) { continue; } // row separator

				if (nID > 0xFFFF)
				{
					nID &= 0xFFFF; // placeholder - get the ID
				}

				if (GetControlBar(nID) == NULL)
				{
					return FALSE;
				}
			}
		}
		
		if (!pInfo->m_bFloating) // floating dockbars can be created later
		{
			if (GetControlBar(pInfo->m_nBarID) == NULL)
			{
				return FALSE; // invalid bar ID
			}
		}
		else
		{
			if(nullptr == dynamic_cast<CDockBar*>(pInfo->m_pBar))
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

void SVMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CMDIFrameWnd::OnSize(nType, cx, cy);

	if ( nType == SIZE_MINIMIZED )
	{
		// Do nothing else.  Just return.
		return;
	}

	if (m_bSizeChanged)
	{
		WINDOWPLACEMENT lpwndpl;

		GetWindowPlacement(&lpwndpl);
		AfxGetApp()->WriteProfileBinary("MainFrame", "WP", (LPBYTE)&lpwndpl, sizeof(lpwndpl));
		m_bSizeChanged = FALSE;
	}

	if ( nType == SIZE_MAXIMIZED )
	{  //only do something if SIZE_RESTORED
		if ( m_PosSize == SIZE_RESTORED )
		{
			m_PosSize = SIZE_MAXIMIZED;
		}
		return;
	}
	
	if ( nType == SIZE_RESTORED )
	{
		WINDOWPLACEMENT *lwp;
		UINT nl;

		if ( m_PosSize == SIZE_MAXIMIZED )
		{
			if(AfxGetApp()->GetProfileBinary("MainFrame", "WP", (LPBYTE*)&lwp, &nl))
			{
				SetWindowPlacement(lwp);
				delete [] lwp;
			}		
		}
		m_PosSize = SIZE_RESTORED;
	}
}

void SVMainFrame::OnSizing(UINT fwSide, LPRECT pRect)
{
	m_bSizeChanged = TRUE;

	CMDIFrameWnd::OnSizing(fwSide, pRect);
}

LRESULT SVMainFrame::OnHideIOViewTab( WPARAM p_wParam, LPARAM p_lParam )
{
	TheSVObserverApp.HideIOTab( (DWORD)p_wParam);
	return 0;
}

double SVMainFrame::GetZoomToolbarValue() const
{
	return m_wndToolBarZoom.GetValue();
}

void SVMainFrame::SetZoomToolbar(const ZoomHelperEx& helper)
{
	m_wndToolBarZoom.SetRange(helper.GetZoomMin(), helper.GetZoomMax(), helper.GetZoomStep());
	m_wndToolBarZoom.SetValue(helper.GetZoom());
}

void SVMainFrame::EnableZoomToolbar(bool enable)
{
	m_wndToolBarZoom.Enable(enable);
}

bool SVMainFrame::IsZoomingPossible()
{
	CMDIChildWnd* pChild = dynamic_cast<CMDIChildWnd*>(GetActiveFrame());

	CView* pView = nullptr;
	if(nullptr != pChild)
	{
		pView = pChild->GetActiveView();
	}

	SVImageViewScroll* pImageViewScroll = dynamic_cast<SVImageViewScroll*>(pView);

	if(nullptr != pImageViewScroll)
	{
		return !pImageViewScroll->ImageIsEmpty();
	}

	SVImageViewClass* pImageView = dynamic_cast<SVImageViewClass*>(pView);

	if(nullptr != pImageView)
	{
		return !pImageView->ImageIsEmpty();
	}

	return false;
}

void SVMainFrame::RefreshAllSplitters()
{
	SVIPSplitterFrame* pSplitterFrame = dynamic_cast< SVIPSplitterFrame*>(GetActiveFrame());
	if(nullptr != pSplitterFrame)
	{
		pSplitterFrame->RefreshAllSplitters();
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVMainFrm.CPP_v  $
 * 
 *    Rev 1.17   19 Dec 2014 04:14:46   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  978
 * SCR Title:  Copy and Paste a Tool within an Inspection or Between Different Inspections
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Changed script string to be a shared pointer to avoid memory leaks
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   12 Dec 2014 13:12:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  918
 * SCR Title:  Implement Method RegisterMonitorList for RemoteControl (SVO-369)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   added new message handler for SV_REGISTER_MONITOR_LIST RemoteRegisterMonitorList.,
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   09 Dec 2014 10:12:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  908
 * SCR Title:  Remove option for Operator Move (SVO 101)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Remove edit move state and supporting functions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   05 Sep 2014 13:17:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  921
 * SCR Title:  Add more complete zoom functionality. (runpage)
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   changed ID_VIEW_TOOLBAR to ID_TOOLBAR_STANDARD so that Help would work correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   02 Sep 2014 12:16:48   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  921
 * SCR Title:  Add more complete zoom functionality. (runpage)
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Added method RefreshAllSplitters.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   27 Aug 2014 11:06:12   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Fixed: Security login dialog not being displayed in force mode
 * Method changed: OnViewImageDisplayUpdate, OnViewResultDisplayUpdate
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   14 Aug 2014 16:09:36   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  921
 * SCR Title:  Add more complete zoom functionality. (runpage)
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   error handling in checkbarstate
 * New zoom toolbar 
 * new entry in ViewMenu 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   17 Jul 2014 19:22:30   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed namespaces and code review changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   07 Jul 2014 16:52:08   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Split the Security access from Image Online Display to Image Display Update and Result Display Update
 * Methods changed OnUpdateViewImageDisplayUpdate, OnUpdateViewResultDisplayUpdate
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   24 Jun 2014 07:08:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  905
 * SCR Title:  Implement Shutdown Command thru Remote Control
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add Shutdown message handler function that will start the shutdown executable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   02 Jun 2014 09:54:04   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added ID_VIEW_IMAGE_DISPLAY_UPDATE and ID_VIEW_RESULT_DISPLAY_UPDATE command and update command handlers.
 * Removed the ID_VIEW_ONLINE_DISPLAY command and update command handlers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   28 Feb 2014 09:16:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  890
 * SCR Title:  Fix SVObserver Memory Leaks
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Delete toolset script string in ParseToolsetScripts.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   04 Feb 2014 15:26:08   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  880
 * SCR Title:  Remove .SEC
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Removed include of SVObjectScriptParserSEC.h.
 * Removed extern declaration of Global function and instead include RemoteCommand.h.
 * Added regions for Declarations and Constructor.
 * Changed method UpdateFrameTitleForDocument to call getConfigFileName instead of GetSECFileName.
 * Changed method OnUpdateStatusInfo to call GlobalRCGetMode from Seidenader::SVObserver namespace instead of the global namespace.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   31 Jan 2014 17:16:30   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed to follow guidelines more closely.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Oct 2013 14:57:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Aug 2013 06:17:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added refresh message to update the status bar for mode changes from SVRemoteControl.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 20:06:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.82   16 Apr 2013 15:35:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:25:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.81   25 Mar 2013 13:36:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed OnViewInspectionToolBar, OnUpdateViewInspectionToolBar
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.80   25 Mar 2013 12:42:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed the Identification Error from the status bar
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.79   20 Mar 2013 07:08:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  819
 * SCR Title:  Modify SVObserver to force the status bar to update
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added call to the status bar Invalidate after the status bar is updated.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.78   27 Feb 2013 11:23:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added case for OnUpdateStatusInfo for license errors
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.77   11 Feb 2013 14:19:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new framework for the remote socket interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.76   10 Jan 2013 15:06:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  792
 * SCR Title:  Change SVObserver Background Color to Körber Grey
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added missing log entries.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.75   11 Oct 2012 10:54:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information based on the navigate tree class moving to the XML library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.74   01 Oct 2012 16:18:00   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  792
 * SCR Title:  Change SVObserver Background Color to Körber Grey
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed MDI Client Window background to grey.
 *   Changed m_BlueBrush to m_BackgroundBrush.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.73   28 Sep 2012 14:33:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upated code to use new templated classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.72   25 Sep 2012 15:21:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.71   18 Sep 2012 18:35:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  781
 * SCR Title:  Update SVObserver Version Number for Esok Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the creation and management for the version information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.70   18 Sep 2012 18:22:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.69   06 Sep 2012 10:09:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Parse Toolset Scripts method to adjust the data collection functionality based on moving the inspection process attributes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.68   04 Sep 2012 15:43:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Navigate Tree class to not force the programmer to create an instance of the class to use it.  Converted all functionality to static.
 * 
 * Update source code to include new parser dialog changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.67.1.0   08 Nov 2012 15:25:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  792
 * SCR Title:  Change SVObserver Background Color to Körber Grey
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   changed backgroud color to grey
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.67   14 Aug 2012 15:56:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated output list functionality to only allow unique elements to the output list and notify user when duplicates are detected.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.66   03 Aug 2012 11:48:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed dead code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.65   09 Jul 2012 14:08:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the include for SVIM State.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.64   02 Jul 2012 17:19:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.63   28 Jun 2012 18:00:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  768
 * SCR Title:  Add the display time-out and throttling options for 5.50
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Reverted back to before changes were made for this change.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.62   19 Jun 2012 13:00:06   jspila
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
 *    Rev 3.61   23 May 2012 10:13:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  768
 * SCR Title:  Add the display time-out and throttling options for 5.50
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed code in OnMsgSetToolSelected from merge error.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.60   16 May 2012 18:53:12   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  763
 * SCR Title:  Add the display time-out and throttling options.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Merge 5.43 changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.59   13 Mar 2012 13:45:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  745
 * SCR Title:  Fix contention issue during load process and display
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated OnMsgSetToolSelected to call through SVObserver App instead of throught configuration object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.58.1.1   15 May 2012 10:38:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  763
 * SCR Title:  Add the display time-out and throttling options.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed Modified Flag from timeout setting because it writes to the svim.ini file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.58.1.0   14 May 2012 14:21:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  763
 * SCR Title:  Add the display time-out and throttling options.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Implement Freeze Dialog
 * Implement display update rate options
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.58   28 Sep 2011 13:09:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to fix issues with camera image interface and methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.57   16 Sep 2011 16:05:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated object to use the new resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.56   05 Jul 2011 12:35:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  723
 * SCR Title:  Remove Free Memory Check from Status Area
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove the memory check process and status bar display.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.55   29 Apr 2011 08:06:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to allow for inspection to for the display to give up when an inspection get queued.  The display is now regulated to a maximum of 10 frames per second.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.54   30 Mar 2011 16:11:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to clairify construction functionality and add clean-up functionality to remove memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.53   27 Jan 2011 11:33:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.52   08 Dec 2010 13:05:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.51   09 Nov 2010 16:21:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.50   15 Oct 2010 15:25:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code assoicated with casting issues and HBITMAP changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.49   27 Jul 2010 10:53:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  697
 * SCR Title:  Add Free Memory Information to Status Bar
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed to use default Device Context instead of status bar Device Context.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.48   23 Jul 2010 09:35:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  697
 * SCR Title:  Add Free Memory Information to Status Bar
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated status bar functionality to better size the data so that it does not scroll off the screen.  Updated version number to 5.32 Beta 026.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.47   19 Jul 2010 13:19:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  694
 * SCR Title:  Upgrade SVObserver version for Chromis release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issue with returning NULL for user name.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.46   13 Jul 2010 12:25:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  697
 * SCR Title:  Add Free Memory Information to Status Bar
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes to status bar to add free memory status.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.45   29 Jun 2010 14:02:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to change object validity test and casting changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.44   16 Dec 2009 12:00:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.43   21 Sep 2009 13:33:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  662
 * SCR Title:  Add Display Change Support to SVImageView
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added OnDisplayChange handler.
 * Added OnActivateApp handler.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.42   14 Nov 2008 14:22:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added IOView Tabs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.41   07 Mar 2008 10:38:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Used DestroyIcon instead of DeleteObject for icons.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.40   20 Jun 2007 16:24:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * These changes also include changes to the SVObserver COM Server and Connection Point functionality.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.39   30 Aug 2005 13:29:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified onUpdatestatusText so text and color update at the same time.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.38   26 Aug 2005 14:36:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  493
 * SCR Title:  Fix Bug in SVOutputObjectList
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Moved ValidateOutputList function to OnConfigurationFinishedInitializing(
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.37   22 Aug 2005 11:16:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed name of function SVIsAllowed to SVIsDisplayable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.36   17 Aug 2005 11:48:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed bug with status bar text
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.35   05 Aug 2005 09:07:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Implement Mode on StatusBar and Add color indicators.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.34   02 Aug 2005 13:51:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed Status Option from View Menu
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.33   01 Aug 2005 08:37:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Status Indicator for Current User
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.32   21 Jul 2005 14:41:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problem with regression test.  moved the run dialog to SVMainFrame, instead of having it owned by IPDoc. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.31   15 Jul 2005 11:32:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added code for switching modes for use with the regression test
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.30   21 Jun 2005 13:07:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial changes to add SVSecurity
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.29   17 Feb 2005 14:48:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.28   16 Feb 2005 14:47:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   12 Jul 2004 12:58:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  428
 * SCR Title:  Improve Load Configuration time
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented changes for the new parser
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   22 Mar 2004 13:25:28   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  409
 * SCR Title:  Fix loading of a configuration from the Run directory.
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed loading from the run directory to not delete unused files until all the objects have been parsed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   08 Jul 2003 09:15:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  374
 * SCR Title:  Add check before changing tool parameters when processing input list
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added call to BuildValueObjectMap to the loading process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   25 Jun 2003 09:12:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  371
 * SCR Title:  Remove DOS Focus COM support
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed the CommPort support for DOS Focus
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   22 Apr 2003 11:06:58   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   27 Feb 2003 15:02:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  323
 * SCR Title:  Bar State problem when switching versions
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added method - VerifyBarStates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   19 Nov 2002 15:31:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed Application Bar
 *             Added Tool Bar
 *             Added Status Bar
 *             Added View Online Display
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   12 Nov 2001 10:10:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  238
 * SCR Title:  Security Issue: Some users without the privlegy can execute "Move Tools".
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed short-cut logic in OnMsgSetToolSelected.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   09 Aug 2001 07:54:00   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  210
 * SCR Title:  Fix of shared data, multiple IPD's, same digitizer
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Removed validation and initialization code from
 * "OnEndObjectScriptParse" and placed this code in new function
 * "OnEndObjectCreation".  
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   07 Aug 2001 15:15:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  208
 * SCR Title:  Improve configuration load time
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Removed unneeded call to SVM_RESET_ALL_OBJECTS
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   12 Jun 2001 16:07:26   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified SVObserver state code in functions, OnUpdateFrameTitle, OnUpdateViewApplicationBar, OnViewPPQBar, OnUpdateViewPPQBar, OnUpdateViewStatusBar, OnUpdateViewToolbar, OnUpdateViewInspectionToolBar, OnEndObjectScriptParse.  Modified the OnClose function concerning SVObserver state and COM server lock count.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   19 Apr 2001 14:42:24   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   08 Mar 2001 12:48:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  193
 * SCR Title:  Fix intermittent problems with the remote go on-line command from SVFocusNT
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update OnEndObjectScriptParse method to include setting SVObserverApp::WasSECLoading flag tp FALSE.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   25 Jan 2001 16:19:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   These changes fixed the SVObserverApp file handling and associated systems.  These systems include the .SEC, .IOD, and .IPD files as well as the camera files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12.1.1   10 May 2001 17:54:28   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  197
 * SCR Title:  Fix lock-ups in application when loading configuration from SVFocusNT
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed SVObserver state code in
 * OnUpdateFrameTitle
 * UpdateFrameTitleForDocument
 * OnClose
 * OnUpdateViewApplicationBar
 * OnViewPPQBar
 * OnUpdateViewPPQBar
 * OnUpdateViewStatusBar
 * OnUpdateViewToolbar
 * OnUpdateViewInspectionToolBar
 * OnEndObjectScriptParse
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12.1.0   15 Feb 2001 09:44:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  193
 * SCR Title:  Fix intermittent problems with the remote go on-line command from SVFocusNT
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update OnEndObjectScriptParse method to include setting SVObserverApp::WasSECLoading flag tp FALSE.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   28 Aug 2000 16:03:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  177
 * SCR Title:  Fix Automatic Load Last SEC Problem
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update FastOCR to prevent memory access violations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   15 Jun 2000 08:49:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  163
 * SCR Title:  OEM Splash Screen, Help About Logo and Application Icon
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added LoadOEMIcon method.
 * Added m_oemLargeIcon and m_oemSmallIcon.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   05 May 2000 13:54:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  134
 * SCR Title:  Restricted Operator Security level
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added OnUserChanged handler.
 * Revised OnUpdateViewApplicationBar to check privileges.
 * Revised OnUpdateViewPPQBar to check privileges.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   03 May 2000 10:15:46   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  119
 * SCR Title:  SVFocusNT Support
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Modified to accept remote commands from SVFocusNT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   Feb 01 2000 10:01:10   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  90
 * SCR Title:  Save  Operator Tool Move Priviledge in Configuration
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added message to process a tool selected for operator move.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   Jan 19 2000 15:08:16   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  89
 * SCR Title:  Outputs not set when going 'OnLine'
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added call to IO view UpdateAllViews() which calls IO view OnUpdate which 'connects' logical IO with physical IO as required after a configuration is loaded and goes 'online'.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   Jan 14 2000 15:18:50   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  85
 * SCR Title:  Version 3.04 Beta 1 versioning changes.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Update SVIODoc  related views with call to UpdateAllViews() after  configuration input 'parsing' is completed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   Jan 03 2000 14:27:10   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  77
 * SCR Title:  Automatic configuration load and go online fails.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes so an automatically loaded configuration will 'go online'.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   01 Dec 1999 17:21:48   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  68
 * SCR Title:  Defer "Go Online" processing till after configuration load is complete
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Modified OnRCLoadSEC to notify SVMainFrm that RCCommandComplete must be called when a configuration load is complete. This required adding SetNotifyComplete function to the SVMainFrmClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   Nov 30 1999 16:08:32   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  65
 * SCR Title:  Remove obsolute class SVToolSetViewClass
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Removed refernces to SVToolSetViewClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   30 Nov 1999 15:37:10   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  68
 * SCR Title:  Defer "Go Online" processing till after configuration load is complete
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Added "IsLoadingToolSet" method which indicates if a configuration is currently being loaded or not.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   Nov 30 1999 13:38:50   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  65
 * SCR Title:  Remove obsolute class SVToolSetViewClass
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Removed references to SVToolSetView and added header.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/