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

#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVLibrary\SVPackedFile.h"
#include "SVObjectLibrary\SVObjectSynchronousCommandTemplate.h"
#include "CameraLibrary\SVDeviceParam.h"
#include "CameraLibrary\SVDeviceParams.h"
#include "CameraLibrary\SVLongValueDeviceParam.h"
#include "SVLibrary\DisplayMessageBox.h"
#include "SVLibrary\SVOINIClass.h"
#include "SVLibrary\SVOINILoader.h"
#include "SVTimerLibrary\SVClock.h"
#include "SVUtilityLibrary\SVGUID.h"
#include "SVMessage\SVMessage.h"
#include "SVObserver_i.h"
#include "SVCommand.h"
#include "SVAboutDialogClass.h"
#include "SVConfigurationPrint.h"
#include "SVEnvironmentSettingsDialog.h"
#include "SVSystemLibrary/SVFileNameManagerClass.h"
#include "SVImageViewScroll.h"
#include "SVIPChildFrm.h"
#include "SVMainFrm.h"
#include "SVDiscreteInputsView.h"
#include "SVMultiDocTemplate.h"
#include "SVStartWindow.h"
#include "SVGlobal.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
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
#include "SVToolSet.h"
#include "ExtrasEngine.h"

#include "SVInputObjectList.h"
#include "SVOutputObjectList.h"
#include "SVDigitalInputObject.h"
#include "SVDigitalOutputObject.h"


#include "SVXMLLibrary\LoadConfiguration.h"
#include "SVDigitizerProcessingClass.h"

#include "SVOConfigAssistantDlg.h"

#include "SVXMLLibrary\SVNavigateTree.h"

#include "SVConfigurationObject.h"
#include "ObjectInterfaces\SVUserMessage.h"

#include "SVGigeCameraManagerDlg.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVMemoryManager.h"
#include "SVArchiveTool.h"
#include "SVArchiveWritingDlg.h"
#include "SVLibrary\SVWinUtility.h"
#include "SVIOLibrary\SVIOParameterEnum.h"
#include "SoftwareTriggerDlg.h"

#include "ObjectInterfaces/GlobalConst.h"

#include "SVIOController.h"

#include "SVDirectX.h"
#include "TriggerInformation/SVHardwareManifest.h"
#include "TriggerInformation/SVTriggerProcessingClass.h"
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
#include "SVSharedMemoryLibrary/SharedMemWriter.h"
#include "SVUtilityLibrary\LoadDll.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "ObjectInterfaces\TextDefineSvOi.h"
#include "TextDefinesSvO.h"
#include "SVXMLLibrary\SVObjectXMLWriter.h"
#include "SVStatusLibrary\MessageContainer.h"
#include "SVStatusLibrary\GlobalPath.h"
#include "SVXMLLibrary\ObsoleteItemChecker.h"
#include "SVObjectLibrary\GlobalConst.h"
#include "SVMatroxLibrary\SVMatroxSystemInterface.h"
#include "SVSharedMemoryLibrary\ShareEvents.h"

#pragma endregion Includes

#pragma region Declarations
#define ID_TRIGGER_SETTINGS 21017

static const int UseLargerArchiveMemoryPool = 16000;
static const int AsyncDefault4GB = 50;
static const int AsyncDefault16GB = 200;
static const int GoOfflineDefault4GB = 300;
static const int GoOfflineDefault16GB = 2000;

static const double	cNormalNonPageMemoryUsage = 80.0;		 //Value as a percentage of the total Non-Page size

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
	ON_UPDATE_COMMAND_UI(ID_RESULTS_TABLE_PICKER, OnUpdateResultsPicker)
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
	ON_UPDATE_COMMAND_UI(ID_ADD_SHIFTTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_WINDOWTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_CYLINDRICALWARPTOOL, OnUpdateAddCylindricalWarpTool)
	ON_UPDATE_COMMAND_UI(ID_APP_ABOUT, OnUpdateAppAbout)
	ON_UPDATE_COMMAND_UI(ID_ADD_LOADIMAGETOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_IMAGETOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_ARCHIVETOOL,OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_MATHTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_STATISTICSTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_TRANSFORMATIONTOOL, OnUpdateAddTransformationTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_EXTERNAL_TOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_POLARUNWRAPTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_ACQUISITIONTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_COLORTOOL, OnUpdateAddColorTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_LINEARTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_REMOTEINPUTTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_RESIZETOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_RINGBUFFERTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_TABLETOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_TABLEANALYZERTOOL, OnUpdateAddGeneralTool)
	ON_UPDATE_COMMAND_UI(ID_ADD_PERSPECTIVEWARPTOOL, OnUpdateAddGeneralTool)
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
: m_InputStreamPortNumber( InputStreamPortNumber )
, m_OutputStreamPortNumber( OutputStreamPortNumber )
, m_FailStatusStreamPortNumber( FailStatusStreamPortNumber )
, m_RemoteCommandsPortNumber( RemoteCommandsPortNumber )
, m_DataValidDelay( 0 )
, m_rInitialInfo(m_IniInfoHandler.GetInitialInfo())
{
	free((void*)m_pszHelpFilePath);
	
	m_pszHelpFilePath = _tcsdup(SvStl::GlobalPath::Inst().GetBinPath(_T("SVObserver.chm")).c_str());
	EnableHtmlHelp();

#if defined (TRACE_THEM_ALL) || defined (TRACE_SVO)
	::OutputDebugString( _T( "Executing => SVObserverApp::SVObserverApp()\n" ) );
#endif

	m_IniInfoHandler.ResetInformation();

	// The Standard Configuration Execution Directory
	m_ConfigExePNVariableName			= _T( "ConfigurationExecutionPathName" );
	
	m_ConfigExePNVariableValue			= _T( "" );

	// The Standard Last Valid Configuration Directory
	m_LastValidConfigPNVariableName			= _T( "LastValidConfigurationPathName" );

	m_LastValidConfigPNVariableValue		= _T( "" );

	m_OfflineCount = 0;
	m_ShouldRunLastEnvironmentAutomatically = FALSE;
	m_AutoRunDelayTime = 1000;

	m_CurrentVersion = SvSyl::SVVersionInfo::GetLongVersion();
	m_LoadingVersion = 0L;

	m_pCurrentDocument = nullptr;	// Set by current Document!!!
	m_pMessageWindow = nullptr;

	m_ConfigFileName.SetFileType( SV_SVX_CONFIGURATION_FILE_TYPE );
	m_ConfigFileName.setExcludeCharacters( SvO::SVEXCLUDECHARS_CONFIG_NAME );

	SVFileNameManagerClass::Instance().AddItem(&m_ConfigFileName);

}// end SVObserver ctor

SVObserverApp::~SVObserverApp()
{
	// File management for config file.
	SVFileNameManagerClass::Instance().RemoveItem( &m_ConfigFileName );
}
#pragma endregion Constructor

