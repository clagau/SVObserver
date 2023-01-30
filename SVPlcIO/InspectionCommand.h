//*****************************************************************************
/// \copyright (c) 2019,2019 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \file InspectionCommand.h
//*****************************************************************************
/// The header of the inspection command and channels
//******************************************************************************
#pragma once

#pragma region Includes
#include "PlcDataTypes.h"
#include "Triggering/TriggerData.h"
#pragma endregion Includes

#pragma region Declarations
constexpr uint8_t cReservedInsCmd = 7;
#pragma endregion Declarations

namespace SvPlc
{
#pragma pack(push, 1)
struct ChannelIn1
{
public:
	ChannelIn1() = default;
	~ChannelIn1() = default;
	auto operator<=>(const ChannelIn1&) const = default;
#pragma region Member Variables
public:
	bool m_activation {false};				//Only for HV
	bool m_enable {false};					//Only for HV
	bool m_loopMode {false};				//True when loop mode is activated
	uint8_t m_unitControl{ 0 };				//When true then do inspection
	int8_t m_sequence{ 0 };
	int16_t m_timeStamp{ 0 };
	uint8_t m_triggerIndex {0};
	uint8_t m_triggerCount {0};
	uint8_t m_objectType{ 0 };
	std::array <uint32_t, cObjectMaxNr> m_objectID {0UL, 0UL, 0UL, 0UL};
	std::array <uint8_t, cObjectMaxNr> m_rotationNr {0, 0, 0, 0};
	std::array <float, cObjectMaxNr> m_measurementValue {0.0, 0.0, 0.0, 0.0};
#pragma endregion Member Variables
};

struct InspectionCommand1
{
public:
	InspectionCommand1() = default;
	~InspectionCommand1() = default;
#pragma region Member Variables
public:
	int32_t m_socRelative{ 0L };				//Relative SOC time
	std::array<ChannelIn1, cNumberOfChannels> m_channels;	//In data for each of the 4 separate channels
#pragma endregion Member Variables
};

struct ChannelIn2
{
public:
	ChannelIn2() = default;
	~ChannelIn2() = default;
	auto operator<=>(const ChannelIn2&) const = default;
#pragma region Member Variables
public:
	uint8_t m_unitControl {0};				//When true then do inspection
	int8_t m_sequence {0};					//Sequence number
	int16_t m_timeStamp1 {0};				//Time stamp 1
	int16_t m_timeStamp2 {0};				//Time stamp 2
	uint8_t m_currentObjectType {0};		//Current object type
	uint32_t m_currentObjectID {0};			//Current object ID
	uint8_t m_previousObjectType {0};		//Previous object type
	uint32_t m_previousObjectID {0};		//Previous object ID
	uint8_t m_triggerIndex {0};				//Trigger index
	uint8_t m_triggerCount {0};				//Trigger count per object
	int32_t m_socTriggerTime {0L};			//SOC trigger time
#pragma endregion Member Variables
};

struct InspectionCommand2
{
public:
	InspectionCommand2() = default;
	~InspectionCommand2() = default;

#pragma region Member Variables
public:
	bool m_enable {false};					//Inspection system enabled when true
	uint32_t m_socAbsSeconds {0UL};			//SOC time seconds
	uint32_t m_socAbsNanoseconds {0UL};		//SOC time nano seconds
	int32_t m_socRelative {0L};				//Relative SOC time
	std::array<uint8_t, cReservedInsCmd>  m_reserved {0, 0, 0, 0, 0, 0, 0};	//Reserved data
	std::array<ChannelIn2, cNumberOfChannels> m_channels;	//In data for each of the 4 separate channels
#pragma endregion Member Variables
};
#pragma pack(pop)
} //namespace SvPlc
