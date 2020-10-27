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

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
#include "MonitorListAttributeStruct.h"
#include "RemoteMonitorList.h"
#include "SVInspectionProcess.h"
#include "SVPPQObject.h"
#include "SVStorage.h"
#include "SVStorageResult.h"
#include "Definitions/StringTypeDef.h"
#include "SVImageLibrary/SVLightReference.h"
#include "SVImageLibrary/SVLut.h"
#include "SVProtoBuf/TriggerRecordController.h"
#include "SVSharedMemoryLibrary/SVProductFilterEnum.h"
#include "SVSystemLibrary/SVObserverEnums.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary/AudidFiles.h"
#pragma endregion Includes


#pragma region Declarations
namespace SvIe
{
	struct SVConfigurationAcquisitionDeviceInfoStruct;
}
namespace  SvOi
{
	class IObjectWriter;
}
namespace SvPb
{
	class InspectionCmdMsgs;
}
namespace SvTi
{
	class SVTriggerObject;
	typedef std::vector<SVTriggerObject*> SVTriggerObjectPtrVector;
}
namespace SvTh
{
	class SvTh::SVTriggerClass;
}
namespace  SvXml
{
	class SVObjectXMLWriter;
}


class SVDeviceParamCollection;
class SVDeviceParam;
class SVFileNameArrayClass;
class SVIOController;
class SVNavigateTree;
class SVRemoteOutputGroup;
class SVRemoteOutputObject;
class SVPPQObject;
class SVLightReference;

typedef SvXml::SVXMLMaterialsTree SVTreeType;
#pragma endregion Declarations

struct SVFindPredicate
{
	SVTreeType& m_rTree;
	std::string m_Name;

	SVFindPredicate(SVTreeType& rTree, const std::string& p_rName) : m_rTree(rTree), m_Name(p_rName) {}
	SVFindPredicate(SVTreeType& rTree, int StringResourceID) : m_rTree(rTree)
	{
		m_Name = SvUl::LoadStdString(StringResourceID);
	}

	bool operator()(const SVTreeType::SVBranchHandle& p_rRight) const
	{
		bool Result(false);
		std::string Name;

		Name = m_rTree.getBranchName(p_rRight);
		Result = (m_Name == Name);

		return Result;
	}
};

class SVConfigurationObject : public SVObjectClass
{
	///This class does not need to call SV_DECLARE_CLASS as it is a base class and only derived classes are instantiated
	//SV_DECLARE_CLASS( SVConfigurationObject );

public:
	typedef std::map<std::string, SvIe::SVConfigurationAcquisitionDeviceInfoStruct*> SVAcquisitionDeviceMap;

	explicit SVConfigurationObject(LPCSTR ObjectName);
	SVConfigurationObject(SVObjectClass* pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVCONFIGURATIONOBJECT);
	virtual ~SVConfigurationObject();

	virtual HRESULT GetChildObject(SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index = 0) const override;

	void SaveConfiguration(SvXml::SVObjectXMLWriter& rWriter) const;

	//************************************
	//! Load Configuration 
	//! \param rTree [in ]  with contents of cfg file 
	//! \returns HRESULT
	//! \throw  MessageContainer
	//************************************
	HRESULT LoadConfiguration(SVTreeType& rTree);

	//************************************
	//! Load Enviroment section of configuration
	//! \param rTree [in]
	//! \param rConfigurationColor [out]
	//! \throw  MessageContainer
	//! \returns void
	//************************************
	void LoadEnviroment(SVTreeType& rTree, bool& rConfigurationColor);

	//************************************
	//! Load IO sectiom 
	//! \param rTree [in]
	//! \throw  MessageContainer
	//! \returns false if no io section was found 
	//************************************
	bool LoadIO(SVTreeType& rTree);
	//************************************
	//! Load AcquisitionDevice
	//! \param rTree [in]
	//! \param BoardName [out]
	//! \param lNumBordDig out]
	//! \throw  MessageContainer
	//! \returns true if AcquisitionDevice section was found 
	//************************************
	bool LoadAcquisitionDevice(SVTreeType& rTree, std::string& BoardName, long& lNumBordDig);

