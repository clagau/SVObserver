//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// ExternalToolTaskController declaration. Provides helper functions to access a specific ExternalToolTask.
//**************

#pragma once

#include "SVProtoBuf/InspectionCommands.h"
#include <map>

class ExternalToolTaskController
{
public:
	enum
	{
		NUM_TOOL_DEPENDENCIES = 50,
		NUM_INPUT_OBJECTS = 50,
		NUM_RESULT_OBJECTS = 50,
		NUM_INPUT_IMAGES = 4,
		NUM_RESULT_IMAGES = 4,
		NUM_RESULT_TABLE_OBJECTS = 4,
	};

	ExternalToolTaskController() = delete;
	ExternalToolTaskController(uint32_t inspectionId, uint32_t ownerId);


	HRESULT setExternalToolTaskId();

	HRESULT initialize(bool inCreationProcess = false, bool initializeAll = false);
	HRESULT initialize(SvPb::InitializeExternalToolTaskResponse& response, bool inCreationProcess = false, bool initializeAll = false);
	bool resetAllObjects();
	HRESULT clearData();
	HRESULT setAllAttributes();

	SvPb::GetInputValuesDefinitionExternalToolResponse getInputValuesDefinition() const;
	long getNumInputValues() const;

	std::map<std::string, bool> getPropTreeState() const;
	HRESULT setPropTreeState(const std::map<std::string, bool>&);

	HRESULT validateValueParameter(uint32_t laskObjectId, long index, _variant_t newVal);
	std::string getDllMessageString(long hResultError) const;

	// results
	SvPb::GetResultValuesDefinitionExternalToolResponse getResultValuesDefinition() const;
	long getNumResultValues() const;
	SvPb::GetTableResultsExternalToolResponse getTableResults() const;
	SvPb::GetResultRangeObjectResponse getResultRangeObjectAtIndex(int index) const;
	SvPb::GetImageInfoExternalToolResponse getInputImageInfoList() const;

	uint32_t getExternalToolTaskObjectId() const;

	SvPb::ValidateValueObjectResponse isValueObjectValid(const std::string& dottedName, const SvPb::InputValueDefinition& valueDefinition) const;
private:
	uint32_t m_inspectionId;
	uint32_t m_objectId;
	uint32_t m_ownerId;

};

