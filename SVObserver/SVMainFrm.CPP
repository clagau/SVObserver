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
#include "ObjectInterfaces/SVUserMessage.h"
#include "SVMessage/SVMessage.h"
#include "SVInspectionProcess.h"
#include "SVPPQObject.h"
#include "SVConfigurationObject.h"
#include "SVRegressionRunDlg.h"
#include "SVDirectX.h"
#include "ModelessMessageBox.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVObjectScriptParserSVX.h"	// new speedy
#include "SVInspectionTreeParser.h"		// new Inspection XML (NOT object script)
#include "RemoteCommand.h"
#include "RootObject.h"
#include "SVImageViewScroll.h"
#include "SVImageView.h"
#include "SVOLicenseManager/SVOLicenseManager.h"
#include "SVIPChildFrm.h" 
#include "ExtrasEngine.h"

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
	ON_COMMAND(ID_EXTRAS_ENABLEDISKPROTECTION , OnExtrasEnableFbwf)
	

	ON_UPDATE_COMMAND_UI(ID_EXTRAS_ENABLEAUTOSAVE, OnUpdateExtrasEnableAutoSave)
	ON_UPDATE_COMMAND_UI(ID_EXTRAS_ENABLEDISKPROTECTION, OnUpdateExtrasEnableFbwf)
	
	ON_WM_SIZE()
	ON_WM_SIZING()
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_INFO, OnUpdateStatusInfo)
	// Status Bar User information
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_USER, OnUpdateStatusInfo)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_MODE, OnUpdateStatusInfo)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_GIGE_LICENSE_ERROR, OnUpdateStatusInfo)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_FBWF, OnUpdateStatusInfo)

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
		case ID_INDICATOR_FBWF:
			{
				auto &ai(ExtrasEngine::Instance());
				
				m_wndStatusBar.SetFbwfInfo(pCmdUI->m_nIndex, ai.IsFbwfAvailable(), ai.IsFbwfActive(), ai.IsFbwfChanging());

				pCtrl->SetText( m_wndStatusBar.m_strFbwfText, pCmdUI->m_nIndex, SBT_OWNERDRAW );
			}
			break;

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

	if( SVNavigateTree::GetItemBranch( p_rTree, CTAG_INSPECTION, nullptr, htiChild ) )
	{
		// Set the Caption
		CString title = _T( "Loading Toolset(s) ..." );
		SVParserProgressDialog l_ParserProgressDialog( title, this );

		SVTreeType::SVBranchHandle htiSubChild( p_rTree.getFirstBranch( htiChild ) );
		;
		while( l_Status == S_OK && nullptr != htiSubChild )
		{
			SVSharedPtr<CString> pToolsetScript = new CString;
			SVGUID ownerGuid;

			SVInspectionProcess* pInspect( nullptr );
			SVTreeType::SVBranchHandle htiSVIPDoc = nullptr;
			SVTreeType::SVBranchHandle htiDataChild( p_rTree.getFirstBranch( htiSubChild ) );
			;

			while( nullptr != htiDataChild )
			{
				SVString DataName = p_rTree.getBranchName( htiDataChild );

				if ( DataName.compare( CTAG_INSPECTION_PROCESS ) == 0 )
				{
					if( SVNavigateTree::GetItemBranch( p_rTree, CTAG_TOOLSET_SET, htiDataChild, hItemToolset ) )
					{
						_variant_t svVariant;

						if( SVNavigateTree::GetItem( p_rTree, CTAG_UNIQUE_REFERENCE_ID, htiDataChild, svVariant ) )
						{
							ownerGuid = svVariant;
						}
					}
				}
				else if ( DataName.compare( CTAG_SVIPDOC ) == 0 )
				{
					htiSVIPDoc = htiDataChild;
				}

				htiDataChild = p_rTree.getNextBranch( htiSubChild, htiDataChild );
			}

			if( nullptr != htiSVIPDoc )
			{
				_variant_t svVariant;

				if( ownerGuid.empty() )
				{
					if( SVNavigateTree::GetItem( p_rTree, CTAG_UNIQUE_REFERENCE_ID, htiSVIPDoc, svVariant ) )
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
					if( SVNavigateTree::GetItem( p_rTree, CTAG_TOOLSET_SCRIPT, htiSVIPDoc, svVariant ) )
					{
						CString newString;
						newString.LoadString(IDS_OBJECTNAME_PAT_DELTAPOS);

						SVString l_String = SvUl_SF::createSVString( svVariant );

						// Pre-Parse Typo fix.
						// This peice of code fixes the Angel to Angle typo.
						SvUl_SF::searchAndReplace( l_String, _T("Axis Secondary Angel"), _T("Axis Secondary Angle") );

						// Pre-Parse Typo fix number two.
						// This peice of code fixes the Principle to Principal typo.
						SvUl_SF::searchAndReplace( l_String, _T("Axis Principle Angle"), _T("Axis Principal Angle") );

						// Pre-Parse Type fix number three.
						// This piece of code changes "Angle delta Positive" to "Angle Delta Positive".
						SvUl_SF::searchAndReplace( l_String, _T("Angle delta Positive"), static_cast< LPCTSTR >( newString ) );

						*pToolsetScript = l_String.c_str();
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

			SVConfigurationObject* pConfig( nullptr );
			SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

			if( S_OK == l_Status && nullptr != pConfig )
			{
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
			htiSubChild = p_rTree.getNextBranch( htiChild, htiSubChild );
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

		bool ImageUpdate = false;
		RootObject::getRootChildValue( ::EnvironmentImageUpdate, ImageUpdate );
		RootObject::setRootChildValue( ::EnvironmentImageUpdate, !ImageUpdate );
	}
}

void SVMainFrame::OnUpdateViewImageDisplayUpdate( CCmdUI* pCmdUI )
{
	BOOL bEnable = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_VIEW_MENU_IMAGE_DISPLAY_UPDATE );
	bEnable &= !SVSVIMStateClass::CheckState( SV_STATE_REGRESSION );
	pCmdUI->Enable( bEnable);
	bool ImageUpdate( false );
	RootObject::getRootChildValue( ::EnvironmentImageUpdate, ImageUpdate );
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

		bool ResultOnline = false;
		RootObject::getRootChildValue( ::EnvironmentResultUpdate, ResultOnline );
		RootObject::setRootChildValue( ::EnvironmentResultUpdate, !ResultOnline );
	}
}

void SVMainFrame::OnUpdateViewResultDisplayUpdate( CCmdUI* pCmdUI )
{
	BOOL bEnable = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_VIEW_MENU_RESULT_DISPLAY_UPDATE );
	bEnable &= !SVSVIMStateClass::CheckState( SV_STATE_REGRESSION );
	pCmdUI->Enable( bEnable);

	bool ResultOnline( false );
	RootObject::getRootChildValue( ::EnvironmentResultUpdate, ResultOnline );
	pCmdUI->SetCheck( ResultOnline );
}

void SVMainFrame::OnExtrasEnableAutoSave()
{
	if( TheSVObserverApp.m_svSecurityMgr.SVValidate(
		SECURITY_POINT_EXTRAS_MENU_AUTOSAVE_CONFIGURATION) != S_OK )
	{
		return; //Arvid: not validated:do nothing
	}

	ExtrasEngine::Instance().ToggleEnable();
}

void SVMainFrame::OnUpdateExtrasEnableAutoSave(CCmdUI *pCmdUI)
{
	BOOL bEnable = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_EXTRAS_MENU_AUTOSAVE_CONFIGURATION );

	pCmdUI->Enable( bEnable );
	pCmdUI->SetCheck( ExtrasEngine::Instance().IsEnabled()==TRUE );
}


void SVMainFrame::OnExtrasEnableFbwf()
{
	if( TheSVObserverApp.m_svSecurityMgr.SVValidate(
		SECURITY_POINT_EXTRAS_MENU_FBWF_CONFIGURATION) != S_OK )
	{
		return; //Arvid: not validated:do nothing
	}

	ExtrasEngine::Instance().ToggleEnableFbwf();
}


void SVMainFrame::OnUpdateExtrasEnableFbwf(CCmdUI *pCmdUI)
{
	BOOL bEnable = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_EXTRAS_MENU_FBWF_CONFIGURATION );

	pCmdUI->Enable( bEnable );

	pCmdUI->Enable(ExtrasEngine::Instance().IsFbwfAvailable()==true);

	pCmdUI->SetCheck( ExtrasEngine::Instance().IsFbwfSelected()==true);
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

