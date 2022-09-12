//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file CommandExternalHelper.h
/// All Rights Reserved
//*****************************************************************************
/// This file contains the helper function for the extern use.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVProtoBuf/InspectionCommands.h"
#include "Definitions/ObjectDefines.h"
#pragma endregion Includes


#pragma region Declarations

namespace SvOi
{
/// struct  holds data for one TaskObject which are relevant 
/// for displaying the object in the navigator
struct ObjectInfo;
typedef std::vector<ObjectInfo> ObjectInfoVector;
}

namespace SvUl
{
typedef std::pair<std::string, uint32_t> NameObjectIdPair;
typedef std::vector<NameObjectIdPair> NameObjectIdList;
}

namespace SvDef
{
struct SVObjectTypeInfoStruct;
}
#pragma endregion Declarations

namespace SvCmd
{

enum class ThreadPref
{ 
	cur,
	inspection,
	async,
	threadDefault
};

/// Call an inspection command synchronous if required. (Send it to the inspection thread.)
/// \param inspectionID [in] id for the inspection to find the inspection thread.
/// \param rRequest [in] The request message.
/// \param pResponse [in, out] The response message (must fit to the request message). If request message has no response message, it should be nullptr.
///\param ThreadPref t  [in] default means use setting from InspectionCommandMap  
/// /// \returns HRESULT ErrorCode.
HRESULT InspectionCommands(uint32_t inspectionID, const SvPb::InspectionCmdRequest& rReq, SvPb::InspectionCmdResponse* pRes,ThreadPref t = ThreadPref::threadDefault);

HRESULT InspectionCommandsCheckState( const SvPb::InspectionCmdRequest& rRequest, SvPb::InspectionCmdResponse* pResp);


HRESULT RunOnceSynchronous(uint32_t inspectionID);

/// Returns the dotted name up to "Tool Set".
/// \param inspectionId [in] The inspectionId of this object.
/// \param objectId [in] The objectId as string possible without index.
/// \param objectIdString [in] The objectId as string possible with index.
/// \returns std::string
std::string getDottedName(uint32_t inspectionId, uint32_t objectId, bool includeToolSet = false);
std::string getDottedName(uint32_t inspectionId, std::string objectIdString);

///convert protobuf message to a vector of objectinfos
bool ResponseToObjectInfo(const SvPb::InspectionCmdResponse& rResponse, SvOi::ObjectInfoVector&  rToolSetInfos);

SvUl::NameObjectIdList convertNameObjectIdList(const ::google::protobuf::RepeatedPtrField< ::SvPb::ObjectNameIdPair >& rPbList);

void setTypeInfos(const SvDef::SVObjectTypeInfoStruct& destInfo, SvPb::SVObjectTypeInfoStruct& sourceInfo);

SvPb::GetObjectSelectorItemsRequest createObjectSelectorRequest(const std::vector<SvPb::SearchArea>& rSearchAreas, uint32_t inspectionID, SvPb::ObjectAttributes attribute, uint32_t instanceID = SvDef::InvalidObjectId, bool wholeArray = false, SvPb::ObjectSelectorType type = SvPb::allValueObjects, SvPb::GetObjectSelectorItemsRequest::FilterCase filter = SvPb::GetObjectSelectorItemsRequest::FilterCase::kAttributesAllowed, uint32_t importantObjectForStopAtClosed = SvDef::InvalidObjectId);
SvPb::GetObjectSelectorItemsRequest createObjectSelectorRequest(const std::vector<SvPb::SearchArea>& rSearchAreas, uint32_t inspectionID, SvPb::ObjectAttributes attribute, uint32_t instanceID, bool wholeArray, SvPb::ObjectSelectorType type, const std::vector<uint32_t>& excludeSameLineageVector, uint32_t importantObjectForStopAtClosed = SvDef::InvalidObjectId);

std::string getComment(uint32_t inspectionId, uint32_t objectId);
HRESULT setComment(uint32_t inspectionId, uint32_t objectId, std::string commentStr);

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
	auto iter = std::find_if(extents.begin(), extents.end(), [type](auto item) { return type == item.type(); });
	if (extents.end() != iter)
	{
		iter->set_value(value);
		return true;
	}
	return false;
}
} //namespace SvCmd