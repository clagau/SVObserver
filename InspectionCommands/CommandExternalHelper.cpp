//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file CommandExternalHelper.cpp
/// All Rights Reserved
//*****************************************************************************
/// This file contains the helper function for the extern use.
//******************************************************************************
#pragma region Includes
#include "StdAfx.h"
#include "CommandExternalHelper.h"
#include "ObjectInterfaces/ObjectInfo.h"
#include "SVProtoBuf/InspectionCommands.h"
#include "SVUtilityLibrary/NameObjectIdList.h"
#include "Definitions/SVObjectTypeInfoStruct.h"
#include "ObjectInterfaces/ICommand.h"
#include "InspectionCommandMap.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "ObjectInterfaces/IObjectClass.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManagerHelper.h"
#pragma endregion Includes

#ifdef _DEBUG
#define TRACE_CATCH true
#endif 



namespace SvCmd
{

HRESULT InspectionCommandsCheckState(const SvPb::InspectionCmdRequest& rRequest, SvPb::InspectionCmdResponse* pResp)
{
	SvPb::InspectionCmdResponse response;
	SvPb::InspectionCmdResponse* pResponse = (nullptr != pResp) ? pResp : &response;
	pResponse->set_hresult(E_FAIL);
	auto it = InspectionCommandMap.find(rRequest.message_case());
	if (it != InspectionCommandMap.end())
	{
		DWORD ForbiddenStates = (std::get<FoSFkt>(it->second))();
		HRESULT hres = SVSVIMStateClass::CheckNotAllowedState(ForbiddenStates);
		pResponse->set_hresult(hres);
	}
	return pResponse->hresult();
}

HRESULT InspectionCommands(uint32_t inspectionID, const SvPb::InspectionCmdRequest& rRequest, SvPb::InspectionCmdResponse* pResp
	, ThreadPref t /*= ThreadPref::threadDefault */)
{
	using InspectionTask = std::packaged_task<SvPb::InspectionCmdResponse()>;
	SvPb::InspectionCmdResponse response;
	SvPb::InspectionCmdResponse* pResponse = (nullptr != pResp) ? pResp : &response;
	pResponse->set_hresult(E_FAIL);
	
	InspectionTask  CurrentTask;
	ThreadPref  CurrentThread {ThreadPref::inspection};
	std::chrono::milliseconds  CurrentTimout {std::chrono::milliseconds(120000)};
	auto it = InspectionCommandMap.find(rRequest.message_case());
	if (it != InspectionCommandMap.end())
	{
		InspectionTask Task([&rRequest, &it]()
		{return   (std::get<ExFkt>(it->second)) (rRequest); });

		CurrentTask = std::move(Task);
		CurrentThread = (std::get<ThFkt>(it->second))();
		CurrentTimout = (std::get<TiFkt>(it->second))();

		if (t != ThreadPref::threadDefault)
		{
			CurrentThread = t;
		}
		if (CurrentThread == ThreadPref::threadDefault)
		{
			CurrentThread = ThreadPref::inspection;
		}
		std::future< SvPb::InspectionCmdResponse> futureResp = CurrentTask.get_future();

		if ((inspectionID > 0) && (CurrentThread == ThreadPref::inspection))
		{
			/// IObjectClass required for include check
			SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*>((SvOi::getObject(inspectionID)));
			if (pInspection)
			{
				SvOi::ICommandPtr pCommand = std::make_shared<  SvOi::CTaskWrapper<InspectionTask>>(std::move(CurrentTask));
				pInspection->SubmitCommand(pCommand);
			}
			else
			{
				Log_Assert(false);
				return E_FAIL;
			}
		}
		else if (CurrentThread == ThreadPref::async)
		{

			std::thread task_td(std::move(CurrentTask));
			task_td.detach();
		}
		else if (CurrentThread == ThreadPref::cur)
		{
			CurrentTask();
		}
		else
		{
			return E_FAIL;
		}
		std::future_status status = futureResp.wait_for(CurrentTimout);
		if (status == std::future_status::ready)
		{
			try
			{
				*pResponse = futureResp.get();
			}
			catch (const SvStl::MessageContainer& rExp)
			{
#ifdef TRACE_CATCH
				std::string msg("catch messagecontainer in InspectionCommands: ");
				msg += rExp.getMessage().getAdditionalText().c_str();
				OutputDebugString(msg.c_str());
#endif
			throw (rExp);
			}

			catch (std::exception& ex)
			{

#ifdef TRACE_CATCH
				std::string msg("catch Std:.exception in InspectionCommands: ");
				msg += ex.what();
				OutputDebugString(msg.c_str());
#endif 
				std::vector<std::string> msgList;
				msgList.push_back(ex.what());
				SvStl::MessageManager MesMan(SvStl::MsgType::Log);
				MesMan.setMessage(SVMSG_SVO_UNHANDLED_EXCEPTION, SvStl::Tid_StdException, msgList, SvStl::SourceFileParams(StdMessageParams));
				MesMan.Throw();
			}
			catch (HRESULT&  hr)
			{
#ifdef TRACE_CATCH
				std::string msg("catch hr in InspectionCommands: ");
				msg += std::to_string(hr);
				OutputDebugString(msg.c_str());
#endif 
				std::vector<std::string> msgList;
				msgList.push_back(std::to_string(hr).c_str());
				SvStl::MessageManager MesMan(SvStl::MsgType::Log);
				MesMan.setMessage(SVMSG_SVO_UNHANDLED_EXCEPTION, SvStl::Tid_StdException, msgList, SvStl::SourceFileParams(StdMessageParams));
				MesMan.Throw();
			}
			catch (...)
			{
#ifdef TRACE_CATCH	
				OutputDebugString("InspectionCommands:  catch ...");
#endif 				
				std::vector<std::string> msgList;
				msgList.push_back("...");
				SvStl::MessageManager MesMan(SvStl::MsgType::Log);
				MesMan.setMessage(SVMSG_SVO_UNHANDLED_EXCEPTION, SvStl::Tid_StdException, msgList, SvStl::SourceFileParams(StdMessageParams));
				MesMan.Throw();
			}
		}
	}

	return pResponse->hresult();
}

HRESULT RunOnceSynchronous(uint32_t inspectionID)
{
	SvPb::InspectionCmdRequest requestCmd;
	auto* pRequest = requestCmd.mutable_inspectionrunoncerequest();
	pRequest->set_inspectionid(inspectionID);
	return SvCmd::InspectionCommands(inspectionID, requestCmd, nullptr);
}

std::string getDottedName(uint32_t inspectionId, uint32_t objectId, bool includeToolSet/* = false*/)
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getobjectnamerequest();
	pRequest->set_objectid(objectId);
	if (includeToolSet)
	{
		pRequest->set_totype(SvPb::SVToolSetObjectType);
	}
	else
	{
		pRequest->set_beforetype(SvPb::SVToolSetObjectType);
	}
	HRESULT hr = SvCmd::InspectionCommands(inspectionId, requestCmd, &responseCmd);
	if (S_OK != hr || false == responseCmd.has_getobjectnameresponse())
	{
		Log_Assert(false);
		return {};
	}
	return responseCmd.getobjectnameresponse().name();
}

