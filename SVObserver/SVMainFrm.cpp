//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "ConfigurationOuttakes.h"
#include "ExtrasEngine.h"
#include "GuiHelpers.h"
#include "RemoteCommand.h"
#include "RootObject.h"
#include "SVAboutDialogClass.h"
#include "SVConfigurationObject.h"
#include "SVConfigurationPrint.h"
#include "SVIOController.h"
#include "SVIODoc.h"
#include "SVIOTabbedView.h"
#include "SVIPDoc.h"
#include "SVIPSplitterFrame.h" 
#include "SVImageView.h"
#include "SVImageViewScroll.h"
#include "SVInspectionProcess.h"
#include "SVMainFrm.h"
#include "SVObserverOuttakes.h"
#include "SVObserver.h"
#include "SVSecurity/SVSecurityManager.h"
#include "SVPPQObject.h"
#include "SVRegressionRunDlg.h"
#include "SVUtilities.h"
#include "SoftwareTriggerDlg.h"
#include "Definitions/Color.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/SVUserMessage.h"
#include "ObjectInterfaces/ISVOApp_Helper.h"
#include "SVIOLibrary/SVInputObjectList.h"
#include "SVMFCControls/ModelessMessageBox.h"
#include "SVMatroxLibrary/SVOLicenseManager.h"
#include "SVMessage/SVMessage.h"
#include "SVOGui/AuditFilesDialog.h"
#include "SVOGui/SVDirectX.h"
#include "SVOGuiUtility/SVGlobal.h"
#include "SVOGui/SVDirectX.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVSharedMemoryLibrary/EditLock.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/SvimState.h"
#include "SVSystemLibrary/SVVersionInfo.h"
#pragma endregion Includes


#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr const char* cMatroxNoLicense = _T("No Matrox License");
constexpr const char* cGigeNoLicense = _T("No GigE License");
#pragma endregion Declarations



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

	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, OnFileNewConfig)
	ON_COMMAND(ID_FILE_OPEN_SVC, OnFileOpenSVC)
	ON_COMMAND(ID_FILE_CLOSE_SVC, OnFileCloseConfig)
	ON_COMMAND(ID_FILE_SAVE_ALL, OnFileSaveAll)
	ON_COMMAND(ID_FILE_SAVE_AS_SVC, OnFileSaveAsSVC)

	ON_COMMAND(ID_EDIT_EDITENVIRONMENT, OnEditEnvironment)
	ON_COMMAND(ID_EDIT_EDITIOLIST, OnEditIOList)

	ON_COMMAND(ID_MODE_TEST, OnTestMode)
	ON_COMMAND(ID_MODE_RUN, OnRunMode)
	ON_COMMAND(ID_MODE_STOPTEST, OnStopTestMode)
	ON_COMMAND(ID_MODE_EDIT, OnModeEdit)
	ON_COMMAND(ID_GO_OFFLINE, OnGoOffline)
	ON_COMMAND(ID_GO_ONLINE, OnGoOnline)
	ON_COMMAND(ID_STOP, OnStop)

	ON_COMMAND(ID_FILE_PRINT_CONFIG, OnFilePrintConfig)

	ON_COMMAND(ID_EXTRAS_LOGOUT, OnExtrasLogout)
	ON_COMMAND(ID_EXTRAS_LOGIN, OnExtrasLogin)
	ON_COMMAND(ID_EXTRAS_UTILITIES_EDIT, OnExtrasUtilitiesEdit)
	ON_COMMAND(ID_EXTRAS_SECURITY_SETUP, OnExtrasSecuritySetup)
	ON_COMMAND(ID_AUDITTRAIL_DEFAULTFILES, OnAuditTrailDefaultFiles)
	ON_COMMAND(ID_AUDITTRAIL_ADDITIONALFILES, OnAuditTrailAdditionalFiles)
	ON_COMMAND_RANGE(ID_EXTRAS_UTILITIES_BASE, ID_EXTRAS_UTILITIES_LIMIT, OnRunUtility)

	ON_COMMAND(ID_RC_GO_OFFLINE, OnRCGoOffline)
	ON_COMMAND(ID_RC_GO_ONLINE, OnRCGoOnline)
	ON_COMMAND(ID_RC_OPEN_CURRENT_SVX, OnRCOpenCurrentSVX)
	ON_COMMAND(ID_MEM_LEAK_DETECTION, OnMemLeakDetection)

	ON_COMMAND(ID_UPDATE_ALL_IOVIEWS, OnUpdateAllIOViews)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_TRIGGER_SETTINGS, OnTriggerSettings)

	ON_COMMAND(SV_AUTO_RUN_LAST_MRU, OnRunMostRecentMRU)
	ON_COMMAND(ID_EDIT_ADD_MONITORLIST, OnEditMonitorList)

	ON_COMMAND(ID_FILE_PRINT_SETUP, OnSVOFilePrintSetup)
	ON_COMMAND(ID_RC_CLOSE, OnRCClose)

	ON_UPDATE_COMMAND_UI(ID_TRIGGER_SETTINGS, OnUpdateTriggerSettings)
	ON_UPDATE_COMMAND_UI(ID_MODE_RUN, OnUpdateModeRun)

	ON_UPDATE_COMMAND_UI(ID_MODE_TEST, OnUpdateModeTest)
	ON_UPDATE_COMMAND_UI(ID_MODE_EDIT, OnUpdateModeEdit)
	ON_UPDATE_COMMAND_UI(ID_GO_OFFLINE, OnUpdateGoOffline)
	ON_UPDATE_COMMAND_UI(ID_GO_ONLINE, OnUpdateGoOnline)
	ON_UPDATE_COMMAND_UI(ID_RUN_REGRESSIONTEST, OnUpdateRegressionTest)

	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE_SVC, OnUpdateFileClose)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, OnUpdateFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN_SVC, OnUpdateFileOpenSVC)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_ALL, OnUpdateFileSaveAll)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS_SVC, OnUpdateFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_COPY_AS, OnUpdateFileSaveCopyAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_IMAGE, OnUpdateFileSaveImage)
	ON_UPDATE_COMMAND_UI(ID_FILE_UPDATE, OnUpdateFileUpdate)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_CONFIG, OnUpdateFilePrintConfig)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, OnUpdateFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_SETUP, OnUpdateFilePrintSetup)
	ON_UPDATE_COMMAND_UI(ID_EDIT_EDITIOLIST, OnUpdateEditIOList)
	ON_UPDATE_COMMAND_UI(ID_EDIT_EDITENVIRONMENT, OnUpdateEditEnvironment)
	ON_UPDATE_COMMAND_UI(ID_EDIT_EDITTOOL, OnUpdateEditEditTool)
	ON_UPDATE_COMMAND_UI(ID_EDIT_EDITTOOLSET, OnUpdateEditEditToolSet)
	ON_UPDATE_COMMAND_UI(ID_RESULTS_PICKER, OnUpdateResultsPicker)
	ON_UPDATE_COMMAND_UI(ID_RESULTS_TABLE_PICKER, OnUpdateResultsPicker)
	ON_UPDATE_COMMAND_UI(ID_SAVE_RESULTS_TO_FILE, OnUpdateResultsPicker)
	ON_UPDATE_COMMAND_UI(ID_SAVE_RESULTSTABLE_TO_FILE, OnUpdateResultsPicker)
	ON_UPDATE_COMMAND_UI(ID_EDIT_EDITREMOTEINPUTS, OnUpdateEditRemoteInputs)

	ON_UPDATE_COMMAND_UI(ID_EXTRAS_LIGHTREFERENCE, OnUpdateExtrasLightReference)
	ON_UPDATE_COMMAND_UI(ID_EXTRAS_TESTOUTPUTS, OnUpdateExtrasTestOutputs)
	ON_UPDATE_COMMAND_UI(ID_HELP, OnUpdateHelp)
	ON_UPDATE_COMMAND_UI(ID_HELP_FINDER, OnUpdateHelpFinder)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_ARRANGE, OnUpdateWindowArrange)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_CASCADE, OnUpdateWindowCascade)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_NEW, OnUpdateWindowNew)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_TILE_HORZ, OnUpdateWindowTileHorz)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_TILE_VERT, OnUpdateWindowTileVert)
	ON_UPDATE_COMMAND_UI(ID_APP_ABOUT, OnUpdateAppAbout)
	ON_UPDATE_COMMAND_UI(ID_EXTRAS_LOGIN, OnUpdateExtrasLogin)
	ON_UPDATE_COMMAND_UI(ID_EXTRAS_LOGOUT, OnUpdateExtrasLogout)
	ON_UPDATE_COMMAND_UI(ID_EXTRAS_UTILITIES_EDIT, OnUpdateExtrasUtilitiesEdit)
	ON_UPDATE_COMMAND_UI_RANGE(ID_EXTRAS_UTILITIES_BASE, ID_EXTRAS_UTILITIES_LIMIT, OnUpdateExtraUtilities)

	ON_UPDATE_COMMAND_UI(ID_NEXT_PANE, OnUpdateNextPane)
	ON_UPDATE_COMMAND_UI(ID_PREV_PANE, OnUpdatePrevPane)
	ON_UPDATE_COMMAND_UI(ID_APP_EXIT, OnUpdateAppExit)
	ON_UPDATE_COMMAND_UI(ID_EXTRAS_SECURITY_SETUP, OnUpdateExtrasSecuritySetup)
	ON_UPDATE_COMMAND_UI(ID_AUDITTRAIL_DEFAULTFILES, OnUpdateAuditTrailDefaultFiles)
	ON_UPDATE_COMMAND_UI(ID_AUDITTRAIL_ADDITIONALFILES, OnUpdateAuditTrailAdditionalFiles)

	ON_UPDATE_COMMAND_UI(ID_EDIT_ADD_MONITORLIST, OnUpdateEditAddMonitorList)


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

	ON_WM_SIZE()
	ON_WM_SIZING()
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_INFO, OnUpdateStatusInfo)
	// Status Bar User information
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_USER, OnUpdateStatusInfo)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_EDIT_LOCK, OnUpdateStatusInfo)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_MODE, OnUpdateStatusInfo)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_GIGE_LICENSE_ERROR, OnUpdateStatusInfo)

	//ON_UPDATE_COMMAND_UI(ID_INDICATOR_FREE_MEMORY, OnUpdateStatusInfo)
	// Globale Hilfebefehle
	ON_COMMAND(ID_HELP_FINDER, CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CMDIFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CMDIFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CMDIFrameWnd::OnHelpFinder)

	// Start Object Script Parser
	ON_MESSAGE(SV_LOGGED_ON_USER_CHANGED,OnUserChanged)
	ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChange)
	ON_WM_ACTIVATEAPP()
	ON_MESSAGE(SV_SET_MODE, OnSetMode)
	ON_MESSAGE(SV_LOAD_PACKED_CONFIGURATION, OnLoadPackedConfiguration)
	ON_MESSAGE(SV_REFRESH_STATUS_BAR, InvalidateStatusBar )
	ON_MESSAGE(SV_SHUTDOWN, Shutdown)
	ON_MESSAGE(SV_UPDATE_IPDOC_VIEWS, UpdateIPDocViews)
	ON_COMMAND(ID_TOOLBAR_ZOOM, &SVMainFrame::OnToolbarZoom)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_ZOOM, &SVMainFrame::OnUpdateToolbarZoom)
	ON_MESSAGE(SV_REGISTER_MONITOR_LIST, &SVMainFrame::RemoteRegisterMonitorList)
	ON_MESSAGE(SV_ADD_FILE_TO_CONFIG, &SVMainFrame::onAddFileToConfig)
	ON_MESSAGE(SV_SAVE_CONFIG, &SVMainFrame::onSaveConfig)
	ON_MESSAGE(SV_SET_IPDOC_EDIT_TOOLID, SetIPDocEditToolID)
