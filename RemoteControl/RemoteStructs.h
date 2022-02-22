//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file RemoteStructs.h
/// The structures required for the remote control
//******************************************************************************

#pragma once

namespace SvRc
{
struct Value
{
	Value() = default;
	Value(LPCTSTR name, HRESULT status, long count, _variant_t item) : m_name{ name }, m_status{ status }, m_count{ count }, m_item{ item } {}
	std::string m_name;
	HRESULT m_status{ S_OK };
	long m_count{ 0L };
	_variant_t m_item;
};

struct Product
{
	std::vector<Value> m_valueVector;
	std::vector<Value> m_imageVector;
	std::vector<Value> m_errorVector;
};

struct DataDefinition
{
	DataDefinition() = default;
	std::string m_name;
	bool m_writeable{ false };
	bool m_published{ false };
	std::string m_type;
	std::vector<std::string> m_additionalInfo;
};

struct MonitorList
{
	MonitorList() = default;
	std::string m_name;
	std::string m_ppqName;
	long m_rejectDepth{ 0L };
	std::vector<std::string> m_productVector;
	std::vector<std::string> m_rejectVector;
	std::vector<std::string> m_failStatusVector;
};

struct TriggerItem
{
	std::string m_inspectionName;
	long m_period{ 0L };
	long m_startObjectID{ 0L };
	long m_triggerPerObjectID{ 0L };
	unsigned long m_numberObjectID{ 0UL };
};
} //namespace SvRc