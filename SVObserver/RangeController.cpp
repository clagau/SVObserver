//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Gui Controller for the Range Value Objects.
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include <string>
#include <sstream>
#include <boost/assign/list_of.hpp>
#include <boost/lexical_cast.hpp>
#include "RangeController.h"
#include "GuiCommands\GetObjectName.h"
#include "GuiCommands\GetPPQObjectName.h"
#include "ObjectSelectorLibrary\ObjectTreeGenerator.h"
#include "SVClsids.h"
#include "GlobalSelector.h"
#include "PPQNameSelector.h"
#include "NoSelector.h"
#include "ToolSetItemSelector.h"
#include "RangeSelectorFilter.h"
#include "RangeValidator.h"
#pragma endregion Includes

#pragma region Declarations
const std::string RangeController::FailHigh = "FailHigh";
const std::string RangeController::FailLow = "FailLow";
const std::string RangeController::WarnHigh = "WarnHigh";
const std::string RangeController::WarnLow = "WarnLow";
static const std::string FailHighIndirect = "FailHighIndirect";
static const std::string FailLowIndirect = "FailLowIndirect";
static const std::string WarnHighIndirect = "WarnHighIndirect";
static const std::string WarnLowIndirect = "WarnLowIndirect";
#pragma endregion Declarations

RangeController::RangeController(const GUID& rInspectionID, const GUID& rTaskObjectID)
: m_InspectionID(rInspectionID)
, m_TaskObjectID(rTaskObjectID)
, m_directRangeValues(SvoGui::BoundValues(rInspectionID, rTaskObjectID, boost::assign::map_list_of
						(FailHigh, SVRangeClassFailHighObjectGuid)
						(FailLow, SVRangeClassFailLowObjectGuid)
						(WarnHigh, SVRangeClassWarnHighObjectGuid)
						(WarnLow, SVRangeClassWarnLowObjectGuid)))
, m_indirectRangeValues(SvoGui::BoundValues(rInspectionID, rTaskObjectID, boost::assign::map_list_of
						(FailHighIndirect, SVRangeClassFailHighIndirectObjectGuid)
						(WarnHighIndirect, SVRangeClassWarnHighIndirectObjectGuid)
						(FailLowIndirect, SVRangeClassFailLowIndirectObjectGuid)
						(WarnLowIndirect, SVRangeClassWarnLowIndirectObjectGuid)))
{
}

RangeController::~RangeController()
{
}

void RangeController::Init()
{
	m_directRangeValues.Init();
	m_indirectRangeValues.Init();
}

void RangeController::Commit()
{
	m_indirectRangeValues.Commit();
	m_directRangeValues.Commit();
}

std::string RangeController::Get(const std::string& rName) const
{
	std::string value;
	// Check if indirect is empty
	if (rName == FailHigh)
	{
		value = GetIndirectValue(FailHighIndirect);
		if (value.empty())
		{
			value = GetDirectValue(FailHigh);
		}
	}
	else if (rName == FailLow)
	{
		value = GetIndirectValue(FailLowIndirect);
		if (value.empty())
		{
			value = GetDirectValue(FailLow);
		}
	}
	else if (rName == WarnHigh)
	{
		value = GetIndirectValue(WarnHighIndirect);
		if (value.empty())
		{
			value = GetDirectValue(WarnHigh);
		}
	}
	else if (rName == WarnLow)
	{
		value = GetIndirectValue(WarnLowIndirect);
		if (value.empty())
		{
			value = GetDirectValue(WarnLow);
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
			SetIndirectValue(FailHighIndirect, std::string());
			SetDirectValue(FailHigh, rValue);
		}
		else
		{
			SetIndirectValue(FailHighIndirect, rValue);
		}
	}
	else if (rName == FailLow)
	{
		if (isNumeric(rValue))
		{
			SetIndirectValue(FailLowIndirect, std::string());
			SetDirectValue(FailLow, rValue);
		}
		else
		{
			SetIndirectValue(FailLowIndirect, rValue);
		}
	}
	else if (rName == WarnHigh)
	{
		if (isNumeric(rValue))
		{
			SetIndirectValue(WarnHighIndirect, std::string());
			SetDirectValue(WarnHigh, rValue);
		}
		else
		{
			SetIndirectValue(WarnHighIndirect, rValue);
		}
	}
	else if (rName == WarnLow)
	{
		if (isNumeric(rValue))
		{
			SetIndirectValue(WarnLowIndirect, std::string());
			SetDirectValue(WarnLow, rValue);
		}
		else
		{
			SetIndirectValue(WarnLowIndirect, rValue);
		}
	}
	else
	{
		throw std::exception("Invalid Argument");
	}
}

