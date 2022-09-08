//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file TriggerSource.h
/// \brief Declaration of the class TriggerSource
//******************************************************************************
#pragma once

#pragma region Includes
#include "PlcDataTypes.h"
#pragma endregion Includes

namespace SvEcat
{
struct ChannelOut1;

class TriggerSource
{
public:
	explicit TriggerSource(std::function<void(const TriggerReport&)> pReportTrigger);
	virtual ~TriggerSource() = default;

	void checkForNewTriggers();

	virtual HRESULT initialize() = 0;
	virtual bool setTriggerChannel(uint8_t channel, bool active);
	virtual bool isReady() const { return m_ready; }
	virtual void analyzeTelegramData() = 0;
	virtual uint32_t getInputs() const = 0;
	virtual void setOutput(uint8_t outputNr, bool state) = 0;
	virtual void setReady(bool ready) { m_ready = ready; }

protected:
	virtual void createTriggerReport(uint8_t channel) = 0;
	void sendTriggerReport(const TriggerReport& rTriggerReport);

	std::mutex m_triggerSourceMutex;

private:
	bool m_ready {false};
	std::function<void(const TriggerReport&)> m_pReportTrigger;
	std::array<std::atomic_bool, cNumberOfChannels> m_activeChannel {false, false, false, false};
};

} //namespace SvEcat

