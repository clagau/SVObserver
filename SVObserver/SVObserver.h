//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObserver
//* .File Name       : $Workfile:   SVObserver.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.21  $
//* .Check In Date   : $Date:   18 Feb 2015 11:06:14  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "TriggerHandling/SVDigitizerLoadLibraryClass.h"
#include "SVIOLibrary/SVIOTriggerLoadLibraryClass.h"
#include "InitialInformationHandler.h"
#include "SVSecurity/SVSecurityManager.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVFileSystemLibrary/SVFileNameClass.h"
#include "SVRemoteFontManager.h"
#include "SVStatusLibrary/SVOIntelRAIDStatusClass.h"
#include "InspectionEngine/SVUtilityIniClass.h"
#include "SVIOTabbedView.h"
#include "SVSystemLibrary/SVObserverEnums.h"
#include "Definitions/SVPPQEnums.h"
#pragma endregion Includes

#pragma region Declarations
class SVConfigurationObject;
class SVInspectionProcess;
class SVIOController;
class SVIODoc;
class SVIPDoc;
class SVMainFrame;
class SVMessageWindowClass;
class SVUserObjectListClass;
namespace SvOi
{
	class IObjectWriter;
}

enum SVConfigFileActionsEnums
{
	LOAD_CONFIG = TRUE,
	SAVE_CONFIG = FALSE,
	RENAME = 2,
	RENAME_CONFIG = RENAME,
	NUM_CONFIG_FILE_ACTIONS
};

typedef SvXml::SVXMLMaterialsTree SVTreeType;
#pragma endregion Declarations

