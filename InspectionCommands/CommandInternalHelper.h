//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file CommandInternalHelper.h
/// All Rights Reserved
//*****************************************************************************
/// This file contains the helper function for the internal use.
//******************************************************************************
#pragma once

#pragma region Includes
#pragma endregion Includes

#pragma region Declarations
namespace SvOi
{
class IObjectClass;
}

namespace SvPb
{
class GetAvailableObjectsRequest;
class GetObjectSelectorItemsRequest;
}

#pragma endregion Declarations


namespace SvCmd
{

typedef std::function<bool(const SvOi::IObjectClass*, bool& bStop)> IsAllowedFunc;
typedef std::function<bool(const SvOi::IObjectClass* pObject, unsigned int Attribute, int ArrayIndex)> IsObjectInfoAllowed;

IsAllowedFunc getAllowedFunc(const SvPb::GetAvailableObjectsRequest&);

IsObjectInfoAllowed getObjectSelectorFilterFunc(const SvPb::GetObjectSelectorItemsRequest& rRequest);
} //namespace SvCmd