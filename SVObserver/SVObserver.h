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
#include "SVMaterialsLibrary/SVMaterialsTree.h"
#include "SVImageLibrary/SVDigitizerLoadLibraryClass.h"
#include "SVIOLibrary/SVIOTriggerLoadLibraryClass.h"
#include "SVOMFCLibrary/SVOINIClass.h"
#include "SVSecurity/SVSecurityManager.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVOMFCLibrary/SVFileNameClass.h"
#include "SVRemoteFontManager.h"
#include "SVOIntelRAIDStatusClass.h"
#include "SVUtilityIniClass.h"
#include "SVIOTabbedView.h"
#include "SVConfigurationLibrary/SVObserverEnums.h"
#pragma endregion Includes

#pragma region Declarations
class SVConfigurationObject;
class SVIMServerWrapper;
class SVInspectionProcess;
class SVIOController;
class SVIODoc;
class SVIPDoc;
class SVLVFastOCR;
class SVMainFrame;
class SVMessageWindowClass;
class SVTriggerObject;
class SVUserObjectListClass;
class SVObjectWriter;

enum SVConfigFileActionsEnums
{
	LOAD_CONFIG = TRUE,
	SAVE_CONFIG = FALSE,
	RENAME = 2,
	RENAME_CONFIG = RENAME,
	NUM_CONFIG_FILE_ACTIONS
};

enum SVHardwareErrorEnums
{
	SV_HARDWARE_FAILURE_IO          = 0xC0000001L,
	SV_HARDWARE_FAILURE_TRIGGER     = 0xC0000004L,
	SV_HARDWARE_FAILURE_ACQUISITION = 0xC0000008L,
	SV_HARDWARE_FAILURE_NO_CAMAERAS = 0xC0000010L,
	SV_HARDWARE_FAILURE_SOFTWARETRIGGER = 0xC0000020L,
	SV_HARDWARE_FAILURE_FILEACQUISITION = 0xC0000040L,
	SV_HARDWARE_FAILURE_CAMERATRIGGER = 0xC0000080L,
	SV_HARDWARE_FAILURE_ALL         = 0xC00000FFL, // this is all the above combined

	SV_CAN_GO_ONLINE_FAILURE_TRIGGER     = 0xC0000100L,
	SV_CAN_GO_ONLINE_FAILURE_ACQUISITION = 0xC0000200L,
	SV_CAN_GO_ONLINE_FAILURE_INSPECTION  = 0xC0000400L,
	SV_CAN_GO_ONLINE_FAILURE_CONDITIONAL_OUTPUT = 0xC0000800L,
	SV_CAN_GO_ONLINE_FAILURE_ALL         = 0xC0000F00L, // this is all the above combined

	SV_GO_ONLINE_FAILURE_ALREADY_ONLINE  = 0xC0010000L,
	SV_GO_ONLINE_FAILURE_RECYCLE_PRODUCT = 0xC0020000L,
	SV_GO_ONLINE_FAILURE_INSPECTION      = 0xC0040000L,
	SV_GO_ONLINE_FAILURE_ACQUISITION     = 0xC0080000L,
	SV_GO_ONLINE_FAILURE_TRIGGER         = 0xC0100000L,
	SV_GO_ONLINE_FAILURE_ALL             = 0xC01F0000L, // this is all the above combined
};

