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
#include "SVOCameraDlg.h"
#include "SVOTriggerDeviceDlg.h"
#include "SVOInspectionSourceDlg.h"
#include "SVOProPosQueDlg.h"
#include "SVOCameraList.h"
#include "SVOInspectionList.h"
#include "TriggerInformation/SVOTriggerList.h"
#include "SVOPPQList.h"
#include "SVConfigurationObject.h"
#include "SVMFCControls\SVOConfigNameEdit.h"
#include "SVSystemLibrary/SVObserverEnums.h"
#include "SVIOBoardCapabilities.h"
#include "SVImportedInspectionInfo.h" 
#include "Definitions/GlobalConstantData.h"
#include "Definitions/StringTypeDef.h"
#include "SVOGeneralConfigDlg.h"
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
	GENERAL_DLG			= 4,
};

enum ConfigMsgTypeEnum
{
	MESSAGE_TYPE_ERROR = 1,
	MESSAGE_TYPE_WARNING = 2,
};


/////////////////////////////////////////////////////////////////////////////
// SVOConfigAssistantDlg dialog

class SVOTriggerList;

class SVOConfigAssistantDlg : public CDialog
{
public:
	typedef std::vector<std::pair<SVIMProductEnum, std::string>> SVProductStringVector;

	SVOConfigAssistantDlg(CWnd* pParent = nullptr);   // standard constructor

	virtual ~SVOConfigAssistantDlg() = default;

	void SetNewConfiguration(bool bNewFlag);
	bool IsNewConfiguration();
	void SetCurrentSystem(long lSysValue = SVIM_PRODUCT_X2_GD4A);
	void SetConfigurationSystem(long lSysValue = SVIM_PRODUCT_X2_GD4A);

	bool AddToCameraList(LPCTSTR CameraName, int Dig, int CameraID);
	bool AddToTriggerList(LPCTSTR TriggerName, int iDig);
	bool AddToInspectionList(LPCTSTR InspectionName, bool NewInspection);
	bool AddToPPQList(LPCTSTR PPQ, LPCTSTR Camera, LPCTSTR Trigger, LPCTSTR Inspection);
	bool AddToPPQList(LPCTSTR PPQ);

	bool RemoveCameraFromList(LPCTSTR CameraName);
	bool RemoveTriggerFromList(LPCTSTR TriggerName);
	bool RemoveInspectionFromList(LPCTSTR Inspeciton);
	bool RemovePPQFromList(LPCTSTR PPQ);

	bool IsDigitizerUsed(LPCTSTR DigString);
	bool IsCameraInList(LPCTSTR CameraName) const;
	bool IsTriggerInList(LPCTSTR TriggerName) const;
	bool IsInspectionInList(LPCTSTR InspectionName) const;
	bool IsOriginalInspectionInList(LPCTSTR InspectionName) const;
	bool IsPPQInList(LPCTSTR PPQName) const;

	std::string BuildDigName( const SVOCameraObj& rCameraObj ) const;
	std::string BuildTrgDig(const SvTi::SVOTriggerObj& rTriggerObj) const;

	bool RenameInspection(LPCTSTR InspectionName, LPCTSTR NewName);

	int GetCameraListCount() const;
	int GetInspectionListCount() const;
	int GetTriggerListCount() const;
	int GetPPQListCount() const;

	SVOCameraObjPtr GetCameraObject(int iPos);
	SVOCameraObjPtr GetCameraObjectByName(LPCTSTR CameraName);
	SvTi::SVOTriggerObjPtr GetTriggerObject(int iPos);
	SvTi::SVOTriggerObjPtr GetTriggerObjectByName(LPCTSTR TriggerName);
	SVOInspectionObjPtr GetInspectionObject(int iPos);
	SVOInspectionObjPtr GetInspectionObjectByName(LPCTSTR Name);
	SVOPPQObjPtr GetPPQObject(int iPos);
	SVOPPQObjPtr GetPPQObjectByName(LPCTSTR PPQName);

	std::string GetNextCameraName();
	int GetNextCameraNumber() const;
	std::string GetNextInspectionName() const;
	std::string GetNextTriggerName(LPCTSTR BaseName) const;
	int GetNextTriggerID() const;
	std::string GetNextPPQName() const;

	SvDef::StringVector m_UsedTriggers;
	SvDef::StringVector m_UsedInspections;

	bool IsTriggerUsed(LPCTSTR TriggerName) const;
	void AddUsedTrigger(LPCTSTR TriggerName);
	void RemoveUsedTrigger(LPCTSTR TriggerName);

	bool IsSoftwareTriggerAllowed(LPCTSTR sTriggerName) const;
	bool IsCameraTriggerAllowed(LPCTSTR sTriggerName) const;
	bool IsCameraLineInputAllowed(LPCTSTR triggerName) const;

	bool IsInspectUsed(LPCTSTR Inspect);
	void AddUsedInspect(LPCTSTR Inspect);
	void RemoveUsedInspect(LPCTSTR Inspect);

	SVIMProductEnum GetProductType() const;
	void SetModified(bool bModified);
	bool Modified();

	bool ItemChanged(int iItemDlg, LPCTSTR LabelName, int iAction);

	std::string GetConfigurationName();

	bool GetNewConfigFlag();

	int GetAllowedNumberOfDigs(bool bTrigger = FALSE);

