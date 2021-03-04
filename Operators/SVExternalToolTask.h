//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExternalToolTask
//* .File Name       : $Workfile:   SVExternalToolTask.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   30 Sep 2014 15:42:52  $
//******************************************************************************

#pragma once

#pragma warning (push)
#pragma warning (disable : 4290)

#pragma region Includes
#include <array>
#include "ObjectInterfaces/IExternalToolTask.h"
#include "ObjectInterfaces/IExternalToolTaskDataAdmin.h"
#include "ObjectInterfaces/IInputValueDefinition.h"
#include "SVHBitmapUtilitiesLibrary/SVHBitmapUtilities.h"
#include "InspectionEngine/SVImageClass.h"
#include "InspectionEngine/SVTaskObject.h"
#include "InspectionEngine/SVTaskObjectList.h"
#include "SVDllToolLoadLibraryClass.h"
#include "SVObjectLibrary/InputObject.h"
#include "SVRPropertyTree/SVRPropTreeState.h"
#include "SVValueObjectLibrary/LinkedValue.h"
#include "SVValueObjectLibrary/SVLongValueObjectClass.h"
#include "SVValueObjectLibrary/SVFileNameValueObjectClass.h"
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes


namespace SvOp
{
class TableObject;
class SVResult;
class SVExternalToolTask;
struct SVExternalToolTaskData
{
	enum
	{
		NUM_TOOL_DEPENDENCIES = 50,
		NUM_INPUT_OBJECTS = 50,
		NUM_RESULT_OBJECTS = 50,
		NUM_INPUT_IMAGES = 4,
		NUM_RESULT_IMAGES = 4,
		NUM_RESULT_TABLE_OBJECTS =4,
	};

	SVExternalToolTaskData();
	SVExternalToolTaskData(const SVExternalToolTaskData& src) = delete;
	virtual ~SVExternalToolTaskData();

	SVExternalToolTaskData& operator = (const SVExternalToolTaskData& rhs) = delete;

	void SetInputValueDefinitions(long ArraySize, InputValueDefinitionStruct  InputValueDefs[]);
	void SetInputValueDefinitions(long ArraySize, InputValueDefinitionStructEx  InputValueDefs[]);
	void  InitializeInputs(SVExternalToolTask*  pExternalToolTask, bool initializeAll);
	long  getNumInputs() const { return static_cast<long>(m_InputDefinitions.size()); };
	long  getNumResults() const { return static_cast<long>(m_ResultDefinitions.size()); };
	long  getNumTableResults() const { return static_cast<long>(m_TableResultDefinitions.size()); };
	SvVol::SVFileNameValueObjectClass m_voDllPath;
	std::vector<SvVol::SVFileNameValueObjectClass>  m_aDllDependencies; //[NUM_TOOL_DEPENDENCIES];

	// DLL info
	SvVol::SVStringValueObjectClass  m_voToolName;
	SvVol::SVLongValueObjectClass  m_voToolVersion;

	// Inputs
	std::array<SvOl::InputObject, NUM_INPUT_IMAGES> m_aInputImageInfo; 
	std::vector<SvVol::LinkedValue> m_aInputObjects; //[NUM_INPUT_OBJECTS]; // our own value objects


	std::vector<SvVol::SVVariantValueObjectClass> m_aResultObjects; //[NUM_RESULT_OBJECTS];
	std::vector<SvOp::TableObject*>  m_ResultTableObjects;// SvOp::SVExternalToolTaskData::NUM_RESULT_TABLE_OBJECTS);
	

	SVImageDefinitionStructArray m_aResultImageDefinitions;
	std::vector<ResultValueDefinition> m_ResultDefinitions;
	std::vector<InputValueDefinition> m_InputDefinitions;
	std::vector<ResultTableDefinition> m_TableResultDefinitions;

	long m_lNumInputImages = 0;
	long m_lNumInputValues = 0;  //Number of input definition m_aInputValueDefinitions.size  m_InspectionInputValues.size
	long m_lNumResultImages = 0;
	long m_lNumResultValues = 0; //Number of resulT definition m_ResultDefinitions.size m_InspectionResultValues.size
	long m_NumLinkedValue {0}; //Number of used InputObjects(LinkedValue)
	long m_NumResultTables{0};
	

	SVRPropTreeState m_PropTreeState;
};	// end struct SVExternalToolTaskData

class SVExternalToolTask : public SvIe::SVTaskObjectListClass, public SvOi::IExternalToolTask, public SvOi::IExternalToolTaskDataAdmin
{
	SV_DECLARE_CLASS

public:
	SVExternalToolTask( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SV_EXTERNAL_TOOL_TASK );
	virtual ~SVExternalToolTask();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	bool CreateTableObjects();
	void CreateArrayInTable();
	void SetResultArraySize();
	virtual bool CloseObject() override;
	HRESULT Initialize(std::vector<std::string>& rStatusMsgs, bool inCreationProcess = false, bool initializeAll = false);

#pragma region IExternalToolTask methods
	virtual SvOi::IExternalToolTaskDataAdmin& getExternalToolDataAdmin() override;
	virtual HRESULT triggerInitialize(std::vector<std::string>& rStatusMsgs, bool inCreationProcess = false, bool initializeAll = false) override;
	virtual void SetAllAttributes() override;
	virtual HRESULT ClearData() override;
	virtual HRESULT validateValueParameter(uint32_t laskObjectId, long index, _variant_t newVal) override;
	virtual std::string getDllMessageString(long hResultError) const override;
	virtual SvOi::IObjectClass* getResultRangeObjectAtIndex(int index) override;
	virtual SvPb::GetImageInfoExternalToolResponse getImageInfoList() const override;
#pragma endregion

#pragma region IExternalToolTaskDataAdmin methods
	virtual std::vector<std::shared_ptr<SvOi::IInputValueDefinition>> getInputValuesDefinition() const override;

