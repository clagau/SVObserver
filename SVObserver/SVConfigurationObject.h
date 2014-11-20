//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConfigurationObject
//* .File Name       : $Workfile:   SVConfigurationObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.15  $
//* .Check In Date   : $Date:   20 Nov 2014 05:01:58  $
//******************************************************************************

#ifndef INC_SVCONFIGURATIONOBJECT_INCLUDED
#define INC_SVCONFIGURATIONOBJECT_INCLUDED

#pragma region Includes
#include "SVContainerLibrary/SVMap.h"
#include "SVImageLibrary/SVLightReference.h"
#include "SVImageLibrary/SVLut.h"
#include "SVMaterialsLibrary/SVMaterialsTree.h"
#include "SVObjectLibrary/SVObserverTemplate.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVSharedMemoryLibrary/SVProductFilterEnum.h"

#include "SVFileNameArrayClass.h"
#include "SVInfoStructs.h"
#include "SVInspectionProcess.h"
#include "SVConfigurationLibrary/SVObserverEnums.h"
#include "SVPPQObject.h"
#include "SVTriggerObject.h"
#include "SVStorage.h"
#include "SVStorageResult.h"
#include "RemoteMonitorList.h"
#pragma endregion Includes

#pragma region Declarations
class SVDeviceParamCollection;
class SVDeviceParam;
class SVIOController;
class SVNavigateTreeClass;
class SVPLCDataController;
class SVRemoteOutputGroup;
class SVRemoteOutputObject;
class SVCameraTriggerClass;
class SVSoftwareTriggerClass;
class SVPPQObject;

struct SVConfigurationAcquisitionDeviceInfoStruct;
typedef std::vector<SVInspectionProcess *> SVInspectionProcessPtrList;
#pragma endregion Declarations

class SVConfigurationObject : public SVObjectClass,	public SVObserverTemplate< SVRenameObject >
{
	SV_DECLARE_CLASS( SVConfigurationObject );

public:
	typedef SVMap< CString, SVConfigurationAcquisitionDeviceInfoStruct* > SVAcquisitionDeviceMap;

	SVConfigurationObject( LPCSTR ObjectName );
	SVConfigurationObject( SVObjectClass *pOwner = NULL, int StringResourceID = IDS_CLASSNAME_SVCONFIGURATIONOBJECT );
	virtual ~SVConfigurationObject();

	virtual HRESULT GetChildObject( SVObjectClass*& p_rpObject, const SVObjectNameInfo& p_rNameInfo, long p_Index = 0 ) const;

	virtual HRESULT ObserverUpdate( const SVRenameObject& p_rData );

	typedef SVXMLMaterialsTree SVTreeType;

	unsigned long GetSVXFileVersion(SVTreeType& rTree);

	BOOL SaveConfiguration(SVTreeType& rTree);

	HRESULT LoadConfiguration(SVTreeType& rTree);

	HRESULT SetMode( unsigned long p_Mode );

	HRESULT GetInspectionItems( const SVNameSet& p_rNames, SVNameStorageResultMap& p_rItems ) const;
	HRESULT GetRemoteInputItems( const SVNameSet& p_rNames, SVNameStorageResultMap& p_rItems ) const;

	HRESULT SetInspectionItems( const SVNameStorageMap& p_rItems, SVNameStatusMap& p_rStatus );
	HRESULT SetRemoteInputItems( const SVNameStorageMap& p_rItems, SVNameStatusMap& p_rStatus );
	HRESULT SetCameraItems( const SVNameStorageMap& p_rItems, SVNameStatusMap& p_rStatus );

	BOOL Create();
	BOOL Destroy();

	BOOL RebuildInputOutputLists();
	BOOL Activate();

	BOOL DestroyConfiguration();

	BOOL SetInputObjectList( SVInputObjectList *pInputObjectList );
	BOOL GetInputObjectList( SVInputObjectList **ppInputObjectList ) const;

	BOOL SetOutputObjectList( SVOutputObjectList *pOutputObjectList );
	BOOL GetOutputObjectList( SVOutputObjectList **ppOutputObjectList ) const;
	HRESULT RebuildOutputObjectList();

