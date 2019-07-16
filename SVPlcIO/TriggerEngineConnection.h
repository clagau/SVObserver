/// \copyright COPYRIGHT (c) 2017, 2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \file TriggerEngineConnection.h
/// \brief the interface to the TriggerEngine: declarations

#pragma once

#include <functional>

struct TriggerReport
{
	TriggerReport(uint8_t channel, uint32_t objectID, uint32_t triggerIndex, double triggerTime_ms, bool isComplete)
		: m_channel {channel}
		, m_objectID {objectID}
		, m_triggerIndex {triggerIndex}
		, m_triggerTime_ms {triggerTime_ms}
		, m_isComplete {isComplete}
	{
	}
	uint8_t m_channel = 0;
	uint32_t m_objectID = 0;
	uint32_t m_triggerIndex = 0;
	double m_triggerTime_ms = 0.0;
	bool m_isComplete = false;
};

enum class NotifyType
{
	None,
	StopLoop,
	EmptyPPQs,
};

inline void doNothingNotify(NotifyType type)
{
}


struct ResultReport
{
	ResultReport(uint8_t channel, uint32_t oid, uint8_t result, uint16_t outbits) : 
		m_channel(channel), m_objectID(oid), m_result(result), m_outbits(outbits) {}
	uint8_t m_channel = 0;
	uint32_t m_objectID = 0;
	uint8_t m_result = 0;
	uint16_t m_outbits = 0;
};


void startTriggerchannel(uint8_t channel);
void stopTriggerchannel(uint8_t channel);

void requestTriggerEngineStop();

void writeResult(const ResultReport& rRr);
void ejectWaitingProducts(uint8_t channel);

void startTriggerEngine(std::function<void(const TriggerReport&)> reportTrigger,
	uint16_t simulateTriggers,
	std::function<void(NotifyType)> onNotify = doNothingNotify);

void stopTriggerEngine();



