// ******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOConfigAssistantDlg
// * .File Name       : $Workfile:   SVOConfigAssistantDlg.h  $
// * ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   28 Apr 2014 17:35:02  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVContainerLibrary/SVBiUniqueMap.h"
#include "SVOCameraDlg.h"
#include "SVOTriggerDeviceDlg.h"
#include "SVOInspectionSourceDlg.h"
#include "SVOProPosQueDlg.h"
#include "SVOCameraList.h"
#include "SVOInspectionList.h"
#include "SVOTriggerList.h"
#include "SVOPPQList.h"
#include "SVConfigurationObject.h"
#include "SVMFCControls\SVOConfigNameEdit.h"
#include "SVSystemLibrary/SVObserverEnums.h"
#include "SVIOBoardCapabilities.h"
#include "TriggerHandling/SVIMTypeInfoStruct.h"
#include "SVImportedInspectionInfo.h" 
#include "ObjectInterfaces/GlobalConstantData.h"
#pragma endregion Includes

// Moved SVIM_FULL_BOARD, etc. to SVObserverEnums.h

enum ConfigItemTypeEnum
{
    ITEM_ACTION_NEW         = 1,
    ITEM_ACTION_DELETE      = 2,
    ITEM_ACTION_PROP        = 3,
    ITEM_PPQ_ADD_TRG        = 4,
    ITEM_PPQ_DEL_TRG        = 5,
    ITEM_PPQ_ADD_CAM        = 6,
    ITEM_PPQ_DEL_CAM        = 7,
    ITEM_PPQ_ADD_INS        = 8,
    ITEM_PPQ_DEL_INS        = 9,
    ITEM_PPQ_DETACH_INS     = 10,
    ITEM_PPQ_ATTACH_INS     = 11,
    ITEM_PPQ_PROP_SRC_IMG   = 12,
	ITEM_ACTION_REFRESH     = 13,
    ITEM_ACTION_SAVE		= 14
};

enum ConfigTypeEnum
{
    TRIGGER_DLG         = 0,
    CAMERA_DLG          = 1,
    INSPECT_DLG         = 2,
    PPQ_DLG             = 3,
};

enum ConfigMsgTypeEnum
{
	MESSAGE_TYPE_ERROR = 1,
	MESSAGE_TYPE_WARNING = 2,
};


/////////////////////////////////////////////////////////////////////////////
// CSVOConfigAssistantDlg dialog

class SVOTriggerList;

class CSVOConfigAssistantDlg : public CDialog
{
public:
	typedef SVBiUniqueMap< SVIMProductEnum, CString >::type SVProductStringMap;

	CSVOConfigAssistantDlg(CWnd* pParent = nullptr);   // standard constructor

	virtual ~CSVOConfigAssistantDlg();

	void SetNewConfiguration(BOOL bNewFlag);
	bool IsNewConfiguration();
	void SetCurrentSystem(long lSysValue = SVIM_PRODUCT_X2_GD4A);
	void SetConfigurationSystem(long lSysValue = SVIM_PRODUCT_X2_GD4A);

	BOOL AddToCameraList(CString sCameraName, int iDig);
	BOOL AddToCameraList(CString sCameraName);
	BOOL AddToTriggerList(CString sTriggerName, int iDig);
	BOOL AddToInspectList(CString sExternal, CString sInternal, bool NewInspection);
	BOOL AddToPPQList(CString sPPQ, CString sCamera, CString sTrigger, CString sInspection);
	BOOL AddToPPQList(CString sPPQ);

	BOOL RemoveCameraFromList(CString sCameraName);
	BOOL RemoveTriggerFromList(CString sTriggerName);
	BOOL RemoveInspectionFromList(CString sInspeciton);
	BOOL RemovePPQFromList(CString sPPQ);

	BOOL IsDigitizerUsed(CString sDigString);
	BOOL IsCameraInList(CString sCameraName) const;
	bool IsTriggerInList(CString sTriggerName) const;
	BOOL IsInspectionInList(CString sInspectionName) const;
	BOOL IsInspectionNameInList(CString sInspectionName) const;
	BOOL IsPPQInList(CString sPPQName) const;

