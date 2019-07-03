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
#include "Definitions/SVObjectTypeInfoStruct.h"
#include "ObjectInterfaces/IObjectClass.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "SVObjectLibrary/SVClsIds.h"
#include "SVProtoBuf/ConverterHelper.h"
#pragma endregion Includes

namespace SvOg
{
#pragma region Declarations
	const std::string RangeController::FailHigh = "FailHigh";
	const std::string RangeController::FailLow = "FailLow";
	const std::string RangeController::WarnHigh = "WarnHigh";
	const std::string RangeController::WarnLow = "WarnLow";
#pragma endregion Declarations

	RangeController::RangeController(const GUID& rInspectionID, const GUID& rTaskObjectID, const GUID& rRangeID)
		: m_rInspectionID(rInspectionID)
		, m_rTaskObjectID(rTaskObjectID)
		, m_RangeID(rRangeID)
		//TaskID is set later
		, m_RangeValues{SvOg::BoundValues{rInspectionID, GUID_NULL}}
	{
	}

	RangeController::~RangeController()
	{
	}

	void RangeController::Init()
	{
		if (GUID_NULL == m_RangeID)
		{
			SvDef::SVObjectTypeInfoStruct ObjectInfo;
			ObjectInfo.ObjectType = SvPb::SVRangeObjectType;
			ObjectInfo.SubType = SvPb::SVNotSetSubObjectType;
			SvOi::IObjectClass* pTool = SvOi::getObject(m_rTaskObjectID);
			if (nullptr != pTool)
			{
				SvOi::IObjectClass* pRange = pTool->getFirstObject(ObjectInfo);
				if (nullptr != pRange)
				{
					m_RangeID = pRange->GetUniqueObjectID();
				}
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
			value = GetIndirectValue(SVRangeClassFailHighIndirectObjectGuid);
			if (value.empty())
			{
				value = GetDirectValue(SVRangeClassFailHighObjectGuid);
			}
		}
		else if (rName == FailLow)
		{
			value = GetIndirectValue(SVRangeClassFailLowIndirectObjectGuid);
			if (value.empty())
			{
				value = GetDirectValue(SVRangeClassFailLowObjectGuid);
			}
		}
		else if (rName == WarnHigh)
		{
			value = GetIndirectValue(SVRangeClassWarnHighIndirectObjectGuid);
			if (value.empty())
			{
				value = GetDirectValue(SVRangeClassWarnHighObjectGuid);
			}
		}
		else if (rName == WarnLow)
		{
			value = GetIndirectValue(SVRangeClassWarnLowIndirectObjectGuid);
			if (value.empty())
			{
				value = GetDirectValue(SVRangeClassWarnLowObjectGuid);
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
			m_RangeValues.Set<CString>(SVRangeClassFailHighObjectGuid, rValue.c_str());
		}
		else if (rName == FailLow)
		{
			m_RangeValues.Set<CString>(SVRangeClassFailLowObjectGuid, rValue.c_str());
		}
		else if (rName == WarnHigh)
		{
			m_RangeValues.Set<CString>(SVRangeClassWarnHighObjectGuid, rValue.c_str());
		}
		else if (rName == WarnLow)
		{
			m_RangeValues.Set<CString>(SVRangeClassWarnLowObjectGuid, rValue.c_str());
		}
		else
		{
			throw std::exception("Invalid Argument");
		}
	}

	std::string RangeController::GetIndirectValue(const GUID& rEmbeddedID) const
	{
		_bstr_t value = m_RangeValues.Get<_bstr_t>(rEmbeddedID);
		return std::string(static_cast<LPCSTR>(value));
	}

	std::string RangeController::GetDirectValue(const GUID& rEmbeddedID) const
	{
		double value = m_RangeValues.Get<double>(rEmbeddedID);
		std::stringstream ss;
		ss.precision(6);
		ss << std::fixed << value;
		return ss.str();
	}

	std::string RangeController::GetOwnerName() const
	{
		std::string name;
		SvPb::InspectionCmdMsgs request, response;
		SvPb::GetObjectParametersRequest* pGetObjectNameRequest = request.mutable_getobjectparametersrequest();

		SvPb::SetGuidInProtoBytes(pGetObjectNameRequest->mutable_objectid(), m_rTaskObjectID);
		HRESULT hr = SvCmd::InspectionCommands(m_rInspectionID, request, &response);
		if (S_OK == hr && response.has_getobjectparametersresponse())
		{
			name = response.getobjectparametersresponse().name();
		}
		return name;
	}

	#pragma region Protected Methods
	std::string RangeController::GetInspectionName() const
	{
		std::string inspectionName;
		SvPb::InspectionCmdMsgs request, response;
		SvPb::GetObjectParametersRequest* pGetObjectNameRequest = request.mutable_getobjectparametersrequest();

		SvPb::SetGuidInProtoBytes(pGetObjectNameRequest->mutable_objectid(), m_rInspectionID);
		HRESULT hr = SvCmd::InspectionCommands(m_rInspectionID, request, &response);
		if (S_OK == hr && response.has_getobjectparametersresponse())
		{
			inspectionName = response.getobjectparametersresponse().name();
		}
		return inspectionName;
	}

	void RangeController::IsFieldValid(SvStl::MessageTextEnum fieldName, const std::string& rValue)
	{
		size_t len = rValue.size();

		if (!len)
		{
			SvDef::StringVector msgList;
			msgList.push_back(SvStl::MessageData::convertId2AddtionalText(fieldName));
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
					msgList.push_back(SvStl::MessageData::convertId2AddtionalText(fieldName));
					msgList.push_back(SvUl::Format(_T("%d"), static_cast<int>(s_RangeMin)));
					msgList.push_back(SvUl::Format(_T("%d"), static_cast<int>(s_RangeMax)));
					SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RangeValue_WrongRange, msgList, SvStl::SourceFileParams(StdMessageParams));
					throw message;
				}
			}
		}
	}
} //namespace SvOg
