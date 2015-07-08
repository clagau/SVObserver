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
#include <iostream>
#include "SVObserver.h"

#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVLibrary/SVPackedFile.h"
#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"
#include "SVOMFCLibrary/SVDeviceParam.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVOMFCLibrary/SVLongValueDeviceParam.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVObserver_i.h"
#include "SVCommand.h"
#include "SVAboutDialogClass.h"
#include "SVConfigurationPrint.h"
#include "SVEnvironmentSettingsDialog.h"
#include "SVFileNameManagerClass.h"
#include "SVImageViewScroll.h"
#include "SVIMServerWrapper.h"
#include "SVIPChildFrm.h"
#include "SVMainFrm.h"
#include "SVDiscreteInputsView.h"
#include "SVMultiDocTemplate.h"
#include "SVStartWindow.h"
#include "SVSVIMStateClass.h"
#include "SVUtilities.h"
#include "SVSystemLibrary/SVCrash.h"
#include "SVLVFastOCR.h"
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
#include "AutoSaver.h"

#include "SVInputObjectList.h"
#include "SVOutputObjectList.h"
#include "SVDigitalInputObject1.h"
#include "SVDigitalOutputObject1.h"

#include "SVConfigurationLibrary/SVOCMGlobals.h"
#include "SVImageProcessingClass.h"

#include "SVOConfigAssistantDlg.h"

#include "SVXMLLibrary/SVNavigateTreeClass.h"

#include "SVConfigurationObject.h"
// BRW - SVImageCompression has been removed for x64.
#ifndef _WIN64
#include "SVImageCompression/SVImageCompressionClass.h"
#endif
#include "ObjectInterfaces/SVUserMessage.h"
#include "SVOMFCLibrary/SVOINIClass.h"
#include "SVOMFCLibrary/SVOIniLoader.h"

#include "SV1394CameraManagerDlg.h"
#include "SVGigeCameraManagerDlg.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVMemoryManager.h"
#include "SVArchiveTool.h"
#include "SVArchiveWritingDlg.h"
#include "SVLibrary/SVWinUtility.h"
#include "SVIOLibrary/SVIOParameterEnum.h"
#include "SoftwareTriggerDlg.h"

#include "SVGlobal.h"

#include "SV1394CameraFileLibrary/SVDCamFactoryRegistrar.h"

#include "SVIOController.h"
#ifndef _WIN64
#include "SVPLCOutputsView.h"
#endif
#include "SVDirectX.h"
#include "SVHardwareManifest.h"
#include "SVTriggerProcessingClass.h"
#include "SVDigitizerProcessingClass.h"
#include "SVSocketLibrary/SVSocketLibrary.h"
#include "SVInputStreamManager.h"
#include "SVOutputStreamManager.h"
#include "SVCommandStreamManager.h"
#include "SVFailStatusStreamManager.h"
#include "SVGetObjectDequeByTypeVisitor.h"
#include "SVSystemLibrary/SVVersionInfo.h"
#include "SVCommandInspectionRunOnce.h"
#include "SVConfigurationTreeWriter.h"
#include "SVOLicenseManager/SVOLicenseManager.h"
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
#include "SVSystemLibrary\LoadDll.h"
#include "SVStatusLibrary\ExceptionManager.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "TextDefinesSvO.h"
#pragma endregion Includes

#pragma region Declarations
using namespace Seidenader::SVSystemLibrary;

#define ID_TRIGGER_SETTINGS 21017

static const int UseLargerArchiveMemoryPool = 16000;
static const int AsyncDefault4GB = 50;
static const int AsyncDefault16GB = 200;
static const int GoOfflineDefault4GB = 300;
static const int GoOfflineDefault16GB = 2000;

LPCTSTR const FRAME_GRABBER_VIPER_DIGITAL                 = (_T("03"));

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

#ifndef _WIN64
	ON_COMMAND(ID_EDIT_EDITPLCOUTPUTS, &SVObserverApp::OnEditPLCOutputs)
	ON_UPDATE_COMMAND_UI(ID_EDIT_EDITPLCOUTPUTS, &SVObserverApp::OnUpdateEditEditPLCOutputs)
#endif
	ON_COMMAND(ID_EDIT_ADD_REMOTE_OUTPUTS, &SVObserverApp::OnEditRemoteOutputs)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADD_REMOTE_OUTPUTS, &SVObserverApp::OnUpdateEditAddRemoteOutputs)
	ON_COMMAND_RANGE(ID_EDIT_PUBLISHEDRESULTS_BASE, ID_EDIT_PUBLISHEDRESULTS_LIMIT, &SVObserverApp::OnEditPublishedResults)

	ON_COMMAND(ID_EDIT_ADD_MONITORLIST, &SVObserverApp::OnEditMonitorList)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADD_MONITORLIST, &SVObserverApp::OnUpdateEditAddMonitorList)

	END_MESSAGE_MAP()

#pragma region Constructor
SVObserverApp::SVObserverApp()
	// BRW - SVImageCompression has been removed for x64.
#ifndef _WIN64
	: m_bImageCompressionStarted( FALSE )
	  , 
#else
	:
#endif
	  m_gigePacketSize( 0 )
	  , m_InputStreamPortNumber( InputStreamPortNumber )
	  , m_OutputStreamPortNumber( OutputStreamPortNumber )
	  , m_FailStatusStreamPortNumber( FailStatusStreamPortNumber )
	  , m_RemoteCommandsPortNumber( RemoteCommandsPortNumber )
	  , m_XMLTree( m_MaterialsTree )
	  , m_MaterialsTree()
{
	free((void*)m_pszHelpFilePath);
	m_pszHelpFilePath = _tcsdup(_T("C:\\SVObserver\\bin\\SVObserver.chm"));
	EnableHtmlHelp();

	::OutputDebugString( _T( "Executing => SVObserverApp::SVObserverApp()\n" ) );

	// Register Dcam Drivers w/Factory
	SVDCamFactoryRegistrar::Register();

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

	//
	// For GetFirstExisitingIPDoc() and GetNextExistingIPDoc()
	//
	m_pFastOcr = nullptr;

	m_ConfigFileName.SetFileType( SV_SVX_CONFIGURATION_FILE_TYPE );

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
			// Logic to remove unused IO Tabs PLC and Remote inputs.
			// PLC Inputs
			SVConfigurationObject* pConfig( nullptr );
			SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

			if( nullptr != pConfig )
			{
#ifndef _WIN64
				if( pConfig->GetPLCCount() == 0 )
				{
					HideIOTab( SVIOPLCOutputsViewID );
				}
#endif
				if( pConfig->GetRemoteOutputGroupCount() == 0)
				{
					HideIOTab( SVRemoteOutputsViewID );
				}
				pConfig->ClearRemoteMonitorList();
			}
			else
			{
#ifndef _WIN64
				HideIOTab( SVIOPLCOutputsViewID );
#endif
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
					AutoSaver::Instance().ResetAutoSaveInformation(); //Arvid: reset autosave timestamp after configuration was loaded
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
	AutoSaver::Instance().ExecuteAutoSaveIfAppropriate(false);//Arvid: before entering test mode: perform autosave

	SetTestMode();
}

void SVObserverApp::OnModeTestBtn() 
{
	OnTestMode();
}

void SVObserverApp::OnRunMode() 
{
	Start();
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

		BOOL bUpdateMenu = m_ShowUpdateFirmwareInMenu;

		cfdDlg.StartLastAutomatically = m_ShouldRunLastEnvironmentAutomatically;
		cfdDlg.m_bUpdateFirmwareCheck = m_ShowUpdateFirmwareInMenu;

		if( cfdDlg.DoModal() == IDOK )
		{
			m_ShouldRunLastEnvironmentAutomatically = cfdDlg.StartLastAutomatically;

			WriteProfileInt( _T( "Settings" ), 
				_T( "Run Last Configuration Automatically" ), 
				m_ShouldRunLastEnvironmentAutomatically ? 1 : 0
				);

			if( bUpdateMenu != m_ShowUpdateFirmwareInMenu )
			{
				SVOINIClass l_svIni;
				CString csIni = "C:\\SVObserver\\bin\\svim.ini";
				if ( m_ShowUpdateFirmwareInMenu )
				{
					l_svIni.SetValue("SVIM Information", "ShowUpdateFirmware", "Y", csIni);
				}
				else
				{
					l_svIni.SetValue("SVIM Information", "ShowUpdateFirmware", "N", csIni);
				}
				SVUtilitiesClass util;
				CWnd *pWindow;
				CMenu *pMenu;
				CString szMenuText;

				pWindow = AfxGetMainWnd();
				pMenu = pWindow->GetMenu();
				szMenuText = _T("&Utilities");

				if ( pMenu = util.FindSubMenuByName( pMenu, szMenuText ) )
				{
					util.LoadMenu (pMenu);
				}

				//rebuild menu
				// @WARNING - This comment indicates the menu should be rebuilt, but there is no code here to do this.
			}
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

	if ( TheSVMemoryManager().ReservedBytes( ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME ) > 0 )
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
	SVException svE;
	SETEXCEPTION1(svE,SVMSG_SVO_28_SVOBSERVER_GO_OFFLINE, l_strTrigCnts);
	svE.LogException(l_strTrigCnts);

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

#ifndef _WIN64
void SVObserverApp::OnUpdateEditEditPLCOutputs( CCmdUI* pCmdUI )
{
	pCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&	
		SVSVIMStateClass::CheckState( SV_STATE_EDIT ) );

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig && pConfig->GetPLCCount() == 0 )
	{
		pCmdUI->SetText( "Add PLC" );
	}
	else
	{
		pCmdUI->SetText( "Edit PLC" );
	}
}
#endif

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
	AutoSaver::Instance().ExecuteAutoSaveIfAppropriate(true);

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

					l_hrStatus = Start();
				}
				else
				{
					CString l_csMessage;

					l_csMessage.Format( _T( "Configuration cannot enter Run.  The "
						"current configuration hardware does not match system hardware.  "
						"The system's Model Number is ( Model # %s %s %s %s ).  "
						"Please verify that the shown model number is correct "
						"and contact your system administrator." ),
						m_csProcessor, m_csFrameGrabber, m_csIOBoard, m_csOptions );

					::AfxMessageBox( l_csMessage, MB_OK | MB_ICONERROR ); 
					SVSVIMStateClass::AddState( l_lPrevState );
				}
			}
			else
			{
				CString l_csItem;
				CString l_csMessage;

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

				l_csMessage.Format( _T( "Hardware configuration error. The SVIM hardware "
					"does not match the Model Number ( Model # %s %s %s %s ).  "
					"%s is either not available or malfunctioning.  "
					"Please verify that the shown model number is correct "
					"and contact your system administrator." ),
					m_csProcessor, m_csFrameGrabber, m_csIOBoard, m_csOptions,
					l_csItem );

				::AfxMessageBox( l_csMessage, MB_OK | MB_ICONERROR ); 
				SVSVIMStateClass::AddState( l_lPrevState );
			}

			if ( SVSVIMStateClass::CheckState( SV_STATE_STOP_PENDING ) )
			{
				PostMessage( m_pMainWnd->m_hWnd, WM_COMMAND, MAKEWPARAM (ID_STOP, 0), 0);
			}
			else if ( l_hrStatus != S_OK )
			{
				CString l_csMessage;

				if ( ( l_hrStatus & SV_CAN_GO_ONLINE_FAILURE_TRIGGER ) == 
					SV_CAN_GO_ONLINE_FAILURE_TRIGGER )
				{
					l_csMessage =  _T( "Configuration cannot enter Run.  There is an "
						"unknown error with a Trigger when the system attempted to enter Run." );
				}
				else if ( ( l_hrStatus & SV_CAN_GO_ONLINE_FAILURE_ACQUISITION ) == 
					SV_CAN_GO_ONLINE_FAILURE_ACQUISITION )
				{
					l_csMessage =  _T( "Configuration cannot enter Run.  There is an "
						"unknown error with a Camera when the system attempted to enter Run." );
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
							l_csMessage.Format("Configuration cannot enter Run.\n Error with \"%s\" : %s",sToolName.c_str(),sToolErrorTxt.c_str());
						}
					}
					if (!bShowToolError)
					{
						l_csMessage =  _T( "Configuration cannot enter Run.  There is an "
							"unknown error with an inspection when the system attempted to enter Run." );
					}
				}
				else if ( ( l_hrStatus & SV_GO_ONLINE_FAILURE_RECYCLE_PRODUCT ) == 
					SV_GO_ONLINE_FAILURE_RECYCLE_PRODUCT )
				{
					l_csMessage =  _T( "Configuration cannot enter Run.  There is an "
						"unknown error with a PPQ when it attempted to recycle a product "
						"when the system was going online." );
				}
				else if ( ( l_hrStatus & SV_GO_ONLINE_FAILURE_INSPECTION ) == 
					SV_GO_ONLINE_FAILURE_INSPECTION )
				{
					l_csMessage =  _T( "Configuration cannot enter Run.  There is an "
						"unknown error with an Inspection when the system was going online." );
				}
				else if ( ( l_hrStatus & SV_GO_ONLINE_FAILURE_ACQUISITION ) == 
					SV_GO_ONLINE_FAILURE_ACQUISITION )
				{
					l_csMessage =  _T( "Configuration cannot enter Run.  There is an "
						"unknown error with a Camera when the system was going online." );
				}
				else if ( ( l_hrStatus & SV_GO_ONLINE_FAILURE_TRIGGER ) == 
					SV_GO_ONLINE_FAILURE_TRIGGER )
				{
					l_csMessage =  _T( "Configuration cannot enter Run.  There is an "
						"unknown error with a Trigger when the system was going online." );
				}
				else if ( ( l_hrStatus & SV_GO_ONLINE_FAILURE_PLC ) == 
					SV_GO_ONLINE_FAILURE_PLC )
				{
					l_csMessage =  _T( "Configuration cannot enter Run.  There is an "
						"unknown error with a PLC." );
				}
				else
				{
					l_csMessage =  _T( "Configuration cannot enter Run.  There is an "
						"unknown error when the system was going online." );
				}
				INT_PTR Res(0);
				SvStl::ExceptionMgr1 Exception(SvStl::LogAndDisplay);

				Res = Exception.setMessage(SVMSG_SVO_54_EMPTY,l_csMessage,StdExceptionParams, SvOi::Err_45000, 0, MB_OK);
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
		AfxMessageBox( _T( "Authorization Failed.\n\nUtility Modification requires 'User Manager' privilege." ), MB_OK );
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
	if( IsColorSVIM() )
	{
		CMDIChildWnd* pMDIChild;
		if( m_pMainWnd && ( pMDIChild = ( ( CMDIFrameWnd* ) m_pMainWnd )->MDIGetActive() ) )
		{
			CDocument* pCurrentDocument;
			if( ( pCurrentDocument = pMDIChild->GetActiveDocument() ) )
			{
				if( ( dynamic_cast< SVIPDoc* >( pCurrentDocument ) )->IsColorInspectionDocument() )
				{
					PCmdUI->Enable( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&
						OkToEdit());
				}
				else
				{
					// Disable
					PCmdUI->Enable( FALSE );
				}
			}
		}
	}
	else
	{
		CMenu *pMenu;
		CWnd* pWindow = AfxGetMainWnd();
		pMenu = pWindow->GetMenu();

		// remove from menu...
		pMenu->RemoveMenu( ID_ADD_COLORTOOL, MF_BYCOMMAND );
	}
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
		AfxMessageBox( _T("Authorization Failed.\n\nUtility Execution requires 'User Manager' privilege."), MB_OK );
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

#ifndef _WIN64
void SVObserverApp::OnEditPLCOutputs()
{
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig )
	{
		pConfig->SetupPLC();
		if( pConfig->GetPLCCount() == 0)
		{
			HidePLCTab();
		}
	}
}
#endif

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
	HRESULT retValue = LoadDll::Instance().getDll( SVOResourceDll, ResourceInstance );
	if (S_OK != retValue || nullptr == ResourceInstance)
	{
		//Because our exception handler (message box) needs the resources, we have to use here the standard message box.
		SvStl::ExceptionMgr1 Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_SVO_53_RESOURCE_DLL_LOADING_FAILED, nullptr, StdExceptionParams, SvOi::Err_10009_LoadOfResourceDllFailed );
		MessageBox(nullptr, SvO::LoadingResourceDllFailed, nullptr, MB_OK | MB_ICONSTOP );
		exit(-SvOi::Err_10009_LoadOfResourceDllFailed);
	}

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
		AfxMessageBox( IDP_OLE_INIT_FAILED );

		return FALSE;
	}

	if( !InitATL() )
	{
		return FALSE;
	}

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

	EnableCrashFilter();

	SetThreadType( APPLICATIONTHREAD );

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

	// JMS - Add Color SVIM Mode Active Flag
	BOOL l_bIsColorSVIM = GetProfileInt(_T( "Settings" ),_T( "Color SVIM Mode Active" ), -1 );
	if ( l_bIsColorSVIM == -1)
	{
		WriteProfileInt(_T( "Settings" ),_T( "Color SVIM Mode Active" ), false );

		l_bIsColorSVIM = false;
	}

	SVObjectManagerClass::Instance().m_bIsColorSVIM = (l_bIsColorSVIM) ? true : false;

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
		CString l_csMessage;

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

		l_csMessage.Format( _T( "Hardware configuration error. The SVIM hardware "
			"does not match the Model Number ( Model # %s %s %s %s ).  "
			"%s is either not available or malfunctioning.  "
			"Please verify that the shown model number is correct "
			"and contact your system administrator." ),
			m_csProcessor, m_csFrameGrabber, m_csIOBoard, m_csOptions,
			l_csItem );

		#ifndef _DEBUG                    // 23 Mar 1999 - frb.
		#ifndef _MINDEBUG
			sWin.ShowWindow( SW_HIDE );
		#endif
		#endif //_DEBUG                  // 23 Mar 1999 - frb.

		::AfxMessageBox( l_csMessage, MB_OK | MB_ICONERROR ); 
	}

	//check to see what licenses are available before setting up any documents
	TheSVOLicenseManager().InitLicenseManager();
	
	// BRW - SVImageCompression has been removed for x64.
#ifndef _WIN64
	SVImageCompressionClass mainCompressionObject (SERVER_COMPRESSION_POOL_SIZE);

	HRESULT hr = mainCompressionObject.CreateResourcePool ();

	m_bImageCompressionStarted = hr == S_OK;
#endif
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
	CString MessageText;
	MessageText.Format(SvO::AmountOfSystemMemoryText,AmountOfRam);
	SvStl::ExceptionMgr1 Exception( SvStl::LogOnly );
	Exception.setMessage(SVMSG_SVO_54_EMPTY,MessageText,StdExceptionParams, SvOi::Memory_Log_45001);

	//if amount of physical memory is around 16 GigE allocate the larger memory pools.
	if (AmountOfRam >= UseLargerArchiveMemoryPool)
	{
		iGoOfflineBufferSize = INI().GetValueInt( _T("Settings"), _T("ArchiveToolGoOfflineBufferSize"), GoOfflineDefault16GB );
		iAsyncBufferSize = INI().GetValueInt( _T("Settings"), _T("ArchiveToolAsyncBufferSize"), AsyncDefault16GB);
		TheSVMemoryManager().CreatePool(ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME, iGoOfflineBufferSize * 1024);
		TheSVMemoryManager().CreatePool(ARCHIVE_TOOL_MEMORY_POOL_ONLINE_ASYNC_NAME, iAsyncBufferSize * 1024);
	}
	else
	{
		iGoOfflineBufferSize = INI().GetValueInt( _T("Settings"), _T("ArchiveToolGoOfflineBufferSize"), GoOfflineDefault4GB );
		iAsyncBufferSize = INI().GetValueInt( _T("Settings"), _T("ArchiveToolAsyncBufferSize"), AsyncDefault4GB );
		TheSVMemoryManager().CreatePool(ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME, iGoOfflineBufferSize * 1024);
		TheSVMemoryManager().CreatePool(ARCHIVE_TOOL_MEMORY_POOL_ONLINE_ASYNC_NAME, iAsyncBufferSize * 1024);
	}
	// Das Hauptfenster ist initialisiert und kann jetzt angezeigt und aktualisiert werden.
#ifdef _DEBUG
	//pMainFrame->ShowWindow(SW_SHOWNORMAL);           // 29 Mar 1999 - frb.
	pMainFrame->ShowWindow( SW_SHOWMAXIMIZED );  //m_nCmdShow
#else
	pMainFrame->ShowWindow( SW_SHOWMAXIMIZED );  //m_nCmdShow
#endif
	pMainFrame->UpdateWindow();

	// Initialize SVIM Server component
	// Check Registry for Enable Remote Commands
	if(  GetProfileInt( _T( "Settings" ), _T( "Enable Remote Commands" ), 0 ) )
	{
		InitSVIMServer();
	}

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
	CString l_strTmp = "\nVersion ";
	if( (m_CurrentVersion & 0xff) == 0xff )
	{
		l_strTmp += SeidenaderVision::SVVersionInfo::GetShortTitleVersion().c_str();
	}
	else
	{
		l_strTmp += SeidenaderVision::SVVersionInfo::GetTitleVersion().c_str();
	}
	SVException svE;
	SETEXCEPTION1( svE, SVMSG_SVO_25_SVOBSERVER_STARTED, l_strTmp );
	svE.LogException( l_strTmp );

	SVDirectX::Instance().Initialize();

	SVSocketLibrary::Init();

	int AutoSaveValue = INI().GetValueInt( _T("Settings"), _T("EnableAutosave"), 0); //Arvid accept a number: non-zero enables

	std::string AutoSaveValueString=INI().GetValueString( _T("Settings"), _T("EnableAutosave"), _T("FALSE")); //Arvid accept a string

	if ( AutoSaveValueString == std::string("TRUE") || AutoSaveValueString == std::string("true"))
	{
		AutoSaveValue = 1;
	}

	AutoSaver::Instance().SetEnabled(AutoSaveValue != 0);
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

	SVSocketRemoteCommandManager::Instance().Startup( m_RemoteCommandsPortNumber );
	SVInputStreamManager::Instance().Startup( m_InputStreamPortNumber );
	SVOutputStreamManager::Instance().Startup( m_OutputStreamPortNumber );
	SVFailStatusStreamManager::Instance().Startup( m_FailStatusStreamPortNumber );

	if ( !TheSVOLicenseManager().HasMatroxLicense() )
	{
		SvStl::ExceptionMgr1 Exception( SvStl::LogAndDisplay );
		Exception.setMessage( SVMSG_SVO_52_NOMATROX_DONGLE, nullptr, StdExceptionParams, SvOi::Err_25013_NoMatroxDongle );
	}

	if ( TheSVOLicenseManager().HasMatroxLicense() && !TheSVOLicenseManager().HasMatroxGigELicense() && IsMatroxGige() )
	{
		AfxMessageBox("Matrox GigE License not found");
	}

	SVVisionProcessorHelper::Instance().Startup();

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

	DestroySVIMServer();
	
	INIClose();

	m_mgrRemoteFonts.Shutdown();

	// BRW - SVImageCompression has been removed for x64.
#ifndef _WIN64
	if ( m_bImageCompressionStarted )
	{
		SVImageCompressionClass mainCompressionObject (SERVER_COMPRESSION_POOL_SIZE);

		mainCompressionObject.DestroyResourcePool ();
	}
