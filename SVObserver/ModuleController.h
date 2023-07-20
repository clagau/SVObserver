//*****************************************************************************
/// \copyright (c) 2022,2022 by Körber Pharma Inspection GmbH. All Rights Reserved
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
	void addModules(SVTreeType& rTree, const std::string& zipFilename);
	void saveModules(SvOi::IObjectWriter& rWriter) const;
	void saveModulesForIP(SvOi::IObjectWriter& rWriter, uint32_t inspectionId) const;

	//Attention: Throw exception in error case.
	void checkIfNameValid(const std::string& rName) const;

	//Attention: Throw exception in error case.
	void convertGroupTool(uint32_t toolId, const std::string& rName);
	void convertModuleInstance(uint32_t toolId);

	void startEditModule(uint32_t toolId);
	void saveEditModule(uint32_t toolId, const std::string& rChangeText);
	void cancelEditModule(uint32_t toolId);

	//Attention: Throw exception in error case.
	void registerInstance(SVGUID guid, uint32_t instanceId, const std::string& rComment, const SvOi::HistoryList& rHistoryList, bool checkComment);
	void unregisterInstance(SVGUID guid, uint32_t instanceId);

	SvOi::IObjectClass* constructAndAddModuleInstance(int index, uint32_t parentId);
	SvPb::InspectionCmdResponse getModuleListResp() const;
	void deleteModule(SVGUID guid);

	//Attention: Throw exception in error case.
	void renameModule(SVGUID guid, const std::string& newName);
	std::string getModuleName(SVGUID guid) const;

	void importModule(const std::string& moduleName, const std::string& moduleContainerStr);
	SvPb::InspectionCmdResponse exportModule(SVGUID moduleGuid) const;

	std::vector<ModuleData> getModuleList() { return m_moduleList; };

private:
	std::vector<ModuleData> m_moduleList;
	std::string m_xmlFilePath;
};