END_MESSAGE_MAP()

#pragma region Constructor

SVMainFrame::SVMainFrame() :m_PosSize {0}, m_bSizeChanged {false}, m_pregTestDlg {nullptr}
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
//******************************************************************************
// Class Wizard Generated Virtual Function(s):
//******************************************************************************

BOOL SVMainFrame::PreCreateWindow( CREATESTRUCT& cs )
{
	return CMDIFrameWnd::PreCreateWindow( cs );
}

void SVMainFrame::SetStatusInfoText( LPCTSTR SIText )
{
	size_t Size = ( nullptr != SIText ) ? ::strlen( SIText ) : 0;

	if( 0 < Size )
	{
		CClientDC DC( nullptr );
		int Width = DC.GetTextExtent( SIText, static_cast<int>(Size) ).cx - static_cast< int >( static_cast< double >( Size ) * 1.5 );

		m_wndStatusBar.SetPaneInfo( 1, ID_INDICATOR_INFO, 0, Width );
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

	CString Version = SvSyl::getSvoVersionAsString().c_str();

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
		lstrcat( szText, Version );
	}
	else
	{
		lstrcpy( szText, m_strTitle );
		lstrcat( szText, " " );
		lstrcat( szText, Version );

		// Get title of currently active SVC
		if( SvimState::CheckState( SV_STATE_READY | SV_STATE_RUNNING ) )
		{
			lstrcat( szText, _T( " - " ) );
			lstrcat( szText, getConfigFileName().c_str() );
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
		Log_Assert(FALSE);
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
}

void SVMainFrame::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CMDIFrameWnd::OnLButtonDblClk(nFlags, point);
}

void SVMainFrame::OnClose()
{
	bool bAllowAccess = false;

	if ( SvimState::CheckState( SV_STATE_REGRESSION ) )
	{
		//not allowed to exit during regression test
		return;
	}

	// If Running, check if access to exit run.
	if ( SvimState::CheckState( SV_STATE_RUNNING ) )
	{
		if( S_OK == TheSecurityManager().SVValidate( SECURITY_POINT_FILE_MENU_EXIT, 
			SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE ) )
		{
			OnStop();
			bAllowAccess = true;
		}
	}
	else if( S_OK == TheSecurityManager().SVValidate( SECURITY_POINT_FILE_MENU_EXIT ) )
	{
		bAllowAccess = true;
	}

	if( bAllowAccess )
	{
		if( SvimState::CheckState( SV_STATE_TEST ) )
		{
			OnStop();
		}

		// Store current bar state...
		try
		{
			SaveBarState( _T( "Bar States" ) );
		}
		catch (CException* e)
		{
			e->Delete();
			Log_Assert(FALSE);
		}

		// Log out...
		TheSVObserverApp().Logout();

		// Last checking...
		if (S_OK == DestroyConfig())
		{
			SvimState::changeState(SV_STATE_UNAVAILABLE, SV_STATE_READY);
			CMDIFrameWnd::OnClose();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnAppAbout
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::OnAppAbout()
{
	SVAboutDialogClass aboutDlg;
	aboutDlg.DoModal();
}

////////////////////////////////////////////////////////////////////////////////
// .Description : OnFileNewConfig - calls the SVApplicationAssistant Dialog to get
//              : correct systems for opening new IPDocuments
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::OnFileNewConfig()
{
	SvimState::AddState(SV_STATE_CREATING);
	if (S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_FILE_MENU_NEW))
	{
		if ((TheSVObserverApp().UpdateConfiguration(true))) // if not cancelled
		{
			if (TheSVObserverApp().OkToEdit() || !TheSecurityManager().SVIsSecured(SECURITY_POINT_MODE_MENU_EDIT_TOOLSET))
			{
				TheSVObserverApp().SetModeEdit(true);
			}
			else
			{
				TheSVObserverApp().SetModeEdit(false);
			}
			// Logic to remove unused remote inputs.
			SVConfigurationObject* pConfig(nullptr);
			SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

			// Never any Remotely Monitored Items on a New Configuration...
			HideIOTabIfPossible(SVRemoteMonitorListViewID);

			// Update Remote Inputs Tab
			UpdateRemoteInputTabs(GetTheIODoc());
		}
	}
	SvimState::RemoveState(SV_STATE_CREATING);
}

void SVMainFrame::OnFileSaveConfig(bool saveAs /*= false*/)
{
	SaveConfig(saveAs);
}

////////////////////////////////////////////////////////////////////////////////
// .Description : General - Opens a config File.
//				: - Calls a config File Open Dialog 
//				: - If a config already exists, calls the config saving procedure
//              : - Calls the config loading procedure
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::OnFileOpenSVC()
{
	TheSVObserverApp().ValidateMRUList();

	SvSml::TemporaryState_Editing tse; /// do this before calling validate for security as it may display a logon dialog!
	if (false == tse.stateWasEntered())
	{
		return;
	}
	// Proof user rights...
	if (S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_FILE_MENU_SELECT_CONFIGURATION))
	{
		SvFs::FileHelper svFileName;

		svFileName.SetFileType(SvFs::FileType::svzConfig);
		//
		// Try to read the current image file path name from registry...
		//
		svFileName.SetPathName(AfxGetApp()->GetProfileString(_T("Settings"), _T("SVCFilePath"), SvStl::GlobalPath::Inst().GetRunPath().c_str()));
		if (svFileName.SelectFile())
		{
			//
			// Write this path name back to registry...to initialize registry.
			//
			AfxGetApp()->WriteProfileString(_T("Settings"), _T("SVCFilePath"), svFileName.GetPathName().c_str());

			SVOLicenseManager::Instance().ClearLicenseErrors();
			if (S_OK == TheSVObserverApp().OpenFile(svFileName.GetFullFileName().c_str(), true))
			{
				if (SVOLicenseManager::Instance().HasToolErrors())
				{
					SVOLicenseManager::Instance().ShowLicenseManagerErrors();
				}
				ExtrasEngine::Instance().ResetAutoSaveInformation(); //reset autosave timestamp after configuration was loaded
			}
		}// end if ( svFileName.SelectFile() )
	}// end if ( S_OK == TheSecurityManager().Validate( SECURITY_POINT_FILE_MENU_SELECT_CONFIGURATION) )
	// Update Remote Inputs Tab
	UpdateRemoteInputTabs(GetTheIODoc());
}


////////////////////////////////////////////////////////////////////////////////
// .Title       : OnFileCloseConfig
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::OnFileCloseConfig()
{
	if (S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_FILE_MENU_CLOSE_CONFIGURATION))
	{
		SvSml::TemporaryState_Editing tse;
		if (false == tse.stateWasEntered())
		{
			return;
		}

		TheSVObserverApp().ValidateMRUList();

		// Check if current config is modified, ask for saving and try to close
		auto hr = DestroyConfig();

		if (S_OK == hr)
		{
			SvimState::RemoveState(SV_STATE_EDIT);
		}

		TheSVObserverApp().ValidateMRUList();
	}
}

void SVMainFrame::OnFileSaveAll()
{
	if (S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION))
	{
		OnFileSaveConfig();
	}
}