	CString BuildDigName(const SVOCameraObj& rCameraObj, bool bIsAcqDev = false) const;
	CString BuildTrgDig(const SVOTriggerObj& rTriggerObj) const;

	CString GetInspectionLabelFromName(CString sInspectName);
	CString GetInspectionNameFromLabel(CString sInspectLabel);
	BOOL RenameInspection(CString sInspectLabel, CString sNewName);

	int GetCameraListCount() const;
	int GetInspectionListCount() const;
	int GetTriggerListCount() const;
	int GetPPQListCount() const;

	SVOCameraObjPtr GetCameraObject(int iPos);
	SVOCameraObjPtr GetCameraObjectByName(CString sCameraName);
	SVOTriggerObjPtr GetTriggerObject(int iPos);
	SVOTriggerObjPtr GetTriggerObjectByName(CString sTriggerName);
	SVOInspectionObjPtr GetInspectionObject(int iPos);
	SVOInspectionObjPtr GetInspectionObjectByName(CString sName);
	SVOPPQObjPtr GetPPQObject(int iPos);
	SVOPPQObjPtr GetPPQObjectByName(CString sPPQName);

	CString GetNextCameraName();
	int GetNextCameraNumber() const;
	CString GetNextInspectionName() const;
	CString GetNextInspectionDisplayName() const;
	CString GetNextTriggerName(const CString& baseName) const;
	int GetNextTriggerID() const;
	CString GetNextPPQName() const;

	CStringList m_slUsedTriggers;
	CStringList m_slUsedInspect;
	CStringList m_slInspectionNamesUsed;
	CStringList m_slInspectionLabelsUsed;

	BOOL IsTriggerUsed(CString sTriggerName) const;
	void AddUsedTrigger(CString sTriggerName);
	void RemoveUsedTrigger(CString sTriggerName);

	bool IsSoftwareTriggerAllowed(LPCTSTR sTriggerName) const;
	bool IsCameraLineInputAllowed(LPCTSTR triggerName) const;

	BOOL IsInspectUsed(CString sInspect);
	void AddUsedInspect(CString sInspect);
	void RemoveUsedInspect(CString sInspect);

	SVIMProductEnum GetProductType() const;
	void SetModified(BOOL bModified);
	BOOL Modified();

	BOOL ItemChanged(int iItemDlg, CString sLabelName, int iAction); 

	CString GetConfigurationName();

	BOOL GetNewConfigFlag();

	int GetAllowedNumberOfDigs(BOOL bTrigger = FALSE);

	void LastInspectionLabelDeleted(CString sInspectionLabel);
	BOOL CanInspectionNameBeUsed(CString sName);
	void SetIOBoardCapabilities( SVIOBoardCapabilities& p_svCapable );

	bool IsNonIOSVIM(SVIMProductEnum productType) const;

	SVImportedInspectionInfoList& GetImportedInspectionInfoList();
	void ClearImportedInspectionInfoList();

	bool IsDigitalSystem() const;
	bool IsGigeSystem() const;
	bool IsValidCamera(int iDig) const;
	bool IsFileAcquisition(int iDig) const;

