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
//Moved to precompiled header: #include <google\protobuf\stubs\common.h>
#include "resource.h"
#include "GuiHelpers.h"
#include "ExtrasEngine.h"
#include "RootObject.h"
#include "SVArchiveWritingDlg.h"
#include "SVConfigurationObject.h"
#include "SVDirectX.h"
#include "SVGigeCameraManagerDlg.h"
#include "SVGlobal.h"
#include "SVIOController.h"
#include "SVIODoc.h"
#include "SVIPDoc.h"
#include "SVIPDocInfoImporter.h"
#include "SVImportedInspectionInfo.h"
#include "SVInspectionProcess.h"
#include "SVMainFrm.h"
#include "SVMessageWindow.h"
#include "SVOConfigAssistantDlg.h"
#include "SVObserver.h"
#include "SVSecurity/SVSecurityManager.h"
#include "SVObserver_i.h"
#include "SVPPQObject.h"
#include "SVRCCommand.h"
#include "SVRCWebsocketServer.h"
#include "SVStartWindow.h"
#include "SVToolSet.h"
#include "SVUtilities.h"
#include "SVVisionProcessorHelper.h"
#include "SVXFunctions.h"
#include "SoftwareTriggerDlg.h"
#include "TextDefinesSvO.h"
#include "WebSocketSettingsLoader.h"
#include "WebsocketSettings.h"
#include "CameraLibrary/SVDeviceParam.h"
#include "CameraLibrary/SVDeviceParams.h"
#include "CameraLibrary/SVLongValueDeviceParam.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/SVUserMessage.h"
#include "Definitions/StringTypeDef.h"
#include "Definitions/TextDefineSVDef.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "InspectionEngine/SVDigitizerProcessingClass.h"
#include "SVFileSystemLibrary/FilepathUtilities.h"
#include "SVFileSystemLibrary/SVFileNameManagerClass.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVLibrary/SVPackedFile.h"
#include "SVLibrary/SVWinUtility.h"
#include "SVLogLibrary/Logging.h"
#include "SVMFCControls/DisplayMessageBox.h"
#include "SVMatroxLibrary/SVMatroxApplicationInterface.h"
#include "SVMatroxLibrary/SVMatroxErrorEnum.h"
#include "SVMatroxLibrary/SVMatroxSystemInterface.h"
#include "SVMatroxLibrary/SVOLicenseManager.h"
#include "SVMessage/SVMessage.h"
#include "SVOLibrary/SVHardwareManifest.h"
#include "SVOLibrary/SVMemoryManager.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVSharedMemoryLibrary/ShareEvents.h"
#include "SVSharedMemoryLibrary/SharedMemWriter.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVSystemLibrary/SVThread.h"
#include "SVSystemLibrary/SVVersionInfo.h"
#include "SVUtilityLibrary/LoadDll.h"
#include "SVUtilityLibrary/SVClock.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary/ZipHelper.h"
#include "Triggering/SVTriggerObject.h"
#include "Triggering/SVTriggerProcessingClass.h"
#pragma endregion Includes

#pragma region Declarations

constexpr double	cNormalNonPageMemoryUsage = 80.0;		 //Value as a percentage of the total Non-Page size

#pragma endregion Declarations


class CSVObserverModule : public CAtlMfcModule
{
public:
	DECLARE_LIBID(LIBID_SVObserver)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SVOBSERVER, "{08B70D3F-F12C-11D4-A927-00106F0309AB}")
};

CSVObserverModule _Module;

SVObserverApp g_theSVObserverApp;

SVObserverApp& TheSVObserverApp()
{
	return g_theSVObserverApp;
}


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
	HMENU GetMenu() { return m_hMenuShared; }
};

// Dieser Bezeichner wurde als statistisch eindeutig für Ihre Anwendung generiert.
// Sie können ihn ändern, wenn Sie einen bestimmten Bezeichnernamen bevorzugen.
// used in .odl
// {F4C4D491-D660-11D0-9B52-00805F717DCE}
constexpr CLSID clsid =
{ 0xf4c4d491, 0xd660, 0x11d0, { 0x9b, 0x52, 0x0, 0x80, 0x5f, 0x71, 0x7d, 0xce } };

constexpr int batExecutionTimeout = 2 * 60000;

IMPLEMENT_SERIAL(SVObserverApp, CWinApp, 0);

BEGIN_MESSAGE_MAP(SVObserverApp, CWinApp)
	//{{AFX_MSG_MAP(SVObserverApp)
	//@TODO [Arvid][10.20][20.10.2021] Moved this back here because OnOpenRecentFile was not called after the first time when this was in SVMainframe.h 
	ON_COMMAND_EX_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, OnOpenRecentFile)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, OnUpdateRecentFileMenu)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#pragma region Constructor
SVObserverApp::SVObserverApp() :
	m_DataValidDelay(0),
	m_rInitialInfo(m_IniInfoHandler.GetInitialInfo())
{
	free((void*)m_pszHelpFilePath);

	m_pszHelpFilePath = _tcsdup(SvStl::GlobalPath::Inst().GetBinPath(_T("SVObserver.chm")).c_str());
	EnableHtmlHelp();

#if defined (TRACE_THEM_ALL) || defined (TRACE_SVO)
	::OutputDebugString(_T("Executing => SVObserverApp::SVObserverApp()\n"));
#endif

	m_IniInfoHandler.ResetInformation();

	// The Standard Configuration Execution Directory
	m_ConfigExePNVariableName = _T("ConfigurationExecutionPathName");

	m_ConfigExePNVariableValue = _T("");

	// The Standard Last Valid Configuration Directory
	m_LastValidConfigPNVariableName = _T("LastValidConfigurationPathName");

	m_LastValidConfigPNVariableValue = _T("");

	m_OfflineCount = 0;
	m_AutoRunDelayTime = 1000;

	m_CurrentVersion = SvSyl::SVVersionInfo::GetLongVersion();
	m_LoadingVersion = 0L;

	m_pCurrentDocument = nullptr;	// Set by current Document!!!
	m_pMessageWindow = nullptr;

	m_ConfigFileName.SetFileType(SV_SVZ_CONFIGURATION_FILE_TYPE);
	m_ConfigFileName.setExcludeCharacters(SvDef::cExcludeCharsConfigName);

}// end SVObserver ctor

SVObserverApp::~SVObserverApp()
{
}
#pragma endregion Constructor

#pragma region Public Methods

#pragma region AFX_VIRTUAL Methods
BOOL SVObserverApp::InitInstance()
{
	CWinApp::InitInstance();

	HINSTANCE ResourceInstance(nullptr);
	//Load resource dll explicitly
	HRESULT retValue = SvUl::LoadDll::Instance().getDll(SvUl::SVOResourceDll, ResourceInstance);
	if (S_OK != retValue || nullptr == ResourceInstance)
	{
		//Because our exception handler (message box) needs the resources, we have to use here the standard message box.
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_SVO_53_RESOURCE_DLL_LOADING_FAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10009_LoadOfResourceDllFailed);
		MessageBox(nullptr, SvO::LoadingResourceDllFailed, nullptr, MB_OK | MB_ICONSTOP);
		exit(-SvStl::Err_10009_LoadOfResourceDllFailed);
	}

	SvStl::MessageManager::setShowDisplayFunction(SvMc::DisplayMessageBox::showDialog);

	// load File based write filter DLL. SVObserver will function normally (except for FBWF functionally, of course) if "fbwflib.dll" is not found
	SvUl::LoadDll::Instance().getDll(SvO::FbwfDllName, ExtrasEngine::ms_FbwfDllInstance);

	//Set the resource instance to the resource dll
	AfxSetResourceHandle(ResourceInstance);

	m_hEvent = CreateEvent(nullptr, false, true, AfxGetAppName());

	if (ERROR_ALREADY_EXISTS == GetLastError())
	{
		return false;
	}

	// OLE-Bibliotheken initialisieren
	if (!AfxOleInit())
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_OleInitFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10125);
		return false;
	}

	if (!InitATL())
	{
		return false;
	}

	//Initializing  must be before first use of  MessageNotification::FireNotify which is i.e called from CheckDrive 
	startInstances();
	SVDirectX::Instance().Initialize();

	// Check for proper setup of V: for SVRemoteControl
	if (S_OK != CheckDrive(SvStl::GlobalPath::Inst().GetPathOnRamDrive().c_str()))
	{
		return false;
	}

#ifdef _DEBUG
	long l_CrtVal = GetPrivateProfileInt(_T("Memory"), _T("CrtSetBreakAlloc"), 0, SvStl::GlobalPath::Inst().GetBinPath("SVDebug.ini").c_str());

	if (0 < l_CrtVal)
	{
		::_CrtSetBreakAlloc(l_CrtVal);
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


	InitializeSecurity();

	// the SVIPDoc constructor will create
	//
	// Make sure the dialog color matches 'system' choice.
	//
#ifdef _DEBUG
	SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
#else
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
#endif

	SetThreadPriority(THREAD_PRIORITY_NORMAL);

	// The start window	
	SVStartWindow sWin;
#ifndef _DEBUG                    // 23 Mar 1999 - frb.
	if (sWin.Create(IDD_START))
	{
		sWin.ShowWindow(SW_SHOW);
	}
	else
		return FALSE;
#endif //_DEBUG                  // 23 Mar 1999 - frb.

	AfxEnableControlContainer();

	// Registry Key...
	SetRegistryKey(_T("SVR Gesellschaft für Bildverarbeitung mbH"));

	LoadStdProfileSettings(7);  // Standard-INI-Dateioptionen einlesen (einschließlich MRU)

	SvUl::LoadDll::Instance().setDefaultPath(SvStl::GlobalPath::Inst().GetBinPath(_T("\\")));

	ValidateMRUList();

	SvSyl::SVThread::SetDiagnostic(m_IniInfoHandler.GetInitialInfo().m_diagnostic);

	// Get SourceImageDepth
	m_lSouceImageDepth = GetProfileInt(_T("Settings"), _T("Source Image Depth"), -1);
	if (m_lSouceImageDepth == -1)
	{
		WriteProfileInt(_T("Settings"), _T("Source Image Depth"), SvDef::cDefaultSourceImageDepth);

		m_lSouceImageDepth = SvDef::cDefaultSourceImageDepth;
	}

	// Get LogDataManager
	m_LogDataManager = UpdateAndGetLogDataManager();

	// Supplement for SVObserver 2.0
	// Set up path name environment variables...

	// Configuration Execution Path Name... ( "Run Directory" )
	m_ConfigExePNVariableValue = SvStl::GlobalPath::Inst().GetRunPath(_T("\\")).c_str();

	// Update registry...
	WriteProfileString(_T("Settings"), m_ConfigExePNVariableName, m_ConfigExePNVariableValue.c_str());

	// Check path, create if necessary but don't delete contents...
	InitPath(m_ConfigExePNVariableValue.c_str(), true, false);
	SetEnvironmentVariable(m_ConfigExePNVariableName, m_ConfigExePNVariableValue.c_str());

	// Last Valid Configuration Path Name... ( "Last Valid Directory" )

	m_LastValidConfigPNVariableValue = SvStl::GlobalPath::Inst().GetLastValidPath(_T("\\")).c_str();
	WriteProfileString(_T("Settings"), m_LastValidConfigPNVariableName, m_LastValidConfigPNVariableValue.c_str());

	// Check path, create if necessary but don't delete contents...
	InitPath(m_LastValidConfigPNVariableValue.c_str(), true, false);
	SetEnvironmentVariable(m_LastValidConfigPNVariableName, m_LastValidConfigPNVariableValue.c_str());

	// Get AutoRunDelay from Registry...
	m_AutoRunDelayTime = GetProfileInt(_T("Settings"), _T("Auto Run Delay Time"), m_AutoRunDelayTime);
	WriteProfileInt(_T("Settings"), _T("Auto Run Delay Time"), m_AutoRunDelayTime);

	if (S_OK != m_IniInfoHandler.GetInitializationStatusFlags())
	{
		SvDef::StringVector msgList;
		msgList.push_back(m_rInitialInfo.ModelNumberString());
		msgList.push_back(m_rInitialInfo.InitializationFailureDescription(m_IniInfoHandler.GetInitializationStatusFlags()));

#ifndef _DEBUG                    // 23 Mar 1999 - frb.
		sWin.ShowWindow(SW_HIDE);
#endif //_DEBUG                  // 23 Mar 1999 - frb.

		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SVObserver_InitializationFailure, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10126);
	}


	//check to see what licenses are available before setting up any documents
	SVOLicenseManager::Instance().InitLicenseManager();

#ifdef LUT_DEBUG
	// temp debugging code EB 2002 12 20
	SVLutTestCases test;
#endif

	AddDocTemplate(CreateIpDocMultiDocTemplate());

	AddDocTemplate(CreateIoDocMultiDocTemplate());


	// Haupt-MDI-Rahmenfenster erzeugen
	SVMainFrame* pMainFrame = new SVMainFrame;
	if (nullptr == pMainFrame) { return FALSE; }

	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		return false;
	}
	m_pMainWnd = pMainFrame;

	//m_pMainWnd has already been checked for null due to pMainFrame check
	SVVisionProcessorHelper::Instance().setMainhWnd(m_pMainWnd->GetSafeHwnd());
	// Load Utilities Menu
	SVUtilities util;
	CMenu* pMenu = util.FindSubMenuByName(m_pMainWnd->GetMenu(), _T("&Utilities"));
	if (nullptr != pMenu)
	{
		util.LoadMenu(pMenu);
	}

	// DDE-Execute-Open aktivieren
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Close Start Window
	sWin.DestroyWindow();

	TheSVMemoryManager().InitializeMemoryManager(SvDef::ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME,
		SvDef::ARCHIVE_TOOL_MEMORY_POOL_ONLINE_ASYNC_NAME, m_IniInfoHandler.GetInitialInfo().m_archiveToolBufferSize, m_IniInfoHandler.GetInitialInfo().m_archiveToolAsyncBufferSize);

	// Das Hauptfenster ist initialisiert und kann jetzt angezeigt und aktualisiert werden.
