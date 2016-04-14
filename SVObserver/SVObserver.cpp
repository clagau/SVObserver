//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision, Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObserver
//* .File Name       : $Workfile:   SVObserver.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.50  $
//* .Check In Date   : $Date:   23 Feb 2015 03:38:38  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <iostream>
#include "SVObserver.h"

#include "SVIOLibrary\SVIOConfigurationInterfaceClass.h"
#include "SVLibrary\SVPackedFile.h"
#include "SVObjectLibrary\SVObjectSynchronousCommandTemplate.h"
#include "SVOMFCLibrary\SVDeviceParam.h"
#include "SVOMFCLibrary\SVDeviceParams.h"
#include "SVOMFCLibrary\SVLongValueDeviceParam.h"
#include "SVTimerLibrary\SVClock.h"
#include "SVUtilityLibrary\SVGUID.h"
#include "SVMessage\SVMessage.h"
#include "SVObserver_i.h"
#include "SVCommand.h"
#include "SVAboutDialogClass.h"
#include "SVConfigurationPrint.h"
#include "SVEnvironmentSettingsDialog.h"
#include "SVFileNameManagerClass.h"
#include "SVImageViewScroll.h"
#include "SVIPChildFrm.h"
#include "SVMainFrm.h"
#include "SVDiscreteInputsView.h"
#include "SVMultiDocTemplate.h"
#include "SVStartWindow.h"
#include "SVSVIMStateClass.h"
#include "SVUtilities.h"
#include "SVIPDoc.h"
#include "SVIODoc.h"
#include "SVOIPDocClass.h"
#include "SVOIPFrameWndClass.h"
#include "SVOIPViewClass.h"
#include "SVOIODocClass.h"
#include "SVOIOFrameWndClass.h"
#include "SVOIOViewClass.h"
#include "SVMessageWindow.h"
#include "SVIO.h"
#include "SVToolSet.h"
#include "ExtrasEngine.h"

#include "SVInputObjectList.h"
#include "SVOutputObjectList.h"
#include "SVDigitalInputObject1.h"
#include "SVDigitalOutputObject1.h"

#include "SVConfigurationLibrary\SVOCMGlobals.h"
#include "SVDigitizerProcessingClass.h"

#include "SVOConfigAssistantDlg.h"

#include "SVXMLLibrary\SVNavigateTree.h"

#include "SVConfigurationObject.h"
#include "ObjectInterfaces\SVUserMessage.h"
#include "SVOMFCLibrary\SVOINIClass.h"
#include "SVOMFCLibrary\SVOIniLoader.h"

#include "SVGigeCameraManagerDlg.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVMemoryManager.h"
#include "SVArchiveTool.h"
#include "SVArchiveWritingDlg.h"
#include "SVLibrary\SVWinUtility.h"
#include "SVIOLibrary\SVIOParameterEnum.h"
#include "SoftwareTriggerDlg.h"

#include "SVGlobal.h"

#include "SVIOController.h"

#include "SVDirectX.h"
#include "SVHardwareManifest.h"
#include "SVTriggerProcessingClass.h"
#include "SVDigitizerProcessingClass.h"
#include "SVSocketLibrary\SVSocketLibrary.h"
#include "SVInputStreamManager.h"
#include "SVOutputStreamManager.h"
#include "SVCommandStreamManager.h"
#include "SVFailStatusStreamManager.h"
#include "SVObjectLibrary\SVGetObjectDequeByTypeVisitor.h"
#include "SVSystemLibrary\SVVersionInfo.h"
#include "GuiCommands/InspectionRunOnce.h"
#include "SVConfigurationTreeWriter.h"
#include "SVOLicenseManager\SVOLicenseManager.h"
#include "SVSocketRemoteCommandManager.h"
#include "SVImportedInspectionInfo.h"
#include "SVIPDocInfoImporter.h"
#include "SVVisionProcessorHelper.h"
#include "RemoteCommand.h"
#include "SVIOBoardCapabilities.h"
#include "SVInspectionProcess.h"
#include "SVPPQObject.h"
#include "RootObject.h"
#include "SVMonitorList.h"
#include "SVThreadInfoDlg.h"
#include "SVSharedMemorySingleton.h"
#include "SVUtilityLibrary\LoadDll.h"
#include "SVStatusLibrary\MessageManagerResource.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "TextDefinesSvO.h"
#include "SVObjectLibrary\SVObjectXMLWriter.h"
#include "SVStatusLibrary\MessageContainer.h"
#pragma endregion Includes

#pragma region Declarations
#define ID_TRIGGER_SETTINGS 21017

static const int UseLargerArchiveMemoryPool = 16000;
static const int AsyncDefault4GB = 50;
static const int AsyncDefault16GB = 200;
static const int GoOfflineDefault4GB = 300;
static const int GoOfflineDefault16GB = 2000;

static const HRESULT ErrorMatroxServiceNotRunning = 0xcf00116f;

extern bool g_bUseCorrectListRecursion;
#pragma endregion Declarations

enum PortNumbers: uint16_t
{ 
	InputStreamPortNumber = 32100,
	OutputStreamPortNumber = 32101,
	FailStatusStreamPortNumber = 28969,
	RemoteCommandsPortNumber = 28960
};

/**
@SVObjectName SVObserver COM Module

@SVObjectOverview This object is the SVObserver COM Module used to interface the SVObserver application to the COM server.  You may derive a class from CAtlMfcModule and use it if you want to override something, but do not change the name of default _Module variable name.

@SVObjectOperations This object presents all of the functionality of the SVObserver Application object.

*/
class CSVObserverModule : public CAtlMfcModule
{
public:
	DECLARE_LIBID(LIBID_SVObserver)
#ifdef _DEBUG
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SVOBSERVER_DEBUG, "{08B70D3F-F12C-11D4-A927-00106F0309AB}")
#else
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SVOBSERVER, "{08B70D3F-F12C-11D4-A927-00106F0309AB}")
#endif
};

CSVObserverModule _Module;


SVObserverApp TheSVObserverApp;

//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CMdiChildWorkaround : public CMDIChildWnd
{
public:
	HMENU GetMenu() {return m_hMenuShared;}
};

// Dieser Bezeichner wurde als statistisch eindeutig für Ihre Anwendung generiert.
// Sie können ihn ändern, wenn Sie einen bestimmten Bezeichnernamen bevorzugen.
// used in .odl
// {F4C4D491-D660-11D0-9B52-00805F717DCE}
static const CLSID clsid =
{ 0xf4c4d491, 0xd660, 0x11d0, { 0x9b, 0x52, 0x0, 0x80, 0x5f, 0x71, 0x7d, 0xce } };

//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVObserverApp
//* Note(s)    : Application Base Class
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

//******************************************************************************
// Adjustments
//******************************************************************************

IMPLEMENT_SERIAL( SVObserverApp, CWinApp, 0 );

//******************************************************************************
// Message Map Entries
//******************************************************************************
BEGIN_MESSAGE_MAP(SVObserverApp, CWinApp)
	//{{AFX_MSG_MAP(SVObserverApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, OnFileNewConfig)
	ON_COMMAND(ID_FILE_SAVE_SVC, OnFileSaveConfig)
	ON_COMMAND(ID_FILE_OPEN_SVC, OnFileOpenSVC)
	ON_COMMAND(ID_FILE_CLOSE_SVC, OnFileCloseConfig)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE_ALL, OnFileSaveAll)
	ON_COMMAND(ID_FILE_PRINT_SETUP, OnSVOFilePrintSetup)
	ON_COMMAND(ID_FILE_PRINT_CONFIG, OnFilePrintConfig)
	ON_COMMAND(ID_FILE_SAVE_AS_SVC, OnFileSaveAsSVC)
	ON_COMMAND_EX_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, OnOpenRecentFile)

	ON_COMMAND(ID_EDIT_EDITENVIRONMENT, OnEditEnvironment)
	ON_COMMAND(ID_EDIT_EDITIOLIST, OnEditIOList)

	ON_COMMAND(ID_MODE_TEST, OnTestMode)
	ON_COMMAND(ID_MODE_TEST_BTN, OnModeTestBtn)
	ON_COMMAND(ID_MODE_RUN, OnRunMode)
	ON_COMMAND(ID_MODE_STOPTEST, OnStopTestMode)
	ON_COMMAND(ID_MODE_EDIT, OnModeEdit)
	ON_COMMAND(ID_GO_OFFLINE, OnGoOffline)
	ON_COMMAND(ID_GO_ONLINE, OnGoOnline)

	ON_COMMAND(ID_EXTRAS_LOGOUT, OnExtrasLogout)
	ON_COMMAND(ID_EXTRAS_LOGIN, OnExtrasLogin)
	ON_COMMAND(ID_EXTRAS_UTILITIES_EDIT, OnExtrasUtilitiesEdit)
	ON_COMMAND(ID_EXTRAS_SECURITY_SETUP, OnExtrasSecuritySetup)
	ON_COMMAND(ID_EXTRAS_ENVIRONMENTSETTINGS, OnEnvironmentSettings)
	ON_COMMAND(ID_EXTRAS_THREAD_AFFINITY, OnThreadAffinitySetup)
	ON_COMMAND_RANGE(ID_EXTRAS_UTILITIES_BASE, ID_EXTRAS_UTILITIES_LIMIT, OnRunUtility)

	ON_COMMAND(ID_RC_GO_OFFLINE, OnRCGoOffline)
	ON_COMMAND(ID_RC_GO_ONLINE, OnRCGoOnline)
	ON_COMMAND(ID_RC_SAVE_ALL_AND_GET_CONFIG, OnRCSaveAllAndGetConfig)
	ON_COMMAND(ID_RC_CLOSE_AND_CLEAN_RUN_DIR, OnRCCloseAndCleanUpDownloadDirectory)
	ON_COMMAND(ID_RC_OPEN_CURRENT_SVX, OnRCOpenCurrentSVX)

	ON_COMMAND(ID_STOP, OnStop)
	ON_COMMAND(ID_UPDATE_ALL_IOVIEWS, OnUpdateAllIOViews )
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_TRIGGER_SETTINGS, OnTriggerSettings)

	ON_UPDATE_COMMAND_UI(ID_TRIGGER_SETTINGS, OnUpdateTriggerSettings)
	ON_UPDATE_COMMAND_UI(ID_MODE_RUN, OnUpdateModeRun)

	ON_UPDATE_COMMAND_UI(ID_MODE_STOPTEST, OnUpdateModeStopTest)
	ON_UPDATE_COMMAND_UI(ID_MODE_TEST, OnUpdateModeTest)
	ON_UPDATE_COMMAND_UI(ID_MODE_TEST_BTN, OnUpdateModeTestBtn)
	ON_UPDATE_COMMAND_UI(ID_MODE_EDIT, OnUpdateModeEdit)
	ON_UPDATE_COMMAND_UI(ID_GO_OFFLINE, OnUpdateGoOffline)
	ON_UPDATE_COMMAND_UI(ID_GO_ONLINE, OnUpdateGoOnline)
	ON_UPDATE_COMMAND_UI(ID_RUN_REGRESSIONTEST, OnUpdateRegressionTest)

	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE_SVC, OnUpdateFileClose)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, OnUpdateFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN_SVC, OnUpdateFileOpenSVC)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_SVC, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_ALL, OnUpdateFileSaveAll)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS_SVC, OnUpdateFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_COPY_AS, OnUpdateFileSaveCopyAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_IMAGE, OnUpdateFileSaveImage)
	ON_UPDATE_COMMAND_UI(ID_FILE_UPDATE, OnUpdateFileUpdate)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_CONFIG, OnUpdateFilePrintConfig)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, OnUpdateFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_SETUP, OnUpdateFilePrintSetup)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, OnUpdateRecentFileMenu)

	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_EDITIOLIST, OnUpdateEditIOList)
	ON_UPDATE_COMMAND_UI(ID_EDIT_EDITENVIRONMENT, OnUpdateEditEnvironment)
	ON_UPDATE_COMMAND_UI(ID_EDIT_EDITTOOL, OnUpdateEditEditTool)
	ON_UPDATE_COMMAND_UI(ID_EDIT_EDITTOOLSETCONDITION, OnUpdateEditEditToolSetCondition)
	ON_UPDATE_COMMAND_UI(ID_RESULTS_PICKER, OnUpdateResultsPicker)
	ON_UPDATE_COMMAND_UI(ID_PUBLISHED_RESULTS_PICKER, OnUpdatePublishedResultsPicker)
	ON_UPDATE_COMMAND_UI(ID_PUBLISHED_RESULT_IMAGES_PICKER, OnUpdatePublishedImagesPicker)
	ON_UPDATE_COMMAND_UI(ID_EDIT_EDITREMOTEINPUTS, OnUpdateEditRemoteInputs)
	ON_UPDATE_COMMAND_UI(ID_EXTRAS_THREAD_AFFINITY, OnUpdateThreadAffinitySetup)
	ON_UPDATE_COMMAND_UI(ID_EXTRAS_LIGHTREFERENCE, OnUpdateExtrasLightReference)
	ON_UPDATE_COMMAND_UI(ID_EXTRAS_TESTOUTPUTS, OnUpdateExtrasTestOutputs)
	ON_UPDATE_COMMAND_UI(ID_HELP, OnUpdateHelp)
	ON_UPDATE_COMMAND_UI(ID_HELP_FINDER, OnUpdateHelpFinder)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_ARRANGE, OnUpdateWindowArrange)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_CASCADE, OnUpdateWindowCascade)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_NEW, OnUpdateWindowNew)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_TILE_HORZ, OnUpdateWindowTileHorz)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_TILE_VERT, OnUpdateWindowTileVert)
	ON_UPDATE_COMMAND_UI(ID_ADD_SHIFTTOOL, OnUpdateAddShiftTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_WINDOWTOOL, OnUpdateAddWindowTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_CYLINDRICALWARPTOOL, OnUpdateAddCylindricalWarpTool)
	ON_UPDATE_COMMAND_UI(ID_APP_ABOUT, OnUpdateAppAbout)
	ON_UPDATE_COMMAND_UI(ID_ADD_LOADIMAGETOOL, OnUpdateAddLoadImageTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_IMAGETOOL, OnUpdateAddImageTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_ARCHIVETOOL,OnUpdateAddArchiveTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_MATHTOOL, OnUpdateAddMathTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_STATISTICSTOOL, OnUpdateAddStatisticsTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_TRANSFORMATIONTOOL, OnUpdateAddTransformationTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_EXTERNAL_TOOL, OnUpdateAddExternalTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_POLARUNWRAPTOOL, OnUpdateAddPolarUnwrapTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_ACQUISITIONTOOL, OnUpdateAddAcquisitionTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_COLORTOOL, OnUpdateAddColorTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_LINEARTOOL, OnUpdateAddLinearTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_REMOTEINPUTTOOL, OnUpdateAddRemoteInputTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_RESIZETOOL, OnUpdateAddResizetool)
	ON_UPDATE_COMMAND_UI(ID_ADD_RINGBUFFERTOOL, OnUpdateAddRingBufferTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_PERSPECTIVEWARPTOOL, OnUpdateAddPerspectiveTool)
	ON_UPDATE_COMMAND_UI(ID_EXTRAS_LOGIN, OnUpdateExtrasLogin)
	ON_UPDATE_COMMAND_UI(ID_EXTRAS_ENVIRONMENTSETTINGS, OnUpdateExtrasAdditionalEnvironmentSettings)
	ON_UPDATE_COMMAND_UI(ID_EXTRAS_LOGOUT, OnUpdateExtrasLogout)
	ON_UPDATE_COMMAND_UI(ID_EXTRAS_UTILITIES_EDIT, OnUpdateExtrasUtilitiesEdit)
	ON_UPDATE_COMMAND_UI_RANGE(ID_EXTRAS_UTILITIES_BASE, ID_EXTRAS_UTILITIES_LIMIT, OnUpdateExtraUtilities)
	ON_UPDATE_COMMAND_UI_RANGE(ID_EDIT_PUBLISHEDRESULTS_BASE, ID_EDIT_PUBLISHEDRESULTS_LIMIT, &SVObserverApp::OnUpdateEditPublishedResults)

	ON_UPDATE_COMMAND_UI(ID_NEXT_PANE, OnUpdateNextPane)
	ON_UPDATE_COMMAND_UI(ID_PREV_PANE, OnUpdatePrevPane)
	ON_UPDATE_COMMAND_UI(ID_APP_EXIT, OnUpdateAppExit)
	ON_UPDATE_COMMAND_UI(ID_EXTRAS_SECURITY_SETUP, OnUpdateExtrasSecuritySetup)
	//}}AFX_MSG_MAP
	ON_COMMAND( SV_AUTO_RUN_LAST_MRU, OnRunMostRecentMRU )
	ON_COMMAND(ID_RC_CLOSE, OnRCClose)

	ON_COMMAND(ID_EDIT_ADD_REMOTE_OUTPUTS, &SVObserverApp::OnEditRemoteOutputs)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADD_REMOTE_OUTPUTS, &SVObserverApp::OnUpdateEditAddRemoteOutputs)
	ON_COMMAND_RANGE(ID_EDIT_PUBLISHEDRESULTS_BASE, ID_EDIT_PUBLISHEDRESULTS_LIMIT, &SVObserverApp::OnEditPublishedResults)

	ON_COMMAND(ID_EDIT_ADD_MONITORLIST, &SVObserverApp::OnEditMonitorList)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADD_MONITORLIST, &SVObserverApp::OnUpdateEditAddMonitorList)

	END_MESSAGE_MAP()

#pragma region Constructor
SVObserverApp::SVObserverApp()
: m_gigePacketSize( 0 )
, m_InputStreamPortNumber( InputStreamPortNumber )
, m_OutputStreamPortNumber( OutputStreamPortNumber )
, m_FailStatusStreamPortNumber( FailStatusStreamPortNumber )
, m_RemoteCommandsPortNumber( RemoteCommandsPortNumber )
, m_DataValidDelay( 0 )
, m_forcedImageUpdateTimeInSeconds(0)
{
	free((void*)m_pszHelpFilePath);
	m_pszHelpFilePath = _tcsdup(_T("C:\\SVObserver\\bin\\SVObserver.chm"));
	EnableHtmlHelp();

	::OutputDebugString( _T( "Executing => SVObserverApp::SVObserverApp()\n" ) );

	m_csProductName.Empty();

	m_csProcessorBoardName = _T( "Unknown board" );
	m_csTriggerBoardName = _T( "Unknown board" );
	m_csAcquisitionBoardName = _T( "Unknown board" );
	m_csFileAcquisitionBoardName = _T( "File Acquisition" );
	m_csDigitalBoardName = _T( "Unknown board" );

	m_csTriggerDLL.Empty();
	m_csSoftwareTriggerDLL.Empty();
	m_csAcquisitionTriggerDLL.Empty();
	m_csDigitizerDLL.Empty();
	m_csFileAcquisitionDLL.Empty();
	m_csDigitalDLL.Empty();

	m_csReloadTriggerDLL = _T( "Y" );
	m_csReloadAcquisitionDLL = _T( "Y" );
	m_csReloadDigitalDLL = _T( "Y" );

	// The Standard Configuration Execution Directory
	m_ConfigExePNVariableName			= _T( "ConfigurationExecutionPathName" );	// LPCTSTR
	m_ConfigExePNVariableDefaultValue	= _T( "C:\\RUN\\" );						// LPCTSTR
	m_ConfigExePNVariableValue			= _T( "" );									// CString

	// The Standard Last Valid Configuration Directory
	m_LastValidConfigPNVariableName			= _T( "LastValidConfigurationPathName" );	// LPCTSTR
	m_LastValidConfigPNVariableDefaultValue	= _T( "C:\\Last Valid\\" );					// LPCTSTR
	m_LastValidConfigPNVariableValue		= _T( "" );									// CString

	m_csProcessor.Empty();
	m_csFrameGrabber.Empty();
	m_csIOBoard.Empty();
	m_csOptions.Empty();

	m_csTrigger.Empty();

	m_hrHardwareFailure = SV_HARDWARE_FAILURE_ALL;
	m_OfflineCount = 0;
	m_ShouldRunLastEnvironmentAutomatically = FALSE;
	m_AutoRunDelayTime = 1000;

	m_CurrentVersion = SeidenaderVision::SVVersionInfo::GetLongVersion();
	m_LoadingVersion = 0L;

	m_pCurrentDocument = nullptr;	// Set by current Document!!!
	m_pMessageWindow = nullptr;

	m_ConfigFileName.SetFileType( SV_SVX_CONFIGURATION_FILE_TYPE );
	m_ConfigFileName.setExcludeCharacters( SvO::SVEXCLUDECHARS_CONFIG_NAME );

	SVFileNameManagerClass svFileManager;
	svFileManager.AddItem( &m_ConfigFileName );
}// end SVObserver ctor

SVObserverApp::~SVObserverApp()
{
	// File management for config file.
	SVFileNameManagerClass svFileManager;
	svFileManager.RemoveItem( &m_ConfigFileName );
}
#pragma endregion Constructor

#pragma region Public Methods
#pragma region AFX_MSG Methods
////////////////////////////////////////////////////////////////////////////////
// .Title       : OnAppAbout
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
void SVObserverApp::OnAppAbout()
{
	SVAboutDialogClass aboutDlg;
	aboutDlg.DoModal();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnFileNewConfig - calls the SVApplicationAssistant Dialog
// -----------------------------------------------------------------------------
// .Description : OnFileNewConfig - calls the SVApplicationAssistant Dialog to get
//              : correct systems for opening new IPDocuments
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
//  :30.06.1999 FRB         Wait cursor
//	:20.10.1999 RO			Merged to ShowConfigurationAssistant().
////////////////////////////////////////////////////////////////////////////////
void SVObserverApp::OnFileNewConfig() 
{
	SVSVIMStateClass::AddState( SV_STATE_CREATING );
	if( S_OK == m_svSecurityMgr.SVValidate( SECURITY_POINT_FILE_MENU_NEW ) )
	{
		if ( (ShowConfigurationAssistant( 0, TRUE )) ) // if not cancelled
		{
			if( OkToEdit() || !m_svSecurityMgr.SVIsSecured( SECURITY_POINT_MODE_MENU_EDIT_TOOLSET ) )
			{
				SetModeEdit( true );
			}
			else
			{
				SetModeEdit( false );
			}
			// Logic to remove unused remote inputs.
			SVConfigurationObject* pConfig( nullptr );
			SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

			if( nullptr != pConfig )
			{

				if( pConfig->GetRemoteOutputGroupCount() == 0)
				{
					HideIOTab( SVRemoteOutputsViewID );
				}
				pConfig->ClearRemoteMonitorList();
			}
			else
			{
				HideIOTab( SVRemoteOutputsViewID );
			}

			// Never any Remotely Monitored Items on a New Configuration...
			HideRemoteMonitorListTab();

			// Update Remote Inputs Tab
			UpdateRemoteInputTabs();
		}
	}
	SVSVIMStateClass::RemoveState( SV_STATE_CREATING );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnFileSaveConfig
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
void SVObserverApp::OnFileSaveConfig()
{
	if( CString( getConfigFullFileName() ).IsEmpty() || 
		CString( getConfigPathName() ).IsEmpty() )
	{
		fileSaveAsSVX();
	}
	else
	{
		SVFileNameManagerClass svFileManager;

		CString csTempName = svFileManager.GetConfigurationPathName();

		if ( csTempName.IsEmpty() )
		{
			csTempName = getConfigFullFileName();
		}
		else
		{
			csTempName += _T( "\\" );
			csTempName += getConfigFileName();
		}

		fileSaveAsSVX( csTempName );
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnFileOpenSVC
// -----------------------------------------------------------------------------
// .Description : General - Opens a config File.
//				: - Calls a config File Open Dialog 
//				: - If a config already exists, calls the config saving procedure
//              : - Calls the config loading procedure
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: void
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :15.10.1998	RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVObserverApp::OnFileOpenSVC() 
{
	ValidateMRUList();
	
	SVSVIMStateClass::AddState( SV_STATE_EDITING ); /// do this before calling validate for security as it may display a logon dialog!
	// Proof user rights...
	if ( m_svSecurityMgr.SVValidate( SECURITY_POINT_FILE_MENU_SELECT_CONFIGURATION) == S_OK )
	{
		SVFileNameClass svFileName;

		svFileName.SetFileType( SV_SVX_CONFIGURATION_FILE_TYPE );
		//
		// Try to read the current image file path name from registry...
		//
		svFileName.SetPathName( AfxGetApp()->GetProfileString( _T( "Settings" ), 
			_T( "SVCFilePath" ), 
			_T( "C:\\RUN" ) ) );
		if ( svFileName.SelectFile() )
		{
			//
			// Write this path name back to registry...to initialize registry.
			//
			AfxGetApp()->WriteProfileString( _T( "Settings" ),
				_T( "SVCFilePath" ),
				svFileName.GetPathName() );

			// Check for SVX file...
			if ( CString( svFileName.GetExtension() ).CompareNoCase( _T( ".svx" ) ) == 0 )
			{
				//
				// Open the configuration file (.svx) and read it and
				// all the associated files for this configuration.
				//
				TheSVOLicenseManager().ClearLicenseErrors();
				if ( OpenSVXFile( svFileName.GetFullFileName() ) == S_OK )
				{
					if ( TheSVOLicenseManager().HasToolErrors() )
					{
						TheSVOLicenseManager().ShowLicenseManagerErrors();
					}
					ExtrasEngine::Instance().ResetAutoSaveInformation(); //Arvid: reset autosave timestamp after configuration was loaded
				}
			}
			if( !m_svSecurityMgr.SVIsSecured( SECURITY_POINT_MODE_MENU_EDIT_TOOLSET ) )
			{
				SetModeEdit( true ); // Set Edit mode
			}

		}// end if ( svFileName.SelectFile() )
	}// end if ( m_svSecurityMgr.Validate( SECURITY_POINT_FILE_MENU_SELECT_CONFIGURATION) == S_OK )
	// Update Remote Inputs Tab
	UpdateRemoteInputTabs();

	SVSVIMStateClass::RemoveState( SV_STATE_EDITING );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnEditEnvironment
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
//	:04.03.1999 RO			Body outlayed to ShowConfigurationAssistant();
////////////////////////////////////////////////////////////////////////////////
void SVObserverApp::OnEditEnvironment() 
{
	ShowConfigurationAssistant();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnTestMode
// -----------------------------------------------------------------------------
// .Description : OnTestMode calls SetTestMode
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
void SVObserverApp::OnTestMode() 
{
	ExtrasEngine::Instance().ExecuteAutoSaveIfAppropriate(false);//Arvid: before entering test mode: perform autosave

	SetTestMode();
}

void SVObserverApp::OnModeTestBtn() 
{
	OnTestMode();
}

void SVObserverApp::OnRunMode() 
{
	try
	{
		Start();
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		//Log exception, (do not display the error because it is called from remote)
		SvStl::MessageMgrNoDisplay  Exception( SvStl::LogOnly );
		Exception.setMessage( rExp.getMessage() );
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnStopTestMode
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
void SVObserverApp::OnStopTestMode() 
{
	if( !SVSVIMStateClass::CheckState( SV_STATE_EDIT 
		| SV_STATE_REGRESSION
		| SV_STATE_TEST
		| SV_STATE_RUNNING ))
	{
		return;
	}

	if( m_svSecurityMgr.SVValidate( SECURITY_POINT_MODE_MENU_STOP ) == S_OK )
	{
		OnStop();
	}
}


////////////////////////////////////////////////////////////////////////////////
// .Title       : OnEnvironmentSettings
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
void SVObserverApp::OnEnvironmentSettings() 
{
	bool l_bShowDDETabs = false;

	SVSVIMStateClass::AddState(SV_STATE_EDITING); /// do this before calling validate for security as it may display a logon dialog!
	if( m_svSecurityMgr.SVValidate( SECURITY_POINT_EXTRAS_MENU_ADDITIONAL_ENVIRON ) == S_OK )
	{
		SVEnvironmentSettingsDialogClass cfdDlg;

		cfdDlg.StartLastAutomatically = m_ShouldRunLastEnvironmentAutomatically;

		if( cfdDlg.DoModal() == IDOK )
		{
			m_ShouldRunLastEnvironmentAutomatically = cfdDlg.StartLastAutomatically;

			WriteProfileInt( _T( "Settings" ), 
				_T( "Run Last Configuration Automatically" ), 
				m_ShouldRunLastEnvironmentAutomatically ? 1 : 0
				);
		}
	}
	SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
}

void SVObserverApp::OnUpdateThreadAffinitySetup(CCmdUI* PCmdUI)
{
	long lPipeCount=0;
	SVThreadManager::Instance().GetPipeCount(lPipeCount);
	PCmdUI->Enable( lPipeCount >=8 && !SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) );
}

void SVObserverApp::OnThreadAffinitySetup()
{
	SVSVIMStateClass::AddState(SV_STATE_EDITING);
	SVThreadInfoDlg dlg;
	dlg.DoModal();
	SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
}

void SVObserverApp::OnUpdateModeRun( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION | SV_STATE_EDIT | SV_STATE_READY )  &&	
		! SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) &&
		m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_MODE_MENU_RUN ));
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnFileCloseConfig
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
void SVObserverApp::OnFileCloseConfig() 
{
	if( m_svSecurityMgr.SVValidate( SECURITY_POINT_FILE_MENU_CLOSE_CONFIGURATION ) == S_OK )
	{
		SVSVIMStateClass::RemoveState( SV_STATE_EDIT );

		ValidateMRUList();

		// Check if current config is modified, ask for saving and try to close
		DestroyConfig();

		ValidateMRUList();
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnFileOpen
// -----------------------------------------------------------------------------
// .Description : 
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
void SVObserverApp::OnFileOpen() 
{
	if( m_svSecurityMgr.SVValidate( SECURITY_POINT_FILE_MENU_SELECT_CONFIGURATION ) == S_OK )
	{
		CWinApp::OnFileOpen();
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnFileSaveAll
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
void SVObserverApp::OnFileSaveAll() 
{
	if( m_svSecurityMgr.SVValidate( SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION ) == S_OK )
	{
		OnFileSaveConfig();
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnEditIOList
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
void SVObserverApp::OnEditIOList() 
{
	if( GetIODoc() )
	{
		// Get the active MDI child window.             
		POSITION pos = GetIODoc()->GetFirstViewPosition();
		CView *pView = GetIODoc()->GetNextView( pos );

		CMDIChildWnd *pFrame = (CMDIChildWnd*) pView->GetParentFrame();
		pFrame->MDIActivate();
	}// end if
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnUpdateEditEnvironment
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
void SVObserverApp::OnUpdateEditEnvironment( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( SVSVIMStateClass::CheckState( SV_STATE_READY ) &&
		! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION )  &&
		OkToEdit() );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnUpdateEditIOList
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
void SVObserverApp::OnUpdateEditIOList( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST  | SV_STATE_REGRESSION ) &&
		SVSVIMStateClass::CheckState( SV_STATE_EDIT ) && GetIODoc() != NULL );
}

void SVObserverApp::OnUpdateFileClose( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( !SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION )
		&& m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_FILE_MENU_CLOSE_CONFIGURATION ) );
}

void SVObserverApp::OnUpdateFileNew( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST  | SV_STATE_REGRESSION) &&
		m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_FILE_MENU_NEW )  && (TheSVOLicenseManager().HasMatroxLicense()) );
}

void SVObserverApp::OnUpdateFileOpen( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST  | SV_STATE_REGRESSION)
		&& m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_FILE_MENU_SELECT_CONFIGURATION )  && (TheSVOLicenseManager().HasMatroxLicense()) );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnUpdateFileOpenSVC
// -----------------------------------------------------------------------------
// .Description : 
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: void
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :15.10.1998	RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVObserverApp::OnUpdateFileOpenSVC( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION)
		&& m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_FILE_MENU_SELECT_CONFIGURATION ) && (TheSVOLicenseManager().HasMatroxLicense()) );
}