#pragma region Public Methods
#pragma region AFX_MSG Methods
////////////////////////////////////////////////////////////////////////////////
// .Title       : OnAppAbout
// -----------------------------------------------------------------------------
// .Description : ...
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
////////////////////////////////////////////////////////////////////////////////
void SVObserverApp::OnFileSaveConfig()
{
	if( SVString( getConfigFullFileName() ).empty() || 
		SVString( getConfigPathName() ).empty() )
	{
		fileSaveAsSVX();
	}
	else
	{
		SVString TempName = SVFileNameManagerClass::Instance().GetConfigurationPathName();

		if ( TempName.empty() )
		{
			TempName = getConfigFullFileName();
		}
		else
		{
			TempName += _T( "\\" );
			TempName += getConfigFileName();
		}

		fileSaveAsSVX( TempName.c_str() );
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnFileOpenSVC
// -----------------------------------------------------------------------------
// .Description : General - Opens a config File.
//				: - Calls a config File Open Dialog 
//				: - If a config already exists, calls the config saving procedure
//              : - Calls the config loading procedure
////////////////////////////////////////////////////////////////////////////////
void SVObserverApp::OnFileOpenSVC() 
{
	ValidateMRUList();
	
	SVSVIMStateClass::AddState( SV_STATE_EDITING ); /// do this before calling validate for security as it may display a logon dialog!
	// Proof user rights...
	if ( S_OK == m_svSecurityMgr.SVValidate( SECURITY_POINT_FILE_MENU_SELECT_CONFIGURATION) )
	{
		SVFileNameClass svFileName;

		svFileName.SetFileType( SV_SVX_CONFIGURATION_FILE_TYPE );
		//
		// Try to read the current image file path name from registry...
		//
		svFileName.SetPathName( AfxGetApp()->GetProfileString( _T( "Settings" ), 
			_T( "SVCFilePath" ), 
			SvStl::GlobalPath::Inst().GetRunPath().c_str() ) );
		if ( svFileName.SelectFile() )
		{
			//
			// Write this path name back to registry...to initialize registry.
			//
			AfxGetApp()->WriteProfileString( _T("Settings"), _T("SVCFilePath"),	svFileName.GetPathName().c_str() );

			// Check for SVX file...
			if ( 0 == SvUl_SF::CompareNoCase( svFileName.GetExtension(), SVString(_T( ".svx")) ) )
			{
				//
				// Open the configuration file (.svx) and read it and
				// all the associated files for this configuration.
				//
				TheSVOLicenseManager().ClearLicenseErrors();
				if ( S_OK == OpenSVXFile( svFileName.GetFullFileName().c_str() ) )
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
	}// end if ( S_OK == m_svSecurityMgr.Validate( SECURITY_POINT_FILE_MENU_SELECT_CONFIGURATION) )
	// Update Remote Inputs Tab
	UpdateRemoteInputTabs();

	SVSVIMStateClass::RemoveState( SV_STATE_EDITING );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnEditEnvironment
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
void SVObserverApp::OnEditEnvironment() 
{
	ShowConfigurationAssistant();
}

// .Description : OnTestMode calls SetTestMode
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
		SvStl::MessageMgrStd  Exception( SvStl::LogOnly );
		Exception.setMessage( rExp.getMessage() );
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnStopTestMode
// -----------------------------------------------------------------------------
// .Description : ...
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

	if( S_OK == m_svSecurityMgr.SVValidate( SECURITY_POINT_MODE_MENU_STOP ) )
	{
		OnStop();
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnEnvironmentSettings
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
void SVObserverApp::OnEnvironmentSettings() 
{
	bool l_bShowDDETabs = false;

	SVSVIMStateClass::AddState(SV_STATE_EDITING); /// do this before calling validate for security as it may display a logon dialog!
	if( S_OK == m_svSecurityMgr.SVValidate( SECURITY_POINT_EXTRAS_MENU_ADDITIONAL_ENVIRON ) )
	{
		SVEnvironmentSettingsDialogClass cfdDlg;

		cfdDlg.StartLastAutomatically = m_ShouldRunLastEnvironmentAutomatically;

		if( IDOK == cfdDlg.DoModal() )
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
////////////////////////////////////////////////////////////////////////////////
void SVObserverApp::OnFileCloseConfig() 
{
	if( S_OK == m_svSecurityMgr.SVValidate( SECURITY_POINT_FILE_MENU_CLOSE_CONFIGURATION ) )
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
////////////////////////////////////////////////////////////////////////////////
void SVObserverApp::OnFileOpen() 
{
	if( S_OK == m_svSecurityMgr.SVValidate( SECURITY_POINT_FILE_MENU_SELECT_CONFIGURATION ) )
	{
		CWinApp::OnFileOpen();
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnFileSaveAll
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
void SVObserverApp::OnFileSaveAll() 
{
	if( S_OK == m_svSecurityMgr.SVValidate( SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION ) )
	{
		OnFileSaveConfig();
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnEditIOList
// -----------------------------------------------------------------------------
// .Description : ...
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
////////////////////////////////////////////////////////////////////////////////
void SVObserverApp::OnUpdateEditIOList( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST  | SV_STATE_REGRESSION ) &&
		SVSVIMStateClass::CheckState( SV_STATE_EDIT ) && nullptr != GetIODoc() );
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

	SVString TriggerCounts;
	GetTriggersAndCounts( TriggerCounts );

	//add message to event viewer - gone off-line
	SvStl::MessageMgrStd Exception( SvStl::LogOnly );
	Exception.setMessage( SVMSG_SVO_28_SVOBSERVER_GO_OFFLINE, TriggerCounts.c_str(), SvStl::SourceFileParams(StdMessageParams) );

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
		RemoveMenu(hmen, _T("&Edit\\Published Results") ); // start empty.
		for( UINT i = 0; i < static_cast< UINT >( l_Inspections.size() ) && i < (ID_EDIT_PUBLISHEDRESULTS_LIMIT - ID_EDIT_PUBLISHEDRESULTS_BASE + 1); i++ )
		{
			// Add a menu for each inspection.
			SVString Name = SvUl_SF::Format(_T("&Edit\\Published Results\\%s"), l_Inspections[i]->GetName());
			AddMenuItem(hmen, Name, static_cast< UINT >( ID_EDIT_PUBLISHEDRESULTS_BASE + i ) );
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

void SVObserverApp::OnUpdateAddGeneralTool( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&	
		OkToEdit() &&
		IsMonochromeImageAvailable() );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnUpdateAppExit
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
void SVObserverApp::OnUpdateAppExit( CCmdUI* PCmdUI ) 
{
	PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_REGRESSION ) 
		&& m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_FILE_MENU_EXIT ));	
}

void SVObserverApp::OnUpdateAppAbout( CCmdUI* PCmdUI ) 
{
}

void SVObserverApp::OnFilePrintConfig() 
{
	if( S_OK == m_svSecurityMgr.SVValidate( SECURITY_POINT_FILE_MENU_PRINT ) )
	{
		if ( ! SVSVIMStateClass::CheckState( SV_STATE_READY | SV_STATE_RUNNING ) || 
			SVString( getConfigFullFileName() ).empty() )
		{
			return;
		}

		SVConfigurationPrint printConfig;
		printConfig.DoPrintConfig();
	}
}

void SVObserverApp::OnSVOFilePrintSetup()
{
	if( S_OK == m_svSecurityMgr.SVValidate( SECURITY_POINT_FILE_MENU_PRINT_SETUP ) )
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
		if( S_OK == m_svSecurityMgr.SVValidate( SECURITY_POINT_MODE_MENU_STOP, 
			SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE ) )
		{
			OnStop();
		}
	}
	else if( S_OK == m_svSecurityMgr.SVValidate( SECURITY_POINT_MODE_MENU_STOP ) )
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
////////////////////////////////////////////////////////////////////////////////
void SVObserverApp::OnGoOnline() 
{
	SvStl::MessageContainer exceptionContainer;
	ExtrasEngine::Instance().ExecuteAutoSaveIfAppropriate(true);

	long l_lPrevState = 0;

	if( SVSVIMStateClass::CheckState( SV_STATE_EDIT ))
	{
		l_lPrevState = SV_STATE_EDIT;
	}

	if( SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) ) { return; }

	if( S_OK == m_svSecurityMgr.SVValidate( SECURITY_POINT_MODE_MENU_RUN ) )
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
			SVSVIMStateClass::RemoveState( SV_STATE_EDIT );
			DeselectTool();

			if ( S_OK == m_IniInfoHandler.GetInitializationStatusFlags() )
			{
				if ( CheckSVIMType() )
				{
					CWaitCursor wait;

					try
					{
						Start();
					}
					catch (const SvStl::MessageContainer& rExp)
					{
						exceptionContainer = rExp;
					}
				}
				else
				{
					SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SVObserver_CannotRun_WrongModelNumber, m_rInitialInfo.GetModelNumberVector(), SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10121 );
					SVSVIMStateClass::AddState( l_lPrevState );
				}
			}
			else
			{
				SVStringVector msgList;
				msgList.push_back(m_rInitialInfo.GetModelNumberString());
				msgList.push_back(m_rInitialInfo.InitializationFailureDescription(m_IniInfoHandler.GetInitializationStatusFlags()));

				SvStl::MessageMgrStd Msg(SvStl::LogAndDisplay);
				Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SVObserver_InitializationFailure, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10126);

				SVSVIMStateClass::AddState( l_lPrevState );
			}

			if ( SVSVIMStateClass::CheckState( SV_STATE_STOP_PENDING ) )
			{
				PostMessage( m_pMainWnd->m_hWnd, WM_COMMAND, MAKEWPARAM (ID_STOP, 0), 0);
			}
			else if ( 0 != exceptionContainer.getMessage().m_MessageCode )
			{
				SvStl::MessageMgrStd Exception(SvStl::LogAndDisplay);
				Exception.setMessage(exceptionContainer.getMessage());
				SVSVIMStateClass::AddState( l_lPrevState );
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnUpdateGoOnline
// -----------------------------------------------------------------------------
// .Description : ...
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

	// We need to deselect any tool that might be set for operator move.
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
// Command message processor for range: ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16
//
BOOL SVObserverApp::OnOpenRecentFile( UINT nID ) 
{
	BOOL l_bOk = false;
	if( S_OK == m_svSecurityMgr.SVValidate( SECURITY_POINT_FILE_MENU_RECENT_CONFIGURATIONS ) )
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
	return true;
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
////////////////////////////////////////////////////////////////////////////////
void SVObserverApp::OnAppExit() 
{
	bool l_bAllowAccess = true;

	if ( SVSVIMStateClass::CheckState(SV_STATE_REGRESSION) )
	{
		return;
	}

	if( SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
	{
		if( S_OK == m_svSecurityMgr.SVValidate( SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE, 
			SECURITY_POINT_FILE_MENU_EXIT ) )
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
////////////////////////////////////////////////////////////////////////////////
void SVObserverApp::OnFileSaveAsSVC() 
{
	if( S_OK == m_svSecurityMgr.SVValidate( SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION_AS ) )
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
	SVSVIMStateClass::AddState(SV_STATE_EDITING); /// do this before calling validate for security as it may display a logon dialog!
	if( S_OK == m_svSecurityMgr.SVValidate( SECURITY_POINT_EXTRAS_MENU_UTILITIES_SETUP ) )
	{
		pWindow = AfxGetMainWnd();
		pMenu = pWindow->GetMenu();

		if ( pMenu = util.FindSubMenuByName( pMenu, _T("&Utilities") ) )
		{
			util.SetupUtilities( pMenu );
			UpdateAllMenuExtrasUtilities();
		}
	}
	else
	{
		SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_AuthorizationFailed_Modification, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10123 );
	}
	SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
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

	if( S_OK == m_svSecurityMgr.SVValidate( SECURITY_POINT_EXTRAS_MENU_UTILITIES_RUN ) )
	{
		util.RunUtility (&m_svSecurityMgr, uiId);
	}
	else
	{
		SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_AuthorizationFailed_Execution, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10124 );
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnRCGoOffline
// -----------------------------------------------------------------------------
// .Description : ...
//              : NOTE: Be careful by implementing an update function for
//				:		for this command! RC should be able to have access
//				:		all the time!
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
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
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
		BOOL bModified = SVSVIMStateClass::CheckState( SV_STATE_MODIFIED );

		SVString ConfigPath = SVFileNameManagerClass::Instance().GetConfigurationPathName();

		if ( !ConfigPath.empty() )
		{
			SVFileNameManagerClass::Instance().SetConfigurationPathName( _T("") );
		}

		SVFileNameClass svFileName;

		svFileName.SetFullFileName( getConfigFullFileName().c_str() );
		svFileName.SetPathName( SvStl::GlobalPath::Inst().GetRunPath().c_str() );
		svFileName.SetExtension( _T( ".svx" ) );

		fileSaveAsSVX( svFileName.GetFullFileName() );

		SVRCSetSVCPathName( getConfigFullFileName().c_str() );

		if ( ! ConfigPath.empty() )
		{
			SVFileNameManagerClass::Instance().SetConfigurationPathName( ConfigPath.c_str() );
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
////////////////////////////////////////////////////////////////////////////////
void SVObserverApp::OnUpdateAllIOViews()
{
	if ( GetIODoc() && 
		SVSVIMStateClass::CheckState( SV_STATE_READY | SV_STATE_RUNNING ) &&
		!SVSVIMStateClass::CheckState( SV_STATE_CANCELING ) )
	{
		GetIODoc()->UpdateAllViews( nullptr );
	}
}

void SVObserverApp::OnExtrasSecuritySetup() 
{
	SVSVIMStateClass::AddState(SV_STATE_EDITING); /// do this before calling validate for security as it may display a logon dialog!
	if( S_OK == m_svSecurityMgr.SVValidate( SECURITY_POINT_EXTRAS_MENU_SECURITY_MANAGER ) )
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
		if( S_OK == m_svSecurityMgr.SVValidate( SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE, 
			SECURITY_POINT_MODE_MENU_EDIT_TOOLSET ) )
		{
			OnStop();
			l_bAllowAccess = true;
		}
	}
	else if( S_OK == m_svSecurityMgr.SVValidate( SECURITY_POINT_MODE_MENU_EDIT_TOOLSET ) )
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
			SVIPDoc* pDoc =  GetIPDoc( l_Inspections[uiInspection]->GetUniqueObjectID() );
			if( nullptr != pDoc )
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
////////////////////////////////////////////////////////////////////////////////
BOOL SVObserverApp::InitInstance()
{
	CWinApp::InitInstance();

	HINSTANCE ResourceInstance( nullptr );
	//Load resource dll explicitly
	HRESULT retValue = SvUl::LoadDll::Instance().getDll( SvUl::SVOResourceDll, ResourceInstance );
	if (S_OK != retValue || nullptr == ResourceInstance)
	{
		//Because our exception handler (message box) needs the resources, we have to use here the standard message box.
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_SVO_53_RESOURCE_DLL_LOADING_FAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10009_LoadOfResourceDllFailed );
		MessageBox(nullptr, SvO::LoadingResourceDllFailed, nullptr, MB_OK | MB_ICONSTOP );
		exit(-SvStl::Err_10009_LoadOfResourceDllFailed);
	}

	SvStl::MessageMgrStd::setShowDisplayFunction( boost::bind( &SvLib::DisplayMessageBox::showDialog, _1, _2, _3, _4 ) );

	// load File based write filter DLL. SVObserver will function normally (except for FBWF functionally, of course) if "fbwflib.dll" is not found
	SvUl::LoadDll::Instance().getDll( SvO::FbwfDllName, ExtrasEngine::ms_FbwfDllInstance);

	//Set the resource instance to the resource dll
	AfxSetResourceHandle( ResourceInstance );

	m_hEvent = CreateEvent(nullptr, false, true, AfxGetAppName()); 

	if ( ERROR_ALREADY_EXISTS == GetLastError() )
	{ 
		return false;
	} 

	// OLE-Bibliotheken initialisieren
	if( !AfxOleInit() )
	{
		SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_OleInitFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10125 );
		return false;
	}

	if( !InitATL() )
	{
		return false;
	}

	//Initializing  must be before first use of  MessageNotification::FireNotify which is i.e called from CheckDrive 
	SVVisionProcessorHelper::Instance().Startup();

	// Check for proper setup of V: for SVRemoteControl
	if( S_OK != CheckDrive(_T("v:\\")) )
	{
		return false;
	}

	#ifdef _DEBUG
		long l_CrtVal = GetPrivateProfileInt( _T( "Memory" ), _T( "CrtSetBreakAlloc" ), 0, SvStl::GlobalPath::Inst().GetBinPath("SVDebug.ini").c_str() ) ;

		if( 0 < l_CrtVal )
		{
			::_CrtSetBreakAlloc( l_CrtVal );
		}
	#endif

	#ifdef __USE_TRACEWIN
		afxTraceEnabled = TRUE;
		PxlTraceInit();                                    // 13 May 1999 - frb.
	#endif //__USE_TRACEWIN
#if defined (TRACE_THEM_ALL) || defined (TRACE_SVO)
	TRACE0("..SVObserver - InitInstance()\n");         // 13 May 1999 - frb.
#endif
	// EB 2002 11 19
	// added to ensure C:\TEMP always exists;
	// bad things happen if it doesn't
	// example of symptom: crash when SVFocus tries to connect
	if (_taccess(SvStl::GlobalPath::Inst().GetTempPath().c_str(), 0) != 0)
	{
		_tmkdir(SvStl::GlobalPath::Inst().GetTempPath().c_str());
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

	SvTi::SVHardwareManifest::Instance().Startup();
	SvTi::SVTriggerProcessingClass::Instance().Startup();
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

	SvLib::SVOINIClass SvimIni( SvStl::GlobalPath::Inst().GetSVIMIniPath() );

	ValidateMRUList();

	// Get SourceImageDepth
	m_lSouceImageDepth = GetProfileInt(_T( "Settings" ),_T( "Source Image Depth" ), -1 );
	if ( m_lSouceImageDepth == -1 )
	{
		WriteProfileInt(_T( "Settings" ),_T( "Source Image Depth" ), SvOi::cDefaultSourceImageDepth );

		m_lSouceImageDepth = SvOi::cDefaultSourceImageDepth;
	}

	// Get SourceImageDepth
	m_lResultImageDepth = GetProfileInt(_T( "Settings" ),_T( "Result Image Depth" ), -1 );
	if ( m_lResultImageDepth == -1 )
	{
		WriteProfileInt(_T( "Settings" ),_T( "Result Image Depth" ), SvOi::cDefaultResultImageDepth );

		m_lResultImageDepth = SvOi::cDefaultResultImageDepth;
	}

	// Get LogDataManager
	m_LogDataManager = UpdateAndGetLogDataManager();

	// Supplement for SVObserver 2.0
	// Set up path name environment variables...

	// Configuration Execution Path Name... ( "Run Directory" )
	m_ConfigExePNVariableValue = SvStl::GlobalPath::Inst().GetRunPath(_T("\\")).c_str();

	// Update registry...
	WriteProfileString( _T( "Settings" ), m_ConfigExePNVariableName, m_ConfigExePNVariableValue.c_str() );

	// Check path, create if necessary but don't delete contents...
	InitPath( m_ConfigExePNVariableValue.c_str(), true, false );
	SetEnvironmentVariable( m_ConfigExePNVariableName, m_ConfigExePNVariableValue.c_str() );

	// Last Valid Configuration Path Name... ( "Last Valid Directory" )
	
	m_LastValidConfigPNVariableValue = SvStl::GlobalPath::Inst().GetLastValidPath(_T("\\")).c_str();
	WriteProfileString( _T( "Settings" ), m_LastValidConfigPNVariableName, m_LastValidConfigPNVariableValue.c_str() );
	
	// Check path, create if necessary but don't delete contents...
	InitPath( m_LastValidConfigPNVariableValue.c_str(), true, false );
	SetEnvironmentVariable( m_LastValidConfigPNVariableName, m_LastValidConfigPNVariableValue.c_str() );

	// Load special profile settings
	if( GetProfileInt( _T( "Settings" ), _T( "Run Last Configuration Automatically" ), 0 ) )
	{
		m_ShouldRunLastEnvironmentAutomatically = true;
	}
	else
	{
		m_ShouldRunLastEnvironmentAutomatically = false;
	}

	// Get AutoRunDelay from Registry...
	m_AutoRunDelayTime = GetProfileInt( _T( "Settings" ), _T( "Auto Run Delay Time" ), m_AutoRunDelayTime );
	WriteProfileInt( _T( "Settings" ), _T( "Auto Run Delay Time" ), m_AutoRunDelayTime );

	// *** // ***
	SVObjectManagerClass::Instance().ConstructRootObject( RootObjectGuid );
	// *** // ***

	m_IniInfoHandler.LoadIniFilesAndDlls();

	if (S_OK != m_IniInfoHandler.GetInitializationStatusFlags())
	{
		SVStringVector msgList;
		msgList.push_back(m_rInitialInfo.GetModelNumberString());
		msgList.push_back(m_rInitialInfo.InitializationFailureDescription(m_IniInfoHandler.GetInitializationStatusFlags()));

#ifndef _DEBUG                    // 23 Mar 1999 - frb.
		sWin.ShowWindow(SW_HIDE);
#endif //_DEBUG                  // 23 Mar 1999 - frb.

		SvStl::MessageMgrStd Msg(SvStl::LogAndDisplay);
		Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SVObserver_InitializationFailure, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10126);
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
	SVMultiDocTemplateClass* pDocTemplate = new SVMultiDocTemplateClass( IDR_SVOBSERVER_IPDOCTYPE,
		RUNTIME_CLASS( SVIPDoc ),			 // Doc
		RUNTIME_CLASS( SVIPSplitterFrame ),  // Frame
		RUNTIME_CLASS( SVImageViewScroll ) );// View

	AddDocTemplate( pDocTemplate );

	// IODoc
	SVMultiDocTemplateClass* pDocTemplate1 = new SVMultiDocTemplateClass( IDR_SVOBSERVER_IODOCTYPE,
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
	{ return false; }
	m_pMainWnd = pMainFrame;

	// Load Utilities Menu
	SVUtilitiesClass util;
	CWnd *pWindow;
	CMenu *pMenu;

	pWindow = AfxGetMainWnd();
	pMenu = pWindow->GetMenu();

	if ( pMenu = util.FindSubMenuByName( pMenu, _T("&Utilities") ) )
	{
		util.LoadMenu( pMenu );
	}

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
	SVStringVector MessageList;
	MessageList.push_back(SvUl_SF::Format(_T("%d"), AmountOfRam));
	SvStl::MessageMgrStd Exception( SvStl::LogOnly );
	Exception.setMessage(SVMSG_SVO_54_EMPTY, SvStl::Tid_AmountOfSystemMemoryText, MessageList, SvStl::SourceFileParams(StdMessageParams), SvStl::Memory_Log_45001);

	//if amount of physical memory is around 16 GigE allocate the larger memory pools.
	if (AmountOfRam >= UseLargerArchiveMemoryPool)
	{
		iGoOfflineBufferSize = SvimIni.GetValueInt( _T("Settings"), _T("ArchiveToolGoOfflineBufferSize"), GoOfflineDefault16GB );
		iAsyncBufferSize = SvimIni.GetValueInt( _T("Settings"), _T("ArchiveToolAsyncBufferSize"), AsyncDefault16GB);
		TheSVMemoryManager().CreatePool(SvO::ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME, iGoOfflineBufferSize * 1024);
		TheSVMemoryManager().CreatePool(SvO::ARCHIVE_TOOL_MEMORY_POOL_ONLINE_ASYNC_NAME, iAsyncBufferSize * 1024);
	}
	else
	{
		iGoOfflineBufferSize = SvimIni.GetValueInt( _T("Settings"), _T("ArchiveToolGoOfflineBufferSize"), GoOfflineDefault4GB );
		iAsyncBufferSize = SvimIni.GetValueInt( _T("Settings"), _T("ArchiveToolAsyncBufferSize"), AsyncDefault4GB );
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
	}

	// add message to event viewer - SVObserver Started
	SVStringVector msgList;
	if( (m_CurrentVersion & 0xff) == 0xff )
	{
		msgList.push_back(SvSyl::SVVersionInfo::GetShortTitleVersion());
	}
	else
	{
		msgList.push_back(SvSyl::SVVersionInfo::GetTitleVersion());
	}
	Exception.setMessage( SVMSG_SVO_25_SVOBSERVER_STARTED, SvStl::Tid_Version, msgList, SvStl::SourceFileParams(StdMessageParams) );

	SVDirectX::Instance().Initialize();

	SvSol::SVSocketLibrary::Init();

	int AutoSaveValue = SvimIni.GetValueInt( _T("Settings"), _T("EnableAutosave"), 0); //Arvid accept a number: non-zero enables

	std::string AutoSaveValueString=SvimIni.GetValueString( _T("Settings"), _T("EnableAutosave"), _T("FALSE")); //Arvid accept a string

	if ( AutoSaveValueString == std::string("TRUE") || AutoSaveValueString == std::string("true"))
	{
		AutoSaveValue = 1;
	}

	ExtrasEngine::Instance().SetEnabled(AutoSaveValue != 0);
	unsigned short defaultPortNo = -1;
	m_RemoteCommandsPortNumber = SvimIni.GetValueInt( _T("Settings"), _T("RemoteCommandsPortNumber"), defaultPortNo );
	if ( m_RemoteCommandsPortNumber == defaultPortNo )
	{
		m_RemoteCommandsPortNumber = RemoteCommandsPortNumber;

		SvimIni.SetValueInt( _T( "Settings" ),_T( "RemoteCommandsPortNumber" ), m_RemoteCommandsPortNumber );
	}

	m_InputStreamPortNumber = SvimIni.GetValueInt( _T("Settings"), _T("InputStreamPortNumber"), defaultPortNo );
	if ( m_InputStreamPortNumber == defaultPortNo )
	{
		m_InputStreamPortNumber = InputStreamPortNumber;

		SvimIni.SetValueInt( _T( "Settings" ),_T( "InputStreamPortNumber" ), m_InputStreamPortNumber );
	}

	m_OutputStreamPortNumber = SvimIni.GetValueInt( _T("Settings"), _T("OutputStreamPortNumber"), defaultPortNo );
	if ( m_OutputStreamPortNumber == defaultPortNo)
	{
		m_OutputStreamPortNumber = OutputStreamPortNumber;

		SvimIni.SetValueInt( _T( "Settings" ),_T( "OutputStreamPortNumber" ), m_OutputStreamPortNumber );
	}

	m_FailStatusStreamPortNumber = SvimIni.GetValueInt( _T("Settings"), _T("FailStatusStreamPortNumber"), defaultPortNo );
	if ( m_FailStatusStreamPortNumber == defaultPortNo )
	{
		m_FailStatusStreamPortNumber = FailStatusStreamPortNumber;

		SvimIni.SetValueInt(_T( "Settings" ),_T( "FailStatusStreamPortNumber" ), m_FailStatusStreamPortNumber );
	}

	m_DataValidDelay = static_cast<long> ( SvimIni.GetValueInt( _T("Settings"), _T("DataValidDelay"), 0 ) );

	SVSocketRemoteCommandManager::Instance().Startup( m_RemoteCommandsPortNumber );
	SVInputStreamManager::Instance().Startup( m_InputStreamPortNumber );
	SVOutputStreamManager::Instance().Startup( m_OutputStreamPortNumber );
	SVFailStatusStreamManager::Instance().Startup( m_FailStatusStreamPortNumber );

	if ( !TheSVOLicenseManager().HasMatroxLicense() )
	{
		SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
		Exception.setMessage( SVMSG_SVO_52_NOMATROX_DONGLE, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25013_NoMatroxDongle );
	}

	if ( TheSVOLicenseManager().HasMatroxLicense() && !TheSVOLicenseManager().HasMatroxGigELicense() && IsMatroxGige() )
	{
		SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_MatroxGigELicenseNotFound, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10127 );
	}
	
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnIdle(LONG lCount)
// -----------------------------------------------------------------------------
// .Description : ...
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

	SvSol::SVSocketLibrary::Destroy();
	SvSml::SharedMemWriter::Instance().Destroy();
	ValidateMRUList();

	// Destroy still open message windows
	DestroyMessageWindow();

	#if !defined( _DEBUG )
		// Display close window	
		m_pMessageWindow = new SVMessageWindowClass;
		if( m_pMessageWindow && m_pMessageWindow->Create( IDD_MESSAGE_DIALOG ) )
		{
			m_pMessageWindow->ShowWindow( SW_SHOW );
		}
	#else
		m_pMessageWindow = nullptr;
	#endif

	// *** // ***
	SVObjectManagerClass::Instance().DestroyRootObject();
	// *** // ***

	m_IniInfoHandler.INIClose();

	m_mgrRemoteFonts.Shutdown();

	SvTi::SVTriggerProcessingClass::Instance().Shutdown();
	SVDigitizerProcessingClass::Instance().Shutdown();
	SvTi::SVHardwareManifest::Instance().Shutdown();

	SVObjectManagerClass::Instance().Shutdown();
	SVClassRegisterListClass::Instance().Shutdown();

	SVIOConfigurationInterfaceClass::Instance().Shutdown();

	// Shutdown MIL
	SVMatroxApplicationInterface::Shutdown();

	SvStl::MessageMgrStd::setShowDisplayFunction( SvStl::ShowDisplayFunctor() );

	//add message to event viewer - SVObserver stopped
	SvStl::MessageMgrStd Exception( SvStl::LogOnly );
	Exception.setMessage( SVMSG_SVO_26_SVOBSERVER_STOPPED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
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
		SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
		//Set the error code to unhandled exception but use the rest of the data from the original exception
		SvStl::MessageData Msg( rExp.getMessage() );
		SVString OrgMessageCode = SvUl_SF::Format( _T("0x%08X"), Msg.m_MessageCode );
		Msg.m_AdditionalTextId = SvStl::Tid_Default;
		SVStringVector msgList;
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
	SvTl::SVTimeStamp l_StartLoading;
	SvTl::SVTimeStamp l_FinishLoad;

	BOOL bOk;

	hr = S_OK;
	hrDestroyed = S_OK;
	bOk = 0;

	while (1)
	{
		// @Note: [SEJ] If DestroyConfig doesn't return S_OK, the MRU is updated incorrectly
		// The file to be loaded, if it was in the MRU, will be removed from the MRU !
		hrDestroyed = DestroyConfig();
		if (S_OK != hrDestroyed )
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

		SVFileNameClass svFileName( PathName );

		//
		// Check if we tried to load the SVC from 
		// Execution path...("C:\RUN\")
		//
		if ( 0 != SvUl_SF::CompareNoCase( SVString(svFileName.GetPathName()), SVFileNameManagerClass::Instance().GetRunPathName() ) )
		{
			// Clean up Execution Directory...
			// Check path, create if necessary and delete contents...
			InitPath( SVString(SVFileNameManagerClass::Instance().GetRunPathName() + "\\").c_str(), TRUE, TRUE );
		}

		try
		{
			unsigned long configVer = 0;

			setConfigFullFileName( PathName );

			SVString FullFileName( getConfigFullFileName() );
			SVRCSetSVCPathName( FullFileName.c_str() );

			while (1)
			{
				SVTreeType XMLTree;
				try 
				{
					hr = SvXml::SVOCMLoadConfiguration( configVer, FullFileName.c_str(), XMLTree );
				}
				catch( const SvStl::MessageContainer& rExp )
				{
					SvStl::MsgTypeEnum  MsgType = SVSVIMStateClass::CheckState(SV_STATE_REMOTE_CMD) ? SvStl::LogOnly : SvStl::LogAndDisplay;
					SvStl::MessageMgrStd Exception( MsgType );
					Exception.setMessage(rExp.getMessage());
					hr = E_FAIL;
					break;
				}
				
				if (hr & SV_ERROR_CONDITION)
				{
					break;
				}
				SVString itemType;
				int errorCode(0);
				hr = SvXml::CheckObsoleteItems( XMLTree, configVer, itemType, errorCode );
				if (hr & SV_ERROR_CONDITION)
				{
					SvStl::MsgTypeEnum  MsgType = SVSVIMStateClass::CheckState(SV_STATE_REMOTE_CMD) ? SvStl::LogOnly : SvStl::LogAndDisplay;
					SvStl::MessageMgrStd Exception( MsgType );
					Exception.setMessage( SVMSG_SVO_76_CONFIGURATION_HAS_OBSOLETE_ITEMS, itemType.c_str(), SvStl::SourceFileParams(StdMessageParams), errorCode );
					break;
				}

				if ( configVer > m_CurrentVersion )
				{
					SVString File, App;

					::SVGetVersionString( App, m_CurrentVersion );
					::SVGetVersionString( File, configVer );
					SVStringVector msgList;
					msgList.push_back( File);
					msgList.push_back( App );

					SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
					INT_PTR result = Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_WrongVersionNumber, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10128, SV_GUID_NULL, MB_YESNO );
					if( IDNO == result )
					{
						hr = E_FAIL;
						break;
					}
				}

				l_StartLoading = SvTl::GetTimeStamp();

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
							SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
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

					if (pConfig->IsConfigurationLoaded())
					{
						// Removes any invalid entries in the output list.
						if (SVMSG_SVO_70_DUPLICATE_DISCRETE_OUTPUT == pConfig->ValidateOutputList())
						{
							SvStl::MessageMgrStd Exception(SvStl::LogAndDisplay);
							Exception.setMessage(SVMSG_SVO_70_DUPLICATE_DISCRETE_OUTPUT, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
						}
						// Upgrade the configuration depending on the version being loaded
						pConfig->UpgradeConfiguration();
					}
				}

				ConstructDocuments( XMLTree );

				GetMainFrame()->OnConfigurationFinishedInitializing();

				l_FinishLoad = SvTl::GetTimeStamp();
				long l_lTime = static_cast<long>(l_FinishLoad - l_StartLoading);

				SVStringVector msgList;
				msgList.push_back(PathName);
				msgList.push_back(SvUl_SF::Format(_T("%d"), l_lTime));
				
				SvStl::MessageMgrStd Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_SVO_29_SVOBSERVER_CONFIG_LOADED, SvStl::Tid_ConfigLoadTime, msgList, SvStl::SourceFileParams(StdMessageParams) );

				break;
			} // while (1)
			
			if (hr & SV_ERROR_CONDITION)
			{
				// If there was an error during configuration loading...
				SVSVIMStateClass::AddState( SV_STATE_AVAILABLE );
				SVSVIMStateClass::RemoveState(SV_STATE_UNAVAILABLE | SV_STATE_LOADING);

				//Use E_FAIL to stop the loading but do not show any error messages
				if( E_FAIL != hr )
				{
					SVStringVector msgList;
					msgList.push_back(SvUl_SF::Format( _T("%d"), hr));
					SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_ConfigurationLoadFailed, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10129 );
				}
				return S_FALSE;
			} // if (hr & 0xc000)

			// ************************************* JMS

			// IsSECModified = FALSE; 03 Dec 1999 - frb. delay until 
			// .ipd parsing is completed.
			SVString FileName = SVFileNameManagerClass::Instance().GetConfigurationPathName();
			if ( FileName.empty() )
			{
				AddToRecentFileList( getConfigFullFileName().c_str() );
			}
			else
			{
				AddToRecentFileList( SVString(FileName + _T("\\") + getConfigFileName()).c_str()  );
			}

			UpdatePPQBar();

		} // try

		catch( CUserException* pUE )
		{
			delete pUE;

			bOk = FALSE;

			SVSVIMStateClass::RemoveState( SV_STATE_LOADING );

			DestroyConfig( false, true );

			setConfigFullFileName( nullptr );

			UpdatePPQBar();

			SVSVIMStateClass::AddState( SV_STATE_AVAILABLE );
			SVSVIMStateClass::RemoveState( SV_STATE_UNAVAILABLE );
		}  // catch

		break;
	} // while (1)

	if ( ! bOk && S_OK == hrDestroyed )
	{
		setConfigFullFileName( nullptr );

		SVSVIMStateClass::AddState( SV_STATE_AVAILABLE );
		SVSVIMStateClass::RemoveState( SV_STATE_UNAVAILABLE | SV_STATE_LOADING );
	}

	if (S_OK == hr)
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
				pDoc = dynamic_cast< SVIODoc* >( pDocTemplate->OpenDocumentFile( nullptr, true ) );   // Make visible

				if( nullptr != pDoc )
				{
					pDoc->SetIOController(&IOController);

					pDoc->SetTitle( DocName );
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

			if( nullptr != pDoc )
			{
				pDoc->SetInspectionID( Inspection.GetUniqueObjectID() );
				pDoc->SetRegressionTestPlayEquationController(Inspection.getRegressionTestPlayConditionController());

				pDoc->SetTitle( DocName );
			}
		}
	}

	return pDoc;
}
#pragma endregion virtual

SVString SVObserverApp::GetConfigurationName() const
{
	SVString ConfigName;

	SVFileNameClass svFileName;

	svFileName.SetFullFileName( getConfigFullFileName().c_str() );

	if( 0 < svFileName.GetFileNameOnly().size() )
	{
		svFileName.SetPathName( SvStl::GlobalPath::Inst().GetRunPath().c_str() );

		ConfigName = svFileName.GetFullFileName();
	}

	return ConfigName;
}

HRESULT SVObserverApp::LoadPackedConfiguration( const SVString& rPackedFileName )
{
	HRESULT l_Status = S_OK;

	SVPackedFile PackedFile;

	if( 0 == _access( rPackedFileName.c_str(), 0 ) )
	{
		SendMessage( m_pMainWnd->m_hWnd, WM_COMMAND, MAKEWPARAM (ID_RC_CLOSE_AND_CLEAN_RUN_DIR, 0), 0 );
	}
	else
	{
		l_Status = E_UNEXPECTED;
	}

	if( S_OK == l_Status )
	{
		if( !( PackedFile.UnPackFiles( rPackedFileName.c_str(), SvStl::GlobalPath::Inst().GetRunPath().c_str() ) ) )
		{
			l_Status = E_UNEXPECTED;
		}
	}

	if( S_OK == l_Status )
	{

		if( PackedFile.getConfigFilePath().empty() || ( _access( PackedFile.getConfigFilePath().c_str(), 0 ) != 0 ) )
		{
			l_Status = E_UNEXPECTED;
		}
		else
		{
			SVRCSetSVCPathName( PackedFile.getConfigFilePath().c_str() );

			l_Status = static_cast<HRESULT>(SendMessage( m_pMainWnd->m_hWnd, SV_LOAD_CONFIGURATION, 0, 0 ));
		}
	}

	return l_Status;
}

HRESULT SVObserverApp::SavePackedConfiguration( const SVString& rPackedFileName )
{
	HRESULT l_Status = S_OK;

	SendMessage( m_pMainWnd->m_hWnd, WM_COMMAND, MAKEWPARAM (ID_RC_SAVE_ALL_AND_GET_CONFIG, 0), 0 );

	SVPackedFile PackedFile;

	if( !( PackedFile.PackFiles( GetConfigurationName().c_str(), rPackedFileName.c_str() ) ) )
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
////////////////////////////////////////////////////////////////////////////////
HRESULT SVObserverApp::CanCloseMainFrame()
{
	return DestroyConfig();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : DestroyConfig
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
HRESULT SVObserverApp::DestroyConfig( BOOL AskForSavingOrClosing /* = TRUE */, 
	BOOL CloseWithoutHint /* = FALSE */ )
{
	BOOL bCancel = FALSE;

	BOOL bOk = ! SVSVIMStateClass::CheckState( SV_STATE_READY | SV_STATE_RUNNING | SV_STATE_TEST );
	HRESULT hr= S_OK;

	if ( ! bOk )
	{
		BOOL bClose = TRUE;

		if ( CloseWithoutHint )
		{
			SVSVIMStateClass::AddState( SV_STATE_CANCELING );
		}
		else
		{
			SVStringVector msgList;
			msgList.push_back(getConfigFileName()); 
			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			INT_PTR result = Msg.setMessage( SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_UserQuestionCloseConfig, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10130, SV_GUID_NULL, MB_YESNO );
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
					SVStringVector msgList;
					msgList.push_back( getConfigFileName() ); 
					SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
					INT_PTR result = Msg.setMessage( SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_UserQuestionSaveChanges, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10131, SV_GUID_NULL, MB_YESNOCANCEL );
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
							if( S_OK == m_svSecurityMgr.SVValidate( SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION ) )
							{
								OnFileSaveConfig();
								ResetAllIPDocModifyFlag(FALSE);

								// Check whether config saving is done.
								// If not, an error or an user cancel
								// command occured!
								bClose = !SVString( getConfigFullFileName() ).empty() &&
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
				bOk = setConfigFullFileName( nullptr ) && bOk;

				wait.Restore();

				m_IniInfoHandler.INIReset();
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
			RootObject *pRoot = nullptr;
			SVObjectManagerClass::Instance().GetRootChildObject( pRoot, SvOl::FqnRoot );
			if(nullptr != pRoot)
			{
				pRoot->destroyConfigurationObject();
			}
		}

		(( SVMainFrame* )m_pMainWnd)->SetStatusInfoText(_T(""));
	}

	if (S_OK == hr )
	{
		hr = bOk ? S_OK : S_FALSE;
	}

	if ( bCancel )
	{
		hr = ERROR_CANCELLED;
	}
	else
	{
		DisconnectCameras();
		ConnectCameras();
	}

	return hr;
}

void SVObserverApp::RemoveUnusedFiles()
{
	SVFileNameManagerClass::Instance().RemoveUnusedFiles( false );
}

SVIODoc* SVObserverApp::GetIODoc() const
{
	SVIODoc* pIODoc( nullptr );

	POSITION pos = GetFirstDocTemplatePosition();
	while( pos && !pIODoc )
	{
		CDocTemplate* pDocTemplate = GetNextDocTemplate( pos );
		if( pDocTemplate )
		{
			POSITION posDoc = pDocTemplate->GetFirstDocPosition();
			while( posDoc && !pIODoc )
			{
				CDocument* newDoc = pDocTemplate->GetNextDoc(posDoc);
				if ( newDoc )
				{
					SVIODoc* pTmpDoc = dynamic_cast <SVIODoc*>(newDoc);
					if( nullptr != pTmpDoc)
					{
						pIODoc = pTmpDoc;
					}
				}
			}
		}
	}
	return pIODoc;
}

BOOL SVObserverApp::Logout( BOOL BForceLogout )
{
	SVSVIMStateClass::RemoveState( SV_STATE_SECURED );

	// We need to deselect any tool that might be set for operator move.
	SVMainFrame* pWndMain = ( SVMainFrame* )GetMainWnd();
	if (pWndMain)
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
////////////////////////////////////////////////////////////////////////////////
bool SVObserverApp::InitPath( LPCTSTR PathName, bool CreateIfDoesNotExist /*= true*/, bool DeleteContents /*= true*/ )
{
	if( nullptr != PathName )
	{
		if( SVCheckPathDir( PathName, CreateIfDoesNotExist ) )
		{
			if( DeleteContents )
			{
				// Get Delete Directory...
				SVString DeleteName;
				SVGetPathInformation( DeleteName, PathName, SVDRIVE | SVDIR );

				// Be sure that this is not the boot, system or windows directory...
				TCHAR dirBuf[ _MAX_PATH ];
				dirBuf[ 0 ] = _TCHAR( '\0' );
				GetWindowsDirectory( dirBuf, _MAX_PATH );
				if( SvUl_SF::CompareNoCase( DeleteName, SVString(dirBuf) ) )
				{
					// Not identical with windows directory...
					GetSystemDirectory( dirBuf, _MAX_PATH );
					if( SvUl_SF::CompareNoCase( DeleteName, SVString(dirBuf) ) )
					{
						// Not identical with system directory...
						if( SvUl_SF::CompareNoCase( DeleteName, SVString( _T("C:\\") ) ) )
						{
							// Not identical with boot directory...
							// Delete contents of this directory...
							DeleteName +=  _T( "*.*" );
							return SVDeleteFiles( DeleteName.c_str(), true );
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

	l_bOk = ( 0 == SvUl_SF::CompareNoCase(m_rInitialInfo.m_ProductName, SvOi::SVO_PRODUCT_KONTRON_X2_GD2A )
		||  0 == SvUl_SF::CompareNoCase(m_rInitialInfo.m_ProductName, SvOi::SVO_PRODUCT_KONTRON_X2_GD4A )
		||  0 == SvUl_SF::CompareNoCase(m_rInitialInfo.m_ProductName, SvOi::SVO_PRODUCT_KONTRON_X2_GD8A )
		||  0 == SvUl_SF::CompareNoCase(m_rInitialInfo.m_ProductName, SvOi::SVO_PRODUCT_KONTRON_X2_GD8A_NONIO ) );

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

	if (0 == SvUl_SF::CompareNoCase(m_rInitialInfo.m_ProductName, SvOi::SVO_PRODUCT_KONTRON_X2_GD1A))
	{
		eType = SVIM_PRODUCT_X2_GD1A;
	}
	else if (0 == SvUl_SF::CompareNoCase(m_rInitialInfo.m_ProductName, SvOi::SVO_PRODUCT_KONTRON_X2_GD2A))
	{
		eType = SVIM_PRODUCT_X2_GD2A;
	}
	else if (0 == SvUl_SF::CompareNoCase(m_rInitialInfo.m_ProductName, SvOi::SVO_PRODUCT_KONTRON_X2_GD4A))
	{
		eType = SVIM_PRODUCT_X2_GD4A;
	}
	else if (0 == SvUl_SF::CompareNoCase(m_rInitialInfo.m_ProductName, SvOi::SVO_PRODUCT_KONTRON_X2_GD8A))
	{
		eType = SVIM_PRODUCT_X2_GD8A;
	}
	else if (0 == SvUl_SF::CompareNoCase(m_rInitialInfo.m_ProductName, SvOi::SVO_PRODUCT_KONTRON_X2_GD8A_NONIO))
	{
		eType = SVIM_PRODUCT_X2_GD8A_NONIO;
	}
	return eType;
}


bool SVObserverApp::IsProductTypeRAID() const
{
	bool bRet = ( SvUl_SF::CompareNoCase(m_rInitialInfo.m_RAIDBoardName, _T("Intel") ) == 0 );
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
		}
	}
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
				SvCmd::InspectionRunOncePtr l_CommandPtr = new SvCmd::InspectionRunOnce( pInspection->GetUniqueObjectID() );
				SVObjectSynchronousCommandTemplate< SvCmd::InspectionRunOncePtr > l_Command( pInspection->GetUniqueObjectID(), l_CommandPtr );

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
	const SVString& rItemText, 
	UINT itemID)
{
	bool bSuccess = false;

	assert(rItemText.size() > 0);
	assert(nullptr != hTargetMenu);

	// first, does the menu item have
	// any required submenus to be found/created?
	size_t Pos = rItemText.find('\\');
	if( SVString::npos != Pos )
	{
		// yes, we need to do a recursive call
		// on a submenu handle and with that sub
		// menu name removed from itemText

		// 1:get the popup menu name
		SVString popupMenuName = SvUl_SF::Left( rItemText, Pos );

		// 2:get the rest of the menu item name
		// minus the delimiting '\' character
		size_t RestSize = rItemText.size()- popupMenuName.size() - 1;
		SVString remainingText = SvUl_SF::Right( rItemText, RestSize );

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
			if (nullptr != hPopupMenu)
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
						reinterpret_cast<UINT_PTR> (hPopupMenu), 
						popupMenuName.c_str()) > 0)
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
				::ModifyMenuA(hTargetMenu, itemIndex, MF_BYPOSITION | MF_STRING, itemID, rItemText.c_str());
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
					rItemText.c_str()) > 0)
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
					rItemText.c_str()) > 0)
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
	const SVString& rItemText)
{
	bool bSuccess = false;

	assert( rItemText.size() > 0 );
	assert( nullptr != hTargetMenu );

	// first, does the menu item have
	// any required submenus to be found/created?
	size_t Pos = rItemText.find('\\');
	if( SVString::npos != Pos )
	{
		// yes, we need to do a recursive call
		// on a submenu handle and with that sub
		// menu name removed from itemText

		// 1:get the popup menu name
		SVString popupMenuName = SvUl_SF::Left( rItemText, Pos );

		// 2:get the rest of the menu item name
		// minus the delimiting '\' character
		size_t RestSize = rItemText.size()- popupMenuName.size() - 1;
		SVString remainingText = SvUl_SF::Right( rItemText, RestSize );

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
				if (rItemText == buffer)
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
				Start();
			}
			catch (const SvStl::MessageContainer& rExp)
			{ //Log exception, (do not display the error because it is called from remote)
				SvStl::MessageMgrStd  Exception( SvStl::LogOnly );
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

	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szFile[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];

	_tsplitpath( SVRCGetSVCPathName(), szDrive, szDir, szFile, szExt );
	SVString Extension = szExt;

	if( 0 == SvUl_SF::CompareNoCase( Extension, SVString( _T(".svx") ) ) )
	{
		l_Status = OpenSVXFile( SVRCGetSVCPathName() );
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

HRESULT SVObserverApp::OnObjectRenamed( const SVString& p_rOldName, const SVGUID& p_rObjectId )
{
	HRESULT l_Status = S_OK;

	SVObjectAppClass* l_pObject = dynamic_cast< SVObjectAppClass* >( SVObjectManagerClass::Instance().GetObject( p_rObjectId ) );

	if( nullptr != l_pObject )
	{
		SVObjectClass* l_pInspect = l_pObject->GetInspection();

		if( nullptr != l_pInspect )
		{
			l_pInspect->OnObjectRenamed(*l_pObject, p_rOldName);
			//@TODO[MZA][7.40][07.10.2016] do we need this command? l_pInspect->BuildValueObjectMap(); 
		}
		else
		{
			l_Status = E_FAIL;
		}

		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		if( nullptr != pConfig )
		{
			pConfig->OnObjectRenamed(*l_pObject, p_rOldName);
		}
		else
		{
			l_Status = E_FAIL;
		}
		SVIODoc* pIODoc = GetIODoc();
		if (pIODoc)
		{
			pIODoc->UpdateAllViews( nullptr );
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
			pIODoc->UpdateAllViews( nullptr );
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
////////////////////////////////////////////////////////////////////////////////
BOOL SVObserverApp::SetStatusText( LPCTSTR PStrStatusText )
{
	// If PStrStatusText is a nullptr, the Main Frame deletes the last status info.
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
////////////////////////////////////////////////////////////////////////////////
void SVObserverApp::UpdatePPQBar()
{
	GetMainFrame()->OnViewPPQBar();
}

const SVString& SVObserverApp::getConfigFileNameOnly() const
{
	return m_ConfigFileName.GetFileNameOnly();
}

const SVString& SVObserverApp::getConfigPathName() const
{
	return m_ConfigFileName.GetPathName();
}

const SVString& SVObserverApp::getConfigFileName() const
{
	return m_ConfigFileName.GetFileName();
}

const SVString& SVObserverApp::getConfigFullFileName() const
{
	return m_ConfigFileName.GetFullFileName();
}

BOOL SVObserverApp::setConfigFullFileName(LPCTSTR csFullFileName, DWORD dwAction)
{
	BOOL bOk = m_ConfigFileName.SetFullFileName( csFullFileName );

	if ( bOk )
	{
		if ( m_ConfigFileName.GetPathName().empty() )
		{
			SVFileNameManagerClass::Instance().SetConfigurationPathName( nullptr );
		}
		else
		{
			if ( 0 == SvUl_SF::CompareNoCase( m_ConfigFileName.GetPathName(), SvStl::GlobalPath::Inst().GetRunPath() ) )
			{
				SVFileNameManagerClass::Instance().SetConfigurationPathName( nullptr );
			}
			else
			{
				bOk = SVFileNameManagerClass::Instance().SetConfigurationPathName( m_ConfigFileName.GetPathName().c_str() );
				// if this returns FALSE, unable to access specified path!
				if ( !bOk )
				{
					ASSERT( dwAction == SAVE_CONFIG ||
						dwAction == LOAD_CONFIG ||
						dwAction == RENAME_CONFIG );

					SVStringVector msgList;
					msgList.push_back(SvStl::MessageData::convertId2AddtionalText(dwAction == LOAD_CONFIG ? SvStl::Tid_Load : SvStl::Tid_Save));
					msgList.push_back(m_ConfigFileName.GetPathName());
					SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_UnableConfig, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10132 );
				}
			}
		}

		if ( bOk )
		{
			switch( dwAction )
			{
			case LOAD_CONFIG:
				{
					bOk = SVFileNameManagerClass::Instance().LoadItem( &m_ConfigFileName );
					break;
				}
			case SAVE_CONFIG:
				{
					bOk = SVFileNameManagerClass::Instance().SaveItem( &m_ConfigFileName );
					break;
				}
			case RENAME_CONFIG:
				{
					bOk = SVFileNameManagerClass::Instance().RenameItem( &m_ConfigFileName );
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
		if ( !SVFileNameManagerClass::Instance().GetConfigurationPathName().empty() )
		{
			AfxGetApp()->WriteProfileString( _T( "Settings" ), _T( "ConfigurationFilePath" ), SVFileNameManagerClass::Instance().GetConfigurationPathName().c_str() );
		}
	}

	return bOk;
}

SVIPDoc* SVObserverApp::GetIPDoc( const SVGUID& rInspectionID ) const
{
	SVIPDoc* pIPDoc(nullptr);
	POSITION pos = GetFirstDocTemplatePosition();
	while( pos && !pIPDoc )
	{
		CDocTemplate* pDocTemplate = GetNextDocTemplate( pos );
		if( pDocTemplate )
		{
			POSITION posDoc = pDocTemplate->GetFirstDocPosition();
			while( posDoc && !pIPDoc )
			{
				SVIPDoc* pDoc = dynamic_cast <SVIPDoc*>(pDocTemplate->GetNextDoc(posDoc));
				if( nullptr != pDoc )
				{
					if (pDoc->GetInspectionID() == rInspectionID)
					{
						pIPDoc = pDoc;
					}
				}
			}
		}
	}
	return pIPDoc;
}

SVIPDoc* SVObserverApp::GetIPDoc( LPCTSTR StrIPDocPathName ) const
{
	// If the function succeeds, it returns a pointer to the found IPDoc.
	// If the function succeeds and the IPDoc is not currently loaded or
	// if the function fails, it returns nullptr !!!
	// !!! The function doesn´t care about lowercase or uppercase strings !!!
	SVIPDoc* pIPDoc(nullptr);

	if( SVSVIMStateClass::CheckState( SV_STATE_READY | SV_STATE_RUNNING ) && StrIPDocPathName )
	{
		CString lowerIPDocName = StrIPDocPathName;
		lowerIPDocName = lowerIPDocName.Left( lowerIPDocName.Find( '.' ) );

		POSITION pos = GetFirstDocTemplatePosition();
		while( pos && !pIPDoc )
		{
			
			CDocTemplate* pDocTemplate = GetNextDocTemplate( pos );
			if( pDocTemplate )
			{
				POSITION posDoc = pDocTemplate->GetFirstDocPosition();
				while( posDoc && !pIPDoc )
				{
					SVIPDoc* pDoc = dynamic_cast <SVIPDoc*>(pDocTemplate->GetNextDoc(posDoc));
					
					if( nullptr != pDoc )
					{
						const CString& strName = pDoc->GetTitle();

						if( 0 == strName.CompareNoCase( lowerIPDocName ) )
						{
							pIPDoc = pDoc;
						}
					}
				}
			}
		}
	}
	return pIPDoc;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : AlreadyExistsIPDocTitle
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
BOOL SVObserverApp::AlreadyExistsIPDocTitle( LPCTSTR StrIPDocTitle )
{
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : ShowConfigurationAssistant
// -----------------------------------------------------------------------------
// .Description : ...
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
		InitPath( SVString( SVFileNameManagerClass::Instance().GetRunPathName() + _T("\\")).c_str(), TRUE, TRUE );

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
	if( _T("10") == m_rInitialInfo.m_IOBoard )
	{
		l_svCapable.SetStrobeInverters( 1 );
		l_svCapable.SetTriggerInverters( 1 );
		l_svCapable.SetTriggerCount( 1 );
	}
	else if( _T("12") == m_rInitialInfo.m_IOBoard )
	{
		l_svCapable.SetStrobeInverters( 1 );
		l_svCapable.SetTriggerInverters( 1 );
		l_svCapable.SetTriggerCount( 3 );
	}
	else if( _T("00") == m_rInitialInfo.m_IOBoard )
	{
		// Get Trigger count from the TriggerDLL (in this case the DigitizerDLL)
		int numTriggers = 0;
		const SvTi::SVIMTypeInfoStruct& info = SvTi::SVHardwareManifest::GetSVIMTypeInfo(eSVIMType);
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
				pIODoc->UpdateAllViews( nullptr );
			}
		}
	}
	SVSVIMStateClass::RemoveState( SV_STATE_EDITING );

	//****RPY - End - added new AppAssistant

	// Call the application assistant dialog...

	if ( bOk )
	{
		CWaitCursor wait;

		SVString NewName = cDlg.GetConfigurationName();
		if( bFileNewConfiguration )
		{
			m_ConfigFileName.SetFileNameOnly( NewName.c_str() );
			m_ConfigFileName.SetExtension( _T(".svx") );

			SVDigitalInputObject* pInput = nullptr;
			SVPPQObject* pPPQ = nullptr;
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

						SVString Name = SvUl_SF::Format( _T("DIO.Input%d"), l+1 );

						pInput = dynamic_cast<SVDigitalInputObject*> (pInputObjectList->GetInputFlyweight(Name, SVDigitalInputObjectType, l));

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
					SVDigitalOutputObject* pOutput(nullptr);
					const int moduleReadyChannel = 15;
					pOutput = dynamic_cast<SVDigitalOutputObject*> (pOutputObjectList->GetOutputFlyweight(SvO::cModuleReady, SVDigitalOutputObjectType, moduleReadyChannel));

					// @HACK:  JAB082212 HACK!!!!!
					if( nullptr != pOutput )
					{
						pOutput->SetChannel(moduleReadyChannel);

						pConfig->GetModuleReady()->m_IOId = pOutput->GetUniqueObjectID();

						SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputIsInverted (moduleReadyChannel, pOutput->IsInverted ());
						SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputIsForced (moduleReadyChannel, pOutput->IsForced ());
						SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputForcedValue (moduleReadyChannel, pOutput->GetForcedValue ());
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
				SVIPDoc* pDoc = GetIPDoc(info.m_inspectionGuid);
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
			SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
			Exception.setMessage( SVMSG_SVO_70_DUPLICATE_DISCRETE_OUTPUT, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
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

				if( nullptr != pImage && nullptr != pImage->GetTool() )
				{
					if( m_pCurrentDocument->GetSelectedToolID() != pImage->GetTool()->GetUniqueObjectID() )
					{
						SVImageInfoClass ImageInfo = pImage->GetImageInfo();

						long l_lBandNumber = 1;

						Monochrome = S_OK == ImageInfo.GetImageProperty( SvOi::SVImagePropertyEnum::SVImagePropertyBandNumber, l_lBandNumber ) && 1 == l_lBandNumber;
					}
					else
					{
						break;
					}
				}
			}
		}
	}
	return Monochrome;
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
	HMENU hMenu = nullptr;
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

	while (nullptr != pMdiChild)
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

	HCURSOR hCursor = nullptr;
	hCursor = ::LoadCursor( nullptr, IDC_WAIT );
	hCursor = ::SetCursor( hCursor );

	try
	{
		hr = DisconnectCameras();
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	try
	{
		::SetCursor( hCursor );

		if ( S_OK == hr )
		{
			if (SvTi::SVHardwareManifest::IsMatroxGige(eProductType))
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

	hCursor = ::LoadCursor( nullptr, IDC_WAIT );
	hCursor = ::SetCursor( hCursor );

	ConnectCameras();

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

HRESULT SVObserverApp::DisconnectCameras()
{
	HRESULT hr = S_OK;

	try
	{
		hr = DisconnectAllCameraBuffers();
	}
	catch (...)
	{
		ASSERT(FALSE);
		hr = E_FAIL;
	}

	SVDigitizerProcessingClass::Instance().DisconnectDevices();

	return hr;
}

HRESULT SVObserverApp::ConnectCameras()
{
	HRESULT hr = S_OK;

	SVDigitizerProcessingClass::Instance().ConnectDevices();

	try
	{
		hr = ConnectCameraBuffers();
	}
	catch (...)
	{
		ASSERT(FALSE);
		hr = E_FAIL;
	}

	return hr;
}

HRESULT SVObserverApp::SendCameraParameters()
{
	HRESULT hr = S_OK;
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	SVStringVector Cameras;
	SVDigitizerProcessingClass::Instance().GetAcquisitionDeviceList( Cameras );
	SVStringVector::const_iterator Iter( Cameras.begin() );
	for( ; Cameras.end() != Iter; ++Iter )
	{
		SVDeviceParamCollection* pDeviceParams = nullptr;

		SVFileNameArrayClass* pDummyFiles = nullptr;
		SVLightReference* pDummyLight = nullptr;
		SVLut* pDummyLut = nullptr;
		
		SVAcquisitionClassPtr pDevice( SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( Iter->c_str() ) );
		if ( nullptr != pDevice && nullptr != pConfig &&
			pConfig->GetAcquisitionDevice(Iter->c_str(), pDummyFiles, pDummyLight, pDummyLut, pDeviceParams ) )
		{
			// Check for the camera file and camera to match.
			if( S_OK == hr )
			{
				HRESULT l_hrTmp = pDevice->IsValidCameraFileParameters( *pDeviceParams );
				if( E_NOTIMPL != l_hrTmp && S_OK != l_hrTmp )
				{
					hr = SV_CAN_GO_ONLINE_FAILURE_ACQUISITION;
				}
			}

			// Send GIGE packet size if was set from hardware.ini
			if (IsMatroxGige() && m_rInitialInfo.m_gigePacketSize != 0)
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
	if ( nullptr != l_pGigePacketSize )
	{
		l_pGigePacketSize->SetValue(_variant_t(m_rInitialInfo.m_gigePacketSize));
	}
	else // add it
	{
		pDeviceParams->SetParameter( DeviceParamGigePacketSize, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( DeviceParamGigePacketSize )) );
		SVLongValueDeviceParam* pParam = pDeviceParams->GetParameter( DeviceParamGigePacketSize ).DerivedValue(pParam);
		ASSERT( pParam );
		pParam->lValue = m_rInitialInfo.m_gigePacketSize;
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
		if( p_bNoSecurity || S_OK == m_svSecurityMgr.SVValidate( SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE, 
			SECURITY_POINT_MODE_MENU_TEST ) )
		{
			OnStop();
			l_bAllowAccess = true;
		}
	}
	else if( p_bNoSecurity || S_OK == m_svSecurityMgr.SVValidate( SECURITY_POINT_MODE_MENU_TEST ) )
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

				SVConfigurationObject* pConfig( nullptr );
				SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

				SVSoftwareTriggerDlg & l_trgrDlg = SVSoftwareTriggerDlg::Instance();
				l_trgrDlg.ShowWindow(SW_HIDE);
				l_trgrDlg.ClearTriggers();
				//If the pointer is a nullptr the lSize is 0
				if( nullptr != pConfig ){ lSize = pConfig->GetPPQCount( ); }
				for( l = 0; l < lSize; l++ )
				{
					pPPQ = pConfig->GetPPQ( l );
					if( nullptr == pPPQ )
					{
						return;
					}
					try
					{
						pPPQ->PrepareGoOnline();
					}
					catch( const SvStl::MessageContainer& rExp )
					{
						//This is the topmost catch for MessageContainer exceptions
						SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
						Exception.setMessage(rExp.getMessage());
						return;
					}
					
					SvTi::SVTriggerObject* pTrigger( nullptr );
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

				for( l = 0; l < lSize; l++ )
				{
					pPPQ = pConfig->GetPPQ( l );
					if( nullptr != pPPQ )
					{
						try
						{
							pPPQ->GoOnline();
						}
						catch( const SvStl::MessageContainer& rExp )
						{
							//This is the topmost catch for MessageContainer exceptions
							SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
							Exception.setMessage(rExp.getMessage());

							//failed, goOffline, before return
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
						}
					}
				}// end for

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

HRESULT SVObserverApp::GetTriggersAndCounts( SVString& rTriggerCounts ) const
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
				SvTi::SVTriggerObject* pTrigger( nullptr );
				//If returns true has valid pointer
				if( pPPQ->GetTrigger( pTrigger ))
				{
					l_hr = S_OK;
					SVString Temp = SvUl_SF::Format( _T("\n%s count-%d"), pTrigger->GetName(), pTrigger->m_lTriggerCount );
					rTriggerCounts += Temp;
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
							if ( nullptr != pTmpDoc )
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
							if ( nullptr != pTmpDoc )
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

							if( nullptr != pTmpDoc )
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

							if( nullptr != pTmpDoc )
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

							if( nullptr != pTmpDoc )
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

	if( nullptr != l_pIODoc )
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

	if( nullptr != l_pIODoc )
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

	if( nullptr != l_pIODoc )
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

HRESULT SVObserverApp::CheckDrive(const SVString& rDrive) const
{
	HRESULT l_hr = S_OK;
	// Check if exists
	if( !PathFileExists( rDrive.c_str() ) )
	{
		SVString Drive = SvUl_SF::Left( rDrive, 1);
		SvUl_SF::MakeUpper( Drive );

		SVStringVector msgList;
		msgList.push_back( Drive );

		SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
		Exception.setMessage( SVMSG_SVO_5051_DRIVEDOESNOTEXIST, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams) );
	}
	TCHAR VolumeName[100];
	TCHAR FileSystemName[32];
	DWORD dwSerialNumber;
	DWORD dwMaxFileNameLength;
	DWORD dwFileSystemFlags;

	if( GetVolumeInformation( rDrive.c_str(),
		VolumeName,
		sizeof( VolumeName),
		&dwSerialNumber,
		&dwMaxFileNameLength,
		&dwFileSystemFlags,
		FileSystemName,
		sizeof(FileSystemName)))
	{
		SVString Name( FileSystemName );
		if( SVString::npos == Name.find( _T("NTFS") ) )
		{
			SVString Drive = SvUl_SF::Left( rDrive, 1);
			SvUl_SF::MakeUpper( Drive );

			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_5052_DRIVENOTNTFSFORMAT, Drive.c_str(), SvStl::SourceFileParams(StdMessageParams) );
		
#ifndef _DEBUG
			ASSERT( false );
#else
#if defined (TRACE_THEM_ALL) || defined (TRACE_SVO)
			::OutputDebugString(Drive.c_str());
#endif
#endif
		}
	}

	return l_hr;
}
#pragma endregion Public Methods

#pragma region Protected Methods
void SVObserverApp::Start()
{
	SVSVIMStateClass::RemoveState( SV_STATE_EDIT );

	if ( SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_STARTING ) )
	{
		if ( SVSVIMStateClass::CheckState( SV_STATE_START_PENDING ) )
		{
			SVSVIMStateClass::RemoveState( SV_STATE_START_PENDING );
		}

		return;
	}

	if ( SVSVIMStateClass::CheckState( SV_STATE_LOADING | SV_STATE_STOPING ) )
	{
		SVSVIMStateClass::AddState( SV_STATE_START_PENDING );

		return;
	}

	UpdateAndGetLogDataManager();

	SvTl::SVTimeStamp l_StartLoading;
	SvTl::SVTimeStamp l_FinishLoad;

	l_StartLoading = SvTl::GetTimeStamp();

	SVObjectManagerClass::Instance().ClearAllIndicator();

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	ASSERT( nullptr != pConfig );

	if ( nullptr == m_pMainWnd || nullptr == pConfig)
	{
		RunAllIPDocuments();

		SvStl::MessageContainer Exception(SVMSG_SVO_54_EMPTY, SvStl::Tid_GoOnlineFailure_InvalidPointerConfig, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_45000);
		throw Exception;
	}

	SVPPQObject* pPPQ( nullptr );

	SVDigitizerProcessingClass::Instance().StoreLastCameraImage();
	long lSize = pConfig->GetPPQCount();

	try
	{
		DisconnectCameras();
		HRESULT Result = ConnectCameras();
		//Buffer allocation error
		if( SVMEE_MATROX_ALLOCATION == Result )
		{
			SvStl::MessageContainer Exception( SVMSG_SVO_NON_PAGED_MEMORY_FULL, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25022_NonPagedMemoryFull );
			throw Exception;
		}
		Result = SendCameraParameters();
		if( Result != S_OK )
		{
			SvStl::MessageContainer Exception(SVMSG_SVO_54_EMPTY, SvStl::Tid_GoOnlineFailure_SendCameraParam, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_45000);
			throw Exception;
		}


		long long MemSize(0);
		long long MemUsed(0);
		SVMatroxSystemInterface::getNonPagedMemory( MemSize, MemUsed );
		if( 0 < MemSize )
		{
			double NonPagedMemUsage = static_cast<double> (MemUsed) / static_cast<double> (MemSize) * 100.0;
			//If more than the normal pecentage used then log a message
			if( cNormalNonPageMemoryUsage < NonPagedMemUsage )
			{
				SVStringVector msgList;
				msgList.push_back( SvUl_SF::Format(_T("%.0f"), cNormalNonPageMemoryUsage ) );
				msgList.push_back( SvUl_SF::Format(_T("%.0f"), NonPagedMemUsage ) );
				SvStl::MessageMgrStd Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_SVO_NON_PAGED_MEMORY_LOW, SvStl::Tid_MoreThanPercentUsed, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25023_NonPagedMemoryLow );
			}
		}

		SVSoftwareTriggerDlg & l_trgrDlg = SVSoftwareTriggerDlg::Instance();
		l_trgrDlg.ShowWindow(SW_HIDE);
		l_trgrDlg.ClearTriggers();

		
		if (SvSml::SharedMemWriter::Instance().HasShares())
		{
			SvSml::ShareEvents::GetInstance().QuiesceSharedMemory();
			
		}
		
		PPQMonitorList ppqMonitorList;
		pConfig->BuildPPQMonitorList(ppqMonitorList); //CREATE MONITORLISTCOPIES 
	
		
		SvSml::SharedMemWriter::Instance().CalculateStoreIds();
		SvSml::SharedMemWriter::Instance().WriteMonitorList();
		

		if (SvSml::SharedMemWriter::Instance().HasShares())
		{
			//clear the shared memory.
			//to clear the shared memory is only real necessary when the Monitorlist has changed
			SvSml::SharedMemWriter::Instance().ClearPPQSharedMemory();
		}

		///In this loop the ImageStores are created 
		for( long l = 0; S_OK == Result && l < lSize; l++ )
		{
			pPPQ =  pConfig->GetPPQ( l );
			//Returns true when pointer valid
			if( nullptr != pPPQ )
			{
				///Set NAK Behavior
				pPPQ->SetNAKMode(m_rInitialInfo.m_NAKMode, m_rInitialInfo.m_NAKParameter);

				// Do this before calling CanGoOnline
				pPPQ->SetMonitorList(ppqMonitorList[pPPQ->GetName()]);

				pPPQ->PrepareGoOnline();

				SvTi::SVTriggerObject* pTrigger( nullptr );
				pPPQ->GetTrigger(pTrigger);
				if ( nullptr != pTrigger && pTrigger->IsSoftwareTrigger())
				{
					l_trgrDlg.AddTrigger( pTrigger );
				}
			}
		}// end for
		
		
		if( pConfig->GetActiveMonitorListCount() > 0) 
		{
			SvSml::SharedMemWriter::Instance().CreateSharedMatroxBuffer();
			SvSml::ShareEvents::GetInstance().SignaltReadyStatus();
		}


		if (l_trgrDlg.HasTriggers())
		{
			l_trgrDlg.SelectTrigger();
		}
	}
	

	catch (const SvStl::MessageContainer& rExp)
	{
		//cleanup goOnline, after fail, before exception leave this method
		SVSVIMStateClass::RemoveState( SV_STATE_START_PENDING );
		RunAllIPDocuments();
		throw rExp;
	}
	catch (std::exception& ex)
	{
		//cleanup goOnline, after fail, before exception leave this method
		SVSVIMStateClass::RemoveState(SV_STATE_START_PENDING);
		RunAllIPDocuments();
		SVStringVector msgList;
		msgList.push_back(ex.what());
		SvStl::MessageMgrStd MesMan(SvStl::LogOnly);
		MesMan.setMessage(SVMSG_SVO_44_SHARED_MEMORY, SvStl::Tid_StdException, msgList, SvStl::SourceFileParams(StdMessageParams));
		MesMan.Throw();
	}

	//SvSml::SharedMemWriter::Instance().RebuildMonitorEntryMap();

	if( SVSVIMStateClass::CheckState( SV_STATE_READY ) )
	{
		try
		{
			for( long l = 0; l < lSize; l++ )
			{
				pPPQ = pConfig->GetPPQ( l );
				if( nullptr != pPPQ )
				{
					pPPQ->GoOnline();
				}
			}// end for
		}
		catch (const SvStl::MessageContainer& rExp)
		{
			//cleanup goOnline, after fail, before exception leave this method
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

			throw rExp;
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
					SvStl::MessageContainer Exception(SVMSG_SVO_54_EMPTY, SvStl::Tid_GoOnlineFailure_RaidBits, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_45000);
					throw Exception;
				}
			}
			else
			{
				if( pConfig->SetRaidErrorBit( false ) != S_OK )
				{
					SvStl::MessageContainer Exception(SVMSG_SVO_54_EMPTY, SvStl::Tid_GoOnlineFailure_RaidBits, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_45000);
					throw Exception;
				}
			}
		}
		else
		{
			if( pConfig->SetRaidErrorBit( true ) != S_OK )
			{
				SvStl::MessageContainer Exception(SVMSG_SVO_54_EMPTY, SvStl::Tid_GoOnlineFailure_RaidBits, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_45000);
				throw Exception;
			}
		}

		if( pConfig->SetModuleReady( true ) != S_OK )
		{
			RunAllIPDocuments();

			SvStl::MessageContainer Exception(SVMSG_SVO_54_EMPTY, SvStl::Tid_GoOnlineFailure_ModuleReadyOutput, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_45000);
			throw Exception;
		}// end if

		SVObjectManagerClass::Instance().SetState( SVObjectManagerClass::ReadOnly );

		SVString TriggerCounts;
		GetTriggersAndCounts( TriggerCounts );
		l_FinishLoad = SvTl::GetTimeStamp();
		long l_lTime = static_cast<long>(l_FinishLoad - l_StartLoading);

		SVStringVector msgList;
		msgList.push_back( TriggerCounts );
		msgList.push_back(SvUl_SF::Format(_T("%d"), l_lTime));

		//add go-online message to the event viewer.
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_SVO_27_SVOBSERVER_GO_ONLINE, SvStl::Tid_GoOnlineTime, msgList, SvStl::SourceFileParams(StdMessageParams) );

		SVSVIMStateClass::RemoveState( SV_STATE_UNAVAILABLE | SV_STATE_STARTING );

		if (SVSoftwareTriggerDlg::Instance().HasTriggers())
		{
			EnableTriggerSettings();
		}
		else
		{
			DisableTriggerSettings();
		}

		SetThreadPriority( THREAD_PRIORITY_BELOW_NORMAL );
	}// end if

	if ( SVSVIMStateClass::CheckState( SV_STATE_STOP_PENDING ) )
	{
		PostMessage( m_pMainWnd->m_hWnd, WM_COMMAND, MAKEWPARAM (ID_STOP, 0), 0);
	}
}


HRESULT SVObserverApp::DisconnectAllCameraBuffers()
{
	HRESULT hr = DisconnectToolsetBuffers();

	if( S_OK == hr )
	{
		hr = SVDigitizerProcessingClass::Instance().DestroyBuffers();
	}

	return hr;
}

HRESULT SVObserverApp::ConnectCameraBuffers()
{
	HRESULT hr = S_OK;

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	SVStringVector Cameras;
	SVDigitizerProcessingClass::Instance().GetAcquisitionDeviceList( Cameras );

	if( nullptr != pConfig )
	{
		SVStringVector::const_iterator Iter( Cameras.begin() );
		for( ; Cameras.end() != Iter; ++Iter )
		{
			SVAcquisitionClassPtr pAcqDevice;

			pAcqDevice = SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( Iter->c_str() );
			if ( nullptr != pAcqDevice )
			{
				// dummy vars
				SVLightReference* pLR;
				SVFileNameArrayClass* pFiles;
				SVLut* pLut;
				SVDeviceParamCollection* pParams;

				if ( pConfig->GetAcquisitionDevice( Iter->c_str(), pFiles, pLR, pLut, pParams ) )
				{
					SVImageInfoClass svImageInfo;
					pAcqDevice->GetImageInfo(&svImageInfo);
					hr = pAcqDevice->CreateBuffers( svImageInfo, TheSVObserverApp.GetSourceImageDepth() );
				}
			}
		}

		if( S_OK == hr  )
		{
			hr = ConnectToolsetBuffers();
		}
	}

	return hr;
}

HRESULT SVObserverApp::InitializeSecurity() 
{
	HMODULE hMessageDll;

	if (hMessageDll = LoadLibraryEx (_T("SVMessage.dll"), nullptr, LOAD_LIBRARY_AS_DATAFILE))
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

	TCHAR szGetBuf[256];
	 
	GetPrivateProfileString(_T("Security"), _T("Security File Path"), SvStl::GlobalPath::Inst().GetBinPath(_T("\\Gatekpr.xml") ).c_str() , szGetBuf, 256, SvStl::GlobalPath::Inst().GetSVIMIniPath());

	if( m_svSecurityMgr.SVLoad(szGetBuf) != S_OK )
	{
		SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_SecurityFileLoadFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10133 );
	}

	FreeLibrary (hMessageDll);
	// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
	Sleep(0);

	return S_OK;
}

//@TODO [Arvid][7.40][25.10.2016] a configuration handler class should be separated out from SVObserverApp
// this class should receive at least the following methods from SVObserverApp:
// fileSaveAsSVX(), DetermineConfigurationSaveName(), SaveConfigurationAndRelatedFiles();

void SVObserverApp::fileSaveAsSVX( SVString SaveAsPathName, bool isAutoSave) 
{
	CWaitCursor wait;

	bool bOk = true;

	SVSVIMStateClass::AddState( SV_STATE_SAVING );
	ResetAllIPDocModifyFlag(FALSE);
	//
	// Is the input parameter for save as path empty?
	//
	if (!isAutoSave)// in an AutoSave the configuration name must not be changed
	{
		if(SaveAsPathName.empty() )
		{
			bOk = DetermineConfigurationSaveName();
		}
		else
		{
			bOk = (setConfigFullFileName( SaveAsPathName.c_str(), FALSE ) == TRUE);
		}
	}
	if ( bOk && !SvUl_SF::CompareNoCase( m_ConfigFileName.GetExtension(), SVString(_T(".svx")) ) )
	{
		SaveConfigurationAndRelatedFiles(isAutoSave);
	}
	else
	{
		SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_WrongPathnameEntered, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10134 );
	}
	SVSVIMStateClass::RemoveState( SV_STATE_SAVING );
}


bool SVObserverApp::DetermineConfigurationSaveName() 
{
	SVFileNameClass svFileName = m_ConfigFileName;

	svFileName.SetFileType( SV_SVX_CONFIGURATION_FILE_TYPE );

	if ( getConfigPathName().empty() ||
		 0 == SvUl_SF::CompareNoCase( getConfigPathName(), SvStl::GlobalPath::Inst().GetRunPath() ) )
	{
		svFileName.SetPathName( AfxGetApp()->GetProfileString( _T( "Settings" ), 
			_T( "ConfigurationFilePath" ), 
			SvStl::GlobalPath::Inst().GetRunPath().c_str() ) );

		if ( 0 == SvUl_SF::CompareNoCase( svFileName.GetPathName(), SvStl::GlobalPath::Inst().GetRunPath().c_str() ) )
		{
			if (  !SVFileNameManagerClass::Instance().GetConfigurationPathName().empty() )
			{
				svFileName.SetPathName( SVFileNameManagerClass::Instance().GetConfigurationPathName().c_str() );
			}
		}
		else
		{
			CString path = svFileName.GetPathName().c_str();
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

	if ( svFileName.SaveFile() )
	{
		if (setConfigFullFileName( svFileName.GetFullFileName().c_str(), RENAME ))
		{
			AfxGetApp()->WriteProfileString( _T( "Settings" ), 
				_T( "ConfigurationFilePath" ), 
				svFileName.GetPathName().c_str() );
		}
		else
		{
			return false;
		}
	}
	else
	{
		SVSVIMStateClass::RemoveState( SV_STATE_SAVING ); // remove the state set at the start of this method
		return false;
	}
	return true;
}


void SVObserverApp::SaveConfigurationAndRelatedFiles(bool isAutoSave) 
{
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	CString csFilePath = m_ConfigFileName.GetFullFileName().c_str();
	if(isAutoSave) 
	{
		ExtrasEngine::Instance().CopyDirectoryToTempDirectory(SvStl::GlobalPath::Inst().GetRunPath().c_str()); //ABX wäre CopyDirectoryToTempDirectory() nicht woanders besser aufgehoben als bei ExtrasEngine?
		ExtrasEngine::Instance().ResetAutoSaveInformation(); //update autosave timestamp

		//in an AutoSave the configuration is not to be saved into its standard directory!
		csFilePath = SvStl::GlobalPath::Inst().GetAutoSaveTempPath(m_ConfigFileName.GetFileName().c_str()).c_str();
	}

	if( nullptr != pConfig ) 
		//@TODO [Arvid][7.40][25.10.2016] should this not be checked much earlier within this function?
		// If we have a null pointer here nothing can be usefully done!
	{
		pConfig->ValidateRemoteMonitorList(); // sanity check
		std::ofstream os;
		os.open( csFilePath );
		if (os.is_open())
		{
			SvXml::SVObjectXMLWriter writer(os);
			pConfig->SaveConfiguration( writer );
			os.close();
		}
	}

	if (isAutoSave) 
	{
		ExtrasEngine::Instance().CopyDirectoryToTempDirectory(SvStl::GlobalPath::Inst().GetRunPath().c_str());
	}
	else 
	{
		//in an autosave some of the of the steps necessary in a normal configuration 
		// save are skipped, 
		// e.g. the configuration name must not be added to the LRU list

		SVFileNameManagerClass::Instance().SaveItem( &m_ConfigFileName );
		SVFileNameManagerClass::Instance().SaveItems();
		SVFileNameManagerClass::Instance().RemoveUnusedFiles();

		SVSVIMStateClass::RemoveState( SV_STATE_MODIFIED );

		SVString ConfigPath(SVFileNameManagerClass::Instance().GetConfigurationPathName());
		if ( ConfigPath.empty() )
		{
			AddToRecentFileList( getConfigFullFileName().c_str() );
		}
		else
		{
			AddToRecentFileList( SVString( ConfigPath + _T("\\") + getConfigFileName()).c_str() );
		}

		( (CMDIFrameWnd*) AfxGetMainWnd() )->OnUpdateFrameTitle(TRUE);
		// Success...
	}
	ExtrasEngine::Instance().ResetAutoSaveInformation(); //update autosave timestamp

}// end if ( !CString( m_ConfigFileName.GetExtension() ).CompareNoCase( ".svx" ) )


/////////////////////////////////////////////////////////////////////////////
//
//  An intermediate method called when user selects a configuration 
//  file from the Most Recent Used (MRU) list under menu File.\
//
//
BOOL SVObserverApp::OpenConfigFileFromMostRecentList(int nID)
{
	ASSERT_VALID(this);
	ASSERT(nullptr != m_pRecentFileList);

	ASSERT(nID >= ID_FILE_MRU_FILE1);
	ASSERT(nID < ID_FILE_MRU_FILE1 + (int)m_pRecentFileList->GetSize());
	int nIndex = nID - ID_FILE_MRU_FILE1;
	ASSERT((*m_pRecentFileList)[nIndex].GetLength() != 0);

#if defined (TRACE_THEM_ALL) || defined (TRACE_SVO)
	TRACE2("MRU: open file (%d) '%s'.\n", (nIndex) + 1,
		(LPCTSTR)(*m_pRecentFileList)[nIndex]);
#endif
	BOOL bResult = false;

	// Check Security to see if save is allowed

	// Open and read the Configuration from the Most Recent Used
	// List under the menu File.
	//
	SVFileNameClass svFileName;

	svFileName.SetFullFileName( (*m_pRecentFileList)[nIndex] );
		
	HRESULT hr = S_FALSE;

	if ( 0 == SvUl_SF::CompareNoCase( svFileName.GetExtension(), SVString( _T(".svx") ) ) )
	{
		hr = OpenSVXFile((*m_pRecentFileList)[nIndex]);
	}

	//if S_OK, check to see if it has any tool errors in the license manager
	if ( S_OK == hr )
	{
		if ( TheSVOLicenseManager().HasToolErrors() )
		{
			TheSVOLicenseManager().ShowLicenseManagerErrors();
		}
	}

	if ( S_FALSE == hr )    // don't do on ERROR_CANCELLED
	{
		m_pRecentFileList->Remove(nIndex);
	}
	bResult = (S_OK == hr );

	return bResult;
}

// get the Document Template for the SVIPDoc
static CMultiDocTemplate* getIPDocTemplate()
{
	CWinApp& rApp = TheSVObserverApp;
	CMultiDocTemplate* pTemplate = nullptr;
	POSITION pos = rApp.GetFirstDocTemplatePosition();
	while  ( !pTemplate  && pos)
	{
		CMultiDocTemplate* pDocTemplate = dynamic_cast<CMultiDocTemplate *>(rApp.GetNextDocTemplate(pos));
		if (pDocTemplate)
		{
			POSITION posDoc = pDocTemplate->GetFirstDocPosition();
			while  (!pTemplate && posDoc)
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

	SVTreeType::SVBranchHandle htiChild = nullptr;

	if( SvXml::SVNavigateTree::GetItemBranch( p_rTree, SvXml::CTAG_IO, nullptr, htiChild ) )
	{
		SVIOController* pIOController( nullptr );

		_variant_t svVariant;

		if( SvXml::SVNavigateTree::GetItem( p_rTree, SvXml::CTAG_UNIQUE_REFERENCE_ID, htiChild, svVariant ) )
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

		if( nullptr != pIOController )
		{
			pIOController->RebuildOutputList();

			SVIODoc* l_pIODoc( TheSVObserverApp.NewSVIODoc( pIOController->GetName(), *pIOController ) );

			if( nullptr != l_pIODoc )
			{
				l_pIODoc->UpdateAllViews( nullptr );
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

	if( S_OK == l_Status && SvXml::SVNavigateTree::GetItemBranch( p_rTree, SvXml::CTAG_INSPECTION, nullptr, htiChild ) )
	{
		SVTreeType::SVBranchHandle htiSubChild( nullptr );

		htiSubChild = p_rTree.getFirstBranch( htiChild );

		while( S_OK == l_Status && nullptr != htiSubChild )
		{
			SVInspectionProcess* pInspection( nullptr );
			SVTreeType::SVBranchHandle htiSVIPDoc = nullptr;
			SVTreeType::SVBranchHandle htiSVInspectionProcess = nullptr;

			SvXml::SVNavigateTree::GetItemBranch( p_rTree, SvXml::CTAG_INSPECTION_PROCESS, htiSubChild, htiSVInspectionProcess );
			SvXml::SVNavigateTree::GetItemBranch( p_rTree, SvXml::CTAG_SVIPDOC, htiSubChild, htiSVIPDoc );

			SVGUID docObjectID;

			SVTreeType::SVBranchHandle htiTempItem = htiSVIPDoc;

			if( nullptr != htiSVInspectionProcess )
			{
				htiTempItem = htiSVInspectionProcess;
			}

			if( nullptr != htiTempItem )
			{
				_variant_t svVariant;

				if( SvXml::SVNavigateTree::GetItem( p_rTree, SvXml::CTAG_UNIQUE_REFERENCE_ID, htiTempItem, svVariant ) )
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

			if( S_OK == l_Status )
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

					if( S_OK == l_Status )
					{
						// Init Document
						if( ! pIPDoc->InitAfterSystemIsDocked() )
						{
							l_Status = E_FAIL;
						}
					}

					if( S_OK == l_Status )
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

		if( nullptr != l_pIOController )
		{
			if( nullptr == l_pIOController->GetIODoc() )
			{
				SVIODoc* l_pIODoc( TheSVObserverApp.NewSVIODoc( l_pIOController->GetName(), *l_pIOController ) );

				if( nullptr != l_pIODoc )
				{
					l_pIODoc->UpdateAllViews( nullptr );
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

		while( S_OK == l_Status && l_Iter != l_Inspections.end() )
		{
			SVInspectionProcess* pInspection( *l_Iter );

			if( nullptr != pInspection )
			{
				if( nullptr == GetIPDoc( pInspection->GetUniqueObjectID() )  )
				{
					SVIPDoc* l_pIPDoc( TheSVObserverApp.NewSVIPDoc( pInspection->GetName(), *pInspection ) );

					if( nullptr != l_pIPDoc )
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

	if (S_OK == l_Status)
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
		m_pMessageWindow = nullptr;
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
	while (nullptr != lpszToken)
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
		SVStringVector msgList;
		msgList.push_back(SvUl_SF::Format( _T("%08X"), l_Status ));
#ifdef _DEBUG
		if( ! l_AppRegister && ! l_AppUnregister )
		{
			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			INT_PTR result = Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_RegisterClassObjectsFailed_Question, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10135, SV_GUID_NULL, MB_YESNO );
			if( IDYES == result )
			{
				return FALSE;
			}
		}
		else

#endif
		{
			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			INT_PTR result = Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_RegisterClassObjectsFailed, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10136 );
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

	CDocTemplate* pDocTemplate = nullptr;
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

bool SVObserverApp::AddSecurityNode( HMODULE hMessageDll, long lId ,LPCTSTR NTGroup, bool bForcePrompt)
{
	LPTSTR pTmp;

	if (FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS |
		FORMAT_MESSAGE_FROM_HMODULE,
		(LPCVOID) hMessageDll, lId,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &pTmp, 0, nullptr))
	{

		SVString Tmp( pTmp );
		Tmp = SvUl_SF::Left( Tmp, Tmp.find('\n') - 1 );
		m_svSecurityMgr.SVAdd( lId, Tmp.c_str(), NTGroup, bForcePrompt);
		LocalFree( pTmp );
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
		(LPTSTR) &pszTmp, 0, nullptr))
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

