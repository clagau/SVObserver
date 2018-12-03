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

HRESULT InspectionCommandsSynchronous(const SVGUID& rInspectionID, SvPb::InspectionCmdMsgs* pRequest, SvPb::InspectionCmdMsgs* pResponse)
{
	InspectionCommands_protoBufPtr CommandPtr = std::make_shared<InspectionCommands_protoBuf>(pRequest, pResponse);
	SVObjectSynchronousCommandTemplate<InspectionCommands_protoBufPtr> Command(rInspectionID, CommandPtr);

	HRESULT Result = Command.Execute(TWO_MINUTE_CMD_TIMEOUT);
	return Result;
}

HRESULT RunOnceSynchronous(const SVGUID& rInspectionID)
{
	SvPb::InspectionCmdMsgs Request;
	SvPb::InspectionRunOnceRequest* pInspectionRunOnceRequest = Request.mutable_inspectionrunoncerequest();

	SvPb::SetGuidInProtoBytes(pInspectionRunOnceRequest->mutable_inspectionid(), rInspectionID);
	return SvCmd::InspectionCommandsSynchronous(rInspectionID, &Request, nullptr);

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
	sourceInfo.set_objecttype(destInfo.ObjectType);
	sourceInfo.set_subtype(destInfo.SubType);
	SvPb::SetGuidInProtoBytes(sourceInfo.mutable_embeddedid(), destInfo.EmbeddedID);
}

} //namespace SvCmd