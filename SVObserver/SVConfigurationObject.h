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
#include "SVImageLibrary/SVLightReference.h"
#include "SVImageLibrary/SVLut.h"
#include "SVObjectLibrary/SVObserverTemplate.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVSharedMemoryLibrary/SVProductFilterEnum.h"

#include "SVFileSystemLibrary/SVFileNameArrayClass.h"
#include "SVInfoStructs.h"
#include "SVInspectionProcess.h"
#include "SVSystemLibrary/SVObserverEnums.h"
#include "SVPPQObject.h"
#include "TriggerInformation/SVTriggerObject.h"
#include "TriggerHandling/SVTriggerClass.h"
#include "TriggerInformation/SVSoftwareTriggerClass.h"
#include "TriggerInformation/SVCameraTriggerClass.h"
#include "SVStorage.h"
#include "SVStorageResult.h"
#include "RemoteMonitorList.h"
#include "SVXMLLibrary\SVObjectXMLWriter.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes


//Namespace used only for forward declaration
namespace SvTh
{
	class SVSoftwareTriggerClass;
	class SVCameraTriggerClass;
} //namespace SvTh



#pragma region Declarations
class SVDeviceParamCollection;
class SVDeviceParam;
class SVIOController;
class SVNavigateTree;
class SVRemoteOutputGroup;
class SVRemoteOutputObject;
class SVPPQObject;

struct SVConfigurationAcquisitionDeviceInfoStruct;
typedef SvXml::SVXMLMaterialsTree SVTreeType;
#pragma endregion Declarations

struct SVFindPredicate
{
	SVTreeType& m_rTree;
	std::string m_Name;

	SVFindPredicate( SVTreeType& rTree, const std::string& p_rName ) : m_rTree( rTree ), m_Name( p_rName ) {}
	SVFindPredicate( SVTreeType& rTree, int StringResourceID ) : m_rTree( rTree ) 
	{
		m_Name = SvUl::LoadStdString(StringResourceID);
	}

	bool operator()( const SVTreeType::SVBranchHandle& p_rRight ) const
	{
		bool Result( false );
		std::string Name;

		Name = m_rTree.getBranchName( p_rRight);
		Result = ( m_Name == Name );

		return Result;
	}
};

class SVConfigurationObject : public SVObjectClass
{
	SV_DECLARE_CLASS( SVConfigurationObject );

public:
	typedef std::map<std::string, SVConfigurationAcquisitionDeviceInfoStruct*> SVAcquisitionDeviceMap;

	SVConfigurationObject( LPCSTR ObjectName );
	SVConfigurationObject( SVObjectClass* pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVCONFIGURATIONOBJECT );
	virtual ~SVConfigurationObject();

