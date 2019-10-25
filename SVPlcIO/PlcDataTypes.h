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

constexpr uint8_t c_majorVersion = 1;
constexpr uint8_t c_minorVersion = 2;

constexpr unsigned int c_NumberOfChannels = 4; /// the number of trigger channels
constexpr unsigned int c_ConfigListSize = 62; /// The config list size
constexpr uint8_t c_ResultSize = 8;
constexpr uint8_t c_HeaderSize = 8;
constexpr uint8_t c_modeSingleDirect = 1;

#pragma pack(push, 1)
struct TypeSocTime
{
public:
	TypeSocTime() = default;
	~TypeSocTime() = default;
	explicit TypeSocTime(const TypeSocTime& rRhs) = default;
	explicit TypeSocTime(TypeSocTime&& rRhs) = default;
	TypeSocTime& operator=(const TypeSocTime& rRhs) = default;
	TypeSocTime& operator=(TypeSocTime&& rRhs) = default;

#pragma region Member Variables
public:
	uint32_t m_socSeconds {0UL};			//SOC time seconds
	uint32_t m_socNanoseconds {0UL};		//SOC time nano seconds
#pragma endregion Member Variables
};

struct TypeObjectDt
{
public:
	TypeObjectDt() = default;
	~TypeObjectDt() = default;
	explicit TypeObjectDt(const TypeObjectDt& rRhs) = default;
	explicit TypeObjectDt(TypeObjectDt&& rRhs) = default;
	TypeObjectDt& operator=(const TypeObjectDt& rRhs) = default;
	TypeObjectDt& operator=(TypeObjectDt&& rRhs) = default;
	bool operator!=(const TypeObjectDt& rRhs)
	{
		if (m_ID != rRhs.m_ID ||
			m_type != rRhs.m_type)
		{
			return true;
		}
		return false;
	}

#pragma region Member Variables
public:
	uint32_t m_ID {0};						//Object ID
	uint8_t m_type {0};						//Object type
#pragma endregion Member Variables
};

struct TypeResults
{
public:
	TypeResults() = default;
	~TypeResults() = default;
	explicit TypeResults(const TypeResults& rRhs) = default;
	explicit TypeResults(TypeResults&& rRhs) = default;
	TypeResults& operator=(const TypeResults& rRhs) = default;
	TypeResults& operator=(TypeResults&& rRhs) = default;

#pragma region Member Variables
public:
	uint8_t m_value[c_ResultSize] {0, 0, 0, 0, 0, 0, 0, 0};
#pragma endregion Member Variables
};
#pragma pack(pop)

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
	SocTimeIndex = 51,
	ObjectDtIndex = 52,
	ResultsIndex = 53,
};



