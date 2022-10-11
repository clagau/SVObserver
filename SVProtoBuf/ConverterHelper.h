//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file ConverterHelper.h
/// All Rights Reserved
//*****************************************************************************
/// Converter helper functions between protobuf and other data types
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVProtoBuf/Variant.h"
#include "SVProtoBuf/SVRC.h"
#include "SVStatusLibrary/MessageTextEnum.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

namespace SvStl
{
class MessageContainer;
struct SourceFileParams;
typedef std::vector<MessageContainer> MessageContainerVector;
}

namespace SvPb
{
HRESULT ConvertVariantToProtobuf(const _variant_t& rVariant, SvPb::Variant* pPbVariant);
/// simpleType is required for the script tester (SVRemoteControl.ocx) which can only handle VT_I4 (signed) and VT_UI4 (unsigned) for all integer types
HRESULT ConvertProtobufToVariant(const SvPb::Variant& rPbVariant, _variant_t& rVariant, bool simpleType = false);

SvPb::MessageContainerVector createErrorMessages(uint32_t objectId, const SvStl::SourceFileParams& rSFParams, SvStl::MessageTextEnum textId, SvDef::StringVector additionalText = {});

// Converts a list into a protobuf Variant by generating a semicolon separated string (The list should not contain any ;)
void ConvertStringListToProtobuf(const std::set<std::string>& rList, SvPb::Variant* pVariant);

/// Set messages from SVObserver to a protobuf-structure.
/// \param messageVec [in] Messages
/// \returns SvPb::MessageContainerVector The protobuf-structure.
MessageContainerVector convertMessageVectorToProtobuf(const SvStl::MessageContainerVector& rMessageVec);
void convertMessageToProtobuf(const SvStl::MessageContainer& rMessage, MessageContainer* pMessagePB);

SvStl::MessageContainer convertProtobufToMessage(const MessageContainer& messagePB);
/// Convert a protobuf-structure to messages from SVObserver.
/// \param messageVecPB [in] The protobuf-structure.
/// \returns SvStl::MessageContainerVector The messages
SvStl::MessageContainerVector convertProtobufToMessageVector(const MessageContainerVector& rMessageVecPB);

// Converts a flat vector to a recursive tree
template<typename TreeItem>
void convertVectorToTree(const std::vector<TreeItem>& rItemVector, TreeItem* pTree);

/// Return the default string for the LinkedValueTypeEnum-value
/// \param type [in]
/// \returns variant_t
variant_t getDefaultString(SvPb::LinkedValueTypeEnum type);

/// Return a bool if a LinkedValueTypeEnum is a value type.
/// \param type [in]
/// \returns bool
constexpr bool isValueType(SvPb::LinkedValueTypeEnum type)
{
	return (SvPb::LinkedValueTypeEnum::TypeDecimal == type || SvPb::LinkedValueTypeEnum::TypeText == type);
}
} //namespace SvPb