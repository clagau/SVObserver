//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
// All Rights Reserved
//*****************************************************************************

// This is the RangeController for the Range Value Objects.
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "RangeController.h"
#include "LinkedValueWidgetHelper.h"
#include "Definitions/RangeEnum.h"
#pragma endregion Includes

namespace SvOgu
{
#pragma region Declarations
std::map<RangeEnum::ERange, SvPb::EmbeddedIdEnum> g_MessageIdToEmbeddedId = {{RangeEnum::ER_FailHigh, SvPb::RangeClassFailHighEId},
	{RangeEnum::ER_FailLow, SvPb::RangeClassFailLowEId},
	{RangeEnum::ER_WarnHigh, SvPb::RangeClassWarnHighEId},
	{RangeEnum::ER_WarnLow, SvPb::RangeClassWarnLowEId}};

namespace
{
CString convertInString(const variant_t& rValue)
{
	try
	{
		double value = rValue;
		std::stringstream ss;
		ss.precision(6);
		ss << std::fixed << value;
		return {ss.str().c_str()};
	}
	catch (...)
	{
		return static_cast<CString>(rValue);
	}
}

bool isFieldValid(const std::string& fieldName, const variant_t& rValue, SvStl::MessageContainer& rMsgContainer)
{
	Log_Assert(VT_R8 == rValue.vt);
	if (VT_R8 == rValue.vt)
	{
		const double s_RangeMax = 17000000;
		const double s_RangeMin = -s_RangeMax;
		if (rValue.dblVal > s_RangeMax || rValue.dblVal < s_RangeMin)
		{
			SvDef::StringVector msgList;
			msgList.push_back(fieldName);
			msgList.push_back(std::format(_T("{}"), static_cast<int>(s_RangeMin)));
			msgList.push_back(std::format(_T("{}"), static_cast<int>(s_RangeMax)));
			rMsgContainer.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RangeValue_WrongRange, msgList, SvStl::SourceFileParams(StdMessageParams));
			return false;
		}
	}
	return true;
}
}
#pragma endregion Declarations

	RangeController::RangeController(uint32_t inspectionID, uint32_t taskID, uint32_t rangeID)
		: m_InspectionID(inspectionID)
		, m_TaskObjectID(taskID)
		, m_RangeID(rangeID)
		//TaskID is set later
		, m_RangeValues{ BoundValues{inspectionID, SvDef::InvalidObjectId} }
	{
	}

	RangeController::~RangeController()
	{
	}

	void RangeController::Init()
	{
		if (SvDef::InvalidObjectId == m_RangeID)
		{
			SvPb::InspectionCmdRequest requestCmd;
			SvPb::InspectionCmdResponse responseCmd;
			auto* pRequest = requestCmd.mutable_getobjectidrequest()->mutable_info();
			pRequest->set_ownerid(m_TaskObjectID);
			pRequest->mutable_infostruct()->set_objecttype(SvPb::SVRangeObjectType);

			HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
			if (S_OK == hr && responseCmd.has_getobjectidresponse())
			{
				m_RangeID = responseCmd.getobjectidresponse().objectid();
			}
		}
		
		m_RangeValues.SetTaskID(m_RangeID);
		m_RangeValues.Init();
	}

	HRESULT RangeController::Commit()
	{
		return m_RangeValues.Commit(PostAction::doReset | PostAction::doRunOnce, true);
	}

	LinkedValueData RangeController::Get(RangeEnum::ERange fieldId) const
	{
		auto iter = g_MessageIdToEmbeddedId.find(fieldId);
		if (g_MessageIdToEmbeddedId.end() != iter)
		{
			return m_RangeValues.Get<LinkedValueData>(iter->second);
		}
		else
		{
			throw std::exception("Invalid Argument");
		}
	}

	void RangeController::Set(RangeEnum::ERange fieldId, const LinkedValueData& rValue)
	{
		auto iter = g_MessageIdToEmbeddedId.find(fieldId);
		if (g_MessageIdToEmbeddedId.end() != iter)
		{
			m_RangeValues.Set<LinkedValueData>(iter->second, rValue);
		}
		else
		{
			throw std::exception("Invalid Argument");
		}
	}

	std::string RangeController::GetOwnerName() const
	{
		std::string name;
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_getobjectparametersrequest();
		pRequest->set_objectid(m_TaskObjectID);

		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getobjectparametersresponse())
		{
			name = responseCmd.getobjectparametersresponse().name();
		}
		return name;
	}

	void RangeController::IsFieldValid(RangeEnum::ERange fieldId) const
	{
		const LinkedValueData& rValue = Get(fieldId);
		if (SvPb::LinkedSelectedOption::DirectValue == rValue.m_selectedOption)
		{
			SvStl::MessageContainer msgContainer;
			if (false == isFieldValid(RangeEnum::ERange2String(fieldId), rValue.m_Value, msgContainer))
			{
				throw msgContainer;
			}
		}
	}

	std::unique_ptr<LinkedValueWidgetHelper> RangeController::createWidget(RangeEnum::ERange fieldId, CEdit& rValueEdit, CButton& rSelectButton)
	{
		auto iter = g_MessageIdToEmbeddedId.find(fieldId);
		if (g_MessageIdToEmbeddedId.end() != iter)
		{
			auto fieldName = RangeEnum::ERange2String(fieldId);
			auto validCheck = [fieldName](const variant_t& rValue, SvStl::MessageContainer& rMsgContainer)
			{
				return isFieldValid(fieldName, rValue, rMsgContainer);
			};
			return std::make_unique<LinkedValueWidgetHelper>(rValueEdit, rSelectButton, m_InspectionID, m_TaskObjectID, iter->second, &m_RangeValues, validCheck, convertInString);
		}
		else
		{
			return nullptr;
		}
	}
} //namespace SvOgu
