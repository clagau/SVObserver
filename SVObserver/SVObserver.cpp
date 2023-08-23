//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved, Harrisburg
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
#include "ConfigurationOuttakes.h"
#include "GuiHelpers.h"
#include "ExtrasEngine.h"
#include "RootObject.h"
#include "SVGigeCameraManagerDlg.h"
#include "SVIODoc.h"
#include "SVIPDoc.h"
#include "SVInspectionProcess.h"
#include "SVMainFrm.h"
#include "SVObserverOuttakes.h"
#include "SVObserver.h"
#include "SVSecurity/SVSecurityManager.h"
#include "SVObserver_i.h"
#include "SVPPQObject.h"
#include "SVRCCommand.h"
#include "SVRCWebsocketServer.h"
#include "SVToolSet.h"
#include "SVUtilities.h"
#include "SVVisionProcessorHelper.h"
#include "SVXFunctions.h"
#include "SoftwareTriggerDlg.h"
#include "TextDefinesSvO.h"
#include "CameraLibrary/SVDeviceParam.h"
#include "CameraLibrary/SVDeviceParams.h"
#include "CameraLibrary/SVLongValueDeviceParam.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/SVUserMessage.h"
#include "Definitions/StringTypeDef.h"
#include "Definitions/TextDefinesSvDef.h"
#include "InspectionEngine/SVDigitizerProcessingClass.h"
#include "FilesystemUtilities/FilepathUtilities.h"
#include "FilesystemUtilities/FileHelperManager.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVLogLibrary/Logging.h"
#include "SVMFCControls/DisplayMessageBox.h"
#include "SVMatroxLibrary/SVMatroxApplicationInterface.h"
#include "SVMatroxLibrary/SVMatroxErrorEnum.h"
#include "SVMatroxLibrary/SVMatroxSystemInterface.h"
#include "SVMatroxLibrary/SVOLicenseManager.h"
#include "SVMessage/SVMessage.h"
#include "SVOGui/SVDirectX.h"
#include "SVOGui/SVDirectX.h"
#include "SVOGuiUtility/SVGlobal.h"
#include "SVOGui/SVMessageWindow.h"
#include "SVOGui/SVStartWindow.h"
#include "SVHttpLibrary/WebSocketSettingsLoader.h"
#include "SVHttpLibrary/WebsocketSettings.h"
#include "SVOLibrary/SVHardwareManifest.h"
#include "SVOLibrary/SVMemoryManager.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVSharedMemoryLibrary/ShareEvents.h"
#include "SVSharedMemoryLibrary/SharedMemWriter.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVSharedMemoryLibrary/EditLock.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/SvimState.h"
#include "SVSystemLibrary/SVThread.h"
#include "SVSystemLibrary/SVVersionInfo.h"
#include "SVUtilityLibrary/Heapwalk.h"
#include "SVUtilityLibrary/LoadDll.h"
#include "SVUtilityLibrary/SVClock.h"
#include "SVUtilityLibrary/ZipHelper.h"
#include "Triggering/SVTriggerObject.h"
#include "Triggering/SVTriggerProcessingClass.h"
#pragma endregion Includes

#pragma region Declarations

constexpr double	cNormalNonPageMemoryUsage = 80.0;		 //Value as a percentage of the total Non-Page size

#pragma endregion Declarations

void CreateImageStores(SVConfigurationObject* pConfig, PPQMonitorList& rPpqMonitorList, HRESULT Result);

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

IMPLEMENT_SERIAL(SVObserverApp, CWinApp, 0);

BEGIN_MESSAGE_MAP(SVObserverApp, CWinApp)
	//{{AFX_MSG_MAP(SVObserverApp)
	//Moved this back here because OnOpenRecentFile was not called after the first time when this was in SVMainframe.h 
	ON_COMMAND_EX_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, OnOpenRecentFile)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, OnUpdateRecentFileMenu)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#pragma region Constructor
