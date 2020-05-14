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

struct ChannelIn
{
public:
	ChannelIn() = default;
	~ChannelIn() = default;
	explicit ChannelIn(const ChannelIn& rRhs) = default;
	explicit ChannelIn(ChannelIn&& rRhs) = default;
	ChannelIn& operator=(const ChannelIn& rRhs) = default;
	ChannelIn& operator=(ChannelIn&& rRhs) = default;
	bool operator!=(const ChannelIn& rRhs)
	{
		///At the moment ignore comparing m_socTriggerTime
		if(m_unitControl != rRhs.m_unitControl ||
		   m_sequence != rRhs.m_sequence ||
		   m_timeStamp1 != rRhs.m_timeStamp1 ||
		   m_timeStamp2 != rRhs.m_timeStamp2 ||
		   m_currentObjectType != rRhs.m_currentObjectType ||
		   m_currentObjectID != rRhs.m_currentObjectID ||
		   m_previousObjectType != rRhs.m_previousObjectType ||
		   m_previousObjectID != rRhs.m_previousObjectID ||
		   m_triggerIndex != rRhs.m_triggerIndex ||
		   m_triggerCount != rRhs.m_triggerCount)
		{
			return true;
		}
		return false;
	}

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

struct InspectionCommand
{
public:
	InspectionCommand() = default;
	~InspectionCommand() = default;
	explicit InspectionCommand(const InspectionCommand& rRhs) = default;
	explicit InspectionCommand(InspectionCommand&& rRhs) = default;
	InspectionCommand& operator=(const InspectionCommand& rRhs) = default;
	InspectionCommand& operator=(InspectionCommand&& rRhs) = default;

	bool hasTriggerDataChanged(const InspectionCommand& rRhs)
	{
		for(int i=0; i < cNumberOfChannels; ++i)
		{
			if(m_channels[i] != rRhs.m_channels[i])
			{
				return true;
			}
		}
		return false;
	}

#pragma region Member Variables
public:
	bool m_enable {false};					//Inspection system enabled when true
	uint32_t m_socAbsSeconds {0UL};			//SOC time seconds
	uint32_t m_socAbsNanoseconds {0UL};		//SOC time nano seconds
	int32_t m_socRelative {0L};				//Relative SOC time
	std::array<uint8_t, c_ReservedInsCmd>  m_reserved {0, 0, 0, 0, 0, 0, 0};	//Reserved data
	std::array<ChannelIn, cNumberOfChannels> m_channels;	//In data for each of the 4 separate channels
#pragma endregion Member Variables
};
#pragma pack(pop)
} //namespace SvPlc