std::string RangeController::GetIndirectValue(const std::string& rName) const
{
	_bstr_t value = m_indirectRangeValues.Get<_bstr_t>(rName);
	return std::string(static_cast<LPCSTR>(value));
}

std::string RangeController::GetDirectValue(const std::string& rName) const
{
	double value = m_directRangeValues.Get<double>(rName);
	std::stringstream ss;
	ss.precision(6);
	ss << std::fixed << value;
	return ss.str();
}

void RangeController::SetIndirectValue(const std::string& rName, const std::string& rValue)
{
	m_indirectRangeValues.Set<LPCSTR>(rName, rValue.c_str());
}

void RangeController::SetDirectValue(const std::string& rName, const std::string& rValue)
{
	double value = boost::lexical_cast<double>(rValue);
	m_directRangeValues.Set<double>(rName, value);
}

void RangeController::FillObjectSelector()
{
	SVString InspectionName = GetInspectionName();
	SVString PPQName = GetPPQName();

	SvOsl::ObjectTreeGenerator::Instance().setAttributeFilters( SV_SELECTABLE_FOR_EQUATION );
	SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterInput, InspectionName, SVString( _T("") ) );
	SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterOutput, InspectionName, SVString( _T("") ) );

	SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterOutput, PPQName, SVString( _T("")  ));
	SvOsl::ObjectTreeGenerator::Instance().setSelectorType( SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeSingleObject );

	SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<GlobalSelector, NoSelector, NoSelector, ToolSetItemSelector<false, RangeSelectorFilter>>(m_InspectionID, m_TaskObjectID);
}

SVString RangeController::GetOwnerName() const
{
	SVString name;
	typedef GuiCmd::GetObjectName Command;
	typedef SVSharedPtr<Command> CommandPtr;

	CommandPtr commandPtr(new Command(m_TaskObjectID));
	SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
	HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
	if (S_OK == hr)
	{
		name = commandPtr->GetName();
	}
	return name;
}

#pragma region Protected Methods
SVString RangeController::GetInspectionName() const
{
	SVString inspectionName;
	typedef GuiCmd::GetObjectName Command;
	typedef SVSharedPtr<Command> CommandPtr;

	CommandPtr commandPtr(new Command(m_InspectionID));
	SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
	HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
	if (S_OK == hr)
	{
		inspectionName = commandPtr->GetName();
	}
	return inspectionName;
}

SVString RangeController::GetPPQName() const
{
	SVString PPQName;
	typedef GuiCmd::GetPPQObjectName Command;
	typedef SVSharedPtr<Command> CommandPtr;

	CommandPtr commandPtr(new Command(m_InspectionID));
	SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
	HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
	if (S_OK == hr)
	{
		PPQName = commandPtr->GetName();
	}
	return PPQName;
}

HRESULT RangeController::IsFieldValid(SVString& rMsg, const SVString& rFieldName, const SVString& rValue)
{
	return RangeValidator::IsFieldValid(rMsg, rFieldName, rValue);
}

HRESULT RangeController::Validate(SVString& msg, HINSTANCE resHandle)
{
	SVString InspectionName = GetInspectionName();

	const SVString& FailHighIndirectValue = GetIndirectValue(FailHighIndirect);
	const SVString& FailLowIndirectValue = GetIndirectValue(FailLowIndirect);
	const SVString& WarnHighIndirectValue = GetIndirectValue(WarnHighIndirect);
	const SVString& WarnLowIndirectValue = GetIndirectValue(WarnLowIndirect);
	double FailHighValue = m_directRangeValues.Get<double>(FailHigh);
	double FailLowValue = m_directRangeValues.Get<double>(FailLow);
	double WarnHighValue = m_directRangeValues.Get<double>(WarnHigh);
	double WarnLowValue = m_directRangeValues.Get<double>(WarnLow);

	return RangeValidator::Validate(msg, resHandle, InspectionName, FailHighIndirectValue, FailLowIndirectValue, WarnHighIndirectValue, WarnLowIndirectValue, FailHighValue, FailLowValue, WarnHighValue, WarnLowValue, m_InspectionID);
}