#ifdef _DEBUG
	//pMainFrame->ShowWindow(SW_SHOWNORMAL);           // 29 Mar 1999 - frb.
	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);  //m_nCmdShow
#else
	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);  //m_nCmdShow
#endif
	pMainFrame->UpdateWindow();

	//
	// Init user list...

	Logout();

	SVSVIMStateClass::AddState(SV_STATE_AVAILABLE);

	bool StartLastConfiguration = false;
	RootObject::getRootChildValue(SvDef::FqnEnvironmentStartLastConfig, StartLastConfiguration);
	// Load last loaded configuration, if necessary...
	if (StartLastConfiguration)
	{
		// Delay Auto Run to allow other applications to start...
		Sleep(m_AutoRunDelayTime);
		// Load Last Configuration...
		::PostMessage(m_pMainWnd->m_hWnd, WM_COMMAND, SV_AUTO_RUN_LAST_MRU, 0);
	}

	// add message to event viewer - SVObserver Started
	SvDef::StringVector msgList;
	msgList.push_back(SvSyl::SVVersionInfo::GetShortTitleVersion());
	SvStl::MessageManager Info(SvStl::MsgType::Log);
	Info.setMessage(SVMSG_SVO_25_SVOBSERVER_STARTED, SvStl::Tid_Version, msgList, SvStl::SourceFileParams(StdMessageParams));

	ExtrasEngine::Instance().SetAutoSaveEnabled(m_IniInfoHandler.GetInitialInfo().m_enableAutosave);

	m_DataValidDelay = m_IniInfoHandler.GetInitialInfo().m_dataValidDelay;

	///This sets the reserved text size for text value objects which can change in size
	int maxTextSize = (0 == m_IniInfoHandler.GetInitialInfo().m_maxTextSize) ? SvDef::cMaxStringByteSize : m_IniInfoHandler.GetInitialInfo().m_maxTextSize;
	SvVol::SVStringValueObjectClass::setMaxTextSize(maxTextSize);


	if (!SVOLicenseManager::Instance().HasMatroxLicense())
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_52_NOMATROX_DONGLE, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25013_NoMatroxDongle);
	}

	if (SVOLicenseManager::Instance().HasMatroxLicense() && !SVOLicenseManager::Instance().HasMatroxGigELicense() && IsMatroxGige())
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_MatroxGigELicenseNotFound, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10127);
	}

	return true;
}

void SVObserverApp::UpdateAllIOViews()
{
	if (GetTheIODoc() &&
		SVSVIMStateClass::CheckState(SV_STATE_READY | SV_STATE_RUNNING) &&
		!SVSVIMStateClass::CheckState(SV_STATE_CANCELING))
	{
		GetTheIODoc()->UpdateAllViews(nullptr);
	}
}

BOOL SVObserverApp::OnIdle(LONG lCount)
{
	BOOL bMore = CWinApp::OnIdle(lCount);

	return bMore;
}

CDocument* SVObserverApp::OpenDocumentFile(LPCTSTR)
{
	return nullptr;
}

void SVObserverApp::Serialize(CArchive& ar)
{
	CWinApp::Serialize(ar);
}

int SVObserverApp::ExitInstance()
{
	stopInstances();

	SVClassRegisterListClass::Instance().Shutdown();
	SVDirectX::Instance().clear();

	ValidateMRUList();

#if !defined( _DEBUG )
	// Display close window	
	m_pMessageWindow = new SVMessageWindow;
	if (m_pMessageWindow && m_pMessageWindow->Create(IDD_MESSAGE_DIALOG))
	{
		m_pMessageWindow->ShowWindow(SW_SHOW);
	}
#else
	m_pMessageWindow = nullptr;
#endif


	//add message to event viewer - SVObserver stopped
	SvStl::MessageManager Msg(SvStl::MsgType::Log);
	Msg.setMessage(SVMSG_SVO_26_SVOBSERVER_STOPPED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
	CloseHandle(m_hEvent);

#if !defined(_WIN32_WCE) || defined(_CE_DCOM)
	if (m_ATLInited)
	{
		_Module.RevokeClassObjects();
	}
#endif

	//! This is required due to the ResultTabbedView using the CMFCTabCtrl which generates this instance and needs to be deleted when app exits
	if (nullptr != CMFCVisualManager::GetInstance())
	{
		delete CMFCVisualManager::GetInstance();
	}

	// Optional:  Delete all global objects allocated by libprotobuf.
	google::protobuf::ShutdownProtobufLibrary();
	return CWinApp::ExitInstance();
}

int SVObserverApp::Run()
{
	int Result(0);

	try
	{
		Result = CWinApp::Run();
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		//This is the topmost catch for MessageContainer exceptions
		SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
		//Set the error code to unhandled exception but use the rest of the data from the original exception
		SvStl::MessageData Msg(rExp.getMessage());
		std::string OrgMessageCode = SvUl::Format(_T("0x%08X"), Msg.m_MessageCode);
		Msg.m_AdditionalTextId = SvStl::Tid_Default;
		SvDef::StringVector msgList;
		msgList.push_back(OrgMessageCode);
		Msg.m_AdditionalTextList = msgList;
		Msg.m_MessageCode = SVMSG_SVO_72_UNHANDLED_EXCEPTION;
		Result = Msg.m_MessageCode;
		Exception.setMessage(Msg);
	}
	return Result;
}
#pragma endregion AFX_Virtual methods


#pragma region virtual
HRESULT SVObserverApp::OpenFile(LPCTSTR PathName, bool editMode /*= false*/, ConfigFileType fileType /*= ConfigFileType::SvzStandard*/) 
//@TODO [Arvid][10.20][18.10.2021]: this function is too long
{
	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szFile[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];

	_tsplitpath(PathName, szDrive, szDir, szFile, szExt);
	std::string Extension = szExt;
	std::string OriginalFile{ szFile };
	std::string FileName{ PathName };
	std::string loadPath{ szDrive };
	loadPath += szDir;

	// @Note: [SEJ] If DestroyConfig doesn't return S_OK, the MRU is updated incorrectly
	// The file to be loaded, if it was in the MRU, will be removed from the MRU !
	HRESULT result = DestroyConfig();
	if (S_OK != result)
	{
		return result;	//keep the cancel state so it does not remove from MRU
	}

	// If not loading from the run path then delete all files in that path
	if (0 != SvUl::CompareNoCase(loadPath, std::string(SVFileNameManagerClass::Instance().GetRunPathName() + "\\")))
	{
		// Clean up Execution Directory...
		// Check path, create if necessary and delete contents...
		InitPath(std::string(SVFileNameManagerClass::Instance().GetRunPathName() + "\\").c_str(), true, true);
	}

	//Is the file of type packed config then we need to first unzip the file into the run folder
	if (0 == SvUl::CompareNoCase(Extension, std::string(SvDef::cPackedConfigExtension)))
	{
		SvDef::StringVector RunFiles;
		if (SvUl::unzipAll(FileName, SVFileNameManagerClass::Instance().GetRunPathName(), RunFiles))
		{
			//Find the svx file
			for (const auto& rFile : RunFiles)
			{
				_tsplitpath(rFile.c_str(), szDrive, szDir, szFile, szExt);
				Extension = szExt;
				if (0 == SvUl::CompareNoCase(Extension, std::string(SvDef::cConfigExtension)))
				{
					m_SvxFileName.SetFullFileName(rFile.c_str());
					//When this is set then we need to rename the file to the name of the svx file
					if (fileType == ConfigFileType::SvzFormatDefaultName)
					{
						std::string oldFileName{ FileName };
						SvUl::searchAndReplace(FileName, OriginalFile.c_str(), szFile);
						::rename(oldFileName.c_str(), FileName.c_str());
						OriginalFile = m_SvxFileName.GetFileNameOnly();
					}
					break;
				}
			}
		}
		else
		{
			SvDef::StringVector msgList;
			msgList.emplace_back(FileName);
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnzipFileFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
			return E_FAIL;
		}
	}
	else if (0 == SvUl::CompareNoCase(Extension, std::string(SvDef::cConfigExtension)))
	{
		m_SvxFileName.SetFullFileName(FileName.c_str());
		SVFileNameManagerClass::Instance().LoadItem(&m_SvxFileName);
		//In the future we want to use the new file format
		SvUl::searchAndReplace(FileName, SvDef::cConfigExtension, SvDef::cPackedConfigExtension);
	}

	if (0 == SvUl::CompareNoCase(Extension, std::string(SvDef::cConfigExtension)))
	{
		if (ConfigFileType::SvzStandard == fileType)
		{
			setConfigFullFileName(FileName.c_str(), true);
			SVRCSetSVCPathName(getConfigFullFileName().c_str());
		}
		else
		{
			m_ConfigFileName.SetFileNameOnly(OriginalFile.c_str());
			m_ConfigFileName.SetExtension(SvDef::cPackedConfigExtension);
			m_ConfigFileName.SetPathName(nullptr);

			RootObject::setRootChildValue(SvDef::FqnEnvironmentConfigurationFileName, m_ConfigFileName.GetFileName());
			RootObject::setRootChildValue(SvDef::FqnEnvironmentConfigurationName, OriginalFile);
		}
		result = OpenSVXFile();
		//Only if loaded locally then add to recent list
		if (editMode && S_OK == result)
		{
			FileName = getConfigFullFileName();
			if (0 == _access(FileName.c_str(), 0))
			{
				AddToRecentFileList(FileName.c_str());
			}
		}
	}
	else
	{
		result = E_INVALIDARG;
	}

	if (S_OK == result)
	{
		if (editMode && !TheSecurityManager().SVIsSecured(SECURITY_POINT_MODE_MENU_EDIT_TOOLSET))
		{
			SetModeEdit(true);
		}
		else
		{
			SetModeEdit(false);
		}
	}

	return result;
}


HRESULT SVObserverApp::OpenSVXFile()
{
	HRESULT hr = S_OK;

	bool bOk = true; 

	while (1)
	{
		SVOLicenseManager::Instance().ClearLicenseErrors();

		SVSVIMStateClass::changeState(SV_STATE_UNAVAILABLE | SV_STATE_LOADING, SV_STATE_AVAILABLE);

		try
		{
			bool isGlobalInit = true;

			try
			{
				//avoid that TRC-memory will be recreated for every loading step, but do it once at the end.
				SvOi::getTriggerRecordControllerRWInstanceThrow().setGlobalInit();
			}
			catch (const SvStl::MessageContainer& rExp)
			{
				SvStl::MessageManager Exception(SvStl::MsgType::Log);
				Exception.setMessage(rExp.getMessage());
				isGlobalInit = false;
			}

			hr = LoadSvxFile(m_CurrentVersion, m_SvxFileName.GetFullFileName(), getConfigFullFileName(), GetSVIMType(), GetMainFrame(), isGlobalInit);

			if (hr & SvDef::svErrorCondition)
			{
				// If there was an error during configuration loading...
				SVSVIMStateClass::changeState(SV_STATE_AVAILABLE, SV_STATE_UNAVAILABLE | SV_STATE_LOADING);

				//Use E_FAIL to stop the loading but do not show any error messages
				if (E_FAIL != hr)
				{
					SvDef::StringVector msgList;
					msgList.push_back(SvUl::Format(_T("%d"), hr));
					SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
					Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_ConfigurationLoadFailed, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10129);
				}
				return S_FALSE;
			} // if (hr & 0xc000)

			auto fileName = saveObjectIdMapping();
			if (!fileName.empty())
			{
				AddFileToConfig(fileName.c_str());
			}

			//Finished loading notify load
			_variant_t configName;
			configName.SetString(getConfigFullFileName().c_str());
			SVVisionProcessorHelper::Instance().FireNotification(SvPb::NotifyType::configLoaded, configName);
		} // try
		catch (CUserException* pUE)
		{
			delete pUE;

			bOk = false;

			SVSVIMStateClass::RemoveState(SV_STATE_LOADING);

			DestroyConfig(false, true);

			setConfigFullFileName(nullptr, true);

			SVSVIMStateClass::changeState(SV_STATE_AVAILABLE, SV_STATE_UNAVAILABLE);
		}  // catch

		break;
	} // while (1)

	if (!bOk)
	{
		setConfigFullFileName(nullptr, true);
		SVSVIMStateClass::changeState(SV_STATE_AVAILABLE, SV_STATE_UNAVAILABLE | SV_STATE_LOADING);
	}

	if (S_OK == hr)
	{
		hr = bOk ? S_OK : S_FALSE;
	}

	HideTabsAfterOpenSvxFile(GetTheIODoc());

	return hr;
}


#pragma endregion virtual

HRESULT SVObserverApp::LoadPackedConfiguration(LPCTSTR pFileName, ConfigFileType fileType)
{
	HRESULT l_Status = S_OK;
	std::string fileName{ pFileName };

	if (0 == _access(fileName.c_str(), 0))
	{
		RCClose();
	}
	else
	{
		l_Status = E_UNEXPECTED;
	}

	if (S_OK == l_Status)
	{
		if (ConfigFileType::PackedFormat == fileType)
		{
			SVPackedFile PackedFile;
			if (PackedFile.UnPackFiles(fileName.c_str(), SvStl::GlobalPath::Inst().GetRunPath().c_str()))
			{
				if (PackedFile.getConfigFilePath().empty() || (_access(PackedFile.getConfigFilePath().c_str(), 0) != 0))
				{
					l_Status = E_UNEXPECTED;
				}
				else
				{
					fileName = PackedFile.getConfigFilePath();
				}
				fileType = ConfigFileType::SvzFormatPutConfig;
			}
			else
			{
				l_Status = E_UNEXPECTED;
			}
		}
	}

	if (S_OK == l_Status)
	{
		l_Status = OpenFile(fileName.c_str(), false, fileType);
	}

	return l_Status;
}