void SVMainFrame::OnFileSaveAsSVC()
{
	if (S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION_AS))
	{
		if (SvimState::CheckState(SV_STATE_READY))
		{
			// Call save as svx with file dialog...
			OnFileSaveConfig(true);
		}
	}
}

void SVMainFrame::OnEditEnvironment()
{
	TheSVObserverApp().UpdateConfiguration();
}

void SVMainFrame::OnEditIOList()
{
	if (GetTheIODoc())
	{
		// Get the active MDI child window.             
		POSITION pos = GetTheIODoc()->GetFirstViewPosition();
		CView* pView = GetTheIODoc()->GetNextView(pos);

		auto* pFrame = (CMDIChildWnd*)pView->GetParentFrame();
		pFrame->MDIActivate();
	}// end if
}

// .Description : OnTestMode calls SetTestMode
void SVMainFrame::OnTestMode()
{
	startDesiredState(SV_STATE_TEST);
}

void SVMainFrame::OnRunMode()
{
	TheSVObserverApp().EnterRunMode();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnStopTestMode
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::OnStopTestMode()
{
	if (false == SvimState::CheckState(SV_STATE_TEST))
	{
		return;
	}

	if (S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_MODE_MENU_STOP))
	{
		SvimState::SVRCBlocker block;
		OnStop();
	}
}

void SVMainFrame::OnModeEdit()
{
	bool bAllowAccess = false;

	if (SvimState::CheckState(SV_STATE_EDIT | SV_STATE_REMOTE_CMD))
	{
		return;
	}

	SvSml::TemporaryState_Editing tse;
	if (false == tse.stateWasEntered())
	{
		return;
	}

	// If Running, check if access to exit run.
	if (SvimState::CheckState(SV_STATE_RUNNING))
	{
		if (S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE,
			SECURITY_POINT_MODE_MENU_EDIT_TOOLSET))
		{
			OnStop();
			bAllowAccess = true;
		}
	}
	else if (S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_MODE_MENU_EDIT_TOOLSET))
	{
		bAllowAccess = true;
	}

	if (bAllowAccess)
	{
		if (!SvimState::CheckState(SV_STATE_EDIT))
		{
			if (SvimState::CheckState(SV_STATE_REGRESSION))
			{
				StopRegression();
			}

			if (SvimState::CheckState(SV_STATE_TEST))
			{
				OnStop();
			}

			if (SvimState::CheckState(SV_STATE_READY))
			{
				TheSVObserverApp().SetModeEdit(true); // Set Edit mode
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnGoOffline
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::OnGoOffline()
{
	if (!SvimState::CheckState(SV_STATE_EDIT
		| SV_STATE_REGRESSION
		| SV_STATE_TEST
		| SV_STATE_RUNNING
		| SV_STATE_REMOTE_CMD
		))
	{
		return;
	}

	TheSVObserverApp().DeselectTool();

	SetStatusInfoText(_T(""));

	if (SvimState::CheckState(SV_STATE_RUNNING))
	{
		// Dual Security access point
		if (S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_MODE_MENU_STOP,
			SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE))
		{
			OnStop();
		}
	}
	else if (S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_MODE_MENU_STOP))
	{
		bool isRegression = SvimState::CheckState(SV_STATE_REGRESSION);
		//Call OnStop first otherwise the SV_STATE_STOP is not set as StopRegression removes SV_STATE_REGRESSION
		OnStop();
		if (isRegression)
		{
			StopRegression();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnGoOnline
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::OnGoOnline()
{
	startDesiredState(SV_STATE_RUNNING);
}

void SVMainFrame::OnFilePrintConfig()
{
	if (S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_FILE_MENU_PRINT))
	{
		if (!SvimState::CheckState(SV_STATE_READY | SV_STATE_RUNNING) ||
			std::string(getConfigFullFileName()).empty())
		{
			return;
		}

		SVConfigurationPrint printConfig;
		printConfig.DoPrintConfig();
	}
}

void SVMainFrame::OnExtrasLogin()
{
	TheSecurityManager().SVLogon();

	// Check if Edit Mode should continue.
	if (SvimState::CheckState(SV_STATE_EDIT))
	{	// If this user is not allowed to edit then remove the edit mode
		if (!TheSecurityManager().SVIsDisplayable(SECURITY_POINT_MODE_MENU_EDIT_TOOLSET))
		{
			SvimState::RemoveState(SV_STATE_EDIT);
		}
		else
		{	// If they are still allowed then select the current tool.
			PostMessage(SV_SET_TOOL_SELECTED_IN_TOOL_VIEW, (WPARAM)TRUE);
			// Logged on User changed
			PostMessage(SV_LOGGED_ON_USER_CHANGED, 0);
		}
	}
}


void SVMainFrame::OnExtrasUtilitiesEdit()
{
	if (S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_EXTRAS_MENU_UTILITIES_SETUP))
	{
		SvSml::TemporaryState_Editing tse;
		if (false == tse.stateWasEntered())
		{
			return;
		}
		SVUtilities util;

		CWnd* pWindow = AfxGetMainWnd();
		CMenu* pMenu = (nullptr != pWindow) ? pWindow->GetMenu() : nullptr;
		pMenu = util.FindSubMenuByName(pMenu, _T("&Utilities"));
		if (nullptr != pMenu)
		{
			util.SetupUtilities(pMenu);
			TheSVObserverApp().UpdateAllMenuExtrasUtilities();
		}
	}
	else
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_AuthorizationFailed_Modification, SvStl::SourceFileParams(StdMessageParams));
	}
}

