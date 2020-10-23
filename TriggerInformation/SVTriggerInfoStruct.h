//*****************************************************************************
/// \copyright COPYRIGHT (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file SVTriggerInfoStruct.h
/// All Rights Reserved
//*****************************************************************************
/// Contains the struct SVTriggerInfoStruct 
//******************************************************************************

#pragma once

namespace SvTi
{
typedef  std::map<int, _variant_t> IntVariantMap;
typedef std::function<void(const IntVariantMap&)> TriggerCallBack;

enum TriggerDataEnum : int
{
	TimeStamp = 1,
	ObjectID,
	TriggerIndex,
	TriggerPerObjectID,
	OutputData,
	TriggerChannel,
};


struct SVTriggerInfoStruct
{

	SVTriggerInfoStruct() = default;
	SVTriggerInfoStruct(const SVTriggerInfoStruct& rRhs) = default;
	SVTriggerInfoStruct(SVTriggerInfoStruct&& rRhs) = default;
	virtual ~SVTriggerInfoStruct() = default;

	SVTriggerInfoStruct& operator=(const SVTriggerInfoStruct& rRhs) = default;
	SVTriggerInfoStruct& operator=(SVTriggerInfoStruct&& rRhs) = default;

	bool bValid{false};
	long lTriggerCount{0L};
	bool m_ToggleState{false};
	double m_triggerTimeStamp{0.0};
	double m_ToggleTimeStamp{0.0};
	double m_PreviousTrigger{0.0};
	IntVariantMap m_Data;
	std::vector<_variant_t> m_Inputs;
};

} //namespace SvTi
