//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file CommandExternalHelper.h
/// All Rights Reserved
//*****************************************************************************
/// This file contains the helper function for the extern use.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVProtoBuf/InspectionCommands.h"
#pragma endregion Includes

#pragma region Declarations
class SVGUID;

namespace SvStl
{
class MessageContainer;
typedef std::vector<MessageContainer> MessageContainerVector;
}

namespace SvOi
{
/// struct  holds data for one TaskObject which are relevant 
/// for displaying the object in the navigator
struct ObjectInfo;
typedef std::vector<ObjectInfo> ObjectInfoVector;
}

namespace SvUl
{
typedef std::pair<std::string, SVGUID> NameGuidPair;
typedef std::vector<NameGuidPair> NameGuidList;
}

namespace SvDef
{
struct SVObjectTypeInfoStruct;
}
#pragma endregion Declarations

namespace SvCmd
{
/// Call an inspection command synchronous if required. (Send it to the inspection thread.)
/// \param rInspectionID [in] Guid for the inspection to find the inspection thread.
/// \param rRequest [in] The request message.
/// \param pResponse [in, out] The response message (must fit to the request message). If request message has no response message, it should be nullptr.
/// \returns HRESULT ErrorCode.
HRESULT InspectionCommands(const SVGUID& rInspectionID, const SvPb::InspectionCmdMsgs& rRequest, SvPb::InspectionCmdMsgs* pResponse);

HRESULT RunOnceSynchronous(const SVGUID& rInspectionID);

/// Set messages from SVObserver to a protobuf-structure.
/// \param messageContainers [in] Messages
/// \returns SvCmd::MessageContainerVector The protobuf-structure.
SvPb::MessageContainerVector setMessageContainerToMessagePB(const SvStl::MessageContainerVector& messageContainers);
/// Convert a protobuf-structure to messages from SVObserver.
/// \param messagesPB [in] The protobuf-structure.
/// \returns SvStl::MessageContainerVector The messages
SvStl::MessageContainerVector setMessageContainerFromMessagePB(const SvPb::MessageContainerVector& messagesPB);

///convert protobuf message to a vector of objectinfos
bool ResponseToObjectInfo(const SvPb::InspectionCmdMsgs& rResponse, SvOi::ObjectInfoVector&  rToolSetInfos);

SvUl::NameGuidPair convertNameGuidPair(const SvPb::ObjectNameGuidPair& rPbPair);

SvUl::NameGuidList convertNameGuidList(const ::google::protobuf::RepeatedPtrField< ::SvPb::ObjectNameGuidPair >& rPbList);

void setTypeInfos(const SvDef::SVObjectTypeInfoStruct& destInfo, SvPb::SVObjectTypeInfoStruct& sourceInfo);

SvPb::GetObjectSelectorItemsRequest createObjectSelectorRequest(const std::vector<SvPb::ObjectSelectorType>& rItemTypes, const GUID& rInspectionID, SvPb::ObjectAttributes attribute, const GUID& rInstanceID = GUID_NULL, bool wholeArray = false, SvPb::SelectorFilter filter = SvPb::SelectorFilter::attributesAllowed);

template<class T>
T getValueForProperties(const ::google::protobuf::RepeatedPtrField< ::SvPb::ExtentParameter >& extents, SvPb::SVExtentPropertyEnum type)
{
	T retValue = 0;
	auto iter = std::find_if(extents.begin(), extents.end(), [type](auto item) { return type == item.type(); });
	if (extents.end() != iter)
	{
		retValue = static_cast<T>(iter->value());
	}
	return retValue;
}

template<class T>
bool setValueForProperties(::google::protobuf::RepeatedPtrField< ::SvPb::ExtentParameter >& extents, SvPb::SVExtentPropertyEnum type, T value)
{
	T retValue = 0;
	auto& iter = std::find_if(extents.begin(), extents.end(), [type](auto item) { return type == item.type(); });
	if (extents.end() != iter)
	{
		iter->set_value(value);
		return true;
	}
	return false;
}
} //namespace SvCmd