	BOOL AddAcquisitionDevice( LPCTSTR szName, SVFileNameArrayClass& rsvFiles,
                               SVLightReference& rsvLight,
							   SVLut& rLut,
							   const SVDeviceParamCollection* rpDeviceParams );
	BOOL ModifyAcquisitionDevice( LPCTSTR szName, SVLightReference& rsvLight );
	BOOL ModifyAcquisitionDevice( LPCTSTR szName, const SVLut& lut );
	BOOL ModifyAcquisitionDevice( LPCTSTR szName, const SVDeviceParamCollection* pParams );

	BOOL RemoveAcquisitionDevice( LPCTSTR szName );
	BOOL GetAcquisitionDevice( LPCTSTR szName, 
	                           SVFileNameArrayClass*& pFiles,
                               SVLightReference*& pLight,
							   SVLut*& rpLut,
							   SVDeviceParamCollection*& rpDeviceParams ) const;
	SVAcquisitionDeviceMap::iterator GetAcquisitionDeviceStartPosition() const;
	SVAcquisitionDeviceMap::iterator GetAcquisitionDeviceEndPosition();
	void GetAcquisitionDeviceNextAssoc( SVAcquisitionDeviceMap::iterator& rNextPosition, CString& rKey );
	void GetAcquisitionDeviceNextAssoc( SVAcquisitionDeviceMap::iterator& rNextPosition, CString& rKey, 
	                                    SVFileNameArrayClass*& pFiles,
                                        SVLightReference*& pLight,
										SVLut*& rpLut,
										SVDeviceParamCollection*& rpDeviceParams );

	BOOL AddTrigger( SVTriggerObject *pTrigger );
	BOOL RemoveTrigger( SVTriggerObject *pTrigger );
	BOOL GetTriggerCount( long &lIndex ) const;
	BOOL GetTrigger( long lIndex, SVTriggerObject **ppTrigger ) const;
	BOOL GetChildObjectByName( LPCTSTR tszName, SVTriggerObject **ppTrigger ) const;

	BOOL AddPPQ( SVPPQObject *pPPQ );
	BOOL RemovePPQ( SVPPQObject *pPPQ );
	BOOL GetPPQCount( long &lIndex ) const;
	BOOL GetPPQ( long lIndex, SVPPQObject **ppPPQ ) const;
	BOOL GetChildObjectByName( LPCTSTR tszName, SVPPQObject **ppPPQ ) const;
	bool GetPPQByName( LPCTSTR name, SVPPQObject **ppPPQ ) const;

	BOOL AddCamera( SVVirtualCamera *pCamera );
	BOOL RemoveCamera( SVVirtualCamera *pCamera );
	BOOL GetCameraCount( long &lIndex ) const;
	BOOL GetCamera( long lIndex, SVVirtualCamera **ppCamera ) const;
	BOOL GetChildObjectByName( LPCTSTR tszName, SVVirtualCamera **ppCamera ) const;

	BOOL AddInspection( SVInspectionProcess *pInspection );
	BOOL RemoveInspection( SVInspectionProcess *pInspection );
	BOOL GetInspectionCount( long &lIndex ) const;
	BOOL GetInspection( long lIndex, SVInspectionProcess **ppInspection ) const;
	BOOL GetInspections( std::vector<SVInspectionProcess*>& rvecInspections ) const;
	BOOL GetChildObjectByName( LPCTSTR tszName, SVInspectionProcess **ppInspection ) const;
	BOOL GetInspectionObject( LPCTSTR tszFullyQualifiedNameOfChild, SVInspectionProcess **ppInspection ) const;

	HRESULT AttachAcqToTriggers();
	HRESULT ValidateOutputList();

	SVIMProductEnum GetProductType() const;
	void SetProductType( SVIMProductEnum eProductType );
	bool IsConfigurationLoaded() const;
	void SetConfigurationLoaded();
	bool RenameOutputListInspectionNames(CString& NewInspectionName, CString& OldInspectionName);
	
	unsigned long GetFileVersion() const;

	HRESULT SetModuleReady( bool p_Value );
	HRESULT SetRaidErrorBit( bool p_Value );

