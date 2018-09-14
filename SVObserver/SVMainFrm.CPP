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
#include "SVSystemLibrary/SVVersionInfo.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVGlobal.h"
#include "SVIOController.h"
#include "SVIODoc.h"
#include "SVIPDoc.h"
#include "SVObjectScriptParser.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/Color.h"
#include "SVObserver.h"
#include "SVParserProgressDialog.h"
#include "SVToolSet.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "Definitions/SVUserMessage.h"
#include "SVMessage/SVMessage.h"
#include "SVInspectionProcess.h"
#include "SVPPQObject.h"
#include "SVConfigurationObject.h"
#include "SVRegressionRunDlg.h"
#include "SVDirectX.h"
#include "SVMFCControls/ModelessMessageBox.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVObjectScriptParserSVX.h"
#include "SVInspectionTreeParser.h"
#include "RemoteCommand.h"
#include "RootObject.h"
#include "SVImageViewScroll.h"
#include "SVImageView.h"
#include "SVOLicenseManager.h"
#include "SVIPChildFrm.h" 
#include "ExtrasEngine.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "ObjectInterfaces/ISVOApp_Helper.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const TCHAR* const cMatroxNoLicense = _T("No Matrox License");
static const TCHAR* const cGigeNoLicense = _T("No GigE License");
#pragma endregion Declarations

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
	ON_COMMAND(ID_EXTRAS_START_LAST_CONFIG, OnExtrasStartLastConfiguration)
	ON_UPDATE_COMMAND_UI(ID_EXTRAS_START_LAST_CONFIG, OnUpdateExtrasStartLastConfiguration)
	ON_COMMAND(ID_EXTRAS_ENABLEAUTOSAVE, OnExtrasEnableAutoSave)
	ON_UPDATE_COMMAND_UI(ID_EXTRAS_ENABLEAUTOSAVE, OnUpdateExtrasEnableAutoSave)
	ON_COMMAND(ID_EXTRAS_ENABLEDISKPROTECTION , OnExtrasEnableFbwf)
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
	ON_MESSAGE(SV_ADD_FILE_TO_CONFIG, &SVMainFrame::onAddFileToConfig)
END_MESSAGE_MAP()

#pragma region Constructor

SVMainFrame::SVMainFrame()
{
	m_oemLargeIcon = nullptr;
	m_oemSmallIcon = nullptr;

	m_hDisplayChangedEvent = ::CreateEvent( nullptr, true, false, nullptr );
}

SVMainFrame::~SVMainFrame()
{
	if( nullptr != m_hDisplayChangedEvent )
	{
		::CloseHandle( m_hDisplayChangedEvent );
		m_hDisplayChangedEvent = nullptr;
	}

	if( m_oemLargeIcon )
	{
		::DestroyIcon( m_oemLargeIcon );
	}
	
	if( m_oemSmallIcon )
	{
		::DestroyIcon( m_oemSmallIcon );
	}

	m_BackgroundBrush.DeleteObject();
}
#pragma endregion Constructor