typedef SVXMLMaterialsTree SVTreeType;
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
	afx_msg void OnUpdateAddShiftTool(CCmdUI* PCmdUI);
	afx_msg void OnUpdateAddWindowTool(CCmdUI* PCmdUI);
	afx_msg void OnUpdateAppExit(CCmdUI* PCmdUI);
	afx_msg void OnUpdateAppAbout(CCmdUI* PCmdUI);
	afx_msg void OnUpdateAddLoadImageTool(CCmdUI* PCmdUI);
	afx_msg void OnUpdateAddImageTool(CCmdUI* PCmdUI);
	afx_msg void OnUpdateAddArchiveTool(CCmdUI* PCmdUI);
	afx_msg void OnUpdateAddMathTool(CCmdUI* PCmdUI);
	afx_msg void OnUpdateAddStatisticsTool(CCmdUI* PCmdUI);
	afx_msg void OnFilePrintConfig();
	afx_msg void OnSVOFilePrintSetup();
	afx_msg void OnExtrasLogin();
	afx_msg void OnUpdateExtrasLogin(CCmdUI* PCmdUI);
	afx_msg void OnUpdateExtrasUserManager(CCmdUI* PCmdUI);
	afx_msg void OnUpdateViewToolBuffers(CCmdUI* PCmdUI);
	afx_msg void OnUpdateSetupMode(CCmdUI* PCmdUI);
	afx_msg void OnUpdateAddTransformationTool(CCmdUI* PCmdUI);
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
	afx_msg void OnUpdateAddPolarUnwrapTool(CCmdUI* PCmdUI);
	afx_msg void OnUpdateAddAcquisitionTool(CCmdUI* PCmdUI);
	afx_msg void OnUpdateResumeFreeze(CCmdUI* PCmdUI);
	//afx_msg void OnUpdateSelectPPQVariable(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAddColorTool(CCmdUI* PCmdUI);
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
	afx_msg void OnUpdateAddExternalTool(CCmdUI* pCmdUI);
	afx_msg void OnExtrasSecuritySetup();
	afx_msg void OnModeEdit();
	afx_msg void OnUpdateModeEdit(CCmdUI* pCmdUI);
	afx_msg void OnTriggerSettings();
	afx_msg void OnUpdateTriggerSettings(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExtrasUtilitiesEdit(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAddLinearTool(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAddRemoteInputTool(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAddResizetool(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAddRingBufferTool(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAddCylindricalWarpTool( CCmdUI* PCmdUI );
	afx_msg void OnUpdateAddPerspectiveTool(CCmdUI* pCmdUI);
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
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	virtual CDocument* OpenDocumentFile(LPCTSTR FileName);
	virtual void Serialize(CArchive& ar);
	virtual int ExitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL
#pragma endregion AFX_VIRTUAL Methods

	//{{AFX_DATA(SVObserverApp)
	//}}AFX_DATA

public:
#pragma region virtual
	virtual HRESULT OpenSVXFile( LPCTSTR PathName );
	virtual void LoadTempIODoc( LPCTSTR FileName );
	virtual void LoadTempIPDoc( LPCTSTR FileName );
	virtual SVIODoc* NewSVIODoc( LPCTSTR DocName, SVIOController& Controller );	
	virtual SVIPDoc* NewSVIPDoc( LPCTSTR DocName, SVInspectionProcess& Inspection );
#pragma endregion virtual

	CString GetConfigurationName() const;

	SVString getModelNumber() const;
	SVString getSerialNumber() const;
	SVString getWinKey() const;

	HRESULT LoadPackedConfiguration( const CString& p_rPackedFileName );
	HRESULT SavePackedConfiguration( const CString& p_rPackedFileName );

	SVMainFrame* GetMainFrame() const;
	HRESULT CanCloseMainFrame();
	HRESULT DestroyConfig( BOOL AskForSavingOrClosing = TRUE, BOOL CloseWithoutHint = FALSE );
	void RemoveUnusedFiles();
	SVIODoc* GetIODoc() const;

	BOOL Login();
	BOOL Logout( BOOL BForceLogout = FALSE );
	BOOL InitPath( LPCTSTR TStrPathName, BOOL BCreateIfNotExists = TRUE, BOOL BDeleteContents = TRUE );

	BOOL InitSVIMServer();
	void DestroySVIMServer();
	BOOL IsSVIMServerEnabled() const;

	bool IsMatrox1394() const;
	bool IsCoreco() const;
	bool IsMatroxGige() const;

	BOOL CheckSVIMType() const;
	SVIMProductEnum GetSVIMType() const;

	bool IsProductTypeRAID() const;
	void ValidateMRUList();

	SVOINIClass& INI();
	void ResetAllCounts();
	bool AddMenuItem(HMENU hTargetMenu, const CString& itemText, UINT itemID);
	bool RemoveMenu(HMENU hTargetMenu,  const CString& itemText);
	HRESULT SetMode( unsigned long p_lNewMode );
	HRESULT LoadConfiguration();

	HRESULT RenameObject( const SVString& p_rOldName, const SVString& p_rNewName, const SVGUID& p_rObjectId );
	HRESULT RebuildOutputList();
	BOOL SetStatusText( LPCTSTR PStrStatusText );
	void UpdatePPQBar();

	LPCTSTR getConfigFileNameOnly() const;
	LPCTSTR getConfigPathName() const;
	LPCTSTR getConfigFileName() const;
	LPCTSTR getConfigFullFileName() const;
	BOOL setConfigFullFileName(LPCTSTR csFullFileName, DWORD bLoadFile = TRUE);

	SVIPDoc* GetIPDoc( LPCTSTR StrIPDocPathName ) const;
	BOOL AlreadyExistsIPDocTitle( LPCTSTR StrIPDocTitle );
	CString GetStringResource( int ResourceID ) const;

	BOOL ShowConfigurationAssistant( int Page = 3, BOOL bFileNewConfiguration = FALSE );

	bool OkToEdit();
	BOOL IsMonochromeImageAvailable();
	void SVGetCurrentConfigName( CString &ConfigName ) const;
	void OnRCOpenCurrentSVX();
	void SetupSVIMServer();
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

	HRESULT DisconnectCameras( CStringArray& rDisconnectedCameras );
	HRESULT ConnectCameras( const CStringArray& rCamerasToConnect );

	HRESULT SendCameraParameters( const CStringArray& rCameras );

	void SetGigePacketSizeDeviceParam(SVDeviceParamCollection* pDeviceParams);

	HRESULT SetModeEdit( bool p_bState );
	void SetTestMode(bool p_bNoSecurity = false);
	HRESULT GetTriggersAndCounts( CString& p_strTrigCnts ) const;

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

	HRESULT CheckDrive(const CString& p_strDrive) const;
	//Arvid 150202: made this public because it is needed by AutoSaver
	//Arvid 150206: added parameter resetModifiedState so the main Modify flag is not changed when called during auto save
	//Arvid 150610: isRegularSave is currently false only for AutoSaves
	void fileSaveAsSVX( CString StrSaveAsPathName = _T( "" ) ,bool isAutoSave=false );

#pragma region Encapsulation Methods
	BOOL getShowUpdateFirmwareInMenu() const { return m_ShowUpdateFirmwareInMenu; }
	void setShowUpdateFirmwareInMenu(BOOL val) { m_ShowUpdateFirmwareInMenu = val; }
	long getGigePacketSize() const { return m_gigePacketSize; }
	long getOfflineCount() const { return m_OfflineCount; }
	SVIPDoc* getCurrentDocument() const { return m_pCurrentDocument; }
	void setCurrentDocument(SVIPDoc* pIPDoc) { m_pCurrentDocument = pIPDoc; }
	BOOL getShouldRunLastEnvironmentAutomatically() const { return m_ShouldRunLastEnvironmentAutomatically; }
	DWORD getCurrentVersion() const { return m_CurrentVersion; }
	DWORD getLoadingVersion() const { return m_LoadingVersion; }
	void setLoadingVersion(DWORD version) { m_LoadingVersion = version; }
#pragma endregion
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	HRESULT Start();

	HRESULT INILoad();
	HRESULT INIClose();
	HRESULT INIReset();

	HRESULT LoadTriggerDLL();
	HRESULT CloseTriggerDLL();

	HRESULT LoadSoftwareTriggerDLL();
	HRESULT LoadAcquisitionTriggerDLL();

	HRESULT LoadAcquisitionDLL();
	HRESULT CloseAcquisitionDLL();

	HRESULT LoadFileAcquisitionDLL();

	HRESULT LoadDigitalDLL();
	HRESULT CloseDigitalDLL();

	LPCTSTR GetProcessorBoardName() const;
	LPCTSTR GetTriggerBoardName() const;
	LPCTSTR GetSoftwareTriggerBoardName() const;
	LPCTSTR GetAcquisitionBoardName() const;
	LPCTSTR GetFileAcquisitionBoardName() const;
	LPCTSTR GetDigitalBoardName() const;

	LPCTSTR GetRAIDBoardName() const;

	HRESULT DisconnectAllCameraBuffers( CStringArray& rDisconnectedCameras );
	HRESULT ConnectCameraBuffers( const CStringArray& rCamerasToConnect );
	HRESULT InitializeSecurity();

    BOOL OpenConfigFileFromMostRecentList(int nID);
	void EnableTriggerSettings();
	void DisableTriggerSettings();

	HRESULT ConstructDocuments( SVTreeType& p_rTree );

	HRESULT ConstructMissingDocuments();

	BOOL DestroyMessageWindow();
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void OnStopAll();
	BOOL InitATL();
	void StopRegression();
	DWORD GetActiveIOTab() const;

	bool AddSecurityNode(HMODULE hMessageDll, long lId, CString sNTGroup, bool bForcePrompt = false);
	bool AddSecurityNode( HMODULE hMessageDll, long lId );

	static int FindMenuItem(CMenu* Menu, LPCTSTR MenuString);

#pragma endregion Private Methods

//@HACK Public Member variables

#pragma region Public member variables
public:
	//firmware command strings
	CString m_csFirmwareCommand;
	CString m_csFirmwareWorkingDir;
	CString m_csFirmwareArguments;

	SVSecurityManager m_svSecurityMgr;	// Security
	// For managing remotely created/edited fonts through the SIAC
	SVRemoteFontManager m_mgrRemoteFonts;

	typedef std::map<UINT, SVUtilityIniClass> UtilityMenuMap;
	UtilityMenuMap m_UtilityMenu;
	//@WARNING m_MaterialsTree MUST be before m_XMLTree in this file, 
	// because it must be initialized in this order. (m_XMLTree need m_MaterialsTree in the constructor)
	SVMaterialsTree m_MaterialsTree;
	SVXMLMaterialsTree m_XMLTree;
	SVXMLMaterialsTree::SVBranchHandle m_Inspection;


#pragma endregion Public Member variables

#pragma region Member variables
private:
	// *** // ***
	HANDLE m_hAppThread;
	// *** // ***

	SVOIntelRAIDStatusClass m_IntelRAID;

	SVLVFastOCR* m_pFastOcr;
	SVIPDoc* m_pCurrentDocument;
	long m_OfflineCount;
	BOOL m_ShouldRunLastEnvironmentAutomatically;
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

	CString m_csDigitizerDLL;
	SVDigitizerLoadLibraryClass m_svDLLDigitizers;

	CString m_csFileAcquisitionDLL;
	SVDigitizerLoadLibraryClass m_svDLLFileAcquisition;

	SVIOTriggerLoadLibraryClass m_svDLLTriggers;
	SVIOTriggerLoadLibraryClass m_svDLLSoftwareTriggers;
	SVIOTriggerLoadLibraryClass m_svDLLAcquisitionTriggers;

	long m_gigePacketSize; 

	CString m_csShowUpdateFirmware;
	BOOL m_ShowUpdateFirmwareInMenu;
	std::set<SVTriggerObject*> m_dirty_triggers;

	// The Standard Configuration Execution Directory
	// PN -> Path Name
	LPCTSTR m_ConfigExePNVariableName;
	LPCTSTR m_ConfigExePNVariableDefaultValue;
	CString m_ConfigExePNVariableValue;

	// The Standard Last Valid Configuration Directory
	// PN -> Path Name
	LPCTSTR m_LastValidConfigPNVariableName;
	LPCTSTR m_LastValidConfigPNVariableDefaultValue;
	CString m_LastValidConfigPNVariableValue;

	long m_lSouceImageDepth;
	long m_lResultImageDepth;
	long m_LogDataManager;

	SVMessageWindowClass* m_pMessageWindow;

	typedef SVVector< SVFileNameClass* > SVFileNamePtrVector;

	SVIMServerWrapper* m_pSVIMServerWrapper;

	SVFileNamePtrVector m_svFileNames;

	BOOL m_ATLInited;

	mutable SVFileNameClass m_ConfigFileName;	// JMS - Added for File Management

	CString m_csProductName;

	CString m_csProcessor;
	CString m_csFrameGrabber;
	CString m_csIOBoard;
	CString m_csOptions;

	CString m_csTrigger;

	CString m_csTriggerDLL;
	CString m_csSoftwareTriggerDLL;
	CString m_csAcquisitionTriggerDLL;
	CString m_csDigitalDLL;
	CString m_csDigitalOption;			// TRB - Added to have a parameter to send to Rabbit Board.


	CString m_csReloadTriggerDLL;
	CString m_csReloadAcquisitionDLL;
	CString m_csReloadDigitalDLL;

	CString m_csProcessorBoardName;
	CString m_csTriggerBoardName;
	CString m_csAcquisitionBoardName;
	CString m_csFileAcquisitionBoardName;
	CString m_csDigitalBoardName;

	CString m_csRAIDBoardName;

	HRESULT m_hrHardwareFailure;

	SVOINIClass m_SvimIni;
	BOOL m_bImageCompressionStarted;
	HMENU m_hAddMenu;
	HANDLE m_hEvent;

	unsigned short m_InputStreamPortNumber;
	unsigned short m_OutputStreamPortNumber;
	unsigned short m_RemoteCommandsPortNumber;
	unsigned short m_FailStatusStreamPortNumber;
#pragma endregion Member variables
};

extern SVObserverApp TheSVObserverApp;

////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// DevStudio inserts additional declarations immediate in front of the preceding line
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVObserver.h_v  $
 * 
 *    Rev 1.21   18 Feb 2015 11:06:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  975
 * SCR Title:  Add Fail Status Stream (SVO-354)
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added FailStatusStreamPortNumber member variable.
 * Revised all port number member variables to be of type unsigned short.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   09 Dec 2014 10:12:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  908
 * SCR Title:  Remove option for Operator Move (SVO 101)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Remove edit move state and supporting functions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   01 Dec 2014 13:05:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add ThreadInfo dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   26 Jun 2014 17:54:24   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   cleanup
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   02 Jun 2014 10:10:42   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed the Environment constants.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   20 May 2014 10:20:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  904
 * SCR Title:  Update SVObserver to use HtmlHelp instead of WinHelp
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Enabled HtmlHelp in the constructor.  Removed method WinHelp
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   15 May 2014 13:34:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised WinHelp method override to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   28 Apr 2014 13:38:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  873
 * SCR Title:  Fix inconsistant GUI labels and functionality on IO pages
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Update_Command_UI_Range for the Published Results.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   17 Apr 2014 17:00:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added functionality for the Monitor List View
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   17 Mar 2014 15:27:18   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added Environment strings and accessor functions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   07 Mar 2014 18:21:38   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Made methods const.
 *   Removed empty method DisplayAddMenu.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   04 Feb 2014 15:33:28   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  880
 * SCR Title:  Remove .SEC
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Removed unused includes for SVVector.h and SVStorage.h.
 * Reordered methods to group them to public, protected and private.
 * Changed member variables to private and encapsulated with access methods if needed.
 * Removed methods:
 *  - OnFileSaveAsSec
 *  - OnUpdateFileCloseSec
 *  - OnUpdateFileSaveAsSec
 *  - OnUpdateFileSaveSec
 *  - OnRCLoadSEC
 *  - CreateSEC
 *  - OpenSECFile
 *  - OnRCOpenCurrentSEC
 * Removed member variables:
 *  - SECMemoryCopyFile
 *  - IsSECMemoryCopyAvailable
 *  - IsSECMemorySaving
 *  - IsSECMemoryLoading
 *  - MemorySECDataPath
 *  - MemorySECPathName
 *  - MemorySECTitle
 *  - ViewToolBuffers
 *  - RefreshRate
 *  - CurrentSECID
 *  - CreationSECDate
 *  - CurrentSECDate
 *  - m_bOpeningSEC
 * Renamed methods as follows:
 *  - OnFileNewSEC -> OnFileNewConfig
 *  - OnFileSaveSec -> OnFileSaveConfig
 *  - OnFileCloseSec -> OnFileCloseConfig
 *  - OnFilePrintSec -> OnFilePrintConfig
 *  - OnFilePrintPreviewSec -> OnFilePrintPreviewConfig
 *  - OnUpdateFilePrintSec -> OnUpdateFilePrintConfig
 *  - OnRCSaveAllAndGetSEC -> OnRCSaveAllAndGetConfig
 *  - DestroySEC -> DestroyConfig
 *  - GetSECFileNameOnly -> getConfigFileNameOnly
 *  - GetSECPathName -> getConfigPathName
 *  - GetSECFileName -> getConfigFileName
 *  - GetSECFullFileName -> getConfigFullFileName
 *  - SetSECFullFileName -> setConfigFullFileName
 *  - fileSaveAsSec -> fileSaveAsSVX
 *  - OpenSECFileFromMostRecentList -> OpenConfigFileFromMostRecentList
 * Renamed member variable msvSECFileName -> m_ConfigFileName.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   31 Jan 2014 17:16:34   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed to follow guidelines more closely.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   09 Dec 2013 07:51:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  873
 * SCR Title:  Fix inconsistant GUI labels and functionality on IO pages
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added dynamic menu option in IO Page - Edit menu - Edit Published Results for each inspection.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   30 Oct 2013 10:45:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added #ifndef _WIN64 to prevent depricated PLC code from compiling in 64bit.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   01 Oct 2013 15:24:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   29 Jul 2013 11:40:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  843
 * SCR Title:  Add check to SVObserver startup to check V-Drive.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added const ref and const to CheckDrive function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   26 Jul 2013 09:47:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  843
 * SCR Title:  Add check to SVObserver startup to check V-Drive.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added V:\\ drive check for SVRemote Control.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   11 Jun 2013 15:26:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.142   21 May 2013 13:04:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added member variables and methods to allow the Windows Key, Model Number, and Serial Number to be retrieved remotely.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.141   21 May 2013 10:03:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  836
 * SCR Title:  Fix loading SEC bug
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Moved SVObserverEnums.h to SCConfigurationLibrary.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 May 2013 16:16:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.140   29 Apr 2013 14:23:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated header to remove modeless debug definition.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 20:06:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.139   16 Apr 2013 15:35:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:43:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.138   27 Mar 2013 14:39:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed ShouldShowInpsectionToolbar variable
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.137   25 Mar 2013 13:36:42   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed UpdateInspectionToolBar
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.136   11 Feb 2013 14:19:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new framework for the remote socket interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.135   21 Jan 2013 11:46:44   sjones
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
 *    Rev 3.134   16 Jan 2013 16:08:50   jspila
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
 *    Rev 3.133   10 Jan 2013 16:16:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added missing log entries.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.132   07 Dec 2012 10:36:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new method OnUpdateAddShiftTool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.131   04 Oct 2012 11:25:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Replaced the CTreeCtrl with MaterialsTree.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.130   28 Sep 2012 14:49:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated saving operation to use the object writer instead of the XML tree.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.129   28 Sep 2012 14:38:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver application tree storage member variable name.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.128   25 Sep 2012 15:21:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.127   18 Sep 2012 18:29:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.126   11 Sep 2012 09:45:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  783
 * SCR Title:  Update SVObserver to Validate PLC Sub-system Before Entering a Running Mode
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add check for PLC when going online.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.125.1.0   05 Dec 2012 11:54:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new method OnUpdateAddShiftTool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.125   15 Aug 2012 17:01:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  772
 * SCR Title:  Implement GigE Camera Reconnect Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code associated with gige packet size parameter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.124   15 Aug 2012 14:41:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new Singleton version of the SVIOConfigurationInterfaceClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.123   09 Aug 2012 16:07:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues when enabling and disabling menu options depending on the application state.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.122   06 Jul 2012 07:15:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  760
 * SCR Title:  Filter Tool Name characters
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added additional exclude characters to prevent illegal characters from being entered in a tool name..
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.121   02 Jul 2012 17:23:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.120   28 Jun 2012 16:17:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removing Shared Memory for the webserver from the SVObserver application.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.119   19 Jun 2012 13:12:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Centralized the location of the configuration object identifier and removed synchronization problems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.118   20 Mar 2012 09:08:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed issue with Remote Input Tool not being grayed out.  Should not be active unless in Edit Mode.  Added new method - OnUpdateAddeRemoteInputTool 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.117   13 Mar 2012 13:49:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  745
 * SCR Title:  Fix contention issue during load process and display
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated RunAllIPDocuments method, ShowConfigurationAssistant method, and IsMonochromeImageAvailiable method to fix formatting and consolidate functionality to IPDoc.  Added new method SetSelectedToolForOperatorMove method for operator move functionailty.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.116   10 Feb 2012 09:35:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated functionality to remove bugs associated with the remote input tool and communications to that tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.115   30 Jan 2012 11:58:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Modified source code to consolidate PLC and New Output stream functionality to I/O Controller.  Added new output stream functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.114   16 Jan 2012 15:29:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated main application and create the command data holder for the JSON commands used with the socket and the remote input tool.  Updated source code to initialize, start and stop the scoket for sending and receiving data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.113   19 Dec 2011 16:15:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated applciation to handle socket processing and share memory configuration.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.112   14 Jul 2011 08:59:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to better track resource management, and clear un-used resources.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.111   14 Jul 2011 08:17:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  729
 * SCR Title:  Adjust SVObserver to fix perfomance problems due to logging
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to remove logging in places that cause performance issues and added a registry controlled variable to handle the go offline data manager dumps to files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.110   06 Jun 2011 13:06:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  722
 * SCR Title:  Fix a problem with Raid Error Information on X2 systems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to correct issues with the RAID error information associated with the X2 products.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.109   27 Apr 2011 11:11:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  720
 * SCR Title:  Change menu options for Test and Regession Modes
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated Menu Options when in Test or Regression Mode
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.108   22 Mar 2011 08:01:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated static global classes to singletons to promote proper and defined construction and destruction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.107   27 Jan 2011 11:41:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.106   18 Jan 2011 12:42:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   added new method "ResetAllIPDocModifyFlag" to walk and update all IPDoc's to reset the Modified Flag.  
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.105   08 Dec 2010 13:15:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.104   09 Nov 2010 16:25:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.103   04 Nov 2010 14:08:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.102   01 Jun 2010 10:59:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.101   19 Feb 2010 15:55:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated version number to 5.31 Beta 035 and added new logic to preduce error message if the hardware does not exist.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.100   15 Feb 2010 12:37:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to remove the generations of board image processing classes from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.99   16 Dec 2009 12:08:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.98   30 Jul 2009 12:14:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.97   23 Jun 2009 15:00:48   jhanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  654
 * SCR Title:  Software Trigger Runtime Adjustments
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Added SoftwareTriggerDialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.96   17 Jun 2009 09:03:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added IsMatroxGigeSystem method
 * Revised the DisplayCameraManager method to use the ProductType as a calling argument
 * Added SetGigePacketSizeDeviceParam method
 * Added variable for Gige packet size
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.95   27 Apr 2009 14:33:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  659
 * SCR Title:  Modify SVObservers IO system behavior to be based on a hardware.ini setting
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Digital option for rabbit board
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.94   27 Jan 2009 14:51:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  647
 * SCR Title:  Add Information to Event Log
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added helper function collect trigger name and trigger counts in a string.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.93   14 Jan 2009 16:11:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Moved Edit / Add PLC Wizard logic to data controller class
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.92   04 Dec 2008 13:37:12   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Moved DirectDraw member variables to singleton class SVDirectX
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.91   14 Nov 2008 14:34:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added PLC Outputs and IO Tab Views
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.90   16 Sep 2008 15:03:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added IsFileAcquisitionAllowed method.
 * Added LoadFileAcquisitionAllowedDLL method.
 * Added GetFileAcquisitionBoardName method.
 * Added enum for SV_HARDWARE_FAILURE_FILEACQUISITION.
 * Added member variables for FileAcquisition.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.89   12 Sep 2008 11:59:52   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added public member variable m_useDirectX
 * Moved DirectDraw object, surface, and clipper here from SVImageView.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.88   11 Sep 2008 07:51:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added methods and variables for Software Trigger
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.87   30 May 2008 13:25:16   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Removed DirectX changes made in revision 3.85 and 3.86.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.86   15 May 2008 15:09:46   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added public member variable m_useDirectX
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.85   10 Apr 2008 08:07:44   BWalter
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
 *    Rev 3.84   31 Oct 2007 08:14:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added member variable to manage hEvent used for SingleInstance of App.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.83   21 Jun 2007 14:33:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.82.1.1   14 Jul 2009 14:52:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated application information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.82.1.0   30 Oct 2007 10:36:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added HEvent member variable so cleanup will occur in ExitInstance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.82   08 May 2007 11:28:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  600
 * SCR Title:  Fix Bug where File - print setup menu option does not function.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed message handler for OnFilePrintSetup
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.81   09 Apr 2007 11:55:14   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  588
 * SCR Title:  Add the RAID Error Bit for the digital IO
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   new method to determine if the product has RAID
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.80   02 Mar 2007 06:25:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  595
 * SCR Title:  Fix issues with Utility Menu when switching products
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new firmware settings and a new utility map
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.79   25 Aug 2006 08:40:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  577
 * SCR Title:  Fix Security bug that interferes with SIAC GoOnline command.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Message Handler OnRcGoOnline to handle the remote goOnline Message.  Also added SetTestMode with a parameter to use no security.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.78   16 Aug 2006 09:56:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  567
 * SCR Title:  Update SVObserver to accept Intek driver for D-series SVIMs
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated the INI Load, Close, and Reset code to allow DLLs to not be reloaded on a close of a configuration.  The reload logic is controlled by the HARDWARE.INI file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.77   04 Aug 2006 08:03:44   tbair
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
 *    Rev 3.76   21 Jun 2006 15:06:26   ebeyeler
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
 *    Rev 3.75   20 Jan 2006 09:02:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  538
 * SCR Title:  Remove instances when displaying RAID error message on event log read errors
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated inspection process to not allow RAID to be checked if not setup as intel in the hardware.ini and no the right processor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.74   22 Dec 2005 15:20:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  530
 * SCR Title:  Allow SVObserver to monitor Intel SATA RAID status
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new resources for the new modeless message box.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.73   08 Dec 2005 08:02:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added ON_UPDATE_COMMAND_UI message handler for Published Images menu option.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.72   14 Nov 2005 08:42:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated error message when Intek camera driver does not find a camera.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.71   11 Nov 2005 10:49:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a new function to see if the current hardware porduct is a Coreco product.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.70   08 Nov 2005 16:02:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated 1394 code to allow for the use of the Intek driver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.69   24 Aug 2005 16:23:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Message Handler for Start Test Mode Button.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.68   17 Aug 2005 11:49:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Accesspoint for exiting run
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.67   16 Aug 2005 15:19:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  506
 * SCR Title:  Update Tool Set List Tab to include an indicator when a tool is invalid
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new method to run all inspections.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.66   02 Aug 2005 13:52:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Functions OkToEdit and OkToEditMove to make menus more consistant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.65   22 Jul 2005 13:55:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new method DeselectTool.  Will deselect tool for toolset list when going into differnet modes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.64   18 Jul 2005 14:38:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   GUI Change - Removed m_dwPreviousState for AutoOffline to Edit
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.63   15 Jul 2005 11:32:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new method StopRegression. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.62   06 Jul 2005 14:05:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Auto Redirect - Offline to Edit logic. in OnStop() and m_dwPrevState
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.61   27 Jun 2005 15:02:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SVEXCLUDECHARS_*
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.60   27 Jun 2005 14:55:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   #define SVTOOLPARAMETERLIST_MARKER
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.59   21 Jun 2005 14:15:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added SVSecurity functions for access.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.58   18 May 2005 15:02:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  485
 * SCR Title:  Add a way to Reset Counts remotely
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added function ResetAllCounts(.  This resets all IPDoc counters by adding an input request to each IP.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.57   09 Mar 2005 13:13:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  478
 * SCR Title:  Add a new Perspective Warp Tool
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Allow the PerspectiveWarp Tool to be added from the add menu.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.56   09 Mar 2005 12:24:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  477
 * SCR Title:  Add Cylindrical warp Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added CylindricalWarp tool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.55   22 Feb 2005 13:56:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  452
 * SCR Title:  Upgrade SVObserver to version 4.50
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated to add new model numbers for matrox boards.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.54   18 Feb 2005 11:05:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated SVObserver App and project files to include new linear tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.53   17 Feb 2005 14:57:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.52   10 Aug 2004 10:51:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  432
 * SCR Title:  Fix SVObserver to allow Image Compression to work via the ActiveX
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problem so that ImageCompression will not startup or get deleted on a second copy of SVObserver.  
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.51   12 Jul 2004 10:37:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added OnUpdateAddExternalTool
 * added INI() access
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.50   22 Mar 2004 13:25:28   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  409
 * SCR Title:  Fix loading of a configuration from the Run directory.
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed loading from the run directory to not delete unused files until all the objects have been parsed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.49   12 Feb 2004 16:49:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed bug with Camera Manager
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.48   04 Feb 2004 14:25:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  403
 * SCR Title:  Add ActiveX interfaces to allow management of Matrox fonts through the SIAC
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new code to support new Matrox font setup
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.47   21 Jan 2004 15:46:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed merge problem
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.46   16 Jan 2004 07:39:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added m_svStrobeSubsystem
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.45   17 Dec 2003 06:17:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  389
 * SCR Title:  Present Error Messages for Invalid Hardware using User Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new method to handle checking for configuration to hardware compatibility.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.44   12 Nov 2003 13:56:28   ebeyeler
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
 *    Rev 3.43   04 Nov 2003 09:38:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed errors
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.42   04 Nov 2003 09:26:18   ebeyeler
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
 *    Rev 3.41   29 Oct 2003 15:16:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  389
 * SCR Title:  Present Error Messages for Invalid Hardware using User Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to allow the go on-line functions to return a status.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.40   16 Sep 2003 08:59:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated INILoad and INIClose methods to fix the change in intreface objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.39   26 Aug 2003 12:50:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated information to use model number to configure SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.38   11 Jul 2003 08:38:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added INIOpen, INIReset, and INIClose methods to the SVObserverApp class to handle issue with Coreco software.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.37   11 Jul 2003 08:34:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Removed obsolute MIL handles from SVObserverApp class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.36   08 Jul 2003 11:15:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  371, 322, 320
 * SCR Title:  Remove DOS Focus COM support
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added and removed method associated with SVCommRC change, Digital I/O, and 1394.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.35   22 Apr 2003 11:48:24   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.34   14 Mar 2003 07:05:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  336
 * SCR Title:  Change Light Reference and LUT dialogs on RGB mono system
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added GetSVIMType
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.33   05 Mar 2003 13:55:28   rschock
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
 *    Rev 3.32   26 Feb 2003 08:00:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added Digital SVIM option and Test SVIM
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.31   10 Feb 2003 13:25:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  316
 * SCR Title:  Add option to extent acquisition image source buffers to the length of the PPQ
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added method to get SourceImageDepth
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.30   10 Jan 2003 09:51:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added OnUpdateEditAdjustLut
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.29   18 Dec 2002 11:55:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  300
 * SCR Title:  Create an interface for SVObserver to Push Output data to a SIAC Client
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added member variable SVCommandHelperClass m_cmdControlHelper to be used with Streaming Data when SVObserver goes online.  
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.28   02 Dec 2002 12:21:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  303
 * SCR Title:  Unable to save a configuration to a UNC path
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added an enumeration to handle errors correctly on save.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   20 Nov 2002 09:45:10   ryoho
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
 *    Rev 3.26   04 Apr 2002 10:13:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  286
 * SCR Title:  Upgrade SVObserver to Version 3.37 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code syntax to allow rational rose to reverse engineer properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   09 Nov 2001 13:16:16   rschock
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
 *    Rev 3.24   11 Oct 2001 09:57:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  230
 * SCR Title:  Fix problem when loading a config from MRU list that points to run directory
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new function that validates the existence of the configuration files listed on the MRU list.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   18 Sep 2001 16:54:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  221
 * SCR Title:  Upgrade SVObserver to Version 3.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Change define that wraps the ModelessDebug class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   18 Jul 2001 12:09:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  213
 * SCR Title:  Upgrade Sapera Libraries from 3.00 to 3.11
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added debug object information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   13 Jun 2001 07:32:28   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified per the revisions in svobserver.cpp
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 *    Rev 3.20   08 Mar 2001 12:51:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  193
 * SCR Title:  Fix intermittent problems with the remote go on-line command from SVFocusNT
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added WasSECLoading member variable to SVObserverApp Object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   25 Jan 2001 16:19:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   These changes fixed the SVObserverApp file handling and associated systems.  These systems include the .SEC, .IOD, and .IPD files as well as the camera files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18.1.0.1.1   16 May 2001 14:09:32   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  202
 * SCR Title:  Operator, delete and cut should not be available in File Open/Save Dialogs
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added include to enable CSVFileDialog to be accessible from the entire application.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18.1.0.1.0   10 May 2001 18:24:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  197
 * SCR Title:  Fix lock-ups in application when loading configuration from SVFocusNT
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed SVObserver state code in the header
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18.1.1   19 Apr 2001 20:30:48   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18.1.0   15 Feb 2001 09:46:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  193
 * SCR Title:  Fix intermittent problems with the remote go on-line command from SVFocusNT
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added WasSECLoading member variable to SVObserverApp Object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   24 Oct 2000 15:51:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  187
 * SCR Title:  Color SVIM Problem when Adding Monochrome Tools
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   These changes remove the ability to add a monochrome tool to the tool set until a color tool gets added.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   06 Sep 2000 16:32:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  179
 * SCR Title:  Create Color SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to include color SVIM changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   28 Aug 2000 16:05:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  177
 * SCR Title:  Fix Automatic Load Last SEC Problem
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update FastOCR to prevent memory access violations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   15 Jun 2000 08:46:02   sjones
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
 *    Rev 3.14   19 May 2000 16:03:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  145
 * SCR Title:  Half field images and rotation
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Moved the definition of SV_IMGOP_AVERAGE to 
 * SVImageArithmetic.h
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   11 May 2000 11:19:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  141
 * SCR Title:  View DDE Inputs/Outputs on Results View
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added ON_UPDATE_COMMAND_UI handler for ID_SELECT_PPQ_VARIABLE.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   05 May 2000 14:47:22   sjones
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
 *    Rev 3.11   05 May 2000 14:07:08   sjones
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
 *    Rev 3.10   03 May 2000 10:15:54   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  119
 * SCR Title:  SVFocusNT Support
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Modified to accept remote commands from SVFocusNT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   23 Mar 2000 16:39:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to include new SVVectorObject.h
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   23 Mar 2000 13:24:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  116
 * SCR Title:  Bug Fixes for Freeze on Reject
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added update message handler for Add Acquistion Tool and Resume Freeze menu.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   Mar 01 2000 12:46:28   Nick
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
 *    Rev 3.6   Feb 24 2000 11:51:32   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  111
 * SCR Title:  Integrate Conditional Tool Set/Tool Drawing
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Introduced new SVEnumeratedValueObject module.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   21 Feb 2000 15:16:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  104
 * SCR Title:  New Build Reference Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added OnUpdateAddTransformationTool method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   16 Feb 2000 16:49:42   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  96
 * SCR Title:  Scroll/Zoom images displayed in the inspection document.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Added multi-image display to the IPD
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   07 Feb 2000 21:27:04   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  93
 * SCR Title:  Integrate Multiple Camera Image Input per IPD
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Added another GUID_TO_AFXSTRING Makro which works with CStrings.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   Feb 01 2000 11:12:02   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  90
 * SCR Title:  Save  Operator Tool Move Priviledge in Configuration
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added code to iterate the IP Document list.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   Jan 04 2000 09:20:18   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  82
 * SCR Title:  Configuration Printing
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes to move Configuration printing to a separate class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:21:02   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   18 Nov 1999 12:44:06   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  60
 * SCR Title:  Create 'Utilities' menu option
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Modified initialization function to load the 'Utility' menu.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   Nov 18 1999 09:36:22   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  59
 * SCR Title:  Version 3.00 Release Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   v3.00 Release versioning changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   Nov 09 1999 10:55:58   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Remove dead code from SVResult and associated files.
 * SVAvailableFeatureListClass was removed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   Nov 05 1999 16:35:32   steve
 * Added string resources for Edge Analyzer printout, and added function SVObserverApp::PrintEdgeParameters() to print the detail data.  Updated function SVObserverApp::PrintDetails(), SVObserverApp:OnVirtualPrint(), SVObserverApp:PrintInputOutputList().
 * 
 *    Rev 1.24   Nov 02 1999 13:32:52   steve
 * Changed name of the function PrintInfo() to PrintDetails() to avoid conflict with the CPrintInfo type and various pointers to this type.  Modified print function to include more data,
 * 
 *    Rev 1.23   Oct 29 1999 18:01:14   steve
 * Added functions SVObserverApp::PrintObject, SVObserverApp::PrintInfo(), SVObserverApp::PrintChildren(), SVObserverApp::PrintFriends(), and removed SVObserverApp::PrintTool() to update the printouts,
 * 
 *    Rev 1.22   28 Oct 1999 18:38:00   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  53
 * SCR Title:  Implement Regression Test Capability in SVObserver
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Implemented Regression Test
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   Oct 26 1999 14:57:44   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  52
 * SCR Title:  Version 3.00 Beta 14 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   v3.00 Beta 13 to v3.00 Beta 14.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   21 Oct 1999 10:36:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  22
 * SCR Title:  Port Multiple Position Queue code from program version 2.4.2
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised destructor to be virtual.
 * Removed extraneous comments
 * Removed obsolete code
 * Revised AFX_DATA (SVObserverApp)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   Oct 14 1999 15:11:12   steve
 * Moved definition of new function PrintTool to the "Printing" section.
 * 
 *    Rev 1.17   05 Oct 1999 11:17:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Statistics Tool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   Oct 04 1999 09:40:28   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Check merge and re-checkin.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   Oct 04 1999 09:16:38   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changed OnSaveSecAs() to be sure no message thread is causing problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   Sep 28 1999 16:48:36   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Remove Obsolete Wit setup flag.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   Sep 27 1999 19:27:54   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  35
 * SCR Title:  PPQ mode to wait for input data
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changed default PPQ Length to 3.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   Sep 24 1999 10:00:30   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  4
 * SCR Title:  Add "Load image" option
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Load Image Tool related changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   Sep 21 1999 08:49:42   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  27
 * SCR Title:  Results Picker
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Add 'OnUpdateUI' methods for the Result Picker and the Publish Results picker for controlling user access to the Edit Menu items.  User access control was also added to SVUser.cpp.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   Sep 20 1999 10:02:28   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  35
 * SCR Title:  PPQ mode to wait for input data
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Removed old SV_FAST_IO code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   Sep 13 1999 13:34:56   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Replaced Update message handlger with 'Add Image Subtraction Tool' with 'Add Image Tool' Updatee Handler.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   Sep 10 1999 17:45:24   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Bug fix in Normalize Class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   30 Aug 1999 19:28:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Color assignment for disabled tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   Aug 28 1999 10:36:40   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Updated Wit (OCR)  library related code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   25 Aug 1999 11:43:02   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Upgrading White Pixel Analyzer to new i/o object interface.  
 * 
 * 
 * Changing White Pixel Analyzer to more generic Pixel Analyzer.
 * 
 * 
 * 
 * Changed 
 * 
 * SVWhitePixelAnalyzerClass to SVPixelAnalyzerClass
 * 
 * 
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   Aug 23 1999 14:01:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  22
 * SCR Title:  Port Multiple Position Queue code from program version 2.4.2
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Revised for new PPQ architecture.
 * Revised for RSLinx Compatibility.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