void SVObserverApp::OnUpdateFileSave( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST  | SV_STATE_REGRESSION)  
		&& SVSVIMStateClass::CheckState( SV_STATE_READY )
		&& m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION ));
}

void SVObserverApp::OnUpdateFileSaveAll( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION )
		&& SVSVIMStateClass::CheckState( SV_STATE_READY )
		&& m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION ));
}

void SVObserverApp::OnUpdateFileSaveAs( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST  | SV_STATE_REGRESSION)
		&& SVSVIMStateClass::CheckState( SV_STATE_READY )
		&& m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION_AS ));
}

void SVObserverApp::OnUpdateFileSaveCopyAs( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST  | SV_STATE_REGRESSION)  );
}

void SVObserverApp::OnUpdateFileSaveImage( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING  | SV_STATE_REGRESSION | SV_STATE_TEST)
		&& SVSVIMStateClass::CheckState( SV_STATE_READY )
		&& m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_FILE_MENU_SAVE_IMAGE ));
}

void SVObserverApp::OnUpdateFileUpdate( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION ) );
}

void SVObserverApp::OnUpdateExtrasAdditionalEnvironmentSettings( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION ) 
		&& m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_EXTRAS_MENU_ADDITIONAL_ENVIRON )  && (TheSVOLicenseManager().HasMatroxLicense()) );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnStop
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
void SVObserverApp::OnStop() 
{
	CWaitCursor wait;

	SVArchiveWritingDlg *pArchiveWriteDlg = nullptr;

	GetMainFrame()->SetStatusInfoText(_T(""));

	DisableTriggerSettings();

	if( !SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) )
	{
		return;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_STOPING ) )
	{
		if ( SVSVIMStateClass::CheckState( SV_STATE_STOP_PENDING ) )
		{
			SVSVIMStateClass::RemoveState( SV_STATE_STOP_PENDING );
		}
		return;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_STARTING ) )
	{
		SVSVIMStateClass::AddState( SV_STATE_STOP_PENDING );
		return;
	}

	if ( TheSVMemoryManager().ReservedBytes( SvO::ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME ) > 0 )
	{
		pArchiveWriteDlg = new SVArchiveWritingDlg;
		pArchiveWriteDlg->Create(IDD_DLG_ARCHIVETOOL_CLOSE_PROGRESS);
		pArchiveWriteDlg->ShowWindow(SW_SHOW);
		SVYieldPaintMessages();
	}

	SVSVIMStateClass::AddState( SV_STATE_UNAVAILABLE | SV_STATE_STOPING );
	SVSVIMStateClass::RemoveState( SV_STATE_READY | SV_STATE_RUNNING | SV_STATE_STOP_PENDING );

	SVObjectManagerClass::Instance().SetState( SVObjectManagerClass::ReadWrite );

	SetThreadPriority( THREAD_PRIORITY_NORMAL );

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig )
	{
		pConfig->SetModuleReady( false );
	}

	// Signal Module Stop...
	OnStopAll();

	// Increment Offline Count
	m_OfflineCount++;

	SVDigitizerProcessingClass::Instance().RestoreLastCameraImage();

	SVPPQObject* pPPQ( nullptr );
	long lSize = pConfig->GetPPQCount( );
	for(long l = 0; l < lSize; l++ )
	{
		pPPQ = pConfig->GetPPQ( l );
		if( nullptr != pPPQ){ pPPQ->GoOffline(); }
	}
	// Stop the FailStatus Stream PPQ Listeners
	SVFailStatusStreamManager::Instance().RemovePPQObservers();

	SetAllIPDocumentsOffline();

	m_dirty_triggers.clear();

	if( IsProductTypeRAID() )
	{
		m_IntelRAID.UpdateStatus();

		if ( SVSVIMStateClass::CheckState( SV_STATE_RAID_FAILURE ) )
		{
			pConfig->SetRaidErrorBit( true );
		}
		else
		{
			pConfig->SetRaidErrorBit( false );
		}
	}
	else
	{
		pConfig->SetRaidErrorBit( true );
	}

	CString l_strTrigCnts;
	GetTriggersAndCounts( l_strTrigCnts );

	//add message to event viewer - gone off-line
	SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
	Exception.setMessage( SVMSG_SVO_28_SVOBSERVER_GO_OFFLINE, l_strTrigCnts, StdMessageParams );

	SVSVIMStateClass::AddState( SV_STATE_READY );
	SVSVIMStateClass::RemoveState( SV_STATE_UNAVAILABLE | SV_STATE_STOPING );
	SVSVIMStateClass::RemoveState( SV_STATE_TEST );
	SVSVIMStateClass::RemoveState( SV_STATE_REGRESSION );

	SVCommandStreamManager::Instance().DisableAllInspections();

	if ( SVSVIMStateClass::CheckState( SV_STATE_START_PENDING ) )
	{
		PostMessage ( m_pMainWnd->m_hWnd, WM_COMMAND, MAKEWPARAM (ID_MODE_RUN, 0), 0);
	}

	if ( pArchiveWriteDlg )
	{
		pArchiveWriteDlg->DestroyWindow();
		delete pArchiveWriteDlg;
	}
}

void SVObserverApp::OnUpdateEditDelete( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_REGRESSION )  &&
		OkToEdit() );
}

void SVObserverApp::OnUpdateEditEditTool( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST  | SV_STATE_REGRESSION) &&
		OkToEdit() );
}

void SVObserverApp::OnUpdateEditEditToolSetCondition( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION )  &&
		OkToEdit() );
}

void SVObserverApp::OnUpdateEditRemoteInputs( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION ) &&
		OkToEdit() );
	SVInputObjectList* l_pInputs( nullptr );
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig )
	{
		l_pInputs = pConfig->GetInputObjectList( );
	}

	bool l_bRemoteInputsExits = false;
	if( nullptr != l_pInputs )
	{
		long lSize = 0;
		l_pInputs->GetRemoteInputCount( lSize );
		if( lSize > 0 )
		{
			l_bRemoteInputsExits = true;
			PCmdUI->SetText( "Edit Remote Inputs" );
		}
	}
	if( l_bRemoteInputsExits )
		PCmdUI->SetText( "Edit Remote Inputs" );
	else
		PCmdUI->SetText( "Add Remote Inputs" );

	//@HACK
	// This code to add menu items for the Published Results does not belong here
	// but we need a message when the edit menu is selected.  So this works until we can find a
	// message for the Edit popup UpdateUI or similar.

	// Get Menu handle
	HMENU hmen = GetMenu( GetMainWnd()->m_hWnd );
	if( nullptr != pConfig )
	{
		// The IO menu is limited to 100 Published results menu items.
		// There is no limit to the number of inspections that can be created 
		// but we are only displaying up to 100 menu items for inspections on the IO page.
		// If there are more than 100, then the user will have to go to the inspection to edit published results
		// If more are to be supported, then these resources for the ID_EDIT_PUBLISHEDRESULTS_LIMIT will need changed. 
		SVInspectionProcessPtrList l_Inspections; // Get Inspections
		pConfig->GetInspections( l_Inspections );
		RemoveMenu(hmen, "&Edit\\Published Results" ); // start empty.
		for( UINT i = 0; i < static_cast< UINT >( l_Inspections.size() ) && i < (ID_EDIT_PUBLISHEDRESULTS_LIMIT - ID_EDIT_PUBLISHEDRESULTS_BASE + 1); i++ )
		{
			// Add a menu for each inspection.
			CString strName;
			strName.Format("&Edit\\Published Results\\%s",l_Inspections[i]->GetName());
			AddMenuItem(hmen, strName, static_cast< UINT >( ID_EDIT_PUBLISHEDRESULTS_BASE + i ) );
		}
	}
}

void SVObserverApp::OnUpdateEditAddRemoteOutputs(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&	
		SVSVIMStateClass::CheckState( SV_STATE_EDIT ) );

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig && pConfig->GetRemoteOutputGroupCount() == 0 )
	{
		pCmdUI->SetText( "Add Remote Output" );
	}
	else
	{
		pCmdUI->SetText( "Edit Remote Output" );
	}
}

void SVObserverApp::OnEditMonitorList()
{
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig )
	{
		if (pConfig->SetupRemoteMonitorList())
		{
			SVSVIMStateClass::AddState(SV_STATE_MODIFIED);
			SVIODoc* pIODoc = GetIODoc();
			if (pIODoc)
			{
				pIODoc->SetModifiedFlag();
			}
		}
	}
}

void SVObserverApp::OnUpdateEditAddMonitorList(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&	
		SVSVIMStateClass::CheckState( SV_STATE_EDIT ) );
	
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if ( nullptr != pConfig && 0 < pConfig->GetRemoteMonitorList().size())
	{
		pCmdUI->SetText( _T("Edit Monitor List") );
	}
	else
	{
		pCmdUI->SetText( _T("Add Monitor List") );
	}
}

void SVObserverApp::OnUpdateExtrasLightReference( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_REGRESSION ) &&
		OkToEdit());
}

void SVObserverApp::OnUpdateFilePrintPreview( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_REGRESSION ) 
		&& m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_FILE_MENU_PRINT ) );
}

void SVObserverApp::OnUpdateFilePrintSetup( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_REGRESSION | SV_STATE_TEST ) 
		&& m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_FILE_MENU_PRINT_SETUP ) );
}

void SVObserverApp::OnUpdateExtrasTestOutputs( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION )
		&& m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_EXTRAS_MENU_TEST_OUTPUTS ) );
}

void SVObserverApp::OnUpdateHelp( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) );
}

void SVObserverApp::OnUpdateHelpFinder( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING  ) );
}

void SVObserverApp::OnUpdateModeStopTest( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( SVSVIMStateClass::CheckState( SV_STATE_TEST )  
		&& m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_MODE_MENU_TEST ) );
}

void SVObserverApp::OnUpdateModeTest( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ( SVSVIMStateClass::CheckState( SV_STATE_TEST | SV_STATE_REGRESSION | SV_STATE_EDIT | SV_STATE_READY | SV_STATE_RUNNING  ) ) &&
		m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_MODE_MENU_TEST ) );

	PCmdUI->SetCheck( SVSVIMStateClass::CheckState( SV_STATE_TEST ) && 
		!SVSVIMStateClass::CheckState( SV_STATE_REGRESSION ) );
}

void SVObserverApp::OnUpdateModeTestBtn(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(( SVSVIMStateClass::CheckState( SV_STATE_REGRESSION | SV_STATE_EDIT | SV_STATE_READY ) ) &&	 
		SVSVIMStateClass::CheckState( SV_STATE_READY ) &&
		!SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&
		m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_MODE_MENU_TEST ));
}

void SVObserverApp::OnUpdateNextPane( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_REGRESSION ) );
}

void SVObserverApp::OnUpdatePrevPane( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_REGRESSION ) );
}

void SVObserverApp::OnUpdateWindowArrange( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_REGRESSION ) );
}

void SVObserverApp::OnUpdateWindowCascade( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_REGRESSION ) );
}

void SVObserverApp::OnUpdateWindowNew( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_REGRESSION ) );
}

void SVObserverApp::OnUpdateWindowTileHorz( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_REGRESSION ) );
}

void SVObserverApp::OnUpdateWindowTileVert( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_REGRESSION ) );
}

void SVObserverApp::OnUpdateAddShiftTool( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&	
		OkToEdit() &&
		IsMonochromeImageAvailable() );
}

void SVObserverApp::OnUpdateAddWindowTool( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&	
		OkToEdit() &&
		IsMonochromeImageAvailable() );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnUpdateAppExit
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
//  :16.10.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVObserverApp::OnUpdateAppExit( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_REGRESSION ) 
		&& m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_FILE_MENU_EXIT ));	
}

void SVObserverApp::OnUpdateAppAbout( CCmdUI* PCmdUI ) 
{
	// BRW - Can we delete this method?
	//PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) );
}

void SVObserverApp::OnUpdateAddLoadImageTool( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&	
		OkToEdit() &&
		IsMonochromeImageAvailable()  );
}

void SVObserverApp::OnUpdateAddImageTool( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&	
		OkToEdit() &&
		IsMonochromeImageAvailable());
}

void SVObserverApp::OnUpdateAddArchiveTool( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&	
		OkToEdit() &&
		IsMonochromeImageAvailable() );
}

void SVObserverApp::OnUpdateAddMathTool( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&	
		OkToEdit() &&
		IsMonochromeImageAvailable() );
}

void SVObserverApp::OnUpdateAddStatisticsTool( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&	
		OkToEdit() &&
		IsMonochromeImageAvailable() );
}

void SVObserverApp::OnFilePrintConfig() 
{
	if( m_svSecurityMgr.SVValidate( SECURITY_POINT_FILE_MENU_PRINT ) == S_OK )
	{
		if ( ! SVSVIMStateClass::CheckState( SV_STATE_READY | SV_STATE_RUNNING ) || 
			CString( getConfigFullFileName() ).IsEmpty() )
		{
			return;
		}

		SVConfigurationPrint printConfig;
		printConfig.DoPrintConfig();
	}
}

void SVObserverApp::OnSVOFilePrintSetup()
{
	if( m_svSecurityMgr.SVValidate( SECURITY_POINT_FILE_MENU_PRINT_SETUP ) == S_OK )
	{
		OnFilePrintSetup();
	}
}

void SVObserverApp::OnExtrasLogin() 
{
	HRESULT hr = m_svSecurityMgr.SVLogon();

	// Check if Edit Mode should continue.
	if( SVSVIMStateClass::CheckState( SV_STATE_EDIT ) )
	{	// If this user is not allowed to edit then remove the edit mode
		if( !m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_MODE_MENU_EDIT_TOOLSET ))
		{
			SVSVIMStateClass::RemoveState( SV_STATE_EDIT );
		}
		else
		{	// If they are still allowed then select the current tool.
			SVMainFrame* pWndMain = ( SVMainFrame* )GetMainWnd();
			if(pWndMain)
			{
				pWndMain->PostMessage(SV_SET_TOOL_SELECTED_IN_TOOL_VIEW, (WPARAM)TRUE);
				// Logged on User changed
				pWndMain->PostMessage( SV_LOGGED_ON_USER_CHANGED, 0 );
			}
		}
	}
}

void SVObserverApp::OnUpdateExtrasLogin( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( SVSVIMStateClass::CheckState( SV_STATE_AVAILABLE | SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_READY | SV_STATE_REGRESSION) );

	if( !m_svSecurityMgr.SVGetUseLogon() )
	{	// If not logon mode then remove the login and logout menu items.
		CMenu *pMenu;
		CWnd* pWindow = AfxGetMainWnd();
		pMenu = pWindow->GetMenu();
		pMenu->RemoveMenu( ID_EXTRAS_LOGIN, MF_BYCOMMAND );
		pMenu->RemoveMenu( ID_EXTRAS_LOGOUT, MF_BYCOMMAND );
	}
	if( SVThreadManager::Instance().IsThreadManagerInstalled() == 0 )
	{
		CMenu *pMenu;
		CWnd* pWindow = AfxGetMainWnd();
		pMenu = pWindow->GetMenu();
		pMenu->RemoveMenu( ID_EXTRAS_THREAD_AFFINITY, MF_BYCOMMAND );
	}
}

void SVObserverApp::OnUpdateViewToolBuffers( CCmdUI* PCmdUI )
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING )  );
	PCmdUI->SetCheck( 0 );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnUpdateSetupMode
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
//  :15.10.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVObserverApp::OnUpdateSetupMode( CCmdUI* PCmdUI )
{
	PCmdUI->Enable( SVSVIMStateClass::CheckState( SV_STATE_READY ) &&
		!SVSVIMStateClass::CheckState( SV_STATE_SECURED | SV_STATE_REGRESSION ) );
}

void SVObserverApp::OnUpdateAddTransformationTool( CCmdUI* PCmdUI )
{
	PCmdUI->Enable( !SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&	
		OkToEdit() &&
		IsMonochromeImageAvailable() );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnGoOffline
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
//  :15.10.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVObserverApp::OnGoOffline() 
{
	if( !SVSVIMStateClass::CheckState( SV_STATE_EDIT 
		| SV_STATE_REGRESSION
		| SV_STATE_TEST
		| SV_STATE_RUNNING ))
	{
		return;
	}

	SVSVIMStateClass::RemoveState( SV_STATE_EDIT );

	DeselectTool();

	GetMainFrame()->SetStatusInfoText(_T(""));

	if ( SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
	{
		// Dual Security access point
		if( m_svSecurityMgr.SVValidate( SECURITY_POINT_MODE_MENU_STOP, 
			SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE ) == S_OK )
		{
			OnStop();
		}
	}
	else if( m_svSecurityMgr.SVValidate( SECURITY_POINT_MODE_MENU_STOP ) == S_OK )
	{
		if ( SVSVIMStateClass::CheckState( SV_STATE_REGRESSION ) )
		{
			StopRegression();
		}

		if ( SVSVIMStateClass::CheckState( SV_STATE_TEST ) )
		{
			OnStop();
		}

	}// end if ( SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnUpdateGoOffline
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
//  :15.10.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVObserverApp::OnUpdateGoOffline( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ( SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION | SV_STATE_EDIT | SV_STATE_READY ) ) &&	
		SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_READY ) &&
		( m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE ) || 
		m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_MODE_MENU_STOP ) ));

	PCmdUI->SetCheck( !SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_EDIT | SV_STATE_REGRESSION ) 
		&& SVSVIMStateClass::CheckState( SV_STATE_READY));
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnGoOnline
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
//  :15.10.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVObserverApp::OnGoOnline() 
{
	SvStl::MessageContainer exceptionContainer;
	ExtrasEngine::Instance().ExecuteAutoSaveIfAppropriate(true);

	//clear the tool error map. will be filled when the inspections/tools gets validated
	SVVisionProcessorHelper::Instance().ClearToolErrorMap();
	
	long l_lPrevState = 0;

	if( SVSVIMStateClass::CheckState( SV_STATE_EDIT ))
	{
		l_lPrevState = SV_STATE_EDIT;
	}

	if( SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) ) { return; }

	if( m_svSecurityMgr.SVValidate( SECURITY_POINT_MODE_MENU_RUN ) == S_OK )
	{
		if ( SVSVIMStateClass::CheckState( SV_STATE_REGRESSION ) )
		{
			StopRegression();
		}

		if( SVSVIMStateClass::CheckState( SV_STATE_TEST ) )
		{
			OnStop();
		}

		if ( SVSVIMStateClass::CheckState( SV_STATE_READY ) && 
			!SVSVIMStateClass::CheckState( SV_STATE_TEST | SV_STATE_RUNNING ))
		{

			HRESULT l_hrStatus = S_OK;

			SVSVIMStateClass::RemoveState( SV_STATE_EDIT );
			DeselectTool();

			if ( m_hrHardwareFailure == S_OK )
			{
				if ( CheckSVIMType() )
				{
					CWaitCursor wait;

					try
					{
						l_hrStatus = Start();
					}
					catch (const SvStl::MessageContainer& rExp)
					{
						exceptionContainer = rExp;
						l_hrStatus = SV_CAN_GO_ONLINE_FAILURE_ACQUISITION;
					}
				}
				else
				{
					SVStringArray	msgList;
					msgList.push_back(SVString(m_csProcessor));
					msgList.push_back(SVString(m_csFrameGrabber));
					msgList.push_back(SVString(m_csIOBoard));
					msgList.push_back(SVString(m_csOptions));
					SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_SVObserver_CannotRun_WrongModelNumber, msgList, StdMessageParams, SvOi::Err_10121 );
					SVSVIMStateClass::AddState( l_lPrevState );
				}
			}
			else
			{
				CString l_csItem;
				
				if ( ( m_hrHardwareFailure & SV_HARDWARE_FAILURE_IO ) == SV_HARDWARE_FAILURE_IO )
				{
					l_csItem = GetDigitalBoardName();
				}
				else if ( ( m_hrHardwareFailure & SV_HARDWARE_FAILURE_TRIGGER ) == SV_HARDWARE_FAILURE_TRIGGER )
				{
					l_csItem = GetTriggerBoardName();
				}
				else if ( ( m_hrHardwareFailure & SV_HARDWARE_FAILURE_SOFTWARETRIGGER ) == SV_HARDWARE_FAILURE_SOFTWARETRIGGER )
				{
					l_csItem = GetSoftwareTriggerBoardName();
				}
				else if ( ( m_hrHardwareFailure & SV_HARDWARE_FAILURE_ACQUISITION ) == SV_HARDWARE_FAILURE_ACQUISITION )
				{
					l_csItem = GetAcquisitionBoardName();
				}
				else if ( ( m_hrHardwareFailure & SV_HARDWARE_FAILURE_FILEACQUISITION ) == SV_HARDWARE_FAILURE_FILEACQUISITION )
				{
					l_csItem = GetFileAcquisitionBoardName();
				}
				else
				{
					l_csItem = _T( "Unknown Item" );
				}

				SVStringArray msgList;
				msgList.push_back(SVString(m_csProcessor));
				msgList.push_back(SVString(m_csFrameGrabber));
				msgList.push_back(SVString(m_csIOBoard));
				msgList.push_back(SVString(m_csOptions));
				msgList.push_back(SVString(l_csItem));
				SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_SVObserver_WrongModelNumber, msgList, StdMessageParams, SvOi::Err_10122 );
				SVSVIMStateClass::AddState( l_lPrevState );
			}

			if ( SVSVIMStateClass::CheckState( SV_STATE_STOP_PENDING ) )
			{
				PostMessage( m_pMainWnd->m_hWnd, WM_COMMAND, MAKEWPARAM (ID_STOP, 0), 0);
			}
			else if ( l_hrStatus != S_OK )
			{
				SvOi::MessageTextEnum messageId = SvOi::Tid_Empty;
				SVStringArray msgList;

				if ( ( l_hrStatus & SV_CAN_GO_ONLINE_FAILURE_TRIGGER ) == 
					SV_CAN_GO_ONLINE_FAILURE_TRIGGER )
				{
					messageId = SvOi::Tid_CanGoOnlineFailure_Trigger;
				}
				else if ( ( l_hrStatus & SV_CAN_GO_ONLINE_FAILURE_ACQUISITION ) == 
					SV_CAN_GO_ONLINE_FAILURE_ACQUISITION )
				{
					messageId = SvOi::Tid_CanGoOnlineFailure_Acquisition;
				}
				else if ( ( l_hrStatus & SV_CAN_GO_ONLINE_FAILURE_INSPECTION ) == 
					SV_CAN_GO_ONLINE_FAILURE_INSPECTION )
				{
					bool bShowToolError = false;
					if ( SVVisionProcessorHelper::Instance().GetNumberOfToolErrors() > 0 )
					{
						SVGUID ToolGuid;
						SVString sToolErrorTxt;
						if ( SVVisionProcessorHelper::Instance().GetFirstErrorMessage(ToolGuid,sToolErrorTxt) )
						{
							bShowToolError = true;
							SVString sToolName = SVObjectManagerClass::Instance().GetCompleteObjectName(ToolGuid);
							msgList.push_back(sToolName);
							msgList.push_back(sToolErrorTxt);
							messageId = SvOi::Tid_CanGoOnlineFailure_InspectionTool;
						}
					}
					if (!bShowToolError)
					{
						messageId = SvOi::Tid_CanGoOnlineFailure_Inspection;
					}
				}
				else if ( ( l_hrStatus & SV_GO_ONLINE_FAILURE_RECYCLE_PRODUCT ) == 
					SV_GO_ONLINE_FAILURE_RECYCLE_PRODUCT )
				{
					messageId = SvOi::Tid_GoOnlineFailure_RecycleProduct;
				}
				else if ( ( l_hrStatus & SV_GO_ONLINE_FAILURE_INSPECTION ) == 
					SV_GO_ONLINE_FAILURE_INSPECTION )
				{
					messageId = SvOi::Tid_GoOnlineFailure_Inspection;
				}
				else if ( ( l_hrStatus & SV_GO_ONLINE_FAILURE_ACQUISITION ) == 
					SV_GO_ONLINE_FAILURE_ACQUISITION )
				{
					messageId = SvOi::Tid_GoOnlineFailure_Acquisition;
				}
				else if ( ( l_hrStatus & SV_GO_ONLINE_FAILURE_TRIGGER ) == 
					SV_GO_ONLINE_FAILURE_TRIGGER )
				{
					messageId = SvOi::Tid_GoOnlineFailure_Trigger;
				}
				else
				{
					messageId = SvOi::Tid_CanGoOnlineFailure_Unknown;
				}
				INT_PTR Res(0);
				SvStl::MessageMgrDisplayAndNotify Exception(SvStl::LogAndDisplay);
				if (0 == exceptionContainer.getMessage().m_MessageCode)
				{
					Res = Exception.setMessage(SVMSG_SVO_54_EMPTY, messageId, msgList, StdMessageParams, SvOi::Err_45000);
				}
				else
				{
					Res = Exception.setMessage(exceptionContainer.getMessage());
				}
				SVSVIMStateClass::AddState( l_lPrevState );
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnUpdateGoOnline
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
//  :15.10.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVObserverApp::OnUpdateGoOnline( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION | SV_STATE_EDIT  | SV_STATE_READY ) &&	
		SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_READY ) &&
		m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_MODE_MENU_RUN ) );

	PCmdUI->SetCheck( SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) );
}

void SVObserverApp::OnUpdateFilePrintConfig( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_REGRESSION | SV_STATE_TEST )  
		&& m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_FILE_MENU_PRINT ));
}

void SVObserverApp::OnExtrasLogout() 
{
	// Force log out, regardless auto login mode...
	m_svSecurityMgr.SVLogout();

	if( SVSVIMStateClass::CheckState( SV_STATE_EDIT ) )
	{
		SVSVIMStateClass::RemoveState( SV_STATE_EDIT );
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_REGRESSION ) )
	{
		StopRegression();
	}

	//
	// We need to deselect any tool that might be set for operator move.
	//
	SVMainFrame* pWndMain = ( SVMainFrame* )GetMainWnd();
	if( nullptr != pWndMain )
	{
		pWndMain->PostMessage( SV_SET_TOOL_SELECTED_IN_TOOL_VIEW, ( WPARAM )FALSE );   // Deselect any tool selected for move.
	}
}

void SVObserverApp::OnUpdateExtrasLogout( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( m_svSecurityMgr.SVIsLoggedOn());
}

/////////////////////////////////////////////////////////////////////////////
//
//
// Command message processor for range: ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16
//
BOOL SVObserverApp::OnOpenRecentFile( UINT nID ) 
{
	BOOL l_bOk = FALSE;
	if( m_svSecurityMgr.SVValidate( SECURITY_POINT_FILE_MENU_RECENT_CONFIGURATIONS ) == S_OK)
	{
		ValidateMRUList();

		if ( !SVSVIMStateClass::CheckState( SV_STATE_UNAVAILABLE ) )
		{
			BOOL bRunning = SVSVIMStateClass::CheckState( SV_STATE_RUNNING );

			if ( !bRunning )
			{
				l_bOk = OpenConfigFileFromMostRecentList(nID);
				if( l_bOk && !m_svSecurityMgr.SVIsSecured( SECURITY_POINT_MODE_MENU_EDIT_TOOLSET ) )
				{
					SetModeEdit( true ); // Set Edit mode
				}
			}
		}
	}

	// @HACK - This method is always returning TRUE.
	return TRUE;
}

