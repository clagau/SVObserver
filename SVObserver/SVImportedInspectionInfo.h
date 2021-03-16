//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc. Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImportedInspectionInfo
//* .File Name       : $Workfile:   SVImportedInspectionInfo.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:57:06  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectAttribute.h"
#include "Definitions/ObjectDefines.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#pragma endregion Includes

struct ImportedInput
{
	std::string m_name;
	std::string m_type;
	long m_ppqPosition{ -1L };
	_variant_t m_value;
	long m_index{ 0L };

	explicit ImportedInput(const std::string& rName, const std::string& rType, long ppqPos, const _variant_t& rValue, long index)
		: m_name{ rName }
		, m_type{ rType }
		, m_ppqPosition{ ppqPos }
		, m_value{ rValue }
		, m_index{ index }
	{}
	~ImportedInput() = default;
};
typedef std::vector<ImportedInput> ImportedInputList;

struct ImportedOutput
{
	std::string m_name;
	long m_subType{ 0L };
	long m_channel{ 0L };
	bool m_isCombined{ false };
	bool m_isAndAck{ false };

	explicit ImportedOutput(const std::string& rName, long subType, long channel, bool isCombined, bool isAndAck)
		: m_name{ rName }
		, m_subType{ subType }
		, m_channel{ channel }
		, m_isCombined {isCombined}
		, m_isAndAck{isAndAck}
	{}
	ImportedOutput() = default;
	~ImportedOutput() = default;
};
typedef std::vector<ImportedOutput> ImportedOutputList;


class SVImportedInspectionInfo
{
public:
	uint32_t m_inspectionId{ SvDef::InvalidObjectId };
	ImportedInputList m_inputList;
	ImportedOutputList m_outputList;
	ObjectAttributeList m_objectAttributeList;
	SVMaterialsTree m_materialsTree;

	SVImportedInspectionInfo() = default;
	~SVImportedInspectionInfo() = default;
};

typedef std::vector<SVImportedInspectionInfo> SVImportedInspectionInfoList;

