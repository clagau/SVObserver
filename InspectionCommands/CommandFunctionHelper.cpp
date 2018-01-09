//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file CommandFunctionHelper.cpp
/// All Rights Reserved
//*****************************************************************************
/// This file contains the helper function for the extern use.
//******************************************************************************
#pragma region Includes
#include "StdAfx.h"
#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"
#include "CommandFunctionHelper.h"
#include "InspectionCommand_protoBuf.h"
#pragma endregion Includes

namespace SvCmd
{
	HRESULT InspectionCommandsSynchronous(const SVGUID& rInspectionID, const google::protobuf::Message* pRequest, google::protobuf::Message* pResponse)
	{
		InspectionCommands_protoBufPtr CommandPtr = InspectionCommands_protoBufPtr(new InspectionCommands_protoBuf(pRequest, pResponse));
		SVObjectSynchronousCommandTemplate< InspectionCommands_protoBufPtr > Command(rInspectionID, CommandPtr);

		HRESULT Result = Command.Execute(TWO_MINUTE_CMD_TIMEOUT);
		return Result;
	}

	SvPB::MessageContainerVector setMessageContainerToMessagePB(const SvStl::MessageContainerVector& messageContainers)
	{
		SvPB::MessageContainerVector messagePB;
		for (auto messageContainer : messageContainers)
		{
			auto messageData = messageContainer.getMessage();
			SvPB::MessageContainer* pMessageContainerPB = messagePB.add_messages();
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
			pMessageContainerPB->mutable_objectid()->CopyFrom(SvPB::setGuidToMessage(messageContainer.getObjectId()));
		}
		return messagePB;
	}

	SvStl::MessageContainerVector setMessageContainerFromMessagePB(const SvPB::MessageContainerVector& messagesPB)
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
			SvStl::MessageContainer messageContainer(messagePB.messagecode(), static_cast<SvStl::MessageTextEnum>(messagePB.additionaltextid()), AdditionalTextList, fileParam, 0, SvPB::getGuidFromMessage(messagePB.objectid()));
			messageContainerVector.push_back(messageContainer);
		}

		return messageContainerVector;
	}
} //namespace SvCmd