	//************************************
	//! Load Camera 
	//! \param rTree [in]
	//! \param lNumCameras [out]
	//! \param rConfigurationColor [in]
	//! \throw  MessageContainer
	//! \returns true if camerasection was found 
	//************************************
	bool LoadCameras(SVTreeType& rTree, long& lNumCameras, bool ConfigurationColor);

	//************************************
	//! Load Trigger section 
	//! \param rTree [in]
	//! \throw  MessageContainer
	//! \returns true  if trigger section was found 
	//************************************
	bool LoadTrigger(SVTreeType& rTree);

	//************************************
	//! Load Inspection 
	//! \param rTree [in]
	//! \throw  MessageContainer
	//! \returns bool
	//************************************
	bool LoadInspection(SVTreeType& rTree);
	//************************************

	//! Load PPQ 
	//! \param rTree [in]
	//! \throw  MessageContainer
	//! \returns bool
	//************************************
	bool LoadPPQ(SVTreeType& rTree);

	HRESULT GetInspectionItems(const SvDef::StringSet& rNames, SVNameStorageResultMap& rItems) const;
	HRESULT GetRemoteInputItems(const SvDef::StringSet& rNames, SVNameStorageResultMap& rItems) const;

	HRESULT SetInspectionItems(const SVNameStorageMap& p_rItems, SVNameStatusMap& p_rStatus, bool RunOnce);
	HRESULT SetRemoteInputItems(const SVNameStorageMap& p_rItems, SVNameStatusMap& p_rStatus);
	HRESULT SetCameraItems(const SVNameStorageMap& p_rItems, SVNameStatusMap& p_rStatus);

	bool RebuildInputOutputLists(bool isLoad = false);
	bool Activate();

	bool DestroyConfiguration();

	void SetInputObjectList(std::unique_ptr<SVInputObjectList>&& pInputObjectList) { m_pInputObjectList.swap(pInputObjectList); }
	SVInputObjectList* GetInputObjectList() const { return (nullptr != m_pInputObjectList) ? m_pInputObjectList.get() : nullptr; }
	void SetOutputObjectList(std::unique_ptr<SVOutputObjectList>&& pOutputObjectList) { m_pOutputObjectList.swap(pOutputObjectList); }
	SVOutputObjectList* GetOutputObjectList() const { return (nullptr != m_pOutputObjectList) ? m_pOutputObjectList.get() : nullptr; }

	std::list<SVFileNameClass>& getAdditionalFiles() { return m_AdditionalFiles; }

	HRESULT RebuildOutputObjectList();

	bool AddAcquisitionDevice(LPCTSTR szName, const SVFileNameArrayClass& rsvFiles,
		const SVLightReference& rsvLight,
		const SVLut& rLut,
		const SVDeviceParamCollection* rpDeviceParams);
	bool ModifyAcquisitionDevice(LPCTSTR szName, const SVLightReference& rsvLight);
	bool ModifyAcquisitionDevice(LPCTSTR szName, const SVLut& lut);
	bool ModifyAcquisitionDevice(LPCTSTR szName, const SVDeviceParamCollection* pParams);

	bool RemoveAcquisitionDevice(LPCTSTR szName);
	bool GetAcquisitionDevice(LPCTSTR szName,
		SVFileNameArrayClass*& pFiles,
		SVLightReference*& pLight,
		SVLut*& rpLut,
		SVDeviceParamCollection*& rpDeviceParams) const;
	SVAcquisitionDeviceMap::iterator GetAcquisitionDeviceStartPosition();
	SVAcquisitionDeviceMap::iterator GetAcquisitionDeviceEndPosition();
	void GetAcquisitionDeviceNextAssoc(SVAcquisitionDeviceMap::const_iterator& rNextPosition, std::string& rKey) const;
	void GetAcquisitionDeviceNextAssoc(SVAcquisitionDeviceMap::const_iterator& rNextPosition, std::string& rKey,
		SVFileNameArrayClass*& pFiles,
		SVLightReference*& pLight,
		SVLut*& rpLut,
		SVDeviceParamCollection*& rpDeviceParams) const;

