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
#include "SVHBitmapUtilitiesLibrary/SVHBitmapUtilities.h"
#include "InspectionEngine/SVTaskObject.h"
#include "InspectionEngine/SVImageClass.h"
#include "SVVariantResultClass.h"
#include "SVLibrary/ISVCancel.h"
#include "SVDllToolLoadLibraryClass.h"
#include "SVRPropertyTree/SVRPropTreeState.h"
#include "SVMatroxLibrary/SVMatroxTypedefs.h"
#include "SVValueObjectLibrary/LinkedValue.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

namespace SvOp
{
struct SVExternalToolTaskData : public SVCancelData
{
	enum
	{
		NUM_TOOL_DEPENDENCIES = 50,
		NUM_INPUT_OBJECTS = 50,
		NUM_RESULT_OBJECTS = 50,
		NUM_INPUT_IMAGES = 4,
		NUM_RESULT_IMAGES = 4,
	};

	SVExternalToolTaskData();
	SVExternalToolTaskData(const SVExternalToolTaskData& src);
	virtual ~SVExternalToolTaskData();

	const SVExternalToolTaskData& operator = (const SVExternalToolTaskData& rhs);

	SvVol::SVFileNameValueObjectClass m_voDllPath;
	std::vector<SvVol::SVFileNameValueObjectClass>  m_aDllDependencies; //[NUM_TOOL_DEPENDENCIES];

	// DLL info
	SvVol::SVStringValueObjectClass  m_voToolName;
	SvVol::SVLongValueObjectClass  m_voToolVersion;

	// Inputs
	std::vector<SvOl::SVInObjectInfoStruct> m_aInputImageInfo; //[NUM_INPUT_IMAGES]; // used to connect to other images
	std::vector<SvVol::LinkedValue> m_aInputObjects; //[NUM_INPUT_OBJECTS]; // our own value objects
	std::vector<SvVol::SVStringValueObjectClass>  m_aInputObjectNames; //[NUM_INPUT_OBJECTS]; // our value object names

	std::vector<SvVol::SVVariantValueObjectClass> m_aResultObjects; //[NUM_RESULT_OBJECTS];

	SVImageDefinitionStructArray m_aResultImageDefinitions;
	ResultValueDefinitionStructArray m_aResultValueDefinitions;
	InputValueDefinitionStructArray m_aInputValueDefinitions;

	SVMultiCancelData m_RangeResultData;

	long m_lNumInputImages;
	long m_lNumInputValues;
	long m_lNumResultImages;
	long m_lNumResultValues;

	SVRPropTreeState m_PropTreeState;
};	// end struct SVExternalToolTaskData

class SVExternalToolTask : public SvIe::SVTaskObjectListClass, public ISVCancel
{
	SV_DECLARE_CLASS( SVExternalToolTask )

public:
	SVExternalToolTask( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SV_EXTERNAL_TOOL_TASK );
	virtual ~SVExternalToolTask();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool CloseObject() override;
	virtual bool ConnectAllInputs() override;

	
	HRESULT Initialize(SVDllLoadLibraryCallback fnNotify = [](LPCTSTR) {});
	virtual HRESULT DisconnectInputsOutputs(SVObjectPtrVector& rListOfObjects) override;
	virtual HRESULT HideInputsOutputs(SVObjectPtrVector& rListOfObjects) override;

	HRESULT SetPathName( const std::string& rPath );
	HRESULT SetDependencies( const SvDef::StringVector& rDependencies );
	HRESULT GetResultImageDefinitions( SVImageDefinitionStructArray& raResultImageDefinitions );
	HRESULT GetResultValueDefinitions ( ResultValueDefinitionStructArray& raResultValueDefinitions );
	HRESULT GetInputValueDefinitions( InputValueDefinitionStructArray& raInputValueDefinitions );

	// ISVCancel interface
	virtual bool CanCancel() override;
	virtual HRESULT GetCancelData(SVCancelData*& rpData) override;
	virtual HRESULT SetCancelData(SVCancelData* pData) override;

	HRESULT GetImageInfo(const SVImageDefinitionStruct* pDefinitionStruct, SVImageInfoClass& rInfo);
	HRESULT GetImageDefinitionStruct( SVImageDefinitionStruct& rImageDef, const SVImageInfoClass& rInfo);

	void SetAllAttributes();
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	enum FindEnum
	{
		FIND_VALUES = 1,
		FIND_INPUT_IMAGES = 1 << 1,
		FIND_RESULT_IMAGES = 1 << 2,
		FIND_IMAGES = FIND_INPUT_IMAGES | FIND_RESULT_IMAGES,
		FIND_ALL_OBJECTS_EXP_INPUT_IMAGES = FIND_RESULT_IMAGES | FIND_VALUES, //This flag is to find all objects expect of the input images.
		FIND_ALL_OBJECTS = FIND_IMAGES | FIND_VALUES
	};
	HRESULT FindInvalidatedObjects(SVObjectPtrVector& rList, const SVCancelData* pOriginalData, FindEnum eWhich );
	void GetDLLMessageString(HRESULT hr, BSTR* bstrMessage) const;

#pragma region Methods to replace processMessage
	virtual bool DisconnectObjectInput(SvOl::SVInObjectInfoStruct* pObjectInInfo) override;
#pragma endregion Methods to replace processMessage

protected:
	HRESULT Uninitialize();
	HRESULT ClearData();
	HRESULT SetDefaultValues();
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
	HRESULT InspectionInputsToVariantArray();

	SvIe::SVImageClass* GetInputImage(int iIndex, bool bRunMode = false);
	SvIe::SVImageClass* GetResultImage(int iIndex);

	SvVol::SVVariantValueObjectClass* GetResultValueObject(int iIndex);
	SVResultClass* GetResultRangeObject(int iIndex);
	std::vector<SVResultClass*> GetResultRangeObjects();

	HRESULT AllocateResult (int iIndex);

private:
	SVDLLToolLoadLibraryClass m_dll;

	SVExternalToolTaskData m_Data;	// this is our cancelable data

	std::vector<long>      m_aInspectionInputImages;
	std::vector<long>      m_aInspectionResultImages;
	std::vector<SVDIBITMAPINFO> m_aInspectionInputHBMImages;
	std::vector<HBITMAP> m_aInspectionResultHBMImages;

	std::vector<_variant_t> m_aInspectionInputValues;
	std::vector<_variant_t> m_aInspectionResultValues;

	std::vector<RECT> m_aPreviousInputImageRect;

	// Result Images
	SvIe::SVImageClass m_aResultImages[SVExternalToolTaskData::NUM_RESULT_IMAGES];

	// Temperary Images to run slower and safer..........
	SvOi::SVImageBufferHandlePtr             m_aResultImagesCopy[SVExternalToolTaskData::NUM_RESULT_IMAGES];
	SvOi::SVImageBufferHandlePtr             m_aInputImagesCopy[SVExternalToolTaskData::NUM_INPUT_IMAGES];
	bool                     m_bUseImageCopies;
	
private:
	HRESULT CollectInputImageNames( );

public:
	friend class SVExternalToolDlg;
	friend class SVExternalToolInputSelectPage;
	friend class SVExternalToolResultPage;
};

} //namespace SvOp
#pragma warning (pop)