#endif
	SVTriggerProcessingClass::Instance().Shutdown();
	SVDigitizerProcessingClass::Instance().Shutdown();
	SVHardwareManifest::Instance().Shutdown();

	SVObjectManagerClass::Instance().Shutdown();
	SVClassRegisterListClass::Instance().Shutdown();

	SVIOConfigurationInterfaceClass::Instance().Shutdown();

	// Shutdown MIL
	SVMatroxApplicationInterface::Shutdown();

	//add message to event viewer - SVObserver stopped
	SVException svE;
	SETEXCEPTION0(svE,SVMSG_SVO_26_SVOBSERVER_STOPPED);
	svE.LogException();
	CloseHandle( m_hEvent );
	DisableCrashFilter();

	#if !defined(_WIN32_WCE) || defined(_CE_DCOM)
		if( m_ATLInited )
		{
			_Module.RevokeClassObjects();
		}
	#endif

	return CWinApp::ExitInstance();
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
				hr = SVOCMLoadConfiguration( m_CurrentVersion, configVer, bStr, m_XMLTree );
				if (hr & 0xc0000000)
				{
					break;
				}

				if ( configVer > m_CurrentVersion )
				{
					CString strText, strFile, strApp;

					::SVGetVersionString( strApp, m_CurrentVersion );
					::SVGetVersionString( strFile, configVer );
					strText.Format( _T( "This configuration was created by SVObserver %s.\n"
						"You are currently running SVObserver %s.\n"
						"This configuration version may be incompatible with\n"
						"the version of SVObserver that you are running.\n"
						"Are you sure you wish to continue ?" ), 
						strFile, strApp );

					if( AfxMessageBox( strText, MB_YESNO ) == IDNO )
					{
						hr = -1801;
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
					hr = pConfig->LoadConfiguration( m_XMLTree );
				}

				if (hr & 0xc0000000)
				{
					break;
				}

				GetMainFrame()->ParseToolsetScripts( m_XMLTree );

				if (nullptr != pConfig)
				{
					hr = pConfig->LoadRemoteMonitorList(m_XMLTree);
					if (hr & SV_ERROR_CONDITION)
					{
						break;
					}
				}

				if (nullptr != pConfig)
				{
					hr = pConfig->LoadGlobalConstants(m_XMLTree);
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
						if( pConfig->ValidateOutputList() == SV_FATAL_SVOBSERVER_2006_DUPLICATE_DISCRETE_OUTPUT )
						{
							::AfxMessageBox( "Invalid Discrete Outputs: Remove all Discrete Outputs and re-add them.", MB_ICONEXCLAMATION );
						}
					}
				}

				ConstructDocuments( m_XMLTree );

				GetMainFrame()->OnConfigurationFinishedInitializing();

#ifndef _WIN64
				if( nullptr != pConfig )
				{
					// Initialize the PLC...
					m_PLCManager.Startup( pConfig );
				}