	virtual long getNumInputValues() const override;
	virtual std::map<std::string, bool> getPropTreeState() const override;
	virtual void setPropTreeState(const std::map<std::string, bool>&)  override;

	virtual long getNumResultValues() const override;
	virtual std::vector<std::shared_ptr<SvOi::IResultValueDefinition>> getResultValuesDefinition() const override;
	virtual SvPb::GetTableResultsExternalToolResponse getTableResults() const override;

#pragma endregion

	void SetPathName( const std::string& rPath );
	HRESULT SetDependencies( const SvDef::StringVector& rDependencies );
	HRESULT GetResultImageDefinitions( SVImageDefinitionStructArray& raResultImageDefinitions );
	HRESULT GetResultValueDefinitions ( std::vector<ResultValueDefinition>& raResultValueDefinitions );

	const std::vector<InputImageInformationStruct>& getInformationStructs() { return m_aInputImageInformationStructs; }
	
	HRESULT GetImageInfo(const SVImageDefinitionStruct* pDefinitionStruct, SVImageInfoClass& rInfo);
	HRESULT GetImageDefinitionStruct( SVImageDefinitionStruct& rImageDef, const SVImageInfoClass& rInfo);

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	virtual bool resetAllObjects(SvStl::MessageContainerVector *pErrorMessages = nullptr) override;
	enum FindEnum
	{
		FIND_VALUES = 1,
		FIND_INPUT_IMAGES = 1 << 1,
		FIND_RESULT_IMAGES = 1 << 2,
		FIND_IMAGES = FIND_INPUT_IMAGES | FIND_RESULT_IMAGES,
		FIND_ALL_OBJECTS_EXP_INPUT_IMAGES = FIND_RESULT_IMAGES | FIND_VALUES, //This flag is to find all objects expect of the input images.
		FIND_ALL_OBJECTS = FIND_IMAGES | FIND_VALUES
	};
	HRESULT GetDLLMessageString(HRESULT hr, BSTR* bstrMessage) const;

	/// Returns true if the External dll Ex functions are not supported in the Version of the loaded config.
	// i.e. loaded version is older than  10.0  
	bool NoExFktInLoadVersion();

	const std::vector<InputImageInformationStruct>& InputImageInformationStructs() { return m_aInputImageInformationStructs; }

	

protected:
	HRESULT Uninitialize();
	
	virtual bool onRun( RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
	void SetIndirectValueSaveFlag();

	SvIe::SVImageClass* GetInputImage(int iIndex, bool bRunMode = false);
	SvIe::SVImageClass* GetResultImage(int iIndex);

	SvVol::SVVariantValueObjectClass* GetResultValueObject(int iIndex);
	SvOp::TableObject*  GetResultTableObject(int iIndex);
	SVResult* GetResultRangeObject(int iIndex);
	std::vector<SVResult*> GetResultRangeObjects();

	HRESULT AllocateResult (int iIndex);

private:
	HRESULT InitializeResultObjects();

	bool prepareInput(SvOi::ITRCImagePtr pResultImageBuffers[], RunStatus& rRunStatus);
	void getResults(SvOi::ITRCImagePtr pResultImageBuffers[]);
	bool anyImagesResized();
	void collectInputValues();
	bool collectInputImages(RunStatus& rRunStatus);
	bool collectMilResultBuffers(SvOi::ITRCImagePtr pResultImageBuffers[]);
	void collectResultValues();
	void collectResultImages(SvOi::ITRCImagePtr pResultImageBuffers[]);

	void updateImageInputInfo();

	SVDLLToolLoadLibraryClass m_dll;

	SVExternalToolTaskData m_Data;	// this is our cancelable data

	std::vector<long>      m_aInspectionInputImages;
	std::vector<long>      m_aInspectionResultImages;
	std::vector<InputImageInformationStruct> m_aInputImageInformationStructs;
	std::vector<SVDIBITMAPINFO> m_aInspectionInputHBMImages;
	std::vector<HBITMAP> m_aInspectionResultHBMImages;

	std::vector<_variant_t> m_InspectionInputValues;
	std::vector<_variant_t> m_InspectionResultValues;
	std::vector<_variant_t> m_InspectionResultTables;

	std::vector<RECT> m_aPreviousInputImageRect;


	// Result Images
	SvIe::SVImageClass m_aResultImages[SVExternalToolTaskData::NUM_RESULT_IMAGES];

	// Temporary Images to run slower and safer..........
	SvOi::SVImageBufferHandlePtr             m_aResultImagesCopy[SVExternalToolTaskData::NUM_RESULT_IMAGES];
	SvOi::SVImageBufferHandlePtr             m_aInputImagesCopy[SVExternalToolTaskData::NUM_INPUT_IMAGES];
	bool                     m_bUseImageCopies;
	
private:
	HRESULT collectInputImageNames( );
	
public:
};

} //namespace SvOp
#pragma warning (pop)
