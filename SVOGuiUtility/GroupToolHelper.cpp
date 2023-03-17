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
bool GroupInputResultData::isOk() const
{
	return (SvDef::InvalidObjectId == m_errorData.m_ObjectId);
}

bool GroupInputResultData::setValue(const std::string& newStr)
{
	if (SvPb::LinkedSelectedOption::DirectValue == m_data.m_selectedOption && SvPb::isValueType(m_type))
	{
		variant_t tmp {newStr.c_str()};
		SvStl::MessageContainer msgContainer;
		bool isValid = (S_OK == ::VariantChangeTypeEx(&m_data.m_directValue, &tmp, SvDef::LCID_USA, VARIANT_ALPHABOOL, m_data.m_defaultValue.vt));
		if (false == isValid)
		{
			SvDef::StringVector msgList;
			msgList.push_back(m_name);
			msgContainer.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_LinkedValue_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
			SvStl::MessageManager Msg(SvStl::MsgType::Display);
			Msg.setMessage(msgContainer.getMessage());
		}
		else
		{
			m_data.m_Value = m_data.m_directValue;
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

bool addEntry(uint32_t ipId, uint32_t taskId, const std::string& firstNameTry, std::vector<GroupInputResultData>& rDataList)
{
	auto errorMessage = SvOgu::checkParameterNames(ipId, taskId, rDataList);
	if (0 < errorMessage.size())
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Display);
		Msg.setMessage(errorMessage[0].getMessage());
		return false;
	}

	if (rDataList.size() < SvDef::c_maxTableColumn)
	{
		GroupInputResultData& rData {rDataList.emplace_back(firstNameTry)};
		int additionalValue = 1;
		do
		{
			rData.m_name = firstNameTry + std::to_string(additionalValue++);
		} while (false == SvOgu::checkParameterNames(ipId, taskId, rDataList).empty());
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

variant_t convertTextToVariant(SvPb::LinkedValueTypeEnum type, const std::string& text)
{
	variant_t result {};
	try
	{
		switch (type)
		{
			case SvPb::LinkedValueTypeEnum::TypeDecimal:
				//@Todo[mec}
				result = std::stod(text);
				break;
			case SvPb::LinkedValueTypeEnum::TypeText:
				result = text.c_str();
				break;
			case SvPb::LinkedValueTypeEnum::TypeStates:
				result = StateHelper::toState(text);

				break;


			default: //do nothing, empty variant
				break;
		}
	}
	catch (...)
	{
		SvDef::StringVector msgList;
		msgList.push_back(text);
		SvStl::MessageManager msg(SvStl::MsgType::Data);
		msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ConvertTextToVariantFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
		msg.Throw();
	}

	return result;
}

variant_t fitVariantToType(SvPb::LinkedValueTypeEnum type, const variant_t& value)
{
	variant_t result {};
	bool isValid {true};
	switch (type)
	{
		case SvPb::LinkedValueTypeEnum::TypeDecimal:
			isValid = (S_OK == ::VariantChangeTypeEx(&result, &value, SvDef::LCID_USA, VARIANT_ALPHABOOL, VT_R8));
			break;
		case SvPb::LinkedValueTypeEnum::TypeText:
			isValid = (S_OK == ::VariantChangeTypeEx(&result, &value, SvDef::LCID_USA, VARIANT_ALPHABOOL, VT_BSTR));
			break;
		case SvPb::LinkedValueTypeEnum::TypeStates:
			isValid = (S_OK == ::VariantChangeTypeEx(&result, &value, SvDef::LCID_USA, VARIANT_ALPHABOOL, VT_INT));
			break;

		default: //do nothing, empty variant
			break;
	}

	if (false == isValid)
	{
		SvDef::StringVector msgList;
		msgList.push_back(static_cast<std::string>(static_cast<CString>(value)));
		SvStl::MessageManager msg(SvStl::MsgType::Data);
		msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ConvertTextToVariantFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
		msg.Throw();
	}

	return result;
}

void checkAndCorrectTypes(SvOgu::GroupInputResultData& rData)
{
	try
	{
		rData.m_data.m_defaultValue = fitVariantToType(rData.m_type, rData.m_data.m_defaultValue);
	}
	catch (...)
	{
		rData.m_data.m_defaultValue = SvPb::getDefaultString(rData.m_type);
	}

	if (rData.m_data.m_selectedOption == SvPb::DirectValue)
	{
		try
		{
			rData.m_data.m_directValue = fitVariantToType(rData.m_type, rData.m_data.m_directValue);
		}
		catch (...)
		{
			rData.m_data.m_directValue = rData.m_data.m_defaultValue;
		}
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





const std::array< std::pair<std::string, int>, 3>& StateHelper::getStates()
{
	
	return m_states;
}
 const std::array< std::pair<std::string, int>, 3> StateHelper::m_states = 
 {
	 std::pair(toString(SvPb::StateDefaultType::Passed), SvPb::StateDefaultType::Passed),
	 std::pair(toString(SvPb::StateDefaultType::Failed), SvPb::StateDefaultType::Failed),
	 std::pair(toString(SvPb::StateDefaultType::Warned), SvPb::StateDefaultType::Warned)
};

int StateHelper::toState(const std::string& name)
{
	
	auto  found = std::find_if(m_states.begin(), m_states.end(), [&name](const auto e){return (e.first == name); });

	if (found != m_states.end())
		return found->second;
	else 
		return SvPb::StateDefaultType::Passed;

		
		
}


std::string StateHelper::toString(const _variant_t& var)
{
	try
	{
		return StateHelper::toString(static_cast<int>(var));
	}
	catch (...)
	{
		return "";
	}
}
std::string StateHelper::toString(int var)
{

	try
	{
		return SvPb::StateDefaultType_Name(SvPb::StateDefaultType(var));
	}
	catch (...)
	{
		return "";
	}

}



}