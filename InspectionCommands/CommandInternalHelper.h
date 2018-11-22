//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file CommandInternalHelper.h
/// All Rights Reserved
//*****************************************************************************
/// This file contains the helper function for the internal use.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVProtoBuf/InspectionCommands.h"
#pragma endregion Includes

#pragma region Declarations
namespace SvOi
{
class IObjectClass;
}
#pragma endregion Declarations


namespace SvCmd
{

typedef std::function<bool(const SvOi::IObjectClass*, bool& bStop)> IsAllowedFunc;

IsAllowedFunc getAllowedFunc(const SvPb::GetAvailableObjectsRequest&);
} //namespace SvCmd