void SVMainFrame::OnExtrasSecuritySetup()
{
	SvSml::TemporaryState_Editing tse;
	if (false == tse.stateWasEntered()) /// do this before calling validate for security as it may display a logon dialog!
	{
		return;
	}
	if (S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_EXTRAS_MENU_SECURITY_MANAGER))
	{
		TheSecurityManager().SVSetupDialog();
		if (TheSecurityManager().SVGetUseLogon())
		{
			CMenu* pMenu;
			CWnd* pWindow = AfxGetMainWnd();
			pMenu = pWindow->GetMenu();

			// Look for "Extras" menu.
			int pos = FindMenuItem(pMenu, "E&xtras");
			if (pos == -1)
			{
				return;
			}

			CMenu* submenu = pMenu->GetSubMenu(pos);
			pos = FindMenuItem(submenu, "&Security Setup");
			if (pos > -1)
			{
				if (FindMenuItem(submenu, "User Log&out") == -1)
				{
					submenu->InsertMenu(pos, MF_BYPOSITION, ID_EXTRAS_LOGOUT, "User Log&out");
					submenu->InsertMenu(pos, MF_BYPOSITION, ID_EXTRAS_LOGIN, "User &Login");
				}
			}
		}

		if (SvimState::CheckState(SV_STATE_EDIT))
		{
			SvimState::RemoveState(SV_STATE_EDIT);
		}

		if (SvimState::CheckState(SV_STATE_REGRESSION))
		{
			StopRegression();
		}
	}
}

void SVMainFrame::OnAuditTrailDefaultFiles()
{
	SvSml::TemporaryState_Editing tse;
	if (false == tse.stateWasEntered())
	{
		return;
	}
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	if ((nullptr != pConfig))
	{
		pConfig->UpdateAuditFiles(false);
	}
	else
	{
		return;
	}
	SvOg::AuditFilesDialog Dlg(pConfig->GetAuditDefaultList(), SvOg::AuditFilesDialog::EDefault);
	if (Dlg.DoModal() == IDOK)
	{
		pConfig->SetAuditDefaultList(std::move(Dlg.GetFiles()));
		SvimState::AddState(SV_STATE_MODIFIED);
	}
}

void SVMainFrame::OnAuditTrailAdditionalFiles()
{
	SvSml::TemporaryState_Editing tse;
	if (false == tse.stateWasEntered()) /// do this before calling validate for security as it may display a logon dialog!
	{
		return;
	}
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	if ((nullptr != pConfig))
	{
		pConfig->UpdateAuditFiles(false);
	}
	else
	{
		return;
	}
	SvOg::AuditFilesDialog Dlg(pConfig->GetAuditWhiteList(), SvOg::AuditFilesDialog::WhiteList);
	if (Dlg.DoModal() == IDOK)
	{
		pConfig->SetAuditWhiteList(std::move(Dlg.GetFiles()));
		SvimState::AddState(SV_STATE_MODIFIED);
	}
}

void SVMainFrame::OnRunUtility(UINT uiId)
{
	SVUtilities util;

	if (S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_EXTRAS_MENU_UTILITIES_RUN))
	{
		util.RunUtility(&(TheSecurityManager()), uiId);
	}
	else
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_AuthorizationFailed_Execution, SvStl::SourceFileParams(StdMessageParams));
	}
}

void SVMainFrame::OnExtrasLogout()
{
	// Force log out, regardless auto login mode...
	TheSecurityManager().SVLogout();

	if (SvimState::CheckState(SV_STATE_EDIT))
	{
		SvimState::RemoveState(SV_STATE_EDIT);
	}

	if (SvimState::CheckState(SV_STATE_REGRESSION))
	{
		StopRegression();
	}

	// We need to deselect any tool that might be set for operator move.
	PostMessage(SV_SET_TOOL_SELECTED_IN_TOOL_VIEW, (WPARAM)FALSE);   // Deselect any tool selected for move.
}

void SVMainFrame::OnRCGoOffline()
{
	TheSVObserverApp().StopIfRunning();
}


void SVMainFrame::OnRCGoOnline()
{
	if (!SvimState::CheckState(SV_STATE_RUNNING))
	{
		TheSVObserverApp().SetMode(static_cast<long> (SvPb::DeviceModeType::runMode));
	}
}

void SVMainFrame::OnRCOpenCurrentSVX()
{
	TheSVObserverApp().OpenFile(SVRCGetSVCPathName());
}

void SVMainFrame::OnMemLeakDetection()
{
	if (getIniInfoHandler().GetInitialInfo().m_memoryLeakDetection)
	{
		if (TheSVObserverApp().m_MemLeakDetection)
		{
			TheSVObserverApp().startInstances();
		}
		else
		{
			TheSVObserverApp().stopInstances();
		}
		TheSVObserverApp().m_MemLeakDetection = !TheSVObserverApp().m_MemLeakDetection;
	}
}

void SVMainFrame::OnSVOFilePrintSetup()
{
	if (S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_FILE_MENU_PRINT_SETUP))
	{
		TheSVObserverApp().PrintSetup();
	}
}


void SVMainFrame::OnStop()
{
	if (SvPb::DeviceModeType::editModuleMode == SvimState::GetMode())
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ActionNotPossibleInModuleEditingMode, {getCompleteObjectNameForId(SvimState::getModuleEditingId())}, SvStl::SourceFileParams(StdMessageParams));
		return;
	}

	if (!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_EDIT | SV_STATE_REGRESSION))
	{
		return;
	}

	SvSml::TemporaryState_Editing tse;
	if (false == tse.stateWasEntered())
	{
		return;
	}

	StopSvo();
}


// This function is used when the m_ShouldRunLastEnvironmentAutomatically is set.
void SVMainFrame::OnRunMostRecentMRU()
{
	TheSVObserverApp().ValidateMRUList();

	if (!SvimState::CheckState(SV_STATE_UNAVAILABLE))
	{
		bool bRunning = SvimState::CheckState(SV_STATE_RUNNING);

		if (!bRunning)
		{
			OpenConfigFileFromMostRecentList(TheSVObserverApp().getRecentFileList(), ID_FILE_MRU_FILE1);
		}
	}
}

void SVMainFrame::OnEditMonitorList()
{
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig)
	{
		if (pConfig->SetupRemoteMonitorList())
		{
			SvimState::AddState(SV_STATE_MODIFIED);
			SVIODoc* pIODoc = GetTheIODoc();
			if (pIODoc)
			{
				pIODoc->SetModifiedFlag();
			}
		}
	}
}

void SVMainFrame::OnUpdateAllIOViews()
{
	if (GetTheIODoc() &&
		SvimState::CheckState(SV_STATE_READY | SV_STATE_RUNNING) &&
		!SvimState::CheckState(SV_STATE_CANCELING))
	{
		GetTheIODoc()->UpdateAllViews(nullptr);
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnAppExit
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::OnAppExit()
{
	bool bAllowAccess = true;

	if (SvimState::CheckState(SV_STATE_REGRESSION))
	{
		return;
	}

	if (SvimState::CheckState(SV_STATE_RUNNING))
	{
		if (S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE,
			SECURITY_POINT_FILE_MENU_EXIT))
		{
			OnStop();
			bAllowAccess = true;
		}
		else
		{
			bAllowAccess = false;
		}
	}

	if (bAllowAccess)
	{
		TheSVObserverApp().OnAppExitWrapper();
	}
}

void SVMainFrame::OnTriggerSettings()
{
	SoftwareTriggerDlg::Instance().ShowWindow(SW_SHOW);
}

void SVMainFrame::OnUpdateAuditTrailDefaultFiles(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(true);
}

void SVMainFrame::OnUpdateAuditTrailAdditionalFiles(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(true);
}

void SVMainFrame::OnUpdateModeRun(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION | SV_STATE_EDIT | SV_STATE_READY) &&
		!SvimState::CheckState(SV_STATE_RUNNING) &&
		TheSecurityManager().SVIsDisplayable(SECURITY_POINT_MODE_MENU_RUN));
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnUpdateEditEnvironment
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::OnUpdateEditEnvironment(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(SvimState::CheckState(SV_STATE_READY) &&
		!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION) && false == SvimState::isModuleEditing() &&
		TheSVObserverApp().OkToEdit());
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnUpdateEditIOList
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::OnUpdateEditIOList(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION) &&
		SvimState::CheckState(SV_STATE_EDIT) && nullptr != GetTheIODoc());
}