#endif

				l_FinishLoad = SVClock::GetTimeStamp();
				long l_lTime = static_cast<long>(l_FinishLoad - l_StartLoading);

				SVException svE;
				CString sExcTxt; //(PathName);
				sExcTxt.Format( "%s\nload time %d ms", PathName, l_lTime );
				SETEXCEPTION1(svE,SVMSG_SVO_29_SVOBSERVER_CONFIG_LOADED,sExcTxt);
				svE.LogException(sExcTxt);

				break;
			} // while (1)
			
			SysFreeString (bStr);
			bStr = NULL;

			m_XMLTree.Clear();

			if (hr & 0xc0000000)
			{
				// If there was an error during configuration loading...
				SVSVIMStateClass::AddState( SV_STATE_AVAILABLE );
				SVSVIMStateClass::RemoveState(SV_STATE_UNAVAILABLE | SV_STATE_LOADING);
				m_XMLTree.Clear();

				if (hr != -1801) // -1801 means the user aborted because 
								//  configuration versions were different.
				{
					CString strText;
					strText.Format( _T( "The configuration could not succussfully load.\n"
						"hr = %d.\n"), 
						hr);

					AfxMessageBox( strText, MB_OK );
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

	// Logic to remove unused IO Tabs PLC and Remote inputs.
	// PLC Inputs
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig )
	{
#ifndef _WIN64
		if( pConfig->GetPLCCount() == 0 )
		{
			HideIOTab( SVIOPLCOutputsViewID );
		}
#endif
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
		HideIOTab( SVIOPLCOutputsViewID );
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
			AfxFormatString1( message, IDS_USER_QUESTION_CLOSE_CONFIG, getConfigFileName() ); 
			bClose = AfxMessageBox( message, MB_YESNO | MB_ICONQUESTION ) == IDYES;
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
					AfxFormatString1( message, IDS_USER_QUESTION_SAVE_CHANGES, getConfigFileName() ); 
					switch( AfxMessageBox( message, MB_YESNOCANCEL | MB_ICONQUESTION ) )
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
					}// end switch( AfxMessageBox( message, MB_YESNOCANCEL | MB_ICONQUESTION ) )
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

#ifndef _WIN64
				// PLC stop and close.
				m_PLCManager.Shutdown();
				wait.Restore();
#endif

				SVConfigurationObject* pConfig( nullptr );
				SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

				if( nullptr != pConfig )
				{
					pConfig->ClearRemoteMonitorList();
					bOk = pConfig->DestroyConfiguration();
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

BOOL SVObserverApp::InitSVIMServer()
{
	if(  !IsSVIMServerEnabled() ) // if not already allocated
	{
		// Initialize SVIMServer for SVFocusNT
		if (m_pSVIMServerWrapper = new SVIMServerWrapper)
		{
			// Check if Connection Failed
			if( !m_pSVIMServerWrapper->mSVIMServer.IsStarted() )
			{
				// Allow user to change params
				SetupSVIMServer();

				// Try to create again
				DestroySVIMServer();

				if (m_pSVIMServerWrapper = new SVIMServerWrapper)
				{
					// Still failed ?
					if( !m_pSVIMServerWrapper->mSVIMServer.IsStarted() )
					{
						DestroySVIMServer();
					}
				}
			}
		}
		// Success
		if( m_pSVIMServerWrapper && m_pSVIMServerWrapper->mSVIMServer.IsStarted() )
		{
			m_pSVIMServerWrapper->mSVIMServer.SetSVObserverWnd( m_pMainWnd->m_hWnd );
			m_pSVIMServerWrapper->mSVIMServer.SetGoOffline( GlobalRCGoOffline );
			m_pSVIMServerWrapper->mSVIMServer.SetGoOnline( GlobalRCGoOnline );
			m_pSVIMServerWrapper->mSVIMServer.SetGetConfigurationName( GlobalRCGetConfigurationName );
			m_pSVIMServerWrapper->mSVIMServer.SetGetState( GlobalRCGetState );
			m_pSVIMServerWrapper->mSVIMServer.SetOpenConfiguration( GlobalRCOpenConfiguration );
			m_pSVIMServerWrapper->mSVIMServer.SetSaveConfiguration( GlobalRCSaveConfiguration );
			m_pSVIMServerWrapper->mSVIMServer.SetCloseConfiguration( GlobalRCCloseAndCleanConfiguration );
			return TRUE;
		}
	}
	return FALSE;
}

void SVObserverApp::DestroySVIMServer()
{
	if( m_pSVIMServerWrapper )
	{
		// Should we call some method to shutdown the server ?
		delete m_pSVIMServerWrapper;
		m_pSVIMServerWrapper = NULL;
	}
}

BOOL SVObserverApp::IsSVIMServerEnabled() const
{
	return (m_pSVIMServerWrapper) ? TRUE : FALSE;
}

bool SVObserverApp::IsColorSVIM() const
{
	return SVObjectManagerClass::Instance().m_bIsColorSVIM;
}

bool SVObserverApp::IsMatrox1394() const
{
	bool l_bOk = false;

	if ( m_csDigitizerDLL.IsEmpty() )	// only true Matrox has empty dll name
	{
		l_bOk |= m_csProductName.CompareNoCase( SVO_PRODUCT_MATROX_D1 ) == 0;
		l_bOk |= m_csProductName.CompareNoCase( SVO_PRODUCT_MATROX_D2 ) == 0;
		l_bOk |= m_csProductName.CompareNoCase( SVO_PRODUCT_MATROX_D3 ) == 0;
	}
	return l_bOk;
}

bool SVObserverApp::IsCoreco() const
{
	bool l_bOk = false;

	l_bOk |= m_csProductName.CompareNoCase( SVO_PRODUCT_CORECO_A4_MONO ) == 0;
	l_bOk |= m_csProductName.CompareNoCase( SVO_PRODUCT_CORECO_A2_MONO ) == 0;
	l_bOk |= m_csProductName.CompareNoCase( SVO_PRODUCT_CORECO_A1_RGB ) == 0;

	return l_bOk;
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
	BOOL l_bOk = FALSE;

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	SVIMProductEnum l_eSVIMType = GetSVIMType();
	//If configuration object pointer is nullptr then set the enum to invalid
	SVIMProductEnum l_eProductType = SVIM_PRODUCT_INVALID_TYPE;
	if( nullptr != pConfig ){ l_eProductType = pConfig->GetProductType(); }

	l_bOk = l_eSVIMType == l_eProductType;

	// Exceptions
	if( ! l_bOk )
	{
		switch( l_eSVIMType )
		{
		case SVIM_PRODUCT_FULL:
			{
				l_bOk = l_eProductType == SVIM_PRODUCT_05;

				break;
			}
		case SVIM_PRODUCT_D3:
			{
				l_bOk |= l_eProductType == SVIM_PRODUCT_D3_HUB;
				l_bOk |= l_eProductType == SVIM_PRODUCT_D2;
				l_bOk |= l_eProductType == SVIM_PRODUCT_D2_HUB;
				l_bOk |= l_eProductType == SVIM_PRODUCT_D1;
				l_bOk |= l_eProductType == SVIM_PRODUCT_D1_HUB;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X1;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X1_HUB;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X3;

				break;
			}
		case SVIM_PRODUCT_D3_COLOR:
			{
				l_bOk |= l_eProductType == SVIM_PRODUCT_D2_COLOR;
				l_bOk |= l_eProductType == SVIM_PRODUCT_D1_COLOR;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X1_COLOR;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X3_COLOR;

				break;
			}
		case SVIM_PRODUCT_D3_HUB:
			{
				l_bOk |= l_eProductType == SVIM_PRODUCT_D3;
				l_bOk |= l_eProductType == SVIM_PRODUCT_D2_HUB;
				l_bOk |= l_eProductType == SVIM_PRODUCT_D2;
				l_bOk |= l_eProductType == SVIM_PRODUCT_D1_HUB;
				l_bOk |= l_eProductType == SVIM_PRODUCT_D1;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X1_HUB;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X1;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X3;

				break;
			}
		case SVIM_PRODUCT_D2:
			{
				l_bOk |= l_eProductType == SVIM_PRODUCT_D2_HUB;
				l_bOk |= l_eProductType == SVIM_PRODUCT_D1;
				l_bOk |= l_eProductType == SVIM_PRODUCT_D1_HUB;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X1;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X1_HUB;

				break;
			}
		case SVIM_PRODUCT_D2_COLOR:
			{
				l_bOk |= l_eProductType == SVIM_PRODUCT_D1_COLOR;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X1_COLOR;

				break;
			}
		case SVIM_PRODUCT_D2_HUB:
			{
				l_bOk |= l_eProductType == SVIM_PRODUCT_D2;
				l_bOk |= l_eProductType == SVIM_PRODUCT_D1_HUB;
				l_bOk |= l_eProductType == SVIM_PRODUCT_D1;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X1_HUB;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X1;

				break;
			}
		case SVIM_PRODUCT_D1:
			{
				l_bOk |= l_eProductType == SVIM_PRODUCT_D1_HUB;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X1;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X1_HUB;

				break;
			}
		case SVIM_PRODUCT_D1_COLOR:
			{
				l_bOk |= l_eProductType == SVIM_PRODUCT_X1_COLOR;

				break;
			}
		case SVIM_PRODUCT_D1_HUB:
			{
				l_bOk |= l_eProductType == SVIM_PRODUCT_D1;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X1_HUB;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X1;

				break;
			}
		case SVIM_PRODUCT_X1:
			{
				l_bOk |= l_eProductType == SVIM_PRODUCT_D1_HUB;
				l_bOk |= l_eProductType == SVIM_PRODUCT_D1;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X1_HUB;

				break;
			}
		case SVIM_PRODUCT_X2:
			{
				l_bOk |= l_eProductType == SVIM_PRODUCT_D1_HUB;
				l_bOk |= l_eProductType == SVIM_PRODUCT_D1;
				l_bOk |= l_eProductType == SVIM_PRODUCT_D2;
				l_bOk |= l_eProductType == SVIM_PRODUCT_D3;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X1_HUB;
				break;
			}

		case SVIM_PRODUCT_X3:
			{
				l_bOk |= l_eProductType == SVIM_PRODUCT_D1_HUB;
				l_bOk |= l_eProductType == SVIM_PRODUCT_D1;
				l_bOk |= l_eProductType == SVIM_PRODUCT_D2;
				l_bOk |= l_eProductType == SVIM_PRODUCT_D3;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X1_HUB;

				break;
			}
		case SVIM_PRODUCT_X1_COLOR:
			{
				l_bOk |= l_eProductType == SVIM_PRODUCT_D1_COLOR;

				break;
			}
		case SVIM_PRODUCT_X2_COLOR:
			{
				l_bOk |= l_eProductType == SVIM_PRODUCT_D1_COLOR;
				l_bOk |= l_eProductType == SVIM_PRODUCT_D2_COLOR;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X1_COLOR;
				break;
			}
		case SVIM_PRODUCT_X3_COLOR:
			{
				l_bOk |= l_eProductType == SVIM_PRODUCT_X1_COLOR;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X2_COLOR;
				l_bOk |= l_eProductType == SVIM_PRODUCT_D1_COLOR;
				l_bOk |= l_eProductType == SVIM_PRODUCT_D2_COLOR;
				l_bOk |= l_eProductType == SVIM_PRODUCT_D3_COLOR;
				break;
			}
		case SVIM_PRODUCT_X1_HUB:
			{
				l_bOk |= l_eProductType == SVIM_PRODUCT_D1;
				l_bOk |= l_eProductType == SVIM_PRODUCT_D1_HUB;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X1;

				break;
			}
		case SVIM_PRODUCT_X2_GD1A_COLOR:
		case SVIM_PRODUCT_X2_GD2A_COLOR:
		case SVIM_PRODUCT_X2_GD4A_COLOR:
		case SVIM_PRODUCT_X2_GD8A_COLOR:
		case SVIM_PRODUCT_X2_GD8A_NONIO_COLOR:
			{
				l_bOk |= l_eProductType == SVIM_PRODUCT_X2_GD1A_COLOR;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X2_GD2A_COLOR;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X2_GD4A_COLOR;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X2_GD8A_COLOR;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X2_GD8A_NONIO_COLOR;
				break;
			}
		case SVIM_PRODUCT_X2_GD1A:
		case SVIM_PRODUCT_X2_GD2A:
		case SVIM_PRODUCT_X2_GD4A:
		case SVIM_PRODUCT_X2_GD8A:
		case SVIM_PRODUCT_X2_GD8A_NONIO:
			{
				l_bOk |= l_eProductType == SVIM_PRODUCT_X2_GD1A;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X2_GD2A;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X2_GD4A;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X2_GD8A;
				l_bOk |= l_eProductType == SVIM_PRODUCT_X2_GD8A_NONIO;
				break;
			}
		default:
			{
				// Do nothing.
				break;
			}
		}
	}
	return l_bOk;
}

SVIMProductEnum SVObserverApp::GetSVIMType() const
{
	SVIMProductEnum eType = SVIM_PRODUCT_TYPE_UNKNOWN;
	bool bIsColor = IsColorSVIM();

	if( m_csProductName.CompareNoCase( SVO_PRODUCT_CORECO_A4_MONO ) == 0 )
	{
		eType = SVIM_PRODUCT_FULL;
	}
	else if( m_csProductName.CompareNoCase( SVO_PRODUCT_CORECO_A2_MONO ) == 0 )
	{
		eType = SVIM_PRODUCT_05;
	}
	else if( m_csProductName.CompareNoCase( SVO_PRODUCT_CORECO_A1_RGB ) == 0 )
	{
		if ( bIsColor )
		{
			eType = SVIM_PRODUCT_RGB_COLOR;
		}
		else
		{
			eType = SVIM_PRODUCT_RGB_MONO;
		}
	}
	else if( m_csProductName.CompareNoCase( SVO_PRODUCT_MATROX_D1 ) == 0 )
	{
		if ( bIsColor )
		{
			eType = SVIM_PRODUCT_D1_COLOR;
		}
		else
		{
			eType = SVIM_PRODUCT_D1;
		}
	}
	else if( m_csProductName.CompareNoCase( SVO_PRODUCT_MATROX_D2 ) == 0 )
	{
		if ( bIsColor )
		{
			eType = SVIM_PRODUCT_D2_COLOR;
		}
		else
		{
			eType = SVIM_PRODUCT_D2;
		}
	}
	else if( m_csProductName.CompareNoCase( SVO_PRODUCT_MATROX_D3 ) == 0 )
	{
		if ( bIsColor )
		{
			eType = SVIM_PRODUCT_D3_COLOR;
		}
		else
		{
			eType = SVIM_PRODUCT_D3;
		}
	}
	else if( m_csProductName.CompareNoCase( SVO_PRODUCT_INTEK_D1 ) == 0 )
	{
		if ( bIsColor )
		{
			eType = SVIM_PRODUCT_X1_COLOR;
		}
		else
		{
			eType = SVIM_PRODUCT_X1;
		}
	}
	else if( m_csProductName.CompareNoCase( SVO_PRODUCT_INTEK_D3 ) == 0 )
	{
		if ( bIsColor )
		{
			eType = SVIM_PRODUCT_X3_COLOR;
		}
		else
		{
			eType = SVIM_PRODUCT_X3;
		}
	}
	else if ( m_csProductName.CompareNoCase(SVO_PRODUCT_KONTRON_X2) == 0 )
	{
		if ( bIsColor )
		{
			eType = SVIM_PRODUCT_X2_COLOR;
		}
		else
		{
			eType = SVIM_PRODUCT_X2;
		}
	}
	else if ( m_csProductName.CompareNoCase(SVO_PRODUCT_KONTRON_X2_GD1A) == 0 )
	{
		if ( bIsColor )
		{
			eType = SVIM_PRODUCT_X2_GD1A_COLOR;
		}
		else
		{
			eType = SVIM_PRODUCT_X2_GD1A;
		}
	}
	else if ( m_csProductName.CompareNoCase(SVO_PRODUCT_KONTRON_X2_GD2A) == 0 )
	{
		if ( bIsColor )
		{
			eType = SVIM_PRODUCT_X2_GD2A_COLOR;
		}
		else
		{
			eType = SVIM_PRODUCT_X2_GD2A;
		}
	}
	else if (m_csProductName.CompareNoCase(SVO_PRODUCT_KONTRON_X2_GD4A) == 0 )
	{
		if( bIsColor )
		{
			eType = SVIM_PRODUCT_X2_GD4A_COLOR;
		}
		else
		{
			eType = SVIM_PRODUCT_X2_GD4A;
		}
	}
	else if (m_csProductName.CompareNoCase(SVO_PRODUCT_KONTRON_X2_GD8A) == 0 )
	{
		if( bIsColor )
		{
			eType = SVIM_PRODUCT_X2_GD8A_COLOR;
		}
		else
		{
			eType = SVIM_PRODUCT_X2_GD8A;
		}
	}
	else if (m_csProductName.CompareNoCase(SVO_PRODUCT_KONTRON_X2_GD8A_NONIO) == 0 )
	{
		eType = (( bIsColor ) ? SVIM_PRODUCT_X2_GD8A_NONIO_COLOR : SVIM_PRODUCT_X2_GD8A_NONIO);
	}
	else if( m_csFrameGrabber.CompareNoCase( FRAME_GRABBER_VIPER_DIGITAL ) == 0 )
	{
		eType = SVIM_PRODUCT_DIGITAL;
	}

	return eType;
}

#ifndef _WIN64
CString SVObserverApp::GetPLCDLL() const
{
	return m_csPLCDLL;
}
#endif

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
				SVCommandInspectionRunOncePtr l_CommandPtr = new SVCommandInspectionRunOnce( pInspection->GetUniqueObjectID() );
				SVObjectSynchronousCommandTemplate< SVCommandInspectionRunOncePtr > l_Command( pInspection->GetUniqueObjectID(), l_CommandPtr );

				l_Command.Execute( 120000 );
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
			if( Start() != S_OK )
				l_hr = SVMSG_SVIMCMD_GO_ONLINE_FAILED;
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

					CString sMsg;
					sMsg.Format(_T("Unable to %s configuration %s !"),
						dwAction == LOAD_CONFIG ? _T("load") : _T("save"),
						m_ConfigFileName.GetPathName());
					ASSERT(FALSE);
					AfxMessageBox(sMsg);
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
		if( pConfig->ValidateOutputList() == SV_FATAL_SVOBSERVER_2006_DUPLICATE_DISCRETE_OUTPUT )
		{
			::AfxMessageBox( "Invalid Discrete Outputs: Remove all Discrete Outputs and re-add them.", MB_ICONEXCLAMATION );
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
	BOOL bOk = !IsColorSVIM();

	CMDIChildWnd* pMDIChild;
	if( m_pMainWnd && ( pMDIChild = ( ( CMDIFrameWnd* ) m_pMainWnd )->MDIGetActive() ) )
	{
		m_pCurrentDocument = dynamic_cast< SVIPDoc* >( pMDIChild->GetActiveDocument() );
	}
	else
	{
		return bOk;
	}

	if ( !bOk && m_pCurrentDocument )
	{
		SVObjectTypeInfoStruct info;

		info.ObjectType = SVImageObjectType;
		info.SubType = SVNotSetSubObjectType;

		SVGetObjectDequeByTypeVisitor l_Visitor( info );

		SVObjectManagerClass::Instance().VisitElements( l_Visitor, m_pCurrentDocument->GetInspectionID() );

		SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;

		for( l_Iter = l_Visitor.GetObjects().begin(); !bOk && l_Iter != l_Visitor.GetObjects().end(); ++l_Iter )
		{
			SVImageClass* pImage = dynamic_cast< SVImageClass* >( const_cast< SVObjectClass* >( *l_Iter ) );

			if( pImage != NULL && pImage->GetTool() != NULL )
			{
				if( m_pCurrentDocument->GetSelectedToolID() != pImage->GetTool()->GetUniqueObjectID() )
				{
					SVImageInfoClass ImageInfo = pImage->GetImageInfo();

					long l_lBandNumber = 1;

					bOk = ImageInfo.GetImageProperty( SVImagePropertyBandNumber, l_lBandNumber ) == S_OK && l_lBandNumber == 1;
				}
				else
				{
					break;
				}
			}
		}
	}

	return bOk;
}

void SVObserverApp::SVGetCurrentConfigName(CString &ConfigName) const
{
	ConfigName = getConfigFullFileName();
}

void SVObserverApp::OnRCOpenCurrentSVX()
{
	OpenSVXFile( SVRCGetSVCPathName() );
}

void SVObserverApp::SetupSVIMServer()
{
	if ( m_pSVIMServerWrapper ) { m_pSVIMServerWrapper->mSVIMServer.SetupConnection(); }
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
			else
			{
				SV1394CameraManagerDlg dlg;
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
		SVAcquisitionClassPtr pDevice;
		SVDeviceParamCollection* pDeviceParams = NULL;

		SVFileNameArrayClass* pDummyFiles = NULL;
		SVLightReference* pDummyLight = NULL;
		SVLut* pDummyLut = NULL;

		if ( SVImageProcessingClass::Instance().GetAcquisitionDevice( rCameras.GetAt(i), pDevice ) == S_OK &&
			 nullptr != pConfig &&
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
				pDevice->SetDeviceParameters( *pDeviceParams );	// must be done before CreateBuffers for 1394 (in case CameraFormat changes) - EB 20030714
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
#ifndef _WIN64
			SVPLCOutputsView* l_pPLCView =  dynamic_cast<SVPLCOutputsView*>(pIOView->GetWindow( GW_HWNDFIRST ));
			if( l_pPLCView )
			{
				OnUpdateAllIOViews(); // OnUpdate( NULL, NULL, NULL );
			}
#endif
			l_bRet = true;
		}
	}// end if( PIODoc
	return l_bRet;
}

#ifndef _WIN64
// This function Sets the active Tab to inputs view and 
// Hides the PLC tab
void SVObserverApp::HidePLCTab()
{
	SVMainFrame* pWndMain = ( SVMainFrame* )GetMainWnd();
	SetActiveIOTabView( SVIODiscreteInputsViewID );

	pWndMain->PostMessage( SV_IOVIEW_HIDE_TAB, SVIOPLCOutputsViewID );
}
#endif

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
#ifndef _WIN64
					SVPLCOutputsView* l_pPLCView =  dynamic_cast<SVPLCOutputsView*>(l_VisObj->GetWnd());
					if( l_pPLCView )
					{
						OnUpdateAllIOViews(); // OnUpdate( NULL, NULL, NULL );
					}
#endif
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
		CString l_strTmp;
		CString l_strDrive;
		l_strDrive = p_strDrive;
		l_strDrive.MakeUpper();
		l_strDrive = l_strDrive.Left(1);
		l_strTmp.Format(_T("%s - Drive does not exist"), l_strDrive);
		SVException svE;
		SETEXCEPTION1(svE,SVMSG_SVO_47_EXCEPTION_SYSTEM_SETUP,l_strTmp);
		svE.LogException(l_strTmp);

		AfxMessageBox(l_strTmp);
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
			CString l_strTmp;
			l_strDrive = p_strDrive;
			l_strDrive.MakeUpper();
			l_strDrive = l_strDrive.Left(1);
			l_strTmp.Format(_T("%s - Drive is not NTFS"), l_strDrive);
			SVException svE;
			SETEXCEPTION1(svE,SVMSG_SVO_47_EXCEPTION_SYSTEM_SETUP,l_strTmp);
			svE.LogException(l_strTmp);
#ifndef _DEBUG
			AfxMessageBox( l_strTmp );
#else
			::OutputDebugString(l_strTmp);
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

#ifndef _WIN64
	if( nullptr != pConfig && pConfig->GetPLCCount() > 0 )
	{
		if( m_PLCManager.TestConnections() == false )
		{
			return SV_GO_ONLINE_FAILURE_PLC;
		}
	}
#endif

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

			CString l_strTime;
			l_FinishLoad = SVClock::GetTimeStamp();
			long l_lTime = static_cast<long>(l_FinishLoad - l_StartLoading);
			l_strTime.Format( "\nGoOnline time %d ms", l_lTime );
			l_strTrigCnts += l_strTime;

			//add go-online message to the event viewer.
			SVException svE;
			SETEXCEPTION1(svE,SVMSG_SVO_27_SVOBSERVER_GO_ONLINE, l_strTrigCnts );
			svE.LogException(l_strTrigCnts);

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

	if (l_hrOk == S_OK)
	{
		// copy settings from the SVOIniLoader class for now
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
#ifndef _WIN64
		m_csPLCDLL = l_iniLoader.m_csPLCDLL;
#endif
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

		//get firmware options
		m_csShowUpdateFirmware = l_iniLoader.m_csShowUpdateFirmware;
		m_csFirmwareCommand = l_iniLoader.m_csFirmwareCommand;
		m_csFirmwareWorkingDir = l_iniLoader.m_csFirmwareWorkingDir;
		m_csFirmwareArguments = l_iniLoader.m_csFirmwareArguments;

		m_ShowUpdateFirmwareInMenu = FALSE;
		if ( m_csShowUpdateFirmware.CompareNoCase("Y") == 0 )
		{
			m_ShowUpdateFirmwareInMenu = TRUE;
		}

		for ( int i = 0; i < 4; i++ )
		{
			SVIOConfigurationInterfaceClass::Instance().SetSVIMTriggerValue( i, l_iniLoader.m_csTriggerEdge[i].CompareNoCase( "R" ) == 0 );
			SVIOConfigurationInterfaceClass::Instance().SetSVIMStrobeValue( i, l_iniLoader.m_csStrobeEdge[i].CompareNoCase( "R" ) == 0 );
			SVIOConfigurationInterfaceClass::Instance().SetSVIMStrobeStartFrameActive( i, l_iniLoader.m_csStartFrameType[i].CompareNoCase( "Y" ) == 0 );
		}

		l_hrOk = l_hrOk | LoadDigitalDLL();
		l_hrOk = l_hrOk | LoadTriggerDLL();
		l_hrOk = l_hrOk | LoadSoftwareTriggerDLL();
		l_hrOk = l_hrOk | LoadAcquisitionTriggerDLL();
		l_hrOk = l_hrOk | LoadAcquisitionDLL();
		l_hrOk = l_hrOk | LoadFileAcquisitionDLL();

		if( -1 < l_iniLoader.m_csHardwareOptions.Find( "FastOCR" ) )
		{
#ifdef _WIN64
			TheSVOLicenseManager().SetFastOCRLicense(false);
			if ( m_pFastOcr != NULL )
			{
				delete m_pFastOcr;
				m_pFastOcr = NULL;
			}
#else
			TheSVOLicenseManager().SetFastOCRLicense(true);
			if ( m_pFastOcr == NULL )
			{
				m_pFastOcr = new SVLVFastOCR();
			}
#endif
		}
		else
		{
			if ( m_pFastOcr != NULL )
			{
				delete m_pFastOcr;
				m_pFastOcr = NULL;
			}
		}
	}
	else
	{
		if (l_iniLoader.m_hrOEMFailure != S_OK)
		{
			ASSERT( FALSE );
			::MessageBox(NULL, _T("The model number specified in OEMINFO.INI is invalid."), _T("SVObserver"), MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
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

	if ( m_pFastOcr )
	{
		delete m_pFastOcr;
		m_pFastOcr = NULL;
	}

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

	if ( ! m_csAcquisitionTriggerDLL .IsEmpty() )
	{
		if ( m_svDLLAcquisitionTriggers.Open( m_csAcquisitionTriggerDLL ) != S_OK )
		{
			l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_CAMERATRIGGER;
		}

		if ( SVTriggerProcessingClass::Instance().UpdateTriggerSubsystem( &m_svDLLAcquisitionTriggers ) != S_OK )
		{
			l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_CAMERATRIGGER;
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
		if( m_svDLLDigitizers.Open( m_csDigitizerDLL ) != S_OK )
		{
			l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_ACQUISITION;
		}

		if( SVDigitizerProcessingClass::Instance().UpdateDigitizerSubsystem( &m_svDLLDigitizers ) != S_OK )
		{
			l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_ACQUISITION;
		}

		if( IsCoreco() )
		{
			if( ! SVDigitizerProcessingClass::Instance().IsValidDigitizerSubsystem( _T( "Viper_Quad_1.Dig_0" ) ) &&
				! SVDigitizerProcessingClass::Instance().IsValidDigitizerSubsystem( _T( "Viper_RGB_1.Dig_0" ) ) )
			{
				l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_ACQUISITION;
			}
		}
		else
		{
			l_hrOk = l_hrOk;
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

			if ( S_OK == SVImageProcessingClass::Instance().GetAcquisitionDevice( sDigName, pAcqDevice ) )
			{
				if ( !( pAcqDevice.empty() ) )
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
			}// end if ( S_OK == TheSVObserverApp.mpsvImaging->GetAcquisitionDevice( sDigName, pAcqDevice ) )
		}// end for ( int i=0; i < rCamerasToConnect.GetSize(); i++)

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
		m_svSecurityMgr.SVProtectData( SECURITY_POINT_EXTRAS_MENU_SECURITY_MANAGER ); // Sets Flag that will prevent data from being changed.
	}

	char szGetBuf[256];
	GetPrivateProfileString(_T("Security"), _T("Security File Path"), _T("c:\\SVObserver\\bin\\Gatekpr.xml"), szGetBuf, 256, "c:\\SVObserver\\Bin\\SVIM.ini");

	if( m_svSecurityMgr.SVLoad(szGetBuf) != S_OK )
	{
		AfxMessageBox("Security File Failed to Load\n Using Default Security" );
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
	long hr;

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
		CString csFileName;

		BSTR bStr = NULL;
		unsigned long ulVer = 0;

		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		if( nullptr != pConfig )
		{
			pConfig->ValidateRemoteMonitorList(); // sanity check
			pConfig->SaveConfiguration( m_XMLTree );
		}

		SaveDocuments( m_XMLTree ); //Arvid required DLLs seem to be copied here

		csFileName = m_ConfigFileName.GetFullFileName();

		bStr = csFileName.AllocSysString();

		hr = SVOCMSaveConfiguration( m_CurrentVersion, ulVer, bStr, m_XMLTree ); 		//Arvid here the Configuration seems to be saved to the Run directory (C:\Run)

		if (hr & 0xc0000000)
		{
			CString	csErrorMessage;
			csErrorMessage.Format ("Error: Could not save configuration. Error Nbr: %d", hr);
			AfxMessageBox (csErrorMessage);
		}

		SysFreeString( bStr );

		if (isAutoSave) 
		{
			//Arvid in an autosave some of the of the steps necessary in a normal configuration 
			// save are skipped, 
			// e.g. the configuration name must not be added to the LRU list
			
			AutoSaver::Instance().CopyDirectoryToTempDirectory(Seidenader::SVObserver::RunFolder + CString("\\"));
			AutoSaver::Instance().ResetAutoSaveInformation(); //Arvid: update autosave timestamp

			SVNavigateTreeClass::DeleteAllItems( m_XMLTree ); //Arvid 150625: this appears to be necessary for AutoSave as well
		}
		else 
		{
			svFileManager.SaveItem( &m_ConfigFileName );//Arvid: here the configuration seems to be copied from the Run directory (C:\Run)
			svFileManager.SaveItems();					//Arvid: here other files required for the configuration seem to be copied from the Run directory (C:\Run)
			svFileManager.RemoveUnusedFiles();

			SVSVIMStateClass::RemoveState( SV_STATE_MODIFIED );
			SVNavigateTreeClass::DeleteAllItems( m_XMLTree );

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
				AutoSaver::Instance().ResetAutoSaveInformation(); //Arvid: configuration successfully saved: update autosave timestamp
			}

			( (CMDIFrameWnd*) AfxGetMainWnd() )->OnUpdateFrameTitle(TRUE);
			// Success...
		}
	}// end if ( !CString( m_ConfigFileName.GetExtension() ).CompareNoCase( ".svx" ) )
	else
	{
		AfxMessageBox( IDS_USER_INFORMATION_WRONG_PATHNAME_ENTERED );
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

void SVObserverApp::SaveIPDoc(SVObjectWriter& rWriter, SVIPDoc* pDoc)
{
	rWriter.StartElement(CTAG_SVIPDOC);
	pDoc->GetParameters(rWriter);
	rWriter.EndElement();
}

HRESULT SVObserverApp::SaveDocuments( SVTreeType& p_rTree )
{
	HRESULT l_Status( S_OK );

	SVTreeType::SVBranchHandle htiChild = NULL;

	if( SVNavigateTreeClass::GetItemBranch( p_rTree, CTAG_INSPECTION, NULL, htiChild ) )
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
									SVTreeType::SVBranchHandle hInspection = NULL;

									SVNavigateTreeClass::GetItemBranch( p_rTree, pTmpDoc->GetTitle(), htiChild, hInspection );
									if ( hInspection != NULL )
									{
										SVConfigurationTreeWriter<SVTreeType> writer(p_rTree, hInspection);

										#ifndef USE_OBJECT_SCRIPT
											SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( pTmpDoc->GetInspectionID() );
											SVInspectionProcess* pInspection = dynamic_cast< SVInspectionProcess* >( l_pObject );

											if( nullptr != pInspection )
											{
												// SEJ 101 New Logic
												pInspection->Persist(writer);
											}
										#endif

										SaveIPDoc(writer, pTmpDoc);
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
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVObserverApp::ConstructDocuments( SVTreeType& p_rTree )
{
	HRESULT l_Status( S_OK );

	SVTreeType::SVBranchHandle htiChild = NULL;

	if( SVNavigateTreeClass::GetItemBranch( p_rTree, CTAG_IO, NULL, htiChild ) )
	{
		SVIOController* pIOController( NULL );

		_variant_t svVariant;

		if( SVNavigateTreeClass::GetItem( p_rTree, CTAG_UNIQUE_REFERENCE_ID, htiChild, svVariant ) )
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

	if( l_Status == S_OK && SVNavigateTreeClass::GetItemBranch( p_rTree, CTAG_INSPECTION, NULL, htiChild ) )
	{
		SVTreeType::SVBranchHandle htiSubChild = NULL;

		p_rTree.GetFirstBranch( htiChild, htiSubChild );

		while( l_Status == S_OK && htiSubChild != NULL )
		{
			SVInspectionProcess* pInspection( nullptr );
			SVTreeType::SVBranchHandle htiSVIPDoc = NULL;
			SVTreeType::SVBranchHandle htiSVInspectionProcess = NULL;

			SVNavigateTreeClass::GetItemBranch( p_rTree, CTAG_INSPECTION_PROCESS, htiSubChild, htiSVInspectionProcess );
			SVNavigateTreeClass::GetItemBranch( p_rTree, CTAG_SVIPDOC, htiSubChild, htiSVIPDoc );

			SVGUID docObjectID;

			SVTreeType::SVBranchHandle htiTempItem = htiSVIPDoc;

			if( htiSVInspectionProcess != NULL )
			{
				htiTempItem = htiSVInspectionProcess;
			}

			if( htiTempItem != NULL )
			{
				_variant_t svVariant;

				if( SVNavigateTreeClass::GetItem( p_rTree, CTAG_UNIQUE_REFERENCE_ID, htiTempItem, svVariant ) )
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

			p_rTree.GetNextBranch( htiChild, htiSubChild );
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

// BRW - PLC has been deprecated.
/*BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_SVCommand, CSVCommand)
END_OBJECT_MAP()*/

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
		CString szTemp;

#ifdef _DEBUG
		if( ! l_AppRegister && ! l_AppUnregister )
		{
			szTemp.Format( _T( "(hr=0x%08X), "
				"Unable to register class objects. "
				"COM interface ISVCommand unavailable. "
				"Do you want to exit?"), 
				l_Status );

			if( AfxMessageBox( szTemp, MB_YESNO ) == IDYES )
				return FALSE;
		}
		else
#endif
		{
			szTemp.Format( _T( "(hr=0x%08X), "
				"Unable to register class objects. "
				"COM interface ISVCommand unavailable."),
				l_Status );

			AfxMessageBox(szTemp);

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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVObserver.cpp_v  $
 * 
 *    Rev 1.50   23 Feb 2015 03:38:38   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  984
 * SCR Title:  Exception Display Class with Exception Manager Template SVO-524
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed exception manager singelton
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.49   18 Feb 2015 11:10:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  975
 * SCR Title:  Add Fail Status Stream (SVO-354)
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added enum for default port numbers.
 * Revised OnStop method to call SVFailStatusStreamManager::RemovePPQObservers.
 * Added logic to initialize the failstatusstream port.
 * Added logic to start and stop the failstatusstream socket.
 * Revised DestroyConfig to clear the fail status stream(s).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.48   18 Feb 2015 02:47:40   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  984
 * SCR Title:  Exception Display Class with Exception Manager Template SVO-524
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added no Matrox Dongle exception error
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.47   17 Dec 2014 07:08:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  941
 * SCR Title:  Update SVObserver Version Number for the 7.10 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added Thread Manager Enable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.46   10 Dec 2014 16:43:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added a disable method to the SVThreadManager
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.45   10 Dec 2014 12:02:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  908
 * SCR Title:  Remove option for Operator Move (SVO 101)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed Edit Move definitions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.44   09 Dec 2014 10:12:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  908
 * SCR Title:  Remove option for Operator Move (SVO 101)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Remove edit move state and supporting functions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.43   01 Dec 2014 13:05:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add ThreadInfo dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.42   23 Oct 2014 09:51:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  957
 * SCR Title:  Remove FastOCR functionality for 64-bit version of SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed INILoad to add a #ifdef _WIN64 to turn off support for FastOCR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.41   02 Sep 2014 12:17:50   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  921
 * SCR Title:  Add more complete zoom functionality. (runpage)
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Changed methods SetTestMode, Start, and OnStopAll to call RefreshAllSplitters.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.40   20 Aug 2014 18:21:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised SVSharedMemorySingleton::Destroy to be static
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   14 Aug 2014 18:29:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised ExitInstance to cal lSVSharedMemorySingleton::Destroy.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   14 Aug 2014 16:10:56   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  921
 * SCR Title:  Add more complete zoom functionality. (runpage)
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   new zoom toolbar in testmode
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   17 Jul 2014 19:38:34   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed namespaces and code review changes
 * Loads resource dll
 * Replace ResultPicker Dialog with Object Selector Dialog
 * Changed methods: OnEditPublishedResults, InitInstance
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   07 Jul 2014 16:53:16   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Split the Security access from Image Online Display to Image Display Update and Result Display Update
 * Method changed InitializeSecurity
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   01 Jul 2014 15:33:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed OnFileNewConfig - when asking if you want to close the exiting configuration, if "No" return from the function so that the monitor list do not get removed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   25 Jun 2014 11:55:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  910
 * SCR Title:  Fix memory leaks for Software Trigger Dialog and Parser Progress Dialog
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed ::Start to use the local copy of the SVSoftwareTriggerDlg intead of doing another Instance later on in the method.  
 * 
 * In OnStopAll, added ClearTriggers.  
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   19 Jun 2014 17:38:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised to send the active monitor list to the PPQ(s) .
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   02 Jun 2014 10:07:40   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Replace the get and set variable methods to the static Environment methods getEnvironmentValue and setEnvironmentValue.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   20 May 2014 10:20:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  904
 * SCR Title:  Update SVObserver to use HtmlHelp instead of WinHelp
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Enabled HtmlHelp in the constructor.  Removed method WinHelp
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   15 May 2014 13:34:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised WinHelp method override to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   28 Apr 2014 14:29:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised Start method to set the MonitorList for the PPQ(s).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   28 Apr 2014 13:38:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  873
 * SCR Title:  Fix inconsistant GUI labels and functionality on IO pages
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Update_Command_UI_Range for the Published Results.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   24 Apr 2014 14:13:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised OnEditPublishedResults to check for valid poniter to IODoc.
 * Revised RenameObject to check for valid poniter to IODoc.
 * Revised RebuildOutputList to check for valid poniter to IODoc.
 * Revised ShowConfigurationAssistant to check for valid poniter to IODoc.
 * Revised ResetAllIPDocModifyFlag to check for valid poniter 
 * to IODoc.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   22 Apr 2014 13:13:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added validation logic for RemoteMonitorList
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   17 Apr 2014 17:00:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added functionality for the Monitor List View
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   17 Mar 2014 15:26:48   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed the environment variables ModelNumber SerialNumber and Windows.WinKey to be inserted into the OM using the BasicValueObject.
 * Removed the methods GetWindowItems and GetEnvironmentItems now using OM for SVRC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   07 Mar 2014 18:20:46   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed image compression comments.
 *   Corrected spelling of ID_TRRIGER_SETTINGS.
 *   Removed THIS_FILE.
 *   Removed empty method DisplayAddMenu.
 *   Made methods const.
 *   Corrected spelling of availiable in message text.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   28 Feb 2014 07:57:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  887
 * SCR Title:  Fix bug where saving can cause a crash (e117)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added check to the return of SetConfigFullFileName in the function SaveAsSVX.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   27 Feb 2014 14:05:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Un-commented Image Compression for 32 bit code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   13 Feb 2014 12:37:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  873
 * SCR Title:  Fix inconsistant GUI labels and functionality on IO pages
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fix check in mode menu - stop to not display a check if no configuration loaded.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   04 Feb 2014 15:30:18   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  880
 * SCR Title:  Remove .SEC
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Added regions.
 * Removed unused includes for SVIOChildFrm.h, SVObjectClass.h, SVAcquisitionClass.h, SVConfigurationTags.h, SVLut.h, SVMessage.h, and SVPLCAddRemoveDlg.h.
 * Added includes for SVDeviceParam.h, RemoteCommand.h, SVIOBoardCapabilities.h, SVInspectionProcess.h, and SVPPQObject.h.
 * Deleted unused enums.
 * Moved the Global functions to RemoteCommand.cpp
 * Reordered methods to have the same order as the header file.
 * Renamed member variables as listed in SVObserver.h check in notes.
 * Set member variables to private and encapsulated with methods if needed.
 * Remove methods as listed in SVObserver.h check in notes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   31 Jan 2014 17:16:32   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed to follow guidelines more closely.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   09 Dec 2013 07:51:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  873
 * SCR Title:  Fix inconsistant GUI labels and functionality on IO pages
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added dynamic menu option in IO Page - Edit menu - Edit Published Results for each inspection.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   30 Oct 2013 10:45:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added #ifndef _WIN64 to prevent depricated PLC code from compiling in 64bit.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   02 Oct 2013 12:05:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platforms.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   23 Sep 2013 12:28:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  853
 * SCR Title:  Fix state after a cancel from new configuration
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified OnFileNewSEC to check the return code form ShowConfigurationAssistant
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   15 Aug 2013 06:17:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added refresh message to update the status bar for mode changes from SVRemoteControl.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   07 Aug 2013 13:33:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised due to change in RebuldInputList in SVPPQObject
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   01 Aug 2013 12:44:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  843
 * SCR Title:  Add check to SVObserver startup to check V-Drive.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified check drive to have a single button message box when there is an error. just Ok to continue.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   29 Jul 2013 11:40:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  843
 * SCR Title:  Add check to SVObserver startup to check V-Drive.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added const ref and const to CheckDrive function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   26 Jul 2013 09:47:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  843
 * SCR Title:  Add check to SVObserver startup to check V-Drive.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added V:\\ drive check for SVRemote Control.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   05 Jul 2013 09:11:12   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 020.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.815   03 Jun 2013 09:41:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  840
 * SCR Title:  Fix the GigE camera to camera file validation
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added check to validate camera to camera file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   18 Jun 2013 19:23:12   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 019.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.814   31 May 2013 07:52:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  839
 * SCR Title:  Add Last Modified Notification to Remote Socket Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated initialize instance and exit instance to include the new startup and shutdown functionality for the vision processor helper.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   11 Jun 2013 15:26:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.813   21 May 2013 13:04:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added member variables and methods to allow the Windows Key, Model Number, and Serial Number to be retrieved remotely.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 May 2013 16:16:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.812   29 Apr 2013 14:22:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.811   24 Apr 2013 14:28:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  826
 * SCR Title:  Fix Memory Leaks in I/O Adjustment Dialogs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed call to DestroyMessageWindow in destructor because it is already in the exist instance and it was found to cause a crash in rare occasions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   06 May 2013 20:06:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.810   16 Apr 2013 15:35:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 14:38:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 009.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.809   10 Apr 2013 14:52:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   when coming out of the configuration assistant display license errors for imported inspections
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:42:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.808   27 Mar 2013 14:39:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed ShouldShowInpsectionToolbar variable
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.807   25 Mar 2013 13:36:42   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed UpdateInspectionToolBar
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.806   25 Mar 2013 12:43:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed matrox dongle message, made call to clear license errors when loading/closing configurations, after loading configuration it checks for tool errors and displays errors.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.805   05 Mar 2013 16:53:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised ShowConfigurationAssistant method to support the importing of inspections
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.804   05 Mar 2013 08:46:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed error string text
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.803   27 Feb 2013 11:23:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   InitLicenseManager in InitInstance.  Aslo in the onUpdate methods put check to grey out if there is no Matrox License
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.802   11 Feb 2013 14:19:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new framework for the remote socket interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.801   21 Jan 2013 11:46:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added support for acquisition triggering
 * Added support for conditional outputs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.800   16 Jan 2013 16:08:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Migrating branch code into primary code base.  
 * 
 * Updated use of color flag and hardware menifest object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.799   10 Jan 2013 15:08:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Merged 6.01 branch code into main development code base.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.798   13 Dec 2012 10:43:52   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   (IsDigitalSVIM) Added case for SVIM_PRODUCT_X2_GD1A, SVIM_PRODUCT_X2_GD1A_COLOR.
 *   (IsMatroxGige) Added SVIM_PRODUCT_X2_GD1A, SVIM_PRODUCT_X2_GD1A_COLOR.
 *   (CheckSVIMType) Added cases for SVIM_PRODUCT_X2_GD1A, SVIM_PRODUCT_X2_GD1A_COLOR.
 *   (IsProductTypeRAID) Added SVIM_PRODUCT_X2_GD1A, SVIM_PRODUCT_X2_GD1A_COLOR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.797   11 Dec 2012 18:48:06   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   (GetSVIMType) Added SVO_PRODUCT_KONTRON_X2_GD1A case.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.796   07 Dec 2012 10:36:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new method OnUpdateAddShiftTool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.795   11 Oct 2012 10:59:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information based on the navigate tree class moving to the XML library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.794   04 Oct 2012 11:25:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Replaced the CTreeCtrl with MaterialsTree.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.793   28 Sep 2012 14:49:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated saving operation to use the object writer instead of the XML tree.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.792   28 Sep 2012 14:38:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver application tree storage member variable name.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.791   27 Sep 2012 18:36:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use generic tree fucntionality to process old server document functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.790   25 Sep 2012 15:21:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.789   18 Sep 2012 18:35:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  781
 * SCR Title:  Update SVObserver Version Number for Esok Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the creation and management for the version information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.788   18 Sep 2012 18:25:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.787   07 Sep 2012 11:43:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  783
 * SCR Title:  Update SVObserver to Validate PLC Sub-system Before Entering a Running Mode
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added test connection for PLC to prevent going online if the plc is configured but not present.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.786   06 Sep 2012 10:49:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Save Documents and Construct Documents method to handle the change of location of the Inspection Process data attributes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.785   04 Sep 2012 15:27:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Navigate Tree class to not force the programmer to create an instance of the class to use it.  Converted all functionality to static.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.784   30 Aug 2012 09:42:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  786
 * SCR Title:  Fix Problems with IEEE 1394 Light Reference and LUT Camera Parameters
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the Send Camera Parameters in the SVObserver Application object to validate variables before using them.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.783.1.4   08 Jan 2013 16:07:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated code to fix issues with selected tool and add tool filtering.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.783.1.3   04 Jan 2013 13:05:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated SVObserverApp::IsMonochromeImageAvailiable method to fix the problems when a monochrome camera is connected to a color SVIM.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.783.1.2   12 Dec 2012 19:32:40   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   (IsDigitalSVIM) Added case for SVIM_PRODUCT_X2_GD1A, SVIM_PRODUCT_X2_GD1A_COLOR.
 *   (IsMatroxGige) Added SVIM_PRODUCT_X2_GD1A, SVIM_PRODUCT_X2_GD1A_COLOR.
 *   (CheckSVIMType) Added cases for SVIM_PRODUCT_X2_GD1A, SVIM_PRODUCT_X2_GD1A_COLOR.
 *   (IsProductTypeRAID) Added SVIM_PRODUCT_X2_GD1A, SVIM_PRODUCT_X2_GD1A_COLOR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.783.1.1   11 Dec 2012 17:00:06   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   (GetSVIMType) Added SVO_PRODUCT_KONTRON_X2_GD1A case.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.783.1.0   05 Dec 2012 11:54:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new method OnUpdateAddShiftTool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.783   23 Aug 2012 10:44:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Jim's change to fix Module Ready on new configurations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.782   15 Aug 2012 14:41:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new Singleton version of the SVIOConfigurationInterfaceClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.781   14 Aug 2012 15:56:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated output list functionality to only allow unique elements to the output list and notify user when duplicates are detected.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.780   13 Aug 2012 14:28:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to change the functionality associated with the Input Object List to use a new method to get the input from the input list.  If the input does not exist in the input list, a NULL pointer is returned.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.779   13 Aug 2012 11:39:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  773
 * SCR Title:  Update X2 GigE Model
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed X2B-GD8A model/configuration information from application.  Converted source code to use X2-GD8A instead of X2B-GD8A.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.778   09 Aug 2012 16:07:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues when enabling and disabling menu options depending on the application state.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.777   01 Aug 2012 12:25:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new command functionality to allow for synchronous command execution through the inspection thread.  This will fix problems with race conditions with the inspection process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.776   01 Aug 2012 08:46:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  777
 * SCR Title:  Change default drive from E to D
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Change method InitInstance to use the "D" drive instead of the "E" drive
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.775   30 Jul 2012 13:05:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the parameter change for the visitor fucntionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.774   30 Jul 2012 12:11:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  770
 * SCR Title:  Upgrade SVObserver to 6.00 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new functionality to use the resource version information instead of data found in SVObserver.cpp.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.773   27 Jul 2012 09:28:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number to 6.00 Beta 073.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.772   27 Jul 2012 08:57:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to handle changes in the visitor functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.771   25 Jul 2012 15:01:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version 6.00 Beta 072.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.770   23 Jul 2012 08:39:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  770
 * SCR Title:  Upgrade SVObserver to 6.00 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version to 6.00 Beta 070.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.769   18 Jul 2012 14:30:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number to 6.00 Beta 069.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.768   16 Jul 2012 13:05:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver to version 6.00 Beta 068.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.767   12 Jul 2012 15:30:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version to 6.00 Beta 067.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.766   10 Jul 2012 12:20:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number to 6.00 Beta 66.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.765   09 Jul 2012 14:50:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  770
 * SCR Title:  Upgrade SVObserver to 6.00 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Change version to 6.00 Beta 065
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.764   09 Jul 2012 14:08:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the include for SVIM State.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.763   09 Jul 2012 13:45:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  774
 * SCR Title:  Update SVObserver to include the new SVIM X2B Product
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added X2_GD4A option for compatibility with this legacy product type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.762   02 Jul 2012 18:05:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  770
 * SCR Title:  Upgrade SVObserver to 6.00 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number to 6.00 Beta 064.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.761   02 Jul 2012 17:23:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.760   02 Jul 2012 15:09:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  770
 * SCR Title:  Upgrade SVObserver to 6.00 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number to 6.00 Beta 032.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.759   02 Jul 2012 14:23:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  774
 * SCR Title:  Update SVObserver to include the new SVIM X2B Product
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new configuration type SVIM X2B-GD8A.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.758   02 Jul 2012 13:24:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  773
 * SCR Title:  Update X2 GigE Model
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed configuration type SVIM X2-GD4A.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.757   02 Jul 2012 12:55:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  773
 * SCR Title:  Update X2 GigE Model
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new configuration type SVIM X2-GD8A.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.756   02 Jul 2012 11:42:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  770
 * SCR Title:  Upgrade SVObserver to 6.00 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number to 6.00 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.755   02 Jul 2012 11:32:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated the startup and shutdown for the Remote Output socket thread based on when a configuration is loaded.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.754   02 Jul 2012 11:26:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated the startup and shutdown for the Remote Input Tool socket thread based on when a configuration is loaded.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.753   28 Jun 2012 16:17:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removing Shared Memory for the webserver from the SVObserver application.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.752   25 Jun 2012 17:06:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version to 5.50 Beta 076
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.751   25 Jun 2012 12:37:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  739
 * SCR Title:  Upgrade SVObserver for Devario Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated beta to 550 Beta 75
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.750   20 Jun 2012 14:10:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version to 5.50 Beta 074.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.749   19 Jun 2012 14:57:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.50 Beta 073.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.748   19 Jun 2012 13:34:42   jspila
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
 *    Rev 3.747   14 Jun 2012 11:56:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  739
 * SCR Title:  Upgrade SVObserver for Devario Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed to 550 Beta 70.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.746   09 May 2012 08:18:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  739
 * SCR Title:  Upgrade SVObserver for Devario Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 5.50 Release
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.745   02 May 2012 11:26:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  739
 * SCR Title:  Upgrade SVObserver for Devario Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Change beta version to 69.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.744   17 Apr 2012 11:53:02   tBair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  739
 * SCR Title:  Upgrade SVObserver for Devario Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Change version to 550 Beta 68.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.743   16 Apr 2012 10:12:42   tBair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  739
 * SCR Title:  Upgrade SVObserver for Devario Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 5.50 Beta 67.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.742   12 Apr 2012 08:59:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   changed to beta 66
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.741   10 Apr 2012 10:37:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  761
 * SCR Title:  Fix issue with Clip on Adjust LUT Dialog using GigE cameras
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed to 5.50 beta 65
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.740   03 Apr 2012 11:39:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  739
 * SCR Title:  Upgrade SVObserver for Devario Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 5.50 Beta 64
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.739   22 Mar 2012 12:39:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  739
 * SCR Title:  Upgrade SVObserver for Devario Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 5.50 Beta 63.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.738   21 Mar 2012 13:58:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated version number to 5.50 Beta 062.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.737   20 Mar 2012 09:11:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed version to 5.50 beta 61
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.736   20 Mar 2012 09:08:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed issue with Remote Input Tool not being grayed out.  Should not be active unless in Edit Mode.  Added new method - OnUpdateAddeRemoteInputTool 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.735   16 Mar 2012 15:23:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  739
 * SCR Title:  Upgrade SVObserver for Devario Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 550 Beta 60.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.734   15 Mar 2012 08:48:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  757
 * SCR Title:  Fix results view to sort PPQ object selected for viewing
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated version number to 5.50 Beta 059.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.733   13 Mar 2012 14:01:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  745
 * SCR Title:  Fix contention issue during load process and display
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated version number to 5.50 Beta 057.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.732   13 Mar 2012 13:49:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  745
 * SCR Title:  Fix contention issue during load process and display
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated RunAllIPDocuments method, ShowConfigurationAssistant method, and IsMonochromeImageAvailiable method to fix formatting and consolidate functionality to IPDoc.  Added new method SetSelectedToolForOperatorMove method for operator move functionailty.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.731   06 Mar 2012 11:47:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated SVObserver version to 5.50 Beta 055.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.730   06 Mar 2012 11:46:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated code to fix problems when diconnecting socket.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.729   21 Feb 2012 13:50:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  739
 * SCR Title:  Upgrade SVObserver for Devario Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated version to 5.50 Beta 050.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.728   21 Feb 2012 13:45:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated input stream socket port number to 32100.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.727   21 Feb 2012 13:35:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated output stream socket port number to 32101.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.726   14 Feb 2012 13:11:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated version number 5.50 Beta 049.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.725   14 Feb 2012 13:02:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated target windows version to XP (_WIN32_WINNT 0x0501).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.724   10 Feb 2012 10:20:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Update version to 5.50 Beta 048
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.723   10 Feb 2012 10:06:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated functionality to remove bugs associated with the remote output stream and communications to that remote output manager.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.722   10 Feb 2012 09:35:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated functionality to remove bugs associated with the remote input tool and communications to that tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.721   30 Jan 2012 11:59:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated version number to 5.50 Beta 032.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.720   30 Jan 2012 11:58:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Modified source code to consolidate PLC and New Output stream functionality to I/O Controller.  Added new output stream functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.719   20 Jan 2012 10:04:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated version to 5.50 Beta 018.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.718   18 Jan 2012 16:30:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated version to 5.50 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.717   18 Jan 2012 16:28:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated call when socket connected for remote input stream to fix problem with Json syntax.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.716   16 Jan 2012 15:50:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated version to 5.50 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.715   16 Jan 2012 15:29:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated main application and create the command data holder for the JSON commands used with the socket and the remote input tool.  Updated source code to initialize, start and stop the scoket for sending and receiving data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.714   04 Jan 2012 08:09:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  745
 * SCR Title:  Fix contention issue during load process and display
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 5.50 Beta 8.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.713   19 Dec 2011 16:18:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.50 Beta 001.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.712   19 Dec 2011 16:15:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated applciation to handle socket processing and share memory configuration.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.711   13 Oct 2011 08:15:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  726
 * SCR Title:  Upgrade SVObserver to 5.41
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated version number to 5.41 Release.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.710   12 Oct 2011 14:30:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated version to 5.41 Beta 052.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.709   10 Oct 2011 15:18:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated version to 5.41 Beta 051.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.708   10 Oct 2011 10:40:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated version to 5.41 Beta 050.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.707   07 Oct 2011 14:24:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated SVObserver to 5.41 Beta 049.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.706   06 Oct 2011 09:07:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated version to 5.41 Beta 047.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.705   04 Oct 2011 18:54:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated version to 5.41 Beta 045.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.704   03 Oct 2011 11:24:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated version number to 5.41 Beta 043.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.703   03 Oct 2011 10:50:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated application thread proirity to below normal when in run mode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.702   30 Sep 2011 09:07:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  726
 * SCR Title:  Upgrade SVObserver to 5.41
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Always allow help About Dialog even while running.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.701   28 Sep 2011 13:21:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated version to 5.41 Beta 041.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.700   26 Sep 2011 19:53:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated defines back to original defines for process proirity.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.699   23 Sep 2011 11:37:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  726
 * SCR Title:  Upgrade SVObserver to 5.41
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed Beta to 541 beta 40
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.698   21 Sep 2011 08:36:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  726
 * SCR Title:  Upgrade SVObserver to 5.41
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update SVObserver to 541 Beta 39.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.697   16 Sep 2011 16:24:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated version number to 5.41 Beta 038.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.696   16 Sep 2011 16:14:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated objects with new device and resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.695   12 Aug 2011 14:16:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  736
 * SCR Title:  Add a 4 Camera GigE System to the list of systems
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added 4 Camera GigE System.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.694   11 Aug 2011 07:28:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  735
 * SCR Title:  Add Gige Color System
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added system type and product type to support GigE color cameras.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.693   26 Jul 2011 08:01:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  732
 * SCR Title:  Fix issue with the Color Tool's color threshold image extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed to Beta 8
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.692   19 Jul 2011 10:02:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  731
 * SCR Title:  Fix issue with going from Run Mode to Edit Move Tool
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed issue when going from Run Mode to Edit Move Tool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.691   15 Jul 2011 11:54:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  727
 * SCR Title:  Fix Acquisition Tool bug
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated version number to 5.41 Beta 007.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.690   14 Jul 2011 16:20:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated version to 5.41 Beta 005.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.689   14 Jul 2011 09:12:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  726
 * SCR Title:  Upgrade SVObserver to 5.41
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update version number to 5.41 Beta 003.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.688   14 Jul 2011 08:59:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to better track resource management, and clear un-used resources.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.687   14 Jul 2011 08:16:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  729
 * SCR Title:  Adjust SVObserver to fix perfomance problems due to logging
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to remove logging in places that cause performance issues and added a registry controlled variable to handle the go offline data manager dumps to files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.686   05 Jul 2011 12:43:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  725
 * SCR Title:  Upgrade SVObserver to version 5.4 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to only display one digit after the decimal point.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.685   13 Jun 2011 14:42:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.40 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.40 Release.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.684   09 Jun 2011 15:17:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.40 Beta 034.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.683   06 Jun 2011 13:10:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.40 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.40 Beta 032.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.682   06 Jun 2011 13:06:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  722
 * SCR Title:  Fix a problem with Raid Error Information on X2 systems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to correct issues with the RAID error information associated with the X2 products.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.681   01 Jun 2011 11:17:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.40 beta 030.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.680   27 May 2011 12:37:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.40 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.39 Release.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.679   26 May 2011 13:16:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  702
 * SCR Title:  Updated Coreco Acquisition Functionality to Include Xcelera-AN Board
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.40 Beta 028.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.678   24 May 2011 15:42:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  702
 * SCR Title:  Updated Coreco Acquisition Functionality to Include Xcelera-AN Board
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.40 Beta 026.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.677   20 May 2011 09:56:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  702
 * SCR Title:  Updated Coreco Acquisition Functionality to Include Xcelera-AN Board
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.40 Beta 024.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.676   18 May 2011 08:39:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  706
 * SCR Title:  Fix Histogram Drawing Problem in Linear Analyzer Dialogs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.40 Beta 023.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.675   16 May 2011 12:47:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.40 Beta 022.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.674   03 May 2011 12:26:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version to 5.40 Beta 021.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.673   02 May 2011 16:17:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.40 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update SVObserver version number to 5.40 Beta 020.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.672   02 May 2011 12:50:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   fixed issues so it would not ask if you want to save IPD when deleting a modified inspection in the Configuration Assistant
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.671   29 Apr 2011 08:07:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.40 Beta 019.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.670   27 Apr 2011 11:11:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  720
 * SCR Title:  Change menu options for Test and Regession Modes
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated Menu Options when in Test or Regression Mode
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.669   19 Apr 2011 16:26:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.40 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.668   19 Apr 2011 16:18:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix issues with IO Assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.667   19 Apr 2011 10:31:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.40 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.40 Beta 015.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.666   18 Apr 2011 11:06:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version to 5.40 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.665   18 Apr 2011 10:36:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.664   13 Apr 2011 15:39:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.40 Beta 013.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.663   11 Apr 2011 18:58:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Version to 5.40 Beta 012.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.662   08 Apr 2011 13:35:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.40 Beta 011.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.661   07 Apr 2011 16:47:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.40 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.660   05 Apr 2011 12:41:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.40 Beta 009.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.659   04 Apr 2011 12:08:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.40 Beta 008.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.658   31 Mar 2011 13:04:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.40 Beta 007.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.657   30 Mar 2011 16:42:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.40 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.40 Beta 006.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.656   30 Mar 2011 16:11:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to clairify construction functionality and add clean-up functionality to remove memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.655   22 Mar 2011 08:04:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.40 Beta 005.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.654   22 Mar 2011 08:01:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated static global classes to singletons to promote proper and defined construction and destruction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.653   21 Mar 2011 12:22:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.40 Beta 004.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.652   18 Mar 2011 07:59:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.40 Beta 003 and update project dependenicies.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.651   16 Mar 2011 14:02:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.40 Beta 002.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.650   16 Mar 2011 13:45:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.649   14 Mar 2011 08:23:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.40 Beta 001.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.648   01 Mar 2011 10:40:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.33 Beta 035.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.647   25 Feb 2011 12:29:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number to 3.33 Beta 034.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.646   25 Feb 2011 12:22:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.645   18 Feb 2011 16:31:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version to 5.33 Beta 033.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.644   16 Feb 2011 12:19:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver Version to 5.33 Beta 032.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.643   14 Feb 2011 15:08:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.33 Beta 031.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.642   14 Feb 2011 14:51:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated app code to include the use of the new object manager state for fast multi-threaded access.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.641   01 Feb 2011 13:15:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.33 Beta 030.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.640   27 Jan 2011 13:38:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update version number to 5.33 Beta 029.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.639   26 Jan 2011 10:27:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   updated to beta 28
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.638   26 Jan 2011 09:19:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   updated to beta 27
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.637   20 Jan 2011 08:53:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   fixed issue with Save/Save As so it will reset the modified flag for the IPDoc's so it will not ask to save them when closing the configuration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.636   18 Jan 2011 12:42:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   added new method "ResetAllIPDocModifyFlag" to walk and update all IPDoc's to reset the Modified Flag.  
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.635   18 Jan 2011 11:08:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.33 Beta 026.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.634   11 Jan 2011 13:06:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.33 Beta 025.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.633   17 Dec 2010 10:09:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.33 Beta 024.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.632   16 Dec 2010 11:19:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.33 Beta 023.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.631   15 Dec 2010 10:40:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.33 Beta 022.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.630   15 Dec 2010 10:04:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix connections and synchronization issues with the IO Sub-system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.629   13 Dec 2010 11:32:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.33 Beta 021.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.628   13 Dec 2010 08:40:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  702
 * SCR Title:  Updated Coreco Acquisition Functionality to Include Xcelera-AN Board
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number to 5.33 Beta 020.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.627   08 Dec 2010 13:49:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.33 Beta 019.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.626   08 Dec 2010 13:15:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.625   09 Nov 2010 16:33:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.33 Beta 018.
 * 
 * Update return type for Can Close method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.624   08 Nov 2010 13:10:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated go offline logic to remove module ready first.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.623   05 Nov 2010 15:00:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.33 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.622   05 Nov 2010 11:15:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.33 Beta 016.
 * 
 * Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.621   04 Nov 2010 14:20:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.33 Beta 015.
 * 
 * Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.620   03 Nov 2010 12:02:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.33 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.619   02 Nov 2010 11:14:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.33 Beta 013.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.618   29 Oct 2010 15:24:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  705
 * SCR Title:  Change File Open Attribute in Command Methods Put and Get
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated versoin number to 5.33 Beta 012.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.617   27 Oct 2010 09:35:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number 5.33 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.616   26 Oct 2010 14:44:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  705
 * SCR Title:  Change File Open Attribute in Command Methods Put and Get
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number to 5.33 Beta 009.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.615   26 Oct 2010 13:47:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  706
 * SCR Title:  Fix Histogram Drawing Problem in Linear Analyzer Dialogs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update version number for SVObserver to 5.33 Beta 008.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.614   22 Oct 2010 08:34:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.33 Beta 007.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.613   21 Oct 2010 14:32:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.33 Beta 006.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.612   21 Oct 2010 11:49:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update version number to 5.33 Beta 005.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.611   15 Oct 2010 15:30:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version to 5.33 Beta 004.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.610   30 Aug 2010 14:01:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.33 Beta 003.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.609   17 Aug 2010 10:46:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  699
 * SCR Title:  Fix problem with AVT Guppy and Software Triggers
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.33 Beta 001.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.608   05 Aug 2010 09:04:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  694
 * SCR Title:  Upgrade SVObserver version for Chromis release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.32 Release.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.607   30 Jul 2010 14:05:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  694
 * SCR Title:  Upgrade SVObserver version for Chromis release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.32 Beta 030.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.606   27 Jul 2010 10:59:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  694
 * SCR Title:  Upgrade SVObserver version for Chromis release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.32 Beta 027.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.605   23 Jul 2010 09:35:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  697
 * SCR Title:  Add Free Memory Information to Status Bar
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated status bar functionality to better size the data so that it does not scroll off the screen.  Updated version number to 5.32 Beta 026.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.604   19 Jul 2010 13:26:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  694
 * SCR Title:  Upgrade SVObserver version for Chromis release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.32 Beta 023.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.603   13 Jul 2010 12:40:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  697
 * SCR Title:  Add Free Memory Information to Status Bar
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.32 Beta 022.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.602   01 Jul 2010 08:56:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  696
 * SCR Title:  Fix Problem with PPQ Reset Delay
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.32 Beta 021.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.601   29 Jun 2010 14:09:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.32 Beta 020.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.600   22 Jun 2010 13:43:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  694
 * SCR Title:  Upgrade SVObserver version for Chromis release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.32 Beta 018.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.599   22 Jun 2010 10:01:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  694
 * SCR Title:  Upgrade SVObserver version for Chromis release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.32 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.598   02 Jun 2010 08:32:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number 5.32 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.597   01 Jun 2010 15:27:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version to 5.32 Beta 005.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.596   01 Jun 2010 10:59:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.595   28 May 2010 14:31:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  694
 * SCR Title:  Upgrade SVObserver version for Chromis release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Version number to 5.32 Beta 003.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.594   24 May 2010 14:52:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include file due to changes in the data manager include path update.  Updated thread creation in PPQ and version number 5.32 Beta 002.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.593   08 Mar 2010 12:19:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.31 Release.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.592   05 Mar 2010 11:03:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.31 Beta 044.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.591   03 Mar 2010 15:59:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  687
 * SCR Title:  Large Images not displayed correctly in SVObserver
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Updated version number to 5.31 Beta 043.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.590   01 Mar 2010 10:42:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated get SVIM product for a Gige system with the color flag is set should return an UNKNOWN type.  Updated version number to 5.31 Beta 042.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.589   26 Feb 2010 12:37:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.31 Beta 041.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.588   25 Feb 2010 09:21:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.31 Beta 040.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.587   24 Feb 2010 16:14:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to move the default color setting into configuratoin dialog.  Updated version to 5.31 Beta 009.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.586   24 Feb 2010 11:02:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated camera store and restore functionality for the cameras and updated version to 5.31 Beta 038.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.585   19 Feb 2010 15:55:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.31 Beta 035 and added new logic to preduce error message if the hardware does not exist.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.584   19 Feb 2010 13:24:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix issues with system type on configuration assistant dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.583   19 Feb 2010 12:54:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix the defaults for he configuration assistant dialog and updated version number to 5.31 Beta 034.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.582   18 Feb 2010 14:34:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number 5.31 Beta 033.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.581   16 Feb 2010 10:32:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.31 Beta 030.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.580   15 Feb 2010 12:37:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to remove the generations of board image processing classes from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.579   12 Feb 2010 15:48:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  685
 * SCR Title:  Update PPQ Bar to allow camera(s) to be moved from the first position
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.31 Beta 028.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.578   12 Feb 2010 12:52:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  687
 * SCR Title:  Large Images not displayed correctly in SVObserver
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Updated Version number to 5.31 Beta 27
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.577   20 Jan 2010 15:00:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  683
 * SCR Title:  Update parameter support for SVS-Vistek eco204MVGE Gige camera
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version to 5.31 Beta 026.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.576   19 Jan 2010 15:56:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  681
 * SCR Title:  Update PPQ to automatically threshold inspections when continuously NAKing
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a parameter to the Thread process method to control the wait functionality.  Updated inspection queueing functionality to handle floundering inspections.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.575   17 Dec 2009 09:16:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  679
 * SCR Title:  Change functionality of IsSecured to include Forced Prompt functionaliity
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.31 Beta 018, and fixed a debug include issue.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.574   16 Dec 2009 12:08:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.573   11 Nov 2009 13:38:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  668
 * SCR Title:  Upgrade SVObserver version for Barracuda release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.30 Release
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.572   10 Nov 2009 12:47:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed Gige Color system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.571   09 Nov 2009 16:12:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  668
 * SCR Title:  Upgrade SVObserver version for Barracuda release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.30 Beta 028
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.570   09 Nov 2009 09:49:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  668
 * SCR Title:  Upgrade SVObserver version for Barracuda release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.30 Beta 026.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.569   06 Nov 2009 16:23:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  668
 * SCR Title:  Upgrade SVObserver version for Barracuda release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.30 Beta 025.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.568   05 Nov 2009 15:10:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  668
 * SCR Title:  Upgrade SVObserver version for Barracuda release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.30 Beta 024
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.567   05 Nov 2009 07:55:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  668
 * SCR Title:  Upgrade SVObserver version for Barracuda release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.30 Beta 023
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.566   04 Nov 2009 18:47:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  668
 * SCR Title:  Upgrade SVObserver version for Barracuda release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.30 Beta 022.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.565   02 Nov 2009 19:07:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  668
 * SCR Title:  Upgrade SVObserver version for Barracuda release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.30 Beta 021
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.564   02 Nov 2009 12:36:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  668
 * SCR Title:  Upgrade SVObserver version for Barracuda release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.30 Beta 020
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.563   29 Oct 2009 14:45:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Fixed issues with missing image processing class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.562   29 Oct 2009 11:27:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix issue with model number tracking.  Updated version to 5.30 Beta 019.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.561   27 Oct 2009 18:52:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix issues with disconnecting cameras and invalid image behavior.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.560   22 Oct 2009 19:55:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  668
 * SCR Title:  Upgrade SVObserver version for Barracuda release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.30 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.559   14 Oct 2009 19:06:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  668
 * SCR Title:  Upgrade SVObserver version for Barracuda release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.30 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.558   14 Oct 2009 18:40:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.29
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated version to 5.30 Beta 015.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.557   13 Oct 2009 15:48:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  668
 * SCR Title:  Upgrade SVObserver version for Barracuda release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update version number to 5.30 Beta 14.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.556   07 Oct 2009 14:15:20   jhanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  654
 * SCR Title:  Software Trigger Runtime Adjustments
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Enabled software trigger setu dialog in test mode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.555   24 Sep 2009 09:40:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 5.30 Beta 013.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.554   21 Sep 2009 13:36:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  662
 * SCR Title:  Add Display Change Support to SVImageView
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Updated version number to 5.30 Beta 12
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.553   03 Sep 2009 10:31:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.30 Beta 011
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.552   20 Aug 2009 15:19:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Version to 5.30 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.551   14 Aug 2009 14:51:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to merge branch functionality back into main line code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.550   31 Jul 2009 10:21:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  667
 * SCR Title:  Fixing the access violation bug in Camera Manager
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Updated version number to 5.30 Beta 009.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.549   30 Jul 2009 14:36:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.548   30 Jul 2009 12:14:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.547   23 Jun 2009 15:00:46   jhanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  654
 * SCR Title:  Software Trigger Runtime Adjustments
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Added SoftwareTriggerDialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 *
 *    Rev 3.546   17 Jun 2009 09:06:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added IsMatroxGigeSystem method
 * Revised the DisplayCameraManager method to use the ProductType as a calling argument
 * Added SetGigePacketSizeDeviceParam method
 * Revised to support Gige system
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.545   03 Jun 2009 10:10:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Include file definitions due to changes in SVDeviceParams
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.544   28 Apr 2009 09:20:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  658
 * SCR Title:  Update SVObserver to handle the new model number info
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed code to correctly handle model numbers with no hardware (Laptop version)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.543   21 Apr 2009 08:59:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  659
 * SCR Title:  Modify SVObservers IO system behavior to be based on a hardware.ini setting
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added code to read IOBoardMode and send it to the IOBoard in LoadDigitalDLL.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.542   12 Feb 2009 13:15:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  638
 * SCR Title:  Upgrade SVObserver to 5.20
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 5.20 Release
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.541   11 Feb 2009 15:51:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  638
 * SCR Title:  Upgrade SVObserver to 5.20
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed Version to 5.20 Beta 20
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.540   10 Feb 2009 10:42:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Updated beta version number
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.539   05 Feb 2009 14:00:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  638
 * SCR Title:  Upgrade SVObserver to 5.20
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 5.20 Beta 18
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.538   05 Feb 2009 13:27:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  638
 * SCR Title:  Upgrade SVObserver to 5.20
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to SVO 5.20 Beta 17
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.537   05 Feb 2009 08:02:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  638
 * SCR Title:  Upgrade SVObserver to 5.20
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Moved initialize security up in InitInstance. This prevents a crash but why??.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.536   04 Feb 2009 15:08:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  645
 * SCR Title:  Add support for X2 and X2 Color products
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added support for X2 and X2 color
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.535   04 Feb 2009 13:33:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Updated Version number for 5.20 Beta 16
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.534   04 Feb 2009 07:18:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  638
 * SCR Title:  Upgrade SVObserver to 5.20
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 5.20 Beta 15
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.533   29 Jan 2009 12:52:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  638
 * SCR Title:  Upgrade SVObserver to 5.20
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 5.20 Beta 14
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.532   27 Jan 2009 14:03:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  638
 * SCR Title:  Upgrade SVObserver to 5.20
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 5.20 Beta 14
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.531   27 Jan 2009 13:58:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  647
 * SCR Title:  Add Information to Event Log
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Additional Logging to event log.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.530   27 Jan 2009 13:51:20   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed SVDirectX::InitializeObject to SVDirectX::Initialize
 * Added call to SVDirectX::clear to ExitInstance
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.529   21 Jan 2009 09:44:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  646
 * SCR Title:  Archive Tool Buffer until Offline Mode Problem
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to update version number to 5.20 Beta 12
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.528   20 Jan 2009 10:41:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  645
 * SCR Title:  Add support for X2 and X2 Color products
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added support for the X2, and upped the beta number to 11
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.527   20 Jan 2009 07:36:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  638
 * SCR Title:  Upgrade SVObserver to 5.20
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 520 beta 010
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.526   15 Jan 2009 08:01:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  638
 * SCR Title:  Upgrade SVObserver to 5.20
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Change version to 520 Beta 009
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.525   14 Jan 2009 16:11:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Moved Edit / Add PLC Wizard logic to data controller class
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.524   13 Jan 2009 13:04:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified PLC startup / shutdown in OnEditPLCOutputs function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.523   13 Jan 2009 09:02:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  638
 * SCR Title:  Upgrade SVObserver to 5.20
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 5.20 Beta 008
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.522   31 Dec 2008 11:40:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  644
 * SCR Title:  Pattern Analyzer Enhancements and Bug Fixes
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised version number to 5.20 Beta 7
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.521   19 Dec 2008 13:10:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified add / remove PLC logic in OnEditPLCOutputs(
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.520   19 Dec 2008 07:58:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  638
 * SCR Title:  Upgrade SVObserver to 5.20
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 5.20 Beta 6
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.519   17 Dec 2008 13:01:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   fixed bug in onEditPLCOutputs when adding a plc for the first time.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.518   10 Dec 2008 10:56:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  638
 * SCR Title:  Upgrade SVObserver to 5.20
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 5.20 Beta 005.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.517   04 Dec 2008 13:42:08   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed to use SVDirectX.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.516   04 Dec 2008 10:00:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  638
 * SCR Title:  Upgrade SVObserver to 5.20
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to SVObserver 5.20 Beta 004
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.515   18 Nov 2008 14:39:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  638
 * SCR Title:  Upgrade SVObserver to 5.20
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 5.20 Beta 3
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.514   17 Nov 2008 11:33:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  638
 * SCR Title:  Upgrade SVObserver to 5.20
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 5.20 Beta 2
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.513   17 Nov 2008 10:18:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fix Merge Error
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.512   14 Nov 2008 14:34:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added PLC Outputs and IO Tab Views
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.511   21 Oct 2008 11:34:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  638
 * SCR Title:  Upgrade SVObserver to 5.20
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 5.20 Beta 01
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.510   20 Oct 2008 11:36:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  637
 * SCR Title:  SVObserver Streaming Data Bug Fixes
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Update version number to 5.02 Beta 51
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.509   01 Oct 2008 16:10:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Incremented version number
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.508   16 Sep 2008 15:23:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised InitInstance and GoOnline to report error if load failure with FileAcquisitionDll.
 * Revised INILoad to laod FileAcquistion DLL.
 * Revised LoadAcqusitionDLL to support FileAcquisition.
 * Revised CloseAcqusitionDLL to support FileAcquisition.
 * Revised INIReset to support FileAcquisition.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.507   12 Sep 2008 11:58:50   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Updated DwCurrentVersion for 5.02 Beta 48.
 * Removed reading of SVIM.ini to determine if MIL or DirectX should be used to display images.
 * Moved DirectDraw object, surface, and clipper here from SVImageView.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.506   11 Sep 2008 12:45:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Update version number to 5.02 Beta 32
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.505   11 Sep 2008 07:51:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added methods and variables for Software Trigger
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.504   11 Jul 2008 13:53:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  631
 * SCR Title:  Upgrade SVObserver to 5.01
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed Version to 5.01 Release
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.503   10 Jul 2008 08:17:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  631
 * SCR Title:  Upgrade SVObserver to 5.01
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 5.01 Beta 01
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.502   19 Jun 2008 09:53:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  623
 * SCR Title:  Upgrade SVObserver to 5.00
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Change version to SVO 5.00 Release
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.501   16 Jun 2008 09:29:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  623
 * SCR Title:  Upgrade SVObserver to 5.00
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to SVO 5.00 Beta 017
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.500   11 Jun 2008 14:04:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  623
 * SCR Title:  Upgrade SVObserver to 5.00
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update version to 5.00 Beta 16
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.499   10 Jun 2008 13:04:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  623
 * SCR Title:  Upgrade SVObserver to 5.00
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 5.00 Beta 15
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.498   06 Jun 2008 15:58:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  623
 * SCR Title:  Upgrade SVObserver to 5.00
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 5.00 Beta 014
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.497   05 Jun 2008 10:48:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  623
 * SCR Title:  Upgrade SVObserver to 5.00
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 5.00 Beta 013
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.496   04 Jun 2008 10:48:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  623
 * SCR Title:  Upgrade SVObserver to 5.00
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update SVObserver to 500 Beta 012
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.495   30 May 2008 14:31:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  626
 * SCR Title:  Fix problems with MRU items getting removed
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed items for MRU
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.494   30 May 2008 13:26:22   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Removed all DirectX changes since 5.00 Beta 1.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.493   29 May 2008 14:48:14   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed to use DirectX only in DEBUG mode
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.492   29 May 2008 10:12:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  623
 * SCR Title:  Upgrade SVObserver to 5.00
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Change Beta to 5.00 Beta 011
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.491   22 May 2008 15:44:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  623
 * SCR Title:  Upgrade SVObserver to 5.00
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 5.00 Beta 010
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.490   21 May 2008 09:11:58   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  623
 * SCR Title:  Upgrade SVObserver to 5.00
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 5.00 Beta 009
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.489   15 May 2008 16:16:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  623
 * SCR Title:  Upgrade SVObserver to 5.00
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Change Beta to 008
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.488   15 May 2008 15:10:00   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added svim.ini switch used to select Matrox or DirectX
 * Made switch default to not using DirectX
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.487   14 May 2008 15:00:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  623
 * SCR Title:  Upgrade SVObserver to 5.00
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated to Beta 007
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.486   07 May 2008 15:01:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  623
 * SCR Title:  Upgrade SVObserver to 5.00
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed to Beta 6
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.485   30 Apr 2008 11:42:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  623
 * SCR Title:  Upgrade SVObserver to 5.00
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade SVObserver to Beta 5
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.484   23 Apr 2008 10:23:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  623
 * SCR Title:  Upgrade SVObserver to 5.00
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated Beta to 004
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.483   14 Apr 2008 14:59:16   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Updated DwCurrentVersion for 5.00 Beta 3.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.482   14 Apr 2008 10:12:20   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Removed reading of SVIM.ini to determine if MIL or DirectX should be used to display images.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.481   11 Apr 2008 11:26:02   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Updated DwCurrentVersion for 5.00 Beta 2.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.480   10 Apr 2008 08:07:42   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   * Moved DirectDraw object, surface, and clipper here from SVImageView.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.479   20 Mar 2008 11:13:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  623
 * SCR Title:  Upgrade SVObserver to 5.00
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Change version to 5.00 Beta 001
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.478   31 Oct 2007 08:06:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   - Added Sleep after Load and Free Library to give time to system resource management.
 * - Added code to InitInstance and ExitInstance to manage hEvent resource.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.477   01 Aug 2007 08:06:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  607
 * SCR Title:  Upgrade SVObserver to Version 4.80
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   4.80 version information from branch
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.476   24 Jul 2007 15:17:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.475   22 Jun 2007 10:43:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include removal of all VC6 constraints, project files, and workspace files.  The new VC8 project and solution files will replace the VC6 files with the new tighter compilation rules.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.474   21 Jun 2007 15:37:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Upated version number to 4.90 Beta 001.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.473   20 Jun 2007 16:24:26   Joe
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
 *    Rev 3.472.1.20   15 Oct 2009 09:14:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  669
 * SCR Title:  Upgrade SVObserver version for 4.82 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 4.82 Release.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.472.1.19   14 Oct 2009 10:51:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 4.82 Beta 184.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.472.1.18   13 Oct 2009 15:04:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to version 4.82 Beta 183.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.472.1.17   12 Oct 2009 13:13:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 4.82 Beta 182.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.472.1.16   05 Oct 2009 16:23:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 4.82 Beta 181.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.472.1.15   22 Sep 2009 18:15:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 4.82 Beta 180
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.472.1.14   16 Sep 2009 16:17:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Version to 4.82 Beta 177.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.472.1.13   16 Sep 2009 07:45:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 4.81 Beta 176.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.472.1.12   10 Sep 2009 11:15:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 4.82 Beta 175.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.472.1.11   08 Sep 2009 15:21:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Version to 4.82 Beta 174
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.472.1.10   25 Aug 2009 17:02:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 4.82 Bete 173
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.472.1.9   19 Aug 2009 14:33:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 4.82 Beta 172
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.472.1.8   18 Aug 2009 09:15:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 4.82 Beta 171.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.472.1.7   14 Aug 2009 08:32:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version to 4.82 Beta 170
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.472.1.6   13 Aug 2009 08:48:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 4.82 Beta 169
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.472.1.5   11 Aug 2009 15:38:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 4.82 Beta 168
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.472.1.4   06 Aug 2009 08:41:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 4.82 Beta 167.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.472.1.3   14 Jul 2009 14:44:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver project settings and version number.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.472.1.1   30 Oct 2007 10:23:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   - Modified InitInstance and ExitInstance to include Joe's ImageCompression changes.
 * - Added Sleep after Load and Free Library to give time to system resource management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.472.1.0   01 Aug 2007 07:56:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  607
 * SCR Title:  Upgrade SVObserver to Version 4.80
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Change version to 480 Release
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.472   16 May 2007 14:58:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  579
 * SCR Title:  Upgrade SVObserver to 4.79 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Update version number to 4.79 Released.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.471   14 May 2007 08:34:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  579
 * SCR Title:  Upgrade SVObserver to 4.79 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed Beta to 13.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.470   08 May 2007 09:56:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  579
 * SCR Title:  Upgrade SVObserver to 4.79 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed Beta to 13.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.469   08 May 2007 09:09:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  600
 * SCR Title:  Fix Bug where File - print setup menu option does not function.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed OnFilePrintSetup Message Handler but not the On_Command Map entry.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.468   19 Apr 2007 14:56:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  579
 * SCR Title:  Upgrade SVObserver to 4.79 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated beta to 11.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.467   09 Apr 2007 11:55:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  588
 * SCR Title:  Add the RAID Error Bit for the digital IO
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   new method to determine if the product has RAID
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.466   30 Mar 2007 07:24:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  592
 * SCR Title:  Add  X3 product type to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   fixed problem with the FastOCR oart of the model number.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.465   29 Mar 2007 15:20:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  579
 * SCR Title:  Upgrade SVObserver to 4.79 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed to Beta to 10.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.464   23 Mar 2007 11:54:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  579
 * SCR Title:  Upgrade SVObserver to 4.79 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Update version to beta 009.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.463   22 Mar 2007 14:50:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  593
 * SCR Title:  Remove HUB product types from SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed reference to HUB in the GetSVIMType function.  Since there are no hub systems this should not be in this function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.462   20 Mar 2007 15:25:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  579
 * SCR Title:  Upgrade SVObserver to 4.79 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated Version to Beta 008.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.461   15 Mar 2007 15:08:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  579
 * SCR Title:  Upgrade SVObserver to 4.79 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed to Beta 7
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.460   02 Mar 2007 06:25:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  595
 * SCR Title:  Fix issues with Utility Menu when switching products
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new firmware settings and a new utility map
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.459   19 Feb 2007 06:45:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  594
 * SCR Title:  Remove issue with interpreting status response incorrectly
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVO 4.78 Beta 064 branch code into main line code base.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.458   13 Feb 2007 08:29:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  592
 * SCR Title:  Add  X3 product type to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added logic to allow new product type to be recognized.  The following functions were modified.
 * - IsDigitalSVIM
 * - CheckSVIMType
 * - GetSVIMType
 * - LoadDigitalDLL
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.457   09 Feb 2007 11:36:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  579
 * SCR Title:  Upgrade SVObserver to 4.79 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed to beta 6
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.456   31 Jan 2007 12:34:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  579
 * SCR Title:  Upgrade SVObserver to 4.79 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised version number to 4.79 Beta 5
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.455   26 Jan 2007 10:03:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  579
 * SCR Title:  Upgrade SVObserver to 4.79 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised version number to 4.79 Beta 4
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.454   23 Jan 2007 15:49:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  590
 * SCR Title:  Modify SVObserver to send the IO Board type to the parallel board
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified LoadDigitalDll in SVObserverApp to set the 2 digit IO board model in the IO Board
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.453   22 Jan 2007 14:44:42   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  579
 * SCR Title:  Upgrade SVObserver to 4.79 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed to beta 3
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.452   22 Jan 2007 10:36:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  588
 * SCR Title:  Add the RAID Error Bit for the digital IO
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added checks to determine if the RaidError bit needs to be turned on or off
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.451   21 Dec 2006 13:49:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  579
 * SCR Title:  Upgrade SVObserver to 4.79 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated version number to 4.79 Beta 2
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.450   30 Nov 2006 12:55:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  579
 * SCR Title:  Upgrade SVObserver to 4.79 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Upate version to 4.79 Beta 001
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.449   29 Nov 2006 15:20:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use SVDCamFactoryRegistrar
 * Revised to use SVOIniLoader
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 *
 *    Rev 3.448.1.0   15 Feb 2007 12:06:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  594
 * SCR Title:  Remove issue with interpreting status response incorrectly
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 4.78 Beta 064.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.448   28 Aug 2006 12:05:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  562
 * SCR Title:  Upgrade SVObserver to the Dione Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed version to 4.77 Release
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.447   25 Aug 2006 08:44:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  577
 * SCR Title:  Fix Security bug that interferes with SIAC GoOnline command.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added message handler OnRcGoOnline for remote GoOnline  command.  Also added SetTestMode with a parameter to bypass security.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.446   25 Aug 2006 08:27:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  562
 * SCR Title:  Upgrade SVObserver to the Dione Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   changed to 4.77 beta 14
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.445   23 Aug 2006 09:46:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  562
 * SCR Title:  Upgrade SVObserver to the Dione Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   SVO 4.77 beta 013
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.444   22 Aug 2006 15:49:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  562
 * SCR Title:  Upgrade SVObserver to the Dione Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   SVO 4.77 b011
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.443   21 Aug 2006 07:40:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  562
 * SCR Title:  Upgrade SVObserver to the Dione Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed version to Beta 11.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.442   16 Aug 2006 09:56:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  567
 * SCR Title:  Update SVObserver to accept Intek driver for D-series SVIMs
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated the INI Load, Close, and Reset code to allow DLLs to not be reloaded on a close of a configuration.  The reload logic is controlled by the HARDWARE.INI file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.441   16 Aug 2006 09:45:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  562
 * SCR Title:  Upgrade SVObserver to the Dione Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 4.77 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.440   07 Aug 2006 10:14:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  562
 * SCR Title:  Upgrade SVObserver to the Dione Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   changed to beta 9
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.439   04 Aug 2006 16:57:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  562
 * SCR Title:  Upgrade SVObserver to the Dione Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update SCR notes for invalid scr checkin.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.438   04 Aug 2006 16:53:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  562
 * SCR Title:  Upgrade SVObserver to the Dione Release
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated version number to 4.77 Beta 009.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.437   04 Aug 2006 09:03:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  562
 * SCR Title:  Upgrade SVObserver to the Dione Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed to Beta 8.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.436   04 Aug 2006 08:55:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  575
 * SCR Title:  Fix Bug dealing with Image locking that causes run once to fail
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed compile errors.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.435   04 Aug 2006 08:03:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  575
 * SCR Title:  Fix Bug dealing with Image locking that causes run once to fail
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added function GetMaxPPQLength().  This function returns the m_lSourceImageDepth.
 * 
 * Modified function GetSourceImageDepth().  This function now returns m_lSourceImageDepth + 2.  This modification allows extra buffers to exist so there will not be a locking issue when going offline when maintain published images is true.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.434   01 Aug 2006 08:12:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  567
 * SCR Title:  Update SVObserver to accept Intek driver for D-series SVIMs
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated version number to 4.77 Beta 007.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.433   31 Jul 2006 14:30:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  562
 * SCR Title:  Upgrade SVObserver to the Dione Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated version number to 4.77 Beta 006.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.432   28 Jul 2006 10:43:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  562
 * SCR Title:  Upgrade SVObserver to the Dione Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated version to 4.77 Beta 005
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.431   28 Jul 2006 10:42:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  567
 * SCR Title:  Update SVObserver to accept Intek driver for D-series SVIMs
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated the Try/Catch logic around the 1394 Camera Manager to execute trailing logic that needs to execute for proper clean-up.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.430   25 Jul 2006 13:12:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  562
 * SCR Title:  Upgrade SVObserver to the Dione Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   upped to beta 4
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.429   17 Jul 2006 11:43:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  570
 * SCR Title:  Fix Bug in math tools that casues nested commands to evaluate incorrectly
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified code on math equation functions MIN, MAX, MED, STD DEV, AVG, and SUM to allow recursion.  Also added code to allow legacy behavior.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.428   10 Jul 2006 10:54:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  562
 * SCR Title:  Upgrade SVObserver to the Dione Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Update version number to 4.77 Beta 003.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.427   28 Jun 2006 07:55:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  562
 * SCR Title:  Upgrade SVObserver to the Dione Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   upped to 4.77 beta 2
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.426   27 Jun 2006 10:10:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  564
 * SCR Title:  Add configurability to SVTVicLpt DLL to set the system type
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added code in LoadIni to set board type parameter based on system type if the io dll is of type Entech.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.425   21 Jun 2006 15:06:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  567
 * SCR Title:  Update SVObserver to accept Intek driver for D-series SVIMs
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added check for digitizer dll for 1394
 * added product name checks
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.424   21 Jun 2006 14:17:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  562
 * SCR Title:  Upgrade SVObserver to the Dione Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed version to 4.77 Beta 1
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.423   28 Apr 2006 08:44:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  539
 * SCR Title:  Upgrade SVObserver to the Mimas release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   set to Release 4.76
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.422   21 Apr 2006 15:19:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  539
 * SCR Title:  Upgrade SVObserver to the Mimas release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated version number to 4.76 Beta 10.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.421   11 Apr 2006 15:15:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  539
 * SCR Title:  Upgrade SVObserver to the Mimas release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   4.76 beta 9
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.420   10 Apr 2006 14:09:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  539
 * SCR Title:  Upgrade SVObserver to the Mimas release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   4.76 beta 8
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.419   30 Mar 2006 15:01:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  539
 * SCR Title:  Upgrade SVObserver to the Mimas release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Update version to SVO 4.76 Beta 007.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.418   28 Mar 2006 14:09:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  539
 * SCR Title:  Upgrade SVObserver to the Mimas release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   4.76 Beta 6
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.417   27 Mar 2006 09:18:14   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  551
 * SCR Title:  Remove the startup message when no cameras are detected
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed the message if no cameras were detected for the Intek driver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.416   20 Mar 2006 14:55:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  545
 * SCR Title:  Display version information for rabbit board component
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added comments about emuns.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.415   20 Mar 2006 14:00:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  539
 * SCR Title:  Upgrade SVObserver to the Mimas release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   SVO 4.76 Beta 5
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.414   15 Mar 2006 14:45:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  545
 * SCR Title:  Display version information for rabbit board component
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added emun for IOBoardTrigger Parameters.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.413   30 Jan 2006 14:54:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  541
 * SCR Title:  Upgrade SVObserver for Pan Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 4.75 Release.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.412   27 Jan 2006 16:13:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  541
 * SCR Title:  Upgrade SVObserver for Pan Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   SVO 4.75 Beta 10
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.411   26 Jan 2006 16:27:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  541
 * SCR Title:  Upgrade SVObserver for Pan Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   SVO 4.75 Beta 9
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.410   25 Jan 2006 15:00:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  541
 * SCR Title:  Upgrade SVObserver for Pan Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   SVO 4.75 beta 8
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.409   24 Jan 2006 15:37:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  541
 * SCR Title:  Upgrade SVObserver for Pan Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   SVO 4.75 beta 7
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.408   24 Jan 2006 12:05:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  541
 * SCR Title:  Upgrade SVObserver for Pan Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 4.75 Beta 006 and fixed issue with hardware error message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.407   24 Jan 2006 12:04:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  538
 * SCR Title:  Remove instances when displaying RAID error message on event log read errors
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated RAID checks to remove RAID error status if not a RAID system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.406   20 Jan 2006 09:04:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  541
 * SCR Title:  Upgrade SVObserver for Pan Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update SVObserver to version 4.75 Beta 005
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.405   20 Jan 2006 09:02:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  538
 * SCR Title:  Remove instances when displaying RAID error message on event log read errors
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated inspection process to not allow RAID to be checked if not setup as intel in the hardware.ini and no the right processor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.404   19 Jan 2006 11:23:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 4.75 Beta 004
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.403   17 Jan 2006 14:57:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   SVO 4.75 beta 3
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.402   17 Jan 2006 11:03:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   SVO 4.75 Beta 2
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.401   16 Jan 2006 17:49:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved removed embedded object to create object method for load of configuration to happen properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.400   05 Jan 2006 14:47:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  524
 * SCR Title:  Upgrade SVObserver version numbers for Titan
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated version number to 4.70 Release
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.399   05 Jan 2006 10:00:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  524
 * SCR Title:  Upgrade SVObserver version numbers for Titan
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated version number to 4.70 Beta 011
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.398   05 Jan 2006 09:55:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  530
 * SCR Title:  Allow SVObserver to monitor Intel SATA RAID status
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to include new RAID Failure state for SIAC
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.397   03 Jan 2006 10:08:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  524
 * SCR Title:  Upgrade SVObserver version numbers for Titan
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated SVObserver version number to 4.70 Beta 010
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.396   22 Dec 2005 15:20:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  530
 * SCR Title:  Allow SVObserver to monitor Intel SATA RAID status
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new resources for the new modeless message box.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.395   13 Dec 2005 14:30:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  524
 * SCR Title:  Upgrade SVObserver version numbers for Titan
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update version number to 4.70 Beta 009.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.394   12 Dec 2005 09:08:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  524
 * SCR Title:  Upgrade SVObserver version numbers for Titan
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   SVObserver 4.70 Beta 008
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.393   08 Dec 2005 15:09:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  524
 * SCR Title:  Upgrade SVObserver version numbers for Titan
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated version number to 4.70 Beta 007.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.392   08 Dec 2005 08:02:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added ON_UPDATE_COMMAND_UI message handler for Published Images menu option.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.391   06 Dec 2005 15:37:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  524
 * SCR Title:  Upgrade SVObserver version numbers for Titan
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update Beta to 006
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.390   06 Dec 2005 15:28:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  527
 * SCR Title:  Fix problems with Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed code to bring the IPDoc to the front when the mode changes out of regression
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.380.1.1   17 Nov 2005 11:13:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  521
 * SCR Title:  Upgrade SVObserver to version 4.61
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated version number to 4.61 Beta 013
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.389   17 Nov 2005 11:06:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  524
 * SCR Title:  Upgrade SVObserver version numbers for Titan
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated version number to 4.70 Beta 005
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.388   14 Nov 2005 08:43:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  524
 * SCR Title:  Upgrade SVObserver version numbers for Titan
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated version number to 4.70 Beta 004.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.387   14 Nov 2005 08:42:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated error message when Intek camera driver does not find a camera.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.380.1.0   11 Nov 2005 15:14:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  521
 * SCR Title:  Upgrade SVObserver to version 4.61
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   SVO 4.61 Beta 12
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.386   11 Nov 2005 10:49:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a new function to see if the current hardware porduct is a Coreco product.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.385   08 Nov 2005 16:11:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  524
 * SCR Title:  Upgrade SVObserver version numbers for Titan
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated version number to 4.70 Beta 003.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.384   08 Nov 2005 16:02:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated 1394 code to allow for the use of the Intek driver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.383   03 Nov 2005 13:31:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  513
 * SCR Title:  Add Parallel Port I/O to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Entech LPT IO selection in Ini-Load and IO_BOARD_ENTECH_LPT enum.  This allows the SVTVicLPT.dll to load.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.382   02 Nov 2005 09:14:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  521
 * SCR Title:  Upgrade SVObserver to version 4.61
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   SVO 4.61 beta 01
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.381   02 Nov 2005 09:13:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  520
 * SCR Title:  Fix select camera ROI dialog crash
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated SVOConfigAssistantDlg call from IsNewConfiguration to SetNewConfiguration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.380   26 Oct 2005 12:34:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated Version Number to 4.60 Release
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.379   24 Oct 2005 15:42:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Change Beta to 53
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.378   24 Oct 2005 10:02:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed to Beta 52
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.377   20 Oct 2005 15:48:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   SVO 4.60 Beta 51
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.376   19 Oct 2005 14:41:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added OnStop to StopRegression to fix bug when changing mode from regression.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.375   19 Oct 2005 12:23:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated version number to 4.60 Beta 50
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.374   14 Oct 2005 12:06:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Change to Beta 49
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.373   13 Oct 2005 16:15:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated version number to 4.60 Beta 48
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.372   12 Oct 2005 10:04:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed Beta number to 47.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.371   12 Oct 2005 09:55:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added additional check in OnUpdateGoOffline to also include the checkpoint SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.370   11 Oct 2005 07:49:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Change Beta to 46
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.369   10 Oct 2005 08:53:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed Beta to 45
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.368   06 Oct 2005 15:57:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed Beta to 43
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.367   06 Oct 2005 10:54:08   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  505
 * SCR Title:  Encrypt security file
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Changed 
 * SVObserverApp::fileSaveAsSec () function 
 * to improve encryption problem error handling.
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.366   04 Oct 2005 10:54:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Upgrade to beta 42
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.365   23 Sep 2005 10:55:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Change to Beta 41
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.364   21 Sep 2005 13:02:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed help for the regression test.  also fixed problem with switching to test mode from regression.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.363   21 Sep 2005 09:28:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   removed BroadcastMessage SVM_GOING_OFFLINE (moved to SVInspectionProcess)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.362   21 Sep 2005 09:11:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   used the call to the memory manager for ReservedBytes to determine if the archive dialog should come up
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.361   21 Sep 2005 08:58:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   changed to beta 40
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.360   16 Sep 2005 15:38:40   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  505
 * SCR Title:  Encrypt security file
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Changed 
 * SVObserverApp::OpernSVXFile ()
 * to correct error handling.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.359   14 Sep 2005 15:36:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added a dialog that will come up when the archive tool is writing out images, when the archive tool is in change mode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.358   14 Sep 2005 15:20:00   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   beta 30
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.357   12 Sep 2005 13:27:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   beta 26
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.356   09 Sep 2005 10:43:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed onGoOnline to call StopRegression before calling OnStop.  OnStop was removing the mode regession and was not allowing the regression test dialog to go away.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.355   08 Sep 2005 15:49:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   SVO 4.60 Beta 25
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.354   06 Sep 2005 15:11:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed Beta to 24
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.353   01 Sep 2005 14:58:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Change to Beta 23
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.352   01 Sep 2005 12:42:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   upped to beta 22
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.351   31 Aug 2005 10:09:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed to Beta 21
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.350   29 Aug 2005 16:55:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Change to Beta 20
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.349   29 Aug 2005 16:52:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added state check for mode change to prevent validation if already in the requested mode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.348   26 Aug 2005 14:36:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  493
 * SCR Title:  Fix Bug in SVOutputObjectList
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Moved ValidateOutputList function to OnConfigurationFinishedInitializing(
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.347   25 Aug 2005 15:06:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  493
 * SCR Title:  Fix Bug in SVOutputObjectList
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Prevent Crash when failed load.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.346   24 Aug 2005 16:23:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Message Handler for Start Test Mode Button.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.345   24 Aug 2005 16:14:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Change to Beta 19
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.344   24 Aug 2005 11:08:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed Menu states when configuration is closed in UpdateUI message handlers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.343   24 Aug 2005 08:04:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed to Beta 18
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.342   23 Aug 2005 13:31:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed to Beta 17
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.341   22 Aug 2005 14:00:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Change to Beta 16
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.340   22 Aug 2005 13:30:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Prevent Run from validating the user if already in run mode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.339   22 Aug 2005 11:14:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Implement Dual Access Point Validate function.
 * Change name of SVIsValid to SVIsDisplayable
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.338   17 Aug 2005 13:32:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to beta 15.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.337   17 Aug 2005 11:49:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Accesspoint for exiting run
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.336   16 Aug 2005 15:19:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  506
 * SCR Title:  Update Tool Set List Tab to include an indicator when a tool is invalid
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new method to run all inspections.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.335   15 Aug 2005 09:41:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated version number to SVO 4.60 Beta 014
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.334   10 Aug 2005 16:20:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed Bug in OnUpdateEditAdjustLightReference
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.333   10 Aug 2005 13:01:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed Status Bar cleared mouse status when stop.
 * Security in MRU
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.332   10 Aug 2005 12:53:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated version number to 4.60 Beta 13 and updated issues with the project files.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.331   09 Aug 2005 10:40:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed to 4.60 Beta 12
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.330   05 Aug 2005 09:46:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Prevent Crash caused by going from run to test.
 * Modified SetMode to prevent going from run to test.
 * Modified OnTestMode to prevent change if running.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.329   03 Aug 2005 15:12:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed bug with running recent MRU
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.328   03 Aug 2005 10:51:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed to Beta 11
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.327   02 Aug 2005 13:52:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Functions OkToEdit and OkToEditMove to make menus more consistant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.326   02 Aug 2005 12:09:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   create e:\temp if it doesn't exist
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.325   02 Aug 2005 09:47:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Consolidated reset counts code to SVToolSetClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.324   01 Aug 2005 14:13:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Change to Beta 10
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.323   01 Aug 2005 08:42:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed Security file name to gatekpr.xml, and made the login function remove edit mode if the user did not have privileges.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.322   28 Jul 2005 16:11:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated GUI Editing Flag to block mode changes from SIAC
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.321   28 Jul 2005 13:55:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   SVO 4.60 Beta 9
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.320   27 Jul 2005 09:40:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  501
 * SCR Title:  Expose Cylindrical Warp and Perspective Warp Tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed ID for PerspectiveWarpTool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.319   27 Jul 2005 08:48:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  501
 * SCR Title:  Expose Cylindrical Warp and Perspective Warp Tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   GUI Changes to expose Cylindrical and Perspective Tools
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.318   26 Jul 2005 15:22:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed Mode Menu descriptions from online and offline to Run and Stop.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.317   26 Jul 2005 14:28:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed to 4.60 Beta 8
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.316   25 Jul 2005 11:33:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added behavior to automatically go into edit if edit is unsecured.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.315   22 Jul 2005 13:55:52   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new method DeselectTool.  Will deselect tool for toolset list when going into differnet modes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.314   19 Jul 2005 16:00:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   More Menu changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.313   19 Jul 2005 14:27:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  475
 * SCR Title:  Increase Hub capabilities
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added D2 / D3 Hub products
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.312   19 Jul 2005 11:24:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to Beta 7
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.311   19 Jul 2005 11:22:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Menu Changes for logoff and GoOnline to previous Mode state if failed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.310   18 Jul 2005 17:02:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   GUI Mode Menu Functions Bugs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.309   18 Jul 2005 15:55:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Mode Menu Auto Offline to Edit Removed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.308   18 Jul 2005 14:36:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changes in GUI Mode menu.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.307   15 Jul 2005 11:32:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new method StopRegression. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.306   15 Jul 2005 08:01:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed to Beta 6
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.305   07 Jul 2005 15:52:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to Beta 5
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.304   07 Jul 2005 13:27:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed problems with menu and sub menu behavior.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.303   06 Jul 2005 14:05:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Auto Redirect - Offline to Edit logic. in OnStop() and m_dwPrevState
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.302   29 Jun 2005 16:09:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   4.60 Beta 4
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.301   29 Jun 2005 07:51:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  493
 * SCR Title:  Fix Bug in SVOutputObjectList
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Calls to ValidateOutputList after loading and after configAssistantDlg.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.300   28 Jun 2005 17:38:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   4.60 Beta 3
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.299   27 Jun 2005 14:54:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   use SVValueObjectReference
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.298   27 Jun 2005 09:46:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  497
 * SCR Title:  Remove Freeze Set from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update SVObserver to not process freeze methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.297   27 Jun 2005 09:09:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added secutity to reset counts menu options.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.296   22 Jun 2005 14:17:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   4.60 Beta 2
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.295   22 Jun 2005 13:41:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fix merge error initial debug
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.294   22 Jun 2005 12:05:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added #includes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.293   22 Jun 2005 10:46:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed includes that were used for old security
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.292   21 Jun 2005 14:15:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added SVSecurity functions for access.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.291   20 Jun 2005 10:07:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   made Archive Tool buffer pool sizes settable in the INI file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.290   20 May 2005 14:22:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   SVO 4.60 Beta 1
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.289   20 May 2005 12:05:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Memory Manager initialization
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.288   18 May 2005 15:02:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  485
 * SCR Title:  Add a way to Reset Counts remotely
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added function ResetAllCounts(.  This resets all IPDoc counters by adding an input request to each IP.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.287   17 May 2005 10:22:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  489
 * SCR Title:  Bug Fix when loading SVX Configuration
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed Extra SysFreeString in OpenSVX to prevent crash.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.286   27 Apr 2005 10:28:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  445
 * SCR Title:  Fix Crash when closing SVObserver from online
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Check to OnAppExit if running then call OnStop().
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.285   22 Mar 2005 09:21:00   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  452
 * SCR Title:  Upgrade SVObserver to version 4.50
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Release 4.50
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.284   22 Mar 2005 07:23:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  452
 * SCR Title:  Upgrade SVObserver to version 4.50
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   beta 226
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.283   21 Mar 2005 15:01:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  452
 * SCR Title:  Upgrade SVObserver to version 4.50
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   beta 226
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.282   21 Mar 2005 11:21:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  452
 * SCR Title:  Upgrade SVObserver to version 4.50
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   beta 224
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.281   18 Mar 2005 09:13:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  452
 * SCR Title:  Upgrade SVObserver to version 4.50
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   beta 223
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.280   16 Mar 2005 13:50:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  452
 * SCR Title:  Upgrade SVObserver to version 4.50
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   beta 222
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.279   15 Mar 2005 15:09:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  452
 * SCR Title:  Upgrade SVObserver to version 4.50
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   beta 221
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.278   14 Mar 2005 07:46:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  452
 * SCR Title:  Upgrade SVObserver to version 4.50
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   beta 220
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.277   09 Mar 2005 15:35:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  452
 * SCR Title:  Upgrade SVObserver to version 4.50
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   4.50 beta 219
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.276   09 Mar 2005 13:13:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  478
 * SCR Title:  Add a new Perspective Warp Tool
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Allow the PerspectiveWarp Tool to be added from the add menu.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.275   09 Mar 2005 12:24:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  477
 * SCR Title:  Add Cylindrical warp Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added CylindricalWarp tool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.274   07 Mar 2005 14:41:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  452
 * SCR Title:  Upgrade SVObserver to version 4.50
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   upped to beta 218
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.273   02 Mar 2005 15:20:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  452
 * SCR Title:  Upgrade SVObserver to version 4.50
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.272   28 Feb 2005 12:11:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  452
 * SCR Title:  Upgrade SVObserver to version 4.50
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed reference to _MINDEBUG
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.271   28 Feb 2005 10:12:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  452
 * SCR Title:  Upgrade SVObserver to version 4.50
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   4.50 beta 216
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.270   28 Feb 2005 08:48:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  452
 * SCR Title:  Upgrade SVObserver to version 4.50
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   in DEBUG mode only: D3's can now run D1 configs, etc.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.269   28 Feb 2005 08:46:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  452
 * SCR Title:  Upgrade SVObserver to version 4.50
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed a spurrious period
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.268   22 Feb 2005 14:18:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  452
 * SCR Title:  Upgrade SVObserver to version 4.50
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   upped to beta 215
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.267   22 Feb 2005 13:55:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  452
 * SCR Title:  Upgrade SVObserver to version 4.50
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated to add new model numbers for matrox boards.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.266   22 Feb 2005 10:54:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  470
 * SCR Title:  Upgrade SVObserver to version 4.50
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated version to 4.50 Beta 2.14
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.265   18 Feb 2005 11:56:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  452
 * SCR Title:  Upgrade SVObserver to version 4.50
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated version number and beta number to 4.50 Beta 213.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.264   18 Feb 2005 11:06:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated SVObserver App and project files to include new linear tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.263   17 Feb 2005 14:54:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.262   16 Feb 2005 14:51:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.261   19 Jan 2005 14:45:10   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  375
 * SCR Title:  Remodel XML usage for configuration storage and SIAC transfers.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   The function
 * 
 * SVObserverApp::OpenSECFileFromMostRecentList() 
 * 
 * was modified to correctly handle return codes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.260   12 Jan 2005 09:45:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  461
 * SCR Title:  Add 1394 Hub capability to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed merge error
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.259   26 Oct 2004 09:43:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  461
 * SCR Title:  Add 1394 Hub capability to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   merged 3.257.1.0 into mainline
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.258   20 Oct 2004 16:25:10   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  375
 * SCR Title:  Remodel XML usage for configuration storage and SIAC transfers.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   SVObserverApp::OpenSECFile () 
 * was modified to use new SVConfigurationObjectLoadConfiguration () return value.
 * 
 * SVObserverApp::OpenSVXFile () was modified to use new SVConfigurationObjectLoadConfiguration () return value and to improve overall error handling and reporting.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 *    Rev 3.257.1.0   26 Oct 2004 09:32:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  461, 460
 * SCR Title:  Add 1394 Hub capability to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   SVO 4.42 Release
 * added D1_Hub support
 * changed constants to enums
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.257   20 Aug 2004 09:04:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  433
 * SCR Title:  Upgrade SVObserver to 4.40
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   upped to Release 4.40
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.256   16 Aug 2004 07:46:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  433
 * SCR Title:  Upgrade SVObserver to 4.40
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   upped to beta 18
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.255   12 Aug 2004 08:10:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  433
 * SCR Title:  Upgrade SVObserver to 4.40
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   upped to beta 17
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.254   11 Aug 2004 13:25:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  433
 * SCR Title:  Upgrade SVObserver to 4.40
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   upped to 4.40 beta 16
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.253   11 Aug 2004 10:16:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  433
 * SCR Title:  Upgrade SVObserver to 4.40
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   upped to 4.40 Beta 15
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.252   10 Aug 2004 10:51:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  432
 * SCR Title:  Fix SVObserver to allow Image Compression to work via the ActiveX
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problem so that ImageCompression will not startup or get deleted on a second copy of SVObserver.  
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.251   06 Aug 2004 12:09:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  433
 * SCR Title:  Upgrade SVObserver to 4.40
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   SVO 4.40 beta 14
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.250   30 Jul 2004 13:25:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  433
 * SCR Title:  Upgrade SVObserver to 4.40
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   4.40 Beta 13
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.249   23 Jul 2004 08:35:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  433
 * SCR Title:  Upgrade SVObserver to 4.40
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   upped to 4.40 Beta 12
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.248   22 Jul 2004 09:19:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  433
 * SCR Title:  Upgrade SVObserver to 4.40
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   upped to beta 11
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.247   21 Jul 2004 09:05:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  433
 * SCR Title:  Upgrade SVObserver to 4.40
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   4.40 Beta 10
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.246   16 Jul 2004 11:30:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  433
 * SCR Title:  Upgrade SVObserver to 4.40
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Upped to 4.40 Beta 9
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.245   12 Jul 2004 13:16:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   4.40 beta 8
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.244   12 Jul 2004 13:15:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added UnitTests
 * added OnUpdateAddExternalTool
 * added INI()
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.243   18 Jun 2004 08:38:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  415
 * SCR Title:  Upgrade SVObserver to 4.31
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Upped to Release 4.31
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.242   16 Jun 2004 15:41:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  415
 * SCR Title:  Upgrade SVObserver to 4.31
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   upped to beta 77
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.241   16 Jun 2004 07:58:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  415
 * SCR Title:  Upgrade SVObserver to 4.31
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   upped to beta 76
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.240   15 Jun 2004 14:08:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  415
 * SCR Title:  Upgrade SVObserver to 4.31
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   upped to beta 75
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.239   11 Jun 2004 14:48:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  415
 * SCR Title:  Upgrade SVObserver to 4.31
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   upped to beta 74
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.238   11 Jun 2004 07:56:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  415
 * SCR Title:  Upgrade SVObserver to 4.31
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   upped to beta 73
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.237   09 Jun 2004 11:14:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  292
 * SCR Title:  Add new ActiveX interfaces that duplicate the existing ones with less memory
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   upped to beta 72
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.236   02 Jun 2004 07:49:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  415
 * SCR Title:  Upgrade SVObserver to 4.31
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   upped to beta 69
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.235   01 Jun 2004 14:29:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  409
 * SCR Title:  Fix loading of a configuration from the Run directory.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   in RemoveUnusedFiles, cleaned up m_svFileNames.  
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.234   28 May 2004 08:33:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  415
 * SCR Title:  Upgrade SVObserver to 4.31
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   upped to beta 68
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.233   27 May 2004 11:48:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  424
 * SCR Title:  Fix Locking and Clean-up Problems with Remote Font Manager for Matrox OCR/OCV
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated both the InitInstance and ExitInstance to start-up and shut-down the Remote File Manager at the appropriate time.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.232   18 May 2004 14:57:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  415
 * SCR Title:  Upgrade SVObserver to 4.31
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   upped to Beta 67
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.231   14 May 2004 08:38:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  419
 * SCR Title:  Add SVObserver status message to the Event Viewer
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added event message to be logged to the event viewer
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.230   04 May 2004 12:50:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  415
 * SCR Title:  Upgrade SVObserver to 4.31
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated beta number to 66.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.229   19 Apr 2004 12:46:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  415
 * SCR Title:  Upgrade SVObserver to 4.31
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed to 4.31 Beta 64
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.228   01 Apr 2004 14:51:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  408
 * SCR Title:  Fix Go Online problem with ISG camera
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problems with reading of the camera.  now it will only read, before it read and reset the camera info.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.227   22 Mar 2004 13:25:26   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  409
 * SCR Title:  Fix loading of a configuration from the Run directory.
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed loading from the run directory to not delete unused files until all the objects have been parsed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.227   22 Mar 2004 13:25:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  409
 * SCR Title:  Fix loading of a configuration from the Run directory.
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed loading from the run directory to not delete unused files until all the objects have been parsed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.226   20 Feb 2004 12:21:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated SVObserver version number to 4.30 Release
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.225   17 Feb 2004 16:59:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Beta 98
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.224   16 Feb 2004 16:34:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated to version 4.30 Beta 97.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.223   13 Feb 2004 11:19:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problem with exiting camera manager - Light Reference values now updated properly
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.222   12 Feb 2004 16:49:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed bug with Camera Manager
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.221   11 Feb 2004 17:31:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Beta 96 RC3
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.220   11 Feb 2004 14:55:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  400
 * SCR Title:  Fix Bug with Configuration saving (wrong path for the camera file)
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fix Memory leak associated with previous code check-in in OpenSECFile
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.219   10 Feb 2004 15:17:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Beta 95 Release Candidate 2
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.218   10 Feb 2004 14:08:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  400
 * SCR Title:  Fix Bug with Configuration saving (wrong path for the camera file)
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed OpenSECFile to keep track of files in use so that RemoveUnusedFiles( will not delete the IOD and IPD files from the run directory.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.217   09 Feb 2004 09:32:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Beta 94 Release Candidate 1 (for real)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.216   06 Feb 2004 10:40:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Beta 92 Release Candidate 1
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.215   05 Feb 2004 16:29:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Beta 91
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.214   04 Feb 2004 15:04:02   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Upped beta to 90
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.213   04 Feb 2004 14:25:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  403
 * SCR Title:  Add ActiveX interfaces to allow management of Matrox fonts through the SIAC
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new code to support new Matrox font setup
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.212   03 Feb 2004 17:36:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Beta 80
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.211   22 Jan 2004 17:28:20   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   really upped beta to 75
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.210   22 Jan 2004 16:23:56   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   upped beta number to 75
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.209   22 Jan 2004 12:28:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   beta 74
 * changed INI entry name
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.208   21 Jan 2004 15:36:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   beta 73
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.207   21 Jan 2004 15:30:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added check for conditional strobe start frame
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.206   20 Jan 2004 09:06:02   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Upped beta number to 72
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.205   19 Jan 2004 14:11:16   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   update beta number to 71
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.204   16 Jan 2004 07:45:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added StrobeSubsystem configuration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.203   12 Jan 2004 12:16:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   upped to beta 70
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.202   06 Jan 2004 13:37:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   upped to beta 65
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.201   24 Dec 2003 14:30:18   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Upped beta to 63.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.200   19 Dec 2003 12:31:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Upped beta to 62.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.199   19 Dec 2003 11:19:34   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Upped beta number to 61
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.198   18 Dec 2003 15:59:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed to beta 60
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.197   18 Dec 2003 15:41:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed to beta 59
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.196   17 Dec 2003 14:14:20   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  381
 * SCR Title:  Combine PPQ Data to Inspection Data for a Digital Output
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed Trigger Toggle and other outputs that are written at times different than the rest of the outputs so that they no longer conflict.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.195   17 Dec 2003 09:22:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated beta number to 4.30 beta 58.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.194   17 Dec 2003 09:18:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  389
 * SCR Title:  Present Error Messages for Invalid Hardware using User Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated INILoad method to use a different Model Number entry in the INI file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.193   16 Dec 2003 14:20:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  389
 * SCR Title:  Present Error Messages for Invalid Hardware using User Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated hardware check to allow SVIM .5 configuration to go-online on a Full SVIM.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.192   15 Dec 2003 12:55:24   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Upped beta number to 57
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.191   10 Dec 2003 13:55:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated to beta 56
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.190   10 Dec 2003 11:28:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed to beta 55
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.189   08 Dec 2003 15:04:52   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed to beta 54
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.188   05 Dec 2003 14:41:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed to Beta 53
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.187   03 Dec 2003 14:11:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed beta to 52
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.186   24 Nov 2003 09:46:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   4.30 beta 51
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.185   21 Nov 2003 10:36:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed beta to 4.30 Beta 50
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.184   19 Nov 2003 13:13:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  388
 * SCR Title:  Update SVObserver to Version 4.25 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated to 4.25 Release
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.183   19 Nov 2003 10:35:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed beta to 45
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.182   19 Nov 2003 09:46:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed GetSVIMType to deal with color system
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.181   18 Nov 2003 14:17:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed to Beta 44
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.180   18 Nov 2003 07:04:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed beta number to 43
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.179   17 Nov 2003 08:31:52   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed beta to 430B42
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.178   17 Nov 2003 07:46:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  389
 * SCR Title:  Present Error Messages for Invalid Hardware using User Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated mesages in OnGoOnline method when a failure occurs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.177   12 Nov 2003 13:56:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  396
 * SCR Title:  Add New Product Types for 1394 SVIM
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed product enums
 * added GetBoardType
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.176   11 Nov 2003 10:46:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  389
 * SCR Title:  Present Error Messages for Invalid Hardware using User Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated InitInstance method to handle issue with splash screen when program was executed in debug mode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.175   10 Nov 2003 08:32:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  389
 * SCR Title:  Present Error Messages for Invalid Hardware using User Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated INILoad method to check for correct acquisition card instead of checking if any Coreco acquisition card was present.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.174   04 Nov 2003 09:38:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed errors
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.173   04 Nov 2003 09:26:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved function DisplayCameraManager from SVOCameraDlg
 * moved functions DisconnectToolsetBuffers / ConnectToolsetBuffers / DisconnectAllCameraBuffers / ConnectCameraBuffers from SVOConfigAssistantDlg
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.172   03 Nov 2003 16:25:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   4.30 beta 40
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.171   31 Oct 2003 08:28:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   4.30 beta 39
 * fixed BOOL/HRESULT problem
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.170   29 Oct 2003 15:18:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  389
 * SCR Title:  Present Error Messages for Invalid Hardware using User Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to add messages for the user interface when the system starts and when the system goes on-line.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.169   22 Oct 2003 09:35:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   4.30 beta 38
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.168   01 Oct 2003 09:46:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated beta number
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.167   29 Sep 2003 11:51:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed OEMINFO.INI model check
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.166   26 Sep 2003 10:19:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added check for bad OEMINFO.INI model number
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.165   24 Sep 2003 12:24:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated beta number
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.164   22 Sep 2003 15:03:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated beta number
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.163   17 Sep 2003 09:12:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated to Beta 33
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.162   16 Sep 2003 08:59:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated INILoad and INIClose methods to fix the change in intreface objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.161   10 Sep 2003 10:36:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated version number to 4.30 beta 32.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.160   03 Sep 2003 10:22:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated INILoad method to fix issues with Trigger Source when Coreco card is used.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.159   02 Sep 2003 11:04:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated Version number to 4.30 beta 31.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.158   02 Sep 2003 09:51:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  328
 * SCR Title:  Upgrade SVObserver to latest released version of MIL
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added change to allow for correct exit of program if MIL version is incorrect ( see SVMatroxAppManager.cpp )
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.157   26 Aug 2003 12:50:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352, 322
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated information to use SVIM model number to configure system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.156   19 Aug 2003 12:57:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed to Beta 28
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.155   18 Aug 2003 15:53:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated functions that use the SVIOConfigurationInterfaceClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.154   13 Aug 2003 14:19:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   update to 4.30 beta 26
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.153   07 Aug 2003 12:49:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   4.30 Beta 25
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.152   01 Aug 2003 12:46:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   update beta number
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.151   31 Jul 2003 10:25:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed SVOINIClass parameter order
 * changed mpsvImaging->AddBoard to AddSubsystem
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.150   11 Jul 2003 08:41:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.149   11 Jul 2003 08:38:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added INIOpen, INIReset, and INIClose methods to the SVObserverApp class to handle issue with Coreco software.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.148   11 Jul 2003 08:32:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated InitInstance and ExitInstatnce method to remove display system handle from SVObserverApp class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.147   08 Jul 2003 11:10:20   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upped beta 4.22 to 7 and fixed problem with .sec files being deleted from the run directory on a load.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.146   08 Jul 2003 10:55:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322, 320
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated local information in most methods to hande changes in SVImageProcessingClass and new I/O Interface Configuration Objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.145   08 Jul 2003 10:50:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  371
 * SCR Title:  Remove DOS Focus COM support
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed code that was only used for SVCommRC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.144   25 Jun 2003 09:30:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  371
 * SCR Title:  Remove DOS Focus COM support
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed methods of initing the SVCommRC
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.143   25 Jun 2003 09:13:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  371
 * SCR Title:  Remove DOS Focus COM support
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed the CommPort support for DOS Focus.  Made check in InitInstance to limit the running of SVObserver to 1 instance
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.142   18 Jun 2003 17:04:56   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upped beta number to 5
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.141   17 Jun 2003 15:05:58   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  362
 * SCR Title:  Streaming data - not all data being sent
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Restructured the Streaming Data code. It also now makes a copy during the callback which allows the data to not remain locked to cause contention later.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.140   17 Jun 2003 14:56:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upped Beta number to 4 to release to Emhart
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.139   11 Jun 2003 13:49:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  359
 * SCR Title:  Version Check on Configurations that are saved with  SVX extensions
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added a check into the OpenSVXFile to check the version of the configuration before it is loaded.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.138   02 Jun 2003 14:56:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed version to Beta 1 for the next release
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.137   29 May 2003 13:44:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed to Release 4.21
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.136   28 May 2003 14:22:40   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated beta version to 20
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.135   27 May 2003 16:07:02   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated beta to 19 and readded accidentally removed serialization code from the mask editor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.134   23 May 2003 13:08:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed to version 4.21 Beta 18
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.133   21 May 2003 17:56:58   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated the beta version to 17.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.132   15 May 2003 16:23:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   upped beta to 16
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.131   14 May 2003 12:12:58   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   upped Beta to 15
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.130   14 May 2003 12:09:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  341
 * SCR Title:  DDE Input and Outputs are no longer working correctly
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed bug so that the Module Ready output will only be written if it is assigned as an output.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.129   09 May 2003 11:52:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated beta number to 14
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.128   08 May 2003 14:45:14   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed Beta number to 13
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.127   07 May 2003 13:16:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   update version number to 4.21 Beta 12
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.126   02 May 2003 09:49:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the Beta number to 11
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.125   25 Apr 2003 11:15:12   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upgraded version to Beta 3, assorted changes plus DDE fixes for germany
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.124   22 Apr 2003 13:17:32   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.123   22 Apr 2003 11:49:48   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.122   11 Apr 2003 09:46:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  345
 * SCR Title:  Inspection Toolbar goes away when editing a configuration
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   In DestroySEC before destroying the configuration, will turn off the inspection toolbar.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.121   10 Apr 2003 10:49:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  344
 * SCR Title:  Fix Bug in SVImageCompressionClass so shutdown is clean
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed bug in InitInstance by moving SVImageCompressionClass after afxOleInit so it can be initialized correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.120   09 Apr 2003 16:05:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  344
 * SCR Title:  Fix Bug in SVImageCompressionClass so shutdown is clean
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added SVImageCompressionClass CreateResourcePool to InitInstance.
 * Added SVImageCompressionClass::DestroyResourcePool to ExitInstance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.119   31 Mar 2003 09:41:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  319
 * SCR Title:  Update SVObserver to Version 4.20 Release
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated version to 4.20 Release.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.118   28 Mar 2003 10:24:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  319
 * SCR Title:  Update SVObserver to Version 4.20 Release
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated to beta 26
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.117   27 Mar 2003 10:07:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  319
 * SCR Title:  Update SVObserver to Version 4.20 Release
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated version to 4.20 Beta 25.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.116   25 Mar 2003 11:52:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  338
 * SCR Title:  Fix Bug  in PutSVIMFile in version SVO 4.xx
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated version number to 4.20 Beta 24.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.115   24 Mar 2003 17:44:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  319
 * SCR Title:  Update SVObserver to Version 4.20 Release
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated version to 4.20 Beta 23.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.114   24 Mar 2003 12:15:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  319
 * SCR Title:  Update SVObserver to Version 4.20 Release
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated version number to 4.20 Beta 22.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.113   20 Mar 2003 16:08:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  338
 * SCR Title:  Fix Bug  in PutSVIMFile in version SVO 4.xx
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated version number to 4.20 Beta 21.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.112   20 Mar 2003 12:20:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  319
 * SCR Title:  Update SVObserver to Version 4.20 Release
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated version to 4.20 beta 20
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.111   20 Mar 2003 11:44:20   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  331
 * SCR Title:  Makes changes to the Security system to prohibit users from renaming files
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed code to allow extra utilities to be available while online.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.110   20 Mar 2003 09:27:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  319
 * SCR Title:  Update SVObserver to Version 4.20 Release
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated beta to 19
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.109   19 Mar 2003 16:32:46   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  319
 * SCR Title:  Update SVObserver to Version 4.20 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed Beta from 17 to 18
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.108   19 Mar 2003 15:20:34   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  332
 * SCR Title:  Module Ready not available after being unassigned and other small I/O bugs
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed some problems with input/outputs and the Go online/Go offline process. Also, made changes to force the PPQ to always have valid inputs and outputs even without an inspection attached.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.107   18 Mar 2003 17:16:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  332
 * SCR Title:  Module Ready not available after being unassigned and other small I/O bugs
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to the GoOnline procedure to make sure things back out successfully when there is an error.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.106   18 Mar 2003 16:59:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  319
 * SCR Title:  Update SVObserver to Version 4.20 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed the beta from 16 to 17.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.105   17 Mar 2003 08:38:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  319
 * SCR Title:  Update SVObserver to Version 4.20 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed beta from 15 to 16
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.104   14 Mar 2003 14:44:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  319
 * SCR Title:  Update SVObserver to Version 4.20 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed the Beta from 14 to 15
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.103   14 Mar 2003 07:07:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  336
 * SCR Title:  Change Light Reference and LUT dialogs on RGB mono system
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added GetSVIMType
 * Changed ShowConfigurationAssistant to initialize Config Assistant dialog with configuration type if not a new configuration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.102   13 Mar 2003 09:43:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  319
 * SCR Title:  Update SVObserver to Version 4.20 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed the Beta from 13 to 14
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.101   07 Mar 2003 14:53:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  319
 * SCR Title:  Update SVObserver to Version 4.20 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated Version Number to 4.20 Beta 13
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.100   06 Mar 2003 10:59:18   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  332
 * SCR Title:  Module Ready not available after being unassigned and other small I/O bugs
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Forced the ResultView to update after removing inputs from the PPQBar. Also changed code to not go online if validating the inspection failed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.99   05 Mar 2003 15:50:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  319
 * SCR Title:  Update SVObserver to Version 4.20 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated version number to 4.20 Beta 12.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.98   05 Mar 2003 13:55:32   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  331
 * SCR Title:  Makes changes to the Security system to prohibit users from renaming files
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed security and menus as requested by Germany.
 * Some users won't be able to rename files on the File Open/Save Dialogs. Other users lost or gained certain privileges.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.97   27 Feb 2003 16:13:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  319
 * SCR Title:  Update SVObserver to Version 4.20 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.96   27 Feb 2003 16:03:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  319
 * SCR Title:  Update SVObserver to Version 4.20 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated constructor to change version to 4.20 Beta 10.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.95   26 Feb 2003 09:04:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   removed reference to 1394 and TestSVIM until after 4.20 release
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.94   26 Feb 2003 08:00:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added Digital SVIM option and Test SVIM
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.93   25 Feb 2003 10:56:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   no change
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.92   24 Feb 2003 13:49:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  319
 * SCR Title:  Update SVObserver to Version 4.20 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated verions number in constructor method to 4.20 Beta 9.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.91   21 Feb 2003 14:27:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  319
 * SCR Title:  Update SVObserver to Version 4.20 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated version number to 4.20 Beta 8.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.90   19 Feb 2003 10:47:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  319
 * SCR Title:  Update SVObserver to Version 4.20 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated version number to 4.20 Beta 7.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.89   12 Feb 2003 10:26:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  319
 * SCR Title:  Update SVObserver to Version 4.20 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated version number in constructor to 4.20 Beta 6.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.88   10 Feb 2003 13:19:52   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  316, 321
 * SCR Title:  Add option to extent acquisition image source buffers to the length of the PPQ
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   chnaged where the PPQ's input list is rebuilt.  After coming out of the ConfigAssistantDlg, it will rebuild the input list for each PPQ.  
 * 
 * Added the registy entry for Source Image Depth.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.87   06 Feb 2003 10:26:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Modified to use new ImageProcessing stuff
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.86   30 Jan 2003 15:13:10   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated IsMonochromeImageAvailiable method to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.85   22 Jan 2003 09:25:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   made changes is ShowConfigurationAssistant method for when it is determining the current product.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.84   10 Jan 2003 09:51:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added OnUpdateEditAdjustLut
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.83   03 Jan 2003 13:56:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the order of determing the product
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.82   02 Jan 2003 15:30:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added Lut debug / testing code
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.81   18 Dec 2002 11:55:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  300
 * SCR Title:  Create an interface for SVObserver to Push Output data to a SIAC Client
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added member variable SVCommandHelperClass m_cmdControlHelper to be used with Streaming Data when SVObserver goes online.  
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.80   09 Dec 2002 18:09:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  310
 * SCR Title:  Update SVObserver to Version 4.01 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number in constructor to 4.01 Release.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.79   02 Dec 2002 13:05:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  312
 * SCR Title:  Fix Archive Tool operation
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserverApp::OnStopAll method to include a GOING_OFFLINE broadcast message to all object in the object manager.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.78   02 Dec 2002 13:00:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  303
 * SCR Title:  Unable to save a configuration to a UNC path
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserverApp fileSaveAsSEC and SetSECFullFileName methods to handle errors correctly based on reason for saving.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.77   02 Dec 2002 12:55:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  310
 * SCR Title:  Update SVObserver to Version 4.01 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserverApp constructor to version 4.01 Beta 11 and updated InitInstance method to include the creation of the C:\TEMP directory upon starting the SVObserver Application.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.76   20 Nov 2002 09:42:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226, 272
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Major file-wide changes including:
 *             Changes due to architecture change
 *             File loading and saving
 *             Change from COleServerDoc to CDocument
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.74   04 Apr 2002 10:11:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  286
 * SCR Title:  Upgrade SVObserver to Version 3.37 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update version number in SVObserverApp::SVObserverApp and also added debug information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.73   31 Jan 2002 13:14:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  279
 * SCR Title:  Upgrade SVObserver to Version 3.36 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated application constructor with the new version number 3.36 release.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.72   16 Jan 2002 10:03:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  279
 * SCR Title:  Upgrade SVObserver to Version 3.36 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number in SVObserverApp constructor to 3.36 Beta 1.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.71   13 Dec 2001 15:19:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  260
 * SCR Title:  Upgrade SVObserver to Version 3.35 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Version number to 3.35 Released.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.70   11 Dec 2001 16:48:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  260
 * SCR Title:  Upgrade SVObserver to Version 3.35 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 3.35 Release.
 * 
 * Updated InitInstance to change which COM registration file is used based on whether a Debug or Release version is being compiled.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.69   14 Nov 2001 08:33:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  251
 * SCR Title:  Inconsistent/Incorrect display of OCR file names
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed version back to 3.34 Release instead of beta
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.68   12 Nov 2001 11:28:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  229
 * SCR Title:  Upgrade SVObserver to Version 3.34 Release
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Updated current version to 3.34 Beta 26.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.67   09 Nov 2001 13:16:16   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  251
 * SCR Title:  Inconsistent/Incorrect display of OCR file names
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to correct configuration save as problem
 * in function ::SetSECFullFileName
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.66   06 Nov 2001 14:28:38   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  253
 * SCR Title:  Fix Occasional startup problem locating resources & registering com objects.
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the following functions: SVObserverApp::InitInstance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.65   31 Oct 2001 07:25:46   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  249
 * SCR Title:  Fix SVObserver memory leaks
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the following functions: SVObserverApp::InitInstance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.64   30 Oct 2001 10:28:38   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  248
 * SCR Title:  Notify user when loading a configuration from a newer version of SVObserver
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added code to SVObserverApp::OpenSECFile to check if a configurations version is compatible with the application version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.63   16 Oct 2001 09:40:44   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  230
 * SCR Title:  Fix problem when loading a config from MRU list that points to run directory
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added check to IsBadReadPtr in ValidateMRUList
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.62   11 Oct 2001 15:19:16   cSchmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  237
 * SCR Title:  Security Issue: Selecting "Cancel" on the Login Dialog.
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Modified the Login sequence to allow for the cancel button
 * and logout the user after a command is completed successfully.
 * This is what will happen with AutoLogin turned off.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.61   11 Oct 2001 09:57:36   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  230
 * SCR Title:  Fix problem when loading a config from MRU list that points to run directory
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new function that validates the existence of the configuration files listed on the MRU list. Added calls to this function at startup, shutdown, open and close of a configuration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.60   04 Oct 2001 19:00:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  229
 * SCR Title:  Upgrade SVObserver to Version 3.34 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to version 3.34 release.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.59   04 Oct 2001 17:33:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  228
 * SCR Title:  Add context information to help the debugging of multi-threaded code
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added code to store thread type identifiers in the TIB block
 * for each thread that is run. Also enabled unhandled exception handling function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.58   02 Oct 2001 15:16:32   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  239
 * SCR Title:  Color SVIM: Unable to add tools in IPD until color tool inserted into ALL IPDs
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified function, SVObserverApp::IsMonochromeImageAvailiable.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.57   18 Sep 2001 17:08:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  221
 * SCR Title:  Upgrade SVObserver to Version 3.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number in SVObserverApp::SVObserverApp to 3.33 Release.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.56   18 Sep 2001 16:52:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  221
 * SCR Title:  Upgrade SVObserver to Version 3.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserverApp::SVObserverApp and SVObserverApp::InitInstance.
 * 
 * Change the condition in which the ModelessDebug class gets included in the application.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.55   31 Aug 2001 07:43:40   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  222
 * SCR Title:  SVObserver does not delete files in run dir when cfg is loaded from NT SVFocus
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Changed the SVIMServer-CloseConfiguration function pointer from GlobalRCCloseConfiguration to GlobalRCCloseAndCleanConfiguration.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.54   15 Aug 2001 13:49:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  212
 * SCR Title:  Upgrade SVObserver to Version 3.32 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 3.32 Release.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.53   19 Jul 2001 20:53:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  213
 * SCR Title:  Upgrade Sapera Libraries from 3.00 to 3.11
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated DestroySEC method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.52   18 Jul 2001 12:38:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  213
 * SCR Title:  Upgrade Sapera Libraries from 3.00 to 3.11
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added debug dialog information to SVObserverApp.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.51   18 Jul 2001 12:36:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  215
 * SCR Title:  Fix SVObserver remote command processing problem with MS-DOS version SVFocus
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated OnRCGoOnline and OnRCGoOffline to fix missing response information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.50   22 Jun 2001 10:41:46   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  211
 * SCR Title:  Upgrade SVObserver to Version 3.31 Release
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed SVObserver version from 3.30 to 3.31
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.49   13 Jun 2001 17:04:20   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Rearranged #include statements.
 * 
 * Added the following functions: CheckCurrentUserPrivilege, OnUpdateAddAcquisitionTool, 
 *                              OnUpdateResumeFreeze, OnUpdateAddTransformationTool, 
 *																OnUpdateAddColortool,  OnRCReadState, 
 *																OnUpdateSelectPPQVariable, GetNumberOfImages, 
 *																SetupSVIMServer, OnRCOpenCurrentSEC, 
 *																SVGetCurrentConfigName, IsSVIMServerEnabled, 
 *																InitSVIMServer, DestroySVIMServer, IsColorSVIM, 
 *																IsMonochromeImageAvailiable, GetSECFullFileName, 
 *																GetSECPathName, GetSECFileName, GetSECFileNameOnly, 
 *																SetSECFullFileName,CSVObserverModule::Unlock, 
 *																CSVObserverModule::Lock,  SVObserverApp::InitATL.
 * 
 * Modified the following functions:  GlobalRCGetConfigurationName, GlobalRCGoOnline, 
 *                                  GlobalRCGoOffline, GlobalRCGetState, 
 *																		GlobalRCOpenConfiguration, GlobalRCSaveConfiguration, 
 *																		GlobalRCCloseAndCleanConfiguration, 
 *																		GlobalRCCloseConfiguration, 
 *																		GlobalRCCloseConfiguration, Serialize, SVObserverApp, 
 *																		~SVObserverApp, InitInstance, ExitInstance, CreateSEC, 
 *																		DestroySEC, IsBelonging, StoreSECToMemory, 
 *																		RefreshSystems, GetNextExistingIPDocHelper, GetIPDoc, 
 *																		AlreadyExistsIPDocTitle, CloseAllPublicDWData, 
 *																		RestoreSECFromMemory, GetDocumentData, NewIPDoc, 
 *																		LoadIODoc, Logout, OpenDocumentFile, OpenDocumentFile, 
 *																		OnFileSaveAll, OnFileSaveSec, fileSaveAsSec, 
 *																		OpenSECFile, OnFileOpenSVC, OnFileSaveAsSVC, 
 *																		OnFileCloseSec, ShowConfigurationAssistant, 
 *																		OnUpdateAdjustInternTrigger, OnUpdateEditEnvironment, 
 *																		OnRunMode, OnUpdateModeRun, OnStop, OnStopAll, 
 *																		OnSetupMode, OnUpdateSetupMode, OnGoOffline, 
 *																		OnUpdateGoOffline, OnGoOnline, OnUpdateGoOnline, 
 *																		OnRCGoOnline, OnRCGoOffline, OnAppExit, 
 *																		OnUpdateAppExit, OnUpdateModeSingleTestOfNewGrab, 
 *																		OnUpdateModeSingleTestOfCurrentGrab, 
 *																		OnUpdateFileClose, OnUpdateFileCloseSec, 
 *																		OnUpdateExtrasAdditionalEnvironmentSettings, 
 *																		OnUpdateFileNew, OnUpdateFileOpen, OnUpdateFileSave, 
 *																		OnUpdateFileSaveAll, OnUpdateFileSaveAs, 
 *																		OnUpdateFileSaveAsSec, OnUpdateFileSaveCopyAs, 
 *																		OnUpdateFileSaveImage, OnUpdateFileSaveSec, 
 *																		OnUpdateFileUpdate, OnUpdateExtrasTestoutputs, 
 *																		OnOpenRecentFile, OpenSECFileFromMostRecentList, 
 *																		OnUpdateRecentFileMenu, OnUpdateRecentFileMenu2, 
 *																		OnUpdateEditCopy, OnUpdateEditCut, OnUpdateEditDelete, 
 *																		OnUpdateEditEdittool, 
 *																		OnUpdateEditEditToolSetCondition, 
 *																		OnUpdateEditDDEOutputs, OnUpdateEditPaste, 
 *																		OnUpdateEditPasteSpecial, OnUpdateEditUndo, 
 *																		OnUpdateExtrasLightreference, 
 *																		OnUpdateExtrasLightreferenceBrightnesscontrast, 
 *																		OnUpdateExtrasRefreshrateDecrease, 
 *																		OnUpdateExtrasRefreshrateIncrease, OnUpdateFilePrint, 
 *																		OnUpdateFilePrintSec, OnUpdateFilePrintPreview, 
 *																		OnUpdateFilePrintSetup, OnUpdateHelp, 
 *																		OnUpdateHelpFinder, 
 *																		OnUpdateModeMultigrabInternaltrigger, 
 *																		OnUpdateModeMultigrabTriggered, OnUpdateModeStoptest, 
 *																		OnUpdateModeTest, OnUpdateNextPane, 
 *																		OnUpdateOleEditLinks, OnUpdateOleInsertNew, 
 *																		OnUpdateOleVerbFirst, OnUpdatePrevPane, 
 *																		OnUpdateWindowArrange, OnUpdateWindowCascade, 
 *																		OnUpdateWindowNew, OnUpdateWindowTileHorz, 
 *																		OnUpdateAddReferencetool, OnUpdateAddWindowtool,
 *																		OnUpdateAddPolarUnwrapTool, OnUpdateAddGageTool, 
 *																		OnUpdateAddBuildReferenceTool,OnUpdateAddProfileTool, 
 *																		OnUpdateAddLoadImageTool, OnUpdateAddImageTool, 
 *																		OnUpdateAddArchiveTool, OnUpdateAddRotateTool, 
 *																		OnUpdateAddRoundWindowTool, OnUpdateAddRoundWindowTool, 
 *																		OnUpdateAddMathTool, void OnUpdateAddStatisticsTool,  
 *																		OnViewToolBuffers, OnUpdateViewToolBuffers, 
 *																		OnUpdateAppAbout, OnUpdateEditAdjustLightReference, 
 *																		OnExtrasLogin, OnUpdateExtrasLogin, 
 *																		OnUpdateExtrasUserManager, OnUpdateExtrasUserManager, 
 *																		OnFilePrintSec, OnRCLoadSEC, OnRCSaveAllAndGetSEC, 
 *																		OnRCCloseAndCleanUpDownloadDirectory, OnRCClose, 
 *																		OnUpdateAllIOViews, OnUpdateModeSingleGrab, 
 *																		OnUpdateModeMultiGrab, OnAdjustInternTrigger, 
 *																		OnUpdateEditIOList,OnUpdatePublishedResultsPicker, 
 *																		OnUpdateResultsPicker.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.44.2.0   19 Apr 2001 18:44:48   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.48   11 Apr 2001 16:21:00   cSchmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Changes were made to the following member functions:
 * SerializeIODoc and LoadIODoc
 * The changes made were in the use of the .sec and .iod extensions
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.47   08 Mar 2001 13:08:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  193
 * SCR Title:  Fix intermittent problems with the remote go on-line command from SVFocusNT
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated global function GetSVIMState to inlcude the use of the new member variable SVObserverApp::WasSECLoading.
 *    Removed use of the IsSECLoading variable in Serialize method.
 * Initialized WasSECLoading in SVObserverApp constructor.     Update IsSECLoading and WasSECLoading flags in OpenSECFile method.  Updated condition in OnRunMode member function to include new WasSECLoading variable.  Updated condition in OnRCGoOnline method to include new WasLoading variable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.46   08 Mar 2001 12:41:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  192
 * SCR Title:  Restore Freeze Tool Resume and Fix Freezing Problems after Initial Load
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Replaced restricted condition in OnFreezeResume method in SVobserverApp Object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.45   25 Jan 2001 16:19:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   These changes fixed the SVObserverApp file handling and associated systems.  These systems include the .SEC, .IOD, and .IPD files as well as the camera files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.44.1.5   16 May 2001 15:45:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  204
 * SCR Title:  Upgrade SVObserver to Version 3.25 Release
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed SVObserver version from 3.24 to 3.25
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.44.1.4   16 May 2001 14:08:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  202
 * SCR Title:  Operator, delete and cut should not be available in File Open/Save Dialogs
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed all occurences of CFileDialog to CSVFileDialog to enable the new security features and privileges
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.44.1.3   10 May 2001 18:20:48   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  197
 * SCR Title:  Fix lock-ups in application when loading configuration from SVFocusNT
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed SVObserver state code in
 * GetSVIMState
 * Serialize
 * SVObserverApp
 * CreateSEC
 * DestroySEC
 * IsBelonging
 * StoreSECToMemory
 * RefreshSystems
 * GetNextExistingIPDocHelper
 * GetIPDoc
 * AlreadyExistsIPDocTitle
 * CloseAllPublicDWData
 * RestoreSECFromMemory
 * GetDocumentData
 * LoadIODoc
 * Logout
 * OpenDocumentFile
 * OnFileSaveSEC
 * fileSaveAsSec
 * OpenSECFile
 * OnFileOpenSVC
 * OnFileSaveAsSVC
 * OnFileCloseSec
 * ShowConfigurationAssisstant
 * OnTestMode
 * OnRunMode
 * OnStopAll
 * OnSetupMode
 * OnGoOffline
 * OnGoOnline
 * OnRCGoOnline
 * OnRCGoOffline
 * OnAppExit
 * OnOpenRecentFile
 * OpenSECFileFromMostRecentList
 * OnViewToolBuffers
 * OnFilePrintSec
 * OnReadState
 * OnRCSaveAllAndGetSec
 * OnRCSaveAllAndCleanUpDownloadDirectory
 * OnAdjustInternTrigger
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.44.1.2   15 Feb 2001 09:57:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  194
 * SCR Title:  Upgrade SVObserver to Version 3.23 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number in SVObserverApp constructor to 3.23 Released.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.44.1.1   15 Feb 2001 09:50:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  193
 * SCR Title:  Fix intermittent problems with the remote go on-line command from SVFocusNT
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated global function GetSVIMState to inlcude the use of the new member variable SVObserverApp::WasSECLoading.
 * Removed use of the IsSECLoading variable in Serialize method.
 * Initialized WasSECLoading in SVObserverApp constructor.
 * Update IsSECLoading and WasSECLoading flags in OpenSECFile method.
 * Updated condition in OnRunMode member function to include new WasSECLoading variable.
 * Updated condition in OnRCGoOnline method to include new WasLoading variable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.44.1.0   Feb 14 2001 13:46:42   Jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  192
 * SCR Title:  Restore Freeze Tool Resume and Fix Freezing Problems after Initial Load
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Replaced restricted condition in OnFreezeResume method in SVobserverApp Object.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.44   06 Nov 2000 13:19:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  190
 * SCR Title:  Upgrade SVObserver to Version 3.22 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update SVObserver to version 3.22 Release
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.43   24 Oct 2000 16:19:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  188
 * SCR Title:  Upgrade SVObserver to Version 3.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update SVObserver version to v3.21 Release.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.42   24 Oct 2000 15:51:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  187
 * SCR Title:  Color SVIM Problem when Adding Monochrome Tools
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   These changes remove the ability to add a monochrome tool to the tool set until a color tool gets added.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.41   06 Sep 2000 16:19:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  180, 179
 * SCR Title:  Upgrade SVObserver to Version 3.20 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update to include changes for color SVIM and version number.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.40   28 Aug 2000 16:00:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  178, 177
 * SCR Title:  Upgrade SVObserver to Version 3.19 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update FastOCR and version number.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.39   07 Aug 2000 13:33:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  176
 * SCR Title:  Upgrade SVObserver to Version 3.18 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update SVObserver version number to 3.18 Release.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.38   19 Jul 2000 10:21:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  170
 * SCR Title:  Upgrade SVObserver to Version 3.17 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number from v3.16 release to v3.17 release
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.37   26 Jun 2000 14:49:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  168
 * SCR Title:  Upgrade SVObserver to Version 3.16 Release.
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the version number from 3.15 to 3.16.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.36   19 Jun 2000 11:59:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  166
 * SCR Title:  Upgrade SVObserver to Version 3.15
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changes the version number of SVObserver from v3.14 to v3.15.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.35   15 Jun 2000 08:46:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  163
 * SCR Title:  OEM Splash Screen, Help About Logo and Application Icon
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added SVAboutDialogClass Destructor.
 * Added SVAboutDialogClass::LoadOEMInfo.
 * Added SVAboutDialogClass::HideSVRText.
 * Added SVAboutDialogClass::SetOEMText.
 * Added logoBmp variable to SVAboutDialogClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.34   14 Jun 2000 16:08:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  154
 * SCR Title:  Enable Remote commands for SVFocusNT causes crash
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised InitSVIMServer to check if the Server object is initialized.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.33   23 May 2000 11:03:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  151
 * SCR Title:  Update to Version 3.14
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   This change update the version number to 3.14.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.32   17 May 2000 17:31:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  141
 * SCR Title:  View DDE Inputs/Outputs on Results View
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised version number to 3.13 Release.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.31   16 May 2000 14:46:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  133, 132
 * SCR Title:  Add OCR Gray Scale to SVObserver and Create OCR Gray Scale Font Training
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upgrade Logical Vision WiT v5.3 to Logical Vision FastOCR v1.2.  Completely changed headers, libraries, and variable name types.  Functionality still the same as prior version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.30   11 May 2000 11:19:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  141
 * SCR Title:  View DDE Inputs/Outputs on Results View
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added ON_UPDATE_COMMAND_UI handler for ID_SELECT_PPQ_VARIABLE.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.29   07 May 2000 23:30:00   Jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  137
 * SCR Title:  Remove the need to explicitly reference specific version numbers.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Changed the Serialization () functions so that versioning does not need to be explicitly addressed unless a change actually affects file compatabillity.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.28   05 May 2000 14:47:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  119
 * SCR Title:  SVFocusNT Support
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Added IsSVIMServerEnabled method.
 * Added InitSVIMServer method.
 * Added DestroySVIMServer method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   05 May 2000 14:07:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  134
 * SCR Title:  Restricted Operator Security level
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added CheckCurrentUserPrivilege method.
 * Revised DestroySEC to correct problem with flags.
 * Revised Logout to send message 
 * SV_LOGGED_ON_USER_CHANGED.
 * Revised OnUpdateAppExit to check privileges.
 * Revised OnOpenRecentFile to use 
 * ID_FILE_OPEN_SVC_MRU command for privilege check.
 * Revised OnOpenSecFileFromMostRecentList to check privileges.
 * Revised OnExtrasLogin to send message
 * SV_LOGGED_ON_USER_CHANGED.
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.26   03 May 2000 10:15:48   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  119
 * SCR Title:  SVFocusNT Support
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Modified to accept remote commands from SVFocusNT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   19 Apr 2000 16:33:54   Jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  128
 * SCR Title:  Suport for 3.11 versioning
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   SVObserverApp::Serialize () and SVObserverApp::SVObserverApp () were  modified to support program version number 3.11 beta 1.
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   11 Apr 2000 17:52:10   Jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  121
 * SCR Title:  Support for 3.06 versioning
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Changed SVObserverApp::Serialize () to support program version number 3.06.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   23 Mar 2000 13:24:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  116
 * SCR Title:  Bug Fixes for Freeze on Reject
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added update message handler for Add Acquistion Tool and Resume Freeze menu.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   14 Mar 2000 14:42:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  101
 * SCR Title:  Versioning 3.10
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised version number to 3.10 release (0x00030AFF)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   09 Mar 2000 08:53:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  101
 * SCR Title:  Versioning 3.10
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Version number from 3.10 Beta 3 to 3.10 Beta 4
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   07 Mar 2000 14:07:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  101
 * SCR Title:  Versioning 3.10
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Changed Version number to 3.1 Beta 3.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   Mar 01 2000 12:46:28   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Revised to handle new Update Message Handler for Menu: ADD->Polar Unwrap Tool.
 *  Added some new default defines for polar unwarp tool.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   Feb 24 2000 11:51:30   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  111
 * SCR Title:  Integrate Conditional Tool Set/Tool Drawing
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Introduced new SVEnumeratedValueObject module.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   21 Feb 2000 15:16:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  104
 * SCR Title:  New Build Reference Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added OnUpdateAddTransformationTool method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   Feb 21 2000 10:50:04   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  109
 * SCR Title:  Freeze on Reject for v3.1
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Versioning to v3.10 Beta 2.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   Feb 16 2000 17:31:02   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  107
 * SCR Title:  Add More Blob Analyzer Features
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added check for Update UI menu so Tools cannot be added when in 'Test Mode'.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   16 Feb 2000 16:49:38   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  96
 * SCR Title:  Scroll/Zoom images displayed in the inspection document.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Added multi-image display to the IPD
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   08 Feb 2000 11:05:54   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  96
 * SCR Title:  Scroll/Zoom images displayed in the inspection document.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Modified InitInstance to attach SVImageViewScroll to the
 * multidoctemplate instead of SVImageViewClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   04 Feb 2000 13:31:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  101
 * SCR Title:  Versioning 3.10
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Changed version to 3.10
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   Feb 03 2000 16:33:22   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  102
 * SCR Title:  Versioning v3.05
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changed version from v3.04 to v3.05.  Corrected multiple inheritance order in System.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   Feb 01 2000 11:12:00   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  90
 * SCR Title:  Save  Operator Tool Move Priviledge in Configuration
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added code to iterate the IP Document list.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   Jan 19 2000 15:18:52   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  88
 * SCR Title:  Version 3.04 versioning changes.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes required to change v3.04 Beta 1 to v3.04 Released.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   Jan 14 2000 15:15:04   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  85
 * SCR Title:  Version 3.04 Beta 1 versioning changes.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes required for v3.03 to v3.04 Beta 1.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   Jan 04 2000 09:20:16   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  82
 * SCR Title:  Configuration Printing
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes to move Configuration printing to a separate class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   Dec 02 1999 08:36:42   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  72
 * SCR Title:  Version 3.02 versioning changes.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Change v3.01 to v3.02.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   Dec 02 1999 08:32:38   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  57
 * SCR Title:  Version 3.00 Beta 18 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changed v3.01 to v3.02.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   01 Dec 1999 17:21:46   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  68
 * SCR Title:  Defer "Go Online" processing till after configuration load is complete
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Modified OnRCLoadSEC to notify SVMainFrm that RCCommandComplete must be called when a configuration load is complete. This required adding SetNotifyComplete function to the SVMainFrmClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   Nov 30 1999 15:48:10   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  67
 * SCR Title:  Version 3.01 versioning changes.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes for v3.00 to v3.01 versioning.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   30 Nov 1999 15:38:54   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  68
 * SCR Title:  Defer "Go Online" processing till after configuration load is complete
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Modified OnRCGoOnline() to check if a configuration is currently being loaded. If so, the request is deferred by posting an "ID_RC_GO_ONLINE" message back to the main window.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   Nov 30 1999 13:49:18   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  65
 * SCR Title:  Remove obsolute class SVToolSetViewClass
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Removed references to SVToolSetViewClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:20:56   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.47   18 Nov 1999 12:44:02   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  60
 * SCR Title:  Create 'Utilities' menu option
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Modified initialization function to load the 'Utility' menu.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.46   Nov 18 1999 09:36:24   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  59
 * SCR Title:  Version 3.00 Release Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   v3.00 Release versioning changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.45   18 Nov 1999 09:15:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added code to correct incorrect PPQDataLinkInfo names when a document is renamed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.44   Nov 10 1999 12:20:56   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  57
 * SCR Title:  Version 3.00 Beta 18 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes v3.00 Beta 17 to v3.00 Beta 18.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.43   Nov 10 1999 11:55:18   steve
 * Added additional elements to the GUID*  pguidNonPrintArray [] for the Blob Analyzer.
 * 
 *    Rev 1.42   08 Nov 1999 17:14:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  22
 * SCR Title:  Port Multiple Position Queue code from program version 2.4.2
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised ShowConfigurationAssistant() to refresh PPQBar
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.41   Nov 05 1999 16:35:28   steve
 * Added string resources for Edge Analyzer printout, and added function SVObserverApp::PrintEdgeParameters() to print the detail data.  Updated function SVObserverApp::PrintDetails(), SVObserverApp:OnVirtualPrint(), SVObserverApp:PrintInputOutputList().
 * 
 *    Rev 1.40   Nov 05 1999 11:18:36   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  56
 * SCR Title:  Version 3.00 Beta 17 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   v3.00 Beta 16 to v3.00 Beta 17.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   Nov 04 1999 15:11:32   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes to eliminate a configuration restore on a configuration save operation.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   Nov 02 1999 13:32:46   steve
 * Changed name of the function PrintInfo() to PrintDetails() to avoid conflict with the CPrintInfo type and various pointers to this type.  Modified print function to include more data,
 * 
 *    Rev 1.37   Nov 02 1999 10:50:56   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  55
 * SCR Title:  Version 3.00 Beta 16 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   File Save As - repaired so file path is correctly save in Registry.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   Nov 02 1999 08:59:00   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  55
 * SCR Title:  Version 3.00 Beta 16 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes required for v3.00 Beta 15 to v3.00 Beta 16 change.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   Nov 01 1999 16:57:32   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  54
 * SCR Title:  Version 3.00 Beta 15 Versioning.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   v3.00 Beta 14 to v3.00 Beta 15
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   Oct 29 1999 18:01:12   steve
 * Added functions SVObserverApp::PrintObject, SVObserverApp::PrintInfo(), SVObserverApp::PrintChildren(), SVObserverApp::PrintFriends(), and removed SVObserverApp::PrintTool() to update the printouts,
 * 
 *    Rev 1.33   28 Oct 1999 18:37:56   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  53
 * SCR Title:  Implement Regression Test Capability in SVObserver
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Implemented Regression Test
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   Oct 26 1999 14:57:44   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  52
 * SCR Title:  Version 3.00 Beta 14 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   v3.00 Beta 13 to v3.00 Beta 14.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   Oct 22 1999 09:02:06   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  51
 * SCR Title:  Version 3.00 Beta 13 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   v3.00 Beta 12 to v3.00 Beta 13.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   21 Oct 1999 10:39:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  22
 * SCR Title:  Port Multiple Position Queue code from program version 2.4.2
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   REvised DestroySEC().
 * Revised OnFileNewSEC().
 * Revised ShowConfigurationAssistant().
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   Oct 20 1999 16:06:42   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  50
 * SCR Title:  Version 3.00 Beta 12 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changed version from v3.00 Beta 11 to v3.00 Beta 12.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   Oct 18 1999 15:35:48   steve
 * Changed printout of tool parameters to remove the ToolSet name and the Tool Name.
 * 
 *    Rev 1.27   Oct 18 1999 12:50:08   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  47
 * SCR Title:  Version 3.00 Beta 11 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes for v3.00 Beta 10 to v3.00 Beta 11.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   Oct 18 1999 11:45:50   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  35
 * SCR Title:  PPQ mode to wait for input data
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   RO - Bug Fix: Added Disconnect from DDE server while Destroying SEC
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   Oct 14 1999 15:09:28   steve
 * Added descriptive header to new function: PrintTool, used for printing configurations.  Header file modified to include the prototype under the "Printing" section.
 * 
 *    Rev 1.24   Oct 13 1999 17:05:16   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  46
 * SCR Title:  Version 3.00 Beta 10 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   RO - Changes for Zip of COMM data transfers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   Oct 12 1999 08:16:42   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  44
 * SCR Title:  Version 3.00 Beta 9 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes required to change v3.00 Beta 8 to v3.00 Beta 9.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   11 Oct 1999 11:41:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Call to SVActivatedSystemList::SetImageLists() to
 * reset the Image List pointers when a new 
 * SVAvailableSystemList is made.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   Oct 08 1999 10:54:54   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  43
 * SCR Title:  Version 3.00 Beta 7 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Versioning changes to Beta 8.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   05 Oct 1999 11:17:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Statistics Tool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   Oct 04 1999 11:32:08   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  43
 * SCR Title:  Version 3.00 Beta 7 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   V3.00 Beta 7 versioning.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   Oct 04 1999 09:40:26   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Check merge and re-checkin.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   Oct 04 1999 09:16:40   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changed OnSaveSecAs() to be sure no message thread is causing problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   Sep 29 1999 14:21:02   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  41
 * SCR Title:  Version 3.00 Beta 5 Versioning.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   v3.00 Beta 5
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   Sep 28 1999 16:48:38   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Remove Obsolete Wit setup flag.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   Sep 27 1999 19:04:38   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  38
 * SCR Title:  Version 3.00 Beta 4 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Version from 0x00030003 to 0x00030004.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   Sep 24 1999 10:00:32   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  4
 * SCR Title:  Add "Load image" option
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Load Image Tool related changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   Sep 23 1999 13:53:54   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  37
 * SCR Title:  Version 3.00 Beta 3 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   New version specific changes to Serialize methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   Sep 21 1999 08:49:44   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  27
 * SCR Title:  Results Picker
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Add 'OnUpdateUI' methods for the Result Picker and the Publish Results picker for controlling user access to the Edit Menu items.  User access control was also added to SVUser.cpp.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   Sep 16 1999 13:02:50   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   BugFix: Serialize System Class->DestroySEC() changed to build new availableSytemList, etc...
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   Sep 13 1999 13:34:54   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Replaced Update message handlger with 'Add Image Subtraction Tool' with 'Add Image Tool' Updatee Handler.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   Sep 10 1999 19:26:06   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added Update Message handler for 'Add Build Reference...'.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   10 Sep 1999 17:38:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use new SVPublishListClass.
 * Corrected problem in DestroySEC where all uniqueIds
 * were being flushed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   01 Sep 1999 11:47:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised versioning to 3.0 Beta 1
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   30 Aug 1999 19:29:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to call SVIPDoc::Validate()
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   Aug 29 1999 16:00:32   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Removed un-needed Wit Igraph code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   Aug 23 1999 14:01:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  22
 * SCR Title:  Port Multiple Position Queue code from program version 2.4.2
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Revised for new PPQ architecture.
 * Revised for RSLinx Compatibility.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Mar 1999 02:06:10   robert
 * Project:  SVO_200Custom
 * Change Request (SCR) nbr:  1
 * SCR Title:  Prepare for QUAD ( Multi-Dig. ) Systems and integrate VIPER RGB acquisition.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Changings to prepare Quad Systems and to integrate VIPER RGB
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Mar 1999 20:23:20   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  7
 * SCR Title:  Clarify the include list links, regard. have diff. versions of same project
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Changed include directive for SVCommRC.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/