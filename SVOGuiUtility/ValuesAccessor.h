//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
// All Rights Reserved
//*****************************************************************************

// This is the Value Accessor class for use by the DataController/IObjectManagerCommand.
//******************************************************************************
#pragma once

#pragma region Includes
#include "BoundValue.h"
#include "LinkedValue.h"
#include "Definitions/NameValueVector.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "SVMessage/SVMessage.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVStatusLibrary/MessageContainer.h"
#pragma endregion Includes

namespace SvOgu
{
class ValuesAccessor
{
public:
	typedef BoundValues value_type;

#pragma region Constructor
public:
	ValuesAccessor() {}
	virtual ~ValuesAccessor() {}

	ValuesAccessor& operator=(const ValuesAccessor&) = delete;
#pragma endregion Constructor

#pragma region Public Methods
public:
	HRESULT GetValues(BoundValues& rValues)
	{
		uint32_t inspectionID = rValues.GetInspectionID();
		uint32_t taskID = rValues.GetTaskID();
		rValues.clear();

		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_getembeddedvaluesrequest();
		pRequest->set_objectid(taskID);

		HRESULT hr = SvCmd::InspectionCommands(inspectionID, requestCmd, &responseCmd);
		if (responseCmd.has_getembeddedvaluesresponse())
		{
			for (auto& rItem : responseCmd.getembeddedvaluesresponse().list())
			{
				switch (rItem.valueData_case())
				{
				case SvPb::ValueObjectValues::ValueDataCase::kValue:
				{
					_variant_t value, defaultValue, minValue, maxValue;
					SvPb::ConvertProtobufToVariant(rItem.value().value(), value);
					SvPb::ConvertProtobufToVariant(rItem.value().defaultvalue(), defaultValue);
					if (rItem.value().has_min_max())
					{
						SvPb::ConvertProtobufToVariant(rItem.value().min_max().minvalue(), minValue);
						SvPb::ConvertProtobufToVariant(rItem.value().min_max().maxvalue(), maxValue);
					}
					rValues[rItem.embeddedid()] = BoundValue{ rItem.objectid(), value, defaultValue, std::move(minValue), std::move(maxValue), rItem.is_readonly() };
					break;
				}
				case SvPb::ValueObjectValues::ValueDataCase::kLinkedValue:
				{
					rValues[rItem.embeddedid()] = LinkedValue {rItem.objectid(), std::move(convertLinkedValue(rItem.linkedvalue())), rItem.is_readonly()};
					break;
				}
				default:
					Log_Assert(false);
					break;
				}
				
			}
		}

		return hr;
	}