class SVObserverApp : public CWinApp
{
	DECLARE_SERIAL( SVObserverApp )

public:
#pragma region Constructor
	SVObserverApp();
	virtual ~SVObserverApp();
#pragma endregion Constructor

#pragma region Public Methods
#pragma region AFX_MSG Methods
	//{{AFX_MSG(SVObserverApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileNewConfig();
	afx_msg void OnFileSaveConfig();
	afx_msg void OnFileOpenSVC();
	afx_msg void OnEditEnvironment();
	afx_msg void OnTestMode();
	afx_msg void OnModeTestBtn();
	afx_msg void OnRunMode();
	afx_msg void OnStopTestMode();
	afx_msg void OnEnvironmentSettings();
	afx_msg void OnThreadAffinitySetup();
	afx_msg void OnUpdateThreadAffinitySetup(CCmdUI* PCmdUI);
	afx_msg void OnUpdateModeRun(CCmdUI* PCmdUI);
	afx_msg void OnFileCloseConfig();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSaveAll();
	afx_msg void OnEditIOList();
	afx_msg void OnUpdateEditEnvironment(CCmdUI* PCmdUI);
	afx_msg void OnUpdateEditIOList(CCmdUI* PCmdUI);
	afx_msg void OnUpdateFileClose(CCmdUI* PCmdUI);
	afx_msg void OnUpdateFileNew(CCmdUI* PCmdUI);
	afx_msg void OnUpdateFileOpen(CCmdUI* PCmdUI);
	afx_msg void OnUpdateFileOpenSVC(CCmdUI* PCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI* PCmdUI);
	afx_msg void OnUpdateFileSaveAll(CCmdUI* PCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI* PCmdUI);
	afx_msg void OnUpdateFileSaveCopyAs(CCmdUI* PCmdUI);
	afx_msg void OnUpdateFileSaveImage(CCmdUI* PCmdUI);
	afx_msg void OnUpdateFileUpdate(CCmdUI* PCmdUI);
	afx_msg void OnUpdateExtrasAdditionalEnvironmentSettings(CCmdUI* PCmdUI);
	afx_msg void OnStop();
	afx_msg void OnUpdateEditDelete(CCmdUI* PCmdUI);
	afx_msg void OnUpdateEditEditTool(CCmdUI* PCmdUI);
	afx_msg void OnUpdateEditEditToolSetCondition(CCmdUI* PCmdUI);
	afx_msg void OnUpdateEditRemoteInputs(CCmdUI* PCmdUI);
	afx_msg void OnUpdateEditAddRemoteOutputs(CCmdUI *pCmdUI);
	afx_msg void OnUpdateExtrasLightReference(CCmdUI* PCmdUI);
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* PCmdUI);
	afx_msg void OnUpdateFilePrintSetup(CCmdUI* PCmdUI);
	afx_msg void OnUpdateExtrasTestOutputs(CCmdUI* PCmdUI);
	afx_msg void OnUpdateHelp(CCmdUI* PCmdUI);
	afx_msg void OnUpdateHelpFinder(CCmdUI* PCmdUI);
	afx_msg void OnUpdateModeStopTest(CCmdUI* PCmdUI);
	afx_msg void OnUpdateModeTest(CCmdUI* PCmdUI);
	afx_msg void OnUpdateModeTestBtn(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNextPane(CCmdUI* PCmdUI);
	afx_msg void OnUpdatePrevPane(CCmdUI* PCmdUI);
	afx_msg void OnUpdateWindowArrange(CCmdUI* PCmdUI);
	afx_msg void OnUpdateWindowCascade(CCmdUI* PCmdUI);
	afx_msg void OnUpdateWindowNew(CCmdUI* PCmdUI);
	afx_msg void OnUpdateWindowTileHorz(CCmdUI* PCmdUI);
	afx_msg void OnUpdateWindowTileVert(CCmdUI* PCmdUI);
	afx_msg void OnUpdateAppExit(CCmdUI* PCmdUI);
	afx_msg void OnUpdateAppAbout(CCmdUI* PCmdUI);
	afx_msg void OnFilePrintConfig();
	afx_msg void OnSVOFilePrintSetup();
	afx_msg void OnExtrasLogin();
	afx_msg void OnUpdateExtrasLogin(CCmdUI* PCmdUI);
	afx_msg void OnGoOffline();
	afx_msg void OnUpdateGoOffline(CCmdUI* PCmdUI);
	afx_msg void OnGoOnline();
	afx_msg void OnUpdateGoOnline(CCmdUI* PCmdUI);
	afx_msg void OnUpdateFilePrintConfig(CCmdUI* PCmdUI);
	afx_msg void OnExtrasLogout();
	afx_msg void OnUpdateExtrasLogout(CCmdUI* PCmdUI);
	afx_msg BOOL OnOpenRecentFile(UINT NID);
	afx_msg void OnRunMostRecentMRU( );
	afx_msg void OnUpdateRecentFileMenu(CCmdUI* PCmdUI);
	afx_msg void OnUpdateExtraUtilities(CCmdUI* PCmdUI);
	afx_msg void OnUpdateEditPublishedResults(CCmdUI* PCmdUI);
	afx_msg void OnAppExit();
	afx_msg void OnFileSaveAsSVC();
	afx_msg void OnUpdatePublishedResultsPicker(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePublishedImagesPicker(CCmdUI* PCmdUI);
	afx_msg void OnUpdateResultsPicker(CCmdUI* pCmdUI);
	afx_msg void OnExtrasUtilitiesEdit();
	afx_msg void OnUpdateRegressionTest(CCmdUI* pCmdUI);
    afx_msg void OnEditMonitorList();
    afx_msg void OnUpdateEditAddMonitorList(CCmdUI* pCmdUT);

	// Special catch in Run Mode...
	afx_msg void OnRunUtility( UINT uiId );

	// Special for SVRCComm.DLL
	afx_msg void OnRCGoOffline();
	afx_msg void OnRCGoOnline();
	afx_msg void OnRCSaveAllAndGetConfig();
	afx_msg void OnRCCloseAndCleanUpDownloadDirectory();
	afx_msg void OnUpdateAllIOViews();
	afx_msg void OnExtrasSecuritySetup();
	afx_msg void OnModeEdit();
	afx_msg void OnUpdateModeEdit(CCmdUI* pCmdUI);
	afx_msg void OnTriggerSettings();
	afx_msg void OnUpdateTriggerSettings(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExtrasUtilitiesEdit(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExtrasSecuritySetup(CCmdUI* pCmdUI);
	afx_msg void OnEditRemoteOutputs();
	afx_msg void OnEditPublishedResults( UINT nID );
	afx_msg void OnRCClose();
	//}}AFX_MSG
#pragma endregion AFX_MSG Methods

	DECLARE_MESSAGE_MAP()

#pragma region AFX_VIRTUAL Methods
	//{{AFX_VIRTUAL(SVObserverApp)
	public:
	virtual BOOL InitInstance() override;
	virtual BOOL OnIdle(LONG lCount) override;
	virtual CDocument* OpenDocumentFile(LPCTSTR FileName) override;
	virtual void Serialize(CArchive& ar) override;
	virtual int ExitInstance() override;
	virtual int Run() override;
	//}}AFX_VIRTUAL
#pragma endregion AFX_VIRTUAL Methods

	//{{AFX_DATA(SVObserverApp)
	//}}AFX_DATA

public:
	
	HRESULT OpenSVXFile( LPCTSTR PathName );
	SVIODoc* NewSVIODoc( LPCTSTR DocName, SVIOController& Controller );	
	SVIPDoc* NewSVIPDoc( LPCTSTR DocName, SVInspectionProcess& Inspection );

	HRESULT  LoadConfiguration( unsigned long& ulSVOConfigVersion, BSTR bstrFileName, SVTreeType& p_rTree);

	std::string GetConfigurationName() const;
	
	HRESULT LoadPackedConfiguration( const std::string& p_rPackedFileName );
	HRESULT SavePackedConfiguration( const std::string& p_rPackedFileName );

	SVMainFrame* GetMainFrame() const;
	HRESULT CanCloseMainFrame();
	HRESULT DestroyConfig( bool AskForSavingOrClosing = true, bool CloseWithoutHint = false );
	void RemoveUnusedFiles();
	SVIODoc* GetIODoc() const;

	bool Logout( bool BForceLogout = false );
	bool InitPath( LPCTSTR PathName, bool CreateIfDoesNotExist = true, bool DeleteContents = true );

	bool IsMatroxGige() const;

	bool CheckSVIMType() const;
	SVIMProductEnum GetSVIMType() const;

	bool IsProductTypeRAID() const;
	void ValidateMRUList();

	void ResetAllCounts();
	bool AddMenuItem(HMENU hTargetMenu, const std::string& itemText, UINT itemID);
	bool RemoveMenu(HMENU hTargetMenu,  const std::string& itemText);
	HRESULT SetMode( unsigned long p_lNewMode );
	HRESULT LoadConfiguration();

	HRESULT OnObjectRenamed( const std::string& p_rOldName, const SVGUID& p_rObjectId );
	HRESULT RebuildOutputList();
	bool SetStatusText( LPCTSTR PStrStatusText );
	void UpdatePPQBar();

	const std::string& getConfigFileNameOnly() const;
	const std::string& getConfigPathName() const;
	const std::string& getConfigFileName() const;
	const std::string& getConfigFullFileName() const;
	bool setConfigFullFileName(LPCTSTR csFullFileName, DWORD bLoadFile = TRUE);

	SVIPDoc* GetIPDoc( const SVGUID& rInspectionID ) const;
	SVIPDoc* GetIPDoc( LPCTSTR StrIPDocPathName ) const;
	bool AlreadyExistsIPDocTitle( LPCTSTR StrIPDocTitle );

	bool ShowConfigurationAssistant( int Page = 3, bool bFileNewConfiguration = false );

	bool OkToEdit();
	void OnRCOpenCurrentSVX();
	void UpdateAllMenuExtrasUtilities();

	long GetSourceImageDepth() const;
	long GetMaxPPQLength() const;

	long GetResultImageDepth() const;
	long GetLogDataManager() const;
	long UpdateAndGetLogDataManager();

	void DeselectTool();

	HRESULT DisplayCameraManager(SVIMProductEnum eProductType);

	HRESULT DisconnectToolsetBuffers();
	HRESULT ConnectToolsetBuffers();

	HRESULT DisconnectCameras();
	HRESULT ConnectCameras();

	HRESULT SendCameraParameters();

	void SetGigePacketSizeDeviceParam(SVDeviceParamCollection* pDeviceParams);

	HRESULT SetModeEdit( bool p_bState );
	void SetTestMode(bool p_bNoSecurity = false);
	HRESULT GetTriggersAndCounts( std::string& rTriggerCounts ) const;

	void RefreshAllIPDocuments();
	void RunAllIPDocuments();
	void SetAllIPDocumentsOnline();
	void SetAllIPDocumentsOffline();
	void ResetAllIPDocModifyFlag(BOOL bModified);

	bool SetActiveIOTabView( SVTabbedViewSplitterIDEnum p_eTabbedID );
	void HideRemoteOutputTab();
	void HideRemoteMonitorListTab();
	void HideIOTab( DWORD p_dwID );
	void ShowIOTab( DWORD p_dwID );
	void UpdateRemoteInputTabs();

	HRESULT CheckDrive(const std::string& p_strDrive) const;
	//Arvid 150202: made this public because it is needed by ExtrasEngine
	//Arvid 150206: added parameter resetModifiedState so the main Modify flag is not changed when called during auto save
	//Arvid 150610: isRegularSave is currently false only for AutoSaves
	void fileSaveAsSVX( std::string StrSaveAsPathName = _T( "" ) ,bool isAutoSave=false );


#pragma region Encapsulation Methods
	long getGigePacketSize() const { return m_rInitialInfo.m_gigePacketSize; }
	long getOfflineCount() const { return m_OfflineCount; }
	SVIPDoc* getCurrentDocument() const { return m_pCurrentDocument; }
	void setCurrentDocument(SVIPDoc* pIPDoc) { m_pCurrentDocument = pIPDoc; }
	bool getShouldRunLastEnvironmentAutomatically() const { return m_ShouldRunLastEnvironmentAutomatically; }
	DWORD getCurrentVersion() const { return m_CurrentVersion; }
	DWORD getLoadingVersion() const { return m_LoadingVersion; }
	void setLoadingVersion(DWORD version) { m_LoadingVersion = version; }
	long getDataValidDelay() const { return m_DataValidDelay; }

	bool IsForcedImageUpdateActive() const { return (m_rInitialInfo.m_forcedImageUpdateTimeInSeconds) ? true : false; }
	unsigned char GetForcedImageUpdateTimeInSeconds() const { return m_rInitialInfo.m_forcedImageUpdateTimeInSeconds; }
#pragma endregion
#pragma endregion Public Methods

#pragma region Protected Methods
protected:

	/// Start the configuration (go online).
	/// In error cases this method throw Exception.
	void Start();

	HRESULT DisconnectAllCameraBuffers();
	HRESULT ConnectCameraBuffers();
	HRESULT InitializeSecurity();

    bool OpenConfigFileFromMostRecentList(int nID);
	void EnableTriggerSettings();
	void DisableTriggerSettings();

	HRESULT ConstructDocuments( SVTreeType& p_rTree );

	HRESULT ConstructMissingDocuments();

	bool DestroyMessageWindow();
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void OnStopAll();
	bool InitATL();
	void StopRegression();

	bool AddSecurityNode(HMODULE hMessageDll, long lId, LPCTSTR NTGroup, bool bForcePrompt = false);
	bool AddSecurityNode( HMODULE hMessageDll, long lId );

	static int FindMenuItem(CMenu* Menu, LPCTSTR MenuString);

	bool DetermineConfigurationSaveName(); ///< determines the name under which a configuration is to be changed
	void SaveConfigurationAndRelatedFiles(bool isAutoSave); ///< saves the configuration and additional files


#pragma endregion Private Methods

//@HACK Public Member variables

#pragma region Public member variables
public:
	SVSecurityManager m_svSecurityMgr;	// Security
	// For managing remotely created/edited fonts through the SIAC
	SVRemoteFontManager m_mgrRemoteFonts;

	typedef std::map<UINT, SVUtilityIniClass> UtilityMenuMap;
	UtilityMenuMap m_UtilityMenu;

#pragma endregion Public Member variables

#pragma region Member variables
private:
	HANDLE m_hAppThread;

	SvStl::SVOIntelRAIDStatusClass m_IntelRAID;

	SVIPDoc* m_pCurrentDocument;
	long m_OfflineCount;
	bool m_ShouldRunLastEnvironmentAutomatically;
	DWORD m_AutoRunDelayTime;

	// Version numbering code:
	//
	// m_CurrentVersion, Type DWORD
	// Meaning:		0x00mmssbb => Version m.s  - Subversion b 
	// EXCEPTION:	if m == 0 then s = b and b = 0 !!! 
	//					No Subversion number in not releasable
	//					versions!
	//	e.g.
	//				0x00000002 => Version 0.2  - Subversion 00 ( Subversion 'a' )
	//				0x00010000 => Version 1.0  - Subversion 00 ( Subversion 'a' )
	//		        0x00011219 => Version 1.18 - Subversion 25 ( Subversion 'z' )
	//				0x00020001 => Version 2.0  - Subversion 01 ( Subversion 'b' )
	DWORD m_CurrentVersion;
	// Refer to m_CurrentVersion
	// Version of the file, 
	//  which SVObserver is now loading
	DWORD m_LoadingVersion;

	// The Standard Configuration Execution Directory
	// PN -> Path Name
	LPCTSTR m_ConfigExePNVariableName;
	std::string m_ConfigExePNVariableValue;

	// The Standard Last Valid Configuration Directory
	// PN -> Path Name
	LPCTSTR m_LastValidConfigPNVariableName;
	std::string m_LastValidConfigPNVariableValue;

	long m_lSouceImageDepth;
	long m_lResultImageDepth;
	long m_LogDataManager;

	SVMessageWindowClass* m_pMessageWindow;

	bool m_ATLInited;

	mutable SVFileNameClass m_ConfigFileName;	// JMS - Added for File Management

	HMENU m_hAddMenu;
	HANDLE m_hEvent;

	unsigned short m_InputStreamPortNumber;
	unsigned short m_OutputStreamPortNumber;
	unsigned short m_RemoteCommandsPortNumber;
	unsigned short m_FailStatusStreamPortNumber;

	long m_DataValidDelay;

	InitialInformationHandler m_IniInfoHandler;

	const SvLib::InitialInformation &m_rInitialInfo; // This reference exists to simplify access to InitializationStatusFlags()

#pragma endregion Member variables
};

extern SVObserverApp TheSVObserverApp;

////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// DevStudio inserts additional declarations immediate in front of the preceding line
////////////////////////////////////////////////////////////////////////////////

