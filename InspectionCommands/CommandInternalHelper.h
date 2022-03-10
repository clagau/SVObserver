//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file CommandInternalHelper.h
/// All Rights Reserved
//*****************************************************************************
/// This file contains the helper function for the internal use.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVProtoBuf/SVO-Enum.h"
#pragma endregion Includes

#pragma region Declarations
namespace SvDef
{
struct SVObjectTypeInfoStruct;
}

namespace SvOi
{
class IObjectClass;
class IInspectionProcess;
}

namespace SvPb
{
class TreeItem;
class TreeSearchParameters;
class GetObjectSelectorItemsRequest;
class InspectionCmdResponse;
}

#pragma endregion Declarations


namespace SvCmd
{

typedef std::function<bool(const SvOi::IObjectClass*, bool& bStop)> IsAllowedFunc;
typedef std::function<bool(const SvOi::IObjectClass* pObject, unsigned int Attribute, int ArrayIndex)> IsObjectInfoAllowed;

IsAllowedFunc getAllowedFunc(const SvPb::TreeSearchParameters&);

IsObjectInfoAllowed getObjectSelectorFilterFunc(const SvPb::GetObjectSelectorItemsRequest& rRequest);

SvPb::InspectionCmdResponse getAvailableObjects(uint32_t startingObjectId, const SvDef::SVObjectTypeInfoStruct& typeInfo, bool addHidden, bool stopIfClosed, SvPb::SVObjectTypeEnum desiredFirstObjectTypeForName, bool excludeFirstObjectNameInName, IsAllowedFunc isAllowedFunc);
std::pair<uint32_t, bool> correctStartId(uint32_t preSearchStartId, uint32_t startingObjectId);
} //namespace SvCmd