	void SetValues(const BoundValues& rValues, PostAction doAction)
	{
		m_MessageFailList.clear();
		uint32_t inspectionID = rValues.GetInspectionID();
		uint32_t taskID = rValues.GetTaskID();

		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_setembeddedvaluesrequest();
		pRequest->set_objectid(taskID);
		for (const auto& rValue : rValues)
		{
			auto* pEntry = pRequest->add_list();
			std::visit([&pEntry](const auto& val) 
				{ 
					pEntry->set_ismodified(val.isModified());
					pEntry->set_isdefaultmodified(val.isDefaultModified());
					pEntry->set_arrayindex(val.GetArrayIndex());
					pEntry->mutable_values()->set_objectid(val.GetObjectID());
				}, rValue.second);
			pEntry->mutable_values()->set_embeddedid(rValue.first);
			switch (rValue.second.index())
			{
			case 0:
			{
				const auto& rBoundValue = std::get<0>(rValue.second);
				auto* pValue = pEntry->mutable_values()->mutable_value();
				SvPb::ConvertVariantToProtobuf(rBoundValue.GetValue(), pValue->mutable_value());
				SvPb::ConvertVariantToProtobuf(rBoundValue.GetDefaultValue(), pValue->mutable_defaultvalue());
				break;
			}
			case 1:
			{
				const auto& rLinkedData = std::get<1>(rValue.second).getLinkedData();
				auto* pValue = pEntry->mutable_values()->mutable_linkedvalue();
				pValue->set_option(rLinkedData.m_selectedOption);
				SvPb::ConvertVariantToProtobuf(rLinkedData.m_directValue, pValue->mutable_directvalue());
				pValue->set_indirectidstring(rLinkedData.m_indirectIdName);
				pValue->set_formula(rLinkedData.m_formula);
				pValue->set_comment(rLinkedData.m_comment);
				break;
			}
			default:
				Log_Assert(false);
				break;
			}
			
		}

		HRESULT hr = SvCmd::InspectionCommands(inspectionID, requestCmd, &responseCmd);
		if (responseCmd.has_errormessage())
		{
			m_MessageFailList = SvPb::convertProtobufToMessageVector(responseCmd.errormessage());
		}

		if (S_OK == hr)
		{
			//Reset modified flags
			for (auto& rValue : rValues)
			{
				std::visit([](const auto& val)
					{
						val.ClearModified();
						val.ClearDefaultModified();
						val.ClearArrayIndex();
					}, rValue.second);
			}

			bool bReset {doAction & PostAction::doReset};
			bool bRunOnce {doAction & PostAction::doRunOnce};

			if (bReset)
			{
				// Do a reset of the Tool
				ResetObject(inspectionID, taskID);
			}
			if (S_OK == hr && bRunOnce)
			{
				hr = RunOnce(inspectionID);
				if(S_OK != hr)
				{
					SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Error_CannotRunOnce, SvStl::SourceFileParams(StdMessageParams), taskID);
					m_MessageFailList.push_back(message);
				}
			}
		}
		if (S_OK != hr && 0 == m_MessageFailList.size())
		{
			SvDef::StringVector msgList;
			msgList.push_back(std::format(_T("{}"), hr));
			SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnknownCommitError, msgList, SvStl::SourceFileParams(StdMessageParams), taskID);
			m_MessageFailList.push_back(message);
		}
		if (0 < m_MessageFailList.size())
		{
			throw m_MessageFailList;
		}
	}

	uint32_t GetAllowedAttribute(uint32_t inspectionID, uint32_t objectID) const
	{
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_getobjectparametersrequest();
		pRequest->set_objectid(objectID);

		HRESULT hr = SvCmd::InspectionCommands(inspectionID, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getobjectparametersresponse())
		{
			return responseCmd.getobjectparametersresponse().allowedattributes();
		}
		return 0;
	}

	SvDef::NameValueVector GetEnums(uint32_t inspectionID, uint32_t objectID) const
	{
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse response;
		SvPb::GetValueObjectEnumsRequest* pGetObjectEnumsRequest = requestCmd.mutable_getvalueobjectenumsrequest();

		pGetObjectEnumsRequest->set_objectid(objectID);
		HRESULT hr = SvCmd::InspectionCommands(inspectionID, requestCmd, &response);
		if (S_OK == hr && response.has_getvalueobjectenumsresponse())
		{
			SvDef::NameValueVector retValue;
			std::ranges::transform(response.getvalueobjectenumsresponse().list(), std::back_inserter(retValue), [](const auto& rEntry) -> SvDef::NameValuePair { return {rEntry.name(), rEntry.value()}; });
			return retValue;
		}
		return SvDef::NameValueVector();
	}

	std::string GetObjectName(uint32_t inspectionID, uint32_t objectID) const
	{
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_getobjectparametersrequest();
		pRequest->set_objectid(objectID);

		HRESULT hr = SvCmd::InspectionCommands(inspectionID, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getobjectparametersresponse())
		{
			return responseCmd.getobjectparametersresponse().name();
		}
		return std::string();
	}

	HRESULT setObjectName(uint32_t inspectionID, uint32_t objectID, const std::string& name)
	{
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_setobjectnamerequest();
		pRequest->set_objectid(objectID);
		pRequest->set_objectname(name);
		
		return SvCmd::InspectionCommands(inspectionID, requestCmd, &responseCmd);
	}

	void ResetObject(uint32_t inspectionID, uint32_t objectID)
	{
		m_MessageFailList.clear();
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_resetobjectrequest();
		pRequest->set_objectid(objectID);

		HRESULT hr = SvCmd::InspectionCommands(inspectionID, requestCmd, &responseCmd);
		if (responseCmd.has_errormessage())
		{
			m_MessageFailList = SvPb::convertProtobufToMessageVector(responseCmd.errormessage());
		}
		if (S_OK != hr && 0 < m_MessageFailList.size())
		{
			SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorInReset, SvStl::SourceFileParams(StdMessageParams), objectID);
			m_MessageFailList.push_back(message);
		}
		if (0 < m_MessageFailList.size())
		{
			throw m_MessageFailList;
		}
	}

	HRESULT RunOnce(uint32_t inspectionID)
	{
		// Do a run once of the whole Inspection
		return SvCmd::RunOnceSynchronous(inspectionID);
	}

	const SvStl::MessageContainerVector& getFailedMessageList() { return m_MessageFailList; };
#pragma endregion Public Methods

#pragma region Member Variables
private:
	SvStl::MessageContainerVector m_MessageFailList;
#pragma endregion Member Variables
};

} //namespace SvOgu