void SVMainFrame::OnUpdateFileClose(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION)
		&& TheSecurityManager().SVIsDisplayable(SECURITY_POINT_FILE_MENU_CLOSE_CONFIGURATION));
}

void SVMainFrame::OnUpdateFileNew(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION) &&
		TheSecurityManager().SVIsDisplayable(SECURITY_POINT_FILE_MENU_NEW) && (SVOLicenseManager::Instance().HasMatroxLicense()));
}

void SVMainFrame::OnUpdateFileOpen(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION)
		&& TheSecurityManager().SVIsDisplayable(SECURITY_POINT_FILE_MENU_SELECT_CONFIGURATION) && (SVOLicenseManager::Instance().HasMatroxLicense()));
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnUpdateFileOpenSVC
// -----------------------------------------------------------------------------
// .Description : 
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::OnUpdateFileOpenSVC(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION)
		&& TheSecurityManager().SVIsDisplayable(SECURITY_POINT_FILE_MENU_SELECT_CONFIGURATION) && (SVOLicenseManager::Instance().HasMatroxLicense()));
}

void SVMainFrame::OnUpdateFileSaveAll(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION)
		&& SvimState::CheckState(SV_STATE_READY)  && false == SvimState::isModuleEditing()
		&& TheSecurityManager().SVIsDisplayable(SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION));
}

void SVMainFrame::OnUpdateFileSaveAs(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION)
		&& SvimState::CheckState(SV_STATE_READY) && false == SvimState::isModuleEditing()
		&& TheSecurityManager().SVIsDisplayable(SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION_AS));
}

void SVMainFrame::OnUpdateFileSaveCopyAs(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION));
}

void SVMainFrame::OnUpdateFileSaveImage(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_REGRESSION | SV_STATE_TEST)
		&& SvimState::CheckState(SV_STATE_READY)
		&& TheSecurityManager().SVIsDisplayable(SECURITY_POINT_FILE_MENU_SAVE_IMAGE));
}

void SVMainFrame::OnUpdateFileUpdate(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION));
}


void SVMainFrame::OnUpdateExtrasLogin(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(SvimState::CheckState(SV_STATE_AVAILABLE | SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_READY | SV_STATE_REGRESSION));
}

void SVMainFrame::OnUpdateEditEditTool(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION) &&
		TheSVObserverApp().OkToEdit());
}

void SVMainFrame::OnUpdateEditEditToolSet(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION) &&
		TheSVObserverApp().OkToEdit());
}

void SVMainFrame::OnUpdateEditRemoteInputs(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION) &&
		TheSVObserverApp().OkToEdit());
	SVInputObjectList* pInputs(nullptr);
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig)
	{
		pInputs = pConfig->GetInputObjectList();
	}

	bool remoteInputsExist = false;
	if (nullptr != pInputs)
	{
		if (pInputs->getRemoteInputCount() > 0)
		{
			remoteInputsExist = true;
		}
	}
	PCmdUI->SetText(remoteInputsExist ? "Edit Remote Inputs" : "Add Remote Inputs");
}

void SVMainFrame::OnUpdateEditAddMonitorList(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST) &&
		SvimState::CheckState(SV_STATE_EDIT));

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig && 0 < pConfig->GetRemoteMonitorList().size())
	{
		pCmdUI->SetText(_T("Edit Monitor List"));
	}
	else
	{
		pCmdUI->SetText(_T("Add Monitor List"));
	}
}

void SVMainFrame::OnUpdateExtrasLightReference(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_REGRESSION) &&
		TheSVObserverApp().OkToEdit());
}

void SVMainFrame::OnUpdateFilePrintPreview(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_REGRESSION)
		&& TheSecurityManager().SVIsDisplayable(SECURITY_POINT_FILE_MENU_PRINT));
}

void SVMainFrame::OnUpdateFilePrintSetup(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_REGRESSION | SV_STATE_TEST)
		&& TheSecurityManager().SVIsDisplayable(SECURITY_POINT_FILE_MENU_PRINT_SETUP));
}

void SVMainFrame::OnUpdateExtrasTestOutputs(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION)
		&& TheSecurityManager().SVIsDisplayable(SECURITY_POINT_EXTRAS_MENU_TEST_OUTPUTS));
}

void SVMainFrame::OnUpdateHelp(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING));
}

void SVMainFrame::OnUpdateHelpFinder(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING));
}

void SVMainFrame::OnUpdateModeTest(CCmdUI* PCmdUI)
{
	PCmdUI->Enable((SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION | SV_STATE_EDIT | SV_STATE_STOP)) &&
		false == SvimState::isModuleEditing() &&	TheSecurityManager().SVIsDisplayable(SECURITY_POINT_MODE_MENU_TEST));

	PCmdUI->SetCheck(SvimState::CheckState(SV_STATE_TEST) &&
		!SvimState::CheckState(SV_STATE_REGRESSION));
}

void SVMainFrame::OnUpdateNextPane(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_REGRESSION));
}

void SVMainFrame::OnUpdatePrevPane(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_REGRESSION));
}

void SVMainFrame::OnUpdateWindowArrange(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_REGRESSION));
}

void SVMainFrame::OnUpdateWindowCascade(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_REGRESSION));
}

void SVMainFrame::OnUpdateWindowNew(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_REGRESSION));
}

void SVMainFrame::OnUpdateWindowTileHorz(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_REGRESSION));
}

void SVMainFrame::OnUpdateWindowTileVert(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_REGRESSION));
}


////////////////////////////////////////////////////////////////////////////////
// .Title       : OnUpdateAppExit
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::OnUpdateAppExit(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_REGRESSION)
		&& TheSecurityManager().SVIsDisplayable(SECURITY_POINT_FILE_MENU_EXIT));
}

void SVMainFrame::OnUpdateAppAbout(CCmdUI*)
{}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnUpdateGoOffline
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::OnUpdateGoOffline(CCmdUI* PCmdUI)
{
	PCmdUI->Enable((SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION | SV_STATE_EDIT | SV_STATE_STOP)) &&
		SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_READY) && false == SvimState::isModuleEditing() &&
		(TheSecurityManager().SVIsDisplayable(SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE) ||
		TheSecurityManager().SVIsDisplayable(SECURITY_POINT_MODE_MENU_STOP)));

	PCmdUI->SetCheck(SvimState::CheckState(SV_STATE_STOP));
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnUpdateGoOnline
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
void SVMainFrame::OnUpdateGoOnline(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION | SV_STATE_EDIT | SV_STATE_STOP) &&
		SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_READY) && false == SvimState::isModuleEditing() &&
		TheSecurityManager().SVIsDisplayable(SECURITY_POINT_MODE_MENU_RUN));

	PCmdUI->SetCheck(SvimState::CheckState(SV_STATE_RUNNING));
}

void SVMainFrame::OnUpdateFilePrintConfig(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_REGRESSION | SV_STATE_TEST)
		&& TheSecurityManager().SVIsDisplayable(SECURITY_POINT_FILE_MENU_PRINT));
}

