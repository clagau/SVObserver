//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file ConverterHelper.h
/// All Rights Reserved
//*****************************************************************************
/// Converter helper functions between protobuf and other data types
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVProtoBuf/Variant.h"
#include "SVProtoBuf/SVRC.h"
#pragma endregion Includes

namespace SvStl
{
class MessageContainer;
typedef std::vector<MessageContainer> MessageContainerVector;
}

namespace SvPb
{
HRESULT ConvertVariantToProtobuf(const _variant_t& rVariant, SvPb::Variant* pPbVariant);
/// simpleType is required for the script tester (SVRemoteControl.ocx) which can only handle VT_I4 (signed) and VT_UI4 (unsigned) for all integer types
HRESULT ConvertProtobufToVariant(const SvPb::Variant& rPbVariant, _variant_t& rVariant, bool simpleType = false);

// Converts a list into a protobuf Variant by generating a semicolon separated string (The list should not contain any ;)
void ConvertStringListToProtobuf(const std::set<std::string>& rList, SvPb::Variant* pVariant);

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