// This function is used when the m_ShouldRunLastEnvironmentAutomatically is set.
void SVObserverApp::OnRunMostRecentMRU()
{
	ValidateMRUList();

	if ( !SVSVIMStateClass::CheckState( SV_STATE_UNAVAILABLE ) )
	{
		BOOL bRunning = SVSVIMStateClass::CheckState( SV_STATE_RUNNING );

		if ( !bRunning )
		{
			OpenConfigFileFromMostRecentList(ID_FILE_MRU_FILE1);
		}
	}
}

void SVObserverApp::OnUpdateRecentFileMenu( CCmdUI* PCmdUI ) 
{
	CWinApp::OnUpdateRecentFileMenu( PCmdUI );

	BOOL bEnable = ( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_REGRESSION | SV_STATE_TEST )
		&& m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_FILE_MENU_RECENT_CONFIGURATIONS ) && (TheSVOLicenseManager().HasMatroxLicense()) );

	for( long l = ID_FILE_MRU_FILE1; !bEnable && l < (long)(PCmdUI->m_nID); l++ )
		PCmdUI->m_pMenu->EnableMenuItem( l, MF_BYCOMMAND | MF_GRAYED );
}

void SVObserverApp::OnUpdateExtraUtilities( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_EXTRAS_MENU_UTILITIES_RUN ) );
}
					
void SVObserverApp::OnUpdateEditPublishedResults(CCmdUI* PCmdUI) 
{
	PCmdUI->Enable( SVSVIMStateClass::CheckState( SV_STATE_READY )
		&& SVSVIMStateClass::CheckState( SV_STATE_EDIT ) );
}


////////////////////////////////////////////////////////////////////////////////
// .Title       : OnAppExit
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
//  :16.10.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVObserverApp::OnAppExit() 
{
	bool l_bAllowAccess = true;

	if ( SVSVIMStateClass::CheckState(SV_STATE_REGRESSION) )
	{
		return;
	}

	if(SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
	{
		if( m_svSecurityMgr.SVValidate( SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE, 
			SECURITY_POINT_FILE_MENU_EXIT ) == S_OK )
		{
			OnStop();
			l_bAllowAccess = true;
		}
		else
		{
			l_bAllowAccess = false;
		}
	}

	if( l_bAllowAccess )
	{
		CWinApp::OnAppExit();
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnFileSaveAsSVC
// -----------------------------------------------------------------------------
// .Description : 
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: void
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :15.10.1998	RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVObserverApp::OnFileSaveAsSVC() 
{
	if( m_svSecurityMgr.SVValidate( SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION_AS ) == S_OK )
	{
		if ( SVSVIMStateClass::CheckState( SV_STATE_READY ) )
		{
			// Call save as svx with file dialog...
			fileSaveAsSVX();
		}
	}
}

void SVObserverApp::OnUpdatePublishedResultsPicker(CCmdUI* PCmdUI) 
{
	PCmdUI->Enable( SVSVIMStateClass::CheckState( SV_STATE_READY )
		&& SVSVIMStateClass::CheckState( SV_STATE_EDIT ) );
}

void SVObserverApp::OnUpdatePublishedImagesPicker(CCmdUI* PCmdUI) 
{
	PCmdUI->Enable( SVSVIMStateClass::CheckState( SV_STATE_READY )
		&& SVSVIMStateClass::CheckState( SV_STATE_EDIT ) );
}

void SVObserverApp::OnUpdateResultsPicker(CCmdUI* PCmdUI) 
{
	PCmdUI->Enable( ((SVSVIMStateClass::CheckState( SV_STATE_READY ) 
		&& SVSVIMStateClass::CheckState( SV_STATE_EDIT ))
		|| SVSVIMStateClass::CheckState(SV_STATE_RUNNING)
		|| SVSVIMStateClass::CheckState(SV_STATE_REGRESSION)
		|| SVSVIMStateClass::CheckState(SV_STATE_TEST)));
}

void SVObserverApp::OnExtrasUtilitiesEdit() 
{
	SVUtilitiesClass util;
	CWnd* pWindow;
	CMenu* pMenu;
	CString szMenuText;
	SVSVIMStateClass::AddState(SV_STATE_EDITING); /// do this before calling validate for security as it may display a logon dialog!
	if( m_svSecurityMgr.SVValidate( SECURITY_POINT_EXTRAS_MENU_UTILITIES_SETUP ) == S_OK )
	{
		pWindow = AfxGetMainWnd();
		pMenu = pWindow->GetMenu();
		szMenuText = _T( "&Utilities" );

		if ( pMenu = util.FindSubMenuByName( pMenu, szMenuText ) )
		{
			util.SetupUtilities( pMenu );
			UpdateAllMenuExtrasUtilities();
		}
	}
	else
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_SVObserver_AuthorizationFailed_Modification, StdMessageParams, SvOi::Err_10123 );
	}
	SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
}

void SVObserverApp::OnUpdateAddPolarUnwrapTool(CCmdUI* PCmdUI) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&	
		OkToEdit() &&
		IsMonochromeImageAvailable() );
}

void SVObserverApp::OnUpdateAddAcquisitionTool( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&	
		OkToEdit() &&
		IsMonochromeImageAvailable() );
}

void SVObserverApp::OnUpdateResumeFreeze( CCmdUI* PCmdUI )  
{
	PCmdUI->Enable( TRUE );
}

void SVObserverApp::OnUpdateAddColorTool( CCmdUI* PCmdUI ) 
{
	BOOL Enabled = ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST );
	// Check current user access...
	Enabled = Enabled && TheSVObserverApp.OkToEdit();

	if( Enabled )
	{
		Enabled = FALSE;
		CMDIChildWnd* pMDIChild;
		if( m_pMainWnd && ( pMDIChild = ( ( CMDIFrameWnd* ) m_pMainWnd )->MDIGetActive() ) )
		{
			SVIPDoc* pCurrentDocument = dynamic_cast< SVIPDoc* > ( pMDIChild->GetActiveDocument() );
			if( nullptr != pCurrentDocument && pCurrentDocument->IsColorInspectionDocument() )
			{
				Enabled = TRUE;
			}
		}
	}

	PCmdUI->Enable( Enabled );
}

void SVObserverApp::OnUpdateRegressionTest(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( SVSVIMStateClass::CheckState( SV_STATE_REGRESSION ) );
}

void SVObserverApp::OnRunUtility( UINT uiId )
{
	SVUtilitiesClass util;

	if( m_svSecurityMgr.SVValidate( SECURITY_POINT_EXTRAS_MENU_UTILITIES_RUN ) == S_OK )
	{
		util.RunUtility (&m_svSecurityMgr, uiId);
	}
	else
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_SVObserver_AuthorizationFailed_Execution, StdMessageParams, SvOi::Err_10124 );
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnRCGoOffline
// -----------------------------------------------------------------------------
// .Description : ...
//              : NOTE: Be careful by implementing an update function for
//				:		for this command! RC should be able to have access
//				:		all the time!
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
//  :17.10.1998 RO			First Implementation
//	:11.10.1999 RO			BugFix: OnStop() must be called! Otherwise even if
//							we are offline this function is not setting the 
//							SVRCVariable right!
////////////////////////////////////////////////////////////////////////////////
void SVObserverApp::OnRCGoOffline() 
{
	if ( SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
	{
		OnStop();

		Sleep (1000);  // wait for running threads to quiesce
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnRCGoOnline
// -----------------------------------------------------------------------------
void SVObserverApp::OnRCGoOnline() 
{
	if ( !SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
	{
		SetMode( SVIM_MODE_ONLINE );
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnRCSaveAllAndGetConfig
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
void SVObserverApp::OnRCSaveAllAndGetConfig()
{
	// Saves the current loaded config completely, transfers the CurrentPathName to 
	// SVRCComm.Dll and at least closes the current config, because the opened 
	// SVObserver documents cannot be transfered to SVFocus!

	BOOL bRunning = SVSVIMStateClass::CheckState( SV_STATE_RUNNING );

	if ( bRunning )
	{
		OnRCGoOffline();
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_READY ) )
	{
		CString csConfigPath;
		SVFileNameManagerClass svFileManager;

		BOOL bModified = SVSVIMStateClass::CheckState( SV_STATE_MODIFIED );

		csConfigPath = svFileManager.GetConfigurationPathName();

		if ( ! csConfigPath.IsEmpty() )
		{
			svFileManager.SetConfigurationPathName( NULL );
		}

		SVFileNameClass svFileName;

		svFileName.SetFullFileName( getConfigFullFileName() );
		svFileName.SetPathName( _T( "C:\\RUN" ) );
		svFileName.SetExtension( _T( ".svx" ) );

		fileSaveAsSVX( svFileName.GetFullFileName() );

		SVRCSetSVCPathName( getConfigFullFileName() );

		if ( ! csConfigPath.IsEmpty() )
		{
			svFileManager.SetConfigurationPathName( csConfigPath );
		}

		if ( bModified )
		{
			SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
		}

		if ( bRunning )
		{
			OnRunMode();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnRCCloseAndCleanUpDownloadDirectory
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
void SVObserverApp::OnRCCloseAndCleanUpDownloadDirectory()
{
	// Returns Nonzero to SVRC Module via SVRCResult, if it is successfully.
	// Otherwise it returns 0.

	// Closes loaded configuration
	// Cleans up execution directory ( download directory )

	// Close config immediately, without hint or user message...
	DestroyConfig( FALSE, TRUE );	// Close config immediately
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnUpdateAllIOViews
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
void SVObserverApp::OnUpdateAllIOViews()
{
	if ( GetIODoc() && 
		SVSVIMStateClass::CheckState( SV_STATE_READY | SV_STATE_RUNNING ) &&
		!SVSVIMStateClass::CheckState( SV_STATE_CANCELING ) )
	{
		GetIODoc()->UpdateAllViews( NULL );
	}
}

void SVObserverApp::OnUpdateAddExternalTool(CCmdUI* PCmdUI) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&	
		OkToEdit() &&
		IsMonochromeImageAvailable() );
}

void SVObserverApp::OnExtrasSecuritySetup() 
{
	SVSVIMStateClass::AddState(SV_STATE_EDITING); /// do this before calling validate for security as it may display a logon dialog!
	if( m_svSecurityMgr.SVValidate( SECURITY_POINT_EXTRAS_MENU_SECURITY_MANAGER ) == S_OK)
	{
		m_svSecurityMgr.SVSetupDialog();
		if( m_svSecurityMgr.SVGetUseLogon() )
		{
			CMenu *pMenu;
			CWnd* pWindow = AfxGetMainWnd();
			pMenu = pWindow->GetMenu();

			// Look for "Extras" menu.
			int pos = FindMenuItem( pMenu, "E&xtras");
			if( pos == -1 )
			{
				return;
			}

			CMenu* submenu = pMenu->GetSubMenu(pos);
			pos = FindMenuItem(submenu, "&Security Setup");
			if (pos > -1)
			{
				if( FindMenuItem( submenu, "User Log&out" ) == -1 )
				{
					submenu->InsertMenu(pos , MF_BYPOSITION, ID_EXTRAS_LOGOUT, "User Log&out");
					submenu->InsertMenu(pos , MF_BYPOSITION, ID_EXTRAS_LOGIN, "User &Login");
				}
			}
		}

		if( SVSVIMStateClass::CheckState( SV_STATE_EDIT ))
		{
			SVSVIMStateClass::RemoveState( SV_STATE_EDIT );
		}

		if ( SVSVIMStateClass::CheckState( SV_STATE_REGRESSION ) )
		{
			StopRegression();
		}
	}
	SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
}

void SVObserverApp::OnModeEdit() 
{
	bool l_bAllowAccess = false;

	if( SVSVIMStateClass::CheckState( SV_STATE_EDIT ) )
	{
		return;
	}

	// If Running, check if access to exit run.
	if ( SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
	{
		if( m_svSecurityMgr.SVValidate( SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE, 
			SECURITY_POINT_MODE_MENU_EDIT_TOOLSET ) == S_OK )
		{
			OnStop();
			l_bAllowAccess = true;
		}
	}
	else if( m_svSecurityMgr.SVValidate( SECURITY_POINT_MODE_MENU_EDIT_TOOLSET ) == S_OK )
	{
		l_bAllowAccess = true;
	}

	if( l_bAllowAccess )
	{
		if ( !SVSVIMStateClass::CheckState( SV_STATE_EDIT ) )
		{
			if ( SVSVIMStateClass::CheckState( SV_STATE_REGRESSION ) )
			{
				StopRegression();
			}

			if( SVSVIMStateClass::CheckState( SV_STATE_TEST ) )
			{
				OnStop();
			}
			
			if ( SVSVIMStateClass::CheckState( SV_STATE_READY ) )
			{
				SetModeEdit( true ); // Set Edit mode
			}
		}
	}
}

void SVObserverApp::OnUpdateModeEdit(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( (SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION | SV_STATE_EDIT | SV_STATE_READY ) ) &&	
		SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_READY ) &&
		m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_MODE_MENU_EDIT_TOOLSET ) );

	pCmdUI->SetCheck( SVSVIMStateClass::CheckState( SV_STATE_EDIT ) );
}

void SVObserverApp::OnTriggerSettings()
{
	SVSoftwareTriggerDlg::Instance().ShowWindow(SW_SHOW);
	// set dirty handler to the insert method of m_dirty_triggers set
	SVSoftwareTriggerDlg::Instance().SetDirtyHandler(boost::bind(&std::set<SVTriggerObject*>::insert, &m_dirty_triggers, _1));
}

void SVObserverApp::OnUpdateTriggerSettings(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}

void SVObserverApp::OnUpdateExtrasUtilitiesEdit(CCmdUI* pCmdUI) 
{
	if( pCmdUI->m_pSubMenu )
	{
		bool l_bEnable = m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_EXTRAS_MENU_UTILITIES_SETUP ) ||
			(m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_EXTRAS_MENU_UTILITIES_RUN ) && 
			!SVSVIMStateClass::CheckState( SV_STATE_RUNNING ));
		unsigned int l_uiGray = l_bEnable ? 0 : MF_GRAYED;
		pCmdUI->m_pMenu->EnableMenuItem( pCmdUI->m_nIndex, MF_BYPOSITION | l_uiGray);
	}
	else
	{
		pCmdUI->Enable( m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_EXTRAS_MENU_UTILITIES_SETUP ) && 
			!SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) );
	}
}

void SVObserverApp::OnUpdateAddLinearTool( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&	
		IsMonochromeImageAvailable() &&
		OkToEdit());
}

void SVObserverApp::OnUpdateAddRemoteInputTool( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&	
		IsMonochromeImageAvailable() &&
		OkToEdit());
}

void SVObserverApp::OnUpdateAddResizetool( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&	
		IsMonochromeImageAvailable() &&
		OkToEdit());
}

void SVObserverApp::OnUpdateAddRingBufferTool( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&	
		IsMonochromeImageAvailable() &&
		OkToEdit());
}

void SVObserverApp::OnUpdateAddCylindricalWarpTool( CCmdUI* pCmdUI ) 
{
	bool l_bEnable = ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&	
		OkToEdit() &&
		IsMonochromeImageAvailable() ;

	if( pCmdUI->m_pSubMenu )
	{
		unsigned int l_uiGray = l_bEnable ? 0 : MF_GRAYED;
		pCmdUI->m_pMenu->EnableMenuItem( pCmdUI->m_nIndex, MF_BYPOSITION | l_uiGray);
	}
	else
	{
		pCmdUI->Enable( l_bEnable );
	}
}

void SVObserverApp::OnUpdateAddPerspectiveTool(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&	
		SVSVIMStateClass::CheckState( SV_STATE_EDIT ) &&
		IsMonochromeImageAvailable() );
}

void SVObserverApp::OnUpdateExtrasSecuritySetup(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( !SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_REGRESSION | SV_STATE_TEST ) );
}


void SVObserverApp::OnEditRemoteOutputs()
{
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig )
	{
		pConfig->SetupRemoteOutput();
	}
}

// This handler handles a range of Ids based on inspection.
// It is called from when the IO Page is displayed.
// The IDR_SVOBSERVER_IODOCTYPE Menu Published Results.
void SVObserverApp::OnEditPublishedResults( UINT nID )
{
	UINT uiInspection = nID - ID_EDIT_PUBLISHEDRESULTS_BASE;
	// Get list of inspections
	// edit published results
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	if( nullptr != pConfig )
	{
		SVInspectionProcessPtrList l_Inspections;
		pConfig->GetInspections( l_Inspections );
		if( uiInspection < l_Inspections.size())
		{
			SVIPDoc* pDoc =  SVObjectManagerClass::Instance().GetIPDoc( l_Inspections[uiInspection]->GetUniqueObjectID() );
			if( NULL != pDoc )
			{
				pDoc->OnPublishedResultsPicker();
			}

		}
	}	
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnRCClose
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
//  :21.05.2001 EJC			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
void SVObserverApp::OnRCClose()
{
	// Returns Nonzero to SVRC Module via SVRCResult, if it is successfully.
	// Otherwise it returns 0.

	// Closes loaded configuration
	// Cleans up execution directory ( download directory )

	// Close config immediately, without hint or user message...
	DestroyConfig( FALSE, TRUE );	// Close config immediately
}
#pragma endregion AFX_MSG Methods

#pragma region AFX_VIRTUAL Methods
////////////////////////////////////////////////////////////////////////////////
// .Title       : // e.g. Compare member function of class ...
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
BOOL SVObserverApp::InitInstance()
{
	CWinApp::InitInstance();

	HINSTANCE ResourceInstance( NULL );
	//Load resource dll explicitly
	HRESULT retValue = SvUl::LoadDll::Instance().getDll( SvUl::SVOResourceDll, ResourceInstance );
	if (S_OK != retValue || nullptr == ResourceInstance)
	{
		//Because our exception handler (message box) needs the resources, we have to use here the standard message box.
		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_SVO_53_RESOURCE_DLL_LOADING_FAILED, SvOi::Tid_Empty, StdMessageParams, SvOi::Err_10009_LoadOfResourceDllFailed );
		MessageBox(nullptr, SvO::LoadingResourceDllFailed, nullptr, MB_OK | MB_ICONSTOP );
		exit(-SvOi::Err_10009_LoadOfResourceDllFailed);
	}

	// load File based write filter DLL. SVObserver will function normally (except for FBWF functionally, of course) if "fbwflib.dll" is not found
	SvUl::LoadDll::Instance().getDll( SvO::FbwfDllName, ExtrasEngine::ms_FbwfDllInstance);

	//Set the resource instance to the resource dll
	AfxSetResourceHandle( ResourceInstance );

	m_hEvent = CreateEvent(NULL, FALSE, TRUE, AfxGetAppName()); 

	if ( GetLastError()==ERROR_ALREADY_EXISTS )
	{ 
		return FALSE; 
	} 

	// OLE-Bibliotheken initialisieren
	if( !AfxOleInit() )
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_SVObserver_OleInitFailed, StdMessageParams, SvOi::Err_10125 );
		return FALSE;
	}

	if( !InitATL() )
	{
		return FALSE;
	}

	//Initializing  must be before first use of  MessageNotification::FireNotify which is i.e called from CheckDrive 
	SVVisionProcessorHelper::Instance().Startup();

	// Check for proper setup of V: for SVRemoteControl
	if( CheckDrive(_T("v:\\")) != S_OK)
	{
		return FALSE;
	}

	#ifdef _DEBUG
		long l_CrtVal = GetPrivateProfileInt( _T( "Memory" ), _T( "CrtSetBreakAlloc" ), 0, _T( "c:\\SVObserver\\Bin\\SVDebug.ini" ) );

		if( 0 < l_CrtVal )
		{
			::_CrtSetBreakAlloc( l_CrtVal );
		}
	#endif

	#ifdef __USE_TRACEWIN
		afxTraceEnabled = TRUE;
		PxlTraceInit();                                    // 13 May 1999 - frb.
	#endif //__USE_TRACEWIN
	TRACE0("..SVObserver - InitInstance()\n");         // 13 May 1999 - frb.

	// EB 2002 11 19
	// added to ensure C:\TEMP always exists;
	// bad things happen if it doesn't
	// example of symptom: crash when SVFocus tries to connect
	if (_taccess(_T("c:\\TEMP"), 0) != 0)
	{
		_tmkdir(_T("c:\\TEMP"));
	}

	// EB 2005 08 01
	// archive tool default storage
	// is now e:\temp
	if (_taccess(_T("D:\\TEMP"), 0) != 0)
	{
		_tmkdir(_T("D:\\TEMP"));
	}

	// Startup Matrox App
	SVMatroxApplicationInterface::Startup();

	SVHardwareManifest::Instance().Startup();
	SVTriggerProcessingClass::Instance().Startup();
	SVDigitizerProcessingClass::Instance().Startup();

	InitializeSecurity();

	int nRet = 0;

	// the SVIPDoc constructor will create
	//
	// Make sure the dialog color matches 'system' choice.
	//
	#ifdef _DEBUG
		SetPriorityClass( GetCurrentProcess(), NORMAL_PRIORITY_CLASS );
	#else
		SetPriorityClass( GetCurrentProcess(), HIGH_PRIORITY_CLASS );
	#endif

	SetThreadPriority( THREAD_PRIORITY_NORMAL );

	// The start window	
	SVStartWindowClass sWin;
#ifndef _DEBUG                    // 23 Mar 1999 - frb.
	if( sWin.Create( IDD_START ) )
	{
		sWin.ShowWindow( SW_SHOW );
	}
	else
		return FALSE;
#endif //_DEBUG                  // 23 Mar 1999 - frb.

	AfxEnableControlContainer();

	// Registry Key...
	SetRegistryKey( _T( "SVR Gesellschaft für Bildverarbeitung mbH" ) );

	LoadStdProfileSettings( 7 );  // Standard-INI-Dateioptionen einlesen (einschließlich MRU)

	m_SvimIni.SetFile(_T("C:\\SVObserver\\bin\\SVIM.INI"));

	ValidateMRUList();

	// Get SourceImageDepth
	m_lSouceImageDepth = GetProfileInt(_T( "Settings" ),_T( "Source Image Depth" ), -1 );
	if ( m_lSouceImageDepth == -1 )
	{
		WriteProfileInt(_T( "Settings" ),_T( "Source Image Depth" ), 15 );

		m_lSouceImageDepth = 15;
	}

	// Get SourceImageDepth
	m_lResultImageDepth = GetProfileInt(_T( "Settings" ),_T( "Result Image Depth" ), -1 );
	if ( m_lResultImageDepth == -1 )
	{
		WriteProfileInt(_T( "Settings" ),_T( "Result Image Depth" ), 5 );

		m_lResultImageDepth = 5;
	}

	// Get LogDataManager
	m_LogDataManager = UpdateAndGetLogDataManager();

	// Supplement for SVObserver 2.0
	// Set up path name environment variables...

	// Configuration Execution Path Name... ( "Run Directory" )
	m_ConfigExePNVariableValue = GetProfileString( _T( "Settings" ), m_ConfigExePNVariableName );
	if( m_ConfigExePNVariableValue.IsEmpty() )
	{
		m_ConfigExePNVariableValue = m_ConfigExePNVariableDefaultValue;
		// Update registry...
		WriteProfileString( _T( "Settings" ), m_ConfigExePNVariableName, m_ConfigExePNVariableValue );
	}
	// Check path, create if necessary but don't delete contents...
	InitPath( m_ConfigExePNVariableValue, TRUE, FALSE );
	SetEnvironmentVariable( m_ConfigExePNVariableName, m_ConfigExePNVariableValue );

	// Last Valid Configuration Path Name... ( "Last Valid Directory" )
	m_LastValidConfigPNVariableValue = GetProfileString( _T( "Settings" ), m_LastValidConfigPNVariableName );
	if( m_LastValidConfigPNVariableValue.IsEmpty() )
	{
		m_LastValidConfigPNVariableValue = m_LastValidConfigPNVariableDefaultValue;
		// Update registry...
		WriteProfileString( _T( "Settings" ), m_LastValidConfigPNVariableName, m_LastValidConfigPNVariableValue );
	}
	// Check path, create if necessary but don't delete contents...
	InitPath( m_LastValidConfigPNVariableValue, TRUE, FALSE );
	SetEnvironmentVariable( m_LastValidConfigPNVariableName, m_LastValidConfigPNVariableValue );

	// Load special profile settings
	if( GetProfileInt( _T( "Settings" ), _T( "Run Last Configuration Automatically" ), 0 ) )
		m_ShouldRunLastEnvironmentAutomatically = TRUE;
	else
		m_ShouldRunLastEnvironmentAutomatically = FALSE;

	// Get AutoRunDelay from Registry...
	m_AutoRunDelayTime = GetProfileInt( _T( "Settings" ), _T( "Auto Run Delay Time" ), m_AutoRunDelayTime );
	WriteProfileInt( _T( "Settings" ), _T( "Auto Run Delay Time" ), m_AutoRunDelayTime );

	// *** // ***
	SVObjectManagerClass::Instance().ConstructRootObject( RootObjectGuid );
	// *** // ***

	HRESULT l_hrLoad = INILoad();
	if ( l_hrLoad != S_OK )
	{
		CString l_csItem;

		if ( ( l_hrLoad & SV_HARDWARE_FAILURE_IO ) == SV_HARDWARE_FAILURE_IO )
		{
			l_csItem = GetDigitalBoardName();
		}
		else if ( ( l_hrLoad & SV_HARDWARE_FAILURE_TRIGGER ) == SV_HARDWARE_FAILURE_TRIGGER )
		{
			l_csItem = GetTriggerBoardName();
		}
		else if ( ( l_hrLoad & SV_HARDWARE_FAILURE_SOFTWARETRIGGER ) == SV_HARDWARE_FAILURE_SOFTWARETRIGGER )
		{
			l_csItem = GetSoftwareTriggerBoardName();
		}
		else if ( ( l_hrLoad & SV_HARDWARE_FAILURE_ACQUISITION ) == SV_HARDWARE_FAILURE_ACQUISITION )
		{
			l_csItem = GetAcquisitionBoardName();
		}
		else if ( ( l_hrLoad & SV_HARDWARE_FAILURE_FILEACQUISITION ) == SV_HARDWARE_FAILURE_FILEACQUISITION )
		{
			l_csItem = GetFileAcquisitionBoardName();
		}
		else
		{
			l_csItem = _T( "Unknown Item" );
		}

		SVStringArray msgList;
		msgList.push_back(SVString(m_csProcessor));
		msgList.push_back(SVString(m_csFrameGrabber));
		msgList.push_back(SVString(m_csIOBoard));
		msgList.push_back(SVString(m_csOptions));
		msgList.push_back(SVString(l_csItem));
		#ifndef _DEBUG                    // 23 Mar 1999 - frb.
		#ifndef _MINDEBUG
			sWin.ShowWindow( SW_HIDE );
		#endif
		#endif //_DEBUG                  // 23 Mar 1999 - frb.

		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_SVObserver_WrongModelNumber, msgList, StdMessageParams, SvOi::Err_10126 );
	}

	//check to see what licenses are available before setting up any documents
	TheSVOLicenseManager().InitLicenseManager();
	
	m_mgrRemoteFonts.Startup();

#ifdef LUT_DEBUG
	// temp debugging code EB 2002 12 20
	SVLutTestCases test;
#endif

	// Dokumentvorlagen der Anwendung registrieren. Dokumentvorlagen
	//  dienen als Verbindung zwischen Dokumenten, Rahmenfenstern und Ansichten.
	SVMultiDocTemplateClass* pDocTemplate;
	pDocTemplate = new SVMultiDocTemplateClass( IDR_SVOBSERVER_IPDOCTYPE,
		RUNTIME_CLASS( SVIPDoc ),			 // Doc
		RUNTIME_CLASS( SVIPSplitterFrame ),  // Frame
		RUNTIME_CLASS( SVImageViewScroll ) );// View

	AddDocTemplate( pDocTemplate );

	// IODoc
	SVMultiDocTemplateClass* pDocTemplate1;
	pDocTemplate1 = new SVMultiDocTemplateClass( IDR_SVOBSERVER_IODOCTYPE,
		RUNTIME_CLASS( SVIODoc ),
		RUNTIME_CLASS( SVIOTabbedView ), // The SVIOTabbedView allows tabbed views using TVisualFramework. TRB
		RUNTIME_CLASS( SVDiscreteInputsView ) );

	AddDocTemplate( pDocTemplate1 );

	CMultiDocTemplate *pDocTemp2 = new CMultiDocTemplate( IDR_IPDOC_MENU,
		RUNTIME_CLASS( SVOIPDocClass ),
		RUNTIME_CLASS( SVOIPFrameWndClass ),
		RUNTIME_CLASS( SVOIPViewClass ) );

	AddDocTemplate( pDocTemp2 );

	CMultiDocTemplate *pDocTemp3 = new CMultiDocTemplate( IDR_IODOC_MENU,
		RUNTIME_CLASS( SVOIODocClass ),
		RUNTIME_CLASS( SVOIOFrameWndClass ),
		RUNTIME_CLASS( SVOIOViewClass ) );
	AddDocTemplate( pDocTemp3 );

	// Haupt-MDI-Rahmenfenster erzeugen
	SVMainFrame* pMainFrame = new SVMainFrame;
	if( ! pMainFrame ) { return FALSE; }

	if ( !pMainFrame->LoadFrame( IDR_MAINFRAME ) ) 
	{ return FALSE; }
	m_pMainWnd = pMainFrame;

	// Load Utilities Menu
	SVUtilitiesClass util;
	CWnd *pWindow;
	CMenu *pMenu;
	CString szMenuText;

	pWindow = AfxGetMainWnd();
	pMenu = pWindow->GetMenu();
	szMenuText = _T("&Utilities");

	if ( pMenu = util.FindSubMenuByName( pMenu, szMenuText ) )
	{
		util.LoadMenu( pMenu );
	}

	// Create IO Control Class
	IOControl.Create( m_pMainWnd->m_hWnd );

	// DDE-Execute-Open aktivieren
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Close Start Window
	sWin.DestroyWindow();

	//Get Amount of System Memory
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof (statex);
	GlobalMemoryStatusEx (&statex);
	DWORDLONG AmountOfRam = (statex.ullTotalPhys/1024)/1024;

	int iGoOfflineBufferSize = 0;
	int iAsyncBufferSize = 0;
	// allocate pools in the memory manager

	//Log amount of physical memory - may help in debugging issues in the future.
	SVStringArray MessageList;
	MessageList.push_back(SvUl_SF::Format(_T("%d"), AmountOfRam));
	SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
	Exception.setMessage(SVMSG_SVO_54_EMPTY, SvOi::Tid_AmountOfSystemMemoryText, MessageList,StdMessageParams, SvOi::Memory_Log_45001);

	//if amount of physical memory is around 16 GigE allocate the larger memory pools.
	if (AmountOfRam >= UseLargerArchiveMemoryPool)
	{
		iGoOfflineBufferSize = INI().GetValueInt( _T("Settings"), _T("ArchiveToolGoOfflineBufferSize"), GoOfflineDefault16GB );
		iAsyncBufferSize = INI().GetValueInt( _T("Settings"), _T("ArchiveToolAsyncBufferSize"), AsyncDefault16GB);
		TheSVMemoryManager().CreatePool(SvO::ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME, iGoOfflineBufferSize * 1024);
		TheSVMemoryManager().CreatePool(SvO::ARCHIVE_TOOL_MEMORY_POOL_ONLINE_ASYNC_NAME, iAsyncBufferSize * 1024);
	}
	else
	{
		iGoOfflineBufferSize = INI().GetValueInt( _T("Settings"), _T("ArchiveToolGoOfflineBufferSize"), GoOfflineDefault4GB );
		iAsyncBufferSize = INI().GetValueInt( _T("Settings"), _T("ArchiveToolAsyncBufferSize"), AsyncDefault4GB );
		TheSVMemoryManager().CreatePool(SvO::ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME, iGoOfflineBufferSize * 1024);
		TheSVMemoryManager().CreatePool(SvO::ARCHIVE_TOOL_MEMORY_POOL_ONLINE_ASYNC_NAME, iAsyncBufferSize * 1024);
	}
	// Das Hauptfenster ist initialisiert und kann jetzt angezeigt und aktualisiert werden.
