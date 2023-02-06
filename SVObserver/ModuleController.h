//*****************************************************************************
/// \copyright (c) 2022,2022 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file ModuleController.h
/// All Rights Reserved 
//*****************************************************************************
/// Controls all modules which are in a configuration.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVUtilityLibrary/SVGUID.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "ObjectInterfaces/IConfigurationObject.h"
#pragma endregion Includes

namespace SvOi
{
class IObjectClass;
class IObjectWriter;
}

namespace SvPb
{
class InspectionCmdResponse;
}

struct ModuleData
{
	std::string m_name;
	SvOi::HistoryList m_historyList;
	std::string m_comment;
	SVGUID m_guid;
	std::vector<uint32_t> m_moduleInstanceIds;
};

class ModuleController
{
public:
	ModuleController();
	~ModuleController() = default;

	void loadModules(SVTreeType& rTree);
	void saveModules(SvOi::IObjectWriter& rWriter) const;

	//Attention: Throw exception in error case.
	void checkIfNameValid(const std::string& rName) const;

	//Attention: Throw exception in error case.
	void convertGroupTool(uint32_t toolId, const std::string& rName);
	void convertModuleInstance(uint32_t toolId);

	//Attention: Throw exception in error case.
	void registerInstance(SVGUID guid, uint32_t instanceId, const std::string& rComment, const SvOi::HistoryList& rHistoryList);
	void unregisterInstance(SVGUID guid, uint32_t instanceId);

	SvOi::IObjectClass* constructAndAddModuleInstance(int index, uint32_t parentId);
	SvPb::InspectionCmdResponse getModuleListResp();
	SvPb::InspectionCmdResponse deleteModule(SVGUID guid);

	std::string getModuleName(SVGUID guid);

	std::vector<ModuleData> getModuleList() { return m_moduleList; };

private:
	std::vector<ModuleData> m_moduleList;
	std::string m_xmlFilePath;
};