	void LastInspectionLabelDeleted(LPCTSTR InspectionLabel);
	void SetIOBoardCapabilities(const SVIOBoardCapabilities& rCapable);

	SVImportedInspectionInfoList& GetImportedInspectionInfoList();
	void ClearImportedInspectionInfoList();

	bool IsGigeSystem() const;
	bool IsFileAcquisition(int iDig) const;

	// Dialog Data
	//{{AFX_DATA(SVOConfigAssistantDlg)
	enum { IDD = IDD_DLG_CONFIG_ASSISTANT };
	CButton	m_btnOk;
	CListBox	m_ctlMsgList;
	SvMc::SVOConfigNameEdit	m_ctlConfigurationName;
	CEdit	m_ctlCurrentSys;
	CComboBox	m_ctlAvailableSys;
	CString	m_AvailableSystem;
	CString	m_CurrentSys;
	CString m_ConfigurationName;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVOConfigAssistantDlg)
	protected:

	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPropertySheet m_dlgPropSheet;
	SVOTriggerDeviceDlg    m_TriggerPage;
	SVOCameraDlg           m_CameraPage;
	SVOInspectionSourceDlg m_InspectionPage;
	SVOProPosQueDlg        m_PPQPage;
	SVOGeneralConfigDlg	   m_GeneralPage;

	// Generated message map functions
	//{{AFX_MSG(SVOConfigAssistantDlg)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSelchangeComboAvalSys();
	virtual void OnOK() override;
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
	bool CheckTrigger( const SvTi::SVOTriggerObj& rTriggerObj);
	void RemoveFileAcquisitionMessages( LPCTSTR CameraName );

	HRESULT ConnectToolsetBuffers();
	void resolveGlobalConflicts( SvDef::GlobalConflictPairVector& rGlobalConflicts );

	static std::string GetNameFromProductID(SVIMProductEnum ProductID);
	static SVIMProductEnum GetProductIDFromName(const std::string& rName);

	void ClearMessages();

	//data members
	bool m_bNewConfiguration {false};
	bool m_bModified {false};
	SVIMProductEnum m_lSystemType {SVIMProductEnum::SVIM_PRODUCT_INVALID_TYPE};
	SVIMProductEnum m_lConfigurationType {SVIMProductEnum::SVIM_PRODUCT_INVALID_TYPE};
	std::string m_sConfigName;

	bool m_bCameraGood {false};
	bool m_bTriggerGood {false};
	bool m_bInspectionGood {false};
	bool m_bPPQGood {false};
	bool m_bConfigName {false};
	bool m_bInspectionDeleted {false};

	int m_iLastInspectionNum{0};

	int m_iNextCameraNumber{0};
	SVIOBoardCapabilities m_svCapabilities;

	static const SVProductStringVector m_ProductStringVector;

	SVImportedInspectionInfoList m_ImportedInspectionInfoList;

	void IsGood();

	SVOCameraList m_CameraList;
	SVOInspectionList m_InspectList;
	SvTi::SVOTriggerList m_TriggerList;
	SVOPPQList m_PPQList;

	SVOCameraList m_TmpCameraList; //used for cancel

	bool AddMessageToList(int iDlg, LPCTSTR Message);
	bool RemoveMessageFromList(LPCTSTR Message);
	int RemoveAllObjectMessages(LPCTSTR ObjectName);
	void RemoveAllDlgMessages(int IDlg);
	bool ContainsErrorMessage() const;
	void SetupMessages();

	//private member function...
	void SetupSystemComboBox();
	void ReloadForCurrentSystem();

	void CreateDefaultForSVIMDigital( int Number, LPCTSTR TriggerBaseName );

	void SetCurrentSystemDisplay();
	std::string BuildDisplayMessage( ConfigMsgTypeEnum iErrorFlag, LPCTSTR ObjectName, LPCTSTR Message );

	void CameraDeletedCheckAgainstPPQ(LPCTSTR CameraName);
	void TriggerDeletedCheckAgainstPPQ(LPCTSTR TriggerName);
	void InspectionDeletedCheckAgainstPPQ(LPCTSTR InspectName);

	void CameraDeletedCheckAgainstInspection(LPCTSTR CameraName);
	void CameraDetachedCheckAgainstInspection(LPCTSTR CameraName);

	//methods for writing out to the config object
	//will be called ONLY from OnOk()
	bool SendDataToConfiguration();
	bool SendPPQDataToConfiguration(SVPPQObjectPtrVector& rPPQsToDelete);
	bool SendAcquisitionDataToConfiguration();
	bool SendCameraDataToConfiguration();
	bool SendTriggerDataToConfiguration();
	bool SendInspectionDataToConfiguration();
	bool SendPPQAttachmentsToConfiguration(SVPPQObjectPtrVector& rPPQsToDelete);

	bool GetConfigurationForExisting();

	void CheckAgainstCurrentList();

	bool IsInspectionOnPPQ(LPCTSTR PPQName, LPCTSTR InspectionLabel);
	bool IsCameraOnPPQ(LPCTSTR PPQName, LPCTSTR CameraName);
	bool IsTriggerOnPPQ(LPCTSTR PPQName, LPCTSTR TriggerName);

	SVOPPQObjPtr GetPPQObjectByInspectionName(const std::string& inspectionName);

	void SetupTriggerStrobeMessage();

	bool RenameInspectionObjects(LPCTSTR InspectionName, LPCTSTR NewInspectionName);
};



