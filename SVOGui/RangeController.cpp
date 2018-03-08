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
#include "InspectionCommands\GetObjectName.h"
#include "InspectionCommands\GetPPQObjectName.h"
#include "ObjectSelectorLibrary\ObjectTreeGenerator.h"
#include "SVObjectLibrary\SVClsIds.h"
#include "SVObjectLibrary\SVObjectSynchronousCommandTemplate.h"
#include "GlobalSelector.h"
#include "NoSelector.h"
#include "ToolSetItemSelector.h"
#include "RangeValidator.h"
#pragma endregion Includes

namespace SvOg
{
#pragma region Declarations
	const std::string RangeController::FailHigh = "FailHigh";
	const std::string RangeController::FailLow = "FailLow";
	const std::string RangeController::WarnHigh = "WarnHigh";
	const std::string RangeController::WarnLow = "WarnLow";
#pragma endregion Declarations

	RangeController::RangeController(const GUID& rInspectionID, const GUID& rTaskObjectID)
		: m_rInspectionID(rInspectionID)
		, m_rTaskObjectID(rTaskObjectID)
		//TaskID is set later
		, m_RangeValues{SvOg::BoundValues{rInspectionID, GUID_NULL}}
	{
	}

	RangeController::~RangeController()
	{
	}

	void RangeController::Init()
	{
		SvDef::SVObjectTypeInfoStruct ObjectInfo;
		ObjectInfo.ObjectType = SvDef::SVRangeObjectType;
		ObjectInfo.SubType = SvDef::SVNotSetSubObjectType;
		SvOi::IObjectClass* pTool = SvOi::getObject(m_rTaskObjectID);
		if (nullptr != pTool)
		{
			SvOi::IObjectClass* pRange = pTool->getFirstObject(ObjectInfo);
			if (nullptr != pRange)
			{
				m_RangeValues.SetTaskID(pRange->GetUniqueObjectID());
				m_RangeValues.Init();
			}
		}
	}

	void RangeController::Commit()
	{
		m_RangeValues.Commit(SvOg::doResetRunOnce);
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

	static bool isNumeric(const std::string& rValue)
	{
	   return !rValue.empty() && std::string::npos == rValue.find_first_not_of("-.0123456789");
	}

	void RangeController::Set(const std::string& rName, const std::string& rValue)
	{
		if (rName == FailHigh)
		{
			if (isNumeric(rValue))
			{
				SetIndirectValue(SVRangeClassFailHighIndirectObjectGuid, std::string());
				SetDirectValue(SVRangeClassFailHighObjectGuid, rValue);
			}
			else
			{
				SetIndirectValue(SVRangeClassFailHighIndirectObjectGuid, rValue);
			}
		}
		else if (rName == FailLow)
		{
			if (isNumeric(rValue))
			{
				SetIndirectValue(SVRangeClassFailLowIndirectObjectGuid, std::string());
				SetDirectValue(SVRangeClassFailLowObjectGuid, rValue);
			}
			else
			{
				SetIndirectValue(SVRangeClassFailLowIndirectObjectGuid, rValue);
			}
		}
		else if (rName == WarnHigh)
		{
			if (isNumeric(rValue))
			{
				SetIndirectValue(SVRangeClassWarnHighIndirectObjectGuid, std::string());
				SetDirectValue(SVRangeClassWarnHighObjectGuid, rValue);
			}
			else
			{
				SetIndirectValue(SVRangeClassWarnHighIndirectObjectGuid, rValue);
			}
		}
		else if (rName == WarnLow)
		{
			if (isNumeric(rValue))
			{
				SetIndirectValue(SVRangeClassWarnLowIndirectObjectGuid, std::string());
				SetDirectValue(SVRangeClassWarnLowObjectGuid, rValue);
			}
			else
			{
				SetIndirectValue(SVRangeClassWarnLowIndirectObjectGuid, rValue);
			}
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

	void RangeController::SetIndirectValue(const GUID& rEmbeddedID, const std::string& rValue)
	{
		m_RangeValues.Set<LPCSTR>(rEmbeddedID, rValue.c_str());
	}

	void RangeController::SetDirectValue(const GUID& rEmbeddedID, const std::string& rValue)
	{
		double value = boost::lexical_cast<double>(rValue);
		m_RangeValues.Set<double>(rEmbeddedID, value);
	}

	std::string RangeController::GetOwnerName() const
	{
		std::string name;
		typedef SvCmd::GetObjectName Command;
		typedef std::shared_ptr<Command> CommandPtr;

		CommandPtr commandPtr(new Command(m_rTaskObjectID));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_rInspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			name = commandPtr->GetName();
		}
		return name;
	}

	#pragma region Protected Methods
	std::string RangeController::GetInspectionName() const
	{
		std::string inspectionName;
		typedef SvCmd::GetObjectName Command;
		typedef std::shared_ptr<Command> CommandPtr;

		CommandPtr commandPtr(new Command(m_rInspectionID));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_rInspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			inspectionName = commandPtr->GetName();
		}
		return inspectionName;
	}

	std::string RangeController::GetPPQName() const
	{
		std::string PPQName;
		typedef SvCmd::GetPPQObjectName Command;
		typedef std::shared_ptr<Command> CommandPtr;

		CommandPtr commandPtr(new Command(m_rInspectionID));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_rInspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			PPQName = commandPtr->GetName();
		}
		return PPQName;
	}

	void RangeController::IsFieldValid(SvStl::MessageTextEnum fieldName, const std::string& rValue)
	{
		RangeValidator::IsFieldValid(fieldName, rValue);
	}

	void RangeController::Validate()
	{
		std::string InspectionName = GetInspectionName();

		const std::string& FailHighIndirectValue = GetIndirectValue(SVRangeClassFailHighIndirectObjectGuid);
		const std::string& FailLowIndirectValue = GetIndirectValue(SVRangeClassFailLowIndirectObjectGuid);
		const std::string& WarnHighIndirectValue = GetIndirectValue(SVRangeClassWarnHighIndirectObjectGuid);
		const std::string& WarnLowIndirectValue = GetIndirectValue(SVRangeClassWarnLowIndirectObjectGuid);
		double FailHighValue = m_RangeValues.Get<double>(SVRangeClassFailHighObjectGuid);
		double FailLowValue = m_RangeValues.Get<double>(SVRangeClassFailLowObjectGuid);
		double WarnHighValue = m_RangeValues.Get<double>(SVRangeClassWarnHighObjectGuid);
		double WarnLowValue = m_RangeValues.Get<double>(SVRangeClassWarnLowObjectGuid);

		RangeValidator::Validate(InspectionName, FailHighIndirectValue, FailLowIndirectValue, WarnHighIndirectValue, WarnLowIndirectValue, FailHighValue, FailLowValue, WarnHighValue, WarnLowValue, m_rInspectionID);
	}
} //namespace SvOg
