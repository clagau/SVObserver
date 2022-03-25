//*****************************************************************************
/// \copyright COPYRIGHT (c) 2016,2016 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file SVTriggerInfoStruct.h
/// All Rights Reserved
//*****************************************************************************
/// Contains the struct SVTriggerInfoStruct 
//******************************************************************************

#pragma once
#pragma region Includes
#include "Definitions/GlobalConst.h"
#pragma endregion Includes

namespace SvTrig
{
enum TriggerDataEnum : int
{
	TimeStamp = 0,
	ObjectID,
	TriggerIndex,
	TriggerPerObjectID,
	OutputData,
	TriggerChannel,
	ObjectType,
	SoftwareTrigger,
	TriggerDataNumber
};

static_assert(TriggerDataEnum::TriggerDataNumber == SvDef::cTriggerDataNumber, "The trigger data enum number must be the same size as the variable SvDef::cTriggerDataNumber");

using TriggerData = std::array<_variant_t, TriggerDataEnum::TriggerDataNumber>;
using TriggerCallBack = std::function<void(TriggerData&&)>;

struct SVTriggerInfoStruct
{

	SVTriggerInfoStruct() = default;
	SVTriggerInfoStruct(const SVTriggerInfoStruct& rRhs) = default;
	SVTriggerInfoStruct(SVTriggerInfoStruct&& rRhs) = default;
	virtual ~SVTriggerInfoStruct() = default;

	SVTriggerInfoStruct& operator=(const SVTriggerInfoStruct& rRhs) = default;
	SVTriggerInfoStruct& operator=(SVTriggerInfoStruct&& rRhs) = default;

	void clear() { *this = {}; }

	bool bValid{false};
	long lTriggerCount{0L};
	bool m_ToggleState{false};
	double m_triggerTimeStamp{0.0};
	double m_ToggleTimeStamp{0.0};
	double m_PreviousTrigger{0.0};
	TriggerData m_Data;
	std::vector<_variant_t> m_Inputs;
};

} //namespace SvTrig