HRESULT SVObserverApp::SavePackedConfiguration(LPCTSTR pFileName)
{
	HRESULT Result {S_OK};

	bool bRunning = SVSVIMStateClass::CheckState(SV_STATE_RUNNING);

	if (bRunning)
	{
		StopIfRunning();
	}

	if (SVSVIMStateClass::CheckState(SV_STATE_READY))
	{
		if (false == fileSaveAsSVXWrapper(pFileName, false))
		{
			Result = E_UNEXPECTED;
		}

		if (bRunning)
		{
			EnterRunMode();
		}
	}

	return Result;
}

SVMainFrame* SVObserverApp::GetMainFrame() const
{
	return dynamic_cast<SVMainFrame*>(m_pMainWnd);
}

HRESULT SVObserverApp::CanCloseMainFrame()
{
	return DestroyConfig();
}

HRESULT SVObserverApp::DestroyConfig(bool AskForSavingOrClosing /* = true */,
	bool CloseWithoutHint /* = false */) //@TODO [Arvid][10.20][18.10.2021]: this function is much too long
{
	SVSVIMStateClass::SVRCBlocker block;
	bool bCancel = false;

	bool bOk = !SVSVIMStateClass::CheckState(SV_STATE_READY | SV_STATE_RUNNING | SV_STATE_TEST);
	HRESULT hr = S_OK;

	if (!bOk)
	{
		bool bClose = true;

		if (CloseWithoutHint)
		{
			SVSVIMStateClass::AddState(SV_STATE_CANCELING);
		}
		else
		{
			SvDef::StringVector msgList;
			msgList.push_back(getConfigFileName());
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			INT_PTR result = Msg.setMessage(SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_UserQuestionCloseConfig, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10130, SvDef::InvalidObjectId, MB_YESNO);
			bClose = IDYES == result;
			if (!bClose)
			{
				hr = ERROR_CANCELLED;
			}
		}

		if (bClose)
		{
			if (SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST))
			{
				StopSvo();
			}

			if (AskForSavingOrClosing)
			{
				// Check if current config is modified and ask for saving
				if (SVSVIMStateClass::CheckState(SV_STATE_MODIFIED))
				{
					SvDef::StringVector msgList;
					msgList.push_back(getConfigFileName());
					SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
					INT_PTR result = Msg.setMessage(SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_UserQuestionSaveChanges, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10131, SvDef::InvalidObjectId, MB_YESNOCANCEL);
					switch (result)
					{
					case IDNO:
					{
						SVSVIMStateClass::AddState(SV_STATE_CANCELING);
						ResetAllIPDocModifyFlag(false);
						break;
					}
					case IDYES:
					{
						if (S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION))
						{
							SaveConfig();
							ResetAllIPDocModifyFlag(false);

							// Check whether config saving is done.
							// If not, an error or an user cancel
							// command occured!
							bClose = !std::string(getConfigFullFileName()).empty() &&
								!SVSVIMStateClass::CheckState(SV_STATE_MODIFIED);
						}
						break;
					}
					case IDCANCEL:  // Fall through to default case.
					default:	// Don´t close config!
					{
						bCancel = true;
						bClose = false;
						break;
					}
					}// end switch( result )
				}// end if ( SVSVIMStateClass::CheckState( SV_STATE_MODIFIED ) )
			}
			else
			{
				SVSVIMStateClass::AddState(SV_STATE_CANCELING);
			}
		}// end if ( bClose )

		if (bClose)
		{
			bOk = bClose = SVSVIMStateClass::CheckState(SV_STATE_READY);
		}

		if (bClose)
		{
			_variant_t configName;
			configName.SetString(getConfigFullFileName().c_str());
			SVVisionProcessorHelper::Instance().FireNotification(SvPb::NotifyType::configUnloaded, configName);
			SVOLicenseManager::Instance().ClearLicenseErrors();
			SVSVIMStateClass::ConfigWasUnloaded();
			SVSVIMStateClass::changeState(SV_STATE_UNAVAILABLE | SV_STATE_CLOSING, SV_STATE_READY | SV_STATE_MODIFIED | SV_STATE_EDIT | SV_STATE_STOP);

			if (bOk)
			{
				CWaitCursor wait;

				wait.Restore();

				SVConfigurationObject* pConfig(nullptr);
				SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

				if (nullptr != pConfig)
				{
					RootObject* pRoot = nullptr;
					SVObjectManagerClass::Instance().GetRootChildObject(pRoot, SvDef::FqnRoot);
					if (nullptr != pRoot)
					{

						pRoot->destroyConfigurationObject(SVObjectManagerClass::Instance().GetMutex());

					}
				}

				wait.Restore();

				GetMainFrame()->SetStatusInfoText(_T(""));

				wait.Restore();

				// Destroy the current PPQ
				GetMainFrame()->DestroyPPQButtons();
				GetMainFrame()->InitToolBars();

				wait.Restore();

				SVSVIMStateClass::changeState(SV_STATE_AVAILABLE, SV_STATE_UNAVAILABLE | SV_STATE_CLOSING | SV_STATE_CANCELING);
				bOk = setConfigFullFileName(nullptr, true);

				wait.Restore();

				m_IniInfoHandler.INIReset();
			}// end if ( bOk )
		}// end if ( bClose )

	}// end if bOk = ! SVSVIMStateClass::CheckState( SV_STATE_READY | SV_STATE_RUNNING );
	else
	{
		SVConfigurationObject* pConfig(nullptr);
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

		if (nullptr != pConfig)
		{
			RootObject* pRoot = nullptr;
			SVObjectManagerClass::Instance().GetRootChildObject(pRoot, SvDef::FqnRoot);
			if (nullptr != pRoot)
			{

				pRoot->destroyConfigurationObject(SVObjectManagerClass::Instance().GetMutex());
			}
		}

		(dynamic_cast<SVMainFrame*> (GetMainWnd()))->SetStatusInfoText(_T(""));
	}

	if (S_OK == hr)
	{
		hr = bOk ? S_OK : S_FALSE;
	}

	if (bCancel)
	{
		hr = ERROR_CANCELLED;
	}
	else
	{
		DisconnectCameras();
		ConnectCameras();
	}

	::OutputDebugString(SvUl::Format(_T("Closing config %s\n"), getConfigFileName().c_str()).c_str());
	SVObjectManagerClass::Instance().listAllObjects();
	return hr;
}

void SVObserverApp::RemoveUnusedFiles()
{
	SVFileNameManagerClass::Instance().RemoveUnusedFiles();
}

bool SVObserverApp::Logout(bool)
{
	// We need to deselect any tool that might be set for operator move.
	SVMainFrame* pWndMain = dynamic_cast<SVMainFrame*> (GetMainWnd());
	if (pWndMain)
	{
		DeselectTool();

		// Logged on User changed
		pWndMain->PostMessage(SV_LOGGED_ON_USER_CHANGED, 0);
	}
	return false;
}

