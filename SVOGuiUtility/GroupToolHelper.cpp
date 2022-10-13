//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file GroupToolHelper.cpp
/// This is the class contains structs and functions for Group Tool GUIs (e.g. input tab)
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "GroupToolHelper.h"
#include "DataController.h"
#include "Definitions/GlobalConst.h"
#pragma endregion Includes

namespace
{
SvDef::StringPairVector getDependency(uint32_t valueId)
{
	SvDef::StringPairVector dependencyList;

	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getdependencyrequest();
	auto* idSet = pRequest->mutable_idsofobjectsdependedon();

	idSet->Add(valueId);
	pRequest->set_objecttype(SvPb::SVToolObjectType);
	pRequest->set_tooldependecytype(SvPb::ToolDependencyEnum::Client);
	pRequest->set_alldependecies(true);
	pRequest->set_dependecytype(SvPb::DependecyTypeEnum::Object);

	HRESULT hr = SvCmd::InspectionCommands(0, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getdependencyresponse())
	{
		SvPb::GetDependencyResponse dependencyResponse = responseCmd.getdependencyresponse();

		std::ranges::transform(dependencyResponse.dependencypair(), std::back_inserter(dependencyList),
		[](SvPb::DependencyPair dependencyPair) {return std::pair<std::string, std::string>{dependencyPair.supplier().name(), dependencyPair.client().name()}; });
	}

	return dependencyList;
}

SvStl::MessageData getMessage(const SvStl::MessageContainerVector& rErrorMessages, uint32_t objectId)
{
	auto iter = std::ranges::find_if(rErrorMessages, [objectId](const auto& rEntry) { return rEntry.getObjectId() == objectId; });
	if (rErrorMessages.end() != iter)
	{
		return iter->getMessage();
	}
	return {};
}
}

namespace SvOgu
{
bool isOk(const GroupInputResultData& rData)
{
	return (SvDef::InvalidObjectId == rData.m_errorData.m_ObjectId);
}

bool setValue(GroupInputResultData& data, const std::string& newStr)
{
	if (SvPb::LinkedSelectedOption::DirectValue == data.m_data.m_selectedOption && SvPb::isValueType(data.m_type))
	{
		variant_t tmp {newStr.c_str()};
		SvStl::MessageContainer msgContainer;
		bool isValid = (S_OK == ::VariantChangeTypeEx(&data.m_data.m_directValue, &tmp, SvDef::LCID_USA, VARIANT_ALPHABOOL, data.m_data.m_defaultValue.vt));
		if (false == isValid)
		{
			SvDef::StringVector msgList;
			msgList.push_back(data.m_name);
			msgContainer.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_LinkedValue_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
			SvStl::MessageManager Msg(SvStl::MsgType::Display);
			Msg.setMessage(msgContainer.getMessage());
		}
		else
		{
			data.m_data.m_Value = data.m_data.m_directValue;
		}
		return isValid;
	}
	return false;
}

SvStl::MessageContainerVector sendValuesToInspection(uint32_t ipId, uint32_t taskId, const std::vector<GroupInputResultData>& rDataList, bool displayMessageBox)
{
	SvStl::MessageContainerVector errorMessages;
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_setandsortembeddedvaluerequest();
	pRequest->set_ownerid(taskId);
	for (const auto& data : rDataList)
	{
		auto* pTmp = pRequest->add_embeddedlist();
		pTmp->set_name(data.m_name);
		pTmp->set_oldembeddedid(data.m_oldEmbeddedId);
		pTmp->set_type(data.m_type);
		*pTmp->mutable_value() = convertLinkedValue(data.m_data);
	}

	HRESULT hr = SvCmd::InspectionCommands(ipId, requestCmd, &responseCmd);
	if (S_OK != hr)
	{
		if (responseCmd.has_setandsortembeddedvalueresponse())
		{
			if (responseCmd.has_errormessage())
			{
				errorMessages = convertProtobufToMessageVector(responseCmd.errormessage());
			}
		}

		if (displayMessageBox)
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			if (0 < errorMessages.size())
			{
				Msg.setMessage(errorMessages[0].getMessage());
			}
			else
			{
				Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_UnknownException, SvStl::SourceFileParams(StdMessageParams));
			}
		}
	}
	return errorMessages;
}

bool addEntry(const std::string& firstNameTry, std::vector<GroupInputResultData>& rDataList)
{
	if (rDataList.size() < SvDef::c_maxTableColumn)
	{
		GroupInputResultData& rData {rDataList.emplace_back(firstNameTry)};
		if (0 < std::count_if(rDataList.begin(), rDataList.end(), [rData](const auto& rEntry) { return rEntry.m_name == rData.m_name; }))
		{ //duplicate
			std::string newName;
			int additionalValue = 1;
			do
			{
				newName = firstNameTry + std::to_string(additionalValue++);
			} while (0 < std::count_if(rDataList.begin(), rDataList.end(), [newName](const auto& rEntry) { return rEntry.m_name == newName; }));
			rData.m_name = newName;
		}
		return true;
	}
	return false;
}

void loadDataList(std::vector<GroupInputResultData>& rDataList, ValueController& rValueCtrl, SvPb::EmbeddedIdEnum valueEId, SvPb::EmbeddedIdEnum typeEId, const SvStl::MessageContainerVector& rErrorMessages)
{
	rDataList.clear();
	rValueCtrl.Init();

	for (int i = 0; SvDef::c_maxTableColumn > i; ++i)
	{
		if (0 == rValueCtrl.GetAllowedAttributeFlags(valueEId + i))
		{	//stop loading values, because it is hidden.
			break;
		}

		GroupInputResultData data;
		data.m_name = rValueCtrl.GetName(valueEId + i);
		data.m_oldEmbeddedId = valueEId + i;
		data.m_type = static_cast<SvPb::LinkedValueTypeEnum>(rValueCtrl.Get<int>(typeEId + i));
		data.m_data = rValueCtrl.Get<LinkedValueData>(valueEId + i);

		auto valueId = rValueCtrl.GetObjectID(valueEId + i);
		data.m_dependencies = getDependency(valueId);
		data.m_errorData = getMessage(rErrorMessages, valueId);
		rDataList.emplace_back(std::move(data));
	}
}

namespace
{
SvStl::MessageContainerVector checkParameterNames(uint32_t ipId, uint32_t taskId, SvPb::CheckParameterNamesRequest request)
{
	SvStl::MessageContainerVector errorMessages;
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_checkparameternamesrequest();
	pRequest->CopyFrom(request);
	pRequest->set_parametertaskid(taskId);

	HRESULT hr = SvCmd::InspectionCommands(ipId, requestCmd, &responseCmd);
	if (S_OK != hr)
	{
		if (responseCmd.has_checkparameternamesresponse())
		{
			if (responseCmd.has_errormessage())
			{
				errorMessages = convertProtobufToMessageVector(responseCmd.errormessage());
			}
		}
	}
	return errorMessages;
}
}

SvStl::MessageContainerVector checkParameterNames(uint32_t ipId, uint32_t taskId, const std::vector<GroupInputResultData>& rDataList)
{
	SvPb::CheckParameterNamesRequest message;
	for (const auto& data : rDataList)
	{
		message.add_parameter_names(data.m_name);
	}
	return checkParameterNames(ipId, taskId, message);
}

SvStl::MessageContainerVector checkParameterNames(uint32_t ipId, uint32_t taskId, const std::vector<std::string>& rDataList)
{
	SvPb::CheckParameterNamesRequest message;
	for (const auto& data : rDataList)
	{
		message.add_parameter_names(data);
	}
	return checkParameterNames(ipId, taskId, message); 
}
}