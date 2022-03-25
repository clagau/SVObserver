//******************************************************************************
/// COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file ExternalToolTaskController.cpp
/// ExternalToolTaskController definition. 
//******************************************************************************


#pragma region Includes
#include "stdafx.h"
#include "ExternalToolTaskController.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/MessageManager.h"
#pragma endregion Includes



ExternalToolTaskController::ExternalToolTaskController(uint32_t inspectionId, uint32_t ownerId)
	: m_inspectionId(inspectionId), m_objectId(0), m_ownerId(ownerId)
{
	setExternalToolTaskId();
}


HRESULT ExternalToolTaskController::setExternalToolTaskId()
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getobjectidrequest()->mutable_info();
	pRequest->set_ownerid(m_ownerId);
	pRequest->mutable_infostruct()->set_objecttype(SvPb::SVExternalToolTaskObjectType);

	HRESULT hr = SvCmd::InspectionCommands(m_inspectionId, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getobjectidresponse())
	{
		m_objectId = responseCmd.getobjectidresponse().objectid();
	}
	else
	{
		m_objectId = 0;
	}

	return hr;
}

std::pair<HRESULT, SvPb::InitializeExternalToolTaskResponse> ExternalToolTaskController::initialize()
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_initializeexternaltooltaskrequest();
	pRequest->set_objectid(m_objectId);
	pRequest->set_increationprocess(false);
	pRequest->set_initializeall(false);
	HRESULT hr = SvCmd::InspectionCommands(m_inspectionId, requestCmd, &responseCmd);

	SvPb::InitializeExternalToolTaskResponse response;

	if (responseCmd.has_initializeexternaltooltaskresponse())
	{
		response = responseCmd.initializeexternaltooltaskresponse();
	}

	return { hr, response };
}

HRESULT ExternalToolTaskController::runOnce()
{	
	return SvCmd::RunOnceSynchronous(m_inspectionId);
}

std::pair<bool, std::string> ExternalToolTaskController::resetAllObjects(bool showFirstError)
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_resetallobjectsrequest();
	pRequest->set_objectid(m_objectId);
	HRESULT hr = SvCmd::InspectionCommands(m_inspectionId, requestCmd, &responseCmd);
	std::pair<bool, std::string> result(false, ""); // ok? / error description

	if (responseCmd.has_resetallobjectsresponse())
	{
		SvPb::ResetAllObjectsResponse response = responseCmd.resetallobjectsresponse();

		SvStl::MessageContainerVector errorMessages = SvPb::convertProtobufToMessageVector(response.errormessages());

		if (S_OK == hr)
		{
			result.first = true;
		}
		else
		{
			if (false == errorMessages.empty() && showFirstError)
			{
				SvStl::MessageManager mm(SvStl::MsgType::Log); // will be displayed in "DLL Status" widget in TA Dialog
				auto& mc = errorMessages[0]; //currently just the first error message is displayed
				mm.setMessage(mc.getMessage());
				mm.Process();
				result.second = mc.What();
			}
			return result;
		}
	}
	else
	{
		result.second = "Wrong response type";
	}

	return result;
}

HRESULT ExternalToolTaskController::clearData()
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_cleardataexternaltoolrequest();
	pRequest->set_objectid(m_objectId);
	HRESULT hr = SvCmd::InspectionCommands(m_inspectionId, requestCmd, &responseCmd);

	return hr;
}

HRESULT ExternalToolTaskController::setAllAttributes()
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_setallattributesexternaltoolrequest();
	pRequest->set_objectid(m_objectId);
	HRESULT hr = SvCmd::InspectionCommands(m_inspectionId, requestCmd, &responseCmd);

	return hr;
}

SvPb::GetInputValuesDefinitionExternalToolResponse ExternalToolTaskController::getInputValuesDefinition() const
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getinputvaluesdefinitionexternaltoolrequest();
	pRequest->set_objectid(m_objectId);
	HRESULT hr = SvCmd::InspectionCommands(m_inspectionId, requestCmd, &responseCmd);

	SvPb::GetInputValuesDefinitionExternalToolResponse response;
	if (S_OK == hr && responseCmd.has_getinputvaluesdefinitionexternaltoolresponse())
	{
		response = responseCmd.getinputvaluesdefinitionexternaltoolresponse();
	}

	return response;
}

long ExternalToolTaskController::getNumInputValues() const
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getinputvaluesdefinitionexternaltoolrequest();
	pRequest->set_objectid(m_objectId);
	HRESULT hr = SvCmd::InspectionCommands(m_inspectionId, requestCmd, &responseCmd);

	if (S_OK == hr && responseCmd.has_getinputvaluesdefinitionexternaltoolresponse())
	{
 		return responseCmd.getinputvaluesdefinitionexternaltoolresponse().numinputvalues();
	}

	return 0;
}

std::map<std::string, bool> ExternalToolTaskController::getPropTreeState() const
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getproptreestateexternaltoolrequest();
	pRequest->set_objectid(m_objectId);
	HRESULT hr = SvCmd::InspectionCommands(m_inspectionId, requestCmd, &responseCmd);

	std::map<std::string, bool> propTreeState;
	if (S_OK == hr && responseCmd.has_getproptreestateexternaltoolresponse())
	{
		std::transform(responseCmd.getproptreestateexternaltoolresponse().proptreestate().begin(),
			responseCmd.getproptreestateexternaltoolresponse().proptreestate().end(), 
			std::inserter(propTreeState, propTreeState.end()),
			[](const SvPb::PropTreeState& treeState) { return std::make_pair(treeState.name(), treeState.state()); });
	}

	return propTreeState;
}