void SVMainFrame::OnUpdateExtrasLogout(CCmdUI* PCmdUI)
{
	if (nullptr != PCmdUI)
	{
		PCmdUI->Enable(TheSecurityManager().SVIsLoggedOn());

		if (!TheSecurityManager().SVGetUseLogon())
		{	// If not logon mode then remove the login and logout menu items (Do this in logout due to it being lower then login)
			if (nullptr != PCmdUI->m_pMenu)
			{
				PCmdUI->m_pMenu->DeleteMenu(ID_EXTRAS_LOGIN, MF_BYCOMMAND);
				PCmdUI->m_pMenu->DeleteMenu(ID_EXTRAS_LOGOUT, MF_BYCOMMAND);
			}
		}
	}
}


void SVMainFrame::OnUpdateExtraUtilities(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(TheSecurityManager().SVIsDisplayable(SECURITY_POINT_EXTRAS_MENU_UTILITIES_RUN));
}

void SVMainFrame::OnUpdateResultsPicker(CCmdUI* PCmdUI)
{
	PCmdUI->Enable(((SvimState::CheckState(SV_STATE_READY)
		&& SvimState::CheckState(SV_STATE_EDIT))
		|| SvimState::CheckState(SV_STATE_RUNNING)
		|| SvimState::CheckState(SV_STATE_REGRESSION)
		|| SvimState::CheckState(SV_STATE_TEST)));
}

void SVMainFrame::OnUpdateRegressionTest(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(SvimState::CheckState(SV_STATE_REGRESSION));
}

void SVMainFrame::OnUpdateModeEdit(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION | SV_STATE_EDIT | SV_STATE_READY)) &&
		SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_READY) &&
		TheSecurityManager().SVIsDisplayable(SECURITY_POINT_MODE_MENU_EDIT_TOOLSET));

	pCmdUI->SetCheck(SvimState::CheckState(SV_STATE_EDIT));
}

void SVMainFrame::OnUpdateTriggerSettings(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}

void SVMainFrame::OnUpdateExtrasUtilitiesEdit(CCmdUI* pCmdUI)
{
	if (pCmdUI->m_pSubMenu)
	{
		bool bEnable = TheSecurityManager().SVIsDisplayable(SECURITY_POINT_EXTRAS_MENU_UTILITIES_SETUP) ||
			(TheSecurityManager().SVIsDisplayable(SECURITY_POINT_EXTRAS_MENU_UTILITIES_RUN) &&
				!SvimState::CheckState(SV_STATE_RUNNING));
		unsigned int uiGray = bEnable ? 0 : MF_GRAYED;
		pCmdUI->m_pMenu->EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION | uiGray);
	}
	else
	{
		pCmdUI->Enable(TheSecurityManager().SVIsDisplayable(SECURITY_POINT_EXTRAS_MENU_UTILITIES_SETUP) &&
			!SvimState::CheckState(SV_STATE_RUNNING));
	}
}

void SVMainFrame::OnUpdateExtrasSecuritySetup(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_REGRESSION | SV_STATE_TEST));
}

void SVMainFrame::OnRCClose()
{
	DestroyConfig(false, true);
}

void SVMainFrame::OnUpdateStatusInfo( CCmdUI *pCmdUI )
{
	UpdateStatusInfo(pCmdUI);
}

