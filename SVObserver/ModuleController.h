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
#pragma endregion Includes

struct ModuleData
{
	std::string m_name;
	time_t m_lastModify;
	SVGUID m_guid;
};

class ModuleController
{
public:
	ModuleController();
	~ModuleController() = default;

	void loadModules();
	void saveModules();

	//Attention: Throw exception in error case.
	void checkIfNameValid(const std::string& rName) const;

	//Attention: Throw exception in error case.
	void convertGroupTool(uint32_t toolId, const std::string& rName);

private:
	std::vector<ModuleData> m_moduleList;
	std::string m_xmlFilePath;
};