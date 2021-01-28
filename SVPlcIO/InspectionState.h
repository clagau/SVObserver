//*****************************************************************************
/// \copyright (c) 2019,2019 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file InspectionState.h
//*****************************************************************************
/// The header of the inspection state
//******************************************************************************
#pragma once

#pragma region Includes
#include <stdint.h>
#include "PlcDataTypes.h"
#pragma endregion Includes

namespace SvPlc
{
#pragma pack(push, 1)

struct ChannelOut
{
public:
	ChannelOut() = default;
	~ChannelOut() = default;

#pragma region Member Variables
public:
	uint8_t m_currentObjectType {0};		//Current object type
	uint32_t m_currentObjectID {0};			//Current object ID
	std::array<uint8_t, cResultSize> m_results
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};	//Results array
#pragma endregion Member Variables
};

struct InspectionState1
{
public:
	InspectionState1() = default;
	~InspectionState1() = default;

#pragma region Member Variables
public:
	std::array<ChannelOut, cNumberOfChannels> m_channels;	//Out data for each of the 4 separate channels
#pragma endregion Member Variables
};

struct InspectionState2
{
public:
	InspectionState2() = default;
	~InspectionState2() = default;

#pragma region Member Variables
public:
	std::array<uint8_t, cHeaderSize> m_header {0, 0, 0, 0, 0, 0, 0, 0};
	std::array<ChannelOut, cNumberOfChannels> m_channels;	//Out data for each of the 4 separate channels
#pragma endregion Member Variables
};

#pragma pack(pop)
} //namespace SvPlc