void SVMainFrame::UpdateStatusInfo(CCmdUI* pCmdUI)
{
	auto* pCtrl = ( CStatusBarCtrl* )pCmdUI->m_pOther;
	switch( pCmdUI->m_nID )
	{
		case ID_INDICATOR_INFO:
		{
			pCmdUI->Enable();
			break;
		}
		case ID_INDICATOR_USER:  // Status Bar User
		{
			BSTR bstUser = nullptr;
			CString strUser = _T("User: ");
			if( TheSecurityManager().SVGetUseLogon() )
			{
				TheSecurityManager().SVGetUser( bstUser );
				if( nullptr != bstUser )
				{
					strUser += bstUser;
					::SysFreeString( bstUser );
				}
			}
			pCmdUI->Enable();

			CClientDC DC( nullptr );
			int Width = DC.GetTextExtent( strUser ).cx - ( strUser.GetLength() * 2 );

			m_wndStatusBar.SetPaneInfo( pCmdUI->m_nIndex, ID_INDICATOR_USER, 0, Width );
			pCmdUI->SetText( strUser );

			break;
		}
		case ID_INDICATOR_MODE:  // Status Bar Mode
		{
			std::string Mode = _T("Mode: ");
			unsigned long lMode = 0;
			GlobalRCGetMode( &lMode );
			switch( lMode )
			{
				case SvPb::DeviceModeType::available:
				{
					m_wndStatusBar.m_lModeBKColor = SvDef::KoerberGray;
					m_wndStatusBar.m_lModeForeColor = SvDef::Black;
					break;
				}
				case SvPb::DeviceModeType::runMode:
				{
					m_wndStatusBar.m_lModeBKColor = RGB( 100, 255, 100 );
					m_wndStatusBar.m_lModeForeColor = SvDef::Black;
					Mode += _T("Run ");
					break;
				}
				case SvPb::DeviceModeType::stopMode:
				{
					m_wndStatusBar.m_lModeBKColor = RGB( 255, 128, 128 );
					m_wndStatusBar.m_lModeForeColor = SvDef::Black;
					Mode += _T("Stop ");
					break;
				}
				case SvPb::DeviceModeType::regressionMode:
				{
					m_wndStatusBar.m_lModeBKColor = SvDef::Fuchsia;
					m_wndStatusBar.m_lModeForeColor = SvDef::Black;
					Mode += _T("Regression ");
					break;
				}
				case SvPb::DeviceModeType::testMode:
				{
					m_wndStatusBar.m_lModeBKColor = SvDef::DarkBlue;
					m_wndStatusBar.m_lModeForeColor = SvDef::White;
					Mode += _T("Test ");
					break;
				}
				case SvPb::DeviceModeType::editMode:
				{
					m_wndStatusBar.m_lModeBKColor = SvDef::PaleYellow;
					m_wndStatusBar.m_lModeForeColor = SvDef::Black;
					Mode += _T("Edit ");
					break;
				}
				case SvPb::DeviceModeType::editModuleMode:
				{
					m_wndStatusBar.m_lModeBKColor = SvDef::PaleYellow;
					m_wndStatusBar.m_lModeForeColor = SvDef::Black;
					Mode += _T("Edit Module");
					break;
				}
				case SvPb::DeviceModeType::modeChanging:
				{
					m_wndStatusBar.m_lModeBKColor = SvDef::White;
					m_wndStatusBar.m_lModeForeColor = SvDef::Black;
					Mode += _T("Changing ");
					break;
				}
				default:
				{
					m_wndStatusBar.m_lModeBKColor = SvDef::KoerberGray;
					m_wndStatusBar.m_lModeForeColor = SvDef::Black;
					Mode += _T("Undefined");
				}
			}

			m_wndStatusBar.m_strModeText = Mode.c_str();

			pCmdUI->Enable();

			CClientDC DC( nullptr );
			int Width = DC.GetTextExtent( Mode.c_str() ).cx - static_cast<int> ( Mode.size() * 2 );

			m_wndStatusBar.SetPaneInfo( pCmdUI->m_nIndex, ID_INDICATOR_MODE, 0, Width );
			pCtrl->SetText( Mode.c_str(), pCmdUI->m_nIndex, SBT_OWNERDRAW );

			break;
		}

		case ID_INDICATOR_EDIT_LOCK:
		{
			CClientDC DC(nullptr);
			int lWidth = DC.GetTextExtent("no Edit Lock").cx/4*3;// DC.GetTextExtent does not seem to provide the proper width: need only 75 %
			m_wndStatusBar.SetPaneInfo(pCmdUI->m_nIndex, ID_INDICATOR_EDIT_LOCK, 0, lWidth);
			pCtrl->SetText("no EditLock", pCmdUI->m_nIndex, SBT_OWNERDRAW);

			break;
		}
		case ID_INDICATOR_GIGE_LICENSE_ERROR:
		{
			bool bMatroxError = false;
			bool bGigeError = false;
			std::string Error;

			if ( !SVOLicenseManager::Instance().HasMatroxLicense() )
			{
				bMatroxError = true;
				Error += cMatroxNoLicense;
			}
			else
			{
				if ( !SVOLicenseManager::Instance().HasMatroxGigELicense() && TheSVObserverApp().IsMatroxGige() )
				{
					Error = cGigeNoLicense;
				}
			}

			if ( (bMatroxError) || (bGigeError) )
			{
				m_wndStatusBar.m_strErrorText = Error.c_str();

				pCmdUI->Enable();

				if (bMatroxError) 
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
		default:
			{
				// Do nothing.
				break;
			}
	}
	m_wndStatusBar.Invalidate();
}



LRESULT SVMainFrame::InvalidateStatusBar( WPARAM , LPARAM  )
{
	m_wndStatusBar.Invalidate();
	return 0;
}

LRESULT SVMainFrame::RemoteRegisterMonitorList( WPARAM , LPARAM  )
{
	GetTheIODoc()->GetIOController()->ShowMonitorListTab();
	return 0;
}

LRESULT SVMainFrame::onAddFileToConfig(WPARAM , LPARAM lParam)
{
	auto pFileName = reinterpret_cast<LPCTSTR> (lParam);
	AddFileToConfig(pFileName);
	return 0;
}

LRESULT SVMainFrame::onSaveConfig(WPARAM , LPARAM lParam)
{
	auto pFileName = reinterpret_cast<LPCTSTR> (lParam);
	return SavePackedConfiguration(pFileName);
}

LRESULT SVMainFrame::Shutdown(WPARAM wParam, LPARAM)
{
	auto shutdownOption = static_cast<long>(wParam);
	char cmd[50];
	sprintf_s(cmd, "SVShutdown.exe %d 15", shutdownOption);
	system(cmd);
	return 0;
}

LRESULT SVMainFrame::UpdateIPDocViews(WPARAM wParam, LPARAM lParam)
{
	SVIPDoc* pDoc = GetIPDocByInspectionID(static_cast<uint32_t> (wParam));
	if (nullptr != pDoc)
	{
		pDoc->UpdateAllViews(nullptr, lParam);
	}
	return 0;
}

LRESULT SVMainFrame::SetIPDocEditToolID(WPARAM wParam, LPARAM lParam)
{
	SVIPDoc* pDoc = GetIPDocByInspectionID(static_cast<uint32_t> (wParam));
	if (nullptr != pDoc)
	{
		pDoc->setEditToolID(static_cast<uint32_t> (lParam));
	}
	return 0;
}

void SVMainFrame::OnMouseMove( UINT nFlags, CPoint point )
{
	SetStatusInfoText( nullptr );
	
	CMDIFrameWnd::OnMouseMove( nFlags, point );
}

void SVMainFrame::OnViewPPQBar()
{
	bool accessAllowed = TheSecurityManager().SVIsDisplayable(SECURITY_POINT_VIEW_MENU_PPQ_BAR);

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

void SVMainFrame::OnConfigurationFinishedInitializing()
{
	// Finally indicate to the 'world' that the configuration is loaded
	// and ready to run or test.
	//
	// All operations that need to happen after the configuration is fully
	// loaded and initialized should go here

	// Update the IO views. 

	SVIODoc* pIODoc( GetTheIODoc() );

	if( nullptr != pIODoc )
	{
		pIODoc->UpdateAllViews( nullptr );
	}// end if

	TheSVObserverApp().UpdateAllMenuExtrasUtilities();

	// Set the tool selected for an operator to move if any.
	PostMessage( SV_SET_TOOL_SELECTED_IN_TOOL_VIEW, (WPARAM) TRUE );

	// Indicate to the world that a configuration is loaded, parsed,
	// and is ready to run.
	SvimState::changeState(SV_STATE_READY, SV_STATE_UNAVAILABLE | SV_STATE_LOADING);

	//
	// If the configuration is required to start immediately after loading
	// or a GoOnline command was received from SVFocusNT,
	// then start the toolset, i.e. go 'online'.
	//
	bool StartLastConfiguration(false);
	RootObject::getRootChildValue(SvDef::FqnEnvironmentStartLastConfig, StartLastConfiguration);
	if ( StartLastConfiguration || SvimState::CheckState( SV_STATE_START_PENDING ) )
	{
		PostMessage ( WM_COMMAND, MAKEWPARAM (ID_MODE_RUN, 0), 0 );
	}

	TheSVObserverApp().RemoveUnusedFiles();
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
				UNREFERENCED_PARAMETER(hr);
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
		int PixelDepth(0);
		hr = SVDirectX::GetPrimarySurfacePixelDepth(PixelDepth);
		if (hr == DD_OK)
		{
			if (p_p1 / 8 != PixelDepth)
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
							auto* pIPDoc = dynamic_cast<SVIPDoc *>(pCurrentDocument);
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
		if ( p_p1 / 8 != PixelDepth )
		{
			if( WAIT_TIMEOUT == WaitForSingleObject( m_hDisplayChangedEvent, 0 ) )
			{
				std::string Message = _T("\n\n\nThe display color settings on the system have changed.\n"
					"Images may not display correctly until this configuration is reopened.");
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

LRESULT SVMainFrame::OnSetMode( WPARAM , LPARAM lParam )
{
	return TheSVObserverApp().SetMode( static_cast<unsigned long>(lParam ));
}

LRESULT SVMainFrame::OnLoadPackedConfiguration( WPARAM wParam, LPARAM lParam )
{
	return LoadPackedConfiguration(reinterpret_cast<LPCTSTR> (lParam), static_cast<ConfigFileType> (wParam));
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

LRESULT SVMainFrame::OnUserChanged(WPARAM, LPARAM  )
{
	bool accessAllowed = TheSecurityManager().SVIsDisplayable( SECURITY_POINT_VIEW_MENU_PPQ_BAR ) ;  // ID_VIEW_PPQ_BAR );

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
	if( S_OK == TheSecurityManager().SVValidate( SECURITY_POINT_VIEW_MENU_IMAGE_DISPLAY_UPDATE ) )
	{
		SvimState::AddState( SV_STATE_MODIFIED );

		bool ImageUpdate = false;
		RootObject::getRootChildValue( SvDef::FqnEnvironmentImageUpdate, ImageUpdate );
		RootObject::setRootChildValue( SvDef::FqnEnvironmentImageUpdate, !ImageUpdate );
	}
}

void SVMainFrame::OnUpdateViewImageDisplayUpdate( CCmdUI* pCmdUI )
{
	BOOL bEnable = TheSecurityManager().SVIsDisplayable( SECURITY_POINT_VIEW_MENU_IMAGE_DISPLAY_UPDATE );
	bEnable &= !SvimState::CheckState( SV_STATE_REGRESSION );
	pCmdUI->Enable( bEnable);
	bool ImageUpdate( false );
	RootObject::getRootChildValue( SvDef::FqnEnvironmentImageUpdate, ImageUpdate );
	pCmdUI->SetCheck( ImageUpdate );
}

void SVMainFrame::OnViewResultDisplayUpdate()
{
	if( S_OK == TheSecurityManager().SVValidate( SECURITY_POINT_VIEW_MENU_RESULT_DISPLAY_UPDATE ) )
	{
		SvimState::AddState( SV_STATE_MODIFIED );

		bool ResultOnline = false;
		RootObject::getRootChildValue( SvDef::FqnEnvironmentResultUpdate, ResultOnline );
		RootObject::setRootChildValue( SvDef::FqnEnvironmentResultUpdate, !ResultOnline );
	}
}

void SVMainFrame::OnUpdateViewResultDisplayUpdate( CCmdUI* pCmdUI )
{
	BOOL bEnable = TheSecurityManager().SVIsDisplayable(SECURITY_POINT_VIEW_MENU_RESULT_DISPLAY_UPDATE );
	bEnable &= !SvimState::CheckState( SV_STATE_REGRESSION );
	pCmdUI->Enable( bEnable);

	bool ResultOnline( false );
	RootObject::getRootChildValue( SvDef::FqnEnvironmentResultUpdate, ResultOnline );
	pCmdUI->SetCheck( ResultOnline );
}

void SVMainFrame::OnExtrasStartLastConfiguration()
{
	SvSml::TemporaryState_Editing tse;
	if (false == tse.stateWasEntered()) /// do this before calling validate for security as it may display a logon dialog!
	{
		return;
	}
	if (S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_EXTRAS_MENU_ADDITIONAL_ENVIRON))
	{
		bool StartLastConfiguration(false);
		RootObject::getRootChildValue(SvDef::FqnEnvironmentStartLastConfig, StartLastConfiguration);
		StartLastConfiguration = !StartLastConfiguration;
		RootObject::setRootChildValue(SvDef::FqnEnvironmentStartLastConfig, StartLastConfiguration);
	}
}

void SVMainFrame::OnUpdateExtrasStartLastConfiguration(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION)
				   && TheSecurityManager().SVIsDisplayable(SECURITY_POINT_EXTRAS_MENU_ADDITIONAL_ENVIRON) 
				   && (SVOLicenseManager::Instance().HasMatroxLicense()));

	bool StartLastConfiguration(false);
	RootObject::getRootChildValue(SvDef::FqnEnvironmentStartLastConfig, StartLastConfiguration);
	pCmdUI->SetCheck(StartLastConfiguration);
}

void SVMainFrame::OnExtrasEnableAutoSave()
{
	if (S_OK != TheSecurityManager().SVValidate(SECURITY_POINT_EXTRAS_MENU_AUTOSAVE_CONFIGURATION))
	{
		return;
	}

	ExtrasEngine::Instance().ToggleEnableAutoSave();
}

void SVMainFrame::OnUpdateExtrasEnableAutoSave(CCmdUI *pCmdUI)
{
	bool bEnable = TheSecurityManager().SVIsDisplayable( SECURITY_POINT_EXTRAS_MENU_AUTOSAVE_CONFIGURATION );

	pCmdUI->Enable( bEnable );
	pCmdUI->SetCheck( ExtrasEngine::Instance().IsAutoSaveEnabled());
}


BOOL SVMainFrame::VerifyBarStates(LPCTSTR lpszProfileName)
{
	CDockState state;
	state.LoadState(lpszProfileName);

	for (int i = 0; i < state.m_arrBarInfo.GetSize(); i++)
	{
		auto* pInfo = (CControlBarInfo*)state.m_arrBarInfo[i];

		Log_Assert(nullptr != pInfo);

		auto nDockedCount = static_cast<int>(pInfo->m_arrBarID.GetSize());
		if (nDockedCount > 0)
		{
			// dockbar
			for (int j = 0; j < nDockedCount; j++)
			{
				auto nID = (UINT) pInfo->m_arrBarID[j];
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
	auto* pChild = dynamic_cast<CMDIChildWnd*>(GetActiveFrame());

	CView* pView = nullptr;
	if(nullptr != pChild)
	{
		pView = pChild->GetActiveView();
	}

	auto* pImageView = dynamic_cast<SVImageView*>(pView);

	if(nullptr != pImageView)
	{
		return !pImageView->ImageIsEmpty();
	}

	return false;
}

void SVMainFrame::RefreshAllSplitters()
{
	auto* pSplitterFrame = dynamic_cast< SVIPSplitterFrame*>(GetActiveFrame());
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

void SVMainFrame::startDesiredState(DWORD desiredState)
{
	if (SvPb::DeviceModeType::editModuleMode == SvimState::GetMode())
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ActionNotPossibleInModuleEditingMode, {getCompleteObjectNameForId(SvimState::getModuleEditingId())}, SvStl::SourceFileParams(StdMessageParams));
		return;
	}

	if (SvimState::CheckState(SV_STATE_REMOTE_CMD | desiredState))
	{
		return;
	}

	SvSml::TemporaryState_Editing tse;
	if (false == tse.stateWasEntered())
	{
		return;
	}

	SvStl::MessageContainer exceptionContainer;
	ExtrasEngine::Instance().ExecuteAutoSaveIfAppropriate(SV_STATE_RUNNING == desiredState);

	long securityMode = (SV_STATE_TEST == desiredState) ? SECURITY_POINT_MODE_MENU_TEST : SECURITY_POINT_MODE_MENU_RUN;
	if (S_OK == TheSecurityManager().SVValidate(securityMode))
	{
		if (SvimState::CheckState(SV_STATE_REGRESSION))
		{
			StopRegression();
		}
		DWORD oppositeState {(SV_STATE_RUNNING == desiredState) ? SV_STATE_TEST : SV_STATE_RUNNING};
		if (SvimState::CheckState(oppositeState))
		{
			//Check if currently in run mode if allowed to exit run mode
			if (SV_STATE_RUNNING == oppositeState && S_OK != TheSecurityManager().SVValidate(SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE))
			{
				return;
			}
			OnStop();
		}

		//After Regression and test both going to stop this should only be either Stop or Edit mode
		long prevState = SvimState::CheckState(SV_STATE_EDIT) ? SV_STATE_EDIT : SvimState::CheckState(SV_STATE_STOP) ? SV_STATE_STOP : 0L;
		prevState |= SvimState::CheckState(SV_STATE_READY) ? SV_STATE_READY : 0L;

		if (SvimState::CheckState(SV_STATE_READY) && false == SvimState::CheckState(SV_STATE_TEST | SV_STATE_RUNNING))
		{
			SvimState::RemoveState(SV_STATE_EDIT);
			TheSVObserverApp().DeselectTool();

			if (S_OK == getIniInfoHandler().GetInitializationStatusFlags())
			{
				if (TheSVObserverApp().CheckSVIMType())
				{
					CWaitCursor wait;

					try
					{
						TheSVObserverApp().Start(desiredState);
						if (SV_STATE_TEST == desiredState)
						{
							SoftwareTriggerDlg::Instance().ShowWindow(SW_SHOW);
						}
					}
					catch (const SvStl::MessageContainer& rExp)
					{
						exceptionContainer = rExp;
					}
				}
				else
				{
					SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
					Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SVObserver_CannotRun_WrongModelNumber, TheSVObserverApp().m_rInitialInfoSvo.GetModelNumberVector(), SvStl::SourceFileParams(StdMessageParams));
					SvimState::changeState(prevState, desiredState);
				}
			}
			else
			{
				SvDef::StringVector msgList;
				msgList.push_back(TheSVObserverApp().m_rInitialInfoSvo.ModelNumberString());
				msgList.push_back(TheSVObserverApp().m_rInitialInfoSvo.InitializationFailureDescription(getIniInfoHandler().GetInitializationStatusFlags()));

				SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
				Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SVObserver_InitializationFailure, msgList, SvStl::SourceFileParams(StdMessageParams));

				SvimState::changeState(prevState, desiredState);
			}

			if (SvimState::CheckState(SV_STATE_STOP_PENDING))
			{
				PostMessage(WM_COMMAND, MAKEWPARAM(ID_STOP, 0), 0);
			}
			else if (0 != exceptionContainer.getMessage().m_MessageCode)
			{
				SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
				Exception.setMessage(exceptionContainer.getMessage());
				SvimState::changeState(prevState, desiredState);
			}
		}
	}
}

SVMainFrame* GetSvoMainFrame()
{
	return dynamic_cast<SVMainFrame*>(AfxGetMainWnd());
}

// get the Document Template for the SVIPDoc
CMultiDocTemplate* getIPDocTemplate()
{
	CMultiDocTemplate* pTemplate = nullptr;
	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	while (!pTemplate && pos)
	{
		auto* pDocTemplate = dynamic_cast<CMultiDocTemplate*>(AfxGetApp()->GetNextDocTemplate(pos));
		if (pDocTemplate)
		{
			POSITION posDoc = pDocTemplate->GetFirstDocPosition();
			while (!pTemplate && posDoc)
			{
				CDocument* pDoc = pDocTemplate->GetNextDoc(posDoc);
				if (pDoc && pDoc->IsKindOf(RUNTIME_CLASS(SVIPDoc)))
				{
					pTemplate = pDocTemplate;
				}
			}
		}
	}
	return pTemplate;
}

