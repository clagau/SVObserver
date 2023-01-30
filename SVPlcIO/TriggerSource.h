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

namespace SvTrig
{
struct ResultData;
struct TriggerData;
}

namespace SvPlc
{
class TriggerSource
{
public:
	explicit TriggerSource(std::function<void(const SvTrig::TriggerData&)> pReportTrigger);
	virtual ~TriggerSource() = default;

	void checkForNewTriggers();

	virtual HRESULT initialize() = 0;
	virtual bool setTriggerChannel(uint8_t channel, bool active);
	virtual bool isReady() { return true; }
	virtual void analyzeTelegramData() = 0;
	virtual void queueResult(const SvTrig::ResultData& rResultData) = 0;
	virtual void setReady(bool ) {}

protected:
	virtual void createTriggerData(uint8_t channel) = 0;
	void sendTriggerData(const SvTrig::TriggerData& rTriggerData);

	std::mutex m_triggerSourceMutex;

private:
	std::function<void(const SvTrig::TriggerData&)> m_pTriggerDataCallback;
	std::array<std::atomic_bool, cNumberOfChannels> m_activeChannel {false, false, false, false};
};

} //namespace SvPlc