	SVIOEntryHostStructPtr GetModuleReady();
	SVIOEntryHostStructPtr GetRaidErrorBit();

	SVIOController* GetIOController() const;
	SVGUID GetIOControllerID() const;
#ifndef _WIN64
	SVPLCDataController* GetPLCData() const;
	size_t GetPLCCount() const;
	void SetupPLC();
	HRESULT GetPLCs( std::vector<CString>& p_astrPLCIds ) const;
	LPCSTR GetConnectString() const;
	long GetQueueSize( const CString& p_strPLC ) const;
	HRESULT GetHeartBeatAddress( CString& p_strHeartBeatAddress ) const;
	HRESULT GetHeartBeatTime( long& p_lTime ) const;
	HRESULT GetPLCControlData( SVMaterials& p_rMaterials, const CString& p_strPLC ) const;
	HRESULT SetPLCControlData( SVMaterials& p_rMaterials, const CString& p_strPLC );
	CString AssociatePPQToPLC( const CString& p_strPPQ );
	HRESULT WriteOutputs( const CString& p_strPLCName, SVProductInfoStruct *pProduct);
#endif
	SVGUID GetRemoteOutputController() const;
	size_t GetRemoteOutputGroupCount() const;
	void SetupRemoteOutput();
	HRESULT ClearRemoteOutputUnUsedData();
	HRESULT GetRemoteOutputGroupNames( std::vector<CString>& p_astrPPQs ) const;
	SVRemoteOutputGroup* GetRemoteOutputGroup( const CString& p_strRemoteGroupID ) const;
	size_t GetRemoteOutputGroupItemCount( const CString& p_strRemoteGroupID ) const;
	HRESULT GetRemoteOutputItem( const CString& p_strRemoteGroupId, long l_lIndex, SVRemoteOutputObject*& p_rItem ) const;
	SVRemoteOutputObject* GetFirstRemoteOutputObject( const CString& p_strRemoteGroupId ) const;
	HRESULT AddRemoteOutputItem( const CString& p_strRemoteGroupId, SVRemoteOutputObject*& p_pNewOutput, GUID p_InputObjectID, const CString p_strPPQ );
	HRESULT DeleteRemoteOutput( const CString& p_strRemoteGroupId );
	HRESULT DeleteRemoteOutputEntry( const CString& p_strRemoteGroupId, SVRemoteOutputObject* p_pOutputObject);
	HRESULT RemoteOutputValidateInputs();

	HRESULT AddImportedRemoteInput(SVPPQObject* p_pPPQ, const SVString& name, long ppqPosition, long index, const _variant_t& p_Value);
	HRESULT AddImportedDigitalInput(SVPPQObject* p_pPPQ, const SVString& name, long ppqPosition);
	HRESULT AddRemoteInput(SVPPQObject* p_pPPQ, const SVString& name, long ppqPosition, long index, const _variant_t& p_Value);
	HRESULT AddDigitalInput(SVPPQObject* p_pPPQ, const SVString& name, long ppqPosition);
	HRESULT AddCameraDataInput(SVPPQObject* pPPQ, SVIOEntryHostStructPtr pIOEntry);

	bool SetupRemoteMonitorList();
	void ClearRemoteMonitorList();
	RemoteMonitorList GetRemoteMonitorList() const;
	void SetRemoteMonitorList(const RemoteMonitorList& rList);

	//************************************
	// Method:    ReplaceOrAddMonitorList
	// Description:  Replace a monitor list if it already exists or add a new monitor list to the configuration.
	// Parameter: const RemoteMonitorNamedList & rList
	// Returns:   void
	//************************************
	void ReplaceOrAddMonitorList(const RemoteMonitorNamedList& rList);
	void ValidateRemoteMonitorList();
	HRESULT ActivateRemoteMonitorList(const SVString& listName, bool bActivate);
	void GetActiveRemoteMonitorList(RemoteMonitorList& rActiveList) const;
	HRESULT GetRemoteMonitorListProductFilter(const SVString& listName, SVProductFilterEnum& rFilter) const;
	HRESULT SetRemoteMonitorListProductFilter(const SVString& listName, SVProductFilterEnum filter);
	void BuildPPQMonitorList(PPQMonitorList& ppqMonitorList) const;