	// Dialog Data
	//{{AFX_DATA(CSVOConfigAssistantDlg)
	enum { IDD = IDD_DLG_CONFIG_ASSISTANT };
	CButton	m_btnOk;
	CListBox	m_ctlMsgList;
	SvMc::CSVOConfigNameEdit	m_ctlConfigurationName;
	CEdit	m_ctlCurrentSys;
	CComboBox	m_ctlAvailableSys;
	CString	m_sAvailableSystem;
	CString	m_sCurrentSys;
	CString	m_sConfigurationName;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVOConfigAssistantDlg)
	protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPropertySheet m_dlgPropSheet;
	CSVOTriggerDeviceDlg    m_Page1;
	CSVOCameraDlg           m_Page2;
	CSVOInspectionSourceDlg m_Page3;
	CSVOProPosQueDlg        m_Page4;

	// Generated message map functions
	//{{AFX_MSG(CSVOConfigAssistantDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboAvalSys();
	virtual void OnOK();
	afx_msg void OnDblclkLstErrList();
	afx_msg void OnChangeEditConfigurationName();
	afx_msg void OnHelpButton();
	afx_msg void OnBnClickedCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void UpdateAvailableSystems( SVIMProductEnum p_CurrentConfigurationType, SVIMProductEnum p_SelectedConfigurationType );
	HRESULT CheckCamera( SVOCameraObj& rCameraObj, bool SetFileParameters = false );
	void CheckColor( const SVOCameraObj& rCameraObj );
	void CheckTriggers();
	BOOL CheckTrigger( const SVOTriggerObj& rTriggerObj);
	void RemoveFileAcquisitionMessages( const CString& rCameraName );

	HRESULT ConnectToolsetBuffers();
	void resolveGlobalConflicts( SvOi::GlobalConflictPairVector& rGlobalConflicts );

	static CString GetNameFromProductID( SVIMProductEnum p_ID );
	static SVIMProductEnum GetProductIDFromName( const CString& p_Name );

	void ClearMessages();
	//data members
	BOOL m_bNewConfiguration;
	BOOL m_bModified;
	SVIMProductEnum m_lSystemType;
	SVIMProductEnum m_lConfigurationType;
	CString m_sConfigName;

	BOOL m_bCameraGood;
	BOOL m_bTriggerGood;
	BOOL m_bInspectionGood;
	BOOL m_bPPQGood;
	BOOL m_bConfigName;
	BOOL m_bInspectionDeleted;

	int m_iLastInspectionNum;

	int m_iNextCameraNumber;
	SVIOBoardCapabilities m_svCapabilities;

	static const SVProductStringMap m_ProductStringMap;

	SVImportedInspectionInfoList m_ImportedInspectionInfoList;

	void IsGood();

	BOOL SystemChangeResetCamera( SVIMProductEnum p_lNewSystemType );

	SVOCameraList m_CameraList;
	SVOInspectionList m_InspectList;
	SVOTriggerList m_TriggerList;
	SVOPPQList m_PPQList;

	SVOCameraList m_TmpCameraList; //used for cancel

	BOOL AddMessageToList(int iDlg, CString sMessage);
	BOOL RemoveMessageFromList(CString sMessage);
	int RemoveAllObjectMessages(CString sObjectName);
	void RemoveAllDlgMessages(int IDlg);
	bool ContainsErrorMessage() const;
	void SetupMessages();

	//private member function...
	void SetupSystemComboBox();
	void ReloadForCurrentSystem();
	void CreateDefaultForSVIM(int Number);

	void CreateDefaultForSVIMDigital( int Number, LPCTSTR TriggerBaseName );

	void SetCurrentSystemDisplay();
	CString BuildDisplayMessage(ConfigMsgTypeEnum iErrorFlag, CString sObjectName, CString sMessage);

	void CameraDeletedCheckAgainstPPQ(CString sCameraName);
	void TriggerDeletedCheckAgainstPPQ(CString sTriggerName);
	void InspectionDeletedCheckAgainstPPQ(CString sInspectName);

	void CameraDeletedCheckAgainstInspection(CString sCameraName);
	void CameraDetachedCheckAgainstInspection(CString sCameraName);

	//methods for writing out to the config object
	//will be called ONLY from OnOk()
	BOOL SendDataToConfiguration();
	BOOL SendPPQDataToConfiguration(SVPPQObjectArray& aPPQsToDelete);
	BOOL SendAcquisitionDataToConfiguration();
	BOOL SendCameraDataToConfiguration();
	BOOL SendTriggerDataToConfiguration();
	BOOL SendInspectionDataToConfiguration();
	BOOL SendPPQAttachmentsToConfiguration(SVPPQObjectArray& aPPQsToDelete);

	BOOL GetConfigurationForExisting();

	void CheckAgainstCurrentList();

	BOOL IsInspectionOnPPQ(CString sPPQName, CString sInspectionLabel);
	BOOL IsCameraOnPPQ(CString sPPQName, CString sCameraName);
	BOOL IsTriggerOnPPQ(CString sPPQName, CString sTriggerName);

	SVOPPQObjPtr GetPPQObjectByInspectionName(const SVString& inspectionName);

	void ConvertToDigital(SVIMProductEnum productType);

	void SetupTriggerStrobeMessage();

	BOOL RenameInspectionObjects(CString sInspectionName, CString sNewInspectionName);
};