SVObserverApp::SVObserverApp() :
	m_DataValidDelay(0),
	m_rInitialInfoSvo(getIniInfoHandler().GetInitialInfo())
{
	free((void*)m_pszHelpFilePath);

	std::string helpPath {SvStl::GlobalPath::Inst().GetBinPath(_T("SVObserver.chm"))};
	TCHAR* pPath = new TCHAR[helpPath.size() + 1];
	memcpy(pPath, helpPath.c_str(), helpPath.size());
	pPath[helpPath.size()] = '\0';
	m_pszHelpFilePath = pPath;

	EnableHtmlHelp();

#if defined (TRACE_THEM_ALL) || defined (TRACE_SVO)
	::OutputDebugString(_T("Executing => SVObserverApp::SVObserverApp()\n"));
#endif

	getIniInfoHandler().ResetInformation();

	// The Standard Configuration Execution Directory
	m_ConfigExePNVariableName = _T("ConfigurationExecutionPathName");

	m_ConfigExePNVariableValue = _T("");

	// The Standard Last Valid Configuration Directory
	m_LastValidConfigPNVariableName = _T("LastValidConfigurationPathName");

	m_LastValidConfigPNVariableValue = _T("");

	m_AutoRunDelayTime = 1000;

	m_CurrentVersion = SvSyl::getSvoVersionAsUnsignedLong();
	m_LoadingVersion = 0L;

	m_pCurrentDocument = nullptr;	// Set by current Document!!!
	m_pMessageWindow = nullptr;
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

	// Registry Key...
	SetRegistryKey(_T("SVR Gesellschaft für Bildverarbeitung mbH"));

	HINSTANCE ResourceInstance(nullptr);
	//Load resource dll explicitly
	HRESULT retValue = SvUl::LoadDll::Instance().getDll(SvUl::SVOResourceDll, ResourceInstance);
	if (S_OK != retValue || nullptr == ResourceInstance)
	{
		//Because our exception handler (message box) needs the resources, we have to use here the standard message box.
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_SVO_53_RESOURCE_DLL_LOADING_FAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
		MessageBox(nullptr, SvO::LoadingResourceDllFailed, nullptr, MB_OK | MB_ICONSTOP);
		exit(SVMSG_SVO_53_RESOURCE_DLL_LOADING_FAILED);
	}

#ifdef LOG_HEAP_INFO
	SvUl::startHeaplogs();
#endif 
	SvStl::MessageManager::setShowDisplayFunction(SvMc::DisplayMessageBox::showDialog);

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
		Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_OleInitFailed, SvStl::SourceFileParams(StdMessageParams));
		return false;
	}

	if (!InitATL())
	{
		return false;
	}

	// The start window	
	SVStartWindow sWin;
#ifndef _DEBUG
	if (sWin.Create(IDD_START))
	{
		sWin.ShowWindow(SW_SHOW);
	}
	else
	{
		return FALSE;
	}
#endif //_DEBUG

	//Initializing  must be before first use of  MessageNotification::FireNotify which is i.e called from checkDrive 
	startInstances();
	SVDirectX::Instance().Initialize();

	// Check for proper setup of V: for SVRemoteControl
	if (S_OK != SvFs::checkDrive(SvStl::GlobalPath::Inst().GetPathOnRamDrive().c_str()))
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

	AfxEnableControlContainer();

	LoadStdProfileSettings(7);  // Standard-INI-Dateioptionen einlesen (einschließlich MRU)

	SvUl::LoadDll::Instance().setDefaultPath(SvStl::GlobalPath::Inst().GetBinPath(_T("\\")));

	ValidateMRUList();

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

	if (S_OK != getIniInfoHandler().GetInitializationStatusFlags())
	{
		SvDef::StringVector msgList;
		msgList.push_back(m_rInitialInfoSvo.ModelNumberString());
		msgList.push_back(m_rInitialInfoSvo.InitializationFailureDescription(getIniInfoHandler().GetInitializationStatusFlags()));

#ifndef _DEBUG                    // 23 Mar 1999 - frb.
		sWin.ShowWindow(SW_HIDE);
#endif //_DEBUG                  // 23 Mar 1999 - frb.

		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SVObserver_InitializationFailure, msgList, SvStl::SourceFileParams(StdMessageParams));
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

	SVMemoryManager::Instance().InitializeMemoryManager(getIniInfoHandler().GetInitialInfo().m_archiveToolBufferSize);

// Close Start Window
	sWin.DestroyWindow();


	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pMainFrame->UpdateWindow();

	//m_pMainWnd has already been checked for null due to pMainFrame check
	SVVisionProcessorHelper::Instance().setMainhWnd(m_pMainWnd->GetSafeHwnd());

	if (!IsWindow(SVVisionProcessorHelper::Instance().getMainhWnd()))
	{
		Log_Error(SvStl::Tid_InvalidMainwindow);
	}
	//
	// Init user list...

	Logout();

	SvimState::AddState(SV_STATE_AVAILABLE);

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
	msgList.push_back(SvSyl::getSvoVersionAsString());
	SvStl::MessageManager Info(SvStl::MsgType::Log);
	Info.setMessage(SVMSG_SVO_25_SVOBSERVER_STARTED, SvStl::Tid_Version, msgList, SvStl::SourceFileParams(StdMessageParams));

	ExtrasEngine::Instance().SetAutoSaveEnabled(getIniInfoHandler().GetInitialInfo().m_enableAutosave);

	m_DataValidDelay = getIniInfoHandler().GetInitialInfo().m_dataValidDelay;

	///This sets the reserved text size for text value objects which can change in size
	int maxTextSize = (0 == getIniInfoHandler().GetInitialInfo().m_maxTextSize) ? SvDef::cMaxStringByteSize : getIniInfoHandler().GetInitialInfo().m_maxTextSize;
	SvVol::SVStringValueObjectClass::setMaxTextSize(maxTextSize);


	if (!SVOLicenseManager::Instance().HasMatroxLicense())
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_52_NOMATROX_DONGLE, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
	}

	if (SVOLicenseManager::Instance().HasMatroxLicense() && !SVOLicenseManager::Instance().HasMatroxGigELicense() && IsMatroxGige())
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_MatroxGigELicenseNotFound, SvStl::SourceFileParams(StdMessageParams));
	}

	return true;
}

