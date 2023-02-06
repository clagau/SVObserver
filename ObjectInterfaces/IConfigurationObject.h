///******************************************************************************
/// COPYRIGHT (c) 2023 by K�rber Pharma Inspection GmbH. All Rights Reserved
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

void registerModuleInstance(SVGUID guid, uint32_t instanceId, const std::string& rComment, const HistoryList& rHistoryList);
void unregisterModuleInstance(SVGUID guid, uint32_t instanceId);

std::string getModuleName(SVGUID guid);
} //namespace SvOi