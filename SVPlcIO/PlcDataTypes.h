//*****************************************************************************
/// \copyright (c) 2019,2019 by Seidenader Maschinenbau GmbH
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
constexpr uint8_t c_majorVersion = 1;
constexpr uint8_t c_minorVersion = 2;

constexpr unsigned int c_NumberOfChannels = 4; /// the number of trigger channels
constexpr unsigned int c_ConfigListSize = 62; /// The config list size
constexpr uint8_t c_ResultSize = 8;
constexpr uint8_t c_HeaderSize = 8;
constexpr uint8_t c_modeSingleDirect = 1;

struct TriggerReport
{
	bool isValid() const { return m_isValid; }
	uint8_t m_channel {0};
	uint32_t m_objectID {0UL};
	int8_t m_sequence {0};
	uint32_t m_triggerIndex {0UL};
	uint8_t m_triggersPerProduct {0};  /// the number of triggers for one product (objectID)
	double m_triggerTimestamp {0.0};
	bool m_isComplete {false};
	int32_t m_syncSoc {0L};
	int32_t m_relativeSoc {0L};
	int32_t m_timeStampSoc {0L};
	bool m_isValid{false};
};

struct ResultReport
{
	uint8_t m_channel = 0;
	uint32_t m_objectID = 0;
	std::array<uint8_t, c_ResultSize> m_results {0, 0, 0, 0, 0, 0, 0, 0};
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
