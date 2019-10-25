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

#pragma region Declarations
#pragma endregion Declarations

#pragma pack(push, 1)

struct ChannelOut
{
public:
	ChannelOut() = default;
	~ChannelOut() = default;
	explicit ChannelOut(const ChannelOut& rRhs) = default;
	explicit ChannelOut(ChannelOut&& rRhs) = default;
	ChannelOut& operator=(const ChannelOut& rRhs) = default;
	ChannelOut& operator=(ChannelOut&& rRhs) = default;

#pragma region Member Variables
public:
	TypeObjectDt m_currentObject;			//Current object
	TypeResults m_results;					//Results array
	float m_generalValue{0.0};				//General value
#pragma endregion Member Variables
};

struct InspectionState
{
public:
	InspectionState() = default;
	~InspectionState() = default;
	explicit InspectionState(const InspectionState& rRhs) = default;
	explicit InspectionState(InspectionState&& rRhs) = default;
	InspectionState& operator=(const InspectionState& rRhs) = default;
	InspectionState& operator=(InspectionState&& rRhs) = default;

#pragma region Member Variables
public:
	uint8_t m_header[c_HeaderSize] {0, 0, 0, 0, 0, 0, 0, 0};
	ChannelOut m_channels[c_NumberOfChannels];	//Out data for each of the 4 separate channels
#pragma endregion Member Variables
};

#pragma pack(pop)