////////////////////////////////////////////////////////////////////////////////
// Load OEM Icon
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::LoadOEMIcon()
{
	std::string OEMICON32FileName = SvStl::GlobalPath::Inst().GetBinPath(_T("OEM\\oem32x32.ico")); 
	
	if( 0 == ::_access( OEMICON32FileName.c_str(), 0 ) )
	{
		m_oemLargeIcon = (HICON)::LoadImage( nullptr, 
										OEMICON32FileName.c_str(),
										IMAGE_ICON,
										0,
										0,
										LR_DEFAULTCOLOR | LR_LOADFROMFILE );
		if( m_oemLargeIcon )
		{
			SetIcon( m_oemLargeIcon,TRUE );
		}

	}// end if

	std::string OEMICON16FileName = SvStl::GlobalPath::Inst().GetBinPath(_T("OEM\\oem16x16.ico")); 

	if( 0 == ::_access( OEMICON16FileName.c_str(), 0 ) )
	{
		m_oemSmallIcon = (HICON)::LoadImage( nullptr, 
										 OEMICON16FileName.c_str(),
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
	ASSERT( nullptr != m_hWnd );
	ASSERT( nullptr == m_hWndMDIClient );
	DWORD dwStyle = WS_VISIBLE | WS_CHILD | WS_BORDER | 
		WS_CLIPCHILDREN | WS_CLIPSIBLINGS | 
		MDIS_ALLCHILDSTYLES;    // allow children to be created invisible
	DWORD dwExStyle = 0;
	// will be inset by the frame

	dwStyle &= ~WS_BORDER;
	dwExStyle = WS_EX_CLIENTEDGE;

	WNDCLASS	windowClass;

	if (!(::GetClassInfo( AfxGetInstanceHandle(), "SVObserverMDIClient", &windowClass)))
	{

	if( !GetClassInfo( AfxGetInstanceHandle(), _T( "mdiclient" ), &windowClass ) )
		return FALSE; 

	// Use Körber Grey (RGB( 204, 204, 204 ) for the application's background.
	m_BackgroundBrush.CreateSolidBrush( SvDef::KoerberGray );
	windowClass.hbrBackground = m_BackgroundBrush.operator HBRUSH();
	windowClass.lpszClassName = _T( "SVObserverMDIClient" );

	if( !AfxRegisterClass( &windowClass ) )
		return( false );
	}
	
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
	if( nullptr == ( m_hWndMDIClient = ::CreateWindowEx( dwExStyle,
											  _T(  "SVObserverMDIClient"  ), 
											  nullptr,
											  dwStyle, 
											  0, 0, 0, 0,
											  m_hWnd,
											  ( HMENU )AFX_IDW_PANE_FIRST,
											  AfxGetInstanceHandle(),
											  ( LPVOID )&ccs ) ) )
	{
		return false;
	}
	// Move it to the top of z-order
	::BringWindowToTop( m_hWndMDIClient );

	return true;
}

//******************************************************************************
// Class Wizard Generated Virtual Function(s):
//******************************************************************************

BOOL SVMainFrame::PreCreateWindow( CREATESTRUCT& cs )
{
	return CMDIFrameWnd::PreCreateWindow( cs );
}

void SVMainFrame::SetStatusInfoText( LPCTSTR SIText )
{
	size_t l_Size = ( nullptr != SIText ) ? ::strlen( SIText ) : 0;

	if( 0 < l_Size )
	{
		CClientDC DC( nullptr );
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

void SVMainFrame::OnUpdateFrameTitle( BOOL bAddToTitle )
{
	if( 0 == ( GetStyle() & FWS_ADDTOTITLE )  )
		return;     // leave it alone!

	CMDIChildWnd* pActiveChild;
	CDocument* pDocument = GetActiveDocument();
	if ( bAddToTitle &&
		nullptr != ( pActiveChild = MDIGetActive() ) &&
		0 == ( pActiveChild->GetStyle() & WS_MAXIMIZE ) &&
		( nullptr != pDocument || 
			nullptr != ( pDocument = pActiveChild->GetActiveDocument() ) ) )
	{
		UpdateFrameTitleForDocument( pDocument->GetTitle() );
	}
	else
	{
		UpdateFrameTitleForDocument( nullptr );
	}
}

void SVMainFrame::UpdateFrameTitleForDocument( LPCTSTR lpszDocName )
{
	// Special for SVObserver
	// copy first part of title loaded at time of frame creation
	TCHAR szText[ 256 +_MAX_PATH ];

	CString l_Version = SvSyl::SVVersionInfo::GetShortTitleVersion().c_str();

	if( GetStyle() & FWS_PREFIXTITLE )
	{
		szText[ 0 ] = '\0';   // start with nothing

		// get name of currently active view
		if( nullptr != lpszDocName )
		{
			lstrcpy( szText, lpszDocName );
			// add current window # if needed
			if( m_nWindow > 0 )
			{
				wsprintf( szText + lstrlen( szText ), _T( ":%d" ), m_nWindow );
			}
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
			lstrcat( szText, TheSVObserverApp.getConfigFileName().c_str() );
		}

		// get name of currently active view
		if( nullptr != lpszDocName )
		{
			lstrcat( szText, _T( " - " ) );
			lstrcat( szText, lpszDocName );
			// add current window # if needed
			if( m_nWindow > 0 )
			{
				wsprintf( szText + lstrlen( szText ), _T( ":%d" ), m_nWindow );
			}
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

BOOL SVMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	return CMDIFrameWnd::OnCreateClient(lpcs, pContext);
	// This function call creates a legal CMenu pointer and calls the overriden
	// SVMainFrame::CreateClient function to perform the MDIClient window
}

int SVMainFrame::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if( CMDIFrameWnd::OnCreate( lpCreateStruct ) == -1 )
	{
		return -1;
	}

	if( !m_wndToolBar.Create( this ) || !m_wndToolBar.LoadToolBar( IDR_MAINFRAME ) )
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

	InitToolBars();

	LoadOEMIcon();

	m_bSizeChanged = FALSE;
	m_PosSize = SIZE_MAXIMIZED;

	UpdateFrameTitleForDocument( nullptr );

	return 0;
}

void SVMainFrame::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CMDIFrameWnd::OnLButtonDown(nFlags, point);
}

void SVMainFrame::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CMDIFrameWnd::OnRButtonDown(nFlags, point);
}

void SVMainFrame::OnSetFocus(CWnd* pOldWnd) 
{
	CMDIFrameWnd::OnSetFocus(pOldWnd);
	
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen
}

void SVMainFrame::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen

	CMDIFrameWnd::OnLButtonDblClk(nFlags, point);
}

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
		if( S_OK == TheSVObserverApp.m_svSecurityMgr.SVValidate( SECURITY_POINT_FILE_MENU_EXIT, 
			SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE ) )
		{
			TheSVObserverApp.OnStop();
			l_bAllowAccess = true;
		}
	}
	else if( S_OK == TheSVObserverApp.m_svSecurityMgr.SVValidate( SECURITY_POINT_FILE_MENU_EXIT ) )
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
			BSTR l_bstUser = nullptr;
			CString l_strUser = _T("User: ");
			if( TheSVObserverApp.m_svSecurityMgr.SVGetUseLogon() )
			{
				HRESULT l_hr = TheSVObserverApp.m_svSecurityMgr.SVGetUser( l_bstUser );
				if( nullptr != l_bstUser )
				{
					l_strUser += l_bstUser;
					::SysFreeString( l_bstUser );
				}
			}
			pCmdUI->Enable();

			CClientDC DC( nullptr );
			int l_Width = DC.GetTextExtent( l_strUser ).cx - ( l_strUser.GetLength() * 2 );

			m_wndStatusBar.SetPaneInfo( pCmdUI->m_nIndex, ID_INDICATOR_USER, 0, l_Width );
			pCmdUI->SetText( l_strUser );

			break;
		}
		case ID_INDICATOR_MODE:  // Status Bar Mode
		{
			std::string Mode = _T("Mode: ");
			unsigned long l_lMode = 0;
			GlobalRCGetMode( &l_lMode );
			switch( l_lMode )
			{
				case SVIM_MODE_UNKNOWN:
				{
					m_wndStatusBar.m_lModeBKColor = SvDef::KoerberGray;
					m_wndStatusBar.m_lModeForeColor = SvDef::Black;
					break;
				}
				case SVIM_MODE_ONLINE:
				{
					m_wndStatusBar.m_lModeBKColor = RGB( 100, 255, 100 );
					m_wndStatusBar.m_lModeForeColor = SvDef::Black;
					Mode += _T("Run ");
					break;
				}
				case SVIM_MODE_OFFLINE:
				{
					m_wndStatusBar.m_lModeBKColor = RGB( 255, 128, 128 );
					m_wndStatusBar.m_lModeForeColor = SvDef::Black;
					Mode += _T("Stop ");
					break;
				}
				case SVIM_MODE_REGRESSION:
				{
					m_wndStatusBar.m_lModeBKColor = SvDef::Fuchsia;
					m_wndStatusBar.m_lModeForeColor = SvDef::Black;
					Mode += _T("Regression ");
					break;
				}
				case SVIM_MODE_TEST:
				{
					m_wndStatusBar.m_lModeBKColor = SvDef::DarkBlue;
					m_wndStatusBar.m_lModeForeColor = SvDef::White;
					Mode += _T("Test ");
					break;
				}
				case SVIM_MODE_EDIT:
				{
					m_wndStatusBar.m_lModeBKColor = SvDef::PaleYellow;
					m_wndStatusBar.m_lModeForeColor = SvDef::Black;
					Mode += _T("Edit ");
					break;
				}
				case SVIM_MODE_CHANGING:
				{
					m_wndStatusBar.m_lModeBKColor = SvDef::White;
					m_wndStatusBar.m_lModeForeColor = SvDef::Black;
					Mode += _T("Changing ");
					break;
				}
				default:
				{
					Mode += _T("Undefined");
				}
			}

			m_wndStatusBar.m_strModeText = Mode.c_str();

			pCmdUI->Enable();

			CClientDC DC( nullptr );
			int l_Width = DC.GetTextExtent( Mode.c_str() ).cx - static_cast<int> ( Mode.size() * 2 );

			m_wndStatusBar.SetPaneInfo( pCmdUI->m_nIndex, ID_INDICATOR_MODE, 0, l_Width );
			pCtrl->SetText( Mode.c_str(), pCmdUI->m_nIndex, SBT_OWNERDRAW );

			break;
		}
		case ID_INDICATOR_GIGE_LICENSE_ERROR:
		{
			bool l_bMatroxError = false;
			bool l_bGigeError = false;
			std::string Error;

			if ( !TheSVOLicenseManager().HasMatroxLicense() )
			{
				l_bMatroxError = true;
				Error += cMatroxNoLicense;
			}
			else
			{
				if ( !TheSVOLicenseManager().HasMatroxGigELicense() && TheSVObserverApp.IsMatroxGige() )
				{
					Error = cGigeNoLicense;
				}
			}

			if ( (l_bMatroxError) || (l_bGigeError) )
			{
				m_wndStatusBar.m_strErrorText = Error.c_str();

				pCmdUI->Enable();

				if (l_bMatroxError) 
				{
					m_wndStatusBar.m_lErrorBKColor = SvDef::LightRed;
					m_wndStatusBar.m_lErrorForeColor = SvDef::Black;
				}
				else
				{
					m_wndStatusBar.m_lErrorBKColor = SvDef::KoerberGray;
					m_wndStatusBar.m_lErrorForeColor = SvDef::Black;
				}

				CClientDC DC(nullptr);
				int lWidth = DC.GetTextExtent( Error.c_str() ).cx - static_cast<int> ( Error.size() *2 ) + 20;
				m_wndStatusBar.SetPaneInfo( pCmdUI->m_nIndex, ID_INDICATOR_GIGE_LICENSE_ERROR, 0, lWidth );
				pCtrl->SetText( Error.c_str(), pCmdUI->m_nIndex, SBT_OWNERDRAW );

			}
			else
			{
				m_wndStatusBar.SetPaneInfo( pCmdUI->m_nIndex,ID_INDICATOR_GIGE_LICENSE_ERROR,SBPS_DISABLED ,0);
				pCtrl->SetText( Error.c_str(), pCmdUI->m_nIndex, SBT_OWNERDRAW );
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

LRESULT SVMainFrame::onAddFileToConfig(WPARAM wParam, LPARAM lParam)
{
	LPCTSTR pFileName = reinterpret_cast<LPCTSTR> (lParam);
	TheSVObserverApp.AddAdditionalFile(pFileName);
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

void SVMainFrame::OnMouseMove( UINT nFlags, CPoint point )
{
	SetStatusInfoText( nullptr );
	
	CMDIFrameWnd::OnMouseMove( nFlags, point );
}

void SVMainFrame::OnViewPPQBar()
{
	bool accessAllowed = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_VIEW_MENU_PPQ_BAR);

	if (accessAllowed && !m_PPQBar.IsWindowVisible())
	{
		ShowControlBar( &m_PPQBar, true, false );
		m_PPQBar.BuildButtons();
	}
	else
	{
		// Hide bar...
		ShowControlBar( &m_PPQBar, false, false );
	}

	RecalcLayout();
}

void SVMainFrame::OnViewToolBar()
{
	if( !m_wndToolBar.IsWindowVisible() )
	{
		ShowControlBar( &m_wndToolBar, true, false );
	}
	else
	{
		// Hide bar...
		ShowControlBar( &m_wndToolBar, false, false );
	}

	RecalcLayout();
}

void SVMainFrame::OnToolbarZoom()
{
	if( !m_wndToolBarZoom.IsWindowVisible() )
	{
		ShowControlBar( &m_wndToolBarZoom, true, false );
	}
	else
	{
		// Hide bar...
		ShowControlBar( &m_wndToolBarZoom, false, false );
	}

	RecalcLayout();
}

void SVMainFrame::OnUpdateViewPPQBar( CCmdUI* pCmdUI )
{
	if( m_PPQBar.IsWindowVisible() )
	{
		pCmdUI->SetCheck( 1 );
	}
	else
	{
		pCmdUI->SetCheck( 0 );
	}
}

void SVMainFrame::OnUpdateViewToolbar( CCmdUI* pCmdUI )
{
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

	if( SvXml::SVNavigateTree::GetItemBranch( p_rTree, SvXml::CTAG_INSPECTION, nullptr, htiChild ) )
	{
		// Set the Caption
		CString title = _T( "Loading Toolset(s) ..." );
		SVParserProgressDialog l_ParserProgressDialog( title, this );

		SVTreeType::SVBranchHandle htiSubChild( p_rTree.getFirstBranch( htiChild ) );
		;
		while( S_OK == l_Status && nullptr != htiSubChild )
		{
			std::shared_ptr<std::string> pToolsetScript{ new std::string };
			SVGUID ownerGuid;

			SVInspectionProcess* pInspect( nullptr );
			SVTreeType::SVBranchHandle htiSVIPDoc = nullptr;
			SVTreeType::SVBranchHandle htiDataChild( p_rTree.getFirstBranch( htiSubChild ) );

			while( nullptr != htiDataChild )
			{
				std::string DataName = p_rTree.getBranchName( htiDataChild );

				if ( 0 == DataName.compare( SvXml::CTAG_INSPECTION_PROCESS ) )
				{
					if( SvXml::SVNavigateTree::GetItemBranch( p_rTree, SvXml::CTAG_TOOLSET_SET, htiDataChild, hItemToolset ) )
					{
						_variant_t svVariant;

						if( SvXml::SVNavigateTree::GetItem( p_rTree, SvXml::CTAG_UNIQUE_REFERENCE_ID, htiDataChild, svVariant ) )
						{
							ownerGuid = svVariant;
						}
					}
				}
				else if ( 0 == DataName.compare( SvXml::CTAG_SVIPDOC ) )
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
					if( SvXml::SVNavigateTree::GetItem( p_rTree, SvXml::CTAG_UNIQUE_REFERENCE_ID, htiSVIPDoc, svVariant ) )
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
					if( SvXml::SVNavigateTree::GetItem( p_rTree, SvXml::CTAG_TOOLSET_SCRIPT, htiSVIPDoc, svVariant ) )
					{
						std::string NewString = SvUl::LoadStdString( IDS_OBJECTNAME_PAT_DELTAPOS );

						std::string ToolsetScript = SvUl::createStdString( svVariant );

						// Pre-Parse Typo fix.
						// This peice of code fixes the Angel to Angle typo.
						SvUl::searchAndReplace( ToolsetScript, _T("Axis Secondary Angel"), _T("Axis Secondary Angle") );

						// Pre-Parse Typo fix number two.
						// This peice of code fixes the Principle to Principal typo.
						SvUl::searchAndReplace( ToolsetScript, _T("Axis Principle Angle"), _T("Axis Principal Angle") );

						// Pre-Parse Type fix number three.
						// This piece of code changes "Angle delta Positive" to "Angle Delta Positive".
						SvUl::searchAndReplace( ToolsetScript, _T("Angle delta Positive"), NewString.c_str() );

						*pToolsetScript = ToolsetScript;
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
BOOL SVMainFrame::DestroyPPQButtons()
{
	return m_PPQBar.DestroyButtons();
}

///////////////////////////////////////////////////////
// handler for End Object Script Parsing message
///////////////////////////////////////////////////////
LRESULT SVMainFrame::OnEndObjectScriptParse(WPARAM wParam, LPARAM lParam)
{
	BOOL rc = true;
	return rc;
}

///////////////////////////////////////////////////////
// handler for End Object Script Parsing message
///////////////////////////////////////////////////////
LRESULT SVMainFrame::OnEndObjectCreation(WPARAM wParam, LPARAM lParam)
{
	BOOL rc = true;
	return rc;
}

void SVMainFrame::OnConfigurationFinishedInitializing()
{
	// Finally indicate to the 'world' that the configuration is loaded
	// and ready to run or test.
	//
	// All operations that need to happen after the configuration is fully
	// loaded and initialized should go here

	// Update the IO views. 

	SVIODoc* l_pIODoc( TheSVObserverApp.GetIODoc() );

	if( nullptr != l_pIODoc )
	{
		SVIOController* l_pIOController( l_pIODoc->GetIOController() );

		if( nullptr != l_pIOController )
		{
			l_pIOController->RebuildOutputList();
		}

		l_pIODoc->UpdateAllViews( nullptr );
	}// end if

	TheSVObserverApp.UpdateAllMenuExtrasUtilities();

	// Set the tool selected for an operator to move if any.
	PostMessage( SV_SET_TOOL_SELECTED_IN_TOOL_VIEW, (WPARAM) TRUE );

	// Indicate to the world that a configuration is loaded, parsed,
	// and is ready to run.
	SVSVIMStateClass::AddState( SV_STATE_READY );
	SVSVIMStateClass::RemoveState( SV_STATE_UNAVAILABLE | SV_STATE_LOADING );

	//
	// If the configuration is required to start immediately after loading
	// or a GoOnline command was received from SVFocusNT,
	// then start the toolset, i.e. go 'online'.
	//
	bool StartLastConfiguration(false);
	RootObject::getRootChildValue(SvDef::FqnEnvironmentStartLastConfig, StartLastConfiguration);
	if ( StartLastConfiguration || SVSVIMStateClass::CheckState( SV_STATE_START_PENDING ) )
	{
		PostMessage ( WM_COMMAND, MAKEWPARAM (ID_MODE_RUN, 0), 0 );
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

#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
				if (hr != DD_OK)
				{
					TRACE(_T( "SVMainFrm::OnActivateApp - RestoreAllSurfaces failed\n" ) );
				}
#endif
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
					while (nullptr != pMDIChild)
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
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
			else
			{
				TRACE(_T( "SVMainFrm::OnDisplayChange - SVDirectX.Initialize() failed\n" ) );
			}
#endif
		}
		// restore surfaces
		else
		{
			hr = SVDirectX::Instance().RestoreAllSurfaces();

#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
			if (hr != DD_OK)
			{
				TRACE(_T( "SVMainFrm::OnDisplayChange - RestoreAllSurfaces failed\n" ) );
			}
#endif
		}

		// if pixel depth changed - show mesage...
		if ( p_p1 / 8 != l_PixelDepth )
		{
			if( WAIT_TIMEOUT == WaitForSingleObject( m_hDisplayChangedEvent, 0 ) )
			{
				std::string Message = SvUl::Format( _T("\n\n\nThe display color settings on the system have changed.\n"
					"Images may not display correctly until this configuration is reopened.") );
				HANDLE hMessage = ::CreateEvent( nullptr, true, false, nullptr );
				::SetEvent( hMessage );
				SvOi::CallModelessMessageBox( Message, hMessage );
				::CloseHandle(hMessage);
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

#ifdef _DEBUG
void SVMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void SVMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

LRESULT SVMainFrame::OnUserChanged( WPARAM wParam, LPARAM lParam )
{
	bool accessAllowed = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_VIEW_MENU_PPQ_BAR ) ;  // ID_VIEW_PPQ_BAR );

	if( !accessAllowed )
	{
		// Hide PPQ Bar
		if( m_PPQBar.IsWindowVisible() )
		{
			ShowControlBar(&m_PPQBar, false, false);
			RecalcLayout();
		}
	}

	return static_cast<LRESULT> (true);
}

void SVMainFrame::OnViewImageDisplayUpdate()
{
	if( S_OK == TheSVObserverApp.m_svSecurityMgr.SVValidate( SECURITY_POINT_VIEW_MENU_IMAGE_DISPLAY_UPDATE ) )
	{
		SVSVIMStateClass::AddState( SV_STATE_MODIFIED );

		bool ImageUpdate = false;
		RootObject::getRootChildValue( SvDef::FqnEnvironmentImageUpdate, ImageUpdate );
		RootObject::setRootChildValue( SvDef::FqnEnvironmentImageUpdate, !ImageUpdate );
	}
}

void SVMainFrame::OnUpdateViewImageDisplayUpdate( CCmdUI* pCmdUI )
{
	BOOL bEnable = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_VIEW_MENU_IMAGE_DISPLAY_UPDATE );
	bEnable &= !SVSVIMStateClass::CheckState( SV_STATE_REGRESSION );
	pCmdUI->Enable( bEnable);
	bool ImageUpdate( false );
	RootObject::getRootChildValue( SvDef::FqnEnvironmentImageUpdate, ImageUpdate );
	pCmdUI->SetCheck( ImageUpdate );
}

void SVMainFrame::OnViewResultDisplayUpdate()
{
	if( S_OK == TheSVObserverApp.m_svSecurityMgr.SVValidate( SECURITY_POINT_VIEW_MENU_RESULT_DISPLAY_UPDATE ) )
	{
		SVSVIMStateClass::AddState( SV_STATE_MODIFIED );

		bool ResultOnline = false;
		RootObject::getRootChildValue( SvDef::FqnEnvironmentResultUpdate, ResultOnline );
		RootObject::setRootChildValue( SvDef::FqnEnvironmentResultUpdate, !ResultOnline );
	}
}

void SVMainFrame::OnUpdateViewResultDisplayUpdate( CCmdUI* pCmdUI )
{
	BOOL bEnable = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_VIEW_MENU_RESULT_DISPLAY_UPDATE );
	bEnable &= !SVSVIMStateClass::CheckState( SV_STATE_REGRESSION );
	pCmdUI->Enable( bEnable);

	bool ResultOnline( false );
	RootObject::getRootChildValue( SvDef::FqnEnvironmentResultUpdate, ResultOnline );
	pCmdUI->SetCheck( ResultOnline );
}

void SVMainFrame::OnExtrasStartLastConfiguration()
{
	SVSVIMStateClass::AddState(SV_STATE_EDITING); /// do this before calling validate for security as it may display a logon dialog!
	if (S_OK == TheSVObserverApp.m_svSecurityMgr.SVValidate(SECURITY_POINT_EXTRAS_MENU_ADDITIONAL_ENVIRON))
	{
		bool StartLastConfiguration(false);
		RootObject::getRootChildValue(SvDef::FqnEnvironmentStartLastConfig, StartLastConfiguration);
		StartLastConfiguration = !StartLastConfiguration;
		RootObject::setRootChildValue(SvDef::FqnEnvironmentStartLastConfig, StartLastConfiguration);
	}
	SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
}

void SVMainFrame::OnUpdateExtrasStartLastConfiguration(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION)
				   && TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_EXTRAS_MENU_ADDITIONAL_ENVIRON) 
				   && (TheSVOLicenseManager().HasMatroxLicense()));

	bool StartLastConfiguration(false);
	RootObject::getRootChildValue(SvDef::FqnEnvironmentStartLastConfig, StartLastConfiguration);
	pCmdUI->SetCheck(StartLastConfiguration);
}

void SVMainFrame::OnExtrasEnableAutoSave()
{
	if (S_OK != TheSVObserverApp.m_svSecurityMgr.SVValidate(SECURITY_POINT_EXTRAS_MENU_AUTOSAVE_CONFIGURATION))
	{
		return; //Arvid: not validated:do nothing
	}

	ExtrasEngine::Instance().ToggleEnableAutoSave();
}

void SVMainFrame::OnUpdateExtrasEnableAutoSave(CCmdUI *pCmdUI)
{
	BOOL bEnable = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_EXTRAS_MENU_AUTOSAVE_CONFIGURATION );

	pCmdUI->Enable( bEnable );
	pCmdUI->SetCheck( ExtrasEngine::Instance().IsAutoSaveEnabled());
}

void SVMainFrame::OnExtrasEnableFbwf()
{
	if( S_OK == TheSVObserverApp.m_svSecurityMgr.SVValidate(SECURITY_POINT_EXTRAS_MENU_FBWF_CONFIGURATION) )
	{
		bool Value {false};
		RootObject::getRootChildValue(SvDef::FqnEnvironmentDiskProtection, Value);
		RootObject::setRootChildValue(SvDef::FqnEnvironmentDiskProtection, !Value);
	}
}

void SVMainFrame::OnUpdateExtrasEnableFbwf(CCmdUI *pCmdUI)
{
	BOOL bEnable = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_EXTRAS_MENU_FBWF_CONFIGURATION );

	pCmdUI->Enable(false); //Arvid temporarily disabled as per SVO-1084. Parameter was: (ExtrasEngine::Instance().IsFbwfAvailable()==true)

	pCmdUI->SetCheck( ExtrasEngine::Instance().IsFbwfSelected());
}

BOOL SVMainFrame::VerifyBarStates(LPCTSTR lpszProfileName)
{
	CDockState state;
	state.LoadState(lpszProfileName);

	for (int i = 0; i < state.m_arrBarInfo.GetSize(); i++)
	{
		CControlBarInfo* pInfo = (CControlBarInfo*)state.m_arrBarInfo[i];

		ASSERT(nullptr != pInfo);

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

				if (nullptr == GetControlBar(nID))
				{
					return false;
				}
			}
		}
		
		if (!pInfo->m_bFloating) // floating dockbars can be created later
		{
			if (nullptr == GetControlBar(pInfo->m_nBarID))
			{
				return false; // invalid bar ID
			}
		}
		else
		{
			if(nullptr == dynamic_cast<CDockBar*>(pInfo->m_pBar))
			{
				return false;
			}
		}
	}

	return true;
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

void SVMainFrame::SetZoomToolbar(const ZoomHelper& rZoomhelper)
{
	m_wndToolBarZoom.SetRange(rZoomhelper.GetZoomMin(), rZoomhelper.GetZoomMax(), rZoomhelper.GetZoomStep());
	m_wndToolBarZoom.SetValue(rZoomhelper.GetZoom());
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

void SVMainFrame::InitToolBars()
{
	ShowControlBar(&m_wndToolBar, false, false);
	ShowControlBar(&m_wndToolBarZoom, false, false);
	ShowControlBar(&m_PPQBar, false, false);
	ShowControlBar(&m_wndStatusBar, true, false);
}

