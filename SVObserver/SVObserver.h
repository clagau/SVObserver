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
#include "Triggering/SVDigitizerLoadLibraryClass.h"
#include "Triggering/SVIOTriggerLoadLibraryClass.h"
#include "InitialInformationHandler.h"
#include "SVSecurity/SVSecurityManager.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVFileSystemLibrary/SVFileNameClass.h"
#include "SVGlobal.h"
#include "SVLibrary/SVUtilityIniClass.h"
#include "SVOLibrary/SVObserverEnums.h"
#pragma endregion Includes

#pragma region Declarations
class SVConfigurationObject;
class SVInspectionProcess;
class SVIOController;
class SVIODoc;
class SVIPDoc;
class SVMainFrame;
class SVMessageWindow;
class SVUserObjectListClass;

namespace SvOi
{
	class IObjectWriter;
}

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

	// Special for SVRCComm.DLL
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
	afx_msg BOOL OnOpenRecentFile(UINT NID);

	//}}AFX_VIRTUAL
#pragma endregion AFX_VIRTUAL Methods

	//{{AFX_DATA(SVObserverApp)
	//}}AFX_DATA

public:
	HRESULT OpenFile(LPCTSTR PathName, bool editMode = false, ConfigFileType fileType = ConfigFileType::SvzStandard);
	HRESULT OpenSVXFile();
	HRESULT LoadSvxFile(bool isGlobalInit);

	SVIODoc* NewSVIODoc( LPCTSTR DocName, SVIOController& Controller );
	SVIPDoc* NewSVIPDoc( LPCTSTR DocName, SVInspectionProcess& Inspection );

	HRESULT LoadConfiguration(unsigned long& ulSVOConfigVersion, BSTR bstrFileName, SVTreeType& p_rTree);
	HRESULT LoadPackedConfiguration(LPCTSTR pFileName, ConfigFileType type);
	HRESULT SavePackedConfiguration(LPCTSTR pFileName);

	SVMainFrame* GetMainFrame() const;
	HRESULT CanCloseMainFrame();
	HRESULT DestroyConfig( bool AskForSavingOrClosing = true, bool CloseWithoutHint = false );
	void RemoveUnusedFiles();
	SVIODoc* GetIODoc() const;

	bool Logout( bool BForceLogout = false );
	bool InitPath( LPCTSTR PathName, bool CreateIfDoesNotExist = true, bool DeleteContents = true );

	bool IsMatroxGige() const;

	void OnAppExitWrapper() { CWinApp::OnAppExit(); }
	void OnUpdateRecentFileMenuWrapper(CCmdUI* PCmdUI) { CWinApp::OnUpdateRecentFileMenu(PCmdUI); }
	void PrintSetup() { OnFilePrintSetup(); }

	void UpdateAllIOViews();

	bool CheckSVIMType() const;
	SVIMProductEnum GetSVIMType() const;

	void ValidateMRUList();

	void ResetAllCounts();
	HRESULT SetMode( unsigned long p_lNewMode );

	HRESULT OnObjectRenamed(const std::string& p_rOldName, uint32_t objectId);
	bool SetStatusText( LPCTSTR PStrStatusText );

	const std::string& getConfigPathName() const;
	const std::string& getConfigFileName() const;
	const std::string getConfigFullFileName() const;
	bool setConfigFullFileName(LPCTSTR csFullFileName, bool bLoadFile);

	std::string  getSvxFullFilename() const { return m_SvxFileName.GetFullFileName(); } 

	SVIPDoc* GetIPDoc(uint32_t inspectionID) const;
	SVIPDoc* GetIPDoc( LPCTSTR StrIPDocPathName ) const;
	bool AlreadyExistsIPDocTitle( LPCTSTR StrIPDocTitle );

	bool ShowConfigurationAssistant( int Page = 3, bool bFileNewConfiguration = false );

	bool OkToEdit();
	void UpdateAllMenuExtrasUtilities();

	long GetMaxPPQLength() const;

	long GetLogDataManager() const;
	long UpdateAndGetLogDataManager();

	void DeselectTool();

	HRESULT DisplayCameraManager(SVIMProductEnum eProductType);

	HRESULT DisconnectToolsetBuffers();
	HRESULT ConnectToolsetBuffers();

	HRESULT DisconnectCameras();
	HRESULT ConnectCameras();

	HRESULT SendCameraParameters();

	HRESULT SetModeEdit( bool p_bState );
	HRESULT GetTriggersAndCounts( std::string& rTriggerCounts ) const;

	void RefreshAllIPDocuments();
	void RunAllIPDocuments();
	void SetAllIPDocumentsOnline();
	void SetAllIPDocumentsOffline();
	void ResetAllIPDocModifyFlag(BOOL bModified);

	bool fileSaveAsSVX(const std::string& rFileName, bool resetAutoSave);
	void executePreOrPostExecutionFile(const std::string& filepath, bool inRunMode = true);

	void StopRegression();
	
	void Start(DWORD desiredState);///< In error cases this method throws an exception.

	bool OpenConfigFileFromMostRecentList(int nID);
	void startInstances();
	void stopInstances();

	void SaveConfig(bool saveAs = false);
	void EnterRunMode();
	void StopSvo();
	void StopIfRunning();
	void RCClose();

