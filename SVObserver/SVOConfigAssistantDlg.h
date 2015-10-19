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
#include "SVConfigurationLibrary/SVObserverEnums.h"
#include "SVIOBoardCapabilities.h"
#include "SVIMTypeInfoStruct.h"
#include "SVImportedInspectionInfo.h" 
#include "ObjectInterfaces\DataStructures.h"
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
#define TRIGGER_FIXED_NAME  ( _T("Trigger_") )

public:
	typedef SVBiUniqueMap< SVIMProductEnum, CString >::type SVProductStringMap;

	CSVOConfigAssistantDlg(CWnd* pParent = NULL);   // standard constructor

	virtual ~CSVOConfigAssistantDlg();

	void SetNewConfiguration(BOOL bNewFlag);
	bool IsNewConfiguration();
	void SetCurrentSystem(long lSysValue = SVIM_PRODUCT_FULL);
	void SetConfigurationSystem(long lSysValue = SVIM_PRODUCT_FULL);

	BOOL AddToCameraList(CString sCameraName, int iDig);
	BOOL AddToCameraList(CString sCameraName);
	BOOL AssignCameraFileToCamera(CString sCameraName, CString sCameraFile);
	BOOL AssignDigToCamera(CString sCameraName, int iDig);
	BOOL AddToTriggerList(CString sTriggerName, int iDig);
	BOOL AddToInspectList(CString sExternal, CString sInternal, bool NewInspection);
	BOOL AddToPPQList(CString sPPQ, CString sCamera, CString sTrigger, CString sInspection);
	BOOL AddToPPQList(CString sPPQ);

	BOOL RemoveCameraFromList(CString sCameraName);
	BOOL RemoveTriggerFromList(CString sTriggerName);
	BOOL RemoveInspectionFromList(CString sInspeciton);
	BOOL RemovePPQFromList(CString sPPQ);

	BOOL IsDigitizerUsed(CString sDigString);
	BOOL IsTriggerDigUsed(CString sDigString);
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
	int GetUsedTriggerCount() const;

	bool IsSoftwareTriggerAllowed(LPCTSTR sTriggerName) const;
	bool IsCameraLineInputAllowed(LPCTSTR triggerName) const;

	SVOPPQObjPtr GetPPQForAttachedTrigger(const CString& sTriggerName);

	BOOL IsInspectUsed(CString sInspect);
	void AddUsedInspect(CString sInspect);
	void RemoveUsedInspect(CString sInspect);
	int GetUsedInspectCount();

	SVIMProductEnum GetProductType() const;
	void SetModified(BOOL bModified);
	BOOL Modified();

	BOOL ItemChanged(int iItemDlg, CString sLabelName, int iAction); 

	CString GetConfigurationName();

	BOOL GetNewConfigFlag();

	int GetAllowedNumberOfDigs(BOOL bTrigger = FALSE);

	void LastInspectionLabelDeleted(CString sInspectionLabel);
	BOOL CanInspectionLableBeUsed(CString sLabelName);
	BOOL CanInspectionNameBeUsed(CString sName);
	void AddInspectionLableUsed(CString sLableName);
	void AddInspectionNameUsed(CString sName);
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

	void VerifyList();
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
	void CreateDefaultForRGBSVIM();

	void CreateDefaultForSVIMDigital( int Number, LPCTSTR TriggerBaseName );
	void CreateDefaultForSVIMDigitalHub(int Number);

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



// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVOConfigAssistantDlg.h_v  $
 * 
 *    Rev 1.3   28 Apr 2014 17:35:02   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Cleaned up whitespace.
 * Added parameter to CheckCamera to fix device params bug.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   26 Jul 2013 10:17:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  848
 * SCR Title:  Fix issue with Configuration Assistant not canceling correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added code to put back camera files and camera parameters when a cancel was pressed
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Jun 2013 15:26:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.54   21 May 2013 10:03:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  836
 * SCR Title:  Fix loading SEC bug
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Moved SVObserverEnums.h to SCConfigurationLibrary.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:49:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.53   05 Mar 2013 16:50:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added  GetImportedInspectionInfoList method
 * Added ClearImportedInspectionInfoList method
 * Added logic to support the importing of inspections
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.52   21 Jan 2013 11:37:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added IsCameraLineInoutAllowed method
 * Added IsNOnIOSVIM method
 * Added CreateDefaultForNonIOSVIM method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.51   16 Jan 2013 16:17:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Migrating branch code into primary code base.  
 * 
 * Migrated all hardware tablized data to the hardware manifest object and coverted lokk-up to use the hardware manifest object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.50   10 Jan 2013 16:18:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Updated log entries.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.49   11 Dec 2012 19:04:00   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   (global) Added SYSTEM_SVIM_X2_GD1A.
 *     Added SYSTEM_SVIM_X2_GD1A_COLOR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.48   04 Sep 2012 15:53:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a new utility method to handle the inspection name lookup.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.47.1.0   11 Dec 2012 17:01:20   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   (global) Added SYSTEM_SVIM_X2_GD1A.
 *     Added SYSTEM_SVIM_X2_GD1A_COLOR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.47   20 Aug 2012 08:54:52   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  774
 * SCR Title:  Update SVObserver to include the new SVIM X2B Product
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   SetupMessagesForNewConfig:  Deleted method.  This method did nothing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.46   15 Aug 2012 17:21:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  772
 * SCR Title:  Implement GigE Camera Reconnect Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issue with messages not being updated correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.45   13 Aug 2012 11:39:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  773
 * SCR Title:  Update X2 GigE Model
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed X2B-GD8A model/configuration information from application.  Converted source code to use X2-GD8A instead of X2B-GD8A.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.44   07 Aug 2012 08:59:12   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  774
 * SCR Title:  Update SVObserver to include the new SVIM X2B Product
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added methods:
 *     UpdateAvailableSystems,
 *     GetNameFromProductID,
 *     GetProductIDFromName.
 *   Added types:
 *     SVPRoductStringMap,
 *     SVIMTypeMap.
 *   Added member variables:
 *     m_ProductStringMap,
 *     m_SVIMTypeMap.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.43   09 Jul 2012 13:45:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  774
 * SCR Title:  Update SVObserver to include the new SVIM X2B Product
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added X2_GD4A option for compatibility with this legacy product type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.42   02 Jul 2012 14:33:12   jspila
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
 *    Rev 1.41   02 Jul 2012 13:51:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  773
 * SCR Title:  Update X2 GigE Model
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed configuration type SVIM X2-GD4A.
 * 
 * Added new configuration type SVIM X2-GD8A.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.40   28 Sep 2011 13:11:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to fix issues with camera image interface and methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   12 Aug 2011 14:16:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  736
 * SCR Title:  Add a 4 Camera GigE System to the list of systems
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added 4 Camera GigE System.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   19 Apr 2011 14:03:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added code to rename PPQ Inspection name to new inspection name.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   26 Jan 2011 09:08:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  713
 * SCR Title:  Fix Help button on Configuration Assistant Dialog
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added handler for the Help Button
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   15 Feb 2010 12:37:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to remove the generations of board image processing classes from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   29 Oct 2009 11:27:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix issue with model number tracking.  Updated version to 5.30 Beta 019.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   27 Oct 2009 18:53:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix issues with disconnecting cameras and invalid image behavior.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   17 Jun 2009 08:56:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to remove unused code
 * Added InitializeConfigSystemConversion method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   28 Apr 2009 09:20:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  658
 * SCR Title:  Update SVObserver to handle the new model number info
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed code to correctly handle model numbers with no hardware (Laptop version)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   20 Jan 2009 10:40:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  645
 * SCR Title:  Add support for X2 and X2 Color products
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added support for the X2
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   21 Oct 2008 14:33:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added message for InvalidFileName for FileAcquisition.
 * Added message for InvalidDirectoryName for FileAcquisition.
 * Added message for Empty FileName for FileAcquisition.
 * Added message for Empty DirectoryName for FileAcquisition.
 * Added message for Invalid Image Size for FileAcquisition.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   17 Oct 2008 10:35:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised the IsSoftwareTriggerAllowed method to use name instead of ordinal.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   08 Oct 2008 09:36:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed Hardware Trigger Not Allowed message.
 * Removed IsHardwareTriggerAllowed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   16 Sep 2008 14:50:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added IsFileAcquisitionAllowed method.
 * Added IsHardwareTriggerAllowed method.
 * Added string definition for FileAcquisition Board name
 * Added message definition for FileAcquisition Not Allowed.
 * Added message definition for Hardware Trigger Not Allowed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   10 Sep 2008 17:11:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Converted monolithic method (SendDataToConfiguration) into sub-methods.
 * Revised to support SoftwareTrigger
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   23 Mar 2007 11:35:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  592
 * SCR Title:  Add  X3 product type to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed wording of X3 warning for Jim.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   13 Feb 2007 08:35:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  592
 * SCR Title:  Add  X3 product type to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added X3 Product Type
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   19 Jan 2006 11:17:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to hide Aux Extent property when a gage tool is in the toolset.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   08 Nov 2005 16:06:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to handle new SVIM X1 product.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   02 Nov 2005 08:35:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  520
 * SCR Title:  Fix select camera ROI dialog crash
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed Format 7 ROI crash (load camera files on selection in new configuration)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   01 Aug 2005 14:20:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  469
 * SCR Title:  Add array capability to the Blob Analyzer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   replaced SV2DArrayClass with vector2d
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   19 Jul 2005 14:35:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  475
 * SCR Title:  Increase Hub capabilities
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented support for > 4 camera hubs (1394)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   18 Feb 2005 12:17:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   fixed bug which caused a crash if deleting a camera from a configuration.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   26 Oct 2004 09:12:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  461
 * SCR Title:  Add 1394 Hub capability to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added D1_Hub support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   21 Jan 2004 15:04:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  386
 * SCR Title:  Change Light Reference handling to match earlier versions of SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed problem where if a digital camera supports color it would always write out the light reference as 3 bands even if the configuration is mono.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   17 Dec 2003 09:37:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  396
 * SCR Title:  Add New Product Types for 1394 SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the conversion from a 1394.  It will now delete the camera's and trigger's.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   10 Dec 2003 13:56:00   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  396
 * SCR Title:  Add New Product Types for 1394 SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed RemoveAllObjectMessages to return the number of deleted messages.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   14 Nov 2003 09:40:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  396
 * SCR Title:  Add New Product Types for 1394 SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added product types for 1394 D1-D3 and the Color version
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   12 Nov 2003 13:55:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  396
 * SCR Title:  Add New Product Types for 1394 SVIM
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed to handle new product types
 * added camera hardware checking
 * added permanent warning message for camera file checksum error
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   04 Nov 2003 09:27:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved Connect / Disconnect camera / toolset buffer functions to SVObserverApp
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   03 Nov 2003 16:27:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added code to allow disconnecting and reconnecting buffers in preparation for unplugging cameras.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   30 Sep 2003 07:31:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   RPY - added support for conversion to 1394
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   02 Sep 2003 14:00:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated Trigger Name for the 1394 SVIM.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   07 Aug 2003 12:46:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   made BuildDigName and BuildTrigDig public instead of private
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   08 Jul 2003 12:50:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added 1394 support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   30 Apr 2003 10:35:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  358
 * SCR Title:  Edit Configuration - Rules for Converting configuration
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added methods ClearMessages, SystemChageResetCamera, and IsCompatible.  These methods will be used for making correct conversion from one system to another
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 Apr 2003 12:29:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   14 Mar 2003 07:15:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  336
 * SCR Title:  Change Light Reference and LUT dialogs on RGB mono system
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed m_lSystemType and m_iBoard to enumerations
 * added maps for type conversion
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   10 Feb 2003 13:26:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  316
 * SCR Title:  Add option to extent acquisition image source buffers to the length of the PPQ
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added code for the new PPQ property.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Jan 2003 09:53:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed #define to const CString
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:35:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
