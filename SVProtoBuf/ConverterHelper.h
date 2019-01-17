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
#pragma endregion Includes


namespace SvPb
{
///Set a Guid to a bytes variable in a protobufmessage 
void SetGuidInProtoBytes(std::string  *pString, const GUID& guid);

///Get a Guid from a bytes variable in a protobufmessage. 
void GetGuidFromProtoBytes(const std::string& strguid, GUID& rGuid);

//!GUID From protobufmessage 
GUID GetGuidFromProtoBytes(const std::string& strguid);

HRESULT ConvertVariantToProtobuf(const _variant_t& rVariant, SvPb::Variant* pPbVariant);
/// simpleType is required for the script tester (SVRemoteControl.ocx) which can only handle VT_I4 (signed) and VT_UI4 (unsigned) for all integer types
HRESULT ConvertProtobufToVariant(const SvPb::Variant& rPbVariant, _variant_t& rVariant, bool simpleType = false);

// Converts a list into a protobuf Variant by generating a semicolon separated string (The list should not contain any ;)
void ConvertStringListToProtobuf(const SvDef::StringSet& rList, SvPb::Variant* pVariant);

// Converts a flat vector to a recursive tree
void convertVectorToTree(const std::vector<SvPb::TreeItem>& rItemVector, SvPb::TreeItem* pTree);
} //namespace SvPb