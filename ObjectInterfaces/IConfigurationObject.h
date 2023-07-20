///******************************************************************************
/// COPYRIGHT (c) 2023 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
///*****************************************************************************
/// IConfiguationManager is the interface to communicated with the configuration object.
///              By now there are only function to call intern static methods.
///******************************************************************************
#pragma once
namespace SvOi
{
class IObjectClass;
}

namespace SvPb
{
class InspectionCmdResponse;
}

class SVGUID;

namespace SvOi
{
using HistoryList = std::vector<std::pair<time_t, std::string>>;

/// Construct an instance of a module tool.
/// \param index [in] The index in the Module List. (It is identically to the position in the module add list.)
/// \returns SvOi::IObjectClass*
IObjectClass* ConstructAndAddModuleInstance(int index, uint32_t parentId);

SvPb::InspectionCmdResponse getModuleList();
SvPb::InspectionCmdResponse deleteModule(SVGUID moduleGuid);
SvPb::InspectionCmdResponse checkNewModuleName(const std::string& newName);
SvPb::InspectionCmdResponse convertGroupToModuleTool(uint32_t groupToolId, const std::string& moduleName);
SvPb::InspectionCmdResponse convertModuleToGroupTool(uint32_t moduleToolId);
SvPb::InspectionCmdResponse startEditModule(uint32_t moduleToolId);
SvPb::InspectionCmdResponse saveEditModule(uint32_t moduleToolId, const std::string& rChangeText);
SvPb::InspectionCmdResponse cancelEditModule(uint32_t moduleToolId);
SvPb::InspectionCmdResponse renameModule(SVGUID moduleGuid, const std::string& newName);
SvPb::InspectionCmdResponse importModule(const std::string& moduleName, const std::string& moduleContainerStr);
SvPb::InspectionCmdResponse exportModule(SVGUID moduleGuid);

void registerModuleInstance(SVGUID guid, uint32_t instanceId, const std::string& rComment, const HistoryList& rHistoryList, bool checkComment);
void unregisterModuleInstance(SVGUID guid, uint32_t instanceId);

std::string getModuleName(SVGUID guid);
} //namespace SvOi