#ifdef _DEBUG
	//pMainFrame->ShowWindow(SW_SHOWNORMAL);           // 29 Mar 1999 - frb.
	pMainFrame->ShowWindow( SW_SHOWMAXIMIZED );  //m_nCmdShow
#else
	pMainFrame->ShowWindow( SW_SHOWMAXIMIZED );  //m_nCmdShow
#endif
	pMainFrame->UpdateWindow();

	//
	// Init user list...

	Logout();

	if( IsProductTypeRAID() )
	{
		m_IntelRAID.UpdateStatus();
	}
	else
	{
		SVSVIMStateClass::RemoveState( SV_STATE_RAID_FAILURE );
	}

	// Load last loaded configuration, if necessary...
	if( m_ShouldRunLastEnvironmentAutomatically )
	{
		// Delay Auto Run to allow other applications to start...
		Sleep( m_AutoRunDelayTime );
		// Load Last Configuration...
		::PostMessage( m_pMainWnd->m_hWnd, WM_COMMAND, SV_AUTO_RUN_LAST_MRU, 0 );
		// Start Configuration...
		//::PostMessage( m_pMainWnd->m_hWnd, SV_AUTO_RUN_LAST_CONFIGURATION, 0, 0 ); 03 Dec 1999 - frb.
	}

	// add message to event viewer - SVObserver Started
	SVStringArray msgList;
	if( (m_CurrentVersion & 0xff) == 0xff )
	{
		msgList.push_back(SeidenaderVision::SVVersionInfo::GetShortTitleVersion());
	}
	else
	{
		msgList.push_back(SeidenaderVision::SVVersionInfo::GetTitleVersion());
	}
	Exception.setMessage( SVMSG_SVO_25_SVOBSERVER_STARTED, SvOi::Tid_Version, msgList, StdMessageParams );

	SVDirectX::Instance().Initialize();

	SVSocketLibrary::Init();

	int AutoSaveValue = INI().GetValueInt( _T("Settings"), _T("EnableAutosave"), 0); //Arvid accept a number: non-zero enables

	std::string AutoSaveValueString=INI().GetValueString( _T("Settings"), _T("EnableAutosave"), _T("FALSE")); //Arvid accept a string

	if ( AutoSaveValueString == std::string("TRUE") || AutoSaveValueString == std::string("true"))
	{
		AutoSaveValue = 1;
	}

	ExtrasEngine::Instance().SetEnabled(AutoSaveValue != 0);
	unsigned short defaultPortNo = -1;
	m_RemoteCommandsPortNumber = INI().GetValueInt( _T("Settings"), _T("RemoteCommandsPortNumber"), defaultPortNo );
	if ( m_RemoteCommandsPortNumber == defaultPortNo )
	{
		m_RemoteCommandsPortNumber = RemoteCommandsPortNumber;

		INI().SetValue(_T( "Settings" ),_T( "RemoteCommandsPortNumber" ), m_RemoteCommandsPortNumber );
	}

	m_InputStreamPortNumber = INI().GetValueInt( _T("Settings"), _T("InputStreamPortNumber"), defaultPortNo );
	if ( m_InputStreamPortNumber == defaultPortNo )
	{
		m_InputStreamPortNumber = InputStreamPortNumber;

		INI().SetValue(_T( "Settings" ),_T( "InputStreamPortNumber" ), m_InputStreamPortNumber );
	}

	m_OutputStreamPortNumber = INI().GetValueInt( _T("Settings"), _T("OutputStreamPortNumber"), defaultPortNo );
	if ( m_OutputStreamPortNumber == defaultPortNo)
	{
		m_OutputStreamPortNumber = OutputStreamPortNumber;

		INI().SetValue(_T( "Settings" ),_T( "OutputStreamPortNumber" ), m_OutputStreamPortNumber );
	}

	m_FailStatusStreamPortNumber = INI().GetValueInt( _T("Settings"), _T("FailStatusStreamPortNumber"), defaultPortNo );
	if ( m_FailStatusStreamPortNumber == defaultPortNo )
	{
		m_FailStatusStreamPortNumber = FailStatusStreamPortNumber;

		INI().SetValue(_T( "Settings" ),_T( "FailStatusStreamPortNumber" ), m_FailStatusStreamPortNumber );
	}

	m_DataValidDelay = static_cast<long> ( INI().GetValueInt( _T("Settings"), _T("DataValidDelay"), 0 ) );

	SVSocketRemoteCommandManager::Instance().Startup( m_RemoteCommandsPortNumber );
	SVInputStreamManager::Instance().Startup( m_InputStreamPortNumber );
	SVOutputStreamManager::Instance().Startup( m_OutputStreamPortNumber );
	SVFailStatusStreamManager::Instance().Startup( m_FailStatusStreamPortNumber );

	if ( !TheSVOLicenseManager().HasMatroxLicense() )
	{
		SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
		Exception.setMessage( SVMSG_SVO_52_NOMATROX_DONGLE, SvOi::Tid_Empty, StdMessageParams, SvOi::Err_25013_NoMatroxDongle );
	}

	if ( TheSVOLicenseManager().HasMatroxLicense() && !TheSVOLicenseManager().HasMatroxGigELicense() && IsMatroxGige() )
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_SVObserver_MatroxGigELicenseNotFound, StdMessageParams, SvOi::Err_10127 );
	}

	

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnIdle(LONG lCount)
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
BOOL SVObserverApp::OnIdle(LONG lCount) 
{
	BOOL bMore = CWinApp::OnIdle(lCount);

	return bMore;
}

CDocument* SVObserverApp::OpenDocumentFile( LPCTSTR FileName ) 
{
	return nullptr;
}

void SVObserverApp::Serialize(CArchive& ar) 
{
	CWinApp::Serialize( ar );
}

int SVObserverApp::ExitInstance() 
{
	SVDirectX::Instance().clear();

	SVVisionProcessorHelper::Instance().Shutdown();
	SVFailStatusStreamManager::Instance().Shutdown();
	SVOutputStreamManager::Instance().Shutdown();
	SVInputStreamManager::Instance().Shutdown();
	SVSocketRemoteCommandManager::Instance().Shutdown();

	SVSocketLibrary::Destroy();
	SVSharedMemorySingleton::Destroy();
	ValidateMRUList();

	// Destroy still open message windows
	DestroyMessageWindow();

	#if !defined( _DEBUG ) && !defined( _MINDEBUG )
		// Display close window	
		m_pMessageWindow = new SVMessageWindowClass;
		if( m_pMessageWindow && m_pMessageWindow->Create( IDD_MESSAGE_DIALOG ) )
		{
			m_pMessageWindow->ShowWindow( SW_SHOW );
		}
	#else
		m_pMessageWindow = NULL;
	#endif

	// *** // ***
	SVObjectManagerClass::Instance().DestroyRootObject();
	// *** // ***

	INIClose();

	m_mgrRemoteFonts.Shutdown();

	SVTriggerProcessingClass::Instance().Shutdown();
	SVDigitizerProcessingClass::Instance().Shutdown();
	SVHardwareManifest::Instance().Shutdown();

	SVObjectManagerClass::Instance().Shutdown();
	SVClassRegisterListClass::Instance().Shutdown();

	SVIOConfigurationInterfaceClass::Instance().Shutdown();

	// Shutdown MIL
	SVMatroxApplicationInterface::Shutdown();

	//add message to event viewer - SVObserver stopped
	SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
	Exception.setMessage( SVMSG_SVO_26_SVOBSERVER_STOPPED, SvOi::Tid_Empty, StdMessageParams );
	CloseHandle( m_hEvent );

	#if !defined(_WIN32_WCE) || defined(_CE_DCOM)
		if( m_ATLInited )
		{
			_Module.RevokeClassObjects();
		}
	#endif

	return CWinApp::ExitInstance();
}

int SVObserverApp::Run()
{
	int Result( 0 );

	try
	{
		Result = CWinApp::Run();
	}
	catch( const SvStl::MessageContainer& rExp )
	{
		//This is the topmost catch for MessageContainer exceptions
		SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
		//Set the error code to unhandled exception but use the rest of the data from the original exception
		SvStl::MessageData Msg( rExp.getMessage() );
		SVString OrgMessageCode = SvUl_SF::Format( _T("0x%08X"), Msg.m_MessageCode );
		Msg.m_AdditionalTextId = SvOi::Tid_Default;
		SVStringArray msgList;
		msgList.push_back(OrgMessageCode);
		Msg.m_AdditionalTextList = msgList;
		Msg.m_MessageCode = SVMSG_SVO_72_UNHANDLED_EXCEPTION;
		Result = Msg.m_MessageCode;
		Exception.setMessage( Msg );
	}
	return Result;
}
#pragma endregion AFX_Virtual methods

#pragma region virtual
HRESULT SVObserverApp::OpenSVXFile(LPCTSTR PathName)
{
	CWaitCursor wait;

	HRESULT hr;
	HRESULT	hrDestroyed;
	SVClock::SVTimeStamp l_StartLoading;
	SVClock::SVTimeStamp l_FinishLoad;

	BOOL bOk;

	hr = S_OK;
	hrDestroyed = S_OK;
	bOk = 0;

	while (1)
	{
		hrDestroyed = DestroyConfig();
		if (hrDestroyed != S_OK)
		{
			hr = hrDestroyed; //keep the cancel state so it does not remove from MRU
			break;
		}

		TheSVOLicenseManager().ClearLicenseErrors();

		bOk = SVSVIMStateClass::AddState( SV_STATE_UNAVAILABLE | SV_STATE_LOADING );

		if (!bOk)
		{
			break;
		}

		bOk = SVSVIMStateClass::RemoveState( SV_STATE_AVAILABLE );

		if (!bOk)
		{
			break;
		}

		SVFileNameManagerClass svFileManager;
		SVFileNameClass svFileName( PathName );

		//
		// Check if we tried to load the SVC from 
		// Execution path...("C:\RUN\")
		//
		if ( CString( svFileName.GetPathName() ).CompareNoCase( svFileManager.GetRunPathName() ) )
		{
			// Clean up Execution Directory...
			// Check path, create if necessary and delete contents...
			InitPath( CString( svFileManager.GetRunPathName() ) + "\\", TRUE, TRUE );
		} // if ( CString

		try
		{
			CString csFullFileName;

			BSTR bStr = NULL;
			unsigned long configVer = 0;

			setConfigFullFileName( PathName );

			SVRCSetSVCPathName( getConfigFullFileName() );

			csFullFileName = getConfigFullFileName();

			bStr = csFullFileName.AllocSysString();

			while (1)
			{
				SVTreeType XMLTree;

				hr = SVOCMLoadConfiguration( m_CurrentVersion, configVer, bStr, XMLTree );
				if (hr & SV_ERROR_CONDITION)
				{
					break;
				}
				CString itemType;
				int errorCode(0);
				hr = SVOCMCheckObsoleteItems( XMLTree, configVer, itemType, errorCode );
				if (hr & SV_ERROR_CONDITION)
				{
					if( SVSVIMStateClass::CheckState( SV_STATE_REMOTE_CMD ) )
					{
						SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
						Exception.setMessage( SVMSG_SVO_76_CONFIGURATION_HAS_OBSOLETE_ITEMS, itemType, StdMessageParams, errorCode );
					}
					else
					{
						SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
						Exception.setMessage( SVMSG_SVO_76_CONFIGURATION_HAS_OBSOLETE_ITEMS, itemType, StdMessageParams, errorCode, 0, nullptr, MB_OK );
					}
					break;
				}

				if ( configVer > m_CurrentVersion )
				{
					CString strFile, strApp;

					::SVGetVersionString( strApp, m_CurrentVersion );
					::SVGetVersionString( strFile, configVer );
					SVStringArray msgList;
					msgList.push_back(SVString(strFile));
					msgList.push_back(SVString(strApp));

					SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
					INT_PTR result = Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_SVObserver_WrongVersionNumber, msgList, StdMessageParams, SvOi::Err_10128, NULL, nullptr, MB_YESNO );
					if( IDNO == result )
					{
						hr = E_FAIL;
						break;
					}
				}

				l_StartLoading = SVClock::GetTimeStamp();

				SVConfigurationObject* pConfig( nullptr );
				SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

				//If no configuration then it needs to be created
				if( nullptr == pConfig )
				{
					RootObject *pRoot = nullptr;

					SVObjectManagerClass::Instance().GetRootChildObject( pRoot, SvOl::FqnRoot );
					if(nullptr != pRoot)
					{
						pRoot->createConfigurationObject();
						SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
					}
				}

				if( nullptr != pConfig )
				{
					try
					{
						hr = pConfig->LoadConfiguration( XMLTree );
					}
					catch ( const SvStl::MessageContainer& rSvE )
					{
						hr = E_FAIL;
						//SVMSG_SVO_IGNORE_EXCEPTION can be used to stop the loading but not display any messages
						if( SVMSG_SVO_IGNORE_EXCEPTION != rSvE.getMessage().m_MessageCode )
						{
							SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
							Exception.setMessage( rSvE.getMessage() );
						}
					}
				}

				if (hr & SV_ERROR_CONDITION)
				{
					break;
				}

				GetMainFrame()->ParseToolsetScripts( XMLTree );
				wait.Restore();

				if (nullptr != pConfig)
				{
					hr = pConfig->LoadRemoteMonitorList(XMLTree);
					if (hr & SV_ERROR_CONDITION)
					{
						break;
					}
				}

				if (nullptr != pConfig)
				{
					hr = pConfig->LoadGlobalConstants(XMLTree);
					if (hr & SV_ERROR_CONDITION)
					{
						break;
					}
				}

				if( nullptr != pConfig )
				{
					pConfig->RebuildInputOutputLists();

					// Removes any invalid entries in the output list.
					if( pConfig->IsConfigurationLoaded() )
					{
						if( SVMSG_SVO_70_DUPLICATE_DISCRETE_OUTPUT == pConfig->ValidateOutputList() )
						{
							SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
							Exception.setMessage( SVMSG_SVO_70_DUPLICATE_DISCRETE_OUTPUT, SvOi::Tid_Empty, StdMessageParams );
						}
					}
				}

				ConstructDocuments( XMLTree );

				GetMainFrame()->OnConfigurationFinishedInitializing();

				l_FinishLoad = SVClock::GetTimeStamp();
				long l_lTime = static_cast<long>(l_FinishLoad - l_StartLoading);

				SVStringArray msgList;
				msgList.push_back(PathName);
				msgList.push_back(SvUl_SF::Format(_T("%d"), l_lTime));
				
				SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_SVO_29_SVOBSERVER_CONFIG_LOADED, SvOi::Tid_ConfigLoadTime, msgList, StdMessageParams );

				break;
			} // while (1)
			
			SysFreeString (bStr);
			bStr = nullptr;

			if (hr & SV_ERROR_CONDITION)
			{
				// If there was an error during configuration loading...
				SVSVIMStateClass::AddState( SV_STATE_AVAILABLE );
				SVSVIMStateClass::RemoveState(SV_STATE_UNAVAILABLE | SV_STATE_LOADING);

				//Use E_FAIL to stop the loading but do not show any error messages
				if( E_FAIL != hr )
				{
					SVStringArray msgList;
					msgList.push_back(SvUl_SF::Format( _T("%d"), hr));
					SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_SVObserver_ConfigurationLoadFailed, msgList, StdMessageParams, SvOi::Err_10129 );
				}
				return S_FALSE;
			} // if (hr & 0xc000)

			// ************************************* JMS

			// IsSECModified = FALSE; 03 Dec 1999 - frb. delay until 
			// .ipd parsing is completed.

			if ( CString( svFileManager.GetConfigurationPathName() ).IsEmpty() )
			{
				AddToRecentFileList( getConfigFullFileName() );
			}
			else
			{
				AddToRecentFileList( CString( svFileManager.GetConfigurationPathName() ) + 
					"\\" + getConfigFileName() );
			}

			UpdatePPQBar();
		} // try

		catch( CUserException* pUE )
		{
			delete pUE;

			bOk = FALSE;

			SVSVIMStateClass::RemoveState( SV_STATE_LOADING );

			DestroyConfig( FALSE, TRUE );

			setConfigFullFileName( ( LPCTSTR )NULL );

			UpdatePPQBar();

			SVSVIMStateClass::AddState( SV_STATE_AVAILABLE );
			SVSVIMStateClass::RemoveState( SV_STATE_UNAVAILABLE );
		}  // catch

		break;
	} // while (1)

	if ( ! bOk && hrDestroyed == S_OK )
	{
		setConfigFullFileName( ( LPCTSTR )NULL );

		SVSVIMStateClass::AddState( SV_STATE_AVAILABLE );
		SVSVIMStateClass::RemoveState( SV_STATE_UNAVAILABLE | SV_STATE_LOADING );
	}

	if (hr == S_OK)
	{
		hr = bOk ? S_OK : S_FALSE;
	}

	// Logic to remove unused IO Tabs Remote inputs.
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig )
	{

		if( pConfig->GetRemoteOutputGroupCount() == 0)
		{
			HideIOTab( SVRemoteOutputsViewID );
		}

		const RemoteMonitorList& rList = pConfig->GetRemoteMonitorList();
		if (!rList.size())
		{
			HideRemoteMonitorListTab();
		}
	}
	else
	{
		HideIOTab( SVRemoteOutputsViewID );
	}

	// Update Remote Inputs Tab
	UpdateRemoteInputTabs();
	return hr;
}

//******************************************************************************
// Message Operation(s):
//******************************************************************************
void SVObserverApp::LoadTempIODoc(LPCTSTR FileName)
{
	CDocTemplate* pDocTemplate = NULL;
	POSITION pos = GetFirstDocTemplatePosition();
	if( pos )
	{
		pDocTemplate = GetNextDocTemplate( pos );
		if( pDocTemplate )
		{
			pDocTemplate = GetNextDocTemplate( pos );
			if( pDocTemplate )
			{
				pDocTemplate = GetNextDocTemplate( pos );
				if( pDocTemplate )
				{
					pDocTemplate = GetNextDocTemplate( pos );
					if( pDocTemplate )
					{
						SVOIODocClass *pDoc = (SVOIODocClass *)pDocTemplate->OpenDocumentFile( FileName, FALSE );
						pDoc->CloseDocument();
					}
				}
			}
		}
	}
}

void SVObserverApp::LoadTempIPDoc(LPCTSTR FileName)
{
	CDocTemplate* pDocTemplate = nullptr;
	POSITION pos = GetFirstDocTemplatePosition();
	if( pos )
	{
		pDocTemplate = GetNextDocTemplate( pos );
		if( pDocTemplate )
		{
			pDocTemplate = GetNextDocTemplate( pos );
			if( pDocTemplate )
			{
				pDocTemplate = GetNextDocTemplate( pos );
				if( pDocTemplate )
				{
					SVOIPDocClass *pDoc = (SVOIPDocClass *)pDocTemplate->OpenDocumentFile( FileName, FALSE );
					pDoc->CloseDocument();
				}
			}
		}
	}
}

SVIODoc *SVObserverApp::NewSVIODoc( LPCTSTR DocName, SVIOController& IOController )
{
	SVIODoc* pDoc = nullptr;
	CDocTemplate* pDocTemplate = nullptr;
	POSITION pos = GetFirstDocTemplatePosition();
	if( pos )
	{
		pDocTemplate = GetNextDocTemplate( pos );
		if( pDocTemplate )
		{
			pDocTemplate = GetNextDocTemplate( pos );
			if( pDocTemplate )
			{
				// Create a new empty visible document
				pDoc = dynamic_cast< SVIODoc* >( pDocTemplate->OpenDocumentFile( nullptr, TRUE ) );   // Make visible

				if( pDoc != nullptr )
				{
					pDoc->m_pIOController = &IOController;

					pDoc->SetTitle( DocName );

					SVObjectManagerClass::Instance().RegisterIODoc( IOController.GetUniqueObjectID(), pDoc );
				}
			}
		}
	}

	return pDoc;
}

SVIPDoc* SVObserverApp::NewSVIPDoc( LPCTSTR DocName, SVInspectionProcess& Inspection )
{
	SVIPDoc* pDoc = nullptr;
	CDocTemplate* pDocTemplate = nullptr;
	POSITION pos = GetFirstDocTemplatePosition();
	if( pos )
	{
		pDocTemplate = GetNextDocTemplate( pos );
		if( pDocTemplate )
		{
			pDoc = dynamic_cast<SVIPDoc* >( pDocTemplate->OpenDocumentFile( nullptr, TRUE ) );   // Make visible

			if( pDoc != nullptr )
			{
				pDoc->SetInspectionID( Inspection.GetUniqueObjectID() );

				pDoc->SetTitle( DocName );
			}
		}
	}

	return pDoc;
}
#pragma endregion virtual

CString SVObserverApp::GetConfigurationName() const
{
	CString l_ConfigName;

	SVFileNameClass svFileName;

	svFileName.SetFullFileName( getConfigFullFileName() );

	if( 0 < strlen( svFileName.GetFileNameOnly() ) )
	{
		svFileName.SetPathName( _T( "C:\\RUN" ) );

		l_ConfigName = svFileName.GetFullFileName();
	}

	return l_ConfigName;
}

SVString SVObserverApp::getModelNumber() const
{
	SVString ModelNumber;

	RootObject::getRootChildValue( ::EnvironmentModelNumber, ModelNumber );

	return ModelNumber;
}

SVString SVObserverApp::getSerialNumber() const
{
	SVString SerialNumber;

	RootObject::getRootChildValue( ::EnvironmentSerialNumber, SerialNumber );

	return SerialNumber;
}

SVString SVObserverApp::getWinKey() const
{
	SVString WinKey;

	RootObject::getRootChildValue( ::EnvironmentWinKey, WinKey );

	return WinKey;
}

HRESULT SVObserverApp::LoadPackedConfiguration( const CString& p_rPackedFileName )
{
	HRESULT l_Status = S_OK;

	SVPackedFile PackedFile;

	if( _access( p_rPackedFileName, 0 ) == 0 )
	{
		SendMessage( m_pMainWnd->m_hWnd, WM_COMMAND, MAKEWPARAM (ID_RC_CLOSE_AND_CLEAN_RUN_DIR, 0), 0 );
	}
	else
	{
		l_Status = E_UNEXPECTED;
	}

	if( l_Status == S_OK )
	{
		if( !( PackedFile.UnPackFiles( p_rPackedFileName, _T("C:\\RUN") ) ) )
		{
			l_Status = E_UNEXPECTED;
		}
	}

	if( l_Status == S_OK )
	{
		CString configFilePath = PackedFile.getConfigFilePath();

		if( configFilePath.IsEmpty() || ( _access( configFilePath, 0 ) != 0 ) )
		{
			l_Status = E_UNEXPECTED;
		}
		else
		{
			SVRCSetSVCPathName(configFilePath);

			l_Status = static_cast<HRESULT>(SendMessage( m_pMainWnd->m_hWnd, SV_LOAD_CONFIGURATION, 0, 0 ));
		}
	}

	return l_Status;
}

HRESULT SVObserverApp::SavePackedConfiguration( const CString& p_rPackedFileName )
{
	HRESULT l_Status = S_OK;

	SendMessage( m_pMainWnd->m_hWnd, WM_COMMAND, MAKEWPARAM (ID_RC_SAVE_ALL_AND_GET_CONFIG, 0), 0 );

	SVPackedFile PackedFile;

	if( !( PackedFile.PackFiles( GetConfigurationName(), p_rPackedFileName ) ) )
	{
		l_Status = E_UNEXPECTED;
	}

	return l_Status;
}

SVMainFrame* SVObserverApp::GetMainFrame() const
{
	return dynamic_cast< SVMainFrame* >( m_pMainWnd );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : .
// -----------------------------------------------------------------------------
// .Description : ...
//              : CanClose Operator
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
HRESULT SVObserverApp::CanCloseMainFrame()
{
	return DestroyConfig();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : DestroyConfig
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
HRESULT SVObserverApp::DestroyConfig( BOOL AskForSavingOrClosing /* = TRUE */, 
	BOOL CloseWithoutHint /* = FALSE */ )
{
	BOOL bCancel = FALSE;

	BOOL bOk = ! SVSVIMStateClass::CheckState( SV_STATE_READY | SV_STATE_RUNNING | SV_STATE_TEST );
	HRESULT hr= S_OK;

	if ( ! bOk )
	{
		CString message;
		BOOL bClose = TRUE;

		if ( CloseWithoutHint )
		{
			SVSVIMStateClass::AddState( SV_STATE_CANCELING );
		}
		else
		{
			SVStringArray msgList;
			msgList.push_back(getConfigFileName()); 
			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
			INT_PTR result = Msg.setMessage( SVMSG_SVO_94_GENERAL_Informational, SvOi::Tid_UserQuestionCloseConfig, msgList, StdMessageParams, SvOi::Err_10130, NULL, nullptr, MB_YESNO | MB_ICONQUESTION );
			bClose = IDYES == result;
			if (bClose == FALSE)
				hr = ERROR_CANCELLED;
		}

		if ( bClose )
		{
			if ( SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) )
			{
				OnStop();
			}

			if ( AskForSavingOrClosing )
			{
				// Check if current config is modified and ask for saving
				if ( SVSVIMStateClass::CheckState( SV_STATE_MODIFIED ) )
				{
					SVStringArray msgList;
					msgList.push_back( getConfigFileName() ); 
					SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
					INT_PTR result = Msg.setMessage( SVMSG_SVO_94_GENERAL_Informational, SvOi::Tid_UserQuestionSaveChanges, msgList, StdMessageParams, SvOi::Err_10131, NULL, nullptr, MB_YESNOCANCEL | MB_ICONQUESTION );
					switch( result )
					{
					case IDNO:
						{
							SVSVIMStateClass::AddState( SV_STATE_CANCELING );
							ResetAllIPDocModifyFlag(FALSE);
							break;
						}
					case IDYES:
						{
							if( m_svSecurityMgr.SVValidate( SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION ) == S_OK )
							{
								OnFileSaveConfig();
								ResetAllIPDocModifyFlag(FALSE);

								// Check whether config saving is done.
								// If not, an error or an user cancel
								// command occured!
								bClose = ! CString( getConfigFullFileName() ).IsEmpty() &&
									! SVSVIMStateClass::CheckState( SV_STATE_MODIFIED );
							}
							break;
						}
					case IDCANCEL:  // Fall through to default case.
					default:	// Don´t close config!
						{
							bCancel = TRUE;
							bClose = FALSE;
							break;
						}
					}// end switch( result )
				}// end if ( SVSVIMStateClass::CheckState( SV_STATE_MODIFIED ) )
			}
			else
			{
				SVSVIMStateClass::AddState( SV_STATE_CANCELING );
			}
		}// end if ( bClose )

		if ( bClose )
		{
			bOk = bClose = SVSVIMStateClass::CheckState( SV_STATE_READY );
		}

		if ( bClose )
		{
			TheSVOLicenseManager().ClearLicenseErrors();

			// Disallow Client Connections
			SVInputStreamManager::Instance().Shutdown();
			SVOutputStreamManager::Instance().Shutdown();

			// Remove all FailStatus Streams
			SVFailStatusStreamManager::Instance().Clear();

			bOk = SVSVIMStateClass::AddState( SV_STATE_UNAVAILABLE | SV_STATE_CLOSING );

			if ( bOk )
			{
				bOk = SVSVIMStateClass::RemoveState( SV_STATE_READY | SV_STATE_MODIFIED | SV_STATE_EDIT );
			}

			if ( bOk )
			{
				CWaitCursor wait;

				wait.Restore();

				// First remove all ActiveX server things associated with this config
				CSVCommand::ResetStreamingDataAndLockedImages();

				wait.Restore();


				SVConfigurationObject* pConfig( nullptr );
				SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

				if( nullptr != pConfig )
				{
					pConfig->ClearRemoteMonitorList();
					bOk = pConfig->DestroyConfiguration();

					RootObject *pRoot = nullptr;
					SVObjectManagerClass::Instance().GetRootChildObject( pRoot, SvOl::FqnRoot );
					if(nullptr != pRoot)
					{
						pRoot->destroyConfigurationObject();
					}
				}

				wait.Restore();

				(( SVMainFrame* )m_pMainWnd)->SetStatusInfoText(_T(""));

				wait.Restore();

				// Destroy the current PPQ
				( ( SVMainFrame* ) m_pMainWnd )->DestroyPPQButtons();

				wait.Restore();

				UpdatePPQBar();

				wait.Restore();

				bOk = SVSVIMStateClass::AddState( SV_STATE_AVAILABLE ) && bOk;
				bOk = SVSVIMStateClass::RemoveState( SV_STATE_UNAVAILABLE | SV_STATE_CLOSING | SV_STATE_CANCELING ) && bOk;
				bOk = setConfigFullFileName( ( LPCTSTR )NULL ) && bOk;

				wait.Restore();

				INIReset();
			}// end if ( bOk )
		}// end if ( bClose )

	}// end if bOk = ! SVSVIMStateClass::CheckState( SV_STATE_READY | SV_STATE_RUNNING );
	else
	{
		// First remove all ActiveX server things associated with this config
		CSVCommand::ResetStreamingDataAndLockedImages();

		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		if( nullptr != pConfig )
		{
			pConfig->ClearRemoteMonitorList();
			bOk = pConfig->DestroyConfiguration();

			RootObject *pRoot = nullptr;
			SVObjectManagerClass::Instance().GetRootChildObject( pRoot, SvOl::FqnRoot );
			if(nullptr != pRoot)
			{
				pRoot->destroyConfigurationObject();
			}
		}

		(( SVMainFrame* )m_pMainWnd)->SetStatusInfoText(_T(""));
	}

	if (hr == S_OK)
	{
		hr = bOk ? S_OK : S_FALSE;
	}

	if ( bCancel )
	{
		hr = ERROR_CANCELLED;
	}
	else
	{
		CStringArray saCameras;

		DisconnectCameras( saCameras );
		ConnectCameras( saCameras );
	}

	return hr;
}

void SVObserverApp::RemoveUnusedFiles()
{
	SVFileNameManagerClass svFileManager;
	svFileManager.RemoveUnusedFiles( FALSE );
}

SVIODoc* SVObserverApp::GetIODoc() const
{
	SVIODoc* l_pIODoc( NULL );

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig && !( pConfig->GetIOControllerID().empty() ) )
	{
		l_pIODoc = SVObjectManagerClass::Instance().GetIODoc( pConfig->GetIOControllerID() );
	}

	return l_pIODoc;
}