	virtual HRESULT GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index = 0 ) const override;

	unsigned long GetSVXFileVersion(SVTreeType& rTree);

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
	void LoadEnviroment(SVTreeType& rTree, bool &rConfigurationColor);
	
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
	bool LoadAcquisitionDevice(SVTreeType& rTree, std::string &BoardName, long &lNumBordDig );
	
	//************************************
	//! Load Camera 
	//! \param rTree [in]
	//! \param lNumCameras [out]
	//! \param rConfigurationColor [in]
	//! \throw  MessageContainer
	//! \returns true if camerasection was found 
	//************************************
	bool LoadCameras(SVTreeType&  rTree, long& lNumCameras,bool ConfigurationColor);
	
	//************************************
	//! Load Trigger section 
	//! \param rTree [in]
	//! \throw  MessageContainer
	//! \returns true  if trigger section was found 
	//************************************
	bool LoadTrigger(SVTreeType& rTree );
	
	//************************************
	//! Load Inspection 
	//! \param rTree [in]
	//! \throw  MessageContainer
	//! \returns bool
	//************************************
	bool LoadInspection(SVTreeType& rTree );
	//************************************
	
	//! Load PPQ 
	//! \param rTree [in]
	//! \throw  MessageContainer
	//! \returns bool
	//************************************
	bool LoadPPQ(SVTreeType& rTree );

	HRESULT GetInspectionItems( const SVNameSet& p_rNames, SVNameStorageResultMap& p_rItems ) const;
	HRESULT GetRemoteInputItems( const SVNameSet& p_rNames, SVNameStorageResultMap& p_rItems ) const;

	HRESULT SetInspectionItems(const SVNameStorageMap& p_rItems, SVNameStatusMap& p_rStatus, bool RunOnce);
	HRESULT SetRemoteInputItems( const SVNameStorageMap& p_rItems, SVNameStatusMap& p_rStatus );
	HRESULT SetCameraItems( const SVNameStorageMap& p_rItems, SVNameStatusMap& p_rStatus );

	bool RebuildInputOutputLists();
	bool Activate();

	bool DestroyConfiguration();

	void SetInputObjectList( SVInputObjectList* pInputObjectList );
	SVInputObjectList* GetInputObjectList( ) const;

	void SetOutputObjectList( SVOutputObjectList* pOutputObjectList );
	SVOutputObjectList* GetOutputObjectList( ) const;
	HRESULT RebuildOutputObjectList();

	bool AddAcquisitionDevice( LPCTSTR szName, SVFileNameArrayClass& rsvFiles,
                               SVLightReference& rsvLight,
							   SVLut& rLut,
							   const SVDeviceParamCollection* rpDeviceParams );
	bool ModifyAcquisitionDevice( LPCTSTR szName, SVLightReference& rsvLight );
	bool ModifyAcquisitionDevice( LPCTSTR szName, const SVLut& lut );
	bool ModifyAcquisitionDevice( LPCTSTR szName, const SVDeviceParamCollection* pParams );

	bool RemoveAcquisitionDevice( LPCTSTR szName );
	bool GetAcquisitionDevice( LPCTSTR szName,
	                           SVFileNameArrayClass*& pFiles,
                               SVLightReference*& pLight,
							   SVLut*& rpLut,
							   SVDeviceParamCollection*& rpDeviceParams ) const;
	SVAcquisitionDeviceMap::iterator GetAcquisitionDeviceStartPosition();
	SVAcquisitionDeviceMap::iterator GetAcquisitionDeviceEndPosition();
	void GetAcquisitionDeviceNextAssoc( SVAcquisitionDeviceMap::const_iterator& rNextPosition, std::string& rKey ) const;
	void GetAcquisitionDeviceNextAssoc( SVAcquisitionDeviceMap::const_iterator& rNextPosition, std::string& rKey,
	                                    SVFileNameArrayClass*& pFiles,
                                        SVLightReference*& pLight,
										SVLut*& rpLut,
										SVDeviceParamCollection*& rpDeviceParams ) const;

	bool AddTrigger( SvTi::SVTriggerObject* pTrigger );
	bool RemoveTrigger( SvTi::SVTriggerObject* pTrigger );
	long GetTriggerCount( ) const;
	SvTi::SVTriggerObject* GetTrigger( long lIndex ) const;
	bool GetChildObjectByName( LPCTSTR tszName, SvTi::SVTriggerObject** ppTrigger ) const;

	bool AddPPQ( SVPPQObject* pPPQ );
	bool RemovePPQ( const SVPPQObject* const pPPQ );
	long GetPPQCount() const;
	SVPPQObject* GetPPQ( long lIndex ) const;
	bool GetChildObjectByName( LPCTSTR tszName, SVPPQObject** ppPPQ ) const;
	bool GetPPQByName( LPCTSTR name, SVPPQObject** ppPPQ ) const;

	bool AddCamera( SVVirtualCamera* pCamera );
	bool RemoveCamera( SVVirtualCamera* pCamera );
	long GetCameraCount( ) const;
	SVVirtualCamera* GetCamera( long lIndex ) const;
	bool GetChildObjectByName( LPCTSTR tszName, SVVirtualCamera** ppCamera ) const;

	bool AddInspection( SVInspectionProcess* pInspection );
	bool RemoveInspection( SVInspectionProcess* pInspection );
	long GetInspectionCount( ) const;
	SVInspectionProcess* GetInspection( long lIndex ) const;
	const SVInspectionProcessVector& GetInspections() const { return m_arInspectionArray; };
	bool GetChildObjectByName( LPCTSTR tszName, SVInspectionProcess** ppInspection ) const;
	bool GetInspectionObject( LPCTSTR tszFullyQualifiedNameOfChild, SVInspectionProcess** ppInspection ) const;

	static bool GetInspection( LPCTSTR InspectionName, SVInspectionProcess*& prInspection );

	HRESULT AttachAcqToTriggers();
	HRESULT ValidateOutputList();

	//! Method makes changes to the loaded configuration as required to upgrade it to the newer version
	void UpgradeConfiguration();

	SVIMProductEnum GetProductType() const;
	void SetProductType( SVIMProductEnum eProductType );
	bool IsConfigurationLoaded() const;
	void SetConfigurationLoaded();
	bool RenameOutputListInspectionNames(LPCTSTR NewInspectionName, LPCTSTR OldInspectionName);
	
	unsigned long GetFileVersion() const;

	HRESULT SetModuleReady( bool p_Value );
	HRESULT SetRaidErrorBit( bool p_Value );

	SVIOEntryHostStructPtr GetModuleReady();
	SVIOEntryHostStructPtr GetRaidErrorBit();

	SVIOController* GetIOController() const;
	SVGUID GetIOControllerID() const;


	SVGUID GetRemoteOutputController() const;
	size_t GetRemoteOutputGroupCount() const;
	void SetupRemoteOutput();
	HRESULT ClearRemoteOutputUnUsedData();
	HRESULT GetRemoteOutputGroupNames( SvDef::StringVector& rPPQs ) const;
	SVRemoteOutputGroup* GetRemoteOutputGroup( const std::string& rRemoteGroupID ) const;
	size_t GetRemoteOutputGroupItemCount( const std::string& rRemoteGroupID ) const;
	HRESULT GetRemoteOutputItem( const std::string& rRemoteGroupID, long l_lIndex, SVRemoteOutputObject*& p_rItem ) const;
	SVRemoteOutputObject* GetFirstRemoteOutputObject( const std::string& rRemoteGroupID ) const;
	HRESULT AddRemoteOutputItem( const std::string& rRemoteGroupID, SVRemoteOutputObject*& p_pNewOutput, GUID p_InputObjectID, const std::string& rPPQ );
	HRESULT DeleteRemoteOutput( const std::string& rRemoteGroupID );
	HRESULT DeleteRemoteOutputEntry( const std::string& rRemoteGroupID, SVRemoteOutputObject* p_pOutputObject);
	HRESULT RemoteOutputValidateInputs();

	HRESULT AddImportedRemoteInput(SVPPQObject* p_pPPQ, const std::string& name, long ppqPosition, long index, const _variant_t& p_Value);
	HRESULT AddImportedDigitalInput(SVPPQObject* p_pPPQ, const std::string& name, long ppqPosition);
	HRESULT AddRemoteInput(SVPPQObject* p_pPPQ, const std::string& name, long ppqPosition, long index, const _variant_t& p_Value);
	HRESULT AddDigitalInput(SVPPQObject* p_pPPQ, const std::string& name, long ppqPosition);
	HRESULT AddCameraDataInput(SVPPQObject* pPPQ, SVIOEntryHostStructPtr pIOEntry);

	bool SetupRemoteMonitorList();
	void ClearRemoteMonitorList();
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
	
	//! Activates the default monitorlists, when no monitorlist is active and default monitorlist exist  
	//! \returns true if at least one default monitorlist was activated 
	bool ActivateDefaultMonitorList();
	void GetActiveRemoteMonitorList(RemoteMonitorListMap& rActiveList) const;
	
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
	HRESULT LoadRemoteMonitorList( SVTreeType& rTree );

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

	bool HasCameraTrigger(SVPPQObject* p_pPPQ) const;

	//************************************
	/// Update a toolset of a configuration tree and change older version to fit to the newer application. 
	/// (e.g. if LUT Equation Clip is missing, it add them with the value false)
	/// \param rTree [in,out] The whole tree of the xml-configuration.
	/// \param rToolset [in,out] The branch of the tool set which should be updated.
	//************************************
	static void updateConfTreeToNewestVersion(SVTreeType &rTree, SVTreeType::SVBranchHandle &rToolset);

