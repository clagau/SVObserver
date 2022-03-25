//*****************************************************************************
/// \copyright (c) 2019,2019 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \file PlcDataTypes.h
//*****************************************************************************
/// The header of the PLC data types
//******************************************************************************
#pragma once

#pragma region Includes
#include <stdint.h>
#pragma endregion Includes

namespace SvPlc
{

constexpr unsigned int cNumberOfChannels = 4; /// the number of trigger channels
constexpr unsigned int cConfigListSize = 62; /// The config list size
constexpr uint8_t cResultSize = 14;
constexpr uint8_t cHeaderSize = 8;
constexpr uint8_t cModeSingleDirect = 1;

struct TriggerReport
{
	uint8_t m_channel {0};
	uint32_t m_objectID{ 0UL };
	uint8_t m_objectType{ 0 };
	uint8_t m_triggerIndex{0};
	uint8_t m_triggerPerObjectID{ 0 };
	double m_triggerTimestamp{ 0.0 };
	bool m_isValid{false};
	std::string m_acquisitionFile;
};

struct ResultReport
{
	uint8_t m_channel{ 0 };
	uint32_t m_objectID{ 0UL };
	uint8_t m_objectType{ 0 };
	double m_timestamp{ 0.0 };
	std::array<uint8_t, cResultSize> m_results {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0, 0, 0};
};

enum PlcDataType : uint16_t
{
	NoneDataType = 0,
	BoolIndex = 1,
	Int8Index = 2,
	Int16Index = 3,
	Int32Index = 4,
	Uint8Index = 5,
	Uint16Index = 6,
	Uint32Index = 7,
	FloatIndex = 8,
	StringIndex = 9,
	WstringIndex = 10,
	DateTimeIndex = 11,
	TimeIndex = 12,
	DateIndex = 13,
	DoubleIndex = 14,
};

} //namespace SvPlc
