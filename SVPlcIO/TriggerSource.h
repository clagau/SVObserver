//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \file TriggerSource.h
/// \brief Declaration of the class TriggerSource
//******************************************************************************
#pragma once

#pragma region Includes
#include "PlcDataTypes.h"
#include "TriggerInformation.h"
#pragma endregion Includes

struct ChannelOut;

struct TriggerChannel
{
	TriggerChannel() = default;
	bool m_newTrigger {false};
	bool m_active {false};
	uint32_t m_period {0UL};
	TriggerInformation m_currentTriggerInfo;
};

class TriggerSource
{
public:
	TriggerSource() = default;

	virtual ~TriggerSource() = default;

	void setTriggerChannel(uint8_t channel, bool active, uint32_t period);
	bool checkForNewTriggers();

	virtual bool initialize() {return false;}
	virtual bool isReady() { return true; }
	virtual void queueResult(uint8_t channel, ChannelOut& rChannelOut) {}
	virtual bool analyzeTelegramData() = 0;


	TriggerInformation getNewTriggerInfo(uint8_t channel);
	const TriggerChannel& getChannel(uint8_t channel) const { return m_triggerChannels[channel]; }

protected:
	virtual void createTriggerInfo(uint8_t channel) = 0;
	void addTriggerInfo(const TriggerInformation& rTriggerInfo);

	std::mutex m_triggerChannelMutex;

private:

	TriggerChannel m_triggerChannels[c_NumberOfChannels];
};

