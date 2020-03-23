//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Value Accessor class for use by the DataController/IObjectManagerCommand.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <guiddef.h>
#include "BoundValue.h"
#include "ObjectInterfaces/NameValueVector.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "SVMessage/SVMessage.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvOg
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
		const GUID& rInspectionID = rValues.GetInspectionID();
		const GUID& rTaskID = rValues.GetTaskID();
		rValues.clear();

		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_getembeddedvaluesrequest();
		SvPb::SetGuidInProtoBytes(pRequest->mutable_objectid(), rTaskID);

		HRESULT hr = SvCmd::InspectionCommands(rInspectionID, requestCmd, &responseCmd);
		if (responseCmd.has_getembeddedvaluesresponse())
		{
			for (auto& rItem : responseCmd.getembeddedvaluesresponse().list())
			{
				_variant_t value, defaultValue;
				ConvertProtobufToVariant(rItem.value(), value);
				ConvertProtobufToVariant(rItem.defaultvalue(), defaultValue);
				rValues[rItem.embeddedid()] = {SvPb::GetGuidFromProtoBytes(rItem.objectid()), value, defaultValue};
			}
		}

		return hr;
	}

	void SetValues(const SvOg::BoundValues& rValues, PostAction doAction)
	{
		m_MessageFailList.clear();
		const GUID& rTaskID = rValues.GetTaskID();
		const GUID& rInspectionID = rValues.GetInspectionID();

		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_setembeddedvaluesrequest();
		SvPb::SetGuidInProtoBytes(pRequest->mutable_objectid(), rTaskID);
		for (const auto& rValue : rValues)
		{
			auto* pEntry = pRequest->add_list();
			pEntry->set_ismodified(rValue.second.isModified());
			pEntry->set_isdefaultmodified(rValue.second.isDefaultModified());
			pEntry->set_arrayindex(rValue.second.GetArrayIndex());
			SvPb::SetGuidInProtoBytes(pEntry->mutable_values()->mutable_objectid(), rValue.second.GetObjectID());
			pEntry->mutable_values()->set_embeddedid(rValue.first);
			ConvertVariantToProtobuf(rValue.second.GetValue(), pEntry->mutable_values()->mutable_value());
			ConvertVariantToProtobuf(rValue.second.GetDefaultValue(), pEntry->mutable_values()->mutable_defaultvalue());
		}

		HRESULT hr = SvCmd::InspectionCommands(rInspectionID, requestCmd, &responseCmd);
		if (responseCmd.has_standardresponse())
		{
			m_MessageFailList = SvPb::setMessageVectorFromMessagePB(responseCmd.standardresponse().errormessages());
		}

		if (S_OK == hr)
		{
			//Reset modified flags
			for (auto& rValue : rValues)
			{
				rValue.second.ClearModified();
				rValue.second.ClearDefaultModified();
				rValue.second.ClearArrayIndex();
			}

			bool bReset {doAction & PostAction::doReset};
			bool bRunOnce {doAction & PostAction::doRunOnce};

			if (bReset)
			{
				// Do a reset of the Tool
				ResetObject(rInspectionID, rTaskID);
			}
			if (S_OK == hr && bRunOnce)
			{
				hr = RunOnce(rInspectionID);
				if(S_OK != hr)
				{
					SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Error_CannotRunOnce, SvStl::SourceFileParams(StdMessageParams), 0, rTaskID);
					m_MessageFailList.push_back(message);
				}
			}
		}
		if (S_OK != hr && 0 == m_MessageFailList.size())
		{
			SvDef::StringVector msgList;
			msgList.push_back(SvUl::Format(_T("%d"),hr));
			SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnknownCommitError, msgList, SvStl::SourceFileParams(StdMessageParams), 0, rTaskID);
			m_MessageFailList.push_back(message);
		}
		if (0 < m_MessageFailList.size())
		{
			throw m_MessageFailList;
		}
	}

	SvOi::NameValueVector GetEnums(const GUID& rInspectionID, const GUID& rObjectID) const
	{
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse response;
		SvPb::GetValueObjectEnumsRequest* pGetObjectEnumsRequest = requestCmd.mutable_getvalueobjectenumsrequest();

		SvPb::SetGuidInProtoBytes(pGetObjectEnumsRequest->mutable_objectid(), rObjectID);
		HRESULT hr = SvCmd::InspectionCommands(rInspectionID, requestCmd, &response);
		if (S_OK == hr && response.has_getvalueobjectenumsresponse())
		{
			SvOi::NameValueVector retValue;
			retValue.reserve(response.getvalueobjectenumsresponse().list().size());
			for(auto& valuePair: response.getvalueobjectenumsresponse().list())
			{
				retValue.emplace_back(valuePair.name(), valuePair.value());
			}
			return retValue;
		}
		return SvOi::NameValueVector();
	}

	std::string GetObjectName(const GUID& rInspectionID, const GUID& rObjectID) const
	{
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_getobjectparametersrequest();
		SvPb::SetGuidInProtoBytes(pRequest->mutable_objectid(), rObjectID);

		HRESULT hr = SvCmd::InspectionCommands(rInspectionID, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getobjectparametersresponse())
		{
			return responseCmd.getobjectparametersresponse().name();
		}
		return std::string();
	}

	void ResetObject(const GUID& rInspectionID, const GUID& rObjectID)
	{
		m_MessageFailList.clear();
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_resetobjectrequest();
		SvPb::SetGuidInProtoBytes(pRequest->mutable_objectid(), rObjectID);

		HRESULT hr = SvCmd::InspectionCommands(rInspectionID, requestCmd, &responseCmd);
		if (responseCmd.has_standardresponse() && responseCmd.standardresponse().has_errormessages())
		{
			m_MessageFailList = SvPb::setMessageVectorFromMessagePB(responseCmd.standardresponse().errormessages());
		}
		if (S_OK != hr && 0 < m_MessageFailList.size())
		{
			SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorInReset, SvStl::SourceFileParams(StdMessageParams), 0, rObjectID);
			m_MessageFailList.push_back(message);
		}
		if (0 < m_MessageFailList.size())
		{
			throw m_MessageFailList;
		}
	}

	HRESULT RunOnce(const GUID& rInspectionID)
	{
		// Do a run once of the whole Inspection
		return SvCmd::RunOnceSynchronous(rInspectionID);
	}

	const SvStl::MessageContainerVector& getFailedMessageList() { return m_MessageFailList; };
#pragma endregion Public Methods

#pragma region Member Variables
private:
	SvStl::MessageContainerVector m_MessageFailList;
#pragma endregion Member Variables
};
} //namespace SvOg