std::string getDottedName(uint32_t inspectionId, std::string objectIdString)
{
	std::string::size_type Pos = objectIdString.find_first_of(_T("["));
	uint32_t objectId = SvDef::InvalidObjectId;
	std::sscanf(objectIdString.substr(0, Pos).c_str(), "{#%u}", &objectId);
	if (SvDef::InvalidObjectId == objectId)
	{
		return {};
	}

	std::string name = getDottedName(inspectionId, objectId);
	if (false == name.empty() && std::string::npos != Pos)
	{
		name += objectIdString.substr(Pos, std::string::npos);
	}
	return name;
}

bool ResponseToObjectInfo(const SvPb::InspectionCmdResponse& rResponse, SvOi::ObjectInfoVector& rToolSetInfos)
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
	std::ranges::transform(rPbList, std::back_inserter(list), [](const auto& rEntry) -> SvUl::NameObjectIdPair { return {rEntry.objectname(), rEntry.objectid()}; });
	return list;
}

void setTypeInfos(const SvDef::SVObjectTypeInfoStruct& destInfo, SvPb::SVObjectTypeInfoStruct& sourceInfo)
{
	sourceInfo.set_objecttype(destInfo.m_ObjectType);
	sourceInfo.set_subtype(destInfo.m_SubType);
	sourceInfo.set_embeddedid(destInfo.m_EmbeddedID);
}