HRESULT ExternalToolTaskController::setPropTreeState(const std::map<std::string, bool>& propTreeState)
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_setproptreestateexternaltoolrequest();
	pRequest->set_objectid(m_objectId);
	for (const auto& item : propTreeState)
	{
		auto* pEntry = pRequest->add_proptreestate();
		pEntry->set_name(item.first);
		pEntry->set_state(item.second);
	}
	HRESULT hr = SvCmd::InspectionCommands(m_inspectionId, requestCmd, &responseCmd);
		
	return hr;
}

bool ExternalToolTaskController::validateValueParameterWrapper(uint32_t taskObjectId, long index, _variant_t newVal, SvPb::ExDllInterfaceType type)
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_validatevalueparameterexternaltoolrequest();
	pRequest->set_taskobjectid(taskObjectId);
	pRequest->set_index(index);
	pRequest->set_exdllinterfacetype(type);
	ConvertVariantToProtobuf(newVal, pRequest->mutable_newvalue());

	HRESULT hr = SvCmd::InspectionCommands(m_inspectionId, requestCmd, &responseCmd);

	if (S_OK == hr)
	{
		hr = responseCmd.hresult();

		if (S_OK == hr)
		{
			return true;
		}
	}

	SvStl::MessageContainerVector messageContainers;

	if (responseCmd.has_validatevalueparameterexternaltoolresponse())
	{
		messageContainers = SvPb::convertProtobufToMessageVector(responseCmd.validatevalueparameterexternaltoolresponse().errormessages());
	}
	else
	{
		SvDef::StringVector msgList;
		msgList.push_back(_T("<error description missing>"));

		SvStl::MessageContainer msg;
		msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ExternalDllError, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10046, getExternalToolTaskObjectId());
		messageContainers.push_back(msg);
	}

	for (const auto& mc : messageContainers)
	{
		SvStl::MessageManager mm(SvStl::MsgType::Log | SvStl::MsgType::Display);
		mm.setMessage(mc.getMessage());
	}
	return false;

}

std::string ExternalToolTaskController::getDllMessageString(long hResultError) const
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getdllmessagestringrequest();
	pRequest->set_objectid(m_objectId);
	pRequest->set_hresulterror(hResultError);
	
	HRESULT hr = SvCmd::InspectionCommands(m_inspectionId, requestCmd, &responseCmd);

	std::string message;

	if (S_OK == hr && responseCmd.has_getdllmessagestringresponse())
	{
		message = responseCmd.getdllmessagestringresponse().errormessage();
	}

	return message;
}

SvPb::GetResultValuesDefinitionExternalToolResponse ExternalToolTaskController::getResultValuesDefinition() const
{
	
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getresultvaluesdefinitionexternaltoolrequest();
	pRequest->set_objectid(m_objectId);
	HRESULT hr = SvCmd::InspectionCommands(m_inspectionId, requestCmd, &responseCmd);

	SvPb::GetResultValuesDefinitionExternalToolResponse response;
	if (S_OK == hr && responseCmd.has_getresultvaluesdefinitionexternaltoolresponse())
	{
		response = responseCmd.getresultvaluesdefinitionexternaltoolresponse();
	}

	return response;
}

long ExternalToolTaskController::getNumResultValues() const
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getresultvaluesdefinitionexternaltoolrequest();
	pRequest->set_objectid(m_objectId);
	HRESULT hr = SvCmd::InspectionCommands(m_inspectionId, requestCmd, &responseCmd);

	if (S_OK == hr && responseCmd.has_getresultvaluesdefinitionexternaltoolresponse())
	{
		return responseCmd.getresultvaluesdefinitionexternaltoolresponse().numresultvalues();
	}

	return 0;
}


SvPb::GetTableResultsExternalToolResponse ExternalToolTaskController::getTableResults() const
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_gettableresultsexternaltoolrequest();
	pRequest->set_objectid(m_objectId);
	HRESULT hr = SvCmd::InspectionCommands(m_inspectionId, requestCmd, &responseCmd);

	SvPb::GetTableResultsExternalToolResponse response;
	if (S_OK == hr && responseCmd.has_gettableresultsexternaltoolresponse())
	{
		response = responseCmd.gettableresultsexternaltoolresponse();
	}

	return response;
}

SvPb::GetResultRangeObjectResponse ExternalToolTaskController::getResultRangeObjectAtIndex(int index) const
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getresultrangeobjectrequest();
	pRequest->set_objectid(m_objectId);
	pRequest->set_index(index);
	HRESULT hr = SvCmd::InspectionCommands(m_inspectionId, requestCmd, &responseCmd);

	SvPb::GetResultRangeObjectResponse response;
	if (S_OK == hr && responseCmd.has_getresultrangeobjectresponse())
	{
		response = responseCmd.getresultrangeobjectresponse();
	}

	return response;
}

SvPb::GetImageInfoExternalToolResponse ExternalToolTaskController::getInputImageInfoList() const
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getimageinfoexternaltoolrequest();
	pRequest->set_objectid(m_objectId);
	HRESULT hr = SvCmd::InspectionCommands(m_inspectionId, requestCmd, &responseCmd);

	SvPb::GetImageInfoExternalToolResponse response;
	if (S_OK == hr && responseCmd.has_getimageinfoexternaltoolresponse())
	{
		response = responseCmd.getimageinfoexternaltoolresponse();
	}

	return response;
}

uint32_t ExternalToolTaskController::getExternalToolTaskObjectId() const
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getobjectidrequest()->mutable_info();
	pRequest->set_ownerid(m_ownerId);
	pRequest->mutable_infostruct()->set_objecttype(SvPb::SVExternalToolTaskObjectType);

	HRESULT hr = SvCmd::InspectionCommands(m_inspectionId, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getobjectidresponse())
	{
		return responseCmd.getobjectidresponse().objectid();
	}

	return 0;
}
