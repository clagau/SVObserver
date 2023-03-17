//*****************************************************************************
/// \copyright COPYRIGHT (c) 2023,2023 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file TriggerData.h
/// All Rights Reserved
//*****************************************************************************
/// Contains the struct TriggerData
//******************************************************************************
#pragma once

#pragma region Includes
#include "Definitions/GlobalConst.h"
#pragma endregion Includes

namespace SvTrig
{

struct ObjectData
{
	uint32_t m_objectID {0UL};
	uint8_t m_rotationNr {0};
	double m_measurementValue {0.0};
};

struct TriggerData
{
	uint8_t m_channel {0};
	uint8_t m_objectType {0};
	uint8_t m_triggerIndex {0};
	uint8_t m_triggerPerObjectID {0};
	bool m_loopMode {false};
	double m_triggerTimestamp {0.0};
	//Do not use std::array due to struct being used over Dll boundary
	ObjectData m_objectData[SvDef::cObjectIndexMaxNr];
	//Uses pointer to a char array as the struct is used over Dll boundary
	//note is only used for simulation mode and the pointer is continuously valid)
	const char* m_pAcquisitionFile {nullptr};
};

} //namespace SvTrig