BOOL SVObserverApp::Login()
{
	ASSERT(FALSE);

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Logout
// -----------------------------------------------------------------------------
// .Description : Log the current user out...
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
// .Output(s)
//	:
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
//  :15.10.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVObserverApp::Logout( BOOL BForceLogout )
{
	SVSVIMStateClass::RemoveState( SV_STATE_SECURED );

	//
	// We need to deselect any tool that might be set for operator move.
	//
	SVMainFrame* pWndMain = ( SVMainFrame* )GetMainWnd();
	if(pWndMain)
	{
		DeselectTool();

		// Logged on User changed
		pWndMain->PostMessage( SV_LOGGED_ON_USER_CHANGED, 0 );
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : InitPath
// -----------------------------------------------------------------------------
// .Description : 
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
//  :13.10.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVObserverApp::InitPath( LPCTSTR TStrPathName, BOOL BCreateIfNotExists /*= TRUE*/, BOOL BDeleteContents /*= TRUE*/ )
{
	if( TStrPathName )
	{
		if( SVCheckPathDir( TStrPathName, BCreateIfNotExists ) )
		{
			if( BDeleteContents )
			{
				// Get Delete Directory...
				CString strDelName;
				SVGetPathInformation( strDelName, TStrPathName, SVDRIVE | SVDIR );

				// Be sure that this is not the boot, system or windows directory...
				TCHAR dirBuf[ _MAX_PATH ];
				dirBuf[ 0 ] = _TCHAR( '\0' );
				GetWindowsDirectory( dirBuf, _MAX_PATH );
				if( strDelName.CompareNoCase( dirBuf ) )
				{
					// Not identical with windows directory...
					GetSystemDirectory( dirBuf, _MAX_PATH );
					if( strDelName.CompareNoCase( dirBuf ) )
					{
						// Not identical with system directory...
						if( strDelName.CompareNoCase( _T( "C:\\" ) ) )
						{
							// Not identical with boot directory...

							// Delete contents of this directory...
							return SVDeleteFiles( strDelName + _T( "*.*" ), TRUE );
						}
					}
				}
			}
			else
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

bool SVObserverApp::IsMatroxGige() const
{
	bool l_bOk = false;

	l_bOk = (m_csProductName.CompareNoCase( SVO_PRODUCT_KONTRON_X2_GD2A ) == 0
		|| m_csProductName.CompareNoCase( SVO_PRODUCT_KONTRON_X2_GD4A ) == 0
		|| m_csProductName.CompareNoCase( SVO_PRODUCT_KONTRON_X2_GD8A ) == 0
		|| m_csProductName.CompareNoCase( SVO_PRODUCT_KONTRON_X2_GD8A_NONIO ) == 0
		);

	return l_bOk;
}

BOOL SVObserverApp::CheckSVIMType() const
{
	BOOL Result = FALSE;

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	SVIMProductEnum SVIMType = GetSVIMType();
	//If configuration object pointer is nullptr then set the enum to invalid
	SVIMProductEnum ProductType = SVIM_PRODUCT_INVALID_TYPE;
	if( nullptr != pConfig )
	{ 
		ProductType = pConfig->GetProductType(); 
	}

	Result = SVIMType == ProductType;

	// Exceptions
	if( ! Result )
	{
		switch( SVIMType )
		{
		case SVIM_PRODUCT_X2_GD1A_COLOR:
		case SVIM_PRODUCT_X2_GD2A_COLOR:
		case SVIM_PRODUCT_X2_GD4A_COLOR:
		case SVIM_PRODUCT_X2_GD8A_COLOR:
		case SVIM_PRODUCT_X2_GD8A_NONIO_COLOR:
			{
				Result |= ProductType == SVIM_PRODUCT_X2_GD1A_COLOR;
				Result |= ProductType == SVIM_PRODUCT_X2_GD2A_COLOR;
				Result |= ProductType == SVIM_PRODUCT_X2_GD4A_COLOR;
				Result |= ProductType == SVIM_PRODUCT_X2_GD8A_COLOR;
				Result |= ProductType == SVIM_PRODUCT_X2_GD8A_NONIO_COLOR;
				break;
			}
		case SVIM_PRODUCT_X2_GD1A:
		case SVIM_PRODUCT_X2_GD2A:
		case SVIM_PRODUCT_X2_GD4A:
		case SVIM_PRODUCT_X2_GD8A:
		case SVIM_PRODUCT_X2_GD8A_NONIO:
			{
				Result |= ProductType == SVIM_PRODUCT_X2_GD1A;
				Result |= ProductType == SVIM_PRODUCT_X2_GD2A;
				Result |= ProductType == SVIM_PRODUCT_X2_GD4A;
				Result |= ProductType == SVIM_PRODUCT_X2_GD8A;
				Result |= ProductType == SVIM_PRODUCT_X2_GD8A_NONIO;
				break;
			}

		default:
			{
				// Do nothing.
				break;
			}
		}
	}
	return Result;
}

SVIMProductEnum SVObserverApp::GetSVIMType() const
{
	SVIMProductEnum eType = SVIM_PRODUCT_TYPE_UNKNOWN;

	if (0 == m_csProductName.CompareNoCase(SVO_PRODUCT_KONTRON_X2_GD1A))
	{
		eType = SVIM_PRODUCT_X2_GD1A;
	}
	else if (0 == m_csProductName.CompareNoCase(SVO_PRODUCT_KONTRON_X2_GD2A))
	{
		eType = SVIM_PRODUCT_X2_GD2A;
	}
	else if (0 == m_csProductName.CompareNoCase(SVO_PRODUCT_KONTRON_X2_GD4A))
	{
		eType = SVIM_PRODUCT_X2_GD4A;
	}
	else if (0 == m_csProductName.CompareNoCase(SVO_PRODUCT_KONTRON_X2_GD8A))
	{
		eType = SVIM_PRODUCT_X2_GD8A;
	}
	else if (0 == m_csProductName.CompareNoCase(SVO_PRODUCT_KONTRON_X2_GD8A_NONIO))
	{
		eType = SVIM_PRODUCT_X2_GD8A_NONIO;
	}
	return eType;
}

bool SVObserverApp::IsProductTypeRAID() const
{
	bool bRet = ( m_csRAIDBoardName.CompareNoCase( "Intel" ) == 0 );
	return bRet;
}

void SVObserverApp::ValidateMRUList()
{
	int i, iSize;
	if( !::IsBadReadPtr( m_pRecentFileList, sizeof(LPVOID) ) )
	{
		iSize = m_pRecentFileList->GetSize();
		for( i = iSize - 1; i >= 0; i-- )
		{
			BOOL bGone = ( _access( m_pRecentFileList->m_arrNames[i], 0 ) != 0 );
			if( bGone )
			{
				m_pRecentFileList->Remove( i );
			}
		}// end for
	}// end if
}

SVOINIClass& SVObserverApp::INI()
{
	return m_SvimIni;
}

void SVObserverApp::ResetAllCounts()
{
	long l = 0;
	long lSize = 0;

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	//If the pointer is a nullptr then lSize is 0
	if( nullptr != pConfig){ lSize = pConfig->GetInspectionCount( ); }
	for( l = 0; l < lSize; l++ )
	{
		SVInspectionProcess* pInspection = pConfig->GetInspection( l );

		if( nullptr != pInspection ){ pInspection->GetToolSet()->ResetCounts(); }
	}// end for

	if( !SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
	{
		for( l = 0; l < lSize; l++ )
		{
			SVInspectionProcess* pInspection = pConfig->GetInspection( l );

			if( nullptr != pInspection )
			{
				GuiCmd::InspectionRunOncePtr l_CommandPtr = new GuiCmd::InspectionRunOnce( pInspection->GetUniqueObjectID() );
				SVObjectSynchronousCommandTemplate< GuiCmd::InspectionRunOncePtr > l_Command( pInspection->GetUniqueObjectID(), l_CommandPtr );

				l_Command.Execute( TWO_MINUTE_CMD_TIMEOUT );
			}

		}
	}
}

// this is a recursive function which will attempt
// to add the item "itemText" to the menu with the
// given ID number. The "itemText" will be parsed for
// delimiting "\" characters for levels between
// popup menus. If a popup menu does not exist, it will
// be created and inserted at the end of the menu.
// ItemID of 0 will cause a separator to be added
bool SVObserverApp::AddMenuItem(
	HMENU hTargetMenu, 
	const CString& itemText, 
	UINT itemID)
{
	bool bSuccess = false;

	ASSERT(itemText.GetLength() > 0);
	ASSERT(hTargetMenu != NULL);

	// first, does the menu item have
	// any required submenus to be found/created?
	if (itemText.Find('\\') >= 0)
	{
		// yes, we need to do a recursive call
		// on a submenu handle and with that sub
		// menu name removed from itemText

		// 1:get the popup menu name
		CString popupMenuName = itemText.Left(itemText.Find('\\'));

		// 2:get the rest of the menu item name
		// minus the delimiting '\' character
		CString remainingText = 
			itemText.Right(itemText.GetLength() 
			- popupMenuName.GetLength() - 1);

		// 3:See whether the popup menu already exists
		int itemCount = ::GetMenuItemCount(hTargetMenu);
		bool bFoundSubMenu = false;
		MENUITEMINFO menuItemInfo;

		memset(&menuItemInfo, 0, sizeof(MENUITEMINFO));
		menuItemInfo.cbSize = sizeof(MENUITEMINFO);
		menuItemInfo.fMask = 
			MIIM_TYPE | MIIM_STATE | MIIM_ID | MIIM_SUBMENU;
		for (int itemIndex = 0 ; 
			itemIndex < itemCount && !bFoundSubMenu ; itemIndex++)
		{
			::GetMenuItemInfo(
				hTargetMenu, 
				itemIndex, 
				TRUE, 
				&menuItemInfo);
			if (menuItemInfo.hSubMenu != 0)
			{
				// this menu item is a popup menu (non popups give 0)
				TCHAR    buffer[MAX_PATH];
				::GetMenuString(
					hTargetMenu, 
					itemIndex, 
					buffer, 
					MAX_PATH, 
					MF_BYPOSITION);
				if (popupMenuName == buffer)
				{
					// this is the popup menu we have to add to
					bFoundSubMenu = true;
				}
			}
		}
		// 4: If exists, do recursive call,
		// else create do recursive call
		// and then insert it
		if (bFoundSubMenu)
		{
			bSuccess = AddMenuItem(
				menuItemInfo.hSubMenu, 
				remainingText, 
				itemID);
		}
		else
		{
			// we need to create a new sub menu and insert it
			HMENU hPopupMenu = ::CreatePopupMenu();
			if (hPopupMenu != NULL)
			{
				bSuccess = AddMenuItem(
					hPopupMenu, 
					remainingText, 
					itemID);
				if (bSuccess)
				{
					if (::AppendMenu(
						hTargetMenu, 
						MF_POPUP, 
						(UINT)hPopupMenu, 
						popupMenuName) > 0)
					{
						bSuccess = true;
						// hPopupMenu now owned by hTargetMenu,
						// we do not need to destroy it
					}
					else
					{
						// failed to insert the popup menu
						bSuccess = false;
						// stop a resource leak
						::DestroyMenu(hPopupMenu);
					}
				}
			}
		}        
	}
	else
	{	
		// 3:See whether the menu item already exists
		int itemCount = ::GetMenuItemCount(hTargetMenu);
		bool bFoundSubMenu = false;
		MENUITEMINFO menuItemInfo;

		memset(&menuItemInfo, 0, sizeof(MENUITEMINFO));
		menuItemInfo.cbSize = sizeof(MENUITEMINFO);
		menuItemInfo.fMask = 
			MIIM_TYPE | MIIM_STATE | MIIM_ID | MIIM_SUBMENU;
		for (int itemIndex = 0 ; 
			itemIndex < itemCount && !bFoundSubMenu ; itemIndex++)
		{
			::GetMenuItemInfo(
				hTargetMenu, 
				itemIndex, 
				TRUE, 
				&menuItemInfo);
			if (menuItemInfo.wID == itemID)
			{
				// Set the inspection name.
				::ModifyMenuA(hTargetMenu, itemIndex, MF_BYPOSITION | MF_STRING, itemID, itemText);
				bFoundSubMenu = true;
			}
		}

		// If not found then append menu.
		if (!bFoundSubMenu)
		{
			if (itemID != 0)
			{
				// its a normal menu command
				if (::AppendMenu(
					hTargetMenu, 
					MF_ENABLED, 
					itemID, 
					itemText) > 0)
				{
					// we successfully added the item to the menu
					bSuccess = true;
				}
			}
			else
			{
				// we are inserting a separator
				if (::AppendMenu(
					hTargetMenu, 
					MF_SEPARATOR, 
					itemID, 
					itemText) > 0)
				{
					// we successfully added the separator to the menu
					bSuccess = true;
				}
			}
		}
	}

	return bSuccess;
}

// this is a recursive function which will attempt
// to remove the menu item with "itemText" from the menu with the
// target menu handle. "itemText" will be parsed for
// delimiting "\" characters for levels between
// popup menus. If the end popup menu exists, it will be deleted.
bool SVObserverApp::RemoveMenu(
	HMENU hTargetMenu, 
	const CString& itemText)
{
	bool bSuccess = false;

	ASSERT(itemText.GetLength() > 0);
	ASSERT(hTargetMenu != NULL);

	// first, does the menu item have
	// any required submenus to be found/created?
	if (itemText.Find('\\') >= 0)
	{
		// yes, we need to do a recursive call
		// on a submenu handle and with that sub
		// menu name removed from itemText

		// 1:get the popup menu name
		CString popupMenuName = itemText.Left(itemText.Find('\\'));

		// 2:get the rest of the menu item name
		// minus the delimiting '\' character
		CString remainingText = 
			itemText.Right(itemText.GetLength() 
			- popupMenuName.GetLength() - 1);

		// 3:See whether the popup menu already exists
		int itemCount = ::GetMenuItemCount(hTargetMenu);
		bool bFoundSubMenu = false;
		MENUITEMINFO menuItemInfo;

		memset(&menuItemInfo, 0, sizeof(MENUITEMINFO));
		menuItemInfo.cbSize = sizeof(MENUITEMINFO);
		menuItemInfo.fMask = 
			MIIM_TYPE | MIIM_STATE | MIIM_ID | MIIM_SUBMENU;
		for (int itemIndex = 0 ; 
			itemIndex < itemCount && !bFoundSubMenu ; itemIndex++)
		{
			::GetMenuItemInfo(
				hTargetMenu, 
				itemIndex, 
				TRUE, 
				&menuItemInfo);
			if (menuItemInfo.hSubMenu != 0)
			{
				// this menu item is a popup menu (non popups give 0)
				TCHAR    buffer[MAX_PATH];
				::GetMenuString(
					hTargetMenu, 
					itemIndex, 
					buffer, 
					MAX_PATH, 
					MF_BYPOSITION);
				if (popupMenuName == buffer)
				{
					// this is the popup menu we are looking for.
					bFoundSubMenu = true;
				}
			}
		}
		// 4: If exists, do recursive call,
		if (bFoundSubMenu)
		{
			bSuccess = RemoveMenu( menuItemInfo.hSubMenu, remainingText);
		}
	}
	else
	{
		// See whether the popup menu exists
		int itemCount = ::GetMenuItemCount(hTargetMenu);
		bool bFoundSubMenu = false;
		MENUITEMINFO menuItemInfo;

		memset(&menuItemInfo, 0, sizeof(MENUITEMINFO));
		menuItemInfo.cbSize = sizeof(MENUITEMINFO);
		menuItemInfo.fMask = 
			MIIM_TYPE | MIIM_STATE | MIIM_ID | MIIM_SUBMENU;
		for (int itemIndex = 0 ; 
			itemIndex < itemCount && !bFoundSubMenu ; itemIndex++)
		{
			::GetMenuItemInfo(
				hTargetMenu, 
				itemIndex, 
				TRUE, 
				&menuItemInfo);
			if (menuItemInfo.hSubMenu != 0)
			{
				// this menu item is a popup menu (non popups give 0)
				TCHAR    buffer[MAX_PATH];
				::GetMenuString(
					hTargetMenu, 
					itemIndex, 
					buffer, 
					MAX_PATH, 
					MF_BYPOSITION);
				if (itemText == buffer)
				{
					// this is the popup menu we have to remove
					bSuccess = DeleteMenu( hTargetMenu, itemIndex, MF_BYPOSITION ) ? true : false;
					break;
				}
			}
		}
	}
	return bSuccess;
}

HRESULT SVObserverApp::SetMode( unsigned long p_lNewMode )
{
	HRESULT l_hr = S_OK;

	svModeEnum l_svMode = (svModeEnum) p_lNewMode;

	if( SVSVIMStateClass::CheckState( SV_STATE_START_PENDING | 
		SV_STATE_STARTING |
		SV_STATE_STOP_PENDING |
		SV_STATE_STOPING ))
	{
		l_hr = SVMSG_50_MODE_CHANGING_ERROR;
	}
	else if( SVSVIMStateClass::CheckState( SV_STATE_LOADING ) )
	{
		l_hr = SVMSG_51_MODE_CONFIGURATION_LOADING_ERROR;
	}
	else if( SVSVIMStateClass::CheckState( SV_STATE_EDITING ) )
	{
		l_hr = SVMSG_52_MODE_GUI_IN_USE_ERROR;
	}
	else if( l_svMode == SVIM_MODE_ONLINE )
	{
		if( SVSVIMStateClass::CheckState( SV_STATE_TEST ) ||
			SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
		{
			OnStop();
		}
		// Try to go online...
		if( !SVSVIMStateClass::CheckState( SV_STATE_REGRESSION ) &&
			SVSVIMStateClass::CheckState( SV_STATE_READY ) )
		{
			try
			{
				if( Start() != S_OK )
					l_hr = SVMSG_SVIMCMD_GO_ONLINE_FAILED;
			}
			catch (const SvStl::MessageContainer& rExp)
			{ //Log exception, (do not display the error because it is called from remote)
				SvStl::MessageMgrNoDisplay  Exception( SvStl::LogOnly );
				Exception.setMessage( rExp.getMessage() );
				l_hr = SVMSG_SVIMCMD_GO_ONLINE_FAILED;
			}
		}
		else
		{
			l_hr = SVMSG_SVIMCMD_GO_ONLINE_FAILED;
		}
	}
	else if( l_svMode == SVIM_MODE_OFFLINE )
	{
		// Go offline
		if( SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) ||
			SVSVIMStateClass::CheckState( SV_STATE_TEST ) )
		{
			OnStop();
		}

		// Cancel edit mode
		if( SVSVIMStateClass::CheckState( SV_STATE_EDIT ) )
		{
			SetModeEdit( false );
		}
	}
	else if( l_svMode == SVIM_MODE_TEST )
	{
		// If the previous mode was running then we cannot stop and 
		// go straight into test mode.  This has caused a crash in testing 
		// when two IPDs are present.  Therefore the activeX user must first
		// put the system in some offline mode and then put it into test mode
		// in two steps.  If running then return request rejected.
		if( SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
		{
			l_hr = SVMSG_SVIMCMD_REQUEST_REJECTED;
		}
		else if( SVSVIMStateClass::CheckState( SV_STATE_REGRESSION ) )
		{
			OnStop();
		}

		if( !SVSVIMStateClass::CheckState( SV_STATE_TEST ) &&
			SVSVIMStateClass::CheckState( SV_STATE_READY ) )
		{
			SetTestMode(true);
		}
		else
		{
			l_hr = SVMSG_SVIMCMD_REQUEST_REJECTED;
		}
	}
	else if( l_svMode == SVIM_MODE_REGRESSION )
	{
		// Later Currently not supported through the Control
		l_hr = SVMSG_SVIMCMD_REQUEST_REJECTED;
	}
	else if( l_svMode == SVIM_MODE_EDIT )
	{
		if( SVSVIMStateClass::CheckState( SV_STATE_TEST ) ||
			SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
		{
			OnStop();
		}

		if( !SVSVIMStateClass::CheckState( SV_STATE_REGRESSION ) &&
			SVSVIMStateClass::CheckState( SV_STATE_READY ) )
		{
			SetModeEdit( true );
		}
		else
		{
			l_hr = SVMSG_SVIMCMD_REQUEST_REJECTED;
		}
	}
	else
	{
		l_hr = SVMSG_SVIMCMD_REQUEST_REJECTED;
	}

	PostMessage( m_pMainWnd->m_hWnd, SV_REFRESH_STATUS_BAR, 0, 0 );
	return l_hr;
}

HRESULT SVObserverApp::LoadConfiguration()
{
	HRESULT l_Status = S_OK;

	CString szConfigName = SVRCGetSVCPathName();

	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szFile[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];
	CString sComp;

	_tsplitpath( szConfigName, szDrive, szDir, szFile, szExt );
	sComp = szExt;

	if( sComp.CompareNoCase(_T(".svx"))==0 )
	{
		l_Status = OpenSVXFile( szConfigName );
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

HRESULT SVObserverApp::RenameObject( const SVString& p_rOldName, const SVString& p_rNewName, const SVGUID& p_rObjectId )
{
	HRESULT l_Status = S_OK;

	SVObjectAppClass* l_pObject = dynamic_cast< SVObjectAppClass* >( SVObjectManagerClass::Instance().GetObject( p_rObjectId ) );

	if( l_pObject != NULL )
	{
		SVRenameObject l_RenameObject( p_rOldName, p_rNewName, p_rObjectId );

		SVInspectionProcess* l_pInspect = l_pObject->GetInspection();

		if( l_pInspect != NULL )
		{
			SVObjectManagerClass::Instance().UpdateObserver( l_pInspect->GetUniqueObjectID(), l_RenameObject );
		}
		else
		{
			l_Status = E_FAIL;
		}

		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		if( nullptr != pConfig )
		{
			SVObjectManagerClass::Instance().UpdateObserver( pConfig->GetUniqueObjectID(), l_RenameObject );
		}
		else
		{
			l_Status = E_FAIL;
		}
		SVIODoc* pIODoc = GetIODoc();
		if (pIODoc)
		{
			pIODoc->UpdateAllViews( NULL );
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVObserverApp::RebuildOutputList()
{
	HRESULT l_Status = S_OK;

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig )
	{
		pConfig->RebuildOutputObjectList();

		SVIODoc* pIODoc = GetIODoc();
		if (pIODoc)
		{
			pIODoc->UpdateAllViews( NULL );
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : SetStatusText
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
BOOL SVObserverApp::SetStatusText( LPCTSTR PStrStatusText )
{
	// If PStrStatusText is NULL, the Main Frame deletes the last status info.
	// If no Main Frame exists, the function returns FALSE.
	// Otherwise ( and this should always be ) it returns TRUE!
	if( m_pMainWnd )
	{
		GetMainFrame()->SetStatusInfoText( PStrStatusText );
		return TRUE;
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : UpdatePPQBar()
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
void SVObserverApp::UpdatePPQBar()
{
	GetMainFrame()->OnViewPPQBar();
}

LPCTSTR SVObserverApp::getConfigFileNameOnly() const
{
	return m_ConfigFileName.GetFileNameOnly();
}

LPCTSTR SVObserverApp::getConfigPathName() const
{
	return m_ConfigFileName.GetPathName();
}

LPCTSTR SVObserverApp::getConfigFileName() const
{
	return m_ConfigFileName.GetFileName();
}

LPCTSTR SVObserverApp::getConfigFullFileName() const
{
	return m_ConfigFileName.GetFullFileName();
}

BOOL SVObserverApp::setConfigFullFileName(LPCTSTR csFullFileName, DWORD dwAction)
{
	BOOL bOk = FALSE;

	SVFileNameManagerClass svFileManager;

	bOk = m_ConfigFileName.SetFullFileName( csFullFileName );

	if ( bOk )
	{
		if ( CString( m_ConfigFileName.GetPathName() ).IsEmpty() )
		{
			svFileManager.SetConfigurationPathName( NULL );
		}
		else
		{
			if ( CString( m_ConfigFileName.GetPathName() ).CompareNoCase( _T( "C:\\RUN" ) ) == 0 )
			{
				svFileManager.SetConfigurationPathName( NULL );
			}
			else
			{
				bOk = svFileManager.SetConfigurationPathName( m_ConfigFileName.GetPathName() );
				// if this returns FALSE, unable to access specified path!
				if ( !bOk )
				{
					ASSERT( dwAction == SAVE_CONFIG ||
						dwAction == LOAD_CONFIG ||
						dwAction == RENAME_CONFIG );

					SVStringArray msgList;
					msgList.push_back(SvStl::MessageData::convertId2AddtionalText(dwAction == LOAD_CONFIG ? SvOi::Tid_Load : SvOi::Tid_Save));
					msgList.push_back(m_ConfigFileName.GetPathName());
					SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_UnableConfig, msgList, StdMessageParams, SvOi::Err_10132 );
				}
			}
		}

		if ( bOk )
		{
			switch( dwAction )
			{
			case LOAD_CONFIG:
				{
					bOk = svFileManager.LoadItem( &m_ConfigFileName );
					break;
				}
			case SAVE_CONFIG:
				{
					bOk = svFileManager.SaveItem( &m_ConfigFileName );
					break;
				}
			case RENAME_CONFIG:
				{
					bOk = svFileManager.RenameItem( &m_ConfigFileName );
					break;
				}
			default:
				{
					// Do nothing.
					break;
				}
			}
		}
	}

	if ( bOk )
	{
		if ( !CString( svFileManager.GetConfigurationPathName() ).IsEmpty() )
		{
			AfxGetApp()->WriteProfileString( _T( "Settings" ), 
				_T( "ConfigurationFilePath" ), 
				svFileManager.GetConfigurationPathName() );
		}
	}

	return bOk;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : GetIPDoc
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
SVIPDoc* SVObserverApp::GetIPDoc( LPCTSTR StrIPDocPathName ) const
{
	// If the function succeeds, it returns a pointer to the found IPDoc.
	// If the function succeeds and the IPDoc is not currently loaded or
	// if the function fails, it returns NULL !!!
	// !!! The function doesn´t care about lowercase or uppercase strings !!!

	if( SVSVIMStateClass::CheckState( SV_STATE_READY | SV_STATE_RUNNING ) && StrIPDocPathName )
	{
		CString lowerIPDocName = StrIPDocPathName;
		lowerIPDocName = lowerIPDocName.Left( lowerIPDocName.Find( '.' ) );

		POSITION pos = GetFirstDocTemplatePosition();
		if( pos )
		{
			do
			{
				CDocTemplate* pDocTemplate = GetNextDocTemplate( pos );
				if( pDocTemplate )
				{
					POSITION posDoc = pDocTemplate->GetFirstDocPosition();
					if ( posDoc )
					{
						do
						{
							CDocument* newDoc = pDocTemplate->GetNextDoc(posDoc);
							if ( newDoc )
							{
								SVIPDoc* pTmpDoc = dynamic_cast <SVIPDoc*> (newDoc);

								if( pTmpDoc != NULL )
								{
									CString strName = pTmpDoc->GetTitle();

									if( strName.CompareNoCase( lowerIPDocName ) == 0 )
									{
										return pTmpDoc;
									}
								}
							}
						}
						while( posDoc );
					}
				}
			}
			while( pos );
		}
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : AlreadyExistsIPDocTitle
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
//  :17.07.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVObserverApp::AlreadyExistsIPDocTitle( LPCTSTR StrIPDocTitle )
{
	return FALSE;
}

CString SVObserverApp::GetStringResource( int ResourceID ) const
{
	CString strResource;
	if( ! strResource.LoadString( ResourceID ) ) { strResource = _T( " " ); }

	return strResource;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : ShowConfigurationAssistant
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
//  :04.03.1999 RO			First Implementation
//	:20.10.1999 RO			Added File New Configuration Capability
////////////////////////////////////////////////////////////////////////////////
BOOL SVObserverApp::ShowConfigurationAssistant( int Page /*= 3*/, 
	BOOL bFileNewConfiguration /*= FALSE*/ )
{
	BOOL bOk = FALSE;

	// Access denied, if... // Check Edit Mode
	if( SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) )
		return FALSE;

	if ( bFileNewConfiguration )
	{
		SVFileNameManagerClass svFileManager;

		// Clean up SVObserver
		if( S_OK != DestroyConfig() )
		{
			// Needs Attention !!!
			// Should get error code if returned S_FALSE; 
			// If returned ERROR_CANCELLED, user cancelled
			return FALSE;
		}

		if (bFileNewConfiguration)
		{
			RootObject *pRoot = nullptr;

			SVObjectManagerClass::Instance().GetRootChildObject( pRoot, SvOl::FqnRoot );
			if(nullptr != pRoot)
			{
				pRoot->createConfigurationObject();
			}
		}

		// Clean up Execution Directory...
		// Check path, create if necessary and delete contents...
		InitPath( CString( svFileManager.GetRunPathName() ) + "\\", TRUE, TRUE );

		// Ensure that DestroyConfig() can do his work...
		SVSVIMStateClass::AddState( SV_STATE_READY );
	}
	else
	{
		CWaitCursor wait;

		SVSVIMStateClass::CheckState( SV_STATE_READY );
		ResetAllIPDocModifyFlag(FALSE);
	}

	//****RPY - Start - added new AppAssistant

	CSVOConfigAssistantDlg cDlg;

	SVIMProductEnum eSVIMType = GetSVIMType();

	cDlg.SetCurrentSystem( eSVIMType );

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig )
	{
		if( pConfig->GetProductType() != SVIM_PRODUCT_TYPE_UNKNOWN )
		{
			cDlg.SetConfigurationSystem( pConfig->GetProductType() );
		}
		else
		{
			cDlg.SetConfigurationSystem( eSVIMType );
		}
	}
	else
	{
		cDlg.SetConfigurationSystem( eSVIMType );
	}

	SVIOBoardCapabilities l_svCapable;
	unsigned long l_lCount;
	SVIOConfigurationInterfaceClass::Instance().GetDigitalInputCount( l_lCount );
	l_svCapable.SetInputCount( l_lCount );
	SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputCount( l_lCount );
	l_svCapable.SetOutputCount( l_lCount );

	// Special code to determine the inverters 
	// and triggers based on IO board
	if( m_csIOBoard.Compare( "10" ) == 0 )
	{
		l_svCapable.SetStrobeInverters( 1 );
		l_svCapable.SetTriggerInverters( 1 );
		l_svCapable.SetTriggerCount( 1 );
	}
	else if( m_csIOBoard.Compare("12") == 0 )
	{
		l_svCapable.SetStrobeInverters( 1 );
		l_svCapable.SetTriggerInverters( 1 );
		l_svCapable.SetTriggerCount( 3 );
	}
	else if( m_csIOBoard.Compare("00") == 0 )
	{
		// SEJ 303 - Get Trigger count from the TriggerDLL (in this case the DigitizerDLL)
		int numTriggers = 0;
		const SVIMTypeInfoStruct& info = SVHardwareManifest::GetSVIMTypeInfo(eSVIMType);
		l_svCapable.SetNonIOSVIM(info.m_MaxTriggers);
	}
	else 
	{
		l_svCapable.SetStrobeInverters( 3 );
		l_svCapable.SetTriggerInverters( 3 );
		l_svCapable.SetTriggerCount( 3 );
	}

	cDlg.SetIOBoardCapabilities( l_svCapable );

	cDlg.SetNewConfiguration( bFileNewConfiguration );

	SVSVIMStateClass::AddState( SV_STATE_EDITING );
	if (cDlg.DoModal() == IDOK)
	{
		bOk = TRUE;
		if (cDlg.Modified())
		{
			SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
		}
		if ( nullptr != pConfig )
		{
			pConfig->ClearRemoteOutputUnUsedData();
			pConfig->ValidateRemoteMonitorList();
			SVIODoc* pIODoc = GetIODoc();
			if (pIODoc)
			{
				pIODoc->UpdateAllViews( NULL );
			}
		}
	}
	SVSVIMStateClass::RemoveState( SV_STATE_EDITING );

	CString sNewName;

	//****RPY - End - added new AppAssistant

	// Call the application assistant dialog...

	if ( bOk )
	{
		CWaitCursor wait;

		sNewName = cDlg.GetConfigurationName();
		if( bFileNewConfiguration )
		{
			m_ConfigFileName.SetFileNameOnly( sNewName );
			m_ConfigFileName.SetExtension( ".svx" );

			SVDigitalInputObject *pInput = NULL;
			SVPPQObject			 *pPPQ = NULL;
			CString strName;
			unsigned long ulCount = 0;
			unsigned long l = 0;
			long lPPQ = 0;
			long lPPQCount = 0;

			SVIOConfigurationInterfaceClass::Instance().GetDigitalInputCount( ulCount );

			if ( nullptr != pConfig )
			{
				// Create all the default inputs
				SVInputObjectList* pInputObjectList = pConfig->GetInputObjectList( );
				if (nullptr != pInputObjectList)
				{
					for( l = 0; l < ulCount; l++ )
					{
						pInput = nullptr;

						strName.Format( "DIO.Input%d", l+1 );

						pInputObjectList->GetInputFlyweight( static_cast< LPCTSTR >( strName ), pInput );

						if( nullptr != pInput )
						{
							pInput->SetChannel( l );
						}
					}// end for
				}

				// Make all the PPQs build their default digital inputs
				lPPQCount = pConfig->GetPPQCount( );
				for( lPPQ = 0; lPPQ < lPPQCount; lPPQ++ )
				{
					pPPQ = pConfig->GetPPQ( lPPQ );
					ASSERT (nullptr != pPPQ);
					pPPQ->RebuildInputList(pConfig->HasCameraTrigger(pPPQ));
					pPPQ->RebuildOutputList();
				}// end for

				// Create all the default outputs
				SVOutputObjectList *pOutputObjectList = pConfig->GetOutputObjectList( );
				if (nullptr != pOutputObjectList)
				{
					SVDigitalOutputObject *pOutput = nullptr;
					pOutputObjectList->GetOutputFlyweight( "Module Ready", pOutput );

					// @HACK:  JAB082212 HACK!!!!!
					if( nullptr != pOutput )
					{
						pOutput->SetChannel( 15 );

						pConfig->GetModuleReady()->m_IOId = pOutput->GetUniqueObjectID();

						SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputIsInverted (15, pOutput->IsInverted ());
						SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputIsForced (15, pOutput->IsForced ());
						SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputForcedValue (15, pOutput->GetForcedValue ());
					}
				}
			}
		}

		if ( nullptr != pConfig )
		{
			bOk = pConfig->RebuildInputOutputLists();

			ConstructMissingDocuments();

			// Check for Imported Inspection and adjust views, window placement and conditional history
			const SVImportedInspectionInfoList& infoList = cDlg.GetImportedInspectionInfoList();
			for (SVImportedInspectionInfoList::const_iterator it = infoList.begin();it != infoList.end();++it)
			{
				const SVImportedInspectionInfo& info = (*it);
				SVIPDoc* pDoc = SVObjectManagerClass::Instance().GetIPDoc(info.m_inspectionGuid);
				if (pDoc)
				{
					SVIPDocInfoImporter::Import(pDoc, info);
				}
			}
			cDlg.ClearImportedInspectionInfoList();

			if ( TheSVOLicenseManager().HasToolErrors() )
			{
				TheSVOLicenseManager().ShowLicenseManagerErrors();
			}
			else
			{
				//close dialog if it is up and has no errors
				TheSVOLicenseManager().ClearLicenseErrors();
			}

			//
			// Set the tool selected for an operator to move if any.
			// 26 Jan 2000 - frb.
			//
			AfxGetMainWnd()->PostMessage( SV_SET_TOOL_SELECTED_IN_TOOL_VIEW, (WPARAM) TRUE );

			bOk = pConfig->Activate() && bOk;
		}

		if( bFileNewConfiguration )
		{
			UpdateAllMenuExtrasUtilities();
			SVSVIMStateClass::AddState( SV_STATE_READY );
			SVSVIMStateClass::RemoveState( SV_STATE_AVAILABLE );
		}

		// Rebuild PPQBar Buttons
		GetMainFrame()->DestroyPPQButtons();
		GetMainFrame()->BuildPPQButtons();

		// Validate Output List TB
		if( SVMSG_SVO_70_DUPLICATE_DISCRETE_OUTPUT == pConfig->ValidateOutputList() )
		{
			SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
			Exception.setMessage( SVMSG_SVO_70_DUPLICATE_DISCRETE_OUTPUT, SvOi::Tid_Empty, StdMessageParams );
		}
	}// end if DoModal == IDOK

	if ( ! bOk )
	{
		if( bFileNewConfiguration )
		{
			SVSVIMStateClass::AddState( SV_STATE_AVAILABLE );
			SVSVIMStateClass::RemoveState( SV_STATE_READY );
			SVSVIMStateClass::RemoveState( SV_STATE_MODIFIED );
		}
	}

	return bOk;
}

bool SVObserverApp::OkToEdit()
{
	bool l_bOk = false;
	if( SVSVIMStateClass::CheckState( SV_STATE_EDIT ))
	{
		if( m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_MODE_MENU_EDIT_TOOLSET ))
		{
			l_bOk = true;
		}
	}
	return l_bOk;
}

BOOL SVObserverApp::IsMonochromeImageAvailable()
{
	BOOL Monochrome(false);

	CMDIChildWnd* pMDIChild( nullptr );
	if( m_pMainWnd && ( pMDIChild = ( ( CMDIFrameWnd* ) m_pMainWnd )->MDIGetActive() ) )
	{
		m_pCurrentDocument = dynamic_cast< SVIPDoc* >( pMDIChild->GetActiveDocument() );
	}
	if( nullptr != m_pCurrentDocument )
	{
		Monochrome = !m_pCurrentDocument->IsColorInspectionDocument();
	}

	if ( !Monochrome )
	{
		SVObjectTypeInfoStruct info;

		info.ObjectType = SVImageObjectType;
		info.SubType = SVNotSetSubObjectType;

		SVGetObjectDequeByTypeVisitor l_Visitor( info );

		SVObjectManagerClass::Instance().VisitElements( l_Visitor, m_pCurrentDocument->GetInspectionID() );

		SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;

		for( l_Iter = l_Visitor.GetObjects().begin(); !Monochrome && l_Iter != l_Visitor.GetObjects().end(); ++l_Iter )
		{
			SVImageClass* pImage = dynamic_cast< SVImageClass* >( const_cast< SVObjectClass* >( *l_Iter ) );

			if( pImage != NULL && pImage->GetTool() != NULL )
			{
				if( m_pCurrentDocument->GetSelectedToolID() != pImage->GetTool()->GetUniqueObjectID() )
				{
					SVImageInfoClass ImageInfo = pImage->GetImageInfo();

					long l_lBandNumber = 1;

					Monochrome = ImageInfo.GetImageProperty( SVImagePropertyBandNumber, l_lBandNumber ) == S_OK && l_lBandNumber == 1;
				}
				else
				{
					break;
				}
			}
		}
	}

	return Monochrome;
}

void SVObserverApp::SVGetCurrentConfigName(CString &ConfigName) const
{
	ConfigName = getConfigFullFileName();
}

void SVObserverApp::OnRCOpenCurrentSVX()
{
	OpenSVXFile( SVRCGetSVCPathName() );
}

void SVObserverApp::UpdateAllMenuExtrasUtilities()
{
	SVUtilitiesClass util;
	CMDIFrameWnd* pMainFrame;
	CMenu* pMenu;
	CString sMenuText;
	HMENU hMenu=NULL;
	CMenu menu;

	pMainFrame = (CMDIFrameWnd*) AfxGetMainWnd ();
	hMenu = pMainFrame->m_hMenuDefault;    // default menu
	menu.Attach(hMenu);
	pMenu = &menu;
	sMenuText = _T("&Utilities");

	if (pMenu = util.FindSubMenuByName (pMenu, sMenuText))
	{
		menu.Detach();
		util.LoadMenu(pMenu);
	}
	else
	{
		menu.Detach();
	}

	// now cycle through all child MDI windows
	CMDIChildWnd* pMdiChild = pMainFrame->MDIGetActive();

	while (pMdiChild != NULL)
	{
		hMenu = ((CMdiChildWorkaround*)pMdiChild)->GetMenu();
		menu.Attach(hMenu);
		pMenu = &menu;

		if (pMenu = util.FindSubMenuByName (pMenu, sMenuText))
		{
			menu.Detach();
			util.LoadMenu(pMenu);
		}
		else
		{
			menu.Detach();
		}
		pMdiChild = (CMDIChildWnd*) pMdiChild->GetWindow(GW_HWNDNEXT);
	}
}

// Source Image Depth plus 2 - adjusted length to handle 
// locking issues with runonce and MaintainImages
long SVObserverApp::GetSourceImageDepth() const
{
	return m_lSouceImageDepth + 2;
}

// Returns sourceImageDepth for the PPQ Length
// This is not the real sourceImageDepth.
long SVObserverApp::GetMaxPPQLength() const
{
	return m_lSouceImageDepth;
}

long SVObserverApp::GetResultImageDepth() const
{
	return m_lResultImageDepth;
}

long SVObserverApp::GetLogDataManager() const
{
	return m_LogDataManager;
}

long SVObserverApp::UpdateAndGetLogDataManager()
{
	m_LogDataManager = GetProfileInt(_T( "Settings" ),_T( "Log Data Manager" ), -1 );
	if ( m_LogDataManager == -1 )
	{
		WriteProfileInt(_T( "Settings" ),_T( "Log Data Manager" ), 0 );

		m_LogDataManager = 0;
	}

	return m_LogDataManager;
}

void SVObserverApp::DeselectTool()
{
	SVMainFrame* pWndMain = ( SVMainFrame* )GetMainWnd();
	if(pWndMain)
	{
		pWndMain->PostMessage( SV_SET_TOOL_SELECTED_IN_TOOL_VIEW, (WPARAM)FALSE );   // Deselect any tool selected for move.
	}
}

HRESULT SVObserverApp::DisplayCameraManager(SVIMProductEnum eProductType) 
{
	HRESULT hr = S_OK;

	CStringArray saCameras;
	HCURSOR hCursor=NULL;
	hCursor = ::LoadCursor( NULL, IDC_WAIT );
	hCursor = ::SetCursor( hCursor );

	try
	{
		hr = DisconnectCameras( saCameras );
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	try
	{
		::SetCursor( hCursor );

		if ( hr == S_OK )
		{
			if (SVHardwareManifest::IsMatroxGige(eProductType))
			{
				SVGigeCameraManagerDlg dlg;
				dlg.DoModal();
			}
		}
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	hCursor = ::LoadCursor( NULL, IDC_WAIT );
	hCursor = ::SetCursor( hCursor );

	ConnectCameras( saCameras );

	::SetCursor( hCursor );

	return hr;
}

HRESULT SVObserverApp::DisconnectToolsetBuffers()
{
	// EB 2002 02 28
	// Added this to fix memory leak freeing MIL buffers
	// before we close Acq devices, we need to tell all toolsets to Close
	// this closes the connection between toolset images and the MIL acq image.

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	long lInspectionCount = 0;
	
	//If the pointer is a nullptr then the count will be 0
	if( nullptr != pConfig ){ lInspectionCount = pConfig->GetInspectionCount( ); }

	for ( long lInspection = 0; lInspection < lInspectionCount; lInspection++ )
	{
		SVInspectionProcess* pInspection = pConfig->GetInspection( lInspection );
		if ( nullptr != pInspection )
		{
			if ( nullptr != pInspection->GetToolSet() )
			{
				pInspection->DisconnectToolSetMainImage();
			}
		}
	}

	return S_OK;
}

HRESULT SVObserverApp::ConnectToolsetBuffers()
{
	// EB 2002 02 28
	// Added this to fix memory leak freeing MIL buffers
	// before we close Acq devices, we need to tell all toolsets to Close
	// reopen the connection between toolset images and the MIL acq image here.

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	long lInspectionCount = 0;
	SVInspectionProcess* pInspection( nullptr );
	//If the pointer is a nullptr then the count will be 0
	if( nullptr != pConfig ){ lInspectionCount = pConfig->GetInspectionCount( ); }

	for ( long lInspection = 0; lInspection < lInspectionCount; lInspection++ )
	{
		SVInspectionProcess* pInspection = pConfig->GetInspection( lInspection );
		if ( nullptr != pInspection )
		{
			if ( nullptr != pInspection->GetToolSet() )
			{
				pInspection->ConnectToolSetMainImage();
			}
		}
	}

	return S_OK;
}

HRESULT SVObserverApp::DisconnectCameras( CStringArray& rDisconnectedCameras )
{
	HRESULT hr = S_OK;

	try
	{
		hr = DisconnectAllCameraBuffers( rDisconnectedCameras );
	}
	catch (...)
	{
		ASSERT(FALSE);
		hr = SV_FALSE;
	}

	SVDigitizerProcessingClass::Instance().DisconnectDevices();

	return hr;
}

HRESULT SVObserverApp::ConnectCameras( const CStringArray& rCamerasToConnect )
{
	HRESULT hr = S_OK;

	SVDigitizerProcessingClass::Instance().ConnectDevices();

	try
	{
		hr = ConnectCameraBuffers( rCamerasToConnect );
	}
	catch (...)
	{
		ASSERT(FALSE);
		hr = SV_FALSE;
	}

	return hr;
}

HRESULT SVObserverApp::SendCameraParameters( const CStringArray& rCameras )
{
	HRESULT hr = S_OK;
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	for ( int i=0; i < rCameras.GetSize(); i++ )
	{
		SVDeviceParamCollection* pDeviceParams = NULL;

		SVFileNameArrayClass* pDummyFiles = NULL;
		SVLightReference* pDummyLight = NULL;
		SVLut* pDummyLut = NULL;
		
		CString DigitizerName( rCameras.GetAt(i) );
		SVAcquisitionClassPtr pDevice( SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( DigitizerName ) );
		if ( nullptr != pDevice && nullptr != pConfig &&
			pConfig->GetAcquisitionDevice(rCameras.GetAt(i), pDummyFiles, pDummyLight, pDummyLut, pDeviceParams ) )
		{
			// Check for the camera file and camera to match.
			if( hr == S_OK )
			{
				HRESULT l_hrTmp = pDevice->IsValidCameraFileParameters( *pDeviceParams );
				if( l_hrTmp != E_NOTIMPL && l_hrTmp != S_OK)
				{
					hr = SV_CAN_GO_ONLINE_FAILURE_ACQUISITION;
				}
			}

			// Send GIGE packet size if was set from hardware.ini
			if (IsMatroxGige() && m_gigePacketSize != 0)
			{
				SetGigePacketSizeDeviceParam(pDeviceParams);
			}

			if (pDeviceParams)
			{
				pDevice->SetDeviceParameters( *pDeviceParams );	// must be done before CreateBuffers (in case CameraFormat changes)
			}

			if (pDummyLight)
			{
				pDevice->SetLightReference( *pDummyLight );	// I HATE LIGHT REFERENCE
			}
			// LR needs to be redone so that it does not contain values, only references to device params.
		}
	}

	return hr;
}

void SVObserverApp::SetGigePacketSizeDeviceParam(SVDeviceParamCollection* pDeviceParams)
{
	// check if Packet Size Device Param exists
	SVDeviceParam* l_pGigePacketSize = pDeviceParams->GetParameter( DeviceParamGigePacketSize );
	// if found - update it
	// else - add it
	if ( l_pGigePacketSize != NULL )
	{
		l_pGigePacketSize->SetValue(_variant_t(m_gigePacketSize));
	}
	else // add it
	{
		pDeviceParams->SetParameter( DeviceParamGigePacketSize, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( DeviceParamGigePacketSize )) );
		SVLongValueDeviceParam* pParam = pDeviceParams->GetParameter( DeviceParamGigePacketSize ).DerivedValue(pParam);
		ASSERT( pParam );
		pParam->lValue = m_gigePacketSize;
		pParam->SetName(DeviceParamGigePacketSize_String);
	}
}

HRESULT SVObserverApp::SetModeEdit( bool p_bState ) 
{
	HRESULT l_hr = S_OK;
	if( p_bState )
	{
		SVSVIMStateClass::AddState( SV_STATE_EDIT );
		SVSVIMStateClass::AddState( SV_STATE_SECURED );
	}
	else
	{
		SVSVIMStateClass::RemoveState( SV_STATE_EDIT );
		SVSVIMStateClass::RemoveState( SV_STATE_SECURED );
		//
		// We need to deselect any tool that might be set for operator move.
		//
		DeselectTool();
	}
	return l_hr;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : SetTestMode
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
void SVObserverApp::SetTestMode(bool p_bNoSecurity )
{
	bool l_bAllowAccess = false;

	if ( SVSVIMStateClass::CheckState( SV_STATE_TEST ) )
	{
		if ( !SVSVIMStateClass::CheckState( SV_STATE_REGRESSION ) )
		{
			return;
		}
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
	{
		if( p_bNoSecurity || m_svSecurityMgr.SVValidate( SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE, 
			SECURITY_POINT_MODE_MENU_TEST ) == S_OK )
		{
			OnStop();
			l_bAllowAccess = true;
		}
	}
	else if( p_bNoSecurity || m_svSecurityMgr.SVValidate( SECURITY_POINT_MODE_MENU_TEST ) == S_OK )
	{
		l_bAllowAccess = true;
	}

	if( l_bAllowAccess )
	{
		if ( SVSVIMStateClass::CheckState( SV_STATE_REGRESSION ) )
		{
			StopRegression();
		}

		if( !SVSVIMStateClass::CheckState( SV_STATE_TEST ) && 
			!SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_STOP_PENDING | SV_STATE_STOPING ) )
		{
			UpdateAndGetLogDataManager();

			if( m_pMainWnd )
			{
				SVPPQObject* pPPQ( nullptr );
				long l;
				long lSize = 0;
				HRESULT hrReady = S_OK;

				SVConfigurationObject* pConfig( nullptr );
				SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

				SVSoftwareTriggerDlg & l_trgrDlg = SVSoftwareTriggerDlg::Instance();
				l_trgrDlg.ShowWindow(SW_HIDE);
				l_trgrDlg.ClearTriggers();
				//If the pointer is a nullptr the lSize is 0
				if( nullptr != pConfig ){ lSize = pConfig->GetPPQCount( ); }
				HRESULT l_hrOk = S_OK;
				for( l = 0; l_hrOk == S_OK && l < lSize; l++ )
				{
					pPPQ = pConfig->GetPPQ( l );
					if( nullptr == pPPQ )
					{
						l_hrOk = S_FALSE;
						break;
					}
					l_hrOk = pPPQ->CanGoOnline();
					SVTriggerObject* pTrigger( nullptr );
					pPPQ->GetTrigger( pTrigger );
					if ( nullptr != pTrigger && pTrigger->IsSoftwareTrigger())
					{
						l_trgrDlg.AddTrigger(pTrigger);
					}
				}// end for

				if (l_trgrDlg.HasTriggers())
				{
					l_trgrDlg.SelectTrigger();
				}
				lSize = 0;
				//If the pointer is a nullptr the lSize is 0
				if( nullptr != pConfig ){ lSize = pConfig->GetPPQCount(); }
				for( l = 0; hrReady == S_OK && l < lSize; l++ )
				{
					pPPQ = pConfig->GetPPQ( l );
					if( nullptr != pPPQ )
					{
						hrReady = pPPQ->CanGoOnline();
					}
				}// end for

				if( hrReady != S_OK )
				{
					return;
				}

				for( l = 0; hrReady == S_OK && l < lSize; l++ )
				{
					pPPQ = pConfig->GetPPQ( l );
					if( nullptr != pPPQ )
					{
						hrReady = pPPQ->GoOnline();
					}
				}// end for

				if( hrReady != S_OK )
				{
					for( l = 0; l < lSize; l++ )
					{
						pPPQ = pConfig->GetPPQ( l );
						if( nullptr != pPPQ )
						{
							pPPQ->GoOffline();
						}
					}// end for

					SetAllIPDocumentsOffline();
					return;
				}// end if

				SetAllIPDocumentsOnline();

				GetMainFrame()->ShowToolBars( FALSE, TRUE, SVMainFrame::Flag_Zoom );
				GetMainFrame()->RefreshAllSplitters();

				SVSVIMStateClass::AddState( SV_STATE_TEST );
				SVSVIMStateClass::RemoveState( SV_STATE_EDIT );

				if( nullptr != pConfig){ pConfig->SetModuleReady( true ); }

				SVObjectManagerClass::Instance().SetState( SVObjectManagerClass::ReadOnly );

				if (SVSoftwareTriggerDlg::Instance().HasTriggers())
				{
					EnableTriggerSettings();
				}
				else
				{
					DisableTriggerSettings();
				}
			}// end if
		}
	}
}

HRESULT SVObserverApp::GetTriggersAndCounts( CString& p_strTrigCnts ) const
{
	HRESULT l_hr = S_FALSE;
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig )
	{
		long l_lCount = pConfig->GetPPQCount();
		for( int l_lPPQ = 0 ; l_lPPQ < l_lCount; l_lPPQ++)
		{
			SVPPQObject* pPPQ = pConfig->GetPPQ(l_lPPQ);
			//Returns true when pointer valid
			if( nullptr != pPPQ )
			{
				CString l_strTmp;
				SVTriggerObject* pTrigger( nullptr );
				//If returns true has valid pointer
				if( pPPQ->GetTrigger( pTrigger ))
				{
					l_hr = S_OK;
					pTrigger->m_lTriggerCount;
					l_strTmp.Format( "\n%s count-%d", pTrigger->GetName(), pTrigger->m_lTriggerCount );
					p_strTrigCnts += l_strTmp;
				}
			}
		}
	}
	return l_hr;
}

void SVObserverApp::RefreshAllIPDocuments()
{
	//get list of IPDoc's.

	POSITION pos = GetFirstDocTemplatePosition();
	if( pos )
	{
		do
		{
			CDocTemplate* pDocTemplate = GetNextDocTemplate( pos );
			if( pDocTemplate )
			{
				POSITION posDoc = pDocTemplate->GetFirstDocPosition();
				if ( posDoc )
				{
					do
					{
						CDocument* newDoc = pDocTemplate->GetNextDoc(posDoc);
						if ( newDoc )
						{
							SVIPDoc* pTmpDoc = dynamic_cast <SVIPDoc*> (newDoc);
							if ( pTmpDoc != NULL )
							{
								pTmpDoc->UpdateWithLastProduct();
							}
						}
					}
					while( posDoc );
				}
			}
		}
		while( pos );
	}
}

void SVObserverApp::RunAllIPDocuments()
{
	//get list of IPDoc's.

	POSITION pos = GetFirstDocTemplatePosition();
	if( pos )
	{
		do
		{
			CDocTemplate* pDocTemplate = GetNextDocTemplate( pos );
			if( pDocTemplate )
			{
				POSITION posDoc = pDocTemplate->GetFirstDocPosition();
				if ( posDoc )
				{
					do
					{
						CDocument* newDoc = pDocTemplate->GetNextDoc(posDoc);
						if ( newDoc )
						{
							SVIPDoc* pTmpDoc = dynamic_cast <SVIPDoc*> (newDoc);
							if ( pTmpDoc != NULL )
							{
								pTmpDoc->RunOnce();
							}
						}
					}
					while( posDoc );
				}
			}
		}
		while( pos );
	}
}

void SVObserverApp::SetAllIPDocumentsOnline()
{
	//get list of IPDoc's.

	POSITION pos = GetFirstDocTemplatePosition();
	if( pos )
	{
		do
		{
			CDocTemplate* pDocTemplate = GetNextDocTemplate( pos );
			if( pDocTemplate )
			{
				POSITION posDoc = pDocTemplate->GetFirstDocPosition();
				if ( posDoc )
				{
					do
					{
						CDocument* newDoc = pDocTemplate->GetNextDoc(posDoc);
						if ( newDoc )
						{
							SVIPDoc* pTmpDoc = dynamic_cast <SVIPDoc*> (newDoc);

							if( pTmpDoc != NULL )
							{
								pTmpDoc && pTmpDoc->GoOnline();
							}
						}
					}
					while( posDoc );
				}
			}
		}
		while( pos );
	}
}

void SVObserverApp::SetAllIPDocumentsOffline()
{
	//get list of IPDoc's.

	POSITION pos = GetFirstDocTemplatePosition();
	if( pos )
	{
		do
		{
			CDocTemplate* pDocTemplate = GetNextDocTemplate( pos );
			if( pDocTemplate )
			{
				POSITION posDoc = pDocTemplate->GetFirstDocPosition();
				if ( posDoc )
				{
					do
					{
						CDocument* newDoc = pDocTemplate->GetNextDoc(posDoc);
						if ( newDoc )
						{
							SVIPDoc* pTmpDoc = dynamic_cast <SVIPDoc*> (newDoc);

							if( pTmpDoc != NULL )
							{
								pTmpDoc && pTmpDoc->GoOffline();
							}
						}
					}
					while( posDoc );
				}
			}
		}
		while( pos );
	}
}

void SVObserverApp::ResetAllIPDocModifyFlag(BOOL bModified)
{
	POSITION pos = GetFirstDocTemplatePosition();
	if( pos )
	{
		do
		{
			CDocTemplate* pDocTemplate = GetNextDocTemplate( pos );
			if( pDocTemplate )
			{
				POSITION posDoc = pDocTemplate->GetFirstDocPosition();
				if ( posDoc )
				{
					do
					{
						CDocument* newDoc = pDocTemplate->GetNextDoc(posDoc);
						if ( newDoc )
						{
							SVIPDoc* pTmpDoc = dynamic_cast <SVIPDoc*> (newDoc);

							if( pTmpDoc != NULL )
							{
								pTmpDoc->SetModifiedFlag(bModified);
							}
						}
					}
					while( posDoc );
				}
			}
		}
		while( pos );
	}
	//also set the IODoc Modified flag
	SVIODoc* pIODoc = GetIODoc();
	if (pIODoc)
	{
		pIODoc->SetModifiedFlag(bModified);
	}
}

bool SVObserverApp::SetActiveIOTabView( SVTabbedViewSplitterIDEnum p_eTabbedID )
{
	bool l_bRet = false;

	SVIODoc* l_pIODoc = GetIODoc();

	if( l_pIODoc != NULL )
	{
		// Get the active MDI child window.             
		POSITION pos = l_pIODoc->GetFirstViewPosition();
		CView *pView = l_pIODoc->GetNextView( pos );

		CMDIChildWnd *pFrame = (CMDIChildWnd*) pView->GetParentFrame();
		SVIOTabbedView* pIOView = dynamic_cast<SVIOTabbedView*>( pFrame );
		if( pIOView )
		{
			TVisualObject* l_VisObj = pIOView->m_Framework.Get(p_eTabbedID);
			pIOView->m_Framework.SetActiveTab( l_VisObj );

			l_bRet = true;
		}
	}// end if( PIODoc
	return l_bRet;
}

void SVObserverApp::HideRemoteOutputTab()
{
	SVMainFrame* pWndMain = ( SVMainFrame* )GetMainWnd();
	SetActiveIOTabView( SVIODiscreteInputsViewID );

	pWndMain->PostMessage( SV_IOVIEW_HIDE_TAB, SVRemoteOutputsViewID );
}

void SVObserverApp::HideRemoteMonitorListTab()
{
	SVMainFrame* pWndMain = ( SVMainFrame* )GetMainWnd();
	SetActiveIOTabView( SVIODiscreteInputsViewID );

	pWndMain->PostMessage( SV_IOVIEW_HIDE_TAB, SVRemoteMonitorListViewID );
}

void SVObserverApp::HideIOTab( DWORD p_dwID )
{
	SVIODoc* l_pIODoc = GetIODoc();

	if( l_pIODoc != NULL )
	{
		POSITION lViewPos = l_pIODoc->GetFirstViewPosition();
		do 
		{
			CView* pView = l_pIODoc->GetNextView( lViewPos );

			CMDIChildWnd *pFrame = (CMDIChildWnd*) pView->GetParentFrame();
			SVIOTabbedView* pIOView = dynamic_cast<SVIOTabbedView*>( pFrame );
			if( pIOView )
			{
				TVisualObject* l_VisObj = pIOView->m_Framework.Get( p_dwID );
				l_VisObj->ShowTab( false );
				break;
			}
		}
		while ( lViewPos );
	}
}

void SVObserverApp::ShowIOTab( DWORD p_dwID )
{
	SVIODoc* l_pIODoc = GetIODoc();

	if( l_pIODoc != NULL )
	{
		POSITION lViewPos = l_pIODoc->GetFirstViewPosition();
		do 
		{
			CView* pView = l_pIODoc->GetNextView( lViewPos );

			CMDIChildWnd *pFrame = (CMDIChildWnd*) pView->GetParentFrame();
			SVIOTabbedView* pIOView = dynamic_cast<SVIOTabbedView*>( pFrame );
			if( pIOView )
			{
				TVisualObject* l_VisObj = pIOView->m_Framework.Get( p_dwID );
				if( l_VisObj )
				{
					l_VisObj->ShowTab( true );

				}
				break;
			}
		}
		while ( lViewPos );
	}
}

void SVObserverApp::UpdateRemoteInputTabs()
{
	// Remote Inputs
	bool l_bHideIOTab = true;
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig )
	{
		SVInputObjectList* pList = pConfig->GetInputObjectList( );
		if( nullptr != pList )
		{
			long lSize;
			pList->GetRemoteInputCount( lSize );
			if( lSize > 0 )
			{
				l_bHideIOTab = false;
			}
		}
	}

	if( l_bHideIOTab )
	{
		HideIOTab( SVIORemoteInputsViewID );
	}
}

HRESULT SVObserverApp::CheckDrive(const CString& p_strDrive) const
{
	HRESULT l_hr = S_OK;
	// Check if exists
	if( PathFileExists(p_strDrive) == FALSE )
	{
		CString l_strDrive = p_strDrive;
		l_strDrive.MakeUpper();

		SVStringArray msgList;
		msgList.push_back(SVString(l_strDrive.Left(1)));

		SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
		Exception.setMessage( SVMSG_SVO_5051_DRIVEDOESNOTEXIST, SvOi::Tid_Default, msgList, StdMessageParams );
	}
	TCHAR szVolumeName[100];
	TCHAR szFileSystemName[32];
	DWORD dwSerialNumber;
	DWORD dwMaxFileNameLength;
	DWORD dwFileSystemFlags;

	if( GetVolumeInformation( p_strDrive,
		szVolumeName,
		sizeof( szVolumeName),
		&dwSerialNumber,
		&dwMaxFileNameLength,
		&dwFileSystemFlags,
		szFileSystemName,
		sizeof(szFileSystemName)))
	{
		CString l_strName(szFileSystemName);
		if( l_strName.Find(_T("NTFS")) < 0)
		{
			CString l_strDrive;
			l_strDrive = p_strDrive;
			l_strDrive.MakeUpper();
			l_strDrive = l_strDrive.Left(1);

			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_5052_DRIVENOTNTFSFORMAT, l_strDrive, StdMessageParams );
		
#ifndef _DEBUG
			ASSERT( false ); //l_strDrive );
#else
			::OutputDebugString(l_strDrive);
#endif
		}
	}

	return l_hr;
}
#pragma endregion Public Methods

#pragma region Protected Methods
HRESULT SVObserverApp::Start()
{
	HRESULT l_hrOk = S_OK;

	SVSVIMStateClass::RemoveState( SV_STATE_EDIT );

	if ( SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_STARTING ) )
	{
		if ( SVSVIMStateClass::CheckState( SV_STATE_START_PENDING ) )
		{
			SVSVIMStateClass::RemoveState( SV_STATE_START_PENDING );
		}

		return S_OK;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_LOADING | SV_STATE_STOPING ) )
	{
		SVSVIMStateClass::AddState( SV_STATE_START_PENDING );

		return S_OK;
	}

	UpdateAndGetLogDataManager();

	SVClock::SVTimeStamp l_StartLoading;
	SVClock::SVTimeStamp l_FinishLoad;

	l_StartLoading = SVClock::GetTimeStamp();

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	ASSERT( nullptr != pConfig );


	if ( m_pMainWnd && nullptr != pConfig)
	{
		SVPPQObject* pPPQ( nullptr );
		CStringArray saCameras;

		SVDigitizerProcessingClass::Instance().StoreLastCameraImage();

		DisconnectCameras( saCameras );
		ConnectCameras( saCameras );
		l_hrOk = SendCameraParameters( saCameras );

		SVSoftwareTriggerDlg & l_trgrDlg = SVSoftwareTriggerDlg::Instance();
		l_trgrDlg.ShowWindow(SW_HIDE);
		l_trgrDlg.ClearTriggers();

		PPQMonitorList ppqMonitorList;
		pConfig->BuildPPQMonitorList(ppqMonitorList);

		long lSize = pConfig->GetPPQCount(  );
		for( long l = 0; l_hrOk == S_OK && l < lSize; l++ )
		{
			pPPQ =  pConfig->GetPPQ( l );
			//Returns true when pointer valid
			if( nullptr != pPPQ )
			{
				pPPQ->SetMonitorList(ppqMonitorList[pPPQ->GetName()]);

				l_hrOk = pPPQ->CanGoOnline();

				SVTriggerObject* pTrigger( nullptr );
				pPPQ->GetTrigger(pTrigger);
				if ( nullptr != pTrigger && pTrigger->IsSoftwareTrigger())
				{
					l_trgrDlg.AddTrigger( pTrigger );
				}
			}
		}// end for
		if (l_trgrDlg.HasTriggers())
		{
			l_trgrDlg.SelectTrigger();
		}
		if( l_hrOk != S_OK )
		{
			SVSVIMStateClass::RemoveState( SV_STATE_START_PENDING );

			RunAllIPDocuments();

			return l_hrOk; // JMS ERROR - Cannot send PPQ on-line.
		}// end if

		if( SVSVIMStateClass::CheckState( SV_STATE_READY ) )
		{
			for( long l = 0; l_hrOk == S_OK && l < lSize; l++ )
			{
				pPPQ = pConfig->GetPPQ( l );
				if( nullptr != pPPQ )
				{
					l_hrOk = pPPQ->GoOnline();
				}
			}// end for

			if( l_hrOk != S_OK )
			{
				for( long l = 0; l < lSize; l++ )
				{
					pPPQ = pConfig->GetPPQ( l );
					if( nullptr != pPPQ )
					{
						pPPQ->GoOffline();
					}
				}// end for

				SVSVIMStateClass::RemoveState( SV_STATE_START_PENDING );

				RunAllIPDocuments();
				SetAllIPDocumentsOffline();

				return l_hrOk;
			}// end if

			SetAllIPDocumentsOnline();

			SVSVIMStateClass::AddState( SV_STATE_UNAVAILABLE | SV_STATE_STARTING );
			SVSVIMStateClass::RemoveState( SV_STATE_READY | SV_STATE_START_PENDING );

			GetMainFrame()->ShowToolBars( FALSE, TRUE, SVMainFrame::Flag_Standard | SVMainFrame::Flag_Zoom );
			GetMainFrame()->RefreshAllSplitters();

			SetPriorityClass( GetCurrentProcess(), REALTIME_PRIORITY_CLASS );

			SVSVIMStateClass::AddState( SV_STATE_RUNNING );

			SVCommandStreamManager::Instance().RebuildCommandObserver();
			m_mgrRemoteFonts.GoOnline();

			if( IsProductTypeRAID() )
			{
				m_IntelRAID.UpdateStatus();

				if ( SVSVIMStateClass::CheckState( SV_STATE_RAID_FAILURE ) )
				{
					if( pConfig->SetRaidErrorBit( true ) != S_OK )
					{
						return S_FALSE;
					}
				}
				else
				{
					if( pConfig->SetRaidErrorBit( false ) != S_OK )
					{
						return S_FALSE;
					}
				}
			}
			else
			{
				if( pConfig->SetRaidErrorBit( true ) != S_OK )
				{
					return S_FALSE;
				}
			}

			if( pConfig->SetModuleReady( true ) != S_OK )
			{
				RunAllIPDocuments();

				return S_FALSE; // JMS ERROR - Cannot write to module ready output.
			}// end if

			SVObjectManagerClass::Instance().SetState( SVObjectManagerClass::ReadOnly );

			CString l_strTrigCnts;
			GetTriggersAndCounts( l_strTrigCnts );
			l_FinishLoad = SVClock::GetTimeStamp();
			long l_lTime = static_cast<long>(l_FinishLoad - l_StartLoading);

			SVStringArray msgList;
			msgList.push_back(SVString(l_strTrigCnts));
			msgList.push_back(SvUl_SF::Format(_T("%d"), l_lTime));

			//add go-online message to the event viewer.
			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_27_SVOBSERVER_GO_ONLINE, SvOi::Tid_GoOnlineTime, msgList, StdMessageParams );
			
			SVSVIMStateClass::RemoveState( SV_STATE_UNAVAILABLE | SV_STATE_STARTING );
			
			if (l_trgrDlg.HasTriggers())
			{
				EnableTriggerSettings();
			}
			else
			{
				DisableTriggerSettings();
			}

			SetThreadPriority( THREAD_PRIORITY_BELOW_NORMAL );
		}// end if
	}// end if
	else
	{
		RunAllIPDocuments();

		l_hrOk = S_FALSE;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_STOP_PENDING ) )
	{
		PostMessage( m_pMainWnd->m_hWnd, WM_COMMAND, MAKEWPARAM (ID_STOP, 0), 0);
	}

	return l_hrOk;
}

HRESULT SVObserverApp::INILoad()
{
	SVOIniLoader l_iniLoader;

	TCHAR l_szSystemDir[ MAX_PATH + 1 ];
	CString l_csSystemDir;

	::GetSystemDirectory( l_szSystemDir, MAX_PATH + 1 );
	l_csSystemDir.Format( "%s\\OEMINFO.INI", l_szSystemDir );

	// clear these variables
	m_csProductName.Empty();

	m_csProcessorBoardName = _T( "Unknown board" );
	m_csTriggerBoardName = _T( "Unknown board" );
	m_csAcquisitionBoardName = _T( "Unknown board" );
	m_csDigitalBoardName = _T( "Unknown board" );
	m_csRAIDBoardName = _T( "Unknown board" );

	m_csProcessor.Empty();
	m_csFrameGrabber.Empty();
	m_csIOBoard.Empty();
	m_csOptions.Empty();

	// load the SVIM.ini, OEMINFO.ini, and HARDWARE.ini
	HRESULT l_hrOk = l_iniLoader.Load("C:\\SVObserver\\bin\\SVIM.INI", l_csSystemDir, "C:\\SVObserver\\bin\\HARDWARE.INI");

	if (S_OK == l_hrOk)
	{
		// copy settings from the SVOIniLoader class for now
		m_forcedImageUpdateTimeInSeconds = l_iniLoader.GetForcedImageUpdateTime();

		g_bUseCorrectListRecursion = l_iniLoader.m_bUseCorrectListRecursion;

		RootObject::setRootChildValue( ::EnvironmentModelNumber, l_iniLoader.m_csModelNumber );
		RootObject::setRootChildValue( ::EnvironmentSerialNumber , l_iniLoader.m_csSerialNumber );
		RootObject::setRootChildValue( ::EnvironmentWinKey, l_iniLoader.m_csWinKey );

		m_csProductName = l_iniLoader.m_csProductName;

		m_csProcessorBoardName = l_iniLoader.m_csProcessorBoardName;
		m_csTriggerBoardName = l_iniLoader.m_csTriggerBoardName;
		m_csAcquisitionBoardName = l_iniLoader.m_csAcquisitionBoardName;
		m_csDigitalBoardName = l_iniLoader.m_csDigitalBoardName;
		m_csRAIDBoardName = l_iniLoader.m_csRAIDBoardName;

		m_csDigitalDLL = l_iniLoader.m_csDigitalDLL;
		m_csDigitalOption = l_iniLoader.m_csIOBoardOption;
		m_csDigitizerDLL = l_iniLoader.m_csDigitizerDLL;
		m_csFileAcquisitionDLL = l_iniLoader.m_csFileAcquisitionDLL;
		m_csTriggerDLL = l_iniLoader.m_csTriggerDLL;
		m_csSoftwareTriggerDLL = l_iniLoader.m_csSoftwareTriggerDLL;
		m_csAcquisitionTriggerDLL = l_iniLoader.m_csAcquisitionTriggerDLL;

		m_csReloadDigitalDLL = l_iniLoader.m_csReloadDigitalDLL;
		m_csReloadAcquisitionDLL = l_iniLoader.m_csReloadAcquisitionDLL;
		m_csReloadTriggerDLL = l_iniLoader.m_csReloadTriggerDLL;

		m_csOptions = l_iniLoader.m_csOptions;
		m_csProcessor = l_iniLoader.m_csProcessor;
		m_csFrameGrabber = l_iniLoader.m_csFrameGrabber;
		m_csIOBoard = l_iniLoader.m_csIOBoard;

		m_csTrigger = l_iniLoader.m_csTrigger;

		// Get GIGE packet Size
		m_gigePacketSize = l_iniLoader.m_gigePacketSize;

		for ( int i = 0; i < 4; i++ )
		{
			SVIOConfigurationInterfaceClass::Instance().SetSVIMTriggerValue( i, 0 == l_iniLoader.m_csTriggerEdge[i].CompareNoCase( "R" ) );
			SVIOConfigurationInterfaceClass::Instance().SetSVIMStrobeValue( i, 0 == l_iniLoader.m_csStrobeEdge[i].CompareNoCase( "R" ) );
			SVIOConfigurationInterfaceClass::Instance().SetSVIMStrobeStartFrameActive( i, 0 == l_iniLoader.m_csStartFrameType[i].CompareNoCase( "Y" ) );
		}

		l_hrOk = l_hrOk | LoadDigitalDLL();
		l_hrOk = l_hrOk | LoadTriggerDLL();
		l_hrOk = l_hrOk | LoadSoftwareTriggerDLL();
		l_hrOk = l_hrOk | LoadAcquisitionTriggerDLL();
		l_hrOk = l_hrOk | LoadAcquisitionDLL();
		l_hrOk = l_hrOk | LoadFileAcquisitionDLL();
	}
	else
	{
		if (S_OK != l_iniLoader.m_hrOEMFailure)
		{
			ASSERT( FALSE );
			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_SVObserver_ModelNumberInvalid, StdMessageParams, SvOi::Err_10236 );
		}

	}
	m_hrHardwareFailure = l_hrOk;
	return l_hrOk;
}

HRESULT SVObserverApp::INIClose()
{
	m_hrHardwareFailure = SV_HARDWARE_FAILURE_ALL;

	m_csProductName.Empty();

	m_csProcessor.Empty();
	m_csFrameGrabber.Empty();
	m_csIOBoard.Empty();
	m_csOptions.Empty();

	m_csProcessorBoardName = _T( "Unknown board" );
	m_csTriggerBoardName = _T( "Unknown board" );
	m_csAcquisitionBoardName = _T( "Unknown board" );
	m_csDigitalBoardName = _T( "Unknown board" );
	m_csRAIDBoardName = _T( "Unknown board" );

	CloseAcquisitionDLL();

	CloseTriggerDLL();

	CloseDigitalDLL();

	return S_OK;
}

HRESULT SVObserverApp::INIReset()
{
	HRESULT l_hrOk = S_OK;

	if( m_csReloadAcquisitionDLL.CompareNoCase( _T( "Y" ) ) == 0 )
	{
		CloseAcquisitionDLL();
	}

	if( m_csReloadTriggerDLL.CompareNoCase( _T( "Y" ) ) == 0 )
	{
		CloseTriggerDLL();
	}

	if( m_csReloadDigitalDLL.CompareNoCase( _T( "Y" ) ) == 0 )
	{
		CloseDigitalDLL();
	}

	if( m_csReloadDigitalDLL.CompareNoCase( _T( "Y" ) ) == 0 )
	{
		l_hrOk = l_hrOk | LoadDigitalDLL();
	}

	if( m_csReloadTriggerDLL.CompareNoCase( _T( "Y" ) ) == 0 )
	{
		l_hrOk = l_hrOk | LoadTriggerDLL();
		l_hrOk = l_hrOk | LoadSoftwareTriggerDLL();
		l_hrOk = l_hrOk | LoadAcquisitionTriggerDLL();
	}

	if( m_csReloadAcquisitionDLL.CompareNoCase( _T( "Y" ) ) == 0 )
	{
		l_hrOk = l_hrOk | LoadAcquisitionDLL();
		l_hrOk = l_hrOk | LoadFileAcquisitionDLL();
	}

	return l_hrOk;
}

HRESULT SVObserverApp::LoadTriggerDLL()
{
	HRESULT l_hrOk = S_OK ;

	if ( ! m_csTriggerDLL.IsEmpty() )
	{
		VARIANT l_varValue;

		::VariantInit( &l_varValue );

		l_varValue.vt = VT_I4;

		if ( m_svDLLTriggers.Open( m_csTriggerDLL ) != S_OK )
		{
			l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_TRIGGER;
		}

		if ( SVTriggerProcessingClass::Instance().UpdateTriggerSubsystem( &m_svDLLTriggers ) != S_OK )
		{
			l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_TRIGGER;
		}

		for ( int i = 0; i < 4; i++ )
		{
			unsigned long l_ulHandle;

			if ( m_svDLLTriggers.GetHandle( &l_ulHandle, i ) == S_OK )
			{
				bool l_bRising;

				SVIOConfigurationInterfaceClass::Instance().GetIOTriggerValue( i, l_bRising );

				if ( l_bRising )
				{
					l_varValue.lVal = 1;
				}
				else
				{
					l_varValue.lVal = -1;
				}
				// SVSignalEdge enum is used here to make the code more clear.
				// however at some time in the future the Dll parameters may be implemented
				// as an array and therefore this enum may not apply.
				m_svDLLTriggers.SetParameterValue( l_ulHandle, SVSignalEdge, &l_varValue );
			}
		}
	}
	else
	{
		if( m_csTrigger != _T( "00" ) )
		{
			l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_TRIGGER;
		}
	}

	return l_hrOk;
}

HRESULT SVObserverApp::CloseTriggerDLL()
{
	SVTriggerProcessingClass::Instance().clear();

	m_svDLLTriggers.Close();
	m_svDLLSoftwareTriggers.Close();
	m_svDLLAcquisitionTriggers.Close();

	return S_OK;
}

HRESULT SVObserverApp::LoadSoftwareTriggerDLL()
{
	HRESULT l_hrOk = S_OK;

	if ( ! m_csSoftwareTriggerDLL.IsEmpty() )
	{
		if ( m_svDLLSoftwareTriggers.Open( m_csSoftwareTriggerDLL ) != S_OK )
		{
			l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_SOFTWARETRIGGER;
		}

		if ( SVTriggerProcessingClass::Instance().UpdateTriggerSubsystem( &m_svDLLSoftwareTriggers ) != S_OK )
		{
			l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_SOFTWARETRIGGER;
		}
	}
	return l_hrOk;
}

HRESULT SVObserverApp::LoadAcquisitionTriggerDLL()
{
	HRESULT l_hrOk = S_OK;

	if ( ! m_csAcquisitionTriggerDLL.IsEmpty() )
	{
		HRESULT ResultLoadDLL( m_svDLLAcquisitionTriggers.Open( m_csAcquisitionTriggerDLL ) );
		//Do not care about the Matrox service here as it will be handled in the LoadAcquisitionDLL
		if( S_OK != ResultLoadDLL && ErrorMatroxServiceNotRunning != ResultLoadDLL )
		{
			l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_CAMERATRIGGER;
		}

		if( S_OK == ResultLoadDLL )
		{
			if( SVTriggerProcessingClass::Instance().UpdateTriggerSubsystem( &m_svDLLAcquisitionTriggers ) != S_OK )
			{
				l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_CAMERATRIGGER;
			}
		}
	}
	return l_hrOk;
}

//*
HRESULT SVObserverApp::LoadAcquisitionDLL()
{
	HRESULT l_hrOk = S_OK;

	if( ! m_csDigitizerDLL.IsEmpty() )
	{
		HRESULT ResultLoadDLL( m_svDLLDigitizers.Open( m_csDigitizerDLL ) );
		if( S_OK != ResultLoadDLL )
		{
			//This is the error result which indicates that the Matrox Gige service is not running
			if( ErrorMatroxServiceNotRunning == ResultLoadDLL )
			{
				SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
				Exception.setMessage( SVMSG_SVO_90_MATROX_SERVICE_NOT_RUNNING, m_csDigitizerDLL, StdMessageParams, SvOi::Err_25048_NoMatroxService );
			}
			else
			{
				l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_ACQUISITION;
			}
		}
		else
		{
			if( SVDigitizerProcessingClass::Instance().UpdateDigitizerSubsystem( &m_svDLLDigitizers ) != S_OK )
			{
				l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_ACQUISITION;
			}
		}
	}
	else
	{
		if( m_csFrameGrabber != _T( "00" ) )
		{
			l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_ACQUISITION;
		}
	}

	return l_hrOk;
}

HRESULT SVObserverApp::CloseAcquisitionDLL()
{
	SVDigitizerProcessingClass::Instance().clear();

	m_svDLLDigitizers.Close();

	m_svDLLFileAcquisition.Close();

	return S_OK;
}

HRESULT SVObserverApp::LoadFileAcquisitionDLL()
{
	HRESULT l_hrOk = S_OK;

	if( ! m_csFileAcquisitionDLL.IsEmpty() )
	{
		if( m_svDLLFileAcquisition.Open( m_csFileAcquisitionDLL ) != S_OK )
		{
			l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_FILEACQUISITION;
		}

		if( SVDigitizerProcessingClass::Instance().UpdateDigitizerSubsystem( &m_svDLLFileAcquisition ) != S_OK )
		{
			l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_FILEACQUISITION;
		}
	}
	return l_hrOk;
}

HRESULT SVObserverApp::LoadDigitalDLL()
{
	HRESULT l_hrOk = S_OK;

	if ( ! m_csDigitalDLL.IsEmpty() )
	{
		if ( SVIOConfigurationInterfaceClass::Instance().OpenDigital( m_csDigitalDLL ) != S_OK )
		{
			l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_IO;
		}
		else
		{	// Send the System type to the IO Board.
			VARIANT l_vt;
			l_vt.vt = VT_I4;

			// Hardware.ini has the new IOBoardOption.
			if( !m_csDigitalOption.IsEmpty() )
			{
				l_vt.lVal = atol( m_csDigitalOption );
				SVIOConfigurationInterfaceClass::Instance().SetParameterValue( SVBoardType, &l_vt );
			}
			else
			{	// Legacy behavior.... Hardware.Ini file does not have new entry...
				l_vt.lVal = atol( m_csIOBoard );
				SVIOConfigurationInterfaceClass::Instance().SetParameterValue( SVBoardType, &l_vt );
			}
		}
	}
	else
	{
		if( m_csIOBoard != _T( "00" ) )
		{
			l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_IO;
		}
	}

	return l_hrOk;
}

HRESULT SVObserverApp::CloseDigitalDLL()
{
	SVIOConfigurationInterfaceClass::Instance().CloseDigital();

	return S_OK;
}

LPCTSTR SVObserverApp::GetProcessorBoardName() const
{
	return m_csProcessorBoardName;
}

LPCTSTR SVObserverApp::GetTriggerBoardName() const
{
	return m_csTriggerBoardName;
}

LPCTSTR SVObserverApp::GetSoftwareTriggerBoardName() const
{
	return m_csSoftwareTriggerDLL;
}

LPCTSTR SVObserverApp::GetAcquisitionBoardName() const
{
	return m_csAcquisitionBoardName;
}

LPCTSTR SVObserverApp::GetFileAcquisitionBoardName() const
{
	return m_csFileAcquisitionBoardName;
}

LPCTSTR SVObserverApp::GetDigitalBoardName() const
{
	return m_csDigitalBoardName;
}

LPCTSTR SVObserverApp::GetRAIDBoardName() const
{
	return m_csRAIDBoardName;
}

HRESULT SVObserverApp::DisconnectAllCameraBuffers( CStringArray& rDisconnectedCameras )
{
	HRESULT hr = DisconnectToolsetBuffers();

	if( hr == S_OK )
	{
		HRESULT l_Temp = SVDigitizerProcessingClass::Instance().GetAcquisitionDeviceList( rDisconnectedCameras );

		hr = SVDigitizerProcessingClass::Instance().DestroyBuffers();

		if( l_Temp != S_OK )
		{
			hr = l_Temp;
		}
	}

	return hr;
}

HRESULT SVObserverApp::ConnectCameraBuffers( const CStringArray& rCamerasToConnect )
{
	HRESULT hr = S_OK;

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig )
	{
		for ( int i=0; i < rCamerasToConnect.GetSize(); i++)
		{
			CString sDigName = rCamerasToConnect.GetAt( i );
			SVAcquisitionClassPtr pAcqDevice;

			SVDigitizerProcessingClass::Instance().SelectDigitizer( sDigName );
			pAcqDevice = SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( sDigName );
			if ( nullptr != pAcqDevice )
			{
				// dummy vars
				SVLightReference* pLR;
				SVFileNameArrayClass* pFiles;
				SVLut* pLut;
				SVDeviceParamCollection* pParams;

				if ( pConfig->GetAcquisitionDevice( sDigName, pFiles, pLR, pLut, pParams ) )
				{
					SVImageInfoClass svImageInfo;
					pAcqDevice->GetImageInfo(&svImageInfo);
					hr = pAcqDevice->CreateBuffers( svImageInfo, TheSVObserverApp.GetSourceImageDepth() );
				}
			}
		}

		hr = ConnectToolsetBuffers();
	}

	return hr;
}

HRESULT SVObserverApp::InitializeSecurity() 
{
	HMODULE hMessageDll;

	if (hMessageDll = LoadLibraryEx (_T("SVMessage.dll"), NULL, LOAD_LIBRARY_AS_DATAFILE))
	{
		// This sleep(0) was added after the LoadLibrary to fix a bug where the system ran out of resources.
		Sleep(0);
		// Add function with two parameters does not have Data.
		// Add Function with more than two parameters are data nodes.
		// These are the Security defaults if the file fails to load.
		AddSecurityNode(hMessageDll, SECURITY_POINT_FILE_MENU ); // First Position in Array Not used because position 0 is not valid in the XML Security Wrapper.

		AddSecurityNode(hMessageDll, SECURITY_POINT_ROOT );
		AddSecurityNode(hMessageDll, SECURITY_POINT_FILE_MENU );     // First Child off Root
		AddSecurityNode(hMessageDll, SECURITY_POINT_FILE_MENU_NEW                   , _T("") );
		AddSecurityNode(hMessageDll, SECURITY_POINT_FILE_MENU_SELECT_CONFIGURATION  , _T("") );
		AddSecurityNode(hMessageDll, SECURITY_POINT_FILE_MENU_CLOSE_CONFIGURATION   , _T("") );
		AddSecurityNode(hMessageDll, SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION_AS , _T("") );
		AddSecurityNode(hMessageDll, SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION    , _T("") );
		AddSecurityNode(hMessageDll, SECURITY_POINT_FILE_MENU_PRINT                 , _T("") );
		AddSecurityNode(hMessageDll, SECURITY_POINT_FILE_MENU_PRINT_SETUP           , _T("") );
		AddSecurityNode(hMessageDll, SECURITY_POINT_FILE_MENU_SAVE_IMAGE            , _T("") );
		AddSecurityNode(hMessageDll, SECURITY_POINT_FILE_MENU_RECENT_CONFIGURATIONS , _T("") );
		AddSecurityNode(hMessageDll, SECURITY_POINT_FILE_MENU_EXIT                  , _T("") );
		AddSecurityNode(hMessageDll, SECURITY_POINT_UNRESTRICTED_FILE_ACCESS        , _T("") );

		AddSecurityNode(hMessageDll, SECURITY_POINT_EDIT_MENU );     // Second Child From Root
		AddSecurityNode(hMessageDll, SECURITY_POINT_EDIT_MENU_RESULT_PICKER               , _T("") );

		AddSecurityNode(hMessageDll, SECURITY_POINT_VIEW_MENU );     // Third Child From Root
		AddSecurityNode(hMessageDll, SECURITY_POINT_VIEW_MENU_PPQ_BAR               , _T("") );
		AddSecurityNode(hMessageDll, SECURITY_POINT_VIEW_MENU_IMAGE_DISPLAY_UPDATE  , _T("") );
		AddSecurityNode(hMessageDll, SECURITY_POINT_VIEW_MENU_RESULT_DISPLAY_UPDATE , _T("") );
		AddSecurityNode(hMessageDll, SECURITY_POINT_VIEW_MENU_RESET_COUNTS_CURRENT  , _T("") );
		AddSecurityNode(hMessageDll, SECURITY_POINT_VIEW_MENU_RESET_COUNTS_ALL      , _T("") );

		AddSecurityNode(hMessageDll, SECURITY_POINT_MODE_MENU );     // forth Child From Root
		AddSecurityNode(hMessageDll, SECURITY_POINT_MODE_MENU_RUN,              _T("") );
		AddSecurityNode(hMessageDll, SECURITY_POINT_MODE_MENU_STOP,             _T("") );
		AddSecurityNode(hMessageDll, SECURITY_POINT_MODE_MENU_REGRESSION_TEST,  _T("") );
		AddSecurityNode(hMessageDll, SECURITY_POINT_MODE_MENU_TEST,             _T("") );
		AddSecurityNode(hMessageDll, SECURITY_POINT_MODE_MENU_EDIT_TOOLSET,     _T("") );
		AddSecurityNode(hMessageDll, SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE,    _T("") );

		AddSecurityNode(hMessageDll, SECURITY_POINT_EXTRAS_MENU );   // fifth Child From Root
		AddSecurityNode(hMessageDll, SECURITY_POINT_EXTRAS_MENU_ADDITIONAL_ENVIRON, _T("") );
		AddSecurityNode(hMessageDll, SECURITY_POINT_EXTRAS_MENU_SECURITY_MANAGER,   _T("Administrators"), true);
		AddSecurityNode(hMessageDll, SECURITY_POINT_EXTRAS_MENU_TEST_OUTPUTS,       _T("") );
		AddSecurityNode(hMessageDll, SECURITY_POINT_EXTRAS_MENU_UTILITIES_SETUP,    _T("") );
		AddSecurityNode(hMessageDll, SECURITY_POINT_EXTRAS_MENU_UTILITIES_RUN,      _T("") );
		AddSecurityNode(hMessageDll, SECURITY_POINT_EXTRAS_MENU_AUTOSAVE_CONFIGURATION, _T("") );
		AddSecurityNode(hMessageDll, SECURITY_POINT_EXTRAS_MENU_FBWF_CONFIGURATION, _T("") );
		m_svSecurityMgr.SVProtectData( SECURITY_POINT_EXTRAS_MENU_SECURITY_MANAGER ); // Sets Flag that will prevent data from being changed.
	}

	char szGetBuf[256];
	GetPrivateProfileString(_T("Security"), _T("Security File Path"), _T("c:\\SVObserver\\bin\\Gatekpr.xml"), szGetBuf, 256, "c:\\SVObserver\\Bin\\SVIM.ini");

	if( m_svSecurityMgr.SVLoad(szGetBuf) != S_OK )
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_SVObserver_SecurityFileLoadFailed, StdMessageParams, SvOi::Err_10133 );
	}

	FreeLibrary (hMessageDll);
	// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
	Sleep(0);

	return S_OK;
}

void SVObserverApp::fileSaveAsSVX( CString StrSaveAsPathName, bool isAutoSave) 
{
	SVFileNameManagerClass svFileManager;
	CWaitCursor wait;

	BOOL bOk=TRUE;

	SVSVIMStateClass::AddState( SV_STATE_SAVING );
	ResetAllIPDocModifyFlag(FALSE);
	//
	// Is the input parameter for save as path empty?
	//
	if ( !isAutoSave && StrSaveAsPathName.IsEmpty() )
	{
		SVFileNameClass svFileName = m_ConfigFileName;

		svFileName.SetFileType( SV_SVX_CONFIGURATION_FILE_TYPE );

		if ( CString( getConfigPathName() ).IsEmpty() ||
			CString( getConfigPathName() ).CompareNoCase( Seidenader::SVObserver::RunFolder ) == 0 )
		{
			svFileName.SetPathName( AfxGetApp()->GetProfileString( _T( "Settings" ), 
				_T( "ConfigurationFilePath" ), 
				_T( Seidenader::SVObserver::RunFolder ) ) );

			if ( CString( svFileName.GetPathName() ).CompareNoCase( Seidenader::SVObserver::RunFolder ) == 0 )
			{
				if ( ! CString( svFileManager.GetConfigurationPathName() ).IsEmpty() )
				{
					svFileName.SetPathName( svFileManager.GetConfigurationPathName() );
				}
			}
			else
			{
				if (!isAutoSave)
				{
					CString path = svFileName.GetPathName();
					int pos = path.ReverseFind(_T('\\'));
					if (pos != -1)
					{
						path.Delete(pos, path.GetLength() - pos);
						// check if just drive left
						if (path[path.GetLength()-1] == _T(':'))
						{
							path += _T('\\');
						}
						svFileName.SetPathName(path);
					}
				}
			}
		}

		if ( svFileName.SaveFile() )
		{
			if(!isAutoSave) //Arvid in an autosave the configuration name must not be changed
			{
				bOk = setConfigFullFileName( svFileName.GetFullFileName(), RENAME );
				if ( bOk )
				{
					AfxGetApp()->WriteProfileString( _T( "Settings" ), 
						_T( "ConfigurationFilePath" ), 
						svFileName.GetPathName() );
				}
			}
		}
		else
		{
			SVSVIMStateClass::RemoveState( SV_STATE_SAVING ); // remove the state set at the start of this method
			/// Why is the return here? Shouldn't bOk be set to false instead ?
			return; // what no error?
		}
	}// end if ( !isAutoSave && StrSaveAsPathName.IsEmpty() )
	else
	{
		if(!isAutoSave) //Arvid in an AutoSave the full name of the configuration must not be changed!
		{
			bOk = setConfigFullFileName( StrSaveAsPathName, FALSE );
		}
	}

	if ( bOk && !CString( m_ConfigFileName.GetExtension() ).CompareNoCase( ".svx" ) )
	{
		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
		CString csFileName = m_ConfigFileName.GetFullFileName();

		if( nullptr != pConfig )
		{
			pConfig->ValidateRemoteMonitorList(); // sanity check
			std::ofstream os;
			os.open( csFileName );
			if (os.is_open())
			{
				SVObjectXMLWriter writer(os);
				pConfig->SaveConfiguration( writer );
				os.close();
			}
		}

		if (isAutoSave) 
		{
			//Arvid in an autosave some of the of the steps necessary in a normal configuration 
			// save are skipped, 
			// e.g. the configuration name must not be added to the LRU list
			
			ExtrasEngine::Instance().CopyDirectoryToTempDirectory(Seidenader::SVObserver::RunFolder + CString("\\"));
			ExtrasEngine::Instance().ResetAutoSaveInformation(); //Arvid: update autosave timestamp
		}
		else 
		{
			svFileManager.SaveItem( &m_ConfigFileName );//Arvid: here the configuration seems to be copied from the Run directory (C:\Run)
			svFileManager.SaveItems();					//Arvid: here other files required for the configuration seem to be copied from the Run directory (C:\Run)
			svFileManager.RemoveUnusedFiles();

			SVSVIMStateClass::RemoveState( SV_STATE_MODIFIED );

			if ( bOk )
			{
				if ( CString( svFileManager.GetConfigurationPathName() ).IsEmpty() )
				{
					AddToRecentFileList( getConfigFullFileName() );
				}
				else
				{
						AddToRecentFileList( CString( svFileManager.GetConfigurationPathName() ) + 
							"\\" + getConfigFileName() );
				}
				ExtrasEngine::Instance().ResetAutoSaveInformation(); //Arvid: configuration successfully saved: update autosave timestamp
			}

			( (CMDIFrameWnd*) AfxGetMainWnd() )->OnUpdateFrameTitle(TRUE);
			// Success...
		}
	}// end if ( !CString( m_ConfigFileName.GetExtension() ).CompareNoCase( ".svx" ) )
	else
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_SVObserver_WrongPathnameEntered, StdMessageParams, SvOi::Err_10134 );
	}
	SVSVIMStateClass::RemoveState( SV_STATE_SAVING );
}

/////////////////////////////////////////////////////////////////////////////
//
//  An intermediate method called when user selects a configuration 
//  file from the Most Recent Used (MRU) list under menu File.\
//
//
BOOL SVObserverApp::OpenConfigFileFromMostRecentList(int nID)
{
	ASSERT_VALID(this);
	ASSERT(m_pRecentFileList != NULL);

	ASSERT(nID >= ID_FILE_MRU_FILE1);
	ASSERT(nID < ID_FILE_MRU_FILE1 + (int)m_pRecentFileList->GetSize());
	int nIndex = nID - ID_FILE_MRU_FILE1;
	ASSERT((*m_pRecentFileList)[nIndex].GetLength() != 0);

	TRACE2("MRU: open file (%d) '%s'.\n", (nIndex) + 1,
		(LPCTSTR)(*m_pRecentFileList)[nIndex]);

	BOOL bResult = FALSE;

	// Check Security to see if save is allowed

	//if (OpenDocumentFile((*m_pRecentFileList)[nIndex]) == NULL)
	//
	// Open and read the Configuration from the Most Recent Used
	// List under the menu File.
	//
	SVFileNameClass svFileName;

	svFileName.SetFullFileName( (*m_pRecentFileList)[nIndex] );

	bResult = FALSE;

	HRESULT hr = S_FALSE;

	if ( CString( svFileName.GetExtension() ).CollateNoCase( ".svx" ) == 0 )
	{
		hr = OpenSVXFile((*m_pRecentFileList)[nIndex]);
	}

	//if S_OK, check to see if it has any tool errors in the license manager
	if ( hr == S_OK )
	{
		if ( TheSVOLicenseManager().HasToolErrors() )
		{
			TheSVOLicenseManager().ShowLicenseManagerErrors();
		}
	}

	if ( hr == S_FALSE )    // don't do on ERROR_CANCELLED
	{
		m_pRecentFileList->Remove(nIndex);
	}
	bResult = (hr == S_OK);

	return bResult;
}

// get the Document Template for the SVIPDoc
static CMultiDocTemplate* getIPDocTemplate()
{
	CWinApp& rApp = TheSVObserverApp;
	CMultiDocTemplate* pTemplate = nullptr;
	POSITION pos = rApp.GetFirstDocTemplatePosition();
	if (pos)
	{
		CMultiDocTemplate* pDocTemplate = dynamic_cast<CMultiDocTemplate *>(rApp.GetNextDocTemplate(pos));
		while (!pTemplate && pDocTemplate)
		{
			POSITION posDoc = pDocTemplate->GetFirstDocPosition();
			if (posDoc)
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

void SVObserverApp::EnableTriggerSettings()
{
	SVUtilitiesClass util;
	CWnd* pWindow = AfxGetMainWnd();
	if (pWindow)
	{
		CMultiDocTemplate* pDocTemplate = getIPDocTemplate();
		if (pDocTemplate)
		{
			CString szMenuText(_T("&View"));
			CMenu menu;
			menu.Attach(pDocTemplate->m_hMenuShared);
			CMenu* pSubMenu = util.FindSubMenuByName(&menu, szMenuText);
			if (pSubMenu)
			{
				pSubMenu->AppendMenu(MF_STRING, ID_TRIGGER_SETTINGS, _T("Software Trigger")); 
			}
			menu.Detach();
			pWindow->DrawMenuBar();
		}
	}
}

void SVObserverApp::DisableTriggerSettings()
{
	SVSoftwareTriggerDlg::Instance().ShowWindow(SW_HIDE);
	SVUtilitiesClass util;
	CWnd* pWindow = AfxGetMainWnd();
	if (pWindow)
	{
		CMultiDocTemplate* pDocTemplate = getIPDocTemplate();
		if (pDocTemplate)
		{
			CString szMenuText(_T("&View"));
			CMenu menu;
			menu.Attach(pDocTemplate->m_hMenuShared);
			CMenu* pSubMenu = util.FindSubMenuByName(&menu, szMenuText);
			if (pSubMenu)
			{
				pSubMenu->RemoveMenu(ID_TRIGGER_SETTINGS, MF_BYCOMMAND); 
			}
			menu.Detach();
			pWindow->DrawMenuBar();
		}
	}
}

HRESULT SVObserverApp::ConstructDocuments( SVTreeType& p_rTree )
{
	HRESULT l_Status( S_OK );

	SVTreeType::SVBranchHandle htiChild = NULL;

	if( SVNavigateTree::GetItemBranch( p_rTree, CTAG_IO, NULL, htiChild ) )
	{
		SVIOController* pIOController( NULL );

		_variant_t svVariant;

		if( SVNavigateTree::GetItem( p_rTree, CTAG_UNIQUE_REFERENCE_ID, htiChild, svVariant ) )
		{
			SVGUID ObjectID( svVariant );

			pIOController = dynamic_cast< SVIOController* >( SVObjectManagerClass::Instance().GetObject( ObjectID ) );
		}
		else
		{
			SVConfigurationObject* pConfig( nullptr );
			SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

			if( nullptr != pConfig )
			{
				pIOController = pConfig->GetIOController();

				l_Status = S_OK;
			}
			else
			{
				l_Status = E_FAIL;
			}
		}

		if( pIOController != NULL )
		{
			pIOController->RebuildOutputList();

			SVIODoc* l_pIODoc( TheSVObserverApp.NewSVIODoc( pIOController->GetName(), *pIOController ) );

			if( l_pIODoc != NULL )
			{
				l_pIODoc->UpdateAllViews( NULL );
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	if( l_Status == S_OK && SVNavigateTree::GetItemBranch( p_rTree, CTAG_INSPECTION, NULL, htiChild ) )
	{
		SVTreeType::SVBranchHandle htiSubChild( nullptr );

		htiSubChild = p_rTree.getFirstBranch( htiChild );

		while( l_Status == S_OK && nullptr != htiSubChild )
		{
			SVInspectionProcess* pInspection( nullptr );
			SVTreeType::SVBranchHandle htiSVIPDoc = NULL;
			SVTreeType::SVBranchHandle htiSVInspectionProcess = NULL;

			SVNavigateTree::GetItemBranch( p_rTree, CTAG_INSPECTION_PROCESS, htiSubChild, htiSVInspectionProcess );
			SVNavigateTree::GetItemBranch( p_rTree, CTAG_SVIPDOC, htiSubChild, htiSVIPDoc );

			SVGUID docObjectID;

			SVTreeType::SVBranchHandle htiTempItem = htiSVIPDoc;

			if( htiSVInspectionProcess != NULL )
			{
				htiTempItem = htiSVInspectionProcess;
			}

			if( htiTempItem != NULL )
			{
				_variant_t svVariant;

				if( SVNavigateTree::GetItem( p_rTree, CTAG_UNIQUE_REFERENCE_ID, htiTempItem, svVariant ) )
				{
					SVGUID docObjectID = svVariant;

					pInspection = dynamic_cast< SVInspectionProcess* >( SVObjectManagerClass::Instance().GetObject( docObjectID ) );

					if( nullptr == pInspection )
					{
						l_Status = E_FAIL;
					}
				}
				else
				{
					l_Status = E_FAIL;
				}
			}
			else
			{
				l_Status = E_FAIL;
			}

			if( l_Status == S_OK )
			{
				SVIPDoc* pIPDoc( nullptr );
				//If Inspection pointer is nullptr then pIPDoc will also be nullptr
				if( nullptr != pInspection ){ pIPDoc = TheSVObserverApp.NewSVIPDoc( pInspection->GetName(), *pInspection ) ; }

				if( nullptr != pIPDoc )
				{
					pIPDoc->IsNew = false;

					if( ! pIPDoc->SetParameters( p_rTree, htiSVIPDoc ) )
					{
						l_Status = E_FAIL;
					}

					if( l_Status == S_OK )
					{
						// Init Document
						if( ! pIPDoc->InitAfterSystemIsDocked() )
						{
							l_Status = E_FAIL;
						}
					}

					if( l_Status == S_OK )
					{
						// Allow Clients to Connect
						SVInputStreamManager::Instance().Startup( m_InputStreamPortNumber );
						SVOutputStreamManager::Instance().Startup( m_OutputStreamPortNumber );
					}
				}
				else
				{
					l_Status = E_FAIL;
				}
			}

			htiSubChild = p_rTree.getNextBranch( htiChild, htiSubChild );
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVObserverApp::ConstructMissingDocuments()
{
	HRESULT l_Status( S_OK );

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig )
	{
		SVIOController* l_pIOController = pConfig->GetIOController();

		if( l_pIOController != NULL )
		{
			if( SVObjectManagerClass::Instance().GetIODoc( l_pIOController->GetUniqueObjectID() ) == NULL )
			{
				SVIODoc* l_pIODoc( TheSVObserverApp.NewSVIODoc( l_pIOController->GetName(), *l_pIOController ) );

				if( l_pIODoc != NULL )
				{
					l_pIODoc->UpdateAllViews( NULL );
				}
				else
				{
					l_Status = E_FAIL;
				}
			}
		}
		else
		{
			l_Status = E_FAIL;
		}

		SVInspectionProcessPtrList l_Inspections;

		pConfig->GetInspections( l_Inspections );

		SVInspectionProcessPtrList::iterator l_Iter( l_Inspections.begin() );

		while( l_Status == S_OK && l_Iter != l_Inspections.end() )
		{
			SVInspectionProcess* pInspection( *l_Iter );

			if( nullptr != pInspection )
			{
				if( SVObjectManagerClass::Instance().GetIPDoc( pInspection->GetUniqueObjectID() ) == NULL )
				{
					SVIPDoc* l_pIPDoc( TheSVObserverApp.NewSVIPDoc( pInspection->GetName(), *pInspection ) );

					if( l_pIPDoc != NULL )
					{
						l_pIPDoc->mbInitImagesByName = true;

						// Init Document
						if( ! l_pIPDoc->InitAfterSystemIsDocked() )
						{
							l_Status = E_FAIL;
						}
					}
				}
			}
			else
			{
				l_Status = E_FAIL;
			}

			++l_Iter;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	if (l_Status == S_OK)
	{
		SVInputStreamManager::Instance().Startup( m_InputStreamPortNumber );
		SVOutputStreamManager::Instance().Startup( m_OutputStreamPortNumber );
	}
	return l_Status;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : DestroyMessageWindow
// -----------------------------------------------------------------------------
// .Description : Destroys still open message windows
// -----------------------------------------------------------------------------
// .Return Value
//	: BOOL									Returns TRUE, if a message window 
//											was existent. Otherwise it returns
//											FALSE.
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :05.05.1998 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
BOOL SVObserverApp::DestroyMessageWindow()
{
	if( m_pMessageWindow )
	{
		if( ::IsWindow( m_pMessageWindow->m_hWnd ) )
		{
			m_pMessageWindow->DestroyWindow();
		}

		delete m_pMessageWindow;
		m_pMessageWindow = NULL;
		return TRUE;
	}
	return FALSE;
}
#pragma endregion Protected Methods

#pragma region Private Methods
////////////////////////////////////////////////////////////////////////////////
// .Title       : OnStopAll
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
void SVObserverApp::OnStopAll()
{
	if( m_pMainWnd )
	{
		GetMainFrame()->SetStatusInfoText(_T(""));

		if ( SVSVIMStateClass::CheckState( SV_STATE_TEST ) )
		{
			CMenu* pMainMenu = m_pMainWnd->GetMenu();
			if( pMainMenu )
			{
				pMainMenu->ModifyMenu( ID_MODE_STOPTEST, MF_BYCOMMAND | MF_STRING, 
					ID_MODE_TEST, _T( "&Test" ) );
			}

			SVSVIMStateClass::RemoveState( SV_STATE_TEST );
		}

		GetMainFrame()->ShowToolBars( TRUE, TRUE,SVMainFrame::Flag_Standard | SVMainFrame::Flag_Zoom );
		GetMainFrame()->RefreshAllSplitters();

		SVSVIMStateClass::RemoveState( SV_STATE_RUNNING );

		SetPriorityClass( GetCurrentProcess(), NORMAL_PRIORITY_CLASS );

		SVSoftwareTriggerDlg & l_trgrDlg = SVSoftwareTriggerDlg::Instance();
		l_trgrDlg.ClearTriggers();
	}
}// end OnStopAll


BOOL SVObserverApp::InitATL()
{
	HRESULT l_Status( S_OK );

	bool l_AppRegister( false );
	bool l_AppUnregister( false );

	m_ATLInited = FALSE;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	LPTSTR lpCmdLine = GetCommandLine(); //this line necessary for _ATL_MIN_CRT
	TCHAR szTokens[] = _T("-/");

	BOOL bRun = TRUE;
	LPCTSTR lpszToken = _Module.FindOneOf(lpCmdLine, szTokens);
	while (lpszToken != NULL)
	{
		if (lstrcmpi(lpszToken, _T("UnregServer"))==0)
		{
			l_AppUnregister = true;
		}
		else if (lstrcmpi(lpszToken, _T("RegServer"))==0)
		{
			l_AppRegister = true;
		}

		lpszToken = _Module.FindOneOf(lpszToken, szTokens);
	}

#if !defined(_WIN32_WCE) || defined(_CE_DCOM)
	// Register class factories via CoRegisterClassObject().
	l_Status = _Module.RegisterClassObjects( CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE );
	if( SUCCEEDED( l_Status ) )
	{
		m_ATLInited = TRUE;
	}
#endif // !defined(_WIN32_WCE) || defined(_CE_DCOM)

	if( FAILED( l_Status ) )
	{
		SVStringArray msgList;
		msgList.push_back(SvUl_SF::Format( _T("%08X"), l_Status ));
#ifdef _DEBUG
		if( ! l_AppRegister && ! l_AppUnregister )
		{
			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
			INT_PTR result = Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_SVObserver_RegisterClassObjectsFailed_Question, msgList, StdMessageParams, SvOi::Err_10135, NULL, nullptr, MB_YESNO );
			if( IDYES == result )
			{
				return FALSE;
			}
		}
		else
#endif
		{
			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
			INT_PTR result = Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_SVObserver_RegisterClassObjectsFailed, msgList, StdMessageParams, SvOi::Err_10136 );
			return FALSE;
		}
	}

	// App was launched with /Unregserver or /Unregister switch.
	if( l_AppUnregister )
	{
		if( SUCCEEDED( l_Status ) )
		{
			_Module.UpdateRegistryAppId(FALSE);

			_Module.UnregisterServer(TRUE);

			std::cout << _T( "****** SVObserver and SVCommand Un-registered ******" ) << std::endl;
		}

		return FALSE;
	}

	// App was launched with /Register or /Regserver switch.
	if( l_AppRegister )
	{
		if( SUCCEEDED( l_Status ) )
		{
			_Module.UpdateRegistryAppId(TRUE);

			_Module.RegisterServer(TRUE);

			std::cout << _T( "***** SVObserver and SVCommand Registered *****" ) << std::endl;
		}

		return FALSE;
	}

	// DON'T display a new MDI child window during startup!!!
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if( !ProcessShellCommand( cmdInfo ) ) { return FALSE; }

	return TRUE;
}

void SVObserverApp::StopRegression()
{
	//get list of IPDoc's.
	//see if the IPDoc is running regression, if so send command to stop.
	//since only 1 IPDoc can run regression at a time, as soon as 1 is found, break...

	CDocTemplate* pDocTemplate = NULL;
	CString strExt;
	POSITION pos = GetFirstDocTemplatePosition();
	bool bDone = false;
	if( pos )
	{
		do
		{
			pDocTemplate = GetNextDocTemplate( pos );
			if( pDocTemplate )
			{
				SVIPDoc* pTmpDoc;
				CDocument* newDoc;
				POSITION posDoc = pDocTemplate->GetFirstDocPosition();
				if ( posDoc )
				{
					do
					{
						newDoc = pDocTemplate->GetNextDoc(posDoc);
						if ( newDoc )
						{
							pTmpDoc = dynamic_cast <SVIPDoc*> (newDoc);
							if ( pTmpDoc )
							{
								if ( pTmpDoc->IsRegressionTestRunning() )
								{
									POSITION TmpPos = pTmpDoc->GetFirstViewPosition();
									CView *pIPView = pTmpDoc->GetNextView(TmpPos);
									CMDIChildWnd *pFrame = (CMDIChildWnd*) pIPView->GetParentFrame();
									pFrame->MDIActivate();
									pTmpDoc->RegressionTestModeChanged();
									bDone = true;
								}
							}
						}
					}
					while( posDoc && !bDone);
				}
			}
		}
		while( pos && !bDone );
	}

	OnStop();
}

DWORD SVObserverApp::GetActiveIOTab() const
{
	DWORD l_dwRet = 0;
	SVIODoc* l_pIODoc = GetIODoc();

	if( l_pIODoc != NULL )
	{
		POSITION lViewPos = l_pIODoc->GetFirstViewPosition();
		do 
		{
			CView* pView = l_pIODoc->GetNextView( lViewPos );

			CMDIChildWnd *pFrame = (CMDIChildWnd*) pView->GetParentFrame();
			SVIOTabbedView* pIOView = dynamic_cast<SVIOTabbedView*>( pFrame );
			if( pIOView )
			{
				TVisualObject* l_VisObj = pIOView->m_Framework.GetActivePane();
				if( l_VisObj )
				{
					l_dwRet = l_VisObj->GetID();
				}
				break;
			}
		}
		while ( lViewPos );
	}
	return l_dwRet;
}

bool SVObserverApp::AddSecurityNode( HMODULE hMessageDll, long lId ,CString sNTGroup, bool bForcePrompt)
{
	LPTSTR pszTmp;

	if (FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS |
		FORMAT_MESSAGE_FROM_HMODULE,
		(LPCVOID) hMessageDll, lId,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &pszTmp, 0, NULL))
	{

		CString strTmp = pszTmp;
		m_svSecurityMgr.SVAdd( lId, strTmp.Left( strTmp.Find('\n',0) -1), sNTGroup, bForcePrompt);
		LocalFree( pszTmp );
		return true;
	}
	return false;
}

bool SVObserverApp::AddSecurityNode( HMODULE hMessageDll, long lId )
{
	bool l_bRet = true;
	LPTSTR pszTmp;

	if (FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS |
		FORMAT_MESSAGE_FROM_HMODULE,
		(LPCVOID) hMessageDll, lId,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &pszTmp, 0, NULL))
	{
		CString strTmp = pszTmp;
		int l_pos = strTmp.Find('\n',0);
		if( l_pos > 0 )
		{
			m_svSecurityMgr.SVAdd( lId, strTmp.Left( l_pos -1) );
		}
		else
		{
			l_bRet = false;
		}
		LocalFree( pszTmp );
	}
	return l_bRet;
}

int SVObserverApp::FindMenuItem(CMenu* Menu, LPCTSTR MenuString)
{
	ASSERT(Menu);
	ASSERT(::IsMenu(Menu->GetSafeHmenu()));

	int count = Menu->GetMenuItemCount();
	for (int i = 0; i < count; i++)
	{
		CString str;
		if (Menu->GetMenuString(i, str, MF_BYPOSITION) &&
			(strcmp(str, MenuString) == 0))
		{
			return i;
		}
	}

	return -1;
}



#pragma endregion Private Methods