	//************************************
	// Method:    LoadRemoteMonitorList
	// Description: Loads the Remote Monitor List(s) from the configuration file
	// Parameter: SVTreeType & rTree the XML-tree with the config.
	// Returns:   HRESULT S_OK, if loading successful.
	//************************************
	HRESULT LoadRemoteMonitorList( SVTreeType& rTree );

	bool HasCameraTrigger(SVPPQObject* p_pPPQ) const;

protected:
	BOOL FinishIPDoc( SVInspectionProcess* pIP );

	SVIOController* m_pIOController;

	SVInputObjectList* m_pInputObjectList;
	SVOutputObjectList* m_pOutputObjectList;

private:
	typedef SVVector< SVInspectionProcess* > SVInspectionProcessVector;
	typedef std::set< SVInspectionProcess* > SVInspectionSet;

	BOOL SaveEnvironment(SVTreeType &rTree);
	BOOL SaveIO(SVTreeType &rTree);
	BOOL SaveAcquisitionDevice(SVTreeType &rTree);
	BOOL SaveCamera(SVTreeType &rTree);
	BOOL SaveTrigger(SVTreeType &rTree);
	BOOL SaveInspection(SVTreeType &rTree);
	BOOL SavePPQ(SVTreeType &rTree);
	//************************************
	// Method:    SaveMonitorList
	// Description: Add the current monitor lists to the tree.
	// Parameter: SVTreeType & rTree XML-tree which will be saved.
	// Returns:   bool
	//************************************
	bool SaveRemoteMonitorList( SVTreeType& rTree ) const;
	//************************************
	// Method:    SaveMonitoredObjectList
	// Description: Add the current sub monitor lists to the tree
	// Parameter: SVTreeType & rTree XML-tree which will be saved.
	// Parameter: SVTreeType::SVBranchHandle hBranch Branch of the tree where the sub list should added.
	// Parameter: const SVString& listName Name of the sublist.
	// Parameter: const MonitoredObjectList & subList The sublist which should saved.
	// Returns:   bool
	//************************************
	bool SaveMonitoredObjectList( SVTreeType& rTree, SVTreeType::SVBranchHandle hBranch, const SVString& listName, const MonitoredObjectList& subList ) const;

	HRESULT SaveAcquisitionDeviceFilename( SVTreeType& rTree, SVTreeType::SVBranchHandle htiDig, SVFileNameArrayClass* pFiles );
	HRESULT LoadAcquisitionDeviceFilename( SVTreeType& rTree, SVTreeType::SVBranchHandle htiDig, SVFileNameArrayClass& svFileArray );

	HRESULT SaveFileAcquisitionConfiguration( SVTreeType& rTree, SVTreeType::SVBranchHandle htiBoard, SVTreeType::SVBranchHandle htiDig, const SVDeviceParamCollection* pDeviceParams );
	HRESULT LoadFileAcquisitionConfiguration( SVTreeType& rTree, SVTreeType::SVBranchHandle htiBoardChild, long& lNumAcqDig );

