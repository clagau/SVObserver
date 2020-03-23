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
#include <comutil.h>
#pragma endregion Includes

namespace SvStl
{
class MessageContainer;
typedef std::vector<MessageContainer> MessageContainerVector;
}

namespace SvPb
{
///Set a Guid to a bytes variable in a protobufmessage 
void SetGuidInProtoBytes(std::string  *pString, const GUID& guid);
std::string SetGuidInProtoBytes(const GUID& guid);

///Get a Guid from a bytes variable in a protobufmessage. 
void GetGuidFromProtoBytes(const std::string& strguid, GUID& rGuid);

//!GUID From protobufmessage 
GUID GetGuidFromProtoBytes(const std::string& strguid);

// allows passing binary representation of the guid and also
// the string representation of it.
GUID GetGuidFromString(const std::string& strguid);

std::string PrettyPrintGuid(const GUID& guid);

HRESULT ConvertVariantToProtobuf(const _variant_t& rVariant, SvPb::Variant* pPbVariant);
/// simpleType is required for the script tester (SVRemoteControl.ocx) which can only handle VT_I4 (signed) and VT_UI4 (unsigned) for all integer types
HRESULT ConvertProtobufToVariant(const SvPb::Variant& rPbVariant, _variant_t& rVariant, bool simpleType = false);

// Converts a list into a protobuf Variant by generating a semicolon separated string (The list should not contain any ;)
void ConvertStringListToProtobuf(const SvDef::StringSet& rList, SvPb::Variant* pVariant);

/// Set messages from SVObserver to a protobuf-structure.
/// \param messageVec [in] Messages
/// \returns SvPb::MessageContainerVector The protobuf-structure.
MessageContainerVector setMessageVectorToMessagePB(const SvStl::MessageContainerVector& rMessageVec);
void setMessageToMessagePB(const SvStl::MessageContainer& rMessage, MessageContainer* pMessagePB);
/// Convert a protobuf-structure to messages from SVObserver.
/// \param messageVecPB [in] The protobuf-structure.
/// \returns SvStl::MessageContainerVector The messages
SvStl::MessageContainerVector setMessageVectorFromMessagePB(const MessageContainerVector& rMessageVecPB);

// Converts a flat vector to a recursive tree
template<typename TreeItem>
void convertVectorToTree(const std::vector<TreeItem>& rItemVector, TreeItem* pTree);
} //namespace SvPb