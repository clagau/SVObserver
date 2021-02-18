//*****************************************************************************
/// \copyright (c) 2019,2019 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file InspectionCommand.h
//*****************************************************************************
/// The header of the inspection command and channels
//******************************************************************************
#pragma once

#pragma region Includes
#include <stdint.h>
#include "PlcDataTypes.h"
#pragma endregion Includes

#pragma region Declarations
constexpr uint8_t c_ReservedInsCmd = 7;
#pragma endregion Declarations

namespace SvPlc
{
#pragma pack(push, 1)
struct ChannelIn1
{
public:
	ChannelIn1() = default;
	~ChannelIn1() = default;
#pragma region Member Variables
public:
	uint8_t m_unitControl{ 0 };				//When true then do inspection
	int16_t m_timeStamp{ 0 };				//Time stamp
	uint8_t m_objectType{ 0 };				//Object type
	uint32_t m_objectID{ 0 };				//Object ID
	uint8_t m_triggerIndex{ 0 };			//Trigger index
	uint8_t m_triggerCount{ 0 };			//Trigger count per object
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
	std::array<uint8_t, c_ReservedInsCmd>  m_reserved {0, 0, 0, 0, 0, 0, 0};	//Reserved data
	std::array<ChannelIn2, cNumberOfChannels> m_channels;	//In data for each of the 4 separate channels
#pragma endregion Member Variables
};
#pragma pack(pop)
} //namespace SvPlc
