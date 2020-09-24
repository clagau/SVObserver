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
#include "ObjectInterfaces/ObjectInfo.h"
#include "SVProtoBuf/InspectionCommands.h"
#include "SVUtilityLibrary/NameObjectIdList.h"
#include "Definitions/SVObjectTypeInfoStruct.h"
#include "ObjectInterfaces/ICommand.h"
#include "InspectionCommandMap.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "ObjectInterfaces/IObjectClass.h"

#pragma endregion Includes

namespace SvCmd
{


	HRESULT InspectionCommands(uint32_t inspectionID, const SvPb::InspectionCmdRequest& rRequest, SvPb::InspectionCmdResponse* pResp)
	{
		using InspectionTask = std::packaged_task<SvPb::InspectionCmdResponse()>;
		SvPb::InspectionCmdResponse response;
		SvPb::InspectionCmdResponse* pResponse = (nullptr != pResp) ? pResp : &response;
		pResponse->set_hresult(E_FAIL);

		InspectionTask  CurrentTask;
		ThreadPref  CurrentThread{ ThreadPref::inspection };
		std::chrono::milliseconds  CurrentTimout{ std::chrono::milliseconds(120000)};
		auto it = InspectionCommandMap.find(rRequest.message_case());
		if (it != InspectionCommandMap.end())
		{
			InspectionTask Task([&rRequest, &it]()
				{return   (std::get<ExFkt>(it->second)) (rRequest); });
			
			CurrentTask = std::move(Task);
			CurrentThread = (std::get<ThFkt>(it->second))();
			CurrentTimout = (std::get<TiFkt>(it->second))();


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
					assert(false);
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
					*pResponse = futureResp.get();
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

	SvPb::GetObjectSelectorItemsRequest createObjectSelectorRequest(const std::vector<SvPb::SearchArea>& rSearchAreas, uint32_t inspectionID, SvPb::ObjectAttributes attribute, uint32_t instanceID /*= 0*/, bool wholeArray /*= false*/, SvPb::SelectorFilter filter /*= SvPb::SelectorFilter::attributesAllowed*/, SvPb::ObjectSelectorType type /*= SvPb::allValueObjects*/)
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
		result.set_filter(filter);
		result.set_type(type);

		return result;
	}
} //namespace SvCmd