#pragma region Methods to replace processMessage
	virtual void OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName) override;
#pragma endregion Methods to replace processMessage

protected:
	bool FinishIPDoc( SVInspectionProcess* pInspection );

	SVIOController* m_pIOController;

	SVInputObjectList* m_pInputObjectList;
	SVOutputObjectList* m_pOutputObjectList;

private:
	typedef std::set<SVInspectionProcess*> SVInspectionSet;
	typedef std::map<UINT, SVObjectReferenceVector> AttributesSetMap;

	void SaveEnvironment(SvXml::SVObjectXMLWriter& rWriter) const;
	void SaveIO(SvXml::SVObjectXMLWriter& rWriter) const;
	void SaveAcquisitionDevice(SvXml::SVObjectXMLWriter& rWriter) const;
	void SaveAcquistionConfiguration(SvXml::SVObjectXMLWriter& rWriter, const SVLightReference& rLight, const SVLut& rLut, const SVDeviceParamCollection& rDeviceParams) const;
	void SaveCamera(SvXml::SVObjectXMLWriter& rWriter) const;
	void SaveTrigger(SvXml::SVObjectXMLWriter& rWriter) const;
	void SaveInspection(SvXml::SVObjectXMLWriter& rWriter, AttributesSetMap& rAttributeSetVector) const;
	void SavePPQ(SvXml::SVObjectXMLWriter& rWriter) const;
	void SavePPQ_Attributes(SvXml::SVObjectXMLWriter& rWriter, const SVPPQObject& rPPQ ) const;
	void SavePPQ_Cameras(SvXml::SVObjectXMLWriter& rWriter, const SVPPQObject& rPPQ ) const;
	void SavePPQ_Inspections(SvXml::SVObjectXMLWriter& rWriter, const SVPPQObject& rPPQ ) const;

	//************************************
	//! Add the current monitor lists to the xml-file.
	//! \param rWriter reference to the xml-writer.
	//! \returns true on success
	//************************************
	bool SaveRemoteMonitorList(SvXml::SVObjectXMLWriter& rWriter ) const;

	//************************************
	//! Add the current sub monitor lists to the tree
	//! \param rWriter reference to the xml-writer.
	//! \param listName Name of the sublist.
	//! \param subList The sublist which should saved.
	//! \returns true on success
	//************************************
	bool SaveMonitoredObjectList(SvXml::SVObjectXMLWriter& rWriter, const std::string& listName, const MonitoredObjectList& subList ) const;

	//************************************
	//! The method saves the Global Constant list
	//! \param rWriter <in> a reference to the xml-writer
	//************************************
	void SaveGlobalConstants(SvXml::SVObjectXMLWriter& rWriter) const;

	//************************************
	//! The method saves the Object Attributes Set list
	//! \param rWriter <in> a reference to the xml-writer
	//! \param rAttributesSetMap <in> a reference to attribute set map
	//************************************
	void SaveObjectAttributesSet(SvXml::SVObjectXMLWriter& rWriter, const AttributesSetMap& rAttributesSetMap) const;

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
	void ConvertColorToStandardProductType( bool& rConfigType );

	HRESULT LoadAcquisitionDeviceFilename( SVTreeType& rTree, SVTreeType::SVBranchHandle hDig, SVFileNameArrayClass& rFileArray );

	void SaveFileAcquisitionConfiguration( SvXml::SVObjectXMLWriter& rWriter, const SVDeviceParamCollection& rDeviceParams ) const;
	HRESULT LoadFileAcquisitionConfiguration( SVTreeType& rTree, SVTreeType::SVBranchHandle htiBoardChild, long& lNumAcqDig );

	void SaveDeviceParameters( SvXml::SVObjectXMLWriter& rWriter, const SVDeviceParamCollection& rDeviceParams ) const;
	HRESULT LoadDeviceParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiDataChild, SVDeviceParamCollection& svDeviceParams );

	void SaveDeviceParamSpecial( SvXml::SVObjectXMLWriter& rWriter, const SVDeviceParam* pParam ) const;
	HRESULT LoadDeviceParamSpecial( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent, SVDeviceParam* pParam );

	void SetupSoftwareTrigger(SvTi::SVSoftwareTriggerClass* pTriggerDevice, int iDigNum, long triggerPeriod, SVPPQObject* pPPQ);
	void SetupCameraTrigger(SvTi::SVCameraTriggerClass* pTriggerDevice, int iDigNum, SVPPQObject* pPPQ, bool bSoftwareTrigger, long triggerPeriod);

	void GetRemoteInputInspections( const std::string& p_rRemoteInputName, SVInspectionSet& p_rInspections ) const;

	//************************************
	// Method:    LoadMonitoredObjectList
	// Description: Load a sub list and return this value. Sub list are e.g. productValue- and productImage-list
	// Parameter: SVTreeType & rTree the XML-tree with the config.
	// Parameter: SVTreeType::SVBranchHandle htiParent The parent branch of the monitor list.
	// Parameter: const std::string& listName Name of the sublist
	// Parameter: MonitoredObjectList & subList The sublist which is returned.
	// Returns:   HRESULT S_OK, if loading successful.
	//************************************
	HRESULT LoadMonitoredObjectList( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent, const std::string& listName, MonitoredObjectList& rList );

	SvTi::SVTriggerObjectPtrVector  m_arTriggerArray;
	SVPPQObjectPtrVector            m_arPPQArray;
	SVVirtualCameraPtrVector        m_arCameraArray;
	SVInspectionProcessVector   m_arInspectionArray;
	SVIMProductEnum             m_eProductType;
	unsigned long				m_ulVersion;
	volatile bool               m_bConfigurationValid;

	SVAcquisitionDeviceMap m_AcquisitionDeviceMap;
};

typedef std::shared_ptr< SVConfigurationObject > SVConfigurationObjectPtr;

