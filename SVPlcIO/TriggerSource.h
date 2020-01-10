//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \file TriggerSource.h
/// \brief Declaration of the class TriggerSource
//******************************************************************************
#pragma once

#pragma region Includes
#include "PlcDataTypes.h"
#pragma endregion Includes

namespace SvPlc
{
struct ChannelOut;

struct TriggerChannel
{
	bool m_newTrigger {false};
	bool m_active {false};
	uint32_t m_period {0UL};
	TriggerReport m_report;
};

class TriggerSource
{
public:
	virtual ~TriggerSource() = default;

	void setTriggerChannel(uint8_t channel, bool active, uint32_t period);
	bool checkForNewTriggers();

	virtual bool initialize() {return false;}
	virtual bool isReady() { return true; }
	virtual void queueResult(uint8_t channel, ChannelOut&& channelOut) {}
	virtual bool analyzeTelegramData() = 0;
	virtual void setReady(bool ready) {}


	TriggerReport getNewTriggerReport(uint8_t channel);
	const TriggerChannel& getChannel(uint8_t channel) const { return m_triggerChannels[channel]; }

protected:
	virtual void createTriggerReport(uint8_t channel) = 0;
	void addTriggerReport(TriggerReport&& triggerReport);

	std::mutex m_triggerSourceMutex;

private:

	TriggerChannel m_triggerChannels[c_NumberOfChannels];
};

} //namespace SvPlc

