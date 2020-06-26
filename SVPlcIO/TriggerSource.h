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

class TriggerSource
{
public:
	explicit TriggerSource(std::function<void(const TriggerReport&)> pReportTrigger);
	virtual ~TriggerSource() = default;

	///Returns if any trigger channels active
	bool checkForNewTriggers();

	virtual HRESULT initialize() = 0;
	virtual bool setTriggerChannel(uint8_t channel, bool active);
	virtual bool isReady() { return true; }
	virtual bool analyzeTelegramData() = 0;
	virtual void queueResult(uint8_t , ChannelOut&& ) {}
	virtual void setReady(bool ) {}

protected:
	virtual void createTriggerReport(uint8_t channel) = 0;
	void sendTriggerReport(const TriggerReport& rTriggerReport);

	std::mutex m_triggerSourceMutex;

private:
	std::function<void(const TriggerReport&)> m_pReportTrigger;
	std::array<std::atomic_bool, cNumberOfChannels> m_activeChannel {false, false, false, false};
};

} //namespace SvPlc

