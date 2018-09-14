//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file ConverterHelper.h
/// All Rights Reserved
//*****************************************************************************
/// Converter helper functions between protobuf and other data types
//******************************************************************************
#pragma once

#pragma region Includes
#include "Definitions/StringTypeDef.h"
#include "Variant.h"
#include "SVRC.h"
#include "SVStatusLibrary/svModeEnum.h"
#include "SVStatusLibrary/NotificationTypeEnum.h"
#pragma endregion Includes


namespace SvPb
{
///Set a Guid to a bytes variable in a protobufmessage 
void SetGuidInProtoBytes(std::string  *pString, const GUID& guid);

///Get a Guid from a bytes variable in a protobufmessage. 
void GetGuidFromProtoBytes(const std::string& strguid, GUID& rGuid);

//!GUID From protobufmessage 
GUID GetGuidFromProtoBytes(const std::string& strguid);

HRESULT ConvertVariantToProtobuf(const _variant_t& rPbVariant, int& rCount, SvPb::Variant* pVariant);
HRESULT ConvertProtobufToVariant(const SvPb::Variant& rPbVariant, int Count, _variant_t& rVariant);

// Converts a list into a protobuf Variant by generating a semicolon separated string (The list should not contain any ;)
int ConvertStringListToProtobuf(const SvDef::StringSet& rList, SvPb::Variant* pVariant);

svModeEnum PbDeviceMode_2_SVIMMode(DeviceModeType type);
DeviceModeType  SVIMMode_2_PbDeviceMode(unsigned long Mode);

UINT PbObjectAttributes2Attributes(const SvPb::ObjectAttributes& rAttributes);
} //namespace SvPb