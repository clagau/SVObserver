//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file BasicStructureHelper.h
/// All Rights Reserved
//*****************************************************************************
/// This file contains the helper function for the extern use.
//******************************************************************************
#pragma once

#pragma region Includes
#include "BasicStructure.h"
#pragma endregion Includes

namespace SvPb
{
///Set a Guid to a bytes variable in a protobufmessage 
void SetGuidInProtoBytes(std::string  *pString, const GUID& guid);

///Get a Guid from a bytes variable in a protobufmessage. 
void GetGuidFromProtoBytes(const std::string& strguid, GUID& rGuid);

//!GUID From protobufmessage 
GUID GetGuidFromProtoBytes(const std::string& strguid);

} //namespace SvPb