#pragma region Encapsulation Methods
	long getGigePacketSize() const { return m_rInitialInfo.m_gigePacketSize; }
	long getOfflineCount() const { return m_OfflineCount; }
	SVIPDoc* getCurrentDocument() const { return m_pCurrentDocument; }
	void setCurrentDocument(SVIPDoc* pIPDoc) { m_pCurrentDocument = pIPDoc; }
	DWORD getCurrentVersion() const { return m_CurrentVersion; }
	DWORD getLoadingVersion() const { return m_LoadingVersion; }
	void setLoadingVersion(DWORD version) { m_LoadingVersion = version; }
	long getDataValidDelay() const { return m_DataValidDelay; }

	bool IsForcedImageUpdateActive() const { return (m_rInitialInfo.m_forcedImageUpdateTimeInSeconds) ? true : false; }
	unsigned char GetForcedImageUpdateTimeInSeconds() const { return m_rInitialInfo.m_forcedImageUpdateTimeInSeconds; }
	
	long getMaxPreloadFileNumber() const { return m_rInitialInfo.m_MaxPreloadFileNumber; }
	long getPreloadTimeDelay() const { return m_rInitialInfo.m_PreloadTimeDelay; }
	
#pragma endregion

	bool DetermineConfigurationSaveName(); ///< determines the name under which a configuration is to be changed

#pragma endregion Public Methods

#pragma region Protected Methods
protected:

	HRESULT DisconnectAllCameraBuffers();
	HRESULT ConnectCameraBuffers();
	HRESULT InitializeSecurity();


	HRESULT ConstructDocuments( SVTreeType& p_rTree );

	HRESULT ConstructMissingDocuments();
	
	bool DestroyMessageWindow();///< Destroys still open message windows. Returns true, if a message window was existent. Otherwise returns false
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void StopAll();
	bool InitATL();

	bool AddSecurityNode(HMODULE hMessageDll, long lId, LPCTSTR NTGroup, bool bForcePrompt = false);
	bool AddSecurityNode( HMODULE hMessageDll, long lId );
		
	void StartTrigger(SVConfigurationObject* pConfig);///< can throw an exception!

	afx_msg void OnUpdateRecentFileMenu(CCmdUI* PCmdUI);


#pragma endregion Private Methods

//@HACK Public Member variables

#pragma region Public member variables
public:
	SVSecurityManager m_svSecurityMgr;	// Security

	InitialInformationHandler m_IniInfoHandler;
	bool m_MemLeakDetection {false};
	const SvLib::InitialInformation& m_rInitialInfo; ///< This reference exists to simplify access to InitializationStatusFlags()
	typedef std::map<UINT, SvLib::SVUtilityIniClass> UtilityMenuMap;
	UtilityMenuMap m_UtilityMenu;
private:

	long m_OfflineCount;


#pragma endregion Public Member variables

#pragma region Member variables
private:
	HANDLE m_hAppThread;

	SVIPDoc* m_pCurrentDocument;
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
	long m_LogDataManager;

	SVMessageWindow* m_pMessageWindow;

	bool m_ATLInited;

	mutable SVFileNameClass m_ConfigFileName;
	SVFileNameClass m_SvxFileName;

	HMENU m_hAddMenu;
	HANDLE m_hEvent;

	long m_DataValidDelay;

#pragma endregion Member variables
};

extern SVObserverApp TheSVObserverApp; //@TODO [Arvid][10.20][18.10.2021]: better use a singleton instead


////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// DevStudio inserts additional declarations immediate in front of the preceding line
////////////////////////////////////////////////////////////////////////////////