void SVObserverApp::UpdateAllIOViews()
{
	if (GetTheIODoc() &&
		SvimState::CheckState(SV_STATE_READY | SV_STATE_RUNNING) &&
		!SvimState::CheckState(SV_STATE_CANCELING))
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

#ifdef LOG_HEAP_INFO
	SvUl::endHeaplogs();
#endif 

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
		std::string OrgMessageCode = std::format(_T("{:#08x}"), Msg.m_MessageCode);
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
	SvSml::TemporaryState_Editing tse;
	if (false == tse.stateWasEntered())
	{
		return E_FAIL;
	}

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
		Log_Error(SvStl::Tid_DestroyConfigFailed);
		return result;	//keep the cancel state so it does not remove from MRU
	}

	// If not loading from the run path then delete all files in that path
	if (0 != SvUl::CompareNoCase(loadPath, std::string(SvFs::FileHelperManager::Instance().GetRunPathName() + "\\")))
	{
		// Clean up Execution Directory...
		// Check path, create if necessary and delete contents...
		InitPath(std::string(SvFs::FileHelperManager::Instance().GetRunPathName() + "\\").c_str(), true, true);
	}

	//Is the file of type packed config then we need to first unzip the file into the run folder
	if (0 == SvUl::CompareNoCase(Extension, std::string(SvDef::cPackedConfigExtension)))
	{
		SvDef::StringVector RunFiles;
		if (SvUl::unzipAll(FileName, SvFs::FileHelperManager::Instance().GetRunPathName(), RunFiles))
		{
			//Find the svx file
			for (const auto& rFile : RunFiles)
			{
				_tsplitpath(rFile.c_str(), szDrive, szDir, szFile, szExt);
				Extension = szExt;
				if (0 == SvUl::CompareNoCase(Extension, std::string(SvDef::cConfigExtension)))
				{
					SetFullSvxFileName(rFile);
					//When this is set then we need to rename the file to the name of the svx file
					if (fileType == ConfigFileType::SvzFormatDefaultName)
					{
						std::string oldFileName{ FileName };
						SvUl::searchAndReplace(FileName, OriginalFile.c_str(), szFile);
						::rename(oldFileName.c_str(), FileName.c_str());
						OriginalFile = GetSvxFileNameOnly();
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
		SetFullSvxFileName(FileName);
		LoadSvxFilenameFromManager();
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
			SetConfigFileNameOnly(OriginalFile);
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
		Log_Error(SvStl::Tid_WrongExtensionInOpenFile);
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

		SvimState::changeState(SV_STATE_UNAVAILABLE | SV_STATE_LOADING, SV_STATE_AVAILABLE);

		try
		{
			SvOi::TRC_RAIIPtr globalInitPtr;

			try
			{
				//avoid that TRC-memory will be recreated for every loading step, but do it once at the end.
				globalInitPtr = SvOi::getTriggerRecordControllerRWInstanceThrow().setGlobalInit();
			}
			catch (const SvStl::MessageContainer& rExp)
			{
				SvStl::MessageManager Exception(SvStl::MsgType::Log);
				Exception.setMessage(rExp.getMessage());
				Log_Error(SvStl::Tid_OpenSvxFileError);
			}

			hr = LoadSvxFile(m_CurrentVersion, GetFullSvxFileName(), getConfigFullFileName(), GetSVIMType(), dynamic_cast<SVMainFrame*>(m_pMainWnd), std::move(globalInitPtr));

			if (hr & SvDef::svErrorCondition)

			{
				Log_Error(SvStl::Tid_OpenSvxFileError);
				
				// If there was an error during configuration loading...
				SvimState::changeState(SV_STATE_AVAILABLE, SV_STATE_UNAVAILABLE | SV_STATE_LOADING);

				//Use E_FAIL to stop the loading but do not show any error messages
				if (E_FAIL != hr)
				{
					SvDef::StringVector msgList;
					msgList.push_back(std::format(_T("{:d}"), hr));
					SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
					Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_ConfigurationLoadFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
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
			
			Log_Error(SvStl::Tid_OpenSvxFileError);
			delete pUE;

			bOk = false;

			SvimState::RemoveState(SV_STATE_LOADING);

			DestroyConfig(false, true);

			setConfigFullFileName(nullptr, true);

			SvimState::changeState(SV_STATE_AVAILABLE, SV_STATE_UNAVAILABLE);
		}  // catch

		break;
	} // while (1)

	if (!bOk)
	{
		setConfigFullFileName(nullptr, true);
		SvimState::changeState(SV_STATE_AVAILABLE, SV_STATE_UNAVAILABLE | SV_STATE_LOADING);
		Log_Error(SvStl::Tid_OpenSvxFileError);
	}

	if (S_OK == hr)
	{
		hr = bOk ? S_OK : S_FALSE;
	}

	HideTabsAfterOpenSvxFile(GetTheIODoc());

	return hr;
}


#pragma endregion virtual

void SVObserverApp::RemoveUnusedFiles()
{
	SvFs::FileHelperManager::Instance().RemoveUnusedFiles();
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

bool SVObserverApp::InitPath(LPCTSTR PathName, bool createIfMissing /*= true*/, bool DeleteContents /*= true*/)
{
	if (nullptr != PathName)
	{
		if (SvFs::ensureDirectoryExists(PathName, createIfMissing))
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
							return SvFs::deleteFiles(DeleteName.c_str(), true);
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
	bool result = (0 == SvUl::CompareNoCase(m_rInitialInfoSvo.m_ProductName, SvDef::SVO_PRODUCT_SVIM_X2_GD2A)
		|| 0 == SvUl::CompareNoCase(m_rInitialInfoSvo.m_ProductName, SvDef::SVO_PRODUCT_SVIM_X2_GD4A)
		|| 0 == SvUl::CompareNoCase(m_rInitialInfoSvo.m_ProductName, SvDef::SVO_PRODUCT_SVIM_X2_GD8A)
		|| 0 == SvUl::CompareNoCase(m_rInitialInfoSvo.m_ProductName, SvDef::SVO_PRODUCT_SVIM_NEO));
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

	if (0 == SvUl::CompareNoCase(m_rInitialInfoSvo.m_ProductName, SvDef::SVO_PRODUCT_SVIM_X2_GD1A))
	{
		eType = SVIM_PRODUCT_X2_GD1A;
	}
	else if (0 == SvUl::CompareNoCase(m_rInitialInfoSvo.m_ProductName, SvDef::SVO_PRODUCT_SVIM_X2_GD2A))
	{
		eType = SVIM_PRODUCT_X2_GD2A;
	}
	else if (0 == SvUl::CompareNoCase(m_rInitialInfoSvo.m_ProductName, SvDef::SVO_PRODUCT_SVIM_X2_GD4A))
	{
		eType = SVIM_PRODUCT_X2_GD4A;
	}
	else if (0 == SvUl::CompareNoCase(m_rInitialInfoSvo.m_ProductName, SvDef::SVO_PRODUCT_SVIM_X2_GD8A))
	{
		eType = SVIM_PRODUCT_X2_GD8A;
	}
	else if (0 == SvUl::CompareNoCase(m_rInitialInfoSvo.m_ProductName, SvDef::SVO_PRODUCT_SVIM_NEO))
	{
		eType = SVIM_PRODUCT_NEO;
	}
	else if (0 == SvUl::CompareNoCase(m_rInitialInfoSvo.m_ProductName, SvDef::SVO_PRODUCT_SVIM_ETHERCAT))
	{
		eType = SVIM_PRODUCT_ETHERCATIO;
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
	SvimState::SVRCBlocker block;
	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	while (nullptr != pos)
	{
		CMultiDocTemplate* pDocTemplate = dynamic_cast<CMultiDocTemplate*>(AfxGetApp()->GetNextDocTemplate(pos));
		if (pDocTemplate)
		{
			POSITION posDoc = pDocTemplate->GetFirstDocPosition();
			while (nullptr != posDoc)
			{
				CDocument* pDoc = pDocTemplate->GetNextDoc(posDoc);
				SVIPDoc* pIpDoc = dynamic_cast<SVIPDoc*> (pDoc);
				if (nullptr != pIpDoc)
				{
					pIpDoc->OnViewResetCountsCurrentIP();
				}
			}
		}
	}
}

HRESULT SVObserverApp::SetMode(unsigned long lNewMode) //@TODO [Arvid ][10.20][18.10.2021]: this function is too long
{
	HRESULT result {S_OK};

	if (SvPb::DeviceModeType::editModuleMode == SvimState::GetMode())
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ActionNotPossibleInModuleEditingMode, {getCompleteObjectNameForId(SvimState::getModuleEditingId())}, SvStl::SourceFileParams(StdMessageParams));
		return SVMSG_SVIMCMD_REQUEST_REJECTED;
	}

	long originalState = SvimState::GetState();
	SvPb::DeviceModeType Mode = SvPb::DeviceModeType_IsValid(lNewMode) ? static_cast<SvPb::DeviceModeType> (lNewMode) : SvPb::DeviceModeType::unknownMode;

	if (SvimState::CheckState(SV_STATE_START_PENDING | SV_STATE_STARTING | SV_STATE_STOP_PENDING | SV_STATE_STOPING))
	{
		result = SVMSG_50_MODE_CHANGING_ERROR;
	}
	else if (SvimState::CheckState(SV_STATE_LOADING))
	{
		result = SVMSG_51_MODE_CONFIGURATION_LOADING_ERROR;
	}
	else if (SvimState::CheckState(SV_STATE_EDITING))
	{
		result = SVMSG_52_MODE_GUI_IN_USE_ERROR;
	}
	else if (SvPb::DeviceModeType::runMode == Mode)
	{
		if (SvimState::CheckState(SV_STATE_TEST | SV_STATE_RUNNING))
		{
			StopSvo();
		}
		// Try to go online...
		if (false == SvimState::CheckState(SV_STATE_REGRESSION) && SvimState::CheckState(SV_STATE_READY))
		{
			try
			{
				Start(SV_STATE_RUNNING);
			}
			catch (const SvStl::MessageContainer& rExp)
			{
				SvStl::MessageManager  Exception(SvStl::MsgType::Log | SvStl::MsgType::Notify);
				Exception.setMessage(rExp.getMessage());
				result = SVMSG_SVIMCMD_GO_ONLINE_FAILED;
			}
		}
		else
		{
			result = SVMSG_SVIMCMD_GO_ONLINE_FAILED;
		}
	}
	else if (SvPb::DeviceModeType::stopMode == Mode)
	{
		// Go offline
		if (SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST))
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
		if (SvimState::CheckState(SV_STATE_RUNNING))
		{
			result = SVMSG_SVIMCMD_REQUEST_REJECTED;
		}
		else if (SvimState::CheckState(SV_STATE_REGRESSION))
		{
			StopSvo();
		}

		if (false == SvimState::CheckState(SV_STATE_TEST) && SvimState::CheckState(SV_STATE_READY))
		{
			try
			{
				Start(SV_STATE_TEST);
			}
			catch (const SvStl::MessageContainer& rExp)
			{
				SvStl::MessageManager  Exception(SvStl::MsgType::Log | SvStl::MsgType::Notify);
				Exception.setMessage(rExp.getMessage());
				result = E_FAIL;
			}
		}
		else
		{
			result = SVMSG_SVIMCMD_REQUEST_REJECTED;
		}
	}
	else if (SvPb::DeviceModeType::regressionMode == Mode)
	{
		// Later Currently not supported through the Control
		result = SVMSG_SVIMCMD_REQUEST_REJECTED;
	}
	else if (SvPb::DeviceModeType::editMode == Mode)
	{
		if (SvimState::CheckState(SV_STATE_TEST | SV_STATE_RUNNING))
		{
			StopSvo();
		}

		if (false == SvimState::CheckState(SV_STATE_REGRESSION) && SvimState::CheckState(SV_STATE_READY))
		{
			SetModeEdit(true);
		}
		else
		{
			result = SVMSG_SVIMCMD_REQUEST_REJECTED;
		}
	}
	else
	{
		result = SVMSG_SVIMCMD_REQUEST_REJECTED;
	}
	if (S_OK != result)
	{
		SvimState::changeState(static_cast<DWORD> (originalState), 0);
	}

	PostMessage(m_pMainWnd->m_hWnd, SV_REFRESH_STATUS_BAR, 0, 0);
	return result;
}

HRESULT SVObserverApp::OnObjectRenamed(const std::string& p_rOldName, uint32_t objectId)
{
	HRESULT l_Status = S_OK;

	SvOl::SVObjectAppClass* l_pObject = dynamic_cast<SvOl::SVObjectAppClass*>(SVObjectManagerClass::Instance().GetObject(objectId));

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

bool SVObserverApp::AlreadyExistsIPDocTitle(LPCTSTR)
{
	return false;
}

bool SVObserverApp::UpdateConfiguration(bool newConfiguration /*= false*/)
{
	if (SvPb::DeviceModeType::editModuleMode == SvimState::GetMode())
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ActionNotPossibleInModuleEditingMode, {getCompleteObjectNameForId(SvimState::getModuleEditingId())}, SvStl::SourceFileParams(StdMessageParams));
		return false;
	}

	// Access denied, if... // Check Edit Mode
	if (SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST))
		return false;

	if (newConfiguration)
	{
		if (false == PrepareForNewConfiguration())
		{
			return false;
		}
	}
	else
	{
		CWaitCursor wait;

		SvimState::CheckState(SV_STATE_READY);
		ResetAllIPDocModifyFlag(false);
	}


	bool ok = CreateAndUseConfigAssistant(newConfiguration);

	if (!ok && newConfiguration)
	{
		SvimState::changeState(SV_STATE_AVAILABLE, SV_STATE_READY | SV_STATE_MODIFIED);
	}

	return ok;
}


bool SVObserverApp::OkToEdit()
{
	bool Result{ false };
	if (SvimState::CheckState(SV_STATE_EDIT) && SvimState::CheckState(SV_STATE_READY))
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
		Log_Assert(FALSE);
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
		Log_Assert(FALSE);
	}

	hCursor = ::LoadCursor(nullptr, IDC_WAIT);
	hCursor = ::SetCursor(hCursor);

	ConnectCameras();

	::SetCursor(hCursor);

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

		SvFs::FileNameContainer* pDummyFiles = nullptr;
		SVLightReference* pDummyLight = nullptr;
		SVLut* pDummyLut = nullptr;

		SvIe::SVAcquisitionClassPtr pDevice(SvIe::SVDigitizerProcessingClass::Instance().GetAcquisitionDevice(Iter->c_str()));
		if (nullptr != pDevice && nullptr != pConfig &&
			pConfig->GetAcquisitionDevice(Iter->c_str(), pDummyFiles, pDummyLight, pDummyLut, pDeviceParams))
		{
			// Check for the camera file and camera to match.
			if (S_OK == hr)
			{
				HRESULT hrTmp = pDevice->IsValidCameraFileParameters(*pDeviceParams);
				if (E_NOTIMPL != hrTmp && S_OK != hrTmp)
				{
					hr = SV_CAN_GO_ONLINE_FAILURE_ACQUISITION;
				}
			}

			// Send GIGE packet size if was set from hardware.ini
			if (IsMatroxGige() && m_rInitialInfoSvo.m_gigePacketSize != 0)
			{
				pDeviceParams->SetParameter(DeviceParamGigePacketSize, SVLongValueDeviceParam{ m_rInitialInfoSvo.m_gigePacketSize });
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
	HRESULT hr = S_OK;
	if (p_bState)
	{
		SvimState::changeState(SV_STATE_EDIT, SV_STATE_STOP);
	}
	else
	{
		SvimState::changeState(SV_STATE_STOP, SV_STATE_EDIT);

		//
		// We need to deselect any tool that might be set for operator move.
		//
		DeselectTool();
	}
	return hr;
}

void SVObserverApp::Start(DWORD desiredState)
{
	SvimState::SVRCBlocker block;

	if (false == InitialChecks(desiredState))
	{
		return;
	}
	if (SV_STATE_TEST == desiredState)
	{
		bool softwareTrigger {true};
		RootObject::setRootChildValue(SvDef::FqnEnvironmentSoftwareTrigger, softwareTrigger);
	}

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (pConfig != nullptr  && SV_STATE_RUNNING == desiredState)
	{
		executePreOrPostExecutionFile(pConfig->getPreRunExecutionFilePath());
	}

	SvimState::RemoveState(SV_STATE_EDIT | SV_STATE_STOP);

	UpdateAndGetLogDataManager();

	double startLoading = SvUl::GetTimeStamp();

	SVObjectManagerClass::Instance().ClearAllIndicator();

	Log_Assert(nullptr != pConfig);

	if (nullptr == m_pMainWnd || nullptr == pConfig)
	{
		RunAllIPDocuments();

		SvStl::MessageContainer Exception(SVMSG_SVO_54_EMPTY, SvStl::Tid_GoOnlineFailure_InvalidPointerConfig, SvStl::SourceFileParams(StdMessageParams));
		throw Exception;
	}

	PrepareForStart(pConfig);

	//SvSml::SharedMemWriter::Instance().RebuildMonitorEntryMap();

	if (SvimState::CheckState(SV_STATE_READY))
	{
		RunInspections(pConfig, desiredState);

		std::string TriggerCounts;
		GetTriggersAndCounts(TriggerCounts);

		long loadTime = static_cast<long> (SvUl::GetTimeStamp() - startLoading);
		SvDef::StringVector msgList;
		msgList.push_back(TriggerCounts);
		msgList.push_back(std::format(_T("{:d}"), loadTime));
		//add go-online message to the event viewer.
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_SVO_27_SVOBSERVER_GO_ONLINE, SvStl::Tid_GoOnlineTime, msgList, SvStl::SourceFileParams(StdMessageParams));

		SVObjectManagerClass::Instance().SetState(SVObjectManagerClass::ReadOnly);

		SetThreadPriority(THREAD_PRIORITY_BELOW_NORMAL);
	}// end if

	if (SvimState::CheckState(SV_STATE_STOP_PENDING))
	{
		PostMessage(m_pMainWnd->m_hWnd, WM_COMMAND, MAKEWPARAM(ID_STOP, 0), 0);
	}
}


bool SVObserverApp::InitialChecks(DWORD desiredState)
{
	if (SvimState::CheckState(desiredState | SV_STATE_STARTING))
	{
		if (SvimState::CheckState(SV_STATE_START_PENDING))
		{
			SvimState::RemoveState(SV_STATE_START_PENDING);
		}

		return false;
	}

	if (SvimState::CheckState(SV_STATE_LOADING | SV_STATE_STOPING))
	{
		SvimState::AddState(SV_STATE_START_PENDING);
		return false;
	}

	return true;
}


void SVObserverApp::PrepareForStart(SVConfigurationObject* pConfig)
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

		CreateImageStores(pConfig, ppqMonitorList, Result);

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
		SvimState::RemoveState(SV_STATE_START_PENDING);
		RunAllIPDocuments();
		throw;
	}
	catch (std::exception& ex)
	{
		//cleanup goOnline, after fail, before exception leave this method
		SvimState::RemoveState(SV_STATE_START_PENDING);
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
			SvStl::MessageContainer Exception(SVMSG_SVO_NON_PAGED_MEMORY_FULL, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
			throw Exception;
		}
		Result = SendCameraParameters();
		if (Result != S_OK)
		{
			SvStl::MessageContainer Exception(SVMSG_SVO_54_EMPTY, SvStl::Tid_GoOnlineFailure_SendCameraParam, SvStl::SourceFileParams(StdMessageParams));
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
				msgList.push_back(std::format(_T("{:f}"), cNormalNonPageMemoryUsage));
				msgList.push_back(std::format(_T("{:f}"), NonPagedMemUsage));
				SvStl::MessageManager Exception(SvStl::MsgType::Log);
				Exception.setMessage(SVMSG_SVO_NON_PAGED_MEMORY_LOW, SvStl::Tid_MoreThanPercentUsed, msgList, SvStl::SourceFileParams(StdMessageParams));
			}
		}

		return Result;
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

		SvimState::RemoveState(SV_STATE_START_PENDING);

		RunAllIPDocuments();
		SetAllIPDocumentsOffline();

		throw;
	}// end if

	SetAllIPDocumentsOnline();
	DWORD removeState = (desiredState == SV_STATE_RUNNING) ? SV_STATE_READY | SV_STATE_START_PENDING : SV_STATE_START_PENDING;
	SvimState::changeState(SV_STATE_UNAVAILABLE | SV_STATE_STARTING, removeState);

	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	auto* pTrcRW = SvOi::getTriggerRecordControllerRWInstance();
	if (nullptr != pTrcRW)
	{
		pTrcRW->lockReset();
	}

	SvimState::changeState(desiredState, SV_STATE_UNAVAILABLE | SV_STATE_STARTING);

	//Now that we are in the running state we allow trigger processing!
	StartTrigger(pConfig);

	//Module ready should be the last set!
	if (pConfig->SetModuleReady(true) != S_OK)
	{
		RunAllIPDocuments();

		SvStl::MessageContainer Exception(SVMSG_SVO_54_EMPTY, SvStl::Tid_GoOnlineFailure_ModuleReadyOutput, SvStl::SourceFileParams(StdMessageParams));
		throw Exception;
	}// end if

}