bool SVObserverApp::InitPath(LPCTSTR PathName, bool CreateIfDoesNotExist /*= true*/, bool DeleteContents /*= true*/)
{
	if (nullptr != PathName)
	{
		if (SVCheckPathDir(PathName, CreateIfDoesNotExist))
		{
			if (DeleteContents)
			{
				// Get Delete Directory...
				std::string DeleteName;
				SVGetPathInformation(DeleteName, PathName, SVDRIVE | SVDIR);

				// Be sure that this is not the boot, system or windows directory...
				TCHAR dirBuf[_MAX_PATH];
				dirBuf[0] = _TCHAR('\0');
				GetWindowsDirectory(dirBuf, _MAX_PATH);
				if (SvUl::CompareNoCase(DeleteName, std::string(dirBuf)))
				{
					// Not identical with windows directory...
					GetSystemDirectory(dirBuf, _MAX_PATH);
					if (SvUl::CompareNoCase(DeleteName, std::string(dirBuf)))
					{
						// Not identical with system directory...
						if (SvUl::CompareNoCase(DeleteName, std::string(_T("C:\\"))))
						{
							// Not identical with boot directory...
							// Delete contents of this directory...
							DeleteName += _T("*.*");
							return SVDeleteFiles(DeleteName.c_str(), true);
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
	bool result = (0 == SvUl::CompareNoCase(m_rInitialInfo.m_ProductName, SvDef::SVO_PRODUCT_SVIM_X2_GD2A)
		|| 0 == SvUl::CompareNoCase(m_rInitialInfo.m_ProductName, SvDef::SVO_PRODUCT_SVIM_X2_GD4A)
		|| 0 == SvUl::CompareNoCase(m_rInitialInfo.m_ProductName, SvDef::SVO_PRODUCT_SVIM_X2_GD8A)
		|| 0 == SvUl::CompareNoCase(m_rInitialInfo.m_ProductName, SvDef::SVO_PRODUCT_SVIM_NEO1));
	return result;
}

bool SVObserverApp::CheckSVIMType() const
{
	bool Result = false;

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	SVIMProductEnum SVIMType = GetSVIMType();
	//If configuration object pointer is nullptr then set the enum to invalid
	SVIMProductEnum ProductType = SVIM_PRODUCT_INVALID_TYPE;
	if (nullptr != pConfig)
	{
		ProductType = pConfig->GetProductType();
	}

	Result = SVIMType == ProductType;

	// Exceptions
	if (!Result)
	{
		switch (SVIMType)
		{
			case SVIM_PRODUCT_X2_GD1A_COLOR:
			case SVIM_PRODUCT_X2_GD2A_COLOR:
			case SVIM_PRODUCT_X2_GD4A_COLOR:
			case SVIM_PRODUCT_X2_GD8A_COLOR:
			{
				Result |= ProductType == SVIM_PRODUCT_X2_GD1A_COLOR;
				Result |= ProductType == SVIM_PRODUCT_X2_GD2A_COLOR;
				Result |= ProductType == SVIM_PRODUCT_X2_GD4A_COLOR;
				Result |= ProductType == SVIM_PRODUCT_X2_GD8A_COLOR;
				break;
			}
			case SVIM_PRODUCT_X2_GD1A:
			case SVIM_PRODUCT_X2_GD2A:
			case SVIM_PRODUCT_X2_GD4A:
			case SVIM_PRODUCT_X2_GD8A:
			{
				Result |= ProductType == SVIM_PRODUCT_X2_GD1A;
				Result |= ProductType == SVIM_PRODUCT_X2_GD2A;
				Result |= ProductType == SVIM_PRODUCT_X2_GD4A;
				Result |= ProductType == SVIM_PRODUCT_X2_GD8A;
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

	if (0 == SvUl::CompareNoCase(m_rInitialInfo.m_ProductName, SvDef::SVO_PRODUCT_SVIM_X2_GD1A))
	{
		eType = SVIM_PRODUCT_X2_GD1A;
	}
	else if (0 == SvUl::CompareNoCase(m_rInitialInfo.m_ProductName, SvDef::SVO_PRODUCT_SVIM_X2_GD2A))
	{
		eType = SVIM_PRODUCT_X2_GD2A;
	}
	else if (0 == SvUl::CompareNoCase(m_rInitialInfo.m_ProductName, SvDef::SVO_PRODUCT_SVIM_X2_GD4A))
	{
		eType = SVIM_PRODUCT_X2_GD4A;
	}
	else if (0 == SvUl::CompareNoCase(m_rInitialInfo.m_ProductName, SvDef::SVO_PRODUCT_SVIM_X2_GD8A))
	{
		eType = SVIM_PRODUCT_X2_GD8A;
	}
	else if (0 == SvUl::CompareNoCase(m_rInitialInfo.m_ProductName, SvDef::SVO_PRODUCT_SVIM_NEO1))
	{
		eType = SVIM_PRODUCT_NEO1;
	}
	return eType;
}


void SVObserverApp::ValidateMRUList()
{
	if (!::IsBadReadPtr(m_pRecentFileList, sizeof(LPVOID)))
	{
		int iSize = m_pRecentFileList->GetSize();
		for (int i = iSize - 1; i >= 0; i--)
		{
			bool bGone = (_access(m_pRecentFileList->m_arrNames[i], 0) != 0);
			if (bGone)
			{
				m_pRecentFileList->Remove(i);
			}
		}
	}
}

void SVObserverApp::ResetAllCounts()
{
	long l = 0;
	long lSize = 0;
	SVSVIMStateClass::SVRCBlocker block;
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	//If the pointer is a nullptr then lSize is 0
	if (nullptr != pConfig) { lSize = pConfig->GetInspectionCount(); }
	for (l = 0; l < lSize; l++)
	{
		SVInspectionProcess* pInspection = pConfig->GetInspection(l);

		if (nullptr != pInspection) { pInspection->GetToolSet()->ResetCounts(); }
	}// end for

	if (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING|SV_STATE_REGRESSION))
	{
		for (l = 0; l < lSize; l++)
		{
			SVInspectionProcess* pInspection = pConfig->GetInspection(l);
			if (nullptr != pInspection)
			{
				SvCmd::RunOnceSynchronous(pInspection->getObjectId());
			}

		}
	}
}

HRESULT SVObserverApp::SetMode(unsigned long lNewMode) //@TODO [Arvid][10.20][18.10.2021]: this function is too long
{
	HRESULT l_hr = S_OK;

	SvPb::DeviceModeType Mode = SvPb::DeviceModeType_IsValid(lNewMode) ? static_cast<SvPb::DeviceModeType> (lNewMode) : SvPb::DeviceModeType::unknownMode;

	if (SVSVIMStateClass::CheckState(SV_STATE_START_PENDING | SV_STATE_STARTING | SV_STATE_STOP_PENDING | SV_STATE_STOPING))
	{
		l_hr = SVMSG_50_MODE_CHANGING_ERROR;
	}
	else if (SVSVIMStateClass::CheckState(SV_STATE_LOADING))
	{
		l_hr = SVMSG_51_MODE_CONFIGURATION_LOADING_ERROR;
	}
	else if (SVSVIMStateClass::CheckState(SV_STATE_EDITING))
	{
		l_hr = SVMSG_52_MODE_GUI_IN_USE_ERROR;
	}
	else if (SvPb::DeviceModeType::runMode == Mode)
	{
		if (SVSVIMStateClass::CheckState(SV_STATE_TEST | SV_STATE_RUNNING))
		{
			StopSvo();
		}
		// Try to go online...
		if (false == SVSVIMStateClass::CheckState(SV_STATE_REGRESSION) && SVSVIMStateClass::CheckState(SV_STATE_READY))
		{
			try
			{
				Start(SV_STATE_RUNNING);
			}
			catch (const SvStl::MessageContainer& rExp)
			{
				SvStl::MessageManager  Exception(SvStl::MsgType::Log | SvStl::MsgType::Notify);
				Exception.setMessage(rExp.getMessage());
				l_hr = SVMSG_SVIMCMD_GO_ONLINE_FAILED;
			}
		}
		else
		{
			l_hr = SVMSG_SVIMCMD_GO_ONLINE_FAILED;
		}
	}
	else if (SvPb::DeviceModeType::stopMode == Mode)
	{
		// Go offline
		if (SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST))
		{
			StopSvo();
		}

		SetModeEdit(false);
	}
	else if (SvPb::DeviceModeType::testMode == Mode)
	{
		// If the previous mode was running then we cannot stop and 
		// go straight into test mode.  This has caused a crash in testing 
		// when two IPDs are present.  Therefore the activeX user must first
		// put the system in some offline mode and then put it into test mode
		// in two steps.  If running then return request rejected.
		if (SVSVIMStateClass::CheckState(SV_STATE_RUNNING))
		{
			l_hr = SVMSG_SVIMCMD_REQUEST_REJECTED;
		}
		else if (SVSVIMStateClass::CheckState(SV_STATE_REGRESSION))
		{
			StopSvo();
		}

		if (false == SVSVIMStateClass::CheckState(SV_STATE_TEST) && SVSVIMStateClass::CheckState(SV_STATE_READY))
		{
			Start(SV_STATE_TEST);
		}
		else
		{
			l_hr = SVMSG_SVIMCMD_REQUEST_REJECTED;
		}
	}
	else if (SvPb::DeviceModeType::regressionMode == Mode)
	{
		// Later Currently not supported through the Control
		l_hr = SVMSG_SVIMCMD_REQUEST_REJECTED;
	}
	else if (SvPb::DeviceModeType::editMode == Mode)
	{
		if (SVSVIMStateClass::CheckState(SV_STATE_TEST | SV_STATE_RUNNING))
		{
			StopSvo();
		}

		if (false == SVSVIMStateClass::CheckState(SV_STATE_REGRESSION) && SVSVIMStateClass::CheckState(SV_STATE_READY))
		{
			SetModeEdit(true);
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

	PostMessage(m_pMainWnd->m_hWnd, SV_REFRESH_STATUS_BAR, 0, 0);
	return l_hr;
}

HRESULT SVObserverApp::OnObjectRenamed(const std::string& p_rOldName, uint32_t objectId)
{
	HRESULT l_Status = S_OK;

	SvIe::SVObjectAppClass* l_pObject = dynamic_cast<SvIe::SVObjectAppClass*>(SVObjectManagerClass::Instance().GetObject(objectId));

	if (nullptr != l_pObject)
	{
		SVObjectClass* l_pInspect = l_pObject->GetInspection();

		if (nullptr != l_pInspect)
		{
			l_pInspect->OnObjectRenamed(*l_pObject, p_rOldName);
		}
		else
		{
			l_Status = E_FAIL;
		}

		SVConfigurationObject* pConfig(nullptr);
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

		if (nullptr != pConfig)
		{
			pConfig->OnObjectRenamed(*l_pObject, p_rOldName);
		}
		else
		{
			l_Status = E_FAIL;
		}
		SVIODoc* pIODoc = GetTheIODoc();
		if (pIODoc)
		{
			pIODoc->UpdateAllViews(nullptr);
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

bool SVObserverApp::SetStatusText(LPCTSTR PStrStatusText)
{
	// If PStrStatusText is a nullptr, the Main Frame deletes the last status info.
	// If no Main Frame exists, the function returns FALSE.
	// Otherwise ( and this should always be ) it returns TRUE!
	if (m_pMainWnd)
	{
		GetMainFrame()->SetStatusInfoText(PStrStatusText);
		return true;
	}
	return false;
}

const std::string& SVObserverApp::getConfigPathName() const
{
	return m_ConfigFileName.GetPathName();
}

const std::string& SVObserverApp::getConfigFileName() const
{
	return m_ConfigFileName.GetFileName();
}

const std::string SVObserverApp::getConfigFullFileName() const
{
	return m_ConfigFileName.GetFullFileName();
}

bool SVObserverApp::setConfigFullFileName(LPCTSTR csFullFileName, bool bLoadFile)
{
	m_ConfigFileName.SetFullFileName(csFullFileName);

	bool bOk = true;

	if (m_ConfigFileName.GetPathName().empty())
	{
		SVFileNameManagerClass::Instance().SetConfigurationPathName(nullptr);
	}
	else
	{
		if (0 == SvUl::CompareNoCase(m_ConfigFileName.GetPathName(), SvStl::GlobalPath::Inst().GetRunPath()))
		{
			SVFileNameManagerClass::Instance().SetConfigurationPathName(nullptr);
		}
		else
		{
			bOk = SVFileNameManagerClass::Instance().SetConfigurationPathName(m_ConfigFileName.GetPathName().c_str());
			// if this returns FALSE, unable to access specified path!
			if (!bOk)
			{
				SvDef::StringVector msgList;
				msgList.push_back(SvStl::MessageData::convertId2AdditionalText(bLoadFile ? SvStl::Tid_Load : SvStl::Tid_Save));
				msgList.push_back(m_ConfigFileName.GetPathName());
				SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
				Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_UnableConfig, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10132);
			}
		}
	}

	if (bOk)
	{
		if (!SVFileNameManagerClass::Instance().GetConfigurationPathName().empty())
		{
			AfxGetApp()->WriteProfileString(_T("Settings"), _T("ConfigurationFilePath"), SVFileNameManagerClass::Instance().GetConfigurationPathName().c_str());
		}
	}

	RootObject::setRootChildValue(SvDef::FqnEnvironmentConfigurationFileName, csFullFileName);
	RootObject::setRootChildValue(SvDef::FqnEnvironmentConfigurationName, m_ConfigFileName.GetFileNameOnly());


	return bOk;
}

bool SVObserverApp::AlreadyExistsIPDocTitle(LPCTSTR)
{
	return false;
}

bool SVObserverApp::ShowConfigurationAssistant(int /*= 3*/,
	bool bFileNewConfiguration /*= false*/) //@TODO [Arvid][10.20][18.10.2021]: this function is much too long
{
	bool bOk = false;

	// Access denied, if... // Check Edit Mode
	if (SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST))
		return false;

	if (bFileNewConfiguration)
	{
		// Clean up SVObserver
		if (S_OK != DestroyConfig())
		{
			// Needs Attention !!!
			// Should get error code if returned S_FALSE; 
			// If returned ERROR_CANCELLED, user cancelled
			return false;
		}

		RootObject* pRoot = nullptr;

		SVObjectManagerClass::Instance().GetRootChildObject(pRoot, SvDef::FqnRoot);
		if (nullptr != pRoot)
		{
			pRoot->createConfigurationObject(SVObjectManagerClass::Instance().GetMutex());
		}

		// Clean up Execution Directory...
		// Check path, create if necessary and delete contents...
		InitPath(std::string(SVFileNameManagerClass::Instance().GetRunPathName() + _T("\\")).c_str(), true, true);

		// Ensure that DestroyConfig() can do his work...
		SVSVIMStateClass::AddState(SV_STATE_READY);
	}
	else
	{
		CWaitCursor wait;

		SVSVIMStateClass::CheckState(SV_STATE_READY);
		ResetAllIPDocModifyFlag(false);
	}

	//****RPY - Start - added new AppAssistant

	SVOConfigAssistantDlg cDlg;

	SVIMProductEnum eSVIMType = GetSVIMType();

	cDlg.SetCurrentSystem(eSVIMType);

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig)
	{
		if (pConfig->GetProductType() != SVIM_PRODUCT_TYPE_UNKNOWN)
		{
			cDlg.SetConfigurationSystem(pConfig->GetProductType());
		}
		else
		{
			cDlg.SetConfigurationSystem(eSVIMType);
		}
	}
	else
	{
		cDlg.SetConfigurationSystem(eSVIMType);
	}

	SVIOBoardCapabilities l_svCapable;
	unsigned long l_lCount;
	SVIOConfigurationInterfaceClass::Instance().GetDigitalInputCount(l_lCount);
	l_svCapable.SetInputCount(l_lCount);
	SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputCount(l_lCount);
	l_svCapable.SetOutputCount(l_lCount);

	// Special code to determine the inverters 
	// and triggers based on IO board
	if (_T("10") == m_rInitialInfo.m_IOBoard)
	{
		l_svCapable.SetStrobeInverters(1);
		l_svCapable.SetTriggerInverters(1);
		l_svCapable.SetTriggerCount(1);
	}
	else if (_T("12") == m_rInitialInfo.m_IOBoard)
	{
		l_svCapable.SetStrobeInverters(1);
		l_svCapable.SetTriggerInverters(1);
		l_svCapable.SetTriggerCount(3);
	}
	else if (_T("00") == m_rInitialInfo.m_IOBoard)
	{
		// Get Trigger count from the TriggerDLL (in this case the DigitizerDLL)
		const SVIMTypeInfoStruct& info = SVHardwareManifest::GetSVIMTypeInfo(eSVIMType);
		l_svCapable.SetNonIOSVIM(info.m_MaxTriggers);
	}
	else if (_T("30") == m_rInitialInfo.m_IOBoard)
	{
		// Get Trigger count from the SVPlcIO.dll
		const SVIMTypeInfoStruct& info = SVHardwareManifest::GetSVIMTypeInfo(eSVIMType);
		l_svCapable.SetStrobeInverters(info.m_MaxTriggers);
		l_svCapable.SetTriggerInverters(info.m_MaxTriggers);
		l_svCapable.SetTriggerCount(info.m_MaxTriggers);
	}
	else
	{
		l_svCapable.SetStrobeInverters(3);
		l_svCapable.SetTriggerInverters(3);
		l_svCapable.SetTriggerCount(3);
	}

	cDlg.SetIOBoardCapabilities(l_svCapable);

	cDlg.SetNewConfiguration(bFileNewConfiguration);

	{
		SVSVIMStateClass::SetResetState stateEditing {SV_STATE_EDITING};
		if (cDlg.DoModal() == IDOK)
		{
			bOk = true;
			if (cDlg.Modified())
			{
				SVSVIMStateClass::AddState(SV_STATE_MODIFIED);
			}
			if (nullptr != pConfig)
			{
				pConfig->ClearRemoteOutputUnUsedData();
				pConfig->ValidateRemoteMonitorList();
				SVIODoc* pIODoc = GetTheIODoc();
				if (pIODoc)
				{
					pIODoc->UpdateAllViews(nullptr);
				}
			}
		}
	}

	//****RPY - End - added new AppAssistant

	// Call the application assistant dialog...

	if (bOk)
	{
		CWaitCursor wait;

		std::string NewName = cDlg.GetConfigurationName();
		if (bFileNewConfiguration)
		{
			m_ConfigFileName.SetFileNameOnly(NewName.c_str());
			m_ConfigFileName.SetExtension(SvDef::cPackedConfigExtension);

			RootObject::setRootChildValue(SvDef::FqnEnvironmentConfigurationName, m_ConfigFileName.GetFileNameOnly());

			if (nullptr != pConfig)
			{
				// Make all the PPQs build their default digital inputs
				long lPPQCount = pConfig->GetPPQCount();
				for (long lPPQ = 0; lPPQ < lPPQCount; lPPQ++)
				{
					SVPPQObject* pPPQ = pConfig->GetPPQ(lPPQ);
					assert(nullptr != pPPQ);
					pPPQ->RebuildInputList();
					pPPQ->RebuildOutputList();
				}// end for
			}
		}

		if (nullptr != pConfig)
		{
			bOk = pConfig->RebuildInputOutputLists();

			ConstructMissingDocuments();

			// Check for Imported Inspection and adjust views, window placement and conditional history
			const SVImportedInspectionInfoList& infoList = cDlg.GetImportedInspectionInfoList();
			for (SVImportedInspectionInfoList::const_iterator it = infoList.begin(); it != infoList.end(); ++it)
			{
				const SVImportedInspectionInfo& info = (*it);
				SVIPDoc* pDoc = GetIPDocByInspectionID(info.m_inspectionId);
				if (pDoc)
				{
					SVIPDocInfoImporter::Import(pDoc, info);
				}
			}
			cDlg.ClearImportedInspectionInfoList();

			if (SVOLicenseManager::Instance().HasToolErrors())
			{
				SVOLicenseManager::Instance().ShowLicenseManagerErrors();
			}
			else
			{
				//close dialog if it is up and has no errors
				SVOLicenseManager::Instance().ClearLicenseErrors();
			}

			//
			// Set the tool selected for an operator to move if any.
			// 26 Jan 2000 - frb.
			//
			AfxGetMainWnd()->PostMessage(SV_SET_TOOL_SELECTED_IN_TOOL_VIEW, (WPARAM)TRUE);

			bOk = pConfig->Activate() && bOk;
		}

		if (bFileNewConfiguration)
		{
			UpdateAllMenuExtrasUtilities();
			SVSVIMStateClass::changeState(SV_STATE_READY, SV_STATE_AVAILABLE);
		}

		// Rebuild PPQBar Buttons
		GetMainFrame()->DestroyPPQButtons();
		GetMainFrame()->BuildPPQButtons();

		// Validate Output List TB
		if (SVMSG_SVO_70_DUPLICATE_DISCRETE_OUTPUT == pConfig->ValidateOutputList())
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Exception.setMessage(SVMSG_SVO_70_DUPLICATE_DISCRETE_OUTPUT, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
		}
	}// end if DoModal == IDOK

	if (!bOk && bFileNewConfiguration)
	{
		SVSVIMStateClass::changeState(SV_STATE_AVAILABLE, SV_STATE_READY | SV_STATE_MODIFIED);
	}

	return bOk;
}

bool SVObserverApp::OkToEdit()
{
	bool Result{ false };
	if (SVSVIMStateClass::CheckState(SV_STATE_EDIT) && SVSVIMStateClass::CheckState(SV_STATE_READY))
	{
		if (TheSecurityManager().SVIsDisplayable(SECURITY_POINT_MODE_MENU_EDIT_TOOLSET))
		{
			Result = true;
		}
	}
	return Result;
}

void SVObserverApp::UpdateAllMenuExtrasUtilities()
{
	SVUtilities util;
	CMDIFrameWnd* pMainFrame;
	CString sMenuText;
	CMenu menu;

	pMainFrame = (CMDIFrameWnd*)AfxGetMainWnd();
	HMENU hMenu = pMainFrame->m_hMenuDefault;    // default menu
	menu.Attach(hMenu);
	CMenu* pMenu = &menu;
	sMenuText = _T("&Utilities");

	pMenu = util.FindSubMenuByName(pMenu, sMenuText);
	if (nullptr != pMenu)
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
		hMenu = static_cast<CMdiChildWorkaround*>(pMdiChild)->GetMenu();
		menu.Attach(hMenu);
		pMenu = &menu;

		pMenu = util.FindSubMenuByName(pMenu, sMenuText);
		if (nullptr != pMenu)
		{
			menu.Detach();
			util.LoadMenu(pMenu);
		}
		else
		{
			menu.Detach();
		}
		pMdiChild = dynamic_cast<CMDIChildWnd*>(pMdiChild->GetWindow(GW_HWNDNEXT));
	}
}

// Returns sourceImageDepth for the PPQ Length
// This is not the real sourceImageDepth.
long SVObserverApp::GetMaxPPQLength() const
{
	return m_lSouceImageDepth;
}

long SVObserverApp::GetLogDataManager() const
{
	return m_LogDataManager;
}

long SVObserverApp::UpdateAndGetLogDataManager()
{
	m_LogDataManager = GetProfileInt(_T("Settings"), _T("Log Data Manager"), -1);
	if (m_LogDataManager == -1)
	{
		WriteProfileInt(_T("Settings"), _T("Log Data Manager"), 0);

		m_LogDataManager = 0;
	}

	return m_LogDataManager;
}

void SVObserverApp::DeselectTool()
{
	SVMainFrame* pWndMain = dynamic_cast<SVMainFrame*> (GetMainWnd());
	if (pWndMain)
	{
		pWndMain->PostMessage(SV_SET_TOOL_SELECTED_IN_TOOL_VIEW, (WPARAM)FALSE);   // Deselect any tool selected for move.
	}
}

HRESULT SVObserverApp::DisplayCameraManager(SVIMProductEnum eProductType)
{
	HRESULT hr = S_OK;

	HCURSOR hCursor = ::LoadCursor(nullptr, IDC_WAIT);
	hCursor = ::SetCursor(hCursor);

	try
	{
		hr = DisconnectCameras();
	}
	catch (...)
	{
		assert(FALSE);
	}

	try
	{
		::SetCursor(hCursor);

		if (S_OK == hr)
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
		assert(FALSE);
	}

	hCursor = ::LoadCursor(nullptr, IDC_WAIT);
	hCursor = ::SetCursor(hCursor);

	ConnectCameras();

	::SetCursor(hCursor);

	return hr;
}

HRESULT SVObserverApp::DisconnectToolsetBuffers()
{
	// EB 2002 02 28
	// Added this to fix memory leak freeing MIL buffers
	// before we close Acq devices, we need to tell all toolsets to Close
	// this closes the connection between toolset images and the MIL acq image.

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	long lInspectionCount = 0;

	//If the pointer is a nullptr then the count will be 0
	if (nullptr != pConfig) { lInspectionCount = pConfig->GetInspectionCount(); }

	for (long lInspection = 0; lInspection < lInspectionCount; lInspection++)
	{
		SVInspectionProcess* pInspection = pConfig->GetInspection(lInspection);
		if (nullptr != pInspection)
		{
			if (nullptr != pInspection->GetToolSet())
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

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	long lInspectionCount = 0;
	//If the pointer is a nullptr then the count will be 0
	if (nullptr != pConfig) { lInspectionCount = pConfig->GetInspectionCount(); }

	for (long lInspection = 0; lInspection < lInspectionCount; lInspection++)
	{
		SVInspectionProcess* pInspection = pConfig->GetInspection(lInspection);
		if (nullptr != pInspection)
		{
			if (nullptr != pInspection->GetToolSet())
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
		assert(FALSE);
		hr = E_FAIL;
	}

	SvIe::SVDigitizerProcessingClass::Instance().DisconnectDevices();

	return hr;
}

HRESULT SVObserverApp::ConnectCameras()
{
	HRESULT hr = S_OK;

	SvIe::SVDigitizerProcessingClass::Instance().ConnectDevices();

	try
	{
		hr = ConnectCameraBuffers();
	}
	catch (...)
	{
		assert(FALSE);
		hr = E_FAIL;
	}

	return hr;
}

HRESULT SVObserverApp::SendCameraParameters()
{
	HRESULT hr = S_OK;
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	SvDef::StringVector Cameras;
	SvIe::SVDigitizerProcessingClass::Instance().GetAcquisitionDeviceList(Cameras);
	SvDef::StringVector::const_iterator Iter(Cameras.begin());
	for (; Cameras.end() != Iter; ++Iter)
	{
		SVDeviceParamCollection* pDeviceParams = nullptr;

		SVFileNameArrayClass* pDummyFiles = nullptr;
		SVLightReference* pDummyLight = nullptr;
		SVLut* pDummyLut = nullptr;

		SvIe::SVAcquisitionClassPtr pDevice(SvIe::SVDigitizerProcessingClass::Instance().GetAcquisitionDevice(Iter->c_str()));
		if (nullptr != pDevice && nullptr != pConfig &&
			pConfig->GetAcquisitionDevice(Iter->c_str(), pDummyFiles, pDummyLight, pDummyLut, pDeviceParams))
		{
			// Check for the camera file and camera to match.
			if (S_OK == hr)
			{
				HRESULT l_hrTmp = pDevice->IsValidCameraFileParameters(*pDeviceParams);
				if (E_NOTIMPL != l_hrTmp && S_OK != l_hrTmp)
				{
					hr = SV_CAN_GO_ONLINE_FAILURE_ACQUISITION;
				}
			}

			// Send GIGE packet size if was set from hardware.ini
			if (IsMatroxGige() && m_rInitialInfo.m_gigePacketSize != 0)
			{
				pDeviceParams->SetParameter(DeviceParamGigePacketSize, SVLongValueDeviceParam{ m_rInitialInfo.m_gigePacketSize });
				SVDeviceParam* pParam = pDeviceParams->GetParameter(DeviceParamGigePacketSize);
				if (nullptr != pParam)
				{
					pParam->SetName(DeviceParamGigePacketSize_String);
				}
			}

			if (pDeviceParams)
			{
				pDevice->SetDeviceParameters(*pDeviceParams);	// must be done before CreateBuffers (in case CameraFormat changes)
			}

			if (pDummyLight)
			{
				pDevice->SetLightReference(*pDummyLight);	// I HATE LIGHT REFERENCE
			}
			// LR needs to be redone so that it does not contain values, only references to device params.
		}
	}

	return hr;
}

HRESULT SVObserverApp::SetModeEdit(bool p_bState)
{
	HRESULT l_hr = S_OK;
	if (p_bState)
	{
		SVSVIMStateClass::changeState(SV_STATE_EDIT, SV_STATE_STOP);
	}
	else
	{
		SVSVIMStateClass::changeState(SV_STATE_STOP, SV_STATE_EDIT);

		//
		// We need to deselect any tool that might be set for operator move.
		//
		DeselectTool();
	}
	return l_hr;
}

HRESULT SVObserverApp::GetTriggersAndCounts(std::string& rTriggerCounts) const
{
	HRESULT l_hr = S_FALSE;
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig)
	{
		long l_lCount = pConfig->GetPPQCount();
		for (int l_lPPQ = 0; l_lPPQ < l_lCount; l_lPPQ++)
		{
			SVPPQObject* pPPQ = pConfig->GetPPQ(l_lPPQ);
			//Returns true when pointer valid
			if (nullptr != pPPQ)
			{
				SvTrig::SVTriggerObject* pTrigger(pPPQ->GetTrigger());
				if (nullptr != pTrigger)
				{
					l_hr = S_OK;
					std::string Temp = SvUl::Format(_T("\n%s count-%d"), pTrigger->GetName(), pTrigger->getTriggerCount());
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
	if (pos)
	{
		do
		{
			CDocTemplate* pDocTemplate = GetNextDocTemplate(pos);
			if (pDocTemplate)
			{
				POSITION posDoc = pDocTemplate->GetFirstDocPosition();
				if (posDoc)
				{
					do
					{
						CDocument* newDoc = pDocTemplate->GetNextDoc(posDoc);
						if (newDoc)
						{
							SVIPDoc* pTmpDoc = dynamic_cast <SVIPDoc*> (newDoc);
							if (nullptr != pTmpDoc)
							{
								pTmpDoc->UpdateWithLastProduct();
							}
						}
					} while (posDoc);
				}
			}
		} while (pos);
	}
}

void SVObserverApp::RunAllIPDocuments()
{
	//get list of IPDoc's.
	SVSVIMStateClass::SVRCBlocker block;
	POSITION pos = GetFirstDocTemplatePosition();
	if (pos)
	{
		do
		{
			CDocTemplate* pDocTemplate = GetNextDocTemplate(pos);
			if (pDocTemplate)
			{
				POSITION posDoc = pDocTemplate->GetFirstDocPosition();
				if (posDoc)
				{
					do
					{
						CDocument* newDoc = pDocTemplate->GetNextDoc(posDoc);
						if (newDoc)
						{
							SVIPDoc* pTmpDoc = dynamic_cast <SVIPDoc*> (newDoc);
							if (nullptr != pTmpDoc)
							{
								pTmpDoc->RunOnce();
							}
						}
					} while (posDoc);
				}
			}
		} while (pos);
	}
}

void SVObserverApp::SetAllIPDocumentsOnline()
{
	//get list of IPDoc's.
	SVSVIMStateClass::SVRCBlocker block;
	POSITION pos = GetFirstDocTemplatePosition();
	if (pos)
	{
		do
		{
			CDocTemplate* pDocTemplate = GetNextDocTemplate(pos);
			if (pDocTemplate)
			{
				POSITION posDoc = pDocTemplate->GetFirstDocPosition();
				if (posDoc)
				{
					do
					{
						CDocument* newDoc = pDocTemplate->GetNextDoc(posDoc);
						if (newDoc)
						{
							SVIPDoc* pTmpDoc = dynamic_cast <SVIPDoc*> (newDoc);

							if (nullptr != pTmpDoc)
							{
								pTmpDoc&& pTmpDoc->GoOnline();
							}
						}
					} while (posDoc);
				}
			}
		} while (pos);
	}
}

void SVObserverApp::SetAllIPDocumentsOffline()
{
	//get list of IPDoc's.

	POSITION pos = GetFirstDocTemplatePosition();
	if (pos)
	{
		do
		{
			CDocTemplate* pDocTemplate = GetNextDocTemplate(pos);
			if (pDocTemplate)
			{
				POSITION posDoc = pDocTemplate->GetFirstDocPosition();
				if (posDoc)
				{
					do
					{
						CDocument* newDoc = pDocTemplate->GetNextDoc(posDoc);
						if (newDoc)
						{
							SVIPDoc* pTmpDoc = dynamic_cast <SVIPDoc*> (newDoc);

							if (nullptr != pTmpDoc)
							{
								pTmpDoc&& pTmpDoc->GoOffline();
							}
						}
					} while (posDoc);
				}
			}
		} while (pos);
	}
}


void SVObserverApp::Start(DWORD desiredState)
{
	SVSVIMStateClass::SVRCBlocker block;

	if (false == InitialChecks(desiredState))
	{
		return;
	}

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (pConfig != nullptr)
	{
		executePreOrPostExecutionFile(pConfig->getPreRunExecutionFilePath());
	}

	SVSVIMStateClass::RemoveState(SV_STATE_EDIT | SV_STATE_STOP);

	UpdateAndGetLogDataManager();

	double startLoading = SvUl::GetTimeStamp();

	SVObjectManagerClass::Instance().ClearAllIndicator();

	assert(nullptr != pConfig);

	if (nullptr == m_pMainWnd || nullptr == pConfig)
	{
		RunAllIPDocuments();

		SvStl::MessageContainer Exception(SVMSG_SVO_54_EMPTY, SvStl::Tid_GoOnlineFailure_InvalidPointerConfig, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_45000);
		throw Exception;
	}

	bool isLocalStart {false == SVSVIMStateClass::CheckState(SV_STATE_REMOTE_CMD)};
	PrepareForStart(pConfig, desiredState, isLocalStart);

	//SvSml::SharedMemWriter::Instance().RebuildMonitorEntryMap();

	if (SVSVIMStateClass::CheckState(SV_STATE_READY))
	{
		RunInspections(pConfig, desiredState);

		std::string TriggerCounts;
		GetTriggersAndCounts(TriggerCounts);

		long loadTime = static_cast<long> (SvUl::GetTimeStamp() - startLoading);
		SvDef::StringVector msgList;
		msgList.push_back(TriggerCounts);
		msgList.push_back(SvUl::Format(_T("%d"), loadTime));
		//add go-online message to the event viewer.
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_SVO_27_SVOBSERVER_GO_ONLINE, SvStl::Tid_GoOnlineTime, msgList, SvStl::SourceFileParams(StdMessageParams));

		SVObjectManagerClass::Instance().SetState(SVObjectManagerClass::ReadOnly);

		EnableTriggerSettings(isLocalStart && SoftwareTriggerDlg::Instance().HasTriggers());

		SetThreadPriority(THREAD_PRIORITY_BELOW_NORMAL);
	}// end if

	if (SVSVIMStateClass::CheckState(SV_STATE_STOP_PENDING))
	{
		PostMessage(m_pMainWnd->m_hWnd, WM_COMMAND, MAKEWPARAM(ID_STOP, 0), 0);
	}
}


bool SVObserverApp::InitialChecks(DWORD desiredState)
{
	if (SVSVIMStateClass::CheckState(desiredState | SV_STATE_STARTING))
	{
		if (SVSVIMStateClass::CheckState(SV_STATE_START_PENDING))
		{
			SVSVIMStateClass::RemoveState(SV_STATE_START_PENDING);
		}

		return false;
	}

	if (SVSVIMStateClass::CheckState(SV_STATE_LOADING | SV_STATE_STOPING))
	{
		SVSVIMStateClass::AddState(SV_STATE_START_PENDING);
		return false;
	}

	return true;
}


void SVObserverApp::PrepareForStart(SVConfigurationObject* pConfig, DWORD desiredState, bool isLocalStart)
{
	try
	{
		HRESULT Result = PrepareCamerasAndMemory();

		SoftwareTriggerDlg::Instance().ShowWindow(SW_HIDE);
		SoftwareTriggerDlg::Instance().ClearTriggers();

		SvSml::ShareEvents::GetInstance().QuiesceSharedMemory();

		PPQMonitorList ppqMonitorList;
		pConfig->BuildPPQMonitorList(ppqMonitorList); //create MonitorlistCopies  in SvSml::SharedMemWriter

		SvSml::SharedMemWriter::Instance().CalculateStoreIds();

		//clear the shared memory.
		//this is only really necessary when the Monitorlist has changed

		SvSml::SharedMemWriter::Instance().CloseDataConnection();

		SvSml::SharedMemWriter::Instance().CreateManagment();

		CreateImageStores(pConfig, ppqMonitorList, Result, desiredState, isLocalStart);

		SvSml::SharedMemWriter::Instance().WriteMonitorList();

		if (SvSml::SharedMemWriter::Instance().GetActiveMonitorListCount() > 0)
		{
			SvSml::ShareEvents::GetInstance().SignaltReadyStatus();
		}

		if (SoftwareTriggerDlg::Instance().HasTriggers())
		{
			SoftwareTriggerDlg::Instance().SelectTrigger();
		}
	}
	catch (SvStl::MessageContainer&)
	{
		//cleanup goOnline, after fail, before exception leave this method
		SVSVIMStateClass::RemoveState(SV_STATE_START_PENDING);
		RunAllIPDocuments();
		throw;
	}
	catch (std::exception& ex)
	{
		//cleanup goOnline, after fail, before exception leave this method
		SVSVIMStateClass::RemoveState(SV_STATE_START_PENDING);
		RunAllIPDocuments();
		SvDef::StringVector msgList;
		msgList.push_back(ex.what());
		SvStl::MessageManager MesMan(SvStl::MsgType::Log);
		MesMan.setMessage(SVMSG_SVO_44_SHARED_MEMORY, SvStl::Tid_StdException, msgList, SvStl::SourceFileParams(StdMessageParams));
		MesMan.Throw();
	}
}


HRESULT SVObserverApp::PrepareCamerasAndMemory()
{
		DisconnectCameras();
		HRESULT Result = ConnectCameras();
		//Buffer allocation error
		if (SVMEE_MATROX_ALLOCATION == Result)
		{
			SvStl::MessageContainer Exception(SVMSG_SVO_NON_PAGED_MEMORY_FULL, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25022_NonPagedMemoryFull);
			throw Exception;
		}
		Result = SendCameraParameters();
		if (Result != S_OK)
		{
			SvStl::MessageContainer Exception(SVMSG_SVO_54_EMPTY, SvStl::Tid_GoOnlineFailure_SendCameraParam, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_45000);
			throw Exception;
		}

		long long MemSize(0);
		long long MemUsed(0);

		SVMatroxSystemInterface::getNonPagedMemory(MemSize, MemUsed);
		if (0 < MemSize)
		{
			double NonPagedMemUsage = static_cast<double> (MemUsed) / static_cast<double> (MemSize) * 100.0;
			//If more than the normal pecentage used then log a message
			if (cNormalNonPageMemoryUsage < NonPagedMemUsage)
			{
				SvDef::StringVector msgList;
				msgList.push_back(SvUl::Format(_T("%.0f"), cNormalNonPageMemoryUsage));
				msgList.push_back(SvUl::Format(_T("%.0f"), NonPagedMemUsage));
				SvStl::MessageManager Exception(SvStl::MsgType::Log);
				Exception.setMessage(SVMSG_SVO_NON_PAGED_MEMORY_LOW, SvStl::Tid_MoreThanPercentUsed, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25023_NonPagedMemoryLow);
			}
		}

		return Result;
}


void SVObserverApp::CreateImageStores(SVConfigurationObject* pConfig, PPQMonitorList& rPpqMonitorList, HRESULT Result, DWORD desiredState, bool isLocalStart)
{
	double preTriggerTimeWidow {0.0};
	if (SVHardwareManifest::isDiscreteIOSystem(pConfig->GetProductType()))
	{
		preTriggerTimeWidow = (0.0 == m_rInitialInfo.m_preTriggerTimeWindow) ? SvDef::cDefaultPreTriggerTimeWindow : m_rInitialInfo.m_preTriggerTimeWindow;
	}

	long lSize = pConfig->GetPPQCount();

	///In this loop the ImageStores are created 
	for (long l = 0; S_OK == Result && l < lSize; l++)
	{
		auto* pPPQ = pConfig->GetPPQ(l);
		//Returns true when pointer valid
		if (nullptr != pPPQ)
		{
			pPPQ->setPreTriggerTimeWindow(preTriggerTimeWidow);
			///Set NAK Behavior
			pPPQ->SetNAKMode(m_rInitialInfo.m_NAKMode, m_rInitialInfo.m_NAKParameter);

			// Do this before calling CanGoOnline 
			const MonitorListAttributeStruct& rActiveList = rPpqMonitorList[pPPQ->GetName()];
			pPPQ->SetMonitorList(rActiveList);
			pPPQ->SetSlotmanager(SvSml::SharedMemWriter::Instance().GetSlotManager(pPPQ->GetName()));
			bool isTestMode {SV_STATE_TEST == desiredState};
			pPPQ->PrepareGoOnline(isTestMode);

			SvTrig::SVTriggerObject* pTrigger {pPPQ->GetTrigger()};
			if (nullptr != pTrigger && SvDef::TriggerType::SoftwareTrigger == pTrigger->getType() || isTestMode)
			{
				SoftwareTriggerDlg::Instance().AddTrigger(pTrigger, isTestMode && isLocalStart);
			}
		}
	}// end for
}


void SVObserverApp::RunInspections(SVConfigurationObject* pConfig, DWORD desiredState)
{
	long lSize = pConfig->GetPPQCount();
	try
	{
		for (long l = 0; l < lSize; l++)
		{
			auto* pPPQ = pConfig->GetPPQ(l);
			if (nullptr != pPPQ)
			{
				pPPQ->GoOnline();
			}
		}// end for
	}
	catch (const SvStl::MessageContainer&)
	{
		//cleanup goOnline, after fail, before exception leave this method
		for (long l = 0; l < lSize; ++l)
		{
			auto* pPPQ = pConfig->GetPPQ(l);
			if (nullptr != pPPQ)
			{
				pPPQ->GoOffline();
			}
		}// end for

		SVSVIMStateClass::RemoveState(SV_STATE_START_PENDING);

		RunAllIPDocuments();
		SetAllIPDocumentsOffline();

		throw;
	}// end if

	SetAllIPDocumentsOnline();
	DWORD removeState = (desiredState == SV_STATE_RUNNING) ? SV_STATE_READY | SV_STATE_START_PENDING : SV_STATE_START_PENDING;
	SVSVIMStateClass::changeState(SV_STATE_UNAVAILABLE | SV_STATE_STARTING, removeState);

	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	auto* pTrcRW = SvOi::getTriggerRecordControllerRWInstance();
	if (nullptr != pTrcRW)
	{
		pTrcRW->lockReset();
	}

	SVSVIMStateClass::changeState(desiredState, SV_STATE_UNAVAILABLE | SV_STATE_STARTING);

	//Now that we are in the running state we allow trigger processing!
	StartTrigger(pConfig);

	//Module ready should be the last set!
	if (pConfig->SetModuleReady(true) != S_OK)
	{
		RunAllIPDocuments();

		SvStl::MessageContainer Exception(SVMSG_SVO_54_EMPTY, SvStl::Tid_GoOnlineFailure_ModuleReadyOutput, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_45000);
		throw Exception;
	}// end if

}

#pragma endregion Public Methods

#pragma region Protected Methods
void SVObserverApp::executePreOrPostExecutionFile(const std::string& filepath, bool inRunMode)
{
	if (std::filesystem::exists(filepath) && inRunMode)
	{
		std::string filename = std::filesystem::path(filepath).filename().generic_string();
		SHELLEXECUTEINFO ShExecInfo = {0};
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.hwnd = NULL;
		ShExecInfo.lpVerb = NULL;
		ShExecInfo.lpFile = filename.c_str();
		ShExecInfo.lpParameters = "";
		ShExecInfo.lpDirectory = SvStl::GlobalPath::Inst().GetRunPath().c_str();
		ShExecInfo.nShow = SW_HIDE;
		ShExecInfo.hInstApp = NULL;

		ShellExecuteEx(&ShExecInfo);
		DWORD returnValue = WaitForSingleObject(ShExecInfo.hProcess, batExecutionTimeout);

		if (returnValue == WAIT_TIMEOUT)
		{
			SvDef::StringVector msgList;
			msgList.push_back(filename);
			SvStl::MessageManager Msg(SvStl::MsgType::Log);
			Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_BatchfileExecutionTimeout, msgList, SvStl::SourceFileParams(StdMessageParams));
		}
	}
}

HRESULT SVObserverApp::DisconnectAllCameraBuffers()
{
	HRESULT hr = DisconnectToolsetBuffers();

	if (S_OK == hr)
	{
		hr = SvIe::SVDigitizerProcessingClass::Instance().DestroyBuffers();
	}

	return hr;
}

HRESULT SVObserverApp::ConnectCameraBuffers()
{
	HRESULT hr = S_OK;

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	SvDef::StringVector Cameras;
	SvIe::SVDigitizerProcessingClass::Instance().GetAcquisitionDeviceList(Cameras);

	if (nullptr != pConfig)
	{
		SvDef::StringVector::const_iterator Iter(Cameras.begin());
		for (; Cameras.end() != Iter; ++Iter)
		{
			SvIe::SVAcquisitionClassPtr pAcqDevice;

			pAcqDevice = SvIe::SVDigitizerProcessingClass::Instance().GetAcquisitionDevice(Iter->c_str());
			if (nullptr != pAcqDevice)
			{
				// dummy vars
				SVLightReference* pLR;
				SVFileNameArrayClass* pFiles;
				SVLut* pLut;
				SVDeviceParamCollection* pParams;

				if (pConfig->GetAcquisitionDevice(Iter->c_str(), pFiles, pLR, pLut, pParams))
				{
					SVImageInfoClass svImageInfo;
					pAcqDevice->GetImageInfo(&svImageInfo);
					hr = pAcqDevice->CreateBuffers(svImageInfo);
				}
			}
		}

		if (S_OK == hr)
		{
			hr = ConnectToolsetBuffers();
		}
	}

	return hr;
}

HRESULT SVObserverApp::InitializeSecurity()
{
	HMODULE hMessageDll{ nullptr };

	if (nullptr != (hMessageDll = LoadLibraryEx(_T("SVMessage.dll"), nullptr, LOAD_LIBRARY_AS_DATAFILE)))
	{
		// This sleep(0) was added after the LoadLibrary to fix a bug where the system ran out of resources.
		Sleep(0);
		// Add function with two parameters does not have Data.
		// Add Function with more than two parameters are data nodes.
		// These are the Security defaults if the file fails to load.
		AddSecurityNode(hMessageDll, SECURITY_POINT_FILE_MENU); // First Position in Array Not used because position 0 is not valid in the XML Security Wrapper.

		AddSecurityNode(hMessageDll, SECURITY_POINT_ROOT);
		AddSecurityNode(hMessageDll, SECURITY_POINT_FILE_MENU);     // First Child off Root
		AddSecurityNode(hMessageDll, SECURITY_POINT_FILE_MENU_NEW, _T(""));
		AddSecurityNode(hMessageDll, SECURITY_POINT_FILE_MENU_SELECT_CONFIGURATION, _T(""));
		AddSecurityNode(hMessageDll, SECURITY_POINT_FILE_MENU_CLOSE_CONFIGURATION, _T(""));
		AddSecurityNode(hMessageDll, SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION_AS, _T(""));
		AddSecurityNode(hMessageDll, SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION, _T(""));
		AddSecurityNode(hMessageDll, SECURITY_POINT_FILE_MENU_PRINT, _T(""));
		AddSecurityNode(hMessageDll, SECURITY_POINT_FILE_MENU_PRINT_SETUP, _T(""));
		AddSecurityNode(hMessageDll, SECURITY_POINT_FILE_MENU_SAVE_IMAGE, _T(""));
		AddSecurityNode(hMessageDll, SECURITY_POINT_FILE_MENU_RECENT_CONFIGURATIONS, _T(""));
		AddSecurityNode(hMessageDll, SECURITY_POINT_FILE_MENU_EXIT, _T(""));
		AddSecurityNode(hMessageDll, SECURITY_POINT_UNRESTRICTED_FILE_ACCESS, _T(""));

		AddSecurityNode(hMessageDll, SECURITY_POINT_EDIT_MENU);     // Second Child From Root
		AddSecurityNode(hMessageDll, SECURITY_POINT_EDIT_MENU_RESULT_PICKER, _T(""));

		AddSecurityNode(hMessageDll, SECURITY_POINT_VIEW_MENU);     // Third Child From Root
		AddSecurityNode(hMessageDll, SECURITY_POINT_VIEW_MENU_PPQ_BAR, _T(""));
		AddSecurityNode(hMessageDll, SECURITY_POINT_VIEW_MENU_IMAGE_DISPLAY_UPDATE, _T(""));
		AddSecurityNode(hMessageDll, SECURITY_POINT_VIEW_MENU_RESULT_DISPLAY_UPDATE, _T(""));
		AddSecurityNode(hMessageDll, SECURITY_POINT_VIEW_MENU_RESET_COUNTS_CURRENT, _T(""));
		AddSecurityNode(hMessageDll, SECURITY_POINT_VIEW_MENU_RESET_COUNTS_ALL, _T(""));

		AddSecurityNode(hMessageDll, SECURITY_POINT_MODE_MENU);     // forth Child From Root
		AddSecurityNode(hMessageDll, SECURITY_POINT_MODE_MENU_RUN, _T(""));
		AddSecurityNode(hMessageDll, SECURITY_POINT_MODE_MENU_STOP, _T(""));
		AddSecurityNode(hMessageDll, SECURITY_POINT_MODE_MENU_REGRESSION_TEST, _T(""));
		AddSecurityNode(hMessageDll, SECURITY_POINT_MODE_MENU_TEST, _T(""));
		AddSecurityNode(hMessageDll, SECURITY_POINT_MODE_MENU_EDIT_TOOLSET, _T(""));
		AddSecurityNode(hMessageDll, SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE, _T(""));

		AddSecurityNode(hMessageDll, SECURITY_POINT_EXTRAS_MENU);   // fifth Child From Root
		AddSecurityNode(hMessageDll, SECURITY_POINT_EXTRAS_MENU_ADDITIONAL_ENVIRON, _T(""));
		AddSecurityNode(hMessageDll, SECURITY_POINT_EXTRAS_MENU_SECURITY_MANAGER, _T("Administrators"), true);
		AddSecurityNode(hMessageDll, SECURITY_POINT_EXTRAS_MENU_TEST_OUTPUTS, _T(""));
		AddSecurityNode(hMessageDll, SECURITY_POINT_EXTRAS_MENU_UTILITIES_SETUP, _T(""));
		AddSecurityNode(hMessageDll, SECURITY_POINT_EXTRAS_MENU_UTILITIES_RUN, _T(""));
		AddSecurityNode(hMessageDll, SECURITY_POINT_EXTRAS_MENU_AUTOSAVE_CONFIGURATION, _T(""));
		AddSecurityNode(hMessageDll, SECURITY_POINT_EXTRAS_MENU_FBWF_CONFIGURATION, _T(""));
		TheSecurityManager().SVProtectData(SECURITY_POINT_EXTRAS_MENU_SECURITY_MANAGER); // Sets Flag that will prevent data from being changed.
	}

	TCHAR szGetBuf[256];

	GetPrivateProfileString(_T("Security"), _T("Security File Path"), SvStl::GlobalPath::Inst().GetBinPath(_T("\\Gatekpr.xml")).c_str(), szGetBuf, 256, SvStl::GlobalPath::Inst().GetSVIMIniPath());

	if (TheSecurityManager().SVLoad(szGetBuf) != S_OK)
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_SecurityFileLoadFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10133);
	}

	FreeLibrary(hMessageDll);
	// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
	Sleep(0);

	return S_OK;
}


bool SVObserverApp::fileSaveAsSVXWrapper(const std::string& rFileName, bool resetAutoSave)
{
	//When a file name is present it must have the correct extension
	if (!rFileName.empty() && std::string::npos == rFileName.find(SvDef::cPackedConfigExtension))
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_WrongPathnameEntered, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10134);
		return false;
	}

	CWaitCursor wait;
	bool result {true};
	SVSVIMStateClass::AddState(SV_STATE_SAVING);
	ResetAllIPDocModifyFlag(false);

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig)
	{
		m_SvxFileName.SetFileName(m_ConfigFileName.GetFileName().c_str());
		m_SvxFileName.SetPathName(SVFileNameManagerClass::Instance().GetRunPathName().c_str());
		m_SvxFileName.SetExtension(SvDef::cConfigExtension);
		std::string svxFilePath = m_SvxFileName.GetFullFileName();

		result = fileSaveAsSVX(pConfig, svxFilePath, rFileName, resetAutoSave);
	}

	SVSVIMStateClass::RemoveState(SV_STATE_SAVING);
	return result;
}


bool SVObserverApp::DetermineConfigurationSaveName()
{
	SVFileNameClass svFileName = m_ConfigFileName;
	svFileName.SetFileType(SV_SVZ_CONFIGURATION_FILE_TYPE);
	//Need to remove the *.svx files being visible
	svFileName.SetFileExtensionFilterList(_T("SVResearch Configuration Files (*.svz)|*.svz||"));

	if (getConfigPathName().empty() ||
		0 == SvUl::CompareNoCase(getConfigPathName(), SvStl::GlobalPath::Inst().GetRunPath()))
	{
		svFileName.SetPathName(AfxGetApp()->GetProfileString(_T("Settings"), _T("ConfigurationFilePath"), SvStl::GlobalPath::Inst().GetRunPath().c_str()));

		if (0 == SvUl::CompareNoCase(svFileName.GetPathName(), SvStl::GlobalPath::Inst().GetRunPath().c_str()))
		{
			if (!SVFileNameManagerClass::Instance().GetConfigurationPathName().empty())
			{
				svFileName.SetPathName(SVFileNameManagerClass::Instance().GetConfigurationPathName().c_str());
			}
		}
	}

	if (svFileName.SaveFile())
	{
		if (setConfigFullFileName(svFileName.GetFullFileName().c_str(), false))
		{
			AfxGetApp()->WriteProfileString(_T("Settings"), _T("ConfigurationFilePath"), svFileName.GetPathName().c_str());
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	return true;
}

void SVObserverApp::StartTrigger(SVConfigurationObject* pConfig)
{
	SVSVIMStateClass::SVRCBlocker block;
	//The pointer is usually checked by the caller
	if (nullptr != pConfig)
	{
		SvStl::MessageContainer Msg;
		long ppqCount = pConfig->GetPPQCount();
		for (long i = 0; i < ppqCount && 0 == Msg.getMessage().m_MessageCode; i++)
		{
			auto* pPPQ = pConfig->GetPPQ(i);
			if (nullptr != pPPQ)
			{
				SvTrig::SVTriggerObject* pTrigger{ pPPQ->GetTrigger() };
				if (nullptr != pTrigger)
				{
					pPPQ->setOnline();
					if (!pTrigger->Start())
					{
						SvDef::StringVector msgList;
						msgList.push_back(pTrigger->GetName());
						Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GoOnlineFailure_Trigger, msgList, SvStl::SourceFileParams(StdMessageParams));
					}
				}
			}
		}
		if (0 != Msg.getMessage().m_MessageCode)
		{
			//If an error has occurred need to clean up
			for (long i = 0; i < ppqCount; i++)
			{
				auto* pPPQ = pConfig->GetPPQ(i);
				if (nullptr != pPPQ)
				{
					pPPQ->GoOffline();
				}
			}
			throw Msg;
		}
	}
}

bool SVObserverApp::OpenConfigFileFromMostRecentList(int nID)
{
	ASSERT_VALID(this);
	assert(nullptr != m_pRecentFileList);

	assert(nID >= ID_FILE_MRU_FILE1);
	assert(nID < ID_FILE_MRU_FILE1 + (int)m_pRecentFileList->GetSize());
	int nIndex = nID - ID_FILE_MRU_FILE1;
	assert((*m_pRecentFileList)[nIndex].GetLength() != 0);

#if defined (TRACE_THEM_ALL) || defined (TRACE_SVO)
	TRACE2("MRU: open file (%d) '%s'.\n", (nIndex)+1,
		(LPCTSTR)(*m_pRecentFileList)[nIndex]);
#endif
	bool bResult = false;

	// Check Security to see if save is allowed

	// Open and read the Configuration from the Most Recent Used
	// List under the menu File.
	//
	SVFileNameClass svFileName;

	svFileName.SetFullFileName((*m_pRecentFileList)[nIndex]);

	HRESULT hr = OpenFile((*m_pRecentFileList)[nIndex]);

	//if S_OK, check to see if it has any tool errors in the license manager
	if (S_OK == hr)
	{
		if (SVOLicenseManager::Instance().HasToolErrors())
		{
			SVOLicenseManager::Instance().ShowLicenseManagerErrors();
		}
	}

	if (S_FALSE == hr)    // don't do on ERROR_CANCELLED
	{
		m_pRecentFileList->Remove(nIndex);
	}
	bResult = (S_OK == hr);

	return bResult;
}

HRESULT SVObserverApp::ConstructMissingDocuments() //@TODO [Arvid][10.20][18.10.2021]: this function is too long
{
	HRESULT l_Status(S_OK);

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig)
	{
		SVIOController* l_pIOController = pConfig->GetIOController();

		if (nullptr != l_pIOController)
		{
			if (nullptr == l_pIOController->GetIODoc())
			{
				SVIODoc* l_pIODoc(NewSVIODoc(l_pIOController->GetName(), *l_pIOController));

				if (nullptr != l_pIODoc)
				{
					l_pIODoc->UpdateAllViews(nullptr);
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

		const SVInspectionProcessVector& rInspections = pConfig->GetInspections();

		SVInspectionProcessVector::const_iterator l_Iter(rInspections.begin());

		while (S_OK == l_Status && l_Iter != rInspections.end())
		{
			SVInspectionProcess* pInspection(*l_Iter);

			if (nullptr != pInspection)
			{
				if (nullptr == GetIPDocByInspectionID(pInspection->getObjectId()))
				{
					SVIPDoc* l_pIPDoc(NewSVIPDoc(pInspection->GetName(), *pInspection));

					if (nullptr != l_pIPDoc)
					{
						l_pIPDoc->mbInitImagesByName = true;

						// Init Document
						if (!l_pIPDoc->InitAfterSystemIsDocked())
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


	return l_Status;
}

bool SVObserverApp::DestroyMessageWindow()
{
	if (m_pMessageWindow)
	{
		if (::IsWindow(m_pMessageWindow->m_hWnd))
		{
			m_pMessageWindow->DestroyWindow();
		}

		delete m_pMessageWindow;
		m_pMessageWindow = nullptr;
		return true;
	}
	return false;
}
#pragma endregion Protected Methods

#pragma region Private Methods
void SVObserverApp::startInstances()
{
	SVFileNameManagerClass::Instance().AddItem(&m_SvxFileName);

	SvStl::MessageManager::setShowDisplayFunction(SvMc::DisplayMessageBox::showDialog);
	SVVisionProcessorHelper::Instance().Startup();
	SVMatroxApplicationInterface::Startup();
	//must be called before SVDigitizerProcessingClass-Startup
	SvOi::createTriggerRecordControllerInstance(SvOi::TRC_DataType::Writer);
	SVHardwareManifest::Instance().Startup();
	SvTrig::SVTriggerProcessingClass::Instance().Startup();
	SvIe::SVDigitizerProcessingClass::Instance().Startup();

	SVObjectManagerClass::Instance().ConstructRootObject(SvPb::RootClassId);
	m_IniInfoHandler.LoadIniFilesAndDlls();

	SvLog::bootstrap_logging();
	WebSocketSettings Settings;
	WebSocketSettingsLoader settingsLoader;
	settingsLoader.loadFromIni(Settings);

	try
	{
		SvLog::init_logging(Settings.logSettings);
	}
	catch (std::runtime_error& rRuntimeError)
	{
		SV_LOG_GLOBAL(error) << std::string(rRuntimeError.what());
	}

	SV_LOG_GLOBAL(info) << "SVObserverIniPath:" << settingsLoader.GetIni();
	SvSml::ShareEvents::GetInstance().SetParameter(Settings.shareControlSettings);
	std::unique_ptr<SvHttp::HttpServerSettings>  pSettings = std::make_unique<SvHttp::HttpServerSettings>();
	*pSettings.get() = Settings.httpSettings;

	std::shared_ptr<SVRCCommand> pSVRCCommand = std::make_shared<SVRCCommand>();
	SVRCWebsocketServer::Instance()->Start(pSVRCCommand, std::move(pSettings));

	SVThreadManager::Instance();
	HINSTANCE hZipInstance {nullptr};
	SvUl::LoadDll::Instance().getDll(SvUl::ZipDll, hZipInstance);


	fillExchangeObjectId();
	fillExchangeEmbedded();
	fillEmbeddedIdFromIndirectLinked();

	SVObjectManagerClass::Instance().fitFirstObjectId();
}

void SVObserverApp::stopInstances()
{
	SVVisionProcessorHelper::Instance().Shutdown();
	SVRCWebsocketServer::Instance()->Stop();

	SvSml::ShareEvents::GetInstance().QuiesceSharedMemory();
	SvSml::SharedMemWriter::Instance().Destroy();
	DestroyMessageWindow();// Destroy still open message windows

	SVObjectManagerClass::Instance().DestroyRootObject();

	SvTrig::SVTriggerProcessingClass::Instance().Shutdown();
	SvIe::SVDigitizerProcessingClass::Instance().Shutdown();
	SVHardwareManifest::Instance().Shutdown();

	SVObjectManagerClass::Instance().Shutdown();

	SVIOConfigurationInterfaceClass::Instance().Shutdown();

	m_IniInfoHandler.INIClose();
	SvOi::destroyTriggerRecordController();
	// Shutdown MIL
	SVMatroxApplicationInterface::Shutdown();
	SvStl::MessageManager::setShowDisplayFunction(nullptr);

	// File management for config file.
	SVFileNameManagerClass::Instance().RemoveItem(&m_SvxFileName);
	SoftwareTriggerDlg::Instance().Close();
}

void SVObserverApp::StopAll()
{
	if (m_pMainWnd)
	{
		GetMainFrame()->SetStatusInfoText(_T(""));

		auto* pTrcRW = SvOi::getTriggerRecordControllerRWInstance();
		if (nullptr != pTrcRW)
		{
			pTrcRW->unlockReset();
		}
		SVSVIMStateClass::RemoveState(SV_STATE_RUNNING | SV_STATE_TEST);

		SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);

		SoftwareTriggerDlg::Instance().ClearTriggers();
	}
}

void SVObserverApp::SaveConfig(bool saveAs /*= false*/)
{
	if (saveAs || getConfigPathName().empty())
	{
		if (false == DetermineConfigurationSaveName())
		{
			return;
		}
	}
	std::string fileName {getConfigFullFileName()};

	if (fileSaveAsSVXWrapper(fileName, true))
	{
		setConfigFullFileName(fileName.c_str(), false);
		if (0 == _access(fileName.c_str(), 0))
		{
			AddToRecentFileList(fileName.c_str());
			SVSVIMStateClass::RemoveState(SV_STATE_MODIFIED);
		}

		((CMDIFrameWnd*)AfxGetMainWnd())->OnUpdateFrameTitle(TRUE);
	}
}

void SVObserverApp::EnterRunMode()
{
	try
	{
		Start(SV_STATE_RUNNING);
	}
	catch (const SvStl::MessageContainer& rExp)
	{

		SvStl::MessageManager  Exception(SvStl::MsgType::Log | SvStl::MsgType::Notify);
		Exception.setMessage(rExp.getMessage());
	}
}

void SVObserverApp::StopIfRunning()
{

	if (SVSVIMStateClass::CheckState(SV_STATE_RUNNING))
	{
		StopSvo();

		Sleep(1000);  // wait for running threads to quiesce
	}
}

void SVObserverApp::StopSvo()
{
	CWaitCursor wait;
	SVSVIMStateClass::SVRCBlocker block;

	SVArchiveWritingDlg* pArchiveWriteDlg = nullptr;

	GetMainFrame()->SetStatusInfoText(_T(""));

	EnableTriggerSettings(false);

	if (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_EDIT | SV_STATE_REGRESSION))
	{
		return;
	}

	if (SVSVIMStateClass::CheckState(SV_STATE_STOPING))
	{
		if (SVSVIMStateClass::CheckState(SV_STATE_STOP_PENDING))
		{
			SVSVIMStateClass::RemoveState(SV_STATE_STOP_PENDING);
		}
		return;
	}

	if (SVSVIMStateClass::CheckState(SV_STATE_STARTING))
	{
		SVSVIMStateClass::AddState(SV_STATE_STOP_PENDING);
		return;
	}

	if (TheSVMemoryManager().ReservedBytes(SvDef::ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME) > 0)
	{
		pArchiveWriteDlg = new SVArchiveWritingDlg;
		pArchiveWriteDlg->Create(IDD_DLG_ARCHIVETOOL_CLOSE_PROGRESS);
		pArchiveWriteDlg->ShowWindow(SW_SHOW);
		SVYieldPaintMessages();
	}

	bool wasInRunMode = SVSVIMStateClass::CheckState(SV_STATE_RUNNING);

	SVSVIMStateClass::changeState(SV_STATE_UNAVAILABLE | SV_STATE_STOPING, SV_STATE_READY | SV_STATE_RUNNING | SV_STATE_STOP_PENDING);
	auto* pTrcRW = SvOi::getTriggerRecordControllerRWInstance();
	if (nullptr != pTrcRW)
	{
		pTrcRW->unlockReset();
	}

	SVObjectManagerClass::Instance().SetState(SVObjectManagerClass::ReadWrite);

	SetThreadPriority(THREAD_PRIORITY_NORMAL);

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig)
	{
		pConfig->SetModuleReady(false);
	}

	// Signal Module Stop...
	StopAll();

	// Increment Offline Count
	m_OfflineCount++;

	long lSize = pConfig->GetPPQCount();
	for (long l = 0; l < lSize; l++)
	{
		SVPPQObject* pPPQ = pConfig->GetPPQ(l);
		if (nullptr != pPPQ) { pPPQ->GoOffline(); }
	}
	// Stop the FailStatus Stream PPQ Listeners

	SetAllIPDocumentsOffline();

	std::string TriggerCounts;
	GetTriggersAndCounts(TriggerCounts);

	//add message to event viewer - gone off-line
	SvStl::MessageManager Exception(SvStl::MsgType::Log);
	Exception.setMessage(SVMSG_SVO_28_SVOBSERVER_GO_OFFLINE, TriggerCounts.c_str(), SvStl::SourceFileParams(StdMessageParams));

	if (pConfig != nullptr)
	{
		executePreOrPostExecutionFile(pConfig->getPostRunExecutionFilePath(), wasInRunMode);
	}

	SVSVIMStateClass::changeState(SV_STATE_READY | SV_STATE_STOP, SV_STATE_UNAVAILABLE | SV_STATE_STOPING | SV_STATE_TEST | SV_STATE_REGRESSION | SV_STATE_EDIT);

	if (SVSVIMStateClass::CheckState(SV_STATE_START_PENDING))
	{
		GetMainFrame()->PostMessage(WM_COMMAND, MAKEWPARAM(ID_MODE_RUN, 0), 0);
	}

	if (pArchiveWriteDlg)
	{
		pArchiveWriteDlg->DestroyWindow();
		delete pArchiveWriteDlg;
	}
}

void SVObserverApp::RCClose()
{
	DestroyConfig(false, true);	// Close config immediately, without hint or user message...
}

bool SVObserverApp::InitATL() //@TODO [Arvid][10.20][18.10.2021]: this function is too long
{
	HRESULT l_Status(S_OK);

	bool l_AppRegister(false);
	bool l_AppUnregister(false);

	m_ATLInited = false;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	LPTSTR lpCmdLine = GetCommandLine(); //this line necessary for _ATL_MIN_CRT
	TCHAR szTokens[] = _T("-/");

	LPCTSTR lpszToken = _Module.FindOneOf(lpCmdLine, szTokens);
	while (nullptr != lpszToken)
	{
		if (lstrcmpi(lpszToken, _T("UnregServer")) == 0)
		{
			l_AppUnregister = true;
		}
		else if (lstrcmpi(lpszToken, _T("RegServer")) == 0)
		{
			l_AppRegister = true;
		}

		lpszToken = _Module.FindOneOf(lpszToken, szTokens);
	}

#if !defined(_WIN32_WCE) || defined(_CE_DCOM)
	// Register class factories via CoRegisterClassObject().
	l_Status = _Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE);
	if (SUCCEEDED(l_Status))
	{
		m_ATLInited = true;
	}
#endif // !defined(_WIN32_WCE) || defined(_CE_DCOM)

	if (FAILED(l_Status))
	{
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%08X"), l_Status));
#ifdef _DEBUG
		if (!l_AppRegister && !l_AppUnregister)
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			INT_PTR result = Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_RegisterClassObjectsFailed_Question, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10135, SvDef::InvalidObjectId, MB_YESNO);
			if (IDYES == result)
			{
				return false;
			}
		}
		else

#endif
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_RegisterClassObjectsFailed, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10136);
			return false;
		}
	}

	// App was launched with /Unregserver or /Unregister switch.
	if (l_AppUnregister)
	{
		if (SUCCEEDED(l_Status))
		{
			_Module.UpdateRegistryAppId(false);

			_Module.UnregisterServer(true);

			std::cout << _T("****** SVObserver and SVCommand Un-registered ******") << std::endl;
		}

		return false;
	}

	// App was launched with /Register or /Regserver switch.
	if (l_AppRegister)
	{
		if (SUCCEEDED(l_Status))
		{
			_Module.UpdateRegistryAppId(TRUE);

			_Module.RegisterServer(TRUE);

			std::cout << _T("***** SVObserver and SVCommand Registered *****") << std::endl;
		}

		return false;
	}

	// DON'T display a new MDI child window during startup!!!
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

	// Dispatch commands specified on the command line.  Will return false if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo)) { return false; }

	return true;
}

void SVObserverApp::StopRegression()
{
	//get list of IPDoc's.
	//see if the IPDoc is running regression, if so send command to stop.
	//since only 1 IPDoc can run regression at a time, as soon as 1 is found, break...
	SVSVIMStateClass::SVRCBlocker block;
	CDocTemplate* pDocTemplate = nullptr;
	CString strExt;
	POSITION pos = GetFirstDocTemplatePosition();
	if (pos)
	{
		bool bDone = false;
		do
		{
			pDocTemplate = GetNextDocTemplate(pos);
			if (pDocTemplate)
			{
				POSITION posDoc = pDocTemplate->GetFirstDocPosition();
				if (posDoc)
				{
					do
					{
						CDocument* newDoc = pDocTemplate->GetNextDoc(posDoc);
						if (newDoc)
						{
							SVIPDoc* pTmpDoc = dynamic_cast <SVIPDoc*> (newDoc);
							if (pTmpDoc)
							{
								if (pTmpDoc->IsRegressionTestRunning())
								{
									POSITION TmpPos = pTmpDoc->GetFirstViewPosition();
									CView* pIPView = pTmpDoc->GetNextView(TmpPos);
									CMDIChildWnd* pFrame = (CMDIChildWnd*)pIPView->GetParentFrame();
									pFrame->MDIActivate();
									pTmpDoc->RegressionTestModeChanged();
									bDone = true;
								}
							}
						}
					} while (posDoc && !bDone);
				}
			}
		} while (pos && !bDone);
	}

	StopSvo();
}

bool SVObserverApp::AddSecurityNode(HMODULE hMessageDll, long lId, LPCTSTR NTGroup, bool bForcePrompt)
{
	LPTSTR pTmp;

	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS |
		FORMAT_MESSAGE_FROM_HMODULE,
		(LPCVOID)hMessageDll, lId,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&pTmp, 0, nullptr))
	{

		std::string Tmp(pTmp);
		Tmp = SvUl::Left(Tmp, Tmp.find('\n') - 1);
		TheSecurityManager().SVAdd(lId, Tmp.c_str(), NTGroup, bForcePrompt);
		LocalFree(pTmp);
		return true;
	}
	return false;
}

bool SVObserverApp::AddSecurityNode(HMODULE hMessageDll, long lId)
{
	bool l_bRet = true;
	LPTSTR pszTmp;

	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS |
		FORMAT_MESSAGE_FROM_HMODULE,
		(LPCVOID)hMessageDll, lId,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&pszTmp, 0, nullptr))
	{
		CString strTmp = pszTmp;
		int l_pos = strTmp.Find('\n', 0);
		if (l_pos > 0)
		{
			TheSecurityManager().SVAdd(lId, strTmp.Left(l_pos - 1));
		}
		else
		{
			l_bRet = false;
		}
		LocalFree(pszTmp);
	}
	return l_bRet;
}


/////////////////////////////////////////////////////////////////////////////
// Command message processor for range: ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16
//
BOOL SVObserverApp::OnOpenRecentFile(UINT nID)
{
	if (S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_FILE_MENU_RECENT_CONFIGURATIONS))
	{
		ValidateMRUList();

		if (!SVSVIMStateClass::CheckState(SV_STATE_UNAVAILABLE))
		{
			bool bRunning = SVSVIMStateClass::CheckState(SV_STATE_RUNNING);

			if (!bRunning)
			{
				bool l_bOk = OpenConfigFileFromMostRecentList(nID);
				if (l_bOk && !TheSecurityManager().SVIsSecured(SECURITY_POINT_MODE_MENU_EDIT_TOOLSET))
				{
					SetModeEdit(true); // Set Edit mode
				}
			}
		}
	}

	// @HACK - This method is always returning TRUE.
	return true;
}


void SVObserverApp::OnUpdateRecentFileMenu(CCmdUI* PCmdUI)
{
	CWinApp::OnUpdateRecentFileMenu(PCmdUI);

	bool bEnable = (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_REGRESSION | SV_STATE_TEST)
		&& TheSecurityManager().SVIsDisplayable(SECURITY_POINT_FILE_MENU_RECENT_CONFIGURATIONS) && (SVOLicenseManager::Instance().HasMatroxLicense()));

	if (false == bEnable)
	{
		for (long l = ID_FILE_MRU_FILE1; l < (long)(PCmdUI->m_nID); l++)
			PCmdUI->m_pMenu->EnableMenuItem(l, MF_BYCOMMAND | MF_GRAYED);
	}
}

#pragma endregion Private Methods