SvPb::GetObjectSelectorItemsRequest createObjectSelectorRequest(const std::vector<SvPb::SearchArea>& rSearchAreas, uint32_t inspectionID, SvPb::ObjectAttributes attribute, uint32_t instanceID /*= 0*/, bool wholeArray /*= false*/, SvPb::ObjectSelectorType type /*= SvPb::allValueObjects*/, SvPb::GetObjectSelectorItemsRequest::FilterCase filter /*= SvPb::GetObjectSelectorItemsRequest::FilterCase::kAttributesAllowed*/, uint32_t importantObjectForStopAtClosed /*= SvDef::InvalidObjectId*/)
{
	SvPb::GetObjectSelectorItemsRequest result;

	for (const auto& rArea : rSearchAreas)
	{
		result.add_areas(rArea);
	}
	result.set_inspectionid(inspectionID);
	result.set_instanceid(instanceID);
	result.set_attribute(attribute);
	result.set_wholearray(wholeArray);
	result.set_type(type);
	result.set_importantobjectforstopatborder(importantObjectForStopAtClosed);
	switch (filter)
	{
		case SvPb::GetObjectSelectorItemsRequest::kAttributesAllowed:
			result.set_attributesallowed(0);
			break;
		case SvPb::GetObjectSelectorItemsRequest::kExcludeSameLineage:
			Log_Assert(false);
			break;
		default: //nothing to do, empty oneof
			break;
	}

	return result;
}

SvPb::GetObjectSelectorItemsRequest createObjectSelectorRequest(const std::vector<SvPb::SearchArea>& rSearchAreas, uint32_t inspectionID, SvPb::ObjectAttributes attribute, uint32_t instanceID, bool wholeArray, SvPb::ObjectSelectorType type, const std::vector<uint32_t>& excludeSameLineageVector, uint32_t importantObjectForStopAtClosed)
{
	SvPb::GetObjectSelectorItemsRequest result = createObjectSelectorRequest(rSearchAreas, inspectionID, attribute, instanceID, wholeArray, type, SvPb::GetObjectSelectorItemsRequest::FilterCase::FILTER_NOT_SET, importantObjectForStopAtClosed);
	auto* pExcludeData = result.mutable_excludesamelineage();
	for (auto id : excludeSameLineageVector)
	{
		pExcludeData->add_excludeids(id);
	}
	return result;
}

std::string getComment(uint32_t inspectionId, uint32_t objectId)
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getobjectparametersrequest();
	pRequest->set_objectid(objectId);

	HRESULT hr = SvCmd::InspectionCommands(inspectionId, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getobjectparametersresponse())
	{
		return responseCmd.getobjectparametersresponse().comment();
	}

	return {};
}

HRESULT setComment(uint32_t inspectionId, uint32_t objectId, std::string commentStr)
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_setobjectcommentrequest();
	pRequest->set_objectid(objectId);
	pRequest->set_comment(commentStr);

	HRESULT hr = SvCmd::InspectionCommands(inspectionId, requestCmd, &responseCmd);
	Log_Assert(S_OK == hr);
	return hr;
}
} //namespace SvCmd