#pragma endregion Public Methods

#pragma region Protected Methods
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
		TheSecurityManager().SVProtectData(SECURITY_POINT_EXTRAS_MENU_SECURITY_MANAGER); // Sets Flag that will prevent data from being changed.
	}

	TCHAR szGetBuf[256];

	GetPrivateProfileString(_T("Security"), _T("Security File Path"), SvStl::GlobalPath::Inst().GetBinPath(_T("\\Gatekpr.xml")).c_str(), szGetBuf, 256, SvStl::GlobalPath::Inst().GetSVIMIniPath());

	if (TheSecurityManager().SVLoad(szGetBuf) != S_OK)
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_SecurityFileLoadFailed, SvStl::SourceFileParams(StdMessageParams));
	}

	FreeLibrary(hMessageDll);
	// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
	Sleep(0);

	return S_OK;
}


#pragma endregion Protected Methods

#pragma region Private Methods
void SVObserverApp::startInstances()
{
	AddSvxFilenameToManager();

	SvStl::MessageManager::setShowDisplayFunction(SvMc::DisplayMessageBox::showDialog);
	SVVisionProcessorHelper::Instance().Startup();
	SVMatroxApplicationInterface::Startup();
	//must be called before SVDigitizerProcessingClass-Startup
	SvOi::createTriggerRecordControllerInstance(SvOi::TRC_DataType::Writer);
	SVHardwareManifest::Instance().Startup();
	SvTrig::SVTriggerProcessingClass::Instance().Startup();
	SvIe::SVDigitizerProcessingClass::Instance().Startup();

	SVObjectManagerClass::Instance().ConstructRootObject(SvPb::RootClassId);
	getIniInfoHandler().LoadIniFilesAndDlls();

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

	HINSTANCE hZipInstance {nullptr};
	SvUl::LoadDll::Instance().getDll(SvUl::ZipDll, hZipInstance);


	fillExchangeObjectId();
	fillExchangeEmbedded();
	fillEmbeddedIdFromIndirectLinked();


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

	getIniInfoHandler().INIClose();
	SvOi::destroyTriggerRecordController();
	// Shutdown MIL
	SVMatroxApplicationInterface::Shutdown();
	SvStl::MessageManager::setShowDisplayFunction(nullptr);

	// File management for config file.
	RemoveSvxFilenameFromManager();
	SoftwareTriggerDlg::Instance().Close();
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
	if (SvimState::CheckState(SV_STATE_RUNNING))
	{
		StopSvo();

		Sleep(1000);  // wait for running threads to quiesce
	}
}

