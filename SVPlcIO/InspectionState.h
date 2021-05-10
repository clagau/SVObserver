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

struct ChannelOut1
{
public:
	ChannelOut1() = default;
	~ChannelOut1() = default;

#pragma region Member Variables
public:
	uint8_t m_objectType{ 0 };				//Object type
	uint32_t m_objectID{ 0 };				//Object ID
	std::array<uint8_t, cResultSize> m_results	//Results array
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	std::array<float, cResultSize> m_measurementValues //Measurement value array
	{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
#pragma endregion Member Variables
};
	
struct InspectionState1
{
public:
	InspectionState1() = default;
	~InspectionState1() = default;

#pragma region Member Variables
public:
	std::array<ChannelOut1, cNumberOfChannels> m_channels;	//Out data for each of the 4 separate channels
#pragma endregion Member Variables
};

struct ChannelOut2
{
public:
	ChannelOut2() = default;
	~ChannelOut2() = default;

#pragma region Member Variables
public:
	uint8_t m_objectType{ 0 };				//Object type
	uint32_t m_objectID{ 0 };				//Object ID
	std::array<uint8_t, cResultSize> m_results
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };	//Results array
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
	std::array<ChannelOut2, cNumberOfChannels> m_channels;	//Out data for each of the 4 separate channels
#pragma endregion Member Variables
};

#pragma pack(pop)
} //namespace SvPlc
