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
#include "SVProtoBuf\ConverterHelper.h"
#include "SVProtoBuf/InspectionCommands.h"
#include "SVStatusLibrary\MessageContainer.h"
#include "SVUtilityLibrary/NameGuidList.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "Definitions/SVObjectTypeInfoStruct.h"
#pragma endregion Includes


namespace SvCmd
{

HRESULT InspectionCommands(const SVGUID& rInspectionID, const SvPb::InspectionCmdMsgs& rRequest, SvPb::InspectionCmdMsgs* pResponse)
{
	InspectionCmdResult result;
	result.m_hResult = E_FAIL;

	switch (rRequest.message_case())
	{
		//GetObjectID does not need to be called synchronously
		case SvPb::InspectionCmdMsgs::kGetObjectIdRequest:
			result = getObjectId(rRequest.getobjectidrequest());
			break;
		default:
			InspectionCommands_protoBufPtr CommandPtr = std::make_shared<InspectionCommands_protoBuf>(rRequest);
			SVObjectSynchronousCommandTemplate<InspectionCommands_protoBufPtr> Command(rInspectionID, CommandPtr);
			Command.Execute(TWO_MINUTE_CMD_TIMEOUT);
			result = CommandPtr->getResult();
			break;
	}

	if (nullptr != pResponse)
	{
		*pResponse = result.m_response;
	}


	return result.m_hResult;
}

HRESULT RunOnceSynchronous(const SVGUID& rInspectionID)
{
	SvPb::InspectionCmdMsgs Request;
	SvPb::InspectionRunOnceRequest* pInspectionRunOnceRequest = Request.mutable_inspectionrunoncerequest();

	SvPb::SetGuidInProtoBytes(pInspectionRunOnceRequest->mutable_inspectionid(), rInspectionID);
	return SvCmd::InspectionCommands(rInspectionID, Request, nullptr);

}


SvPb::MessageContainerVector setMessageContainerToMessagePB(const SvStl::MessageContainerVector& messageContainers)
{
	SvPb::MessageContainerVector messagePB;
	for (auto messageContainer : messageContainers)
	{
		auto messageData = messageContainer.getMessage();
		SvPb::MessageContainer* pMessageContainerPB = messagePB.add_messages();
		pMessageContainerPB->set_messagecode(messageData.m_MessageCode);
		pMessageContainerPB->set_additionaltextid(messageData.m_AdditionalTextId);
		for (auto text : messageData.m_AdditionalTextList)
		{
			pMessageContainerPB->add_additionaltextlist(text);
		}
		pMessageContainerPB->set_compiledate(messageData.m_SourceFile.m_CompileDate);
		pMessageContainerPB->set_compiletime(messageData.m_SourceFile.m_CompileTime);
		pMessageContainerPB->set_filename(messageData.m_SourceFile.m_FileName);
		pMessageContainerPB->set_fileline(messageData.m_SourceFile.m_Line);
		pMessageContainerPB->set_filedatetime(messageData.m_SourceFile.m_FileDateTime);
		SvPb::SetGuidInProtoBytes(pMessageContainerPB->mutable_objectid(), messageContainer.getObjectId());
	}
	return messagePB;
}

SvStl::MessageContainerVector setMessageContainerFromMessagePB(const SvPb::MessageContainerVector& messagesPB)
{
	SvStl::MessageContainerVector messageContainerVector;
	for (auto messagePB : messagesPB.messages())
	{
		SvStl::SourceFileParams fileParam(messagePB.compiledate().c_str(), messagePB.compiletime().c_str(), messagePB.filename().c_str(), messagePB.fileline(), messagePB.filedatetime().c_str());
		SvDef::StringVector AdditionalTextList;
		for (auto text : messagePB.additionaltextlist())
		{
			AdditionalTextList.push_back(text);
		}
		SvStl::MessageContainer messageContainer(messagePB.messagecode(), static_cast<SvStl::MessageTextEnum>(messagePB.additionaltextid()), AdditionalTextList, fileParam, 0, SvPb::GetGuidFromProtoBytes(messagePB.objectid()));
		messageContainerVector.push_back(messageContainer);
	}
	return messageContainerVector;
}

bool ResponseToObjectInfo(const SvPb::InspectionCmdMsgs& rResponse, SvOi::ObjectInfoVector&  rToolSetInfos)
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
		SvPb::GetGuidFromProtoBytes(rTOI.taskobjectid(), objectInfo.guid);
		objectInfo.isValid = rTOI.isvalid();
		objectInfo.ObjectSubType = rTOI.objectsubtype();
		objectInfo.ObjectType = rTOI.objecttype();
		rToolSetInfos.push_back(objectInfo);
	}
	return true;
}

SvUl::NameGuidPair convertNameGuidPair(const SvPb::ObjectNameGuidPair& rPbPair)
{
	return {rPbPair.objectname(), SvPb::GetGuidFromProtoBytes(rPbPair.objectid())};
}

SvUl::NameGuidList convertNameGuidList(const ::google::protobuf::RepeatedPtrField< ::SvPb::ObjectNameGuidPair >& rPbList)
{
	SvUl::NameGuidList list;
	for (auto& rEntry : rPbList)
	{
		list.push_back(convertNameGuidPair(rEntry));
	}
	return list;
}

void setTypeInfos(const SvDef::SVObjectTypeInfoStruct& destInfo, SvPb::SVObjectTypeInfoStruct& sourceInfo)
{
	sourceInfo.set_objecttype(destInfo.m_ObjectType);
	sourceInfo.set_subtype(destInfo.m_SubType);
	sourceInfo.set_embeddedid(destInfo.m_EmbeddedID);
}

SvPb::GetObjectSelectorItemsRequest createObjectSelectorRequest(const std::vector<SvPb::ObjectSelectorType>& rItemTypes, const GUID& rInspectionID, SvPb::ObjectAttributes attribute, const GUID& rInstanceID /*= GUID_NULL*/, bool wholeArray /*= false*/, SvPb::SelectorFilter filter /*= SvPb::SelectorFilter::attributesAllowed*/)
{
	SvPb::GetObjectSelectorItemsRequest result;

	for (const auto& rType : rItemTypes)
	{
		result.add_types(rType);
	}
	SvPb::SetGuidInProtoBytes(result.mutable_inspectionid(), rInspectionID);
	SvPb::SetGuidInProtoBytes(result.mutable_instanceid(), rInstanceID);
	result.set_attribute(attribute);
	result.set_wholearray(wholeArray);
	result.set_filter(filter);
	return result;
}
} //namespace SvCmd