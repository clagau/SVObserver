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
#include "TriggerInformation/SVOTriggerList.h"
#include "SVOPPQList.h"
#include "SVConfigurationObject.h"
#include "SVMFCControls\SVOConfigNameEdit.h"
#include "SVSystemLibrary/SVObserverEnums.h"
#include "SVIOBoardCapabilities.h"
#include "SVImportedInspectionInfo.h" 
#include "Definitions/GlobalConstantData.h"
#include "Definitions/StringTypeDef.h"
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
	typedef SVBiUniqueMap< SVIMProductEnum, std::string >::type SVProductStringMap;

	CSVOConfigAssistantDlg(CWnd* pParent = nullptr);   // standard constructor

	virtual ~CSVOConfigAssistantDlg();

	void SetNewConfiguration(BOOL bNewFlag);
	bool IsNewConfiguration();
	void SetCurrentSystem(long lSysValue = SVIM_PRODUCT_X2_GD4A);
	void SetConfigurationSystem(long lSysValue = SVIM_PRODUCT_X2_GD4A);

	BOOL AddToCameraList(LPCTSTR CameraName, int Dig, int CameraID);
	BOOL AddToTriggerList(LPCTSTR TriggerName, int iDig);
	BOOL AddToInspectList(LPCTSTR External, LPCTSTR Internal, bool NewInspection);
	BOOL AddToPPQList(LPCTSTR PPQ, LPCTSTR Camera, LPCTSTR Trigger, LPCTSTR Inspection);
	BOOL AddToPPQList(LPCTSTR PPQ);

	BOOL RemoveCameraFromList(LPCTSTR CameraName);
	BOOL RemoveTriggerFromList(LPCTSTR TriggerName);
	BOOL RemoveInspectionFromList(LPCTSTR Inspeciton);
	BOOL RemovePPQFromList(LPCTSTR PPQ);

	BOOL IsDigitizerUsed(LPCTSTR DigString);
	BOOL IsCameraInList(LPCTSTR CameraName) const;
	bool IsTriggerInList(LPCTSTR TriggerName) const;
	BOOL IsInspectionInList(LPCTSTR InspectionName) const;
	BOOL IsInspectionNameInList(LPCTSTR InspectionName) const;
	BOOL IsPPQInList(LPCTSTR PPQName) const;

	std::string BuildDigName( const SVOCameraObj& rCameraObj ) const;
	std::string BuildTrgDig(const SvTi::SVOTriggerObj& rTriggerObj) const;

	std::string GetInspectionLabelFromName(LPCTSTR InspectName);
	std::string GetInspectionNameFromLabel(LPCTSTR InspectLabel);
	BOOL RenameInspection(LPCTSTR InspectLabel, LPCTSTR NewName);

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
	std::string GetNextInspectionDisplayName() const;
	std::string GetNextTriggerName(LPCTSTR BaseName) const;
	int GetNextTriggerID() const;
	std::string GetNextPPQName() const;

	SvDef::StringVector m_UsedTriggers;
	SvDef::StringVector m_UsedInspections;
	SvDef::StringVector m_InspectionNamesUsed;
	SvDef::StringVector m_InspectionLabelsUsed;

	BOOL IsTriggerUsed(LPCTSTR TriggerName) const;
	void AddUsedTrigger(LPCTSTR TriggerName);
	void RemoveUsedTrigger(LPCTSTR TriggerName);

	bool IsSoftwareTriggerAllowed(LPCTSTR sTriggerName) const;
	bool IsCameraLineInputAllowed(LPCTSTR triggerName) const;

	BOOL IsInspectUsed(LPCTSTR Inspect);
	void AddUsedInspect(LPCTSTR Inspect);
	void RemoveUsedInspect(LPCTSTR Inspect);

	SVIMProductEnum GetProductType() const;
	void SetModified(BOOL bModified);
	BOOL Modified();

	BOOL ItemChanged(int iItemDlg, LPCTSTR LabelName, int iAction); 

	std::string GetConfigurationName();

	BOOL GetNewConfigFlag();

	int GetAllowedNumberOfDigs(BOOL bTrigger = FALSE);

	void LastInspectionLabelDeleted(LPCTSTR InspectionLabel);
	BOOL CanInspectionNameBeUsed(LPCTSTR Name);
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
	CString	m_AvailableSystem;
	CString	m_CurrentSys;
	CString m_ConfigurationName;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVOConfigAssistantDlg)
	protected:

	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
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
	BOOL CheckTrigger( const SvTi::SVOTriggerObj& rTriggerObj);
	void RemoveFileAcquisitionMessages( LPCTSTR CameraName );

	HRESULT ConnectToolsetBuffers();
	void resolveGlobalConflicts( SvDef::GlobalConflictPairVector& rGlobalConflicts );

	static std::string GetNameFromProductID( SVIMProductEnum p_ID );
	static SVIMProductEnum GetProductIDFromName( const std::string& rName );

	void ClearMessages();
	//data members
	BOOL m_bNewConfiguration;
	BOOL m_bModified;
	SVIMProductEnum m_lSystemType;
	SVIMProductEnum m_lConfigurationType;
	std::string m_sConfigName;

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
	SvTi::SVOTriggerList m_TriggerList;
	SVOPPQList m_PPQList;

	SVOCameraList m_TmpCameraList; //used for cancel

	BOOL AddMessageToList(int iDlg, LPCTSTR Message);
	BOOL RemoveMessageFromList(LPCTSTR Message);
	int RemoveAllObjectMessages(LPCTSTR ObjectName);
	void RemoveAllDlgMessages(int IDlg);
	bool ContainsErrorMessage() const;
	void SetupMessages();

	//private member function...
	void SetupSystemComboBox();
	void ReloadForCurrentSystem();
	void CreateDefaultForSVIM(int Number);

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
	BOOL SendDataToConfiguration();
	BOOL SendPPQDataToConfiguration(SVPPQObjectPtrVector& rPPQsToDelete);
	BOOL SendAcquisitionDataToConfiguration();
	BOOL SendCameraDataToConfiguration();
	BOOL SendTriggerDataToConfiguration();
	BOOL SendInspectionDataToConfiguration();
	BOOL SendPPQAttachmentsToConfiguration(SVPPQObjectPtrVector& rPPQsToDelete);

	BOOL GetConfigurationForExisting();

	void CheckAgainstCurrentList();

	BOOL IsInspectionOnPPQ(LPCTSTR PPQName, LPCTSTR InspectionLabel);
	BOOL IsCameraOnPPQ(LPCTSTR PPQName, LPCTSTR CameraName);
	BOOL IsTriggerOnPPQ(LPCTSTR PPQName, LPCTSTR TriggerName);

	SVOPPQObjPtr GetPPQObjectByInspectionName(const std::string& inspectionName);

	void ConvertToDigital(SVIMProductEnum productType);

	void SetupTriggerStrobeMessage();

	BOOL RenameInspectionObjects(LPCTSTR InspectionName, LPCTSTR NewInspectionName);
};



