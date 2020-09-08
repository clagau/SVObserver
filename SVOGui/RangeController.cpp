//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the RangeController for the Range Value Objects.
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <sstream>
//Moved to precompiled header: #include <boost/lexical_cast.hpp>
#include "RangeController.h"
#pragma endregion Includes

namespace SvOg
{
#pragma region Declarations
	const std::string RangeController::FailHigh = "FailHigh";
	const std::string RangeController::FailLow = "FailLow";
	const std::string RangeController::WarnHigh = "WarnHigh";
	const std::string RangeController::WarnLow = "WarnLow";
#pragma endregion Declarations

	RangeController::RangeController(uint32_t inspectionID, uint32_t taskID, uint32_t rangeID)
		: m_InspectionID(inspectionID)
		, m_TaskObjectID(taskID)
		, m_RangeID(rangeID)
		//TaskID is set later
		, m_RangeValues{ SvOg::BoundValues{inspectionID, SvDef::InvalidObjectId} }
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
		return m_RangeValues.Commit(SvOg::PostAction::doReset | SvOg::PostAction::doRunOnce, true);
	}

	std::string RangeController::Get(const std::string& rName) const
	{
		std::string value;
		// Check if indirect is empty
		if (rName == FailHigh)
		{
			value = GetIndirectValue(SvPb::RangeClassFailHighIndirectEId);
			if (value.empty())
			{
				value = GetDirectValue(SvPb::RangeClassFailHighEId);
			}
		}
		else if (rName == FailLow)
		{
			value = GetIndirectValue(SvPb::RangeClassFailLowIndirectEId);
			if (value.empty())
			{
				value = GetDirectValue(SvPb::RangeClassFailLowEId);
			}
		}
		else if (rName == WarnHigh)
		{
			value = GetIndirectValue(SvPb::RangeClassWarnHighIndirectEId);
			if (value.empty())
			{
				value = GetDirectValue(SvPb::RangeClassWarnHighEId);
			}
		}
		else if (rName == WarnLow)
		{
			value = GetIndirectValue(SvPb::RangeClassWarnLowIndirectEId);
			if (value.empty())
			{
				value = GetDirectValue(SvPb::RangeClassWarnLowEId);
			}
		}
		else
		{
			throw std::exception("Invalid Argument");
		}
		return value;
	}

	void RangeController::Set(const std::string& rName, const std::string& rValue)
	{
		if (rName == FailHigh)
		{
			m_RangeValues.Set<CString>(SvPb::RangeClassFailHighEId, rValue.c_str());
		}
		else if (rName == FailLow)
		{
			m_RangeValues.Set<CString>(SvPb::RangeClassFailLowEId, rValue.c_str());
		}
		else if (rName == WarnHigh)
		{
			m_RangeValues.Set<CString>(SvPb::RangeClassWarnHighEId, rValue.c_str());
		}
		else if (rName == WarnLow)
		{
			m_RangeValues.Set<CString>(SvPb::RangeClassWarnLowEId, rValue.c_str());
		}
		else
		{
			throw std::exception("Invalid Argument");
		}
	}

	std::string RangeController::GetIndirectValue(SvPb::EmbeddedIdEnum embeddedID) const
	{
		_bstr_t value = m_RangeValues.Get<_bstr_t>(embeddedID);
		return std::string(static_cast<LPCSTR>(value));
	}

	std::string RangeController::GetDirectValue(SvPb::EmbeddedIdEnum embeddedID) const
	{
		double value = m_RangeValues.Get<double>(embeddedID);
		std::stringstream ss;
		ss.precision(6);
		ss << std::fixed << value;
		return ss.str();
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

	#pragma region Protected Methods
	void RangeController::IsFieldValid(SvStl::MessageTextEnum fieldName, const std::string& rValue)
	{
		size_t len = rValue.size();

		if (!len)
		{
			SvDef::StringVector msgList;
			msgList.push_back(SvStl::MessageData::convertId2AdditionalText(fieldName));
			SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RangeValue_EmptyString, msgList, SvStl::SourceFileParams(StdMessageParams));
			throw message;
		}
		else
		{
			double val = 0.0;
			std::string tmp = rValue;
			bool isNumber = SvUl::Convert2Number<double>(tmp, val, true);
			if (isNumber)
			{
				const double s_RangeMax = 17000000;
				const double s_RangeMin = -s_RangeMax;
				if (val > s_RangeMax || val < s_RangeMin)
				{
					SvDef::StringVector msgList;
					msgList.push_back(SvStl::MessageData::convertId2AdditionalText(fieldName));
					msgList.push_back(SvUl::Format(_T("%d"), static_cast<int>(s_RangeMin)));
					msgList.push_back(SvUl::Format(_T("%d"), static_cast<int>(s_RangeMax)));
					SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RangeValue_WrongRange, msgList, SvStl::SourceFileParams(StdMessageParams));
					throw message;
				}
			}
		}
	}
} //namespace SvOg