	HRESULT SaveDeviceParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiDig, const SVDeviceParamCollection* pDeviceParams );
	HRESULT LoadDeviceParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiDataChild, SVDeviceParamCollection& svDeviceParams );

	HRESULT SaveDeviceParamSpecial( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent, const SVDeviceParam* pParam );
	HRESULT LoadDeviceParamSpecial( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent, SVDeviceParam* pParam );

	void ConvertLightReferenceEnum(DWORD &dwType);

	void SetupSoftwareTrigger(SVSoftwareTriggerClass* pTriggerDevice, int iDigNum, long triggerPeriod, SVPPQObject* pPPQ);
	void SetupCameraTrigger(SVCameraTriggerClass* pTriggerDevice, int iDigNum, SVPPQObject* pPPQ, bool bSoftwareTrigger, long triggerPeriod);

	void GetRemoteInputInspections( const SVString& p_rRemoteInputName, SVInspectionSet& p_rInspections ) const;

	//************************************
	// Method:    LoadMonitoredObjectList
	// Description: Load a sub list and return this value. Sub list are e.g. productValue- and productImage-list
	// Parameter: SVTreeType & rTree the XML-tree with the config.
	// Parameter: SVTreeType::SVBranchHandle htiParent The parent branch of the monitor list.
	// Parameter: const SVString& listName Name of the sublist
	// Parameter: MonitoredObjectList & subList The sublist which is returned.
	// Returns:   HRESULT S_OK, if loading successful.
	//************************************
	HRESULT LoadMonitoredObjectList( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent, const SVString& listName, MonitoredObjectList& rList );

	SVTriggerObjectArray        m_arTriggerArray;
	SVPPQObjectArray            m_arPPQArray;
	SVVirtualCameraArray        m_arCameraArray;
	SVInspectionProcessVector   m_arInspectionArray;
	SVIMProductEnum             m_eProductType;
	unsigned long				m_ulVersion;
	volatile bool               m_bConfigurationValid;

	SVAcquisitionDeviceMap mAcquisitionDeviceMap;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\svobserver\SVConfigurationObject.h_v  $
 * 
 *    Rev 1.15   20 Nov 2014 05:01:58   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  918
 * SCR Title:  Implement Method RegisterMonitorList for RemoteControl (SVO-369)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   add method ReplaceOrAddMonitorList
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   29 Aug 2014 17:49:04   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added support for get/set product filter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   29 Aug 2014 15:43:16   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  934
 * SCR Title:  Add Remote Access to Environment.Mode Parameters
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Removed GetMode.  Use SVSVIMStateClass::GetMode instead.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   28 Apr 2014 14:22:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added BuildPPQMonitorList method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   24 Apr 2014 10:47:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised the GetActiveRemoteMonitorList method to use a reference rather than return a copy of the list.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   23 Apr 2014 10:39:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added ActivateRemoteMonitorList and GetActiveRemoteMonitorList methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   22 Apr 2014 09:44:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added ValidateRemoteMonitorList method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   17 Apr 2014 16:58:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new methods for the Remote Monitor List
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   17 Mar 2014 15:20:50   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added method SetCameraItems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   07 Mar 2014 18:11:08   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added regions.
 *   Made methods const.
 *   Removed empty method DetachAcqFromTriggers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   30 Oct 2013 10:45:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added #ifndef _WIN64 to prevent depricated PLC code from compiling in 64bit.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   01 Oct 2013 12:48:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   07 Aug 2013 13:31:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Renamed SVIOEntryHostStructPtrVector to SVIOEntryHostStructPtrList.
 * Added HasCameraTrigger method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Jun 2013 15:26:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.41   21 May 2013 13:00:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated configuration object to include the new naming requirements found in IF00100.9401.003.  Also add new funcitonality to get and set remote inputs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.40   21 May 2013 10:00:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  836
 * SCR Title:  Fix loading SEC bug
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Moved SVObserverEnums.h to SCConfigurationLibrary.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 20:05:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   16 Apr 2013 14:15:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:02:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   19 Mar 2013 14:45:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Renamed AddOrUpdateDigitalInput to AddImportedDigitalInput.
 * Renamed AddOrUpdateDDEInput to AddImportedDDEInput.
 * Renamed AddOrUpdateRemoteInput to AddImportedRemoteInput.
 * Revised AddImportedDigitalInput to only add the input is it doesn't already exists.
 * Revised AddImportedDDEInput to only add the input is it doesn't already exists.
 * Revised AddImportedRemoteInput to only add the input is it doesn't already exists.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   11 Feb 2013 14:16:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new framework for the remote socket interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   21 Jan 2013 10:50:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GetPPQByName method
 * Added AddCameraDataInput method
 * Added SetupSoftwareTrigger method
 * Added SetupCameraTrigger method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   16 Jan 2013 16:02:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Migrating branch code into primary code base.  
 * 
 * Consolidated adding and updating inputs to configuration elements to configuratoin object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   04 Oct 2012 11:25:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Replaced the CTreeCtrl with MaterialsTree.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   25 Sep 2012 15:16:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   04 Sep 2012 15:27:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Navigate Tree class to not force the programmer to create an instance of the class to use it.  Converted all functionality to static.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   15 Aug 2012 14:30:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new Singlton version of the SVIOConfigurationInterfaceClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   02 Jul 2012 16:39:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   18 Jun 2012 18:15:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Centralized the location of the configuration object identifier and removed synchronization problems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   30 Jan 2012 11:09:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Modified source code to consolidate PLC and New Output stream functionality to I/O Controller.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   20 Dec 2011 14:55:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  741
 * SCR Title:  Fix IO List when changing Inspection Name
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added function calls to rename the output list when inspection name has been changed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   19 Dec 2011 15:41:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the configuration object to handle new functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   06 Jun 2011 13:06:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  722
 * SCR Title:  Fix a problem with Raid Error Information on X2 systems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to correct issues with the RAID error information associated with the X2 products.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   27 Jan 2011 10:58:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   08 Dec 2010 07:51:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   09 Nov 2010 16:09:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   15 Feb 2010 12:25:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to remove the generations of board image processing classes from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   15 Dec 2009 15:13:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   14 Nov 2008 13:54:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added code to save PLC Outputs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   16 Sep 2008 14:29:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added SaveFileAcquisitionConfiguration method.
 * Added LoadFileAcquisitionConfiguration method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   10 Sep 2008 16:53:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added SaveAcquisitionDeviceFilename
 * Added LoadAcquisitionDeviceFilename
 * Added SaveDeviceParameters
 * Added LoadDeviceParameters
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   21 Jun 2007 12:01:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   09 Apr 2007 11:54:14   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  588
 * SCR Title:  Add the RAID Error Bit for the digital IO
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new method to determine if the configuration is of a RAID type
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   17 Jan 2006 10:00:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added GetInspections
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   29 Jun 2005 07:37:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  493
 * SCR Title:  Fix Bug in SVOutputObjectList
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added ValidateOutputList Function
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   20 Oct 2004 15:41:42   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  375
 * SCR Title:  Remodel XML usage for configuration storage and SIAC transfers.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   The return type of SVConfigurationObject::LoadConfiguration(CTreeCtrl &rTree) was changed from BOOL to HRESULT in order to better debug loading problems.
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   12 Jul 2004 12:53:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  428
 * SCR Title:  Improve Load Configuration time
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented get file version information
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   16 Jan 2004 07:48:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added temporary functions to connect acquisitionclass objects to trigger signals (including the strobe start frame)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   10 Dec 2003 09:16:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added convert function for Light Refernece values from the Coreco values to SVObservers ENUMS
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   28 Jul 2003 10:44:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for specialized DeviceParams
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   08 Jul 2003 11:47:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   add SVDeviceParam for Digital support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   11 Jun 2003 13:49:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  359
 * SCR Title:  Version Check on Configurations that are saved with  SVX extensions
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new method to return the SVX version before the configuration is loaded.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   17 Apr 2003 17:16:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   28 Mar 2003 12:31:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  318
 * SCR Title:  Image Transfer Compression
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added support for knowing if the configuration is valid (loaded) - IsConfigurationLoaded, SetConfigurationLoaded, m_bIsConfigurationValid
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   14 Mar 2003 07:04:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  336
 * SCR Title:  Change Light Reference and LUT dialogs on RGB mono system
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added awareness of configuration type
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   10 Jan 2003 09:40:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Modified AcquisitionDevice functions to use Lut
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Dec 2002 15:18:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  299
 * SCR Title:  SIAC needs to be able to request Source Images
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added:
 * GetChildObjectByName(LPCTSTR tszName, SVTriggerObject** ppTrigger);
 * GetChildObjectByName(LPCTSTR tszName, SVPPQObject** ppPPQ);
 * GetChildObjectByName(LPCTSTR tszName, SVVirtualCamera** ppCamera);
 * GetChildObjectByName(LPCTSTR tszName, SVInspectionProcess** ppInspection);
 * GetInspectionObject(LPCTSTR tszFullyQualifiedNameOfChild,
 * SVInspectionProcess** ppInspection);
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 12:51:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
