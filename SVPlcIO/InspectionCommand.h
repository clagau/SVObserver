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
		if(m_unitControl != rRhs.m_unitControl ||
		   m_sequence != rRhs.m_sequence ||
		   m_timeStamp1 != rRhs.m_timeStamp1 ||
		   m_timeStamp2 != rRhs.m_timeStamp2 ||
		   m_currentObject != rRhs.m_currentObject ||
		   m_originalObject != rRhs.m_originalObject ||
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
	TypeObjectDt m_currentObject;			//Current object
	TypeObjectDt m_originalObject;			//Original object
	uint16_t m_triggerIndex {0};			//Trigger index
	uint8_t m_triggerCount {0};				//Trigger count per object
	float m_generalValue{0.0};				//General value
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
		for(int i=0; i < c_NumberOfChannels; ++i)
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
	TypeSocTime m_socAbsolute;				//Absolute SOC time seconds and nano seconds
	int32_t m_socRelative {0L};				//Relative SOC time
	uint8_t m_reserved[c_ReservedInsCmd] {0, 0, 0, 0, 0, 0, 0};	//Reserved data
	ChannelIn m_channels[c_NumberOfChannels];	//In data for each of the 4 separate channels
#pragma endregion Member Variables
};

#pragma pack(pop)