	bool AddTrigger(SvTi::SVTriggerObject* pTrigger);
	bool RemoveTrigger(SvTi::SVTriggerObject* pTrigger);
	long GetTriggerCount() const;
	SvTi::SVTriggerObject* GetTrigger(long lIndex) const;
	bool GetChildObjectByName(LPCTSTR tszName, SvTi::SVTriggerObject** ppTrigger) const;

	bool AddPPQ(SVPPQObject* pPPQ);
	bool RemovePPQ(const SVPPQObject* const pPPQ);
	long GetPPQCount() const;
	SVPPQObject* GetPPQ(long lIndex) const;
	bool GetChildObjectByName(LPCTSTR tszName, SVPPQObject** ppPPQ) const;
	bool GetPPQByName(LPCTSTR name, SVPPQObject** ppPPQ) const;

	bool AddCamera(SvIe::SVVirtualCamera* pCamera);
	bool RemoveCamera(SvIe::SVVirtualCamera* pCamera);
	long GetCameraCount() const;
	SvIe::SVVirtualCamera* GetCamera(long lIndex) const;
	bool GetChildObjectByName(LPCTSTR tszName, SvIe::SVVirtualCamera** ppCamera) const;

	bool AddInspection(SVInspectionProcess* pInspection);
	bool RemoveInspection(SVInspectionProcess* pInspection);
	long GetInspectionCount() const;
	SVInspectionProcess* GetInspection(long lIndex) const;
	const SVInspectionProcessVector& GetInspections() const { return m_arInspectionArray; };
	bool GetChildObjectByName(LPCTSTR tszName, SVInspectionProcess** ppInspection) const;
	bool GetInspectionObject(LPCTSTR tszFullyQualifiedNameOfChild, SVInspectionProcess** ppInspection) const;

	static bool GetInspection(LPCTSTR InspectionName, SVInspectionProcess*& prInspection);

	HRESULT AttachAcqToTriggers();
	HRESULT ValidateOutputList();

	//! Method makes changes to the loaded configuration as required to upgrade it to the newer version
	void UpgradeConfiguration();

	SVIMProductEnum GetProductType() const;
	void SetProductType(SVIMProductEnum eProductType, bool newConfig = false);
	bool IsConfigurationLoaded() const;
	void SetConfigurationLoaded();
	bool RenameOutputListInspectionNames(LPCTSTR OldInspectionName, LPCTSTR NewInspectionName);

	HRESULT SetModuleReady(bool p_Value);
	HRESULT SetRaidErrorBit(bool p_Value);

	SVIOEntryHostStructPtr GetModuleReady();
	SVIOEntryHostStructPtr GetRaidErrorBit();

	SVIOController* GetIOController() const { return (nullptr != m_pIOController) ? m_pIOController.get() : nullptr; }

	uint32_t GetRemoteOutputController() const;
	size_t GetRemoteOutputGroupCount() const;
	void SetupRemoteOutput();
	HRESULT ClearRemoteOutputUnUsedData();
	HRESULT GetRemoteOutputGroupNames(SvDef::StringVector& rPPQs) const;
	SVRemoteOutputGroup* GetRemoteOutputGroup(const std::string& rRemoteGroupID) const;
	size_t GetRemoteOutputGroupItemCount(const std::string& rRemoteGroupID) const;
	HRESULT GetRemoteOutputItem(const std::string& rRemoteGroupID, long l_lIndex, SVRemoteOutputObject*& p_rItem) const;
	SVRemoteOutputObject* GetFirstRemoteOutputObject(const std::string& rRemoteGroupID) const;
	HRESULT AddRemoteOutputItem(const std::string& rRemoteGroupID, SVRemoteOutputObject*& p_pNewOutput, uint32_t p_InputObjectID, const std::string& rPPQ);
	HRESULT DeleteRemoteOutput(const std::string& rRemoteGroupID);
	HRESULT DeleteRemoteOutputEntry(const std::string& rRemoteGroupID, SVRemoteOutputObject* p_pOutputObject);
	HRESULT RemoteOutputValidateInputs();

