//*****************************************************************************
/// \copyright COPYRIGHT (c) 2016,2016 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file SVTriggerInfoStruct.h
/// All Rights Reserved
//*****************************************************************************
/// Contains the struct SVTriggerInfoStruct 
//******************************************************************************

#pragma once
#pragma region Includes
#include "TriggerData.h"
#pragma endregion Includes

namespace SvTrig
{
using TriggerCallBack = std::function<void(TriggerData&&)>;

struct SVTriggerInfoStruct
{
	SVTriggerInfoStruct() = default;

	void clear() { *this = {}; }

	bool bValid{false};
	long lTriggerCount{0L};
	bool m_ToggleState{false};
	double m_triggerTimeStamp{0.0};
	double m_ToggleTimeStamp{0.0};
	double m_PreviousTrigger{0.0};
	bool m_softwareTrigger {false};
	TriggerData m_Data;
	std::vector<_variant_t> m_Inputs;
};

} //namespace SvTrig
