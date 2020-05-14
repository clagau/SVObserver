//*****************************************************************************
/// \copyright COPYRIGHT (c) 2019,2019 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \file SimulatedTriggerSource.h
/// This is the class used to simulate triggers without the Hilscher card
//******************************************************************************
#pragma once

#pragma region Includes
#include "TriggerSource.h"
#pragma endregion Includes

namespace SvPlc
{
/// a simplified simulation of HardwareTriggerSource
class SimulatedTriggerSource : public TriggerSource
{
public:
	SimulatedTriggerSource() = default;
	virtual ~SimulatedTriggerSource() = default;

	HRESULT initialize() override {return S_OK;};
	bool setTriggerChannel(uint8_t channel, bool active, uint32_t period) override;
	bool analyzeTelegramData() override;
	void createTriggerReport(uint8_t channel) override;

private:
	std::thread m_timerThread[cNumberOfChannels];
	std::atomic_bool m_runThread[cNumberOfChannels];
};

} //namespace SvPlc
