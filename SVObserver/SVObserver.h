//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "InitialInformationHandler.h"
#include "SVLibrary/SVUtilityIniClass.h"
#include "SVOLibrary/SVObserverEnums.h"
#include "SVGlobal.h"
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
class InitialInformationHandler;

namespace SvOi
{
	class IObjectWriter;
}

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

	void RemoveUnusedFiles();

	bool Logout( bool BForceLogout = false );
	bool InitPath( LPCTSTR PathName, bool CreateIfDoesNotExist = true, bool DeleteContents = true );

	bool IsMatroxGige() const;

	void OnAppExitWrapper() { CWinApp::OnAppExit(); }
	void PrintSetup() { OnFilePrintSetup(); }

	void UpdateAllIOViews();

	bool CheckSVIMType() const;
	SVIMProductEnum GetSVIMType() const;

	void ValidateMRUList();

	void ResetAllCounts();
	HRESULT SetMode( unsigned long p_lNewMode );

	HRESULT OnObjectRenamed(const std::string& p_rOldName, uint32_t objectId);

	bool AlreadyExistsIPDocTitle( LPCTSTR StrIPDocTitle );

	bool UpdateConfiguration(bool newConfiguration = false);

	bool OkToEdit();
	void UpdateAllMenuExtrasUtilities();

	long GetMaxPPQLength() const;

	long GetLogDataManager() const;
	long UpdateAndGetLogDataManager();

	void DeselectTool();

	HRESULT DisplayCameraManager(SVIMProductEnum eProductType);

	HRESULT SendCameraParameters();

	HRESULT SetModeEdit( bool p_bState );

	void Start(DWORD desiredState);///< In error cases this method throws an exception.
	bool InitialChecks(DWORD desiredState);
	void PrepareForStart(SVConfigurationObject* pConfig);
	HRESULT PrepareCamerasAndMemory();
	void RunInspections(SVConfigurationObject* pConfig, DWORD desiredState);

	void startInstances();
	void stopInstances();

	void EnterRunMode();
	void StopIfRunning();

	CRecentFileList* getRecentFileList() { return  m_pRecentFileList; }

#pragma region Encapsulation Methods
	long getGigePacketSize() const { return m_rInitialInfoSvo.m_gigePacketSize; }
	SVIPDoc* getCurrentDocument() const { return m_pCurrentDocument; }
	void setCurrentDocument(SVIPDoc* pIPDoc) { m_pCurrentDocument = pIPDoc; }
	DWORD getCurrentVersion() const { return m_CurrentVersion; }
	DWORD getLoadingVersion() const { return m_LoadingVersion; }
	void setLoadingVersion(DWORD version) { m_LoadingVersion = version; }
	long getDataValidDelay() const { return m_DataValidDelay; }

	bool IsForcedImageUpdateActive() const { return (m_rInitialInfoSvo.m_forcedImageUpdateTimeInSeconds) ? true : false; }
	unsigned char GetForcedImageUpdateTimeInSeconds() const { return m_rInitialInfoSvo.m_forcedImageUpdateTimeInSeconds; }
	
	long getMaxPreloadFileNumber() const { return m_rInitialInfoSvo.m_MaxPreloadFileNumber; }
	long getPreloadTimeDelay() const { return m_rInitialInfoSvo.m_PreloadTimeDelay; }
	#pragma endregion

#pragma endregion Public Methods

#pragma region Protected Methods
protected:

	HRESULT InitializeSecurity();
	
	bool DestroyMessageWindow();///< Destroys still open message windows. Returns true, if a message window was existent. Otherwise returns false
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	bool InitATL();

	afx_msg void OnUpdateRecentFileMenu(CCmdUI* PCmdUI);

#pragma endregion Private Methods

//@HACK Public Member variables

#pragma region Public member variables
public:
	bool m_MemLeakDetection {false};
	const SvLib::InitialInformation& m_rInitialInfoSvo; ///< This reference exists to simplify access to InitializationStatusFlags()
	typedef std::map<UINT, SvLib::SVUtilityIniClass> UtilityMenuMap;
	UtilityMenuMap m_UtilityMenu;

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

	HMENU m_hAddMenu;
	HANDLE m_hEvent;

	long m_DataValidDelay;

#pragma endregion Member variables
};

SVObserverApp& TheSVObserverApp();

////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// DevStudio inserts additional declarations immediate in front of the preceding line
////////////////////////////////////////////////////////////////////////////////