	HRESULT AddImportedRemoteInput(SVPPQObject* p_pPPQ, const std::string& name, long ppqPosition, long index, const _variant_t& p_Value);
	HRESULT AddImportedDigitalInput(SVPPQObject* p_pPPQ, const std::string& name, long ppqPosition);
	HRESULT AddRemoteInput(SVPPQObject* p_pPPQ, const std::string& name, long ppqPosition, long index, const _variant_t& p_Value);
	HRESULT AddDigitalInput(SVPPQObject* p_pPPQ, const std::string& name, long ppqPosition);

	bool SetupRemoteMonitorList();
	RemoteMonitorListMap GetRemoteMonitorList() const;
	void SetRemoteMonitorList(const RemoteMonitorListMap& rList);

	//************************************
	// Method:    ReplaceOrAddMonitorList
	// Description:  Replace a monitor list if it already exists or add a new monitor list to the configuration.
	// Parameter: const RemoteMonitorNamedList & rList
	// Returns:   void
	//************************************
	void ReplaceOrAddMonitorList(const RemoteMonitorNamedList& rList);
	void ValidateRemoteMonitorList();
	//! Activate or deactivate the Monitorlist.
	//!	When the Monitorlist is activated all Other Monitorlist which belong to the same PPQ are deactivated
	//! \param listName [in]
	//! \param bActivate [in]
	//! \returns S_OK when unsuccessfully 
	HRESULT ActivateRemoteMonitorList(const std::string& listName, bool bActivate);

	//Return the number of active Monitorlist
	int GetActiveMonitorListCount() const;

	HRESULT GetRemoteMonitorListProductFilter(const std::string& listName, SvSml::SVProductFilterEnum& rFilter) const;
	HRESULT SetRemoteMonitorListProductFilter(const std::string& listName, SvSml::SVProductFilterEnum filter);
	void BuildPPQMonitorList(PPQMonitorList& ppqMonitorList) const;

	//************************************
	// Method:    LoadRemoteMonitorList
	// Description: Loads the Remote Monitor List(s) from the configuration file
	// Parameter: SVTreeType & rTree the XML-tree with the config.
	// Returns:   HRESULT S_OK, if loading successful.
	//************************************
	HRESULT LoadRemoteMonitorList(SVTreeType& rTree);

	//************************************
	//! The method loads the Global Constant list
	//! \param rTree <in> a reference to the XML-tree which it will be loaded from
	//! \returns S_OK, if loading successful
	//************************************
	HRESULT LoadGlobalConstants(SVTreeType& rTree);

	//************************************
	//! The method loads the ObjectAttributesSet
	//! \param rTree <in> a reference to the XML-tree which it will be loaded from
	//! \returns S_OK, if loading successful
	//************************************
	HRESULT LoadObjectAttributesSet(SVTreeType& rTree);

	//************************************
	//! The method loads the Additional File list
	//! \param rTree <in> a reference to the XML-tree which it will be loaded from
	//! \returns S_OK, if loading successful
	//************************************
	HRESULT LoadAdditionalFiles(SVTreeType& rTree);

	SvPb::InspectionList GetInspectionList4TRC() const { return m_inspList4TRC; };
	void UpdateInspectionList4TRC();

	//************************************
	/// Update a toolset of a configuration tree and change older version to fit to the newer application. 
	/// (e.g. if LUT Equation Clip is missing, it add them with the value false)
	/// \param rTree [in,out] The whole tree of the xml-configuration.
	/// \param rToolset [in,out] The branch of the tool set which should be updated.
	//************************************
	static void updateConfTreeToNewestVersion(SVTreeType& rTree, SVTreeType::SVBranchHandle& rToolset);

	/// Return true, if adding of parameters of a tool from the active monitor list possible.
	/// \param ppqName [in] PPQ-name.
	/// \param toolId [in] Tool ID.
	/// \returns bool
	bool isAddParameter2MonitorListPossible(LPCTSTR ppqName, uint32_t toolId) const;
	/// Return true, if removing of parameters of a tool from the active monitor list possible.
	/// \param ppqName [in] PPQ-name.
	/// \param toolId [in] Tool ID.
	/// \returns bool
	bool isRemoveParameter2MonitorListPossible(LPCTSTR ppqName, uint32_t toolId) const;

