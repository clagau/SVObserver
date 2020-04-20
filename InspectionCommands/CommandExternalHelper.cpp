//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file CommandExternalHelper.cpp
/// All Rights Reserved
//*****************************************************************************
/// This file contains the helper function for the extern use.
//******************************************************************************
#pragma region Includes
#include "StdAfx.h"
#include "CommandExternalHelper.h"
#include "InspectionCommand_protoBuf.h"
#include "ObjectInterfaces/ObjectInfo.h"
#include "SVCommandLibrary/SVObjectSynchronousCommandTemplate.h"
#include "SVProtoBuf/InspectionCommands.h"
#include "SVStatusLibrary\MessageContainer.h"
#include "SVUtilityLibrary/NameObjectIdList.h"
#include "Definitions/SVObjectTypeInfoStruct.h"
#pragma endregion Includes

namespace SvCmd
{
HRESULT InspectionCommands(uint32_t inspectionID, const SvPb::InspectionCmdRequest& rRequest, SvPb::InspectionCmdResponse* pResponse)
{
	SvPb::InspectionCmdResponse response;
	response.set_hresult(E_FAIL);

	switch (rRequest.message_case())
	{
		//GetObjectID does not need to be called synchronously
		case SvPb::InspectionCmdRequest::kGetObjectIdRequest:
			response = getObjectId(rRequest.getobjectidrequest());
			break;
		default:
			InspectionCommands_protoBufPtr CommandPtr = std::make_shared<InspectionCommands_protoBuf>(rRequest);
			SVObjectSynchronousCommandTemplate<InspectionCommands_protoBufPtr> Command(inspectionID, CommandPtr);
			Command.Execute(TWO_MINUTE_CMD_TIMEOUT);
			response = CommandPtr->getResponse();
			break;
	}

	if (nullptr != pResponse)
	{
		*pResponse = response;
	}


	return response.hresult();
}

HRESULT RunOnceSynchronous(uint32_t inspectionID)
{
	SvPb::InspectionCmdRequest requestCmd;
	auto* pRequest = requestCmd.mutable_inspectionrunoncerequest();
	pRequest->set_inspectionid(inspectionID);
	return SvCmd::InspectionCommands(inspectionID, requestCmd, nullptr);
}

bool ResponseToObjectInfo(const SvPb::InspectionCmdResponse& rResponse, SvOi::ObjectInfoVector&  rToolSetInfos)
{
	if (false == rResponse.has_taskobjectlistresponse())
	{
		return false;
	}

	for (int t = 0; t < rResponse.taskobjectlistresponse().taskobjectinfos_size(); t++)
	{
		auto rTOI = rResponse.taskobjectlistresponse().taskobjectinfos(t);
		SvOi::ObjectInfo objectInfo;
		objectInfo.DisplayName = rTOI.displayname().c_str();
		objectInfo.m_objectId = rTOI.taskobjectid();
		objectInfo.isValid = rTOI.isvalid();
		objectInfo.ObjectSubType = rTOI.objectsubtype();
		objectInfo.ObjectType = rTOI.objecttype();
		rToolSetInfos.push_back(objectInfo);
	}
	return true;
}

SvUl::NameObjectIdList convertNameObjectIdList(const ::google::protobuf::RepeatedPtrField< ::SvPb::ObjectNameIdPair >& rPbList)
{
	SvUl::NameObjectIdList list;
	for (const auto& rEntry : rPbList)
	{
		list.emplace_back(rEntry.objectname(), rEntry.objectid());
	}
	return list;
}

void setTypeInfos(const SvDef::SVObjectTypeInfoStruct& destInfo, SvPb::SVObjectTypeInfoStruct& sourceInfo)
{
	sourceInfo.set_objecttype(destInfo.m_ObjectType);
	sourceInfo.set_subtype(destInfo.m_SubType);
	sourceInfo.set_embeddedid(destInfo.m_EmbeddedID);
}

SvPb::GetObjectSelectorItemsRequest createObjectSelectorRequest(const std::vector<SvPb::ObjectSelectorType>& rItemTypes, uint32_t inspectionID, SvPb::ObjectAttributes attribute, uint32_t instanceID /*= 0*/, bool wholeArray /*= false*/, SvPb::SelectorFilter filter /*= SvPb::SelectorFilter::attributesAllowed*/)
{
	SvPb::GetObjectSelectorItemsRequest result;

	for (const auto& rType : rItemTypes)
	{
		result.add_types(rType);
	}
	result.set_inspectionid(inspectionID);
	result.set_instanceid(instanceID);
	result.set_attribute(attribute);
	result.set_wholearray(wholeArray);
	result.set_filter(filter);
	return result;
}
} //namespace SvCmd