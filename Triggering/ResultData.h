//*****************************************************************************
/// \copyright COPYRIGHT (c) 2023,2023 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file ResultData.h
/// All Rights Reserved
//*****************************************************************************
/// Contains the struct ResultData
//******************************************************************************
#pragma once

constexpr uint8_t cResultSize = 14;
constexpr uint8_t cSerialCodeSize = 5;

namespace SvTrig
{

struct ResultData
{
	uint8_t m_channel {0};
	uint32_t m_objectID {0UL};
	uint8_t m_objectType {0};
	//Do not use std::array due to struct being used over Dll boundary
	uint8_t m_results[cResultSize] {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0, 0, 0};
	double m_measurementValue {0.0};
	//Do not use std::array due to struct being used over Dll boundary
	uint32_t m_serializationCode[cSerialCodeSize] {0UL, 0UL, 0UL, 0UL, 0UL};
};
} //namespace SvTrig