	/// Return true, if all requested parameter of a tool in the active monitor list.
	/// \param ppqName [in] PPQ-name.
	/// \param toolId [in] Tool ID.
	/// \returns bool
	bool areParametersInMonitorList(LPCTSTR ppqName, uint32_t toolId) const;

	SvStl::MessageContainerVector addParameter2MonitorList(LPCTSTR ppqName, uint32_t toolId);
	SvStl::MessageContainerVector removeParameter2MonitorList(LPCTSTR ppqName, uint32_t toolId);

#pragma region Methods to replace processMessage
	virtual void OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName) override;
#pragma endregion Methods to replace processMessage

	
	size_t SetAudidWhiteList(std::vector<SvUl::AudidFile>&& Files)
	{
		return m_AudidWhiteList.SetValues(std::move(Files));
	}
	
	size_t SetAudidDefaultList(std::vector< SvUl::AudidFile>&& Files)
	{
		return m_AudidDefaultList.SetValues(std::move(Files));
	}
	void UpdateAudidFiles(bool calculatehash);
	const std::vector< SvUl::AudidFile>& GetAudidDefaultList()   const;
	const std::vector< SvUl::AudidFile>& GetAudidWhiteList()    const ;
private:
	typedef std::set<SVInspectionProcess*> SVInspectionSet;
	typedef std::map<UINT, SVObjectReferenceVector> AttributesSetMap;

	bool FinishIPDoc(SVInspectionProcess* pInspection, bool isLoad = false);
	void SaveEnvironment(SvOi::IObjectWriter& rWriter) const;
	void SaveIO(SvOi::IObjectWriter& rWriter) const;
	void SaveAcquisitionDevice(SvOi::IObjectWriter& rWriter) const;
	void SaveAcquistionConfiguration(SvOi::IObjectWriter& rWriter, const SVLightReference& rLight, const SVLut& rLut, const SVDeviceParamCollection& rDeviceParams) const;
	void SaveCamera(SvOi::IObjectWriter& rWriter) const;
	void SaveTrigger(SvOi::IObjectWriter& rWriter) const;
	void SaveInspection(SvOi::IObjectWriter& rWriter, AttributesSetMap& rAttributeSetVector) const;
	void SavePPQ(SvOi::IObjectWriter& rWriter) const;
	void SavePPQ_Attributes(SvOi::IObjectWriter& rWriter, const SVPPQObject& rPPQ) const;
	void SavePPQ_Cameras(SvOi::IObjectWriter& rWriter, const SVPPQObject& rPPQ) const;
	void SavePPQ_Inspections(SvOi::IObjectWriter& rWriter, const SVPPQObject& rPPQ) const;

	//************************************
	//! Add the current monitor lists to the xml-file.
	//! \param rWriter reference to the xml-writer.
	//! \returns true on success
	//************************************
	bool SaveRemoteMonitorList(SvOi::IObjectWriter& rWriter) const;

	//************************************
	//! Add the current sub monitor lists to the tree
	//! \param rWriter reference to the xml-writer.
	//! \param listName Name of the sublist.
	//! \param subList The sublist which should saved.
	//! \returns true on success
	//************************************
	bool SaveMonitoredObjectList(SvOi::IObjectWriter& rWriter, const std::string& listName, const MonitoredObjectList& subList) const;

	//************************************
	//! The method saves the Global Constant list
	//! \param rWriter <in> a reference to the xml-writer
	//************************************
	void SaveGlobalConstants(SvOi::IObjectWriter& rWriter) const;

	//************************************
	//! The method saves the Object Attributes Set list
	//! \param rWriter <in> a reference to the xml-writer
	//! \param rAttributesSetMap <in> a reference to attribute set map
	//************************************
	void SaveObjectAttributesSet(SvOi::IObjectWriter& rWriter, const AttributesSetMap& rAttributesSetMap) const;

	//************************************
	//! The method saves the Additional file list
	//! \param rWriter <in> a reference to the xml-writer
	//************************************
	void SaveAdditionalFiles(SvOi::IObjectWriter& rWriter) const;

	//************************************
	//! The method gets Object Attributes for the inspection
	//! \param pInspection <in> a pointer to the inspection
	//! \param rAttributesSetMap <in> a reference to attribute set map
	//************************************
	void getInspectionObjectAttributesSet(const SVInspectionProcess* pInspection, AttributesSetMap& rAttributesSetMap) const;

	//************************************
	//! The method checks and if necessary converts the product type to a mixed type
	//! \param rConfigType <out> a reference to obtain the configuration color
	//************************************
	void ConvertColorToStandardProductType(bool& rConfigType);

	HRESULT LoadAcquisitionDeviceFilename(SVTreeType& rTree, SVTreeType::SVBranchHandle hDig, SVFileNameArrayClass& rFileArray);

	void SaveFileAcquisitionConfiguration(SvOi::IObjectWriter& rWriter, const SVDeviceParamCollection& rDeviceParams) const;
	HRESULT LoadFileAcquisitionConfiguration(SVTreeType& rTree, SVTreeType::SVBranchHandle htiBoardChild, long& lNumAcqDig);

	void SaveDeviceParameters(SvOi::IObjectWriter& rWriter, const SVDeviceParamCollection& rDeviceParams) const;
	HRESULT LoadDeviceParameters(SVTreeType& rTree, SVTreeType::SVBranchHandle htiDataChild, SVDeviceParamCollection& svDeviceParams);

	void SaveDeviceParamSpecial(SvOi::IObjectWriter& rWriter, const SVDeviceParam* pParam) const;
	HRESULT LoadDeviceParamSpecial(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent, SVDeviceParam* pParam);

	void SetupSoftwareTrigger(SvTh::SVTriggerClass* pTriggerDevice, int iDigNum, long triggerPeriod, SVPPQObject* pPPQ);

	void GetRemoteInputInspections(const std::string& p_rRemoteInputName, SVInspectionSet& p_rInspections) const;

	//************************************
	// Method:    LoadMonitoredObjectList
	// Description: Load a sub list and return this value. Sub list are e.g. productValue- and productImage-list
	// Parameter: SVTreeType & rTree the XML-tree with the config.
	// Parameter: SVTreeType::SVBranchHandle htiParent The parent branch of the monitor list.
	// Parameter: const std::string& listName Name of the sublist
	// Parameter: MonitoredObjectList & subList The sublist which is returned.
	// Returns:   HRESULT S_OK, if loading successful.
	//************************************
	HRESULT LoadMonitoredObjectList(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent, const std::string& listName, MonitoredObjectList& rList);

	bool getObjectsForMonitorList(uint32_t toolId, SvPb::InspectionCmdResponse& rResponse) const;

	void initializeIO(SVIMProductEnum newConfigType);
	void changeSystemResetIO(SVIMProductEnum newConfigType);


	void SaveAudidList(SvOi::IObjectWriter& rWriter, SvUl::AudidListType type) const;
	HRESULT LoadAudidList(SVTreeType& rTree, SvUl::AudidListType type)  ;

	void SaveAudidList(SvOi::IObjectWriter& rWriter) const;

	HRESULT LoadAudidList(SVTreeType& rTree);

	std::list<SVFileNameClass>    m_AdditionalFiles;  //We need a list as the file manager has pointers to these objects!
	std::unique_ptr<SVIOController> m_pIOController;
	std::unique_ptr<SVInputObjectList> m_pInputObjectList;
	std::unique_ptr<SVOutputObjectList> m_pOutputObjectList;
	SvTi::SVTriggerObjectPtrVector  m_arTriggerArray;
	SVPPQObjectPtrVector            m_arPPQArray;
	SvIe::SVVirtualCameraPtrVector  m_arCameraArray;
	SVInspectionProcessVector   m_arInspectionArray;
	SVIMProductEnum             m_eProductType{ SVIM_PRODUCT_TYPE_UNKNOWN };
	volatile bool               m_bConfigurationValid = false;

	SVAcquisitionDeviceMap m_AcquisitionDeviceMap;
	SvPb::InspectionList m_inspList4TRC;

	///list with additional files for audidtrail  
	
	mutable SvUl::CAudidFiles m_AudidWhiteList;
	/// list with default files for audidtrail  
	mutable SvUl::CAudidFiles m_AudidDefaultList;

};


