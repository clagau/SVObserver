//*****************************************************************************
/// \copyright (c) 2019,2019 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file TimeSync.h
//*****************************************************************************
/// The header of the time synchronization data
//******************************************************************************
#pragma once

#pragma region Includes
#include <stdint.h>
#pragma endregion Includes

namespace SvPlc
{
#pragma pack(push, 1)
struct TimeSync
{
public:
	TimeSync() = default;
	~TimeSync() = default;
	explicit TimeSync(const TimeSync& rRhs) = default;
	explicit TimeSync(TimeSync&& rRhs) = default;
	TimeSync& operator=(const TimeSync& rRhs) = default;
	TimeSync& operator=(TimeSync&& rRhs) = default;

#pragma region Member Variables
public:
	uint8_t m_plcBusCycles {0UL};				//The number of PLC bus cycles
#pragma endregion Member Variables
};
#pragma pack(pop)
} //namespace SvPlc
