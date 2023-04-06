//******************************************************************************
/// COPYRIGHT (c) 2015 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// ExternalToolTaskController declaration. Provides helper functions to access a specific ExternalToolTask.
//**************

#pragma once

#include "SVProtoBuf/InspectionCommands.h"
#include <map>


namespace SvStl
{
	class MessageContainer;
	typedef std::vector<MessageContainer> MessageContainerVector;
}

namespace SvOgu
{
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

	/// initialize the External Dll. In error case it can throw an exception
	std::pair<HRESULT, SvPb::InitializeExternalToolTaskResponse> initialize(bool initializeall);
	std::pair<bool, std::string> resetAllObjects(bool showFirstError);
	HRESULT runOnce();

	HRESULT clearData();
	HRESULT setAllAttributes();

	SvPb::GetInputValuesDefinitionExternalToolResponse getInputValuesDefinition() const;
	long getNumInputValues() const;

	std::map<std::string, bool> getPropTreeState() const;
	HRESULT setPropTreeState(const std::map<std::string, bool>&);

	///encapsulates the protobuf communication to check the correctness of an external DLL input parameter
	bool validateValueParameterWrapper(uint32_t laskObjectId, long index, _variant_t newVal, SvPb::ExDllInterfaceType type);

	std::string getDllMessageString(long hResultError) const;

	// results
	SvPb::GetResultValuesDefinitionExternalToolResponse getResultValuesDefinition() const;
	long getNumResultValues() const;
	SvPb::GetTableResultsExternalToolResponse getTableResults() const;
	SvPb::GetResultRangeObjectResponse getResultRangeObjectAtIndex(int index) const;
	SvPb::GetImageInfoExternalToolResponse getInputImageInfoList() const;

	uint32_t getExternalToolTaskObjectId() const;

private:
	uint32_t m_inspectionId;
	uint32_t m_objectId;
	uint32_t m_ownerId;

};
}//namespace SvOgu