bool SVObserverApp::InitATL(){
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
		msgList.push_back(std::format(_T("{:08x}"), l_Status));
#ifdef _DEBUG
		if (!l_AppRegister && !l_AppUnregister)
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			INT_PTR result = Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_RegisterClassObjectsFailed_Question, msgList, SvStl::SourceFileParams(StdMessageParams), SvDef::InvalidObjectId, MB_YESNO);
			if (IDYES == result)
			{
				return false;
			}
		}
		else

#endif
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_RegisterClassObjectsFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
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

/////////////////////////////////////////////////////////////////////////////
// Command message processor for range: ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16
//
BOOL SVObserverApp::OnOpenRecentFile(UINT nID)
{
	if (S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_FILE_MENU_RECENT_CONFIGURATIONS))
	{
		ValidateMRUList();

		if (!SvimState::CheckState(SV_STATE_UNAVAILABLE))
		{
			bool bRunning = SvimState::CheckState(SV_STATE_RUNNING);

			if (!bRunning)
			{
				bool l_bOk = OpenConfigFileFromMostRecentList(m_pRecentFileList, nID);
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

	bool bEnable = (!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_REGRESSION | SV_STATE_TEST)
		&& TheSecurityManager().SVIsDisplayable(SECURITY_POINT_FILE_MENU_RECENT_CONFIGURATIONS) && (SVOLicenseManager::Instance().HasMatroxLicense()));

	if (false == bEnable)
	{
		for (long l = ID_FILE_MRU_FILE1; l < (long)(PCmdUI->m_nID); l++)
			PCmdUI->m_pMenu->EnableMenuItem(l, MF_BYCOMMAND | MF_GRAYED);
	}
}
#pragma endregion Private Methods

//this function is here because it requires a PPQMonitorListParameter - and spurious CheckInclude issues appear if it is in SVObserverOuttakes, fo rexample
void CreateImageStores(SVConfigurationObject* pConfig, PPQMonitorList& rPpqMonitorList, HRESULT Result)
{
	double preTriggerTimeWidow {(0.0 == TheSVObserverApp().m_rInitialInfoSvo.m_preTriggerTimeWindow) ? SvDef::cDefaultPreTriggerTimeWindow : TheSVObserverApp().m_rInitialInfoSvo.m_preTriggerTimeWindow};
	double postTriggerTimeWidow {(0.0 == TheSVObserverApp().m_rInitialInfoSvo.m_postTriggerTimeWindow) ? SvDef::cDefaultPostTriggerTimeWindow : TheSVObserverApp().m_rInitialInfoSvo.m_postTriggerTimeWindow};

	SVPPQObject::SetTimerResolution(TheSVObserverApp().m_rInitialInfoSvo.m_timerResolution);
	long lSize = pConfig->GetPPQCount();

	bool isLocalStart {false == SvimState::CheckState(SV_STATE_REMOTE_CMD)};
	///In this loop the ImageStores are created 
	for (long l = 0; S_OK == Result && l < lSize; l++)
	{
		auto* pPPQ = pConfig->GetPPQ(l);
		//Returns true when pointer valid
		if (nullptr != pPPQ)
		{
			pPPQ->setTriggerTimeWindow(preTriggerTimeWidow, postTriggerTimeWidow);
			///Set NAK Behavior
			pPPQ->SetNAKMode(TheSVObserverApp().m_rInitialInfoSvo.m_NAKMode, TheSVObserverApp().m_rInitialInfoSvo.m_NAKParameter);

			// Do this before calling CanGoOnline 
			const MonitorListAttributeStruct& rActiveList = rPpqMonitorList[pPPQ->GetName()];
			pPPQ->SetMonitorList(rActiveList);

			bool softwareTrigger {false};
			RootObject::getRootChildValue(SvDef::FqnEnvironmentSoftwareTrigger, softwareTrigger);
			pPPQ->PrepareGoOnline(softwareTrigger);

			SvTrig::SVTriggerObject* pTrigger {pPPQ->GetTrigger()};
			if (nullptr != pTrigger && SvDef::TriggerType::SoftwareTrigger == pTrigger->getType())
			{
				SoftwareTriggerDlg::Instance().AddTrigger(pTrigger, softwareTrigger && isLocalStart);
			}
		}
	}
	EnableTriggerSettings(isLocalStart && SoftwareTriggerDlg::